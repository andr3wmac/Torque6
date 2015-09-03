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


#ifndef _POST_RENDERING_H_
#define _POST_RENDERING_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "common.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#ifndef _RENDERABLE_H_
#include <3d/rendering/renderable.h>
#endif

#ifndef _SCENE_FEATURE_H_
#include "3d/scene/feature.h"
#endif

namespace Rendering 
{
   class PostRenderFeature : public Scene::SceneFeature
   {
      private:
         typedef Scene::SceneFeature Parent;

      public:
         S16 mPriority;

         PostRenderFeature() : mPriority(0) { }

         virtual void render() { }
         virtual void resize() { }

         virtual void onActivate();
         virtual void onDeactivate();

         DECLARE_CONOBJECT(PostRenderFeature);
   };

   class PostRendering : public virtual Renderable
   {
      protected:
         Vector<PostRenderFeature*> mPostFeatureList;
         bool                       mBeginEnabled;
         Graphics::Shader*          mBeginShader;
         Graphics::ViewTableEntry*  mBeginView;
         bool                       mFinishEnabled;
         Graphics::Shader*          mFinishShader;
         Graphics::ViewTableEntry*  mFinishView;

         void initBuffers();
         void destroyBuffers();

      public:
         U32                        mPostBufferIdx;
         bgfx::FrameBufferHandle    mPostBuffers[2];

         PostRendering();
         ~PostRendering();

         void addPostFeature(PostRenderFeature* feature);
         void removePostFeature(PostRenderFeature* feature);
         Graphics::ViewTableEntry* overrideBegin();
         Graphics::ViewTableEntry* overrideFinish();

         virtual void preRender();
         virtual void render();
         virtual void postRender();
         virtual void resize();
   };

   void                       postInit();
   void                       postDestroy();
   bgfx::FrameBufferHandle    getPostSource();
   bgfx::FrameBufferHandle    getPostTarget();
   Graphics::ViewTableEntry*  overridePostBegin();
   Graphics::ViewTableEntry*  overridePostFinish();
   void                       flipPostBuffers();
}

#endif