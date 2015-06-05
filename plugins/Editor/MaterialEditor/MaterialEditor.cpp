//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include "MaterialEditor.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>

#include <bx/fpumath.h>
#include <../nanovg/nanovg.h>

#include "MaterialEditorNodes.h"

using namespace Plugins;

MaterialEditor matEditor;

MaterialEditor::MaterialEditor()
{
   name = "Material Editor";
   matEditorArea = -1;
   openMaterialArea = -1;
   openMaterialList = -1;
   openMaterialOpen = false;

   for(S32 n = 0; n < 16; ++n)
      guiTextures[n] = -1;

   guiSelNodeName = -1;
   guiSelRValue = -1;
   guiSelGValue = -1;
   guiSelBValue = -1;
   guiSelAValue = -1;
   guiSelMulValue = -1;
   guiSelSlotValue = -1;

   mMaterialAsset = NULL;
   mMaterialTemplate = NULL;
   activeConnection = NULL;
   selectedNode = NULL;
   mouseDownPoint.set(0, 0);
}

MaterialEditor::~MaterialEditor()
{

}

void MaterialEditor::deleteKey()
{
   if ( selectedNode == NULL )
      return;

   // Erase any connections.
   Vector<Connection> newConnectionList;
   for(S32 n = 0; n < connectionList.size(); ++n )
   {
      Connection* connection = &connectionList[n];
      if ( dStrcmp(connection->inputNodeName, selectedNode->name) != 0 && dStrcmp(connection->outputNodeName, selectedNode->name) != 0 )
         newConnectionList.push_back(*connection);
   }
   connectionList.clear();
   connectionList.merge(newConnectionList);
   activeConnection = NULL;

   // Erase from Template
   if ( mMaterialTemplate != NULL && selectedNode->materialNode != NULL )
      mMaterialTemplate->removeObject(selectedNode->materialNode);

   // Erase the Node
   Vector<Node> newNodeList;
   for(S32 n = 0; n < nodeList.size(); ++n )
   {
      Node* node = &nodeList[n];
      if ( node != selectedNode )
         newNodeList.push_back(*node);
   }
   nodeList.clear();
   nodeList.merge(newNodeList);

   selectedNode = NULL;
}

void MaterialEditor::enable()
{
   if ( matEditorArea < 0 )
   {
      // Scene Editor
      matEditorArea = Link.SysGUI.beginScrollArea("Material Editor", 5, 160, 300, 600);
      Link.SysGUI.separator();
      Link.SysGUI.button("New Material", "", clickNewMaterial);
      Link.SysGUI.button("Open Material", "", clickOpenMaterial);
      Link.SysGUI.button("Save & Compile", "", clickSaveMaterial);
      Link.SysGUI.separator();
      Link.SysGUI.textInput("Material Name", "");

      Link.SysGUI.beginCollapse("Textures", "", false);
         for(S32 n = 0; n < 16; ++n )
         {
            char buf[256];
            dSprintf(buf, 256, "Texture Slot %d", n);

            Link.SysGUI.beginCollapse(buf, "", false);
            guiTextures[n] = Link.SysGUI.image(NULL, "", NULL);
            Link.SysGUI.button("Select Texture", "", NULL);
            Link.SysGUI.button("Clear Texture", "", NULL);
            Link.SysGUI.endCollapse();
         }
      Link.SysGUI.endCollapse();

      Link.SysGUI.separator();

      Link.SysGUI.beginCollapse("Add Node", "", false);

         Link.SysGUI.beginCollapse("Outputs", "", false);
         Link.SysGUI.button("Deferred", "", clickAddNode);
         Link.SysGUI.button("Forward", "", clickAddNode);
         Link.SysGUI.endCollapse();

         Link.SysGUI.beginCollapse("Inputs", "", false);
         Link.SysGUI.button("Float", "", clickAddNode);
         Link.SysGUI.button("Vec2", "", clickAddNode);
         Link.SysGUI.button("Vec3", "", clickAddNode);
         Link.SysGUI.button("Vec4", "", clickAddNode);
         Link.SysGUI.button("Texture", "", clickAddNode);
         Link.SysGUI.button("Time", "", clickAddNode);
         Link.SysGUI.endCollapse();

         Link.SysGUI.beginCollapse("Math", "", false);
         Link.SysGUI.button("Add", "", clickAddNode);
         Link.SysGUI.button("Subtract", "", clickAddNode);
         Link.SysGUI.button("Multiply", "", clickAddNode);
         Link.SysGUI.button("Divide", "", clickAddNode);
         Link.SysGUI.button("Sin", "", clickAddNode);
         Link.SysGUI.button("Cos", "", clickAddNode);
         Link.SysGUI.endCollapse();

      Link.SysGUI.endCollapse();

      Link.SysGUI.beginCollapse("Edit Node", "", false);
         guiSelNodeName = Link.SysGUI.textInput("Internal Name", "");
         Link.SysGUI.separator();

         // For Textures
         guiSelSlotValue = Link.SysGUI.slider("Slot", 0, 0, 15);

         // Float, Vec2, Vec3, Vec4
         guiSelRValue = Link.SysGUI.slider("R", 0, 0, 255);
         guiSelGValue = Link.SysGUI.slider("G", 0, 0, 255);
         guiSelBValue = Link.SysGUI.slider("B", 0, 0, 255);
         guiSelAValue = Link.SysGUI.slider("A", 0, 0, 255);

         // Time
         guiSelMulValue = Link.SysGUI.textInput("Multiplier", "1.0");
      Link.SysGUI.endCollapse();

      Link.SysGUI.separator();
      Link.SysGUI.endScrollArea();
   }
   Link.SysGUI.setElementHidden(matEditorArea, false);

   openMaterialArea = Link.SysGUI.beginScrollArea("Open Material", 310, 5, 200, 400);
   Link.SysGUI.separator();
   openMaterialList = Link.SysGUI.list("", clickOpenMaterialList);
   Link.SysGUI.separator();
   Link.SysGUI.endScrollArea();
   Link.SysGUI.setElementHidden(openMaterialArea, true);

   setProcessTicks(true);
}

