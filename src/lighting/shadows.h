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


#ifndef _SHADOWS_H_
#define _SHADOWS_H_

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

#ifndef _DEBUG_MODE_H_
#include "debug/debugMode.h"
#endif

namespace Lighting
{
   // Cascaded Shadow Mapping
   // Based On: https://github.com/bkaradzic/bgfx/blob/master/examples/16-shadowmaps/

   class CascadedShadowMap : public Rendering::RenderHook
   {
      protected:
         bool                       mEnabled;

         // Settings
         Point3F                    mDirection;
         F32                        mSplitDistribution;
         F32                        mFarPlane;
         F32                        mBias;
         F32                        mNormalOffset;
         bgfx::UniformHandle        mShadowParamsUniform;
         
         //
         F32                        mLightView[16];
         F32                        mLightProj[4][16];

         bgfx::TextureHandle        mShadowMap;
         bgfx::FrameBufferHandle    mShadowMapBuffer;
         F32                        mShadowMtx[4][16];
         bgfx::UniformHandle        mShadowMtxUniform;

         // Cascades
         U16                        mCascadeSize;
         Graphics::ViewTableEntry*  mCascadeViews[4];

         // PCF ShadowMap shaders
         Graphics::Shader*          mPCFShader;
         Graphics::Shader*          mPCFSkinnedShader;

         void worldSpaceFrustumCorners(F32* _corners24f, F32 _near, F32 _far, F32 _projWidth, F32 _projHeight, const F32* __restrict _invViewMtx);
         void splitFrustum(F32* _splits, U8 _numSplits, F32 _near, F32 _far, F32 _splitWeight = 0.75f);

      public:
         CascadedShadowMap();
         ~CascadedShadowMap();

         void init(S16 cascadeSize);
         void destroy();
         void configure(Point3F direction, F32 splitDistribution, F32 farPlane, F32 bias, F32 normalOffset);
         void render(Rendering::RenderCamera* camera);

         bgfx::TextureHandle getShadowMap() { return mShadowMap; }
   };

   // ShadowMapCascadeDebug Debug Mode visually displays ShadowMap cascades
   class ShadowMapCascadeDebug : public Debug::DebugMode
   {
      public:
         static bool CascadeDebugEnabled;

         void onEnable();
         void onDisable();

         DECLARE_DEBUG_MODE("ShadowMapCascade", ShadowMapCascadeDebug);
   };
}

#endif // _SHADOWS_H_