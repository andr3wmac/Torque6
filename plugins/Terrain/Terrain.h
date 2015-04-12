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

#include <bgfx.h>

extern "C" 
{
   PLUGIN_FUNC void create();
   PLUGIN_FUNC void destroy();
   PLUGIN_FUNC void render();
}

extern bool terrainEnabled;
extern bgfx::TextureHandle terrainTextures[1];
extern bgfx::FrameBufferHandle terrainTextureBuffer;
extern bgfx::ProgramHandle terrainMegaShader;
extern bgfx::TextureHandle mTextures[3];
extern Vector<Rendering::TextureData> mTextureData;
extern bool redrawMegatexture;
extern Rendering::UniformSet mUniformSet;
extern Point2F lastFocusPoint;

void loadTexture(SimObject *obj, S32 argc, const char *argv[]);
void loadEmptyTerrain(SimObject *obj, S32 argc, const char *argv[]);
void loadHeightMap(SimObject *obj, S32 argc, const char *argv[]);
void enableTerrain(SimObject *obj, S32 argc, const char *argv[]);
void disableTerrain(SimObject *obj, S32 argc, const char *argv[]);
void refresh();