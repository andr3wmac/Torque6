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

#include "TerrainDLL.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>
#include <bx/fpumath.h>

#include "TerrainCell.h"

using namespace Plugins;

bool terrainEnabled = false;
Vector<TerrainCell> terrainGrid;

// Called when the plugin is loaded.
void create()
{
   // Register Console Functions
   Link.Con.addCommand("Terrain", "loadHeightMap", loadHeightMap, "", 4, 4);
   Link.Con.addCommand("Terrain", "loadTexture", loadTexture, "", 5, 5);
   Link.Con.addCommand("Terrain", "enable", enableTerrain, "", 1, 1);
   Link.Con.addCommand("Terrain", "disable", disableTerrain, "", 1, 1);
   Link.Con.addCommand("Terrain", "stitchEdges", stitchEdges, "", 1, 1);
}

void destroy()
{
   //
}

// Console Functions
void enableTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   terrainEnabled = true;
}

void disableTerrain(SimObject *obj, S32 argc, const char *argv[])
{
   terrainEnabled = false;
}

void loadHeightMap(SimObject *obj, S32 argc, const char *argv[])
{
   S32 gridX = dAtoi(argv[1]);
   S32 gridY = dAtoi(argv[2]);
   for(U32 n = 0; n < terrainGrid.size(); ++n)
   {
      if ( terrainGrid[n].gridX != gridX || terrainGrid[n].gridY != gridY )
         continue;

      terrainGrid[n].loadHeightMap(argv[3]);
      return;
   }

   // Create new cell
   TerrainCell cell(gridX, gridY);
   terrainGrid.push_back(cell);
   terrainGrid.back().loadHeightMap(argv[3]);
}

void loadTexture(SimObject *obj, S32 argc, const char *argv[])
{
   S32 gridX = dAtoi(argv[1]);
   S32 gridY = dAtoi(argv[2]);
   for(U32 n = 0; n < terrainGrid.size(); ++n)
   {
      if ( terrainGrid[n].gridX != gridX || terrainGrid[n].gridY != gridY )
         continue;

      terrainGrid[n].loadTexture(dAtoi(argv[3]), argv[4]);
      return;
   }

   // Create new cell
   TerrainCell cell(gridX, gridY);
   terrainGrid.push_back(cell);
   terrainGrid.back().loadTexture(dAtoi(argv[3]), argv[4]);
}

void stitchEdges(SimObject *obj, S32 argc, const char *argv[])
{
   for(U32 i = 0; i < terrainGrid.size(); ++i)
   {
      TerrainCell* curCell = &terrainGrid[i];

      for(U32 n = 0; n < terrainGrid.size(); ++n)
      {
         TerrainCell* compareCell = &terrainGrid[n];
         
         // Left
         if ( compareCell->gridX == (curCell->gridX - 1) && compareCell->gridY == curCell->gridY )
         {
            for(U32 y = 0; y < curCell->height; ++y)
            {
               U32 left_index = ((y + 1) * compareCell->width) - 1;
               U32 right_index = y * curCell->width;
               F32 average_height = (compareCell->heightMap[left_index] + curCell->heightMap[right_index]) / 2.0f;

               compareCell->heightMap[left_index] = average_height;
               curCell->heightMap[right_index] = average_height;
            }

            compareCell->rebuild();
            curCell->rebuild();
         }

         // Bottom
         if ( compareCell->gridY == (curCell->gridY - 1) && compareCell->gridX == curCell->gridX )
         {
            for(U32 x = 0; x < curCell->width; ++x)
            {
               U32 bottom_index = (curCell->width * (curCell->height - 2)) + x;
               U32 top_index = x;
               F32 average_height = (compareCell->heightMap[bottom_index] + curCell->heightMap[top_index]) / 2.0f;

               compareCell->heightMap[bottom_index] = average_height;
               curCell->heightMap[top_index] = average_height;
            }

            compareCell->rebuild();
            curCell->rebuild();
         }
      }
   }
}