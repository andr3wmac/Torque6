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
#include "3d/scene/scene.h"

// Script bindings.
#include "textComponent_Binding.h"

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
   IMPLEMENT_CONOBJECT(TextComponent);

   bool TextComponent::smCanRender = false;

   TextComponent::TextComponent()
   {
      mRenderData       = NULL;
      mRedrawText       = false;
      mText             = StringTable->insert("TextComponent");
      mTextSize         = 32.0f;
      mTextTexture.idx  = bgfx::invalidHandle;
      mTextureWidth     = 256.0f;
      mTextureHeight    = 256.0f;
      mUScrollSpeed     = 0.0f;
      mVScrollSpeed     = 0.0f;

      mTextColor.set(1.0f, 1.0f, 1.0f, 1.0f);
      initTexture();
   }

   TextComponent::~TextComponent()
   {
      destroyTexture();
   }

   void TextComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("TextComponent");

         addProtectedField("Text", TypeString, Offset(mText, TextComponent), &setText, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("TextColor", TypeColorF, Offset(mTextColor, TextComponent), &setTextColor, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("TextSize", TypeF32, Offset(mTextSize, TextComponent), &setTextSize, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("TextureWidth", TypeF32, Offset(mTextureWidth, TextComponent), &setTextureWidth, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");
         addProtectedField("TextureHeight", TypeF32, Offset(mTextureHeight, TextComponent), &setTextureHeight, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");

         addField("UScrollSpeed", TypeF32, Offset(mUScrollSpeed, TextComponent), "");
         addField("VScrollSpeed", TypeF32, Offset(mVScrollSpeed, TextComponent), "");

      endGroup("TextComponent");
   }

   void TextComponent::initTexture()
   {
      destroyTexture();

      // Texture we copy text to.
      mTextTexture = bgfx::createTexture2D(mTextureWidth, mTextureHeight, 1, bgfx::TextureFormat::BGRA8);
   }
   void TextComponent::destroyTexture()
   {
      if (bgfx::isValid(mTextTexture))
         bgfx::destroyTexture(mTextTexture);
   }

   void TextComponent::onAddToScene()
   {  
      mRenderData = Rendering::createRenderData();
      mRenderData->flags         = 0;
      mRenderData->view          = Graphics::getView("TransparencyBuffer", 3000);
      mRenderData->indexBuffer   = Graphics::planeIB;
      mRenderData->vertexBuffer  = Graphics::planeVB;
      mRenderData->shader        = Graphics::getDefaultShader("gui/world_text_vs.sc", "gui/world_text_fs.sc")->mProgram;
      mRenderData->state         = 0 | BGFX_STATE_RGB_WRITE 
                                     | BGFX_STATE_ALPHA_WRITE 
                                     | BGFX_STATE_DEPTH_TEST_LESS 
                                     | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE)
                                     | BGFX_STATE_BLEND_INDEPENDENT;
      mRenderData->stateRGBA     = 0 | BGFX_STATE_BLEND_FUNC_RT_1(BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_INV_SRC_ALPHA);

      refresh();
      setRendering(true);
      mRedrawText = true;
   }

   void TextComponent::onRemoveFromScene()
   {
      mRenderData->flags |= Rendering::RenderData::Deleted;
      mRenderData = NULL;
   }

   void TextComponent::preRender()
   {
      smCanRender = true;
   }

   void TextComponent::render()
   {
      if (mRedrawText)
         mRedrawText = !renderText(mTextureWidth, mTextureHeight, mText, mTextColor, mTextSize, mTextTexture);
   }

   void TextComponent::postRender()
   {

   }

   void TextComponent::setText(const char* text)
   {
      mText = StringTable->insert(text);
      mRedrawText = true;
   }

   void TextComponent::setTextColor(ColorF textColor)
   {
      mTextColor = textColor;
      mRedrawText = true;
   }

   void TextComponent::setTextSize(F32 textSize)
   {
      mTextSize = textSize;
      mRedrawText = true;
   }

   void TextComponent::setTextureWidth(F32 width)
   {
      mTextureWidth = width;
      initTexture();
      mRedrawText = true;
   }

   void TextComponent::setTextureHeight(F32 height)
   {
      mTextureHeight = height;
      initTexture();
      mRedrawText = true;
   }

   void TextComponent::refresh()
   {
      Parent::refresh();

      mBoundingBox.minExtents.set(-1.0f, -0.01f, -1.0f);
      mBoundingBox.maxExtents.set(1.0f, 0.01f, 1.0f);
      mBoundingBox.transform(mLocalTransformMatrix);

      // Sanity Checks.
      if ( mOwnerObject == NULL ) return;

      // Debug Render.
      if ( mRenderData )
      {
         // Base Component transform matrix is always slot 0 in the transform table.
         mRenderData->transformTable = &mTransformMatrix[0];
         mRenderData->transformCount = 1;

         // Setup Uniforms with Light Data
         mRenderData->uniforms.uniforms = &mUniforms;
         mUniforms.clear();

         // [U Scroll Speed, V Scroll Speed, Empty, Empty]
         mUniforms.push_back(Rendering::UniformData(Graphics::Shader::getUniformVec4("textParams")));
         Rendering::UniformData* uTextParams = &mUniforms.back();
         uTextParams->setValue(Point4F(mUScrollSpeed, mVScrollSpeed, 0.0f, 0.0f));

         mTextures.clear();
         mRenderData->textures = &mTextures;

         // Text Texture
         Rendering::TextureData textTex;
         textTex.uniform = Graphics::Shader::getTextureUniform(0);
         textTex.handle = mTextTexture;
         mTextures.push_back(textTex);
      }
   }

   // Static function.
   bool TextComponent::renderText(F32 width, F32 height, StringTableEntry text, ColorF textColor, F32 textSize, bgfx::TextureHandle targetTexture)
   {
      if (!smCanRender)
         return false;

      bgfx::FrameBufferHandle tempTextBuffer = bgfx::createFrameBuffer(width, height, bgfx::TextureFormat::BGRA8);
      if (!bgfx::isValid(tempTextBuffer))
         return false;

      Graphics::ViewTableEntry* tempView     = Graphics::getTemporaryView("TextTexture", 250);
      Graphics::ViewTableEntry* tempCopyView = Graphics::getTemporaryView("TextTextureCopy", 251);

      // GUI Orthographic Projection
      float ortho[16];
      bx::mtxOrtho(ortho, 0.0f, width, height, 0.0f, 0.0f, 1000.0f);

      bgfx::setViewFrameBuffer(tempView->id, tempTextBuffer);
      bgfx::setViewRect(tempView->id, 0, 0, width, height);
      bgfx::setViewTransform(tempView->id, NULL, ortho);
      bgfx::setViewClear(tempView->id
         , BGFX_CLEAR_COLOR
         , 0x00000000
         , 1.0f
         , 0
         );
      bgfx::touch(tempView->id);

      // Use NVG to render our text.
      NVGcontext* nvgContext = dglGetNVGContext();
      nvgViewId(nvgContext, tempView->id);
      nvgBeginFrame(nvgContext, width, height, 1.0f);
      nvgFontFace(nvgContext, "lucida console");
      nvgFontSize(nvgContext, textSize);
      nvgFillColor(nvgContext, nvgRGBA(textColor.red * 255, textColor.green * 255, textColor.blue * 255, textColor.alpha * 255));
      nvgTextAlign(nvgContext, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
      nvgText(nvgContext, width / 2.0f, height / 2.0f, text, NULL);
      nvgEndFrame(nvgContext);

      // Copy the data from NVG into text texture.
      bgfx::blit(tempCopyView->id, targetTexture, 0, 0, tempTextBuffer, 0);

      // Clean up
      bgfx::destroyFrameBuffer(tempTextBuffer);

      // Only do one text render per frame.
      smCanRender = false;
      return true;
   }
}
