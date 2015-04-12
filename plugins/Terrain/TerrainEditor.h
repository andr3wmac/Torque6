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

#include "../Editor/Editor_pluginAPI.h"
#include "TerrainCell.h"

#include <bgfx.h>

class TerrainEditor : public EditorBase, public Tickable
{
   protected:
      U32 mActiveTool;

      S32 mBrushSize;
      F32 mBrushPower;
      F32 mBrushSoftness;

      S32 terrainEditorArea;
      S32 guiToolList;
      S32 guiBrushSize;
      S32 guiBrushPower;
      S32 guiBrushSoftness;
      bool paintTerrain;
      Point2I mousePosition;
      Point2I lastMousePosition;

      TerrainCell* mTerrainCell;
      Point2I mTerrainPoint;

      bgfx::VertexBufferHandle vertexBuffer;
      bgfx::IndexBufferHandle indexBuffer;
      bgfx::ProgramHandle decalShader;
      Rendering::RenderData* decalRenderData;
      F32 decalTransform[16];
      Vector<Rendering::TextureData> decalTextures;

   public:
      TerrainEditor();
      ~TerrainEditor();

      virtual void enable();
      virtual void disable();
      virtual void render();
      virtual void deleteKey();

      virtual void onMouseMoveEvent(const GuiEvent &event);
      virtual void onMouseDownEvent(const GuiEvent &event);
      virtual void onMouseDraggedEvent(const GuiEvent &event);

      virtual void processTick();
      virtual void advanceTime(F32 delta);
      virtual void interpolateTick(F32 delta);

      void updateTerrainPosition();
      void clickTerrainCell(TerrainCell* cell, U32 x, U32 y);
      void switchTool(U32 num);
};

extern TerrainEditor terrainEditor;
void loadEditorAPI(Plugins::PluginAPI* api);
void clickToolList(S32 id);