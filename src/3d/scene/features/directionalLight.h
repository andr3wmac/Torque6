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


#ifndef _DIRECTIONAL_LIGHT_FEATURE_H_
#define _DIRECTIONAL_LIGHT_FEATURE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERINGCOMMON_H_
#include "common.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#ifndef _SCENE_FEATURE_H_
#include "3d/scene/feature.h"
#endif

#ifndef _RENDERABLE_H_
#include <3d/rendering/renderable.h>
#endif

namespace Scene
{
   // Directional Light + Cascaded Shadow Mapping
   // Based On: https://github.com/bkaradzic/bgfx/blob/master/examples/16-shadowmaps/

   class DirectionalLight : public SceneFeature, public virtual Renderable
   {
      private:
         typedef SceneFeature Parent;

      protected:
         Graphics::ViewTableEntry*  mDeferredLightView;
         Graphics::Shader*          mLightShader;
         F32                        mLightView[16];
         F32                        mLightProj[4][16];

         // Cascades
         U16                        mCascadeSize;
         bgfx::TextureHandle        mCascadeTextures[4];
         bgfx::FrameBufferHandle    mCascadeBuffers[4];
         Graphics::ViewTableEntry*  mCascadeViews[4];
         bgfx::UniformHandle        mCascadeMtxUniforms[4];
         F32                        mCascadeMtx[4][16];

         // Cascade Blur
         bgfx::FrameBufferHandle    mBlurBuffer;
         Graphics::Shader*          mHBlurShader;
         Graphics::Shader*          mVBlurShader;
         bgfx::UniformHandle        mBlurParamsUniform;
         Point4F                    mBlurParams;
         Graphics::ViewTableEntry*  mVBlurViews[4];
         Graphics::ViewTableEntry*  mHBlurViews[4];

         // Shadow Buffer + Shadow Buffer Blur
         Graphics::Shader*          mShadowBufferShader;
         Graphics::Shader*          mShadowBufferHBlurShader;
         Graphics::Shader*          mShadowBufferVBlurShader;
         bgfx::TextureHandle        mShadowTexture;
         bgfx::FrameBufferHandle    mShadowBuffer;
         Graphics::ViewTableEntry*  mShadowBufferView;
         bgfx::FrameBufferHandle    mShadowBlurBuffer;
         Graphics::ViewTableEntry*  mShadowBufferVBlurView;
         Graphics::ViewTableEntry*  mShadowBufferHBlurView;

         // Variance Shadow Map Shaders
         Graphics::Shader*          mVSMShader;
         Graphics::Shader*          mVSMSkinnedShader;

         void initBuffers();
         void destroyBuffers();

         void worldSpaceFrustumCorners(F32* _corners24f, F32 _near, F32 _far, F32 _projWidth, F32 _projHeight, const F32* __restrict _invViewMtx);
         void splitFrustum(F32* _splits, U8 _numSplits, F32 _near, F32 _far, F32 _splitWeight = 0.75f);

      public:
         DirectionalLight();
         ~DirectionalLight();

         virtual void preRender();
         virtual void render();
         virtual void postRender();

         virtual void onActivate();
         virtual void onDeactivate();

         void refresh();

         DECLARE_CONOBJECT(DirectionalLight);
   };
}

#endif