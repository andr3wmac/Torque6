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

#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include <3d/entity/components/baseComponent.h>
#endif

namespace Scene 
{
   struct Particle
   {
      Point3F velocity;
      Point3F position;
      ColorF  color;
      F32     lifetime;
   };

   class ParticleEmitter : public BaseComponent, public virtual Tickable
   {
      private:
         typedef BaseComponent Parent;

         S32                              mCount;
         S32                              mRange;
         F32                              mSpeed;

         Vector<Particle>                 mParticles;

         bgfx::ProgramHandle              mShader;
         Rendering::RenderData*           mRenderData;
         bgfx::TextureHandle              mTexture;
         Vector<Rendering::InstanceData>  mInstanceData;
         Vector<Rendering::TextureData>   mTextureData;

      protected:
         virtual void interpolateTick( F32 delta );
         virtual void processTick();
         virtual void advanceTime( F32 timeDelta );

      public:
         ParticleEmitter();

         static bgfx::VertexBufferHandle  vertexBuffer;
         static bgfx::IndexBufferHandle   indexBuffer;

         void emitParticle(Particle* part);
         void onAddToScene();
         void refresh();

         static void initPersistFields();

         DECLARE_PLUGIN_CONOBJECT(ParticleEmitter);
   };
}

#endif // _PARTICLE_EMITTER_H_