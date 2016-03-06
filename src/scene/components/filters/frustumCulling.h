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


#ifndef _DLAA_FEATURE_H_
#define _DLAA_FEATURE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx/bgfx.h>
#endif

#ifndef _RENDER_CAMERA_H
#include "rendering/renderCamera.h"
#endif

#ifndef _BASE_COMPONENT_H_
#include <scene/components/baseComponent.h>
#endif

#ifndef _DEBUG_MODE_H_
#include "debug/debugMode.h"
#endif

namespace Scene
{
   // Frustum Culling – on steroids!
   // http://blog.bwhiting.co.uk/?p=355

   class FrustumCulling : public BaseComponent, public Rendering::RenderFilter
   {
      private:
         typedef BaseComponent Parent;

      protected:
         S8 mPlaneCache[TORQUE_MAX_RENDER_DATA];

      public:
         FrustumCulling();
         ~FrustumCulling();

         virtual void onAddToScene();
         virtual void onRemoveFromScene();

         virtual void onAddToCamera();
         virtual void onRemoveFromCamera();

         virtual void execute();

         DECLARE_CONOBJECT(FrustumCulling);
   };

   // FrustumCulling Debug Mode that displays the bounding spheres being tested.
   class FrustumCullingDebugger : public Debug::DebugMode
   {
      public:
         void render(Rendering::RenderCamera* camera);

         DECLARE_DEBUG_MODE("FrustumCulling", FrustumCullingDebugger);
   };
}

#endif