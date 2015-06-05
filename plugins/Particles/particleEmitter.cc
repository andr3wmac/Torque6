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

#include "console/consoleTypes.h"
#include "particleEmitter.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"

// Script bindings.
#include "particleEmitter_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_PLUGIN_CONOBJECT(ParticleEmitter);

   bgfx::VertexBufferHandle ParticleEmitter::vertexBuffer = BGFX_INVALID_HANDLE;
   bgfx::IndexBufferHandle  ParticleEmitter::indexBuffer = BGFX_INVALID_HANDLE;

   ParticleEmitter::ParticleEmitter()
   {
      mCount = 100;
      mRange = 100;
      mSpeed = 0.1f;

      mShader.idx = bgfx::invalidHandle;
      mRenderData = NULL;
      mTexture.idx = bgfx::invalidHandle;
   }

   void ParticleEmitter::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addField("count", Plugins::Link.Con.TypeS32, Offset(mCount, ParticleEmitter), "");
      addField("range", Plugins::Link.Con.TypeS32, Offset(mRange, ParticleEmitter), "");
      addField("speed", Plugins::Link.Con.TypeF32, Offset(mSpeed, ParticleEmitter), "");
   }

   void ParticleEmitter::onAddToScene()
   {  
      setProcessTicks(true);

      // Load Shader
      Graphics::ShaderAsset* particleShaderAsset = Plugins::Link.Graphics.getShaderAsset("Particles:particleShader");
      if ( particleShaderAsset )
         mShader = particleShaderAsset->getProgram();

      // Load Texture
      TextureObject* texture_obj = Plugins::Link.Graphics.loadTexture("smoke.png", TextureHandle::BitmapKeepTexture, false, false, false);
      if ( texture_obj )
         mTexture = texture_obj->getBGFXTexture();

      refresh();
   }

   void ParticleEmitter::refresh()
   {
      Parent::refresh();

      if ( mShader.idx == bgfx::invalidHandle ||
           mTexture.idx == bgfx::invalidHandle ||
           mCount < 1 )
         return;

      if ( mRenderData == NULL )
         mRenderData = Plugins::Link.Rendering.createRenderData();

      mRenderData->indexBuffer = indexBuffer;
      mRenderData->vertexBuffer = vertexBuffer;

      // Render in Forward (for now) with our custom terrain shader.
      mRenderData->shader = mShader;
      mRenderData->view = Plugins::Link.Graphics.getView("TransparencyBuffer", "", false);
      mRenderData->state = 0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            | BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE)
				| BGFX_STATE_BLEND_INDEPENDENT;
      mRenderData->stateRGBA = 0
            | BGFX_STATE_BLEND_FUNC_RT_1(BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_INV_SRC_ALPHA);

      // Transform of emitter.
      mRenderData->transformTable = &mTransformMatrix[0];
      mRenderData->transformCount = 1;

      // Instances
      mParticles.clear();
      mRenderData->instances = &mInstanceData;
   
      // Generate 50k particle instances for stress testing.
      for( U32 n = 0; n < mCount; ++n )
      {
         Particle part;
         emitParticle(&part);
         mParticles.push_back(part);
      }

      // Textures
      mTextureData.clear();
      mRenderData->textures = &mTextureData;
      Rendering::TextureData* texture = mRenderData->addTexture();
      texture->handle = mTexture;
      texture->uniform = Plugins::Link.Graphics.getTextureUniform(0);
   }

   void ParticleEmitter::interpolateTick( F32 delta )
   {  
      //
   }

   void ParticleEmitter::processTick()
   {  
      // 
   }

   void ParticleEmitter::advanceTime( F32 timeDelta )
   {  
      mInstanceData.clear();

      //
      for(U32 n = 0; n < mParticles.size(); ++n)
      {
         Particle* part = &mParticles[n];
         part->velocity += Point3F(0.0f, -9.81f, 0.0f) * timeDelta;
         part->position += part->velocity * timeDelta;

         part->lifetime -= timeDelta;
         if ( part->lifetime < 0 )
            emitParticle(part);

         Rendering::InstanceData particle;
         particle.i_data0.set(part->position.x, part->position.y, part->position.z, 0.0f);
         particle.i_data1.set(part->color.red, part->color.green, part->color.blue, mClampF(part->lifetime, 0.0f, 1.0f));
         mInstanceData.push_back(particle);
      }
   }

   void ParticleEmitter::emitParticle(Particle* part)
   {
      part->position.set(mRandF(-mRange, mRange), mRandF(-mRange, mRange), mRandF(-mRange, mRange));
      part->velocity.set(mRandF(-50, 50), mRandF(0, 100), mRandF(-50, 50));
      part->color.set(mRandF(0.0f, 1.0f), mRandF(0.0f, 1.0f), mRandF(0.0f, 1.0f), 1.0f);
      part->lifetime = mRandF(1.0f, 10.0f);
   }
}
