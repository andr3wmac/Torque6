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
#include "cameraComponent.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "scene/scene.h"

// Script bindings.
#include "cameraComponent_Binding.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(CameraComponent);

   CameraComponent::CameraComponent()
   {
      mRender        = false;
      mRenderCamera  = NULL;
   }

   CameraComponent::~CameraComponent()
   {
      if (mRenderCamera != NULL)
         SAFE_DELETE(mRenderCamera);
   }

   void CameraComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("CameraComponent");

         addProtectedField("Render", TypeBool, Offset(mRender, CameraComponent), &CameraComponent::setRender, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");

      endGroup("CameraComponent");
   }

   void CameraComponent::onAddToScene()
   {  
      refresh();
   }

   void CameraComponent::onRemoveFromScene()
   {
      //
   }

   void CameraComponent::setRender(bool value)
   {
      if (mRender == value)
         return;

      mRender = value;

      // Destroy render camera
      if (mRenderCamera != NULL)
      {
          Rendering::removeRenderCamera(mRenderCamera);
          SAFE_DELETE(mRenderCamera);
      }

      // Activating rendering
      if (mRender)
      {
         mRenderCamera = new Rendering::RenderCamera();
         Rendering::addRenderCamera(mRenderCamera);
      }
   }

   Rendering::RenderCamera* CameraComponent::getRenderCamera()
   {
      return mRenderCamera;
   }

   // -----------------------------------------------------

   void CameraComponent::refresh()
   {
      Parent::refresh();

      if (!mRenderCamera)
         return;

      VectorF up(0.0f, 0.0f, 1.0f);
      Point3F look;
      Point3F cameraForward(1.0f, 0.0f, 0.0f);

      bx::vec3MulMtx(look, cameraForward, mTransformMatrix);
      bx::mtxLookAt(mRenderCamera->viewMatrix, mWorldPosition, look, up);

      mRenderCamera->position = mWorldPosition;
   }
}
