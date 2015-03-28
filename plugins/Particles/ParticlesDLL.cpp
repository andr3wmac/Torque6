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
bgfx::TextureHandle        particleTexture = BGFX_INVALID_HANDLE;
Vector<Rendering::InstanceData> particleInstanceData;
Vector<Rendering::TextureData>  particleTextureData;

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
   SimObject2::dynClassRep.registerClass();

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

   TextureObject* texture_obj = Link.Graphics.loadTexture("./smoke.png", TextureHandle::TextureHandleType::BitmapKeepTexture, false, false, false);
   if ( texture_obj )
      particleTexture = texture_obj->getBGFXTexture();
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
         | BGFX_STATE_ALPHA_WRITE
         | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
			| BGFX_STATE_DEPTH_TEST_LESS;

   // Transform of emitter.
   F32* cubeMtx = new F32[16];
   bx::mtxSRT(cubeMtx, 1, 1, 1, 0, 0, 0, 0, 0, 0);
   particleRenderData->transformTable = cubeMtx;
   particleRenderData->transformCount = 1;

   particleRenderData->instances = &particleInstanceData;
   
   // Generate 50k particle instances for stress testing.
   for( U32 n = 0; n < 50000; ++n )
   {
      Rendering::InstanceData particle;

      // Random Position
      particle.i_data0.set(mRandF(-200.0f, 200.0f), mRandF(-300.0f, 0.0f), mRandF(-200.0f, 200.0f), 0.0f);

      // Random Color
      particle.i_data1.set(mRandF(0.0f, 1.0f), mRandF(0.0f, 1.0f), mRandF(0.0f, 1.0f), 1.0f);

      particleInstanceData.push_back(particle);
   }

   // Setup texture.
   particleTextureData.clear();
   particleRenderData->textures = &particleTextureData;
   Rendering::TextureData* texture = particleRenderData->addTexture();
   texture->handle = particleTexture;
   texture->uniform = Link.Graphics.getTextureUniform(0);
}

void processTick()
{
   // Move the particles upward on tick.
   for( U32 n = 0; n < particleInstanceData.size(); ++n )
      particleInstanceData[n].i_data0.y += 0.1f;
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

AbstractClassRep* SimObject2::getClassRep() const { return &SimObject2::dynClassRep; }
AbstractClassRep* SimObject2::getStaticClassRep() { return &dynClassRep; }
AbstractClassRep* SimObject2::getParentStaticClassRep() { return Parent::getStaticClassRep(); }
AbstractClassRep* SimObject2::getContainerChildStaticClassRep() { return NULL; }
AbstractClassRep::WriteCustomTamlSchema SimObject2::getStaticWriteCustomTamlSchema() { return NULL; }
SimObject2Rep SimObject2::dynClassRep("SimObject2", 0, -1, 0, SimObject2::getParentStaticClassRep());