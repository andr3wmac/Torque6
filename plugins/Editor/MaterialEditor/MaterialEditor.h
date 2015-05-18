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

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

#ifndef _EDITOR_PLUGIN_API_
#include "../Editor_pluginAPI.h"
#endif

struct InputPoint
{
   const char* name;

   bool mouseOver;
   Point2F lastPosition;
};

struct OutputPoint
{
   ColorI color;

   bool mouseOver;
   Point2F lastPosition;
};

class Node
{
   public:
      const char* type;
      char name[256];
      F32 x;
      F32 y;
      F32 width;
      F32 height;
      bool mouseOver;

      bool showColor;
      ColorI color;

      bool showTexture;
      bgfx::TextureHandle texture;

      Scene::BaseNode* materialNode;

      Vector<InputPoint> inputs;
      Vector<OutputPoint> outputs;

      Node()
      {
         texture.idx = bgfx::invalidHandle;
         showTexture = false;
         showColor = false;
         name[0] = '\0';
         type = "";
         materialNode = NULL;
         x = 500.0f;
         y = 300.0f;
         width = 100;
         height = 150;
         mouseOver = false;
      }

      void addInput(const char* name)
      {
         InputPoint inp1;
         inp1.name = name;
         inputs.push_back(inp1);
      }

      void addOutput(ColorI color)
      {
         OutputPoint out1;
         out1.color = color;
         outputs.push_back(out1);
      }
};

struct Connection
{
   const char* outputNodeName;
   U32 outputIndex;
   const char* inputNodeName;
   U32 inputIndex;

   bool outputToMouse;
   bool inputToMouse;

   Connection()
   {
      outputToMouse = false;
      inputToMouse = false;
   }
};

class MaterialEditor : public EditorBase, public virtual Tickable
{
   public:
      MaterialAsset* mMaterialAsset;
      Scene::MaterialTemplate* mMaterialTemplate;

      MaterialEditor();
      ~MaterialEditor();

      void deleteKey();
      void enable();
      void disable();
      void render();

      S32 matEditorArea;
      bool openMaterialOpen;
      S32 openMaterialArea;
      S32 openMaterialList;
      void toggleOpenMaterial();

      S32 guiTextures[16];
      S32 guiSelNodeName;
      S32 guiSelRValue;
      S32 guiSelGValue;
      S32 guiSelBValue;
      S32 guiSelAValue;
      S32 guiSelMulValue;
      S32 guiSelSlotValue;
      void selectNode(Node* node);

      void clearMaterial();
      void loadMaterial(MaterialAsset* mat);
      void saveMaterial();

      Vector<Node> nodeList;
      Vector<Connection> connectionList;

      Connection* activeConnection;
      Node* selectedNode;
      Point2I mouseDownPoint;
      Point2I lastMousePoint;

      bgfx::TextureHandle* getTextureHandle(S32 slot);
      void getUniqueNodeName(char* dest, const char* name);

      Node* findNode(const char* name);
      Connection* findConnectionFromOutput(const char* name, U32 index = -1);
      Connection* findConnectionFromInput(const char* name, U32 index = -1);
      void renderNode(Node* node);
      void renderConnection(Connection* connection);

      virtual void processTick();
      virtual void advanceTime(F32 delta);
      virtual void interpolateTick(F32 delta);

      // Mouse/Gizmo Related
      virtual void onMouseMoveEvent(const GuiEvent &event);
      virtual void onMouseDownEvent(const GuiEvent &event);
      virtual void onMouseDraggedEvent(const GuiEvent &event);
};

extern MaterialEditor matEditor;

void clickNewMaterial(S32 id);
void clickOpenMaterial(S32 id);
void clickOpenMaterialList(S32 id);
void clickSaveMaterial(S32 id);

void clickAddNode(S32 id);