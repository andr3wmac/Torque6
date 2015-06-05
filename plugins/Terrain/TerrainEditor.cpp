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

#include "TerrainEditor.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>
#include <bx/fpumath.h>

#include "TerrainCell.h"

// Link to Editor Plugin
#include "../Editor/Editor_pluginAPI.h"

using namespace Plugins;

struct PosUVColorVertex
{
   F32 m_x;
	F32 m_y;
	F32 m_z;
	F32 m_u;
	F32 m_v;
	U32 m_abgr;
};

TerrainEditor terrainEditor;
PosUVColorVertex decalVerts[4] = 
{
	{-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0xffffffff },
	{-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0xffffffff },
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff },
	{ 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0xffffffff }
};

U16 decalIndices[6] = {0, 2, 1, 1, 2, 3};

TerrainEditor::TerrainEditor()
{
   name = "Terrain Editor";
   terrainEditorArea = -1;
   guiToolList = -1;
   guiBrushSize = -1;
   guiBrushPower = -1;
   guiBrushSoftness = -1;

   mActiveTool = 0;

   mBrushSize = 20;
   mBrushPower = 10.0f;
   mBrushSoftness = 1.0f;

   paintTerrain = false;
   mousePosition.set(0, 0);
   lastMousePosition.set(0, 0);

   // Load Shared index/vertex buffer.
   const bgfx::Memory* mem;

   mem = Link.bgfx.makeRef(&decalVerts[0], sizeof(PosUVColorVertex) * 4, NULL, NULL);
   vertexBuffer = Link.bgfx.createVertexBuffer(mem, *Link.Graphics.PosUVColorVertex, BGFX_BUFFER_NONE);

	mem = Link.bgfx.makeRef(&decalIndices[0], sizeof(uint16_t) * 6, NULL, NULL);
	indexBuffer = Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_NONE);

   decalShader.idx = bgfx::invalidHandle;
   Graphics::ShaderAsset* decalShaderAsset = Link.Graphics.getShaderAsset("Terrain:decalShader");
   if ( decalShaderAsset )
      decalShader = decalShaderAsset->getProgram();

   decalRenderData = Link.Rendering.createRenderData();
   decalRenderData->shader = decalShader;
   decalRenderData->indexBuffer = *Link.Graphics.cubeIB;
   decalRenderData->vertexBuffer = *Link.Graphics.cubeVB;
   decalRenderData->view = Link.Graphics.getView("TerrainMegaTexture", "DeferredGeometry", true);
   decalRenderData->transformTable = decalTransform;
   decalRenderData->transformCount = 1;

   decalRenderData->textures = &decalTextures;
   Rendering::TextureData* depthTexture = decalRenderData->addTexture();
   depthTexture->isDepthTexture = true;
   depthTexture->uniform = Link.Graphics.getTextureUniform(0);

   decalRenderData->state = 0
			   | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
   bx::mtxSRT(&decalTransform[0], 1, 1, 1, 0, 0, 0, 50, 0, 50);
}

TerrainEditor::~TerrainEditor()
{
   if ( vertexBuffer.idx != bgfx::invalidHandle )
      Link.bgfx.destroyVertexBuffer(vertexBuffer);

   if ( indexBuffer.idx != bgfx::invalidHandle )
      Link.bgfx.destroyIndexBuffer(indexBuffer);
}

void TerrainEditor::enable()
{
   if ( terrainEditorArea < 0 )
   {
      // Scene Editor
      terrainEditorArea = Link.SysGUI.beginScrollArea("Terrain Editor", 5, 160, 300, 600);

      Link.SysGUI.separator();
      Link.SysGUI.button("Load Terrain", "", NULL);
      Link.SysGUI.button("Save Terrain", "", NULL);
      Link.SysGUI.separator();
      Link.SysGUI.label("Tool");
      Link.SysGUI.separator();

      guiToolList = Link.SysGUI.list("", clickToolList);
      Link.SysGUI.addListValue(guiToolList, "Raise", "", NULL);
      Link.SysGUI.addListValue(guiToolList, "Lower", "", NULL);
      Link.SysGUI.addListValue(guiToolList, "Paint Layer 0", "", NULL);
      Link.SysGUI.addListValue(guiToolList, "Paint Layer 1", "", NULL);
      Link.SysGUI.addListValue(guiToolList, "Paint Layer 2", "", NULL);
      Link.SysGUI.addListValue(guiToolList, "Paint Layer 3", "", NULL);

      Link.SysGUI.separator();
      Link.SysGUI.label("Brush Settings");
      Link.SysGUI.separator();
      guiBrushSize = Link.SysGUI.slider("Size", mBrushSize, 0, 100);
      guiBrushPower = Link.SysGUI.slider("Power", (S32)mBrushPower, 0, 100);
      guiBrushSoftness = Link.SysGUI.slider("Softness", (S32)(mBrushSoftness * 50), 0, 100);

      Link.SysGUI.endScrollArea();
   }

   Link.SysGUI.setElementHidden(terrainEditorArea, false);
   setProcessTicks(true);
}

void TerrainEditor::disable()
{
   Link.SysGUI.setElementHidden(terrainEditorArea, true);
   setProcessTicks(false);
}

void TerrainEditor::render()
{

}

void TerrainEditor::deleteKey()
{

}

void TerrainEditor::onMouseMoveEvent(const GuiEvent &event)
{
   mousePosition = event.mousePoint;
}

void TerrainEditor::onMouseDownEvent(const GuiEvent &event)
{
   mousePosition = event.mousePoint;
   paintTerrain = true;
}

void TerrainEditor::onMouseDraggedEvent(const GuiEvent &event)
{
   mousePosition = event.mousePoint;
   paintTerrain = true;
}

