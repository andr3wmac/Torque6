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

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _BASE_COMPONENT_H_
#include "scene/components/baseComponent.h"
#endif

#ifndef _RENDER_CAMERA_H
#include "rendering/renderCamera.h"
#endif

namespace Scene
{
   // Directional Light + Cascaded Shadow Mapping
   // Based On: https://github.com/bkaradzic/bgfx/blob/master/examples/16-shadowmaps/

   class DirectionalLight : public BaseComponent, public Rendering::RenderHook
   {
      private:
         typedef BaseComponent Parent;

      protected:
         bool                       mEnabled;

         // Settings
         ColorF                     mColor;
         Point3F                    mDirection;
         F32                        mSplitDistribution;
         F32                        mFarPlane;
         F32                        mBias;
         F32                        mNormalOffset;
         bool                       mDebugCascades;
         bgfx::UniformHandle        mShadowParamsUniform;
         
         //
         Graphics::ViewTableEntry*  mDeferredLightView;
         Graphics::Shader*          mLightShader;
         Graphics::Shader*          mDebugLightShader;
         F32                        mLightView[16];
         F32                        mLightProj[4][16];

         // Cascades
         U16                        mCascadeSize;
         bgfx::TextureHandle        mCascadeTextures[4];
         bgfx::FrameBufferHandle    mCascadeBuffers[4];
         Graphics::ViewTableEntry*  mCascadeViews[4];
         bgfx::UniformHandle        mCascadeMtxUniforms[4];
         F32                        mCascadeMtx[4][16];

         // PCF Shadow Map Shaders
         Graphics::Shader*          mPCFShader;
         Graphics::Shader*          mPCFSkinnedShader;

         // RenderCamera from camera component
         Rendering::RenderCamera*   mCamera;

         void initBuffers();
         void destroyBuffers();

         void worldSpaceFrustumCorners(F32* _corners24f, F32 _near, F32 _far, F32 _projWidth, F32 _projHeight, const F32* __restrict _invViewMtx);
         void splitFrustum(F32* _splits, U8 _numSplits, F32 _near, F32 _far, F32 _splitWeight = 0.75f);

      public:
         DirectionalLight();
         ~DirectionalLight();

         virtual void preRender(Rendering::RenderCamera* camera);
         virtual void render(Rendering::RenderCamera* camera);
         virtual void postRender(Rendering::RenderCamera* camera);
         virtual void resize();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();

         void refresh();

         static void initPersistFields();

         DECLARE_CONOBJECT(DirectionalLight);
   };
}

#endif