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
#include "decalComponent.h"
#include "graphics/core.h"
#include "rendering/rendering.h"
#include "scene/scene.h"
#include "rendering/renderCamera.h"

// Script bindings.
#include "decalComponent_Binding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx/bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(DecalComponent);

   DecalComponent::DecalComponent()
   {
      mTexturePath   = StringTable->EmptyString;
      mTexture.idx   = bgfx::invalidHandle;

      mDeferredDecalView      = Graphics::getView("DeferredDecal");
      mDeferredDecalShader    = Graphics::getDefaultShader("gui/decal_vs.tsh", "gui/decal_fs.tsh")->mProgram;
      mInverseModelMtxUniform = Graphics::Shader::getUniformMat4("u_invModel");
   }

   DecalComponent::~DecalComponent()
   {

   }

   void DecalComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("DecalComponent");

         addProtectedField("Texture", TypeAssetLooseFilePath, Offset(mTexturePath, DecalComponent), &DecalComponent::setTexture, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");

      endGroup("DecalComponent");
   }

   void DecalComponent::onAddToScene()
   {  
      refresh();
   }

   void DecalComponent::onRemoveFromScene()
   {

   }

   void DecalComponent::refresh()
   {
      Parent::refresh();

      mBoundingBox.minExtents.set(-1.0f, -1.0f, -1.0f);
      mBoundingBox.maxExtents.set(1.0f, 1.0f, 1.0f);
      mBoundingBox.transform(mLocalTransformMatrix);

      // Sanity Checks.
      if ( mOwnerObject == NULL ) return;
   }

   void DecalComponent::loadTexture(StringTableEntry path)
   {
      mTexturePath = path;
      TextureObject* textureObj = TextureManager::loadTexture(mTexturePath, TextureHandle::BitmapKeepTexture, 0);
      if (textureObj != NULL)
      {
         mTexture = textureObj->getBGFXTexture();
      }
   }

   void DecalComponent::preRender(Rendering::RenderCamera* camera)
   {

   }

   void DecalComponent::render(Rendering::RenderCamera* camera)
   {
      // Inverse Model Matrix
      F32 invModelMtx[16];
      bx::mtxInverse(invModelMtx, mTransformMatrix);
      bgfx::setUniform(mInverseModelMtxUniform, invModelMtx);

      bgfx::setTransform(&mTransformMatrix[0]);

      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), camera->getRenderPath()->getDepthTexture());
      bgfx::setTexture(0, Graphics::Shader::getTextureUniform(1), mTexture);

      bgfx::setIndexBuffer(Graphics::cubeIB);
      bgfx::setVertexBuffer(Graphics::cubeVB);
      bgfx::setState(0 | BGFX_STATE_RGB_WRITE | BGFX_STATE_ALPHA_WRITE | BGFX_STATE_DEPTH_TEST_LESS);

      bgfx::submit(mDeferredDecalView->id, mDeferredDecalShader);
   }

   void DecalComponent::postRender(Rendering::RenderCamera* camera)
   {

   }
}