void TerrainEditor::updateTerrainPosition()
{
   lastMousePosition = mousePosition;

   Point3F world_ray = Link.Rendering.screenToWorld(mousePosition);
   Point3F startPos = Link.Scene.getActiveCamera()->getPosition();
   Point3F endPos = startPos + (world_ray * 1000.0f);

   Point3F direction = endPos - startPos;
   F32 length = direction.len();
   direction.normalize();

   Point3F rayPos = startPos;
   for(U32 i = 0; i < length; i++)
   {
      for(S32 n = 0; n < terrainGrid.size(); ++n)
      {
         TerrainCell* cell = &terrainGrid[n];

         Point2I cellPos((S32)rayPos.x - (cell->gridX * cell->width), 
                         (S32)rayPos.z - (cell->gridY * cell->height));

         if ( cellPos.x >= (S32)cell->width || cellPos.x < 0 || cellPos.y >= (S32)cell->height || cellPos.y < 0 )
            continue;

         S32 mapPos = (cellPos.y * cell->width) + cellPos.x;
         F32 heightValue = cell->heightMap[mapPos];

         if ( rayPos.y <= heightValue )
         {
            mTerrainCell = cell;
            mTerrainPoint.set(cellPos.x, cellPos.y);
            bx::mtxSRT(&decalTransform[0], 
                       (F32)mBrushSize, (mTerrainCell->maxTerrainHeight / 2.0f) + 1.0f, (F32)mBrushSize, 
                       0.0f, 0.0f, 0.0f, 
                       (F32)(cell->gridX * cell->width) + cellPos.x, (F32)(mTerrainCell->maxTerrainHeight / 2) + 1, (F32)(cell->gridY * cell->height) + cellPos.y);
            return;
         }
      }

      rayPos += direction;
   }
}

void TerrainEditor::processTick()
{
   mBrushSize = Link.SysGUI.getIntValue(guiBrushSize);
   mBrushPower = (F32)Link.SysGUI.getIntValue(guiBrushPower);
   mBrushSoftness = ((F32)Link.SysGUI.getIntValue(guiBrushSoftness) / 100.0f) * 2.0f;

   if ( lastMousePosition != mousePosition )
      updateTerrainPosition();

   if ( paintTerrain )
   {
      paintTerrain = false;
      clickTerrainCell(mTerrainCell, mTerrainPoint.x, mTerrainPoint.y);
   }
}

void TerrainEditor::advanceTime(F32 delta)
{

}

void TerrainEditor::interpolateTick(F32 delta)
{

}

void TerrainEditor::clickTerrainCell(TerrainCell* cell, U32 x, U32 y)
{
   // This should be replaced by a whole tool system.
   for ( S32 area_y = -mBrushSize; area_y < mBrushSize; ++area_y )
   {
      for ( S32 area_x = -mBrushSize; area_x < mBrushSize; ++area_x )
      {
         S32 brush_x = x + area_x;
         if ( brush_x < 0 || (U32)brush_x > cell->width ) continue;
         S32 brush_y = y + area_y;
         if ( brush_y < 0 || (U32)brush_y > cell->height ) continue;

         Point2F area_point((F32)area_x, (F32)area_y);
         F32 dist = area_point.len();
         F32 impact = mBrushPower - (dist * mBrushSoftness);
         if ( impact <= 0.0f ) continue;

         U32 mapPos = (brush_y  * cell->width) + brush_x;

         if ( mActiveTool == 0 || mActiveTool == 1 )
         {
            // Raise
            if ( mActiveTool == 0 )
               cell->heightMap[mapPos] += impact;

            // Lower
            if ( mActiveTool == 1 )
               cell->heightMap[mapPos] -= impact;
   
            if ( cell->heightMap[mapPos] > cell->maxTerrainHeight )
               cell->maxTerrainHeight = cell->heightMap[mapPos];
         }

         if ( mActiveTool == 2 )
         {
            if ( cell->blendMap[mapPos].red < 225 )
               cell->blendMap[mapPos].red += 30;
            if ( cell->blendMap[mapPos].green > 10 )
               cell->blendMap[mapPos].green -= 10;
            if ( cell->blendMap[mapPos].blue > 10 )
               cell->blendMap[mapPos].blue -= 10;
            if ( cell->blendMap[mapPos].alpha > 10 )
               cell->blendMap[mapPos].alpha -= 10;
         }

         if ( mActiveTool == 3 )
         {
            if ( cell->blendMap[mapPos].red > 10 )
               cell->blendMap[mapPos].red -= 10;
            if ( cell->blendMap[mapPos].green < 225 )
               cell->blendMap[mapPos].green += 30;
            if ( cell->blendMap[mapPos].blue > 10 )
               cell->blendMap[mapPos].blue -= 10;
            if ( cell->blendMap[mapPos].alpha > 10 )
               cell->blendMap[mapPos].alpha -= 10;
         }

         if ( mActiveTool == 4 )
         {
            cell->blendMap[mapPos].set(255, 0, 0, 0);
         }

         if ( mActiveTool == 5 )
         {
            cell->blendMap[mapPos].set(0, 0, 0, 255);
         }
      }
   }
   
   cell->rebuild();
}

void TerrainEditor::switchTool(U32 num)
{
   mActiveTool = num;
}

void loadEditorAPI(PluginAPI* api)
{
   EditorAPI* editorAPI = static_cast<EditorAPI*>(api);
   editorAPI->addEditor(&terrainEditor);
}

void clickToolList(S32 id)
{
   S32 selectedItem = Link.SysGUI.getListSelected(id);
   const char* val = Link.SysGUI.getListValue(id, selectedItem);
   Con::printf("Selected Value: %s Selected Item: %d", val, selectedItem);
   terrainEditor.switchTool(selectedItem);
}