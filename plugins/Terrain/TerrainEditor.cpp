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
#include <graphics/utilities.h>
#include <bx/fpumath.h>

#include "TerrainCell.h"

// Link to Editor Plugin
#include "../Editor/Editor_pluginAPI.h"

using namespace Plugins;

TerrainEditor terrainEditor;

TerrainEditor::TerrainEditor()
{
   name = "Terrain Editor";
   terrainEditorArea = -1;
   paintTerrain = false;
   mousePosition.set(0, 0);
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
      Link.SysGUI.label("Camera Settings");
      Link.SysGUI.separator();
      Link.SysGUI.slider("Speed", 50, 0, 100);
      Link.SysGUI.button("Return to Center", "", NULL);

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

void TerrainEditor::processTick()
{
   if ( paintTerrain )
   {
      paintTerrain = false;

      Point3F world_ray = Link.Rendering.screenToWorld(mousePosition);
      Point3F startPos = Link.Scene.getActiveCamera()->getPosition();
      Point3F endPos = startPos + (world_ray * 1000.0f);

      Point3F direction = endPos - startPos;
      F32 length = direction.len();
      direction.normalize();

      Point3F rayPos = startPos;
      for(U32 i = 0; i < length; i++)
      {
         for(U32 n = 0; n < terrainGrid.size(); ++n)
         {
            TerrainCell* cell = &terrainGrid[n];

            Point2I cellPos(rayPos.x - (cell->gridX * cell->width), 
                            rayPos.z - (cell->gridY * cell->height));

            if ( cellPos.x >= cell->width || cellPos.x < 0 || cellPos.y >= cell->height || cellPos.y < 0 )
               continue;

            S32 mapPos = (cellPos.y * cell->width) + cellPos.x;
            F32 heightValue = cell->heightMap[mapPos];

            if ( rayPos.y <= heightValue )
            {
               clickTerrainCell(cell, cellPos.x, cellPos.y);
               return;
            }
         }

         rayPos += direction;
      }
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
   
   for ( S32 area_y = -20; area_y < 20; ++area_y )
   {
      for ( S32 area_x = -20; area_x < 20; ++area_x )
      {
         S32 brush_x = x + area_x;
         if ( brush_x < 0 || brush_x > cell->width ) continue;
         S32 brush_y = y + area_y;
         if ( brush_y < 0 || brush_y > cell->height ) continue;

         Point2F area_point(area_x, area_y);
         F32 dist = area_point.len();
         F32 impact = 10.0 - dist;
         if ( impact < 0.0f ) continue;

         U32 heightMapPos = (brush_y  * cell->width) + brush_x;
         cell->heightMap[heightMapPos] += 10.0 - dist;
      }
   }
   
   //cell->rebuild();
}

void loadEditorAPI(PluginAPI* api)
{
   EditorAPI* editorAPI = static_cast<EditorAPI*>(api);
   editorAPI->addEditor(&terrainEditor);
}