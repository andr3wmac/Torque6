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

#ifndef _DEBUG_MODE_H_
#include "debug/debugMode.h"
#endif

#ifndef _SHADOWS_H_
#include "lighting/shadows.h"
#endif

namespace Scene
{
   // Directional Light
   class DirectionalLight : public BaseComponent, public Rendering::RenderHook
   {
      private:
         typedef BaseComponent Parent;

      protected:
         // Settings
         bool     mEnabled;
         ColorF   mColor;
         Point3F  mDirection;

         // ShadowMap
         Lighting::CascadedShadowMap*  mShadowMap;
         U16                           mShadowCascadeSize;
         F32                           mSplitDistribution;
         F32                           mFarPlane;
         F32                           mBias;
         F32                           mNormalOffset;
         
         // Shaders
         Graphics::Shader* mLightShader;
         Graphics::Shader* mDebugLightShader;

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