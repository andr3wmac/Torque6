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

#include "ParticlesDLL.h"
#include <plugins/plugins_shared.h>

#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/utilities.h>

#include <bx/fpumath.h>

using namespace Plugins;
PluginLink Plugins::Link;

bool                       particlesEnabled = false;
bgfx::ProgramHandle        particleShader = BGFX_INVALID_HANDLE;
Rendering::RenderData*     particleRenderData = NULL;
bgfx::VertexBufferHandle   particleVB = BGFX_INVALID_HANDLE;
bgfx::IndexBufferHandle    particleIB = BGFX_INVALID_HANDLE;
Vector<Rendering::InstanceData> particleInstanceData;

PosUVColorVertex particleVerts[4] = 
{
	{-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0xffffffff },
	{-1.0f, 1.0f,  1.0f, 1.0f, 1.0f, 0xffffffff },
	{ 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0xffffffff },
	{ 1.0f, 1.0f,  1.0f, 0.0f, 1.0f, 0xffffffff }
};

U16 particleIndices[6] = {0, 2, 1, 1, 2, 3};

// Called when the plugin is loaded.
void create(PluginLink _link)
{
   Link = _link;

   // Load Shader
   Graphics::ShaderAsset* particleShaderAsset = Link.Graphics.getShaderAsset("Particles:particleShader");
   if ( particleShaderAsset )
      particleShader = particleShaderAsset->getProgram();

   // Register Console Functions
   Link.Con.addCommand("Particles", "enable", enableParticles, "", 1, 1);
   Link.Con.addCommand("Particles", "disable", disableParticles, "", 1, 1);

   const bgfx::Memory* mem;

   mem = Link.bgfx.makeRef(&particleVerts[0], sizeof(PosUVColorVertex) * 4);
   particleVB = Link.bgfx.createVertexBuffer(mem, *Link.Graphics.PosUVColorVertex, BGFX_BUFFER_NONE);

	mem = Link.bgfx.makeRef(&particleIndices[0], sizeof(uint16_t) * 6 );
	particleIB = Link.bgfx.createIndexBuffer(mem, BGFX_BUFFER_NONE);
}

void destroy()
{
   if ( particleVB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyVertexBuffer(particleVB);

   if ( particleIB.idx != bgfx::invalidHandle )
      Link.bgfx.destroyIndexBuffer(particleIB);
}

void refresh()
{
   if ( particleRenderData == NULL )
      particleRenderData = Link.Rendering.createRenderData();

   particleRenderData->indexBuffer = particleIB;
   particleRenderData->vertexBuffer = particleVB;

   // Render in Forward (for now) with our custom terrain shader.
   particleRenderData->shader = particleShader;
   particleRenderData->view = Graphics::ViewTable::Transparent;
   particleRenderData->state = 0
			| BGFX_STATE_RGB_WRITE
			| BGFX_STATE_BLEND_ADD
			| BGFX_STATE_DEPTH_TEST_LESS;

   // Transform
   F32* cubeMtx = new F32[16];
   bx::mtxSRT(cubeMtx, 1, 1, 1, 0, 0, 0, 0, 10, 0);
   particleRenderData->transformTable = cubeMtx;
   particleRenderData->transformCount = 1;

   particleRenderData->instances = &particleInstanceData;
   
   for( U32 n = 0; n < 100; ++n )
   {
      Rendering::InstanceData part1;
      part1.i_data0.set(mRandF(-50.0f, 50.0f), mRandF(-50.0f, 50.0f), mRandF(-50.0f, 50.0f), 0.0f);
      particleInstanceData.push_back(part1);
   }
}

void enableParticles(SimObject *obj, S32 argc, const char *argv[])
{
   particlesEnabled = true;
   refresh();
}

void disableParticles(SimObject *obj, S32 argc, const char *argv[])
{
   particlesEnabled = false;
}