void MaterialEditor::disable()
{
   Link.SysGUI.setElementHidden(matEditorArea, true);
   setProcessTicks(false);
}

void MaterialEditor::render()
{
   for(S32 n = 0; n < nodeList.size(); ++n )
   {
      Node* node = &nodeList[n];
      renderNode(node);
   }

   for(S32 n = 0; n < connectionList.size(); ++n )
   {
      Connection* connection = &connectionList[n];
      renderConnection(connection);
   }
}

bgfx::TextureHandle* MaterialEditor::getTextureHandle(S32 slot)
{
   if ( slot < 0 || mMaterialAsset == NULL ) 
      return NULL;
   
   Vector<bgfx::TextureHandle>* textures = mMaterialAsset->getTextureHandles();
   if ( slot >= textures->size() )
      return NULL;

   return &textures->at(slot);
}

void MaterialEditor::getUniqueNodeName(char* dest, const char* name)
{
   if ( !findNode(name) )
   {
      dStrcpy(dest, name);
      return;
   }

   U32 extension = 0;
   while(true)
   {
      char newName[256];
      dSprintf(newName, 256, "%s%d", name, extension);

      if ( !findNode(newName) )
      {
         dStrcpy(dest, newName);
         return;
      }

      extension++;
   }
}

Node* MaterialEditor::findNode(const char* name)
{
   for(S32 n = 0; n < nodeList.size(); ++n )
   {
      Node* node = &nodeList[n];
      if ( dStrcmp(node->name, name) == 0 )
      {
         return node;
      }
   }
   return NULL;
}

Connection* MaterialEditor::findConnectionFromOutput(const char* name, U32 index)
{

   for(S32 n = 0; n < connectionList.size(); ++n )
   {
      Connection* connection = &connectionList[n];
      if ( dStrcmp(connection->outputNodeName, name) == 0 )
      {
         if ( index == -1 || connection->outputIndex == index )
            return connection;
      }
   }
   return NULL;
}

Connection* MaterialEditor::findConnectionFromInput(const char* name, U32 index)
{
   for(S32 n = 0; n < connectionList.size(); ++n )
   {
      Connection* connection = &connectionList[n];
      if ( dStrcmp(connection->inputNodeName, name) == 0 )
      {
         if ( index == -1 || connection->inputIndex == index )
            return connection;
      }
   }
   return NULL;
}

void MaterialEditor::renderNode(Node* node)
{
   NVGcontext* vg = Link.Graphics.dglGetNVGContext();
	if (vg)
	{
      Link.NanoVG.nvgSave(vg);

	   // Window
	   Link.NanoVG.nvgBeginPath(vg);
	   Link.NanoVG.nvgRoundedRect(vg, node->x, node->y, node->width, node->height, 10);
	   Link.NanoVG.nvgFillColor(vg, Link.NanoVG.nvgRGBA(28,30,34,192));
	   Link.NanoVG.nvgFill(vg);

      // Header
	   Link.NanoVG.nvgBeginPath(vg);
	   Link.NanoVG.nvgRoundedRect(vg, node->x + 1, node->y + 1, node->width - 2, 30, 9);

      if ( node->mouseOver || node == selectedNode )
      {
	      Link.NanoVG.nvgFillPaint(vg, 
            Link.NanoVG.nvgLinearGradient(vg, node->x, node->y, node->x, node->y + 15, Link.NanoVG.nvgRGBA(128,130,134,255), Link.NanoVG.nvgRGBA(128,130,134,192)));
      } else {
	      Link.NanoVG.nvgFillPaint(vg, 
            Link.NanoVG.nvgLinearGradient(vg, node->x, node->y, node->x, node->y + 15, Link.NanoVG.nvgRGBA(28,30,34,255), Link.NanoVG.nvgRGBA(28,30,34,192)));
      }

	   Link.NanoVG.nvgFill(vg);
      Link.NanoVG.nvgFontSize(vg, 14.0f);
	   Link.NanoVG.nvgTextAlign(vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
	   Link.NanoVG.nvgFillColor(vg, Link.NanoVG.nvgRGBA(255,255,255,200));
	   Link.NanoVG.nvgText(vg, node->x + (node->width / 2), node->y + 18, node->name, NULL);

      if ( node->showColor )
      {
         Link.NanoVG.nvgBeginPath(vg);
         Link.NanoVG.nvgTextAlign(vg, NVG_ALIGN_LEFT);
	      Link.NanoVG.nvgRoundedRect(vg, node->x + 15, node->y + 50, 60, 60, 5);
	      Link.NanoVG.nvgFillColor(vg, Link.NanoVG.nvgRGBA(node->color.red, node->color.green, node->color.blue, node->color.alpha));
	      Link.NanoVG.nvgFill(vg);
      }

      if ( node->showTexture )
      {
         if ( node->texture.idx != bgfx::invalidHandle )
         {
            S32 width, height;
            Link.NanoVG.nvgImageSize(vg, node->texture.idx, &width, &height);
		      Link.NanoVG.nvgBeginPath(vg);
		      Link.NanoVG.nvgRoundedRect(vg, node->x + 15, node->y + 45, 90, 90, 5);
		      Link.NanoVG.nvgFillPaint(vg, Link.NanoVG.nvgImagePattern(vg, node->x + 15, node->y + 45, 90, 90, 0.0f, node->texture.idx, 1.0f));
		      Link.NanoVG.nvgFill(vg);
         }
      }

      // Input Points
      for(S32 n = 0; n < node->inputs.size(); ++n )
      {
         InputPoint* input = &node->inputs[n];
         input->lastPosition.set(node->x + 12.0f, node->y + 49.0f + (n * 20.0f));

         U8 alpha_val = input->mouseOver ? 255 : 175;

         Link.NanoVG.nvgBeginPath(vg);
         Link.NanoVG.nvgFillColor(vg, Link.NanoVG.nvgRGBA(255, 255, 255, alpha_val));
	      Link.NanoVG.nvgCircle(vg, node->x + 12.0f, node->y + 49.0f + (n * 20.0f), 5.0f);
         Link.NanoVG.nvgFill(vg);

         Link.NanoVG.nvgFontSize(vg, 12.0f);
         Link.NanoVG.nvgTextAlign(vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
	      Link.NanoVG.nvgFillColor(vg, Link.NanoVG.nvgRGBA(255, 255, 255, alpha_val));
	      Link.NanoVG.nvgText(vg, node->x + 22, node->y + 50 + (n * 20), input->name, NULL);
      }

      // Output Points
      for(S32 n = 0; n < node->outputs.size(); ++n )
      {
         OutputPoint* output = &node->outputs[n];
         output->lastPosition.set(node->x + node->width - 12.0f, node->y + 49.0f + (n * 20.0f));

         U8 alpha_val = output->mouseOver ? 255 : 175;

         Link.NanoVG.nvgBeginPath(vg);
         Link.NanoVG.nvgFillColor(vg, Link.NanoVG.nvgRGBA(output->color.red, output->color.green, output->color.blue, alpha_val));
	      Link.NanoVG.nvgCircle(vg, node->x + node->width - 12.0f, node->y + 49.0f + (n * 20.0f), 5.0f);
         Link.NanoVG.nvgFill(vg);
      }

      Link.NanoVG.nvgRestore(vg);
   }
}

void MaterialEditor::renderConnection(Connection* connection)
{
   NVGcontext* vg = Link.Graphics.dglGetNVGContext();
	if (vg)
	{
      Point2F mStart;
      Point2F mEnd;

      if ( connection->outputToMouse )
         mEnd.set(lastMousePoint.x, lastMousePoint.y);
      else {
         Node* endNode = findNode(connection->inputNodeName);
         if ( !endNode || connection->inputIndex >= endNode->inputs.size() ) 
            return;
         mEnd.set(endNode->inputs[connection->inputIndex].lastPosition.x, endNode->inputs[connection->inputIndex].lastPosition.y);
      }

      if ( connection->inputToMouse )
         mStart.set(lastMousePoint.x, lastMousePoint.y);
      else {
         Node* startNode = findNode(connection->outputNodeName);
         if ( !startNode || connection->outputIndex >= startNode->outputs.size() ) 
            return;
         mStart.set(startNode->outputs[connection->outputIndex].lastPosition.x, startNode->outputs[connection->outputIndex].lastPosition.y);
      }

      Point2F mControlPointA(mStart.x + 50, mStart.y);
      Point2F mControlPointB(mEnd.x - 50, mEnd.y);

      F32 diff = (mEnd.y - mStart.y) * 0.25f;
      mControlPointA.y -= diff;
      mControlPointB.y += diff;

		Link.NanoVG.nvgBeginPath(vg);
		Link.NanoVG.nvgMoveTo(vg, mStart.x, mStart.y);
		Link.NanoVG.nvgBezierTo(vg, mControlPointA.x, mControlPointA.y, 
                                  mControlPointB.x, mControlPointB.y, 
                                  mEnd.x, mEnd.y);
		Link.NanoVG.nvgStrokeColor(vg, Link.NanoVG.nvgRGBA(255, 255, 255, 200));
		Link.NanoVG.nvgStrokeWidth(vg, 5.0f);
		Link.NanoVG.nvgStroke(vg);
   }
}

void MaterialEditor::processTick()
{
   if ( selectedNode != NULL )
   {
      U8 r = Link.SysGUI.getIntValue(guiSelRValue);
      U8 g = Link.SysGUI.getIntValue(guiSelGValue);
      U8 b = Link.SysGUI.getIntValue(guiSelBValue);
      U8 a = Link.SysGUI.getIntValue(guiSelAValue);
      selectedNode->color.set(r, g, b, a);
   }
}

void MaterialEditor::advanceTime(F32 delta)
{
   //
}

void MaterialEditor::interpolateTick(F32 delta)
{
   //
}

void MaterialEditor::onMouseMoveEvent(const GuiEvent &event)
{
   lastMousePoint = event.mousePoint;

   for(S32 n = 0; n < nodeList.size(); ++n )
   {
      Node* node = &nodeList[n];
      node->mouseOver = false;

      if ( event.mousePoint.x >= node->x && event.mousePoint.x <= node->x + node->width
         && event.mousePoint.y >= node->y && event.mousePoint.y <= node->y + node->height )
      {
         node->mouseOver = true;

         for ( S32 i = 0; i < node->inputs.size(); ++i )
         {
            InputPoint* input = &node->inputs[i];
            input->mouseOver = false;

            Point2F dist = input->lastPosition - Point2F(lastMousePoint.x, lastMousePoint.y);
            if (  dist.len() <= 5 )
               input->mouseOver = true;
         }

         for ( S32 i = 0; i < node->outputs.size(); ++i )
         {
            OutputPoint* output = &node->outputs[i];
            output->mouseOver = false;

            Point2F dist = output->lastPosition - Point2F(lastMousePoint.x, lastMousePoint.y);
            if (  dist.len() <= 5 )
               output->mouseOver = true;
         }
      }
   }
}

void MaterialEditor::onMouseDownEvent(const GuiEvent &event)
{
   lastMousePoint = event.mousePoint;
   selectedNode = NULL;
   mouseDownPoint = event.mousePoint;

   for(S32 n = 0; n < nodeList.size(); ++n )
   {
      Node* node = &nodeList[n];

      if ( node->mouseOver )
      {
         for ( S32 i = 0; i < node->inputs.size(); ++i )
         {
            InputPoint* input = &node->inputs[i];
            if ( input->mouseOver ) 
            {
               if ( activeConnection != NULL )
               {
                  if ( activeConnection->outputToMouse )
                  {
                     activeConnection->inputNodeName = node->name;
                     activeConnection->inputIndex = i;
                     activeConnection->outputToMouse = false;
                     activeConnection = NULL;
                     return;
                  }
               } else {
                  Connection* conn = findConnectionFromInput(node->name, i);
                  if ( conn )
                  {
                     conn->outputToMouse = true;
                     activeConnection = conn;
                     return;
                  } else {
                     Connection newConn;
                     newConn.inputNodeName = node->name;
                     newConn.inputIndex = i;
                     newConn.inputToMouse = true;
                     connectionList.push_back(newConn);
                     activeConnection = &connectionList.back();
                     return;
                  }
               }
            }
         }

         for ( S32 i = 0; i < node->outputs.size(); ++i )
         {
            OutputPoint* output = &node->outputs[i];
            if ( output->mouseOver ) 
            {
               if ( activeConnection != NULL )
               {
                  if ( activeConnection->inputToMouse )
                  {
                     activeConnection->outputNodeName = node->name;
                     activeConnection->outputIndex = i;
                     activeConnection->inputToMouse = false;
                     activeConnection = NULL;
                     return;
                  }
               } else {
                  Connection* conn = findConnectionFromOutput(node->name, i);
                  if ( conn )
                  {
                     conn->inputToMouse = true;
                     activeConnection = conn;
                     return;
                  } else {
                     Connection newConn;
                     newConn.outputNodeName = node->name;
                     newConn.outputIndex = i;
                     newConn.outputToMouse = true;
                     connectionList.push_back(newConn);
                     activeConnection = &connectionList.back();
                     return;
                  }
               }
            }
         }

         selectNode(node);
         return;
      }
   }

   // Clicked on nothing, delete connection.
   if ( activeConnection != NULL )
   {
      Vector<Connection> newConnectionList;

      for(S32 n = 0; n < connectionList.size(); ++n )
      {
         Connection* connection = &connectionList[n];
         if ( connection != activeConnection )
            newConnectionList.push_back(*connection);
      }

      connectionList.clear();
      connectionList.merge(newConnectionList);
      activeConnection = NULL;
   }
}

void MaterialEditor::selectNode(Node* node)
{
   Link.SysGUI.setElementHidden(guiSelRValue, true);
   Link.SysGUI.setElementHidden(guiSelGValue, true);
   Link.SysGUI.setElementHidden(guiSelBValue, true);
   Link.SysGUI.setElementHidden(guiSelAValue, true);
   Link.SysGUI.setElementHidden(guiSelMulValue, true);
   Link.SysGUI.setElementHidden(guiSelSlotValue, true);

   selectedNode = node;
   Link.SysGUI.setTextValue(guiSelNodeName, node->name);

   if ( dStrcmp(node->type, "Float") == 0 )
   {
      Link.SysGUI.setElementHidden(guiSelRValue, false);
   }

   if ( dStrcmp(node->type, "Vec2") == 0 )
   {
      Link.SysGUI.setElementHidden(guiSelRValue, false);
      Link.SysGUI.setElementHidden(guiSelGValue, false);
   }

   if ( dStrcmp(node->type, "Vec3") == 0 )
   {
      Link.SysGUI.setElementHidden(guiSelRValue, false);
      Link.SysGUI.setElementHidden(guiSelGValue, false);
      Link.SysGUI.setElementHidden(guiSelBValue, false);
   }

   if ( dStrcmp(node->type, "Vec4") == 0 )
   {
      Link.SysGUI.setElementHidden(guiSelRValue, false);
      Link.SysGUI.setElementHidden(guiSelGValue, false);
      Link.SysGUI.setElementHidden(guiSelBValue, false);
      Link.SysGUI.setElementHidden(guiSelAValue, false);
   }

   if ( dStrcmp(node->type, "Texture") == 0 )
   {
      Link.SysGUI.setElementHidden(guiSelSlotValue, false);
   }

   if ( dStrcmp(node->type, "Time") == 0 )
   {
      Link.SysGUI.setElementHidden(guiSelMulValue, false);
   }

   Link.SysGUI.setIntValue(guiSelRValue, node->color.red);
   Link.SysGUI.setIntValue(guiSelGValue, node->color.green);
   Link.SysGUI.setIntValue(guiSelBValue, node->color.blue);
   Link.SysGUI.setIntValue(guiSelAValue, node->color.alpha);
}

void MaterialEditor::onMouseDraggedEvent(const GuiEvent &event)
{
   lastMousePoint = event.mousePoint;

   if ( selectedNode != NULL )
   {
      Point2I dif = event.mousePoint - mouseDownPoint;
      mouseDownPoint = event.mousePoint;
      selectedNode->x += dif.x;
      selectedNode->y += dif.y;

      if ( selectedNode->materialNode != NULL )
         selectedNode->materialNode->mPosition.set(selectedNode->x,  selectedNode->y);
   }
}

void MaterialEditor::toggleOpenMaterial()
{
   openMaterialOpen = !openMaterialOpen;
   Link.SysGUI.setElementHidden(openMaterialArea, !openMaterialOpen);
}

void MaterialEditor::clearMaterial()
{
   activeConnection = NULL;
   selectedNode = NULL;
   nodeList.clear();
   connectionList.clear();
}

void MaterialEditor::loadMaterial(MaterialAsset* mat)
{
   clearMaterial();

   mMaterialAsset = mat;
   mMaterialTemplate = mat->getTemplate();
   
   for(S32 n = 0; n < mMaterialTemplate->size(); ++n)
   {
      Scene::BaseNode* node = dynamic_cast<Scene::BaseNode*>(mMaterialTemplate->at(n));
      addNode(mMaterialTemplate, node->type, node);
   }

   for(S32 n = 0; n < nodeList.size(); ++n)
   {
      Node* node = &nodeList[n];
      addNodeConnection(node);
   }

   Vector<bgfx::TextureHandle>* textures = mMaterialAsset->getTextureHandles();
   for (S32 n = 0; n < textures->size(); ++n)
      Link.SysGUI.setImageValue(guiTextures[n], textures->at(n));
}

void MaterialEditor::saveMaterial()
{
   if ( mMaterialAsset == NULL || mMaterialTemplate == NULL )
      return;

   for(S32 n = 0; n < nodeList.size(); ++n )
   {
      Node* node = &nodeList[n];
      saveNode(mMaterialTemplate, node);
   }

   for(S32 n = 0; n < connectionList.size(); ++n )
   {
      Connection* connection = &connectionList[n];
      saveConnection(connection);
   }

   mMaterialAsset->saveMaterial();
   mMaterialAsset->reloadMaterial();
}

void clickNewMaterial(S32 id)
{
   matEditor.clearMaterial();
}

void clickOpenMaterial(S32 id)
{
   Link.SysGUI.clearList(matEditor.openMaterialList);

   bool inCategory = false;
   char last_mod_name[256];
   AssetQuery assQuery;
   Link.AssetDatabaseLink.findAssetType(&assQuery, "MaterialAsset", false);
   for ( U32 n = 0; n < assQuery.size(); n++)
   {
      StringTableEntry assetID = assQuery[n];
      char buf[256];
      dStrcpy(buf, assetID);

      char* mod_name = dStrtok(buf, ":");
      if ( dStrcmp(last_mod_name, mod_name) != 0 )
      {
         dStrcpy(last_mod_name, mod_name);
         inCategory = true;
      }

      char* asset_name = dStrtok(NULL, ":");
      Link.SysGUI.addListValue(matEditor.openMaterialList, assetID, "", NULL);
   }

   matEditor.toggleOpenMaterial();
}


void clickOpenMaterialList(S32 id)
{
   S32 selectedItem = Link.SysGUI.getListSelected(matEditor.openMaterialList);
   const char* selected_value = Link.SysGUI.getListValue(matEditor.openMaterialList, selectedItem);

   MaterialAsset* mat = Link.Scene.getMaterialAsset(selected_value);
   if ( mat )
      matEditor.loadMaterial(mat);

   matEditor.toggleOpenMaterial();
}

void clickSaveMaterial(S32 id)
{
   matEditor.saveMaterial();
}

void clickAddNode(S32 id)
{
   const char* value = Link.SysGUI.getLabelValue(id);
   addNode(matEditor.mMaterialTemplate, value);
}