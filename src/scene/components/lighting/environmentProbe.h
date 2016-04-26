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


#ifndef _ENVIRONMENT_PROBE_H_
#define _ENVIRONMENT_PROBE_H_

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

#ifndef _CUBEMAP_PROCESSOR_H_
#include "lighting/cubemapProcessor.h"
#endif

#ifndef _SCENE_CORE_H_
#include "scene/scene.h"
#endif

#ifndef _DEBUG_MODE_H_
#include "debug/debugMode.h"
#endif

namespace Scene
{
   class EnvironmentProbeFilter;

   // Environment Probe
   class EnvironmentProbe : public BaseComponent, public Rendering::RenderHook, public Scene::ScenePreprocessor
   {
      private:
         typedef BaseComponent Parent;

      protected:
         U32                           mState;
         Graphics::Shader*             mDiffuseShader;
         Graphics::Shader*             mSpecularShader;

         Rendering::RenderCamera*      mEnvironmentCamera;
         EnvironmentProbeFilter*       mEnvironmentCameraFilter;
         bgfx::TextureHandle           mEnvironmentCubemap;
         bgfx::FrameBufferHandle       mEnvironmentCubemapBuffers[6];
         U8                            mEnvironmentCaptureSide;

         Lighting::CubemapProcessor*   mCubemapProcessor;
         bgfx::TextureHandle           mBRDFTexture;
         bgfx::TextureHandle           mIrradianceCubemap;
         bgfx::TextureHandle           mRadianceCubemap;

         void startEnvironmentCapture();
         void captureEnvironment();
         void endEnvironmentCapture();

         void initBuffers();
         void destroyBuffers();

      public:
         EnvironmentProbe();
         ~EnvironmentProbe();

         // BaseComponent
         void onAddToScene();
         void onRemoveFromScene();
         void refresh();

         // Rendering::RenderHook
         void preRender(Rendering::RenderCamera* camera);
         void render(Rendering::RenderCamera* camera);
         void postRender(Rendering::RenderCamera* camera);
         void resize();

         // Scene::ScenePreprocessor
         void preprocess();

         static void initPersistFields();
         DECLARE_CONOBJECT(EnvironmentProbe);
   };

   class EnvironmentProbeFilter : public Rendering::RenderFilter
   {
      public:
         EnvironmentProbeFilter();
         void execute();
   };

   // EnvProbeDebug Debug Mode visually displays bounds of environment probes.
   class EnvProbeDebug : public Debug::DebugMode
   {
   public:
      static bool EnvProbeDebugEnabled;

      void onEnable();
      void onDisable();
      void render(Rendering::RenderCamera* camera);

      DECLARE_DEBUG_MODE("EnvProbe", EnvProbeDebug);
   };
}

#endif // _ENVIRONMENT_PROBE_H_