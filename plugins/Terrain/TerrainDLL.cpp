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
bgfx::TextureHandle terrainTextures[2] = {BGFX_INVALID_HANDLE, BGFX_INVALID_HANDLE};
bgfx::FrameBufferHandle terrainTextureBuffer = BGFX_INVALID_HANDLE;
bgfx::ProgramHandle terrainMegaShader = BGFX_INVALID_HANDLE;

// Called when the plugin is loaded.
void create()
{
   // Register Console Functions
   Link.Con.addCommand("Terrain", "loadHeightMap", loadHeightMap, "", 4, 4);
   Link.Con.addCommand("Terrain", "loadTexture", loadTexture, "", 5, 5);
   Link.Con.addCommand("Terrain", "enable", enableTerrain, "", 1, 1);
   Link.Con.addCommand("Terrain", "disable", disableTerrain, "", 1, 1);
   Link.Con.addCommand("Terrain", "stitchEdges", stitchEdges, "", 1, 1);

   // Load Shader
   Graphics::ShaderAsset* terrainMegaShaderAsset = Plugins::Link.Graphics.getShaderAsset("Terrain:megaShader");
   if ( terrainMegaShaderAsset )
      terrainMegaShader = terrainMegaShaderAsset->getProgram();

   const U32 samplerFlags = 0
      | BGFX_TEXTURE_RT
      | BGFX_TEXTURE_MIN_POINT
      | BGFX_TEXTURE_MAG_POINT
      | BGFX_TEXTURE_MIP_POINT
      | BGFX_TEXTURE_U_CLAMP
      | BGFX_TEXTURE_V_CLAMP;

   // G-Buffer
   terrainTextures[0] = Link.bgfx.createTexture2D(2048, 2048, 1, bgfx::TextureFormat::BGRA8, BGFX_TEXTURE_RT, NULL);
   terrainTextures[1] = Link.bgfx.createTexture2D(2048, 2048, 1, bgfx::TextureFormat::D16, BGFX_TEXTURE_RT_BUFFER_ONLY, NULL);
   terrainTextureBuffer = Link.bgfx.createFrameBuffer(BX_COUNTOF(terrainTextures), terrainTextures, false);
}

void render()
{
   F32 proj[16];
   bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
   Link.bgfx.setViewFrameBuffer(Graphics::ViewTable::TerrainTexture, terrainTextureBuffer);
   Link.bgfx.setViewTransform(Graphics::ViewTable::TerrainTexture, NULL, proj, BGFX_VIEW_STEREO, NULL);
   Link.bgfx.setViewRect(Graphics::ViewTable::TerrainTexture, 0, 0, 2048, 2048);

   Link.bgfx.setViewClear(Graphics::ViewTable::TerrainTexture,
      BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH,
      0xffff00ff, // YELLOW for debugging.
      1.0f, 
      0);
   Link.bgfx.submit(Graphics::ViewTable::TerrainTexture, 0);

   Link.bgfx.setProgram(terrainMegaShader);
   Link.bgfx.setState(BGFX_STATE_RGB_WRITE|BGFX_STATE_ALPHA_WRITE, 0);
   Link.Graphics.fullScreenQuad(2048, 2048);
   Link.bgfx.submit(Graphics::ViewTable::TerrainTexture, 0);
}

void destroy()
{
   //
   Link.bgfx.destroyFrameBuffer(terrainTextureBuffer);
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
   TerrainCell cell(&terrainTextures[0], gridX, gridY);
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
   TerrainCell cell(&terrainTextures[0], gridX, gridY);
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