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
#include "textComponent.h"
#include "graphics/core.h"
#include "3d/rendering/common.h"
#include "3d/scene/core.h"

// Script bindings.
#include "textComponent_ScriptBinding.h"

// Debug Profiling.
#include "debug/profiler.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

namespace Scene
{
   IMPLEMENT_CONOBJECT(TextComponent);

   TextComponent::TextComponent()
   {
      mRenderData = NULL;
      mNVGContext = NULL;
   }

   TextComponent::~TextComponent()
   {
      if ( mNVGContext != NULL )
         nvgDelete(mNVGContext);
   }

   void TextComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();
   }

   void TextComponent::onAddToScene()
   {  
      mView = Graphics::getView("TextTexture");

      const U32 samplerFlags = 0
            | BGFX_TEXTURE_RT
            | BGFX_TEXTURE_MIN_POINT
            | BGFX_TEXTURE_MAG_POINT
            | BGFX_TEXTURE_MIP_POINT
            | BGFX_TEXTURE_U_CLAMP
            | BGFX_TEXTURE_V_CLAMP;
      mTextTexture = bgfx::createTexture2D(256, 256, 1, bgfx::TextureFormat::BGRA8, samplerFlags);
      mTextBuffer = bgfx::createFrameBuffer(1, &mTextTexture, false);

      mNVGContext = nvgCreate(1, mView->id);

      char buf[1024];
      dSprintf(buf, sizeof(buf), "%s/%s.ttf", Con::getVariable("$GUI::fontDirectory"), "lucida console");
      nvgCreateFont(mNVGContext, "lucida console", buf);

      bgfx::setViewSeq(mView->id, true);

      // Render in World
      mRenderData = Rendering::createRenderData();
      mRenderData->view = Graphics::getView("RenderLayer2");
      mRenderData->indexBuffer = Graphics::planeIB;
      mRenderData->vertexBuffer = Graphics::planeVB;
      mRenderData->shader = Graphics::getShader("gui/world_text_vs.sc", "gui/world_text_fs.sc")->mProgram;

      refresh();

      setRendering(true);
   }

   void TextComponent::preRender()
   {

   }

   void TextComponent::render()
   {
      if ( mNVGContext == NULL )
         return;

      bgfx::setViewRect(mView->id, 0, 0, 256, 256);
      bgfx::setViewFrameBuffer(mView->id, mTextBuffer);

      // GUI Orthographic Projection
      float ortho[16];
      bx::mtxOrtho(ortho, 0.0f, 256.0f, 256.0f, 0.0f, 0.0f, 1000.0f);
      bgfx::setViewTransform(mView->id, NULL, ortho);

      bgfx::setViewClear(mView->id
		   , BGFX_CLEAR_COLOR
		   , 0x303030ff
		   , 1.0f
		   , 0
		   );
      bgfx::submit(mView->id);

      nvgBeginFrame(mNVGContext, 256, 256, 1.0f);

      //nvgBeginPath(mNVGContext);
	   //nvgRoundedRect(mNVGContext, 50, 50, 400, 400, 5);
	   //nvgFillColor(mNVGContext, nvgRGBA(255, 255, 0, 255));
	   //nvgFill(mNVGContext);

      nvgFontFace(mNVGContext, "lucida console");
      nvgFontSize(mNVGContext, 32.0f);
      nvgFillColor(mNVGContext, nvgRGBA(255,255,255,255));
      nvgText(mNVGContext, 10, 50, "HELLO WORLD!", NULL);

      nvgEndFrame(mNVGContext);
   }

   void TextComponent::postRender()
   {

   }

   void TextComponent::refresh()
   {
      Parent::refresh();

      // Sanity Checks.
      if ( mOwnerEntity == NULL ) return;

      // Debug Render.
      if ( mRenderData )
      {
         // Base Component transform matrix is always slot 0 in the transform table.
         mRenderData->transformTable = &mTransformMatrix[0];
         mRenderData->transformCount = 1;

         textures.clear();
         mRenderData->textures = &textures;

         // Text Texture
         Rendering::TextureData textTex;
         textTex.uniform = Graphics::Shader::getTextureUniform(0);
         textTex.handle = mTextTexture;
         textures.push_back(textTex);
      }
   }
}
