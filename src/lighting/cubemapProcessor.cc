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

#include "cubemapProcessor.h"
#include "console/consoleInternal.h"
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "graphics/core.h"
#include "scene/scene.h"
#include "rendering/rendering.h"
#include "rendering/renderCamera.h"
#include "scene/components/cameraComponent.h"

#include <bx/fpumath.h>

namespace Lighting
{
   // ----------------------------------------------
   // Utility functions.
   // ----------------------------------------------

   F32 radicalInverse_VdC(S32 bits) {
      bits = (bits << 16u) | (bits >> 16u);
      bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
      bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
      bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
      bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
      return F32(bits) * 2.3283064365386963e-10f; // / 0x100000000
   }

   // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
   Point2F Hammersley(S32 i, S32 N) {
      return Point2F(F32(i) / F32(N), radicalInverse_VdC(i));
   }

   Point3F ImportanceSampleGGX(Point2F Xi, F32 Roughness, Point3F N)
   {
      F32 a = Roughness * Roughness; // DISNEY'S ROUGHNESS [see Burley'12 siggraph]

      // Compute distribution direction
      F32 Phi = 2.0f * M_PI_F * Xi.x;
      F32 CosTheta = mSqrt((1.0f - Xi.y) / (1.0f + (a*a - 1.0f) * Xi.y));
      F32 SinTheta = mSqrt((F32)abs(1.0f - CosTheta * CosTheta));

      // Convert to spherical direction
      Point3F H;
      H.x = SinTheta * mCos(Phi);
      H.y = SinTheta * mSin(Phi);
      H.z = CosTheta;

      Point3F UpVector = abs(N.z) < 0.999 ? Point3F(0.0, 0.0, 1.0) : Point3F(1.0, 0.0, 0.0);
      Point3F TangentX = mCross(UpVector, N);
      TangentX.normalize();
      Point3F TangentY = mCross(N, TangentX);

      // Tangent to world space
      return TangentX * H.x + TangentY * H.y + N * H.z;
   }

   Point3F texelCoordToVec(Point2F uv, U32 faceID)
   {
      Point3F faceUvVectors[6][3];

      // +x
      faceUvVectors[0][0] = Point3F(0.0, 0.0, -1.0); // u -> -z
      faceUvVectors[0][1] = Point3F(0.0, -1.0, 0.0); // v -> -y
      faceUvVectors[0][2] = Point3F(1.0, 0.0, 0.0);  // +x face

      // -x
      faceUvVectors[1][0] = Point3F(0.0, 0.0, 1.0);  // u -> +z
      faceUvVectors[1][1] = Point3F(0.0, -1.0, 0.0); // v -> -y
      faceUvVectors[1][2] = Point3F(-1.0, 0.0, 0.0); // -x face

      // +y
      faceUvVectors[2][0] = Point3F(1.0, 0.0, 0.0);  // u -> +x
      faceUvVectors[2][1] = Point3F(0.0, 0.0, 1.0);  // v -> +z
      faceUvVectors[2][2] = Point3F(0.0, 1.0, 0.0);  // +y face

      // -y
      faceUvVectors[3][0] = Point3F(1.0, 0.0, 0.0);  // u -> +x
      faceUvVectors[3][1] = Point3F(0.0, 0.0, -1.0); // v -> -z
      faceUvVectors[3][2] = Point3F(0.0, -1.0, 0.0); // -y face

      // +z
      faceUvVectors[4][0] = Point3F(1.0, 0.0, 0.0);  // u -> +x
      faceUvVectors[4][1] = Point3F(0.0, -1.0, 0.0); // v -> -y
      faceUvVectors[4][2] = Point3F(0.0, 0.0, 1.0);  // +z face

      // -z
      faceUvVectors[5][0] = Point3F(-1.0, 0.0, 0.0); // u -> -x
      faceUvVectors[5][1] = Point3F(0.0, -1.0, 0.0); // v -> -y
      faceUvVectors[5][2] = Point3F(0.0, 0.0, -1.0); // -z face

      // out = u * s_faceUv[0] + v * s_faceUv[1] + s_faceUv[2].
      Point3F result = (faceUvVectors[faceID][0] * uv.x) + (faceUvVectors[faceID][1] * uv.y) + faceUvVectors[faceID][2];
      result.normalize();
      return result;
   }

   // ----------------------------------------------
   // GPU Cubemap Processor
   // ----------------------------------------------
   
   GPUCubemapProcessor::GPUCubemapProcessor()
   {
      mSourceCubemapUniform      = Graphics::Shader::getUniform("u_skyCube", bgfx::UniformType::Int1);
      mGenerateParamsUniform     = Graphics::Shader::getUniform("u_generateParams", bgfx::UniformType::Vec4);
      mGenerateRadianceShader    = Graphics::getDefaultShader("lighting/generateRad_vs.tsh",   "lighting/generateRad_fs.tsh");
      mGenerateIrradianceShader  = Graphics::getDefaultShader("lighting/generateIrr_vs.tsh",   "lighting/generateIrr_fs.tsh");
      mGenerateBRDFShader        = Graphics::getDefaultShader("lighting/generateBRDF_vs.tsh",  "lighting/generateBRDF_fs.tsh");

      mStage               = 0;
      mHammersleyLUT.idx   = bgfx::invalidHandle;
   }

   GPUCubemapProcessor::~GPUCubemapProcessor()
   {
      if (!bgfx::isValid(mHammersleyLUT))
      {
         bgfx::destroyTexture(mHammersleyLUT);
      }
   }
   
   void GPUCubemapProcessor::init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
      bgfx::TextureHandle radianceCubemap, U32 radianceSize,
      bgfx::TextureHandle irradianceCubemap, U32 irradianceSize,
      bgfx::TextureHandle brdfTexture)
   {
      CubemapProcessor::init(sourceCubemap, sourceSize, radianceCubemap, radianceSize, irradianceCubemap, irradianceSize, brdfTexture);

      // Generate temp lookup table of Hammersley points
      // Note: This is done for compatibility reasons. 
      //       Older openGL and DX9 can't do Hammersley in a shader (no bitwise operations)
      if (!bgfx::isValid(mHammersleyLUT))
      {
         const bgfx::Memory* mem = bgfx::alloc(1024);
         for (U32 i = 0, n = 0; i < 512; ++i, n += 2)
         {
            Point2F pt = Hammersley(i, 512);
            mem->data[n] = (U8)(pt.x * 255.0f);
            mem->data[n + 1] = (U8)(pt.y * 255.0f);
         }
         mHammersleyLUT = bgfx::createTexture2D(512, 1, 1, bgfx::TextureFormat::RG8, BGFX_TEXTURE_MAG_POINT | BGFX_TEXTURE_MIN_POINT | BGFX_TEXTURE_MIP_POINT, mem);
      }

      mStage = 1;
      mGenerateRadianceMip = 0;
   }

   void GPUCubemapProcessor::process()
   {
      if (mStage == 1)
      {
         generateRadianceCubeTexture();
         return;
      }

      if (mStage == 2)
      {
         generateIrradianceCubeTexture();
         return;
      }

      if (mStage == 3)
      {
         generateBRDFTexture();
         return;
      }
   }

   bool GPUCubemapProcessor::isFinished()
   {
      return (mStage > 3);
   }

   void GPUCubemapProcessor::generateRadianceCubeTexture()
   {
      // Initialize temporary buffers to use to generate radiance cube.
      Graphics::ViewTableEntry*  tempRadianceView[6];
      bgfx::FrameBufferHandle    tempRadianceBuffers[6];

      U32 radianceSize = mRadianceSize;
      for (U8 side = 0; side < 6; ++side)
      {
         char viewName[64];
         dSprintf(viewName, 64, "GenerateRadianceCubeMip%dSide%d", mGenerateRadianceMip, side);

         bgfx::Attachment frameBufferAttachment;
         frameBufferAttachment.handle = mRadianceCubemap;
         frameBufferAttachment.layer = side;
         frameBufferAttachment.mip = mGenerateRadianceMip;

         tempRadianceView[side]    = Graphics::getTemporaryView(StringTable->insert(viewName), 100 + (0 * 6));
         tempRadianceBuffers[side] = bgfx::createFrameBuffer(1, &frameBufferAttachment);
      }

      // Process
      radianceSize = mRadianceSize >> mGenerateRadianceMip;
      for (U32 side = 0; side < 6; ++side)
      {
         // Side, Mip, Roughness
         F32 generateParams[4] = { (F32)side, (F32)mGenerateRadianceMip, (F32)mGenerateRadianceMip / 6.0f, 0.0f };
         bgfx::setUniform(mGenerateParamsUniform, generateParams);

         // This projection matrix is used because its a full screen quad.
         F32 proj[16];
         bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
         bgfx::setViewTransform(tempRadianceView[side]->id, NULL, proj);
         bgfx::setViewRect(tempRadianceView[side]->id, 0, 0, radianceSize, radianceSize);
         bgfx::setViewFrameBuffer(tempRadianceView[side]->id, tempRadianceBuffers[side]);

         U32 tileCount = getMax((radianceSize / 128), (U32)1);
         U32 tileSize  = getMin(radianceSize, (U32)128);
         for (U32 y = 0; y < tileCount; ++y)
         {
            for (U32 x = 0; x < tileCount; ++x)
            {
               bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mHammersleyLUT);
               bgfx::setTexture(1, mSourceCubemapUniform, mSourceCubemap);

               bgfx::setState(0
                  | BGFX_STATE_RGB_WRITE
                  | BGFX_STATE_ALPHA_WRITE
                  );

               screenSpaceTile((F32)(x * tileSize), (F32)(y * tileSize), (F32)tileSize, (F32)tileSize, (F32)radianceSize, (F32)radianceSize);
               bgfx::submit(tempRadianceView[side]->id, mGenerateRadianceShader->mProgram);
            }
         }
      }

      mGenerateRadianceMip++;
      if (mGenerateRadianceMip > 5)
         mStage++;

      // Destroy temporary buffers.
      for (U32 side = 0; side < 6; ++side)
      {
         if (bgfx::isValid(tempRadianceBuffers[side]))
            bgfx::destroyFrameBuffer(tempRadianceBuffers[side]);
      }
   }

   void GPUCubemapProcessor::generateIrradianceCubeTexture()
   {
      // Initialize temporary buffers to use to generate irradiance cube.
      Graphics::ViewTableEntry*  tempIrradianceView[6];
      bgfx::FrameBufferHandle    tempIrradianceBuffers[6];
      for (U8 side = 0; side < 6; ++side)
      {
         char viewName[64];
         dSprintf(viewName, 64, "GenerateIrradianceCubeSide%d", side);

         bgfx::Attachment frameBufferAttachment;
         frameBufferAttachment.handle = mIrradianceCubemap;
         frameBufferAttachment.layer = side;
         frameBufferAttachment.mip = 0;

         tempIrradianceView[side]    = Graphics::getTemporaryView(StringTable->insert(viewName), 200);
         tempIrradianceBuffers[side] = bgfx::createFrameBuffer(1, &frameBufferAttachment);
      }

      // Process
      for (U32 side = 0; side < 6; ++side)
      {
         F32 generateParams[4] = { (F32)side, 0.0f, 0.0f, 0.0f };
         bgfx::setUniform(mGenerateParamsUniform, generateParams);

         // This projection matrix is used because its a full screen quad.
         F32 proj[16];
         bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
         bgfx::setViewTransform(tempIrradianceView[side]->id, NULL, proj);
         bgfx::setViewRect(tempIrradianceView[side]->id, 0, 0, 128, 128);
         bgfx::setViewFrameBuffer(tempIrradianceView[side]->id, tempIrradianceBuffers[side]);

         // Setup textures
         bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mHammersleyLUT);
         bgfx::setTexture(1, mSourceCubemapUniform, mSourceCubemap);

         bgfx::setState(0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            );

         fullScreenQuad(128, 128);
         bgfx::submit(tempIrradianceView[side]->id, mGenerateIrradianceShader->mProgram);
      }

      mStage++;

      // Destroy temporary buffers.
      for (U32 side = 0; side < 6; ++side)
      {
         if (bgfx::isValid(tempIrradianceBuffers[side]))
            bgfx::destroyFrameBuffer(tempIrradianceBuffers[side]);
      }
   }

   void GPUCubemapProcessor::generateBRDFTexture()
   {
      // Initialize temporary buffers to use to generate BRDF texture.
      Graphics::ViewTableEntry*  tempBRDFView;
      bgfx::FrameBufferHandle    tempBRDFBuffer;

      tempBRDFView = Graphics::getTemporaryView(StringTable->insert("GenerateBRDF"), 50);
      tempBRDFBuffer = bgfx::createFrameBuffer(1, &mBRDFTexture);

      // Process
      F32 proj[16];
      bx::mtxOrtho(proj, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 100.0f);
      bgfx::setViewTransform(tempBRDFView->id, NULL, proj);
      bgfx::setViewRect(tempBRDFView->id, 0, 0, mRadianceSize, mRadianceSize);
      bgfx::setViewFrameBuffer(tempBRDFView->id, tempBRDFBuffer);

      U32 tileCount = getMax((mRadianceSize / 128), (U32)1);
      U32 tileSize = getMin(mRadianceSize, (U32)128);
      for (U32 y = 0; y < tileCount; ++y)
      {
         for (U32 x = 0; x < tileCount; ++x)
         {
            bgfx::setTexture(0, Graphics::Shader::getTextureUniform(0), mHammersleyLUT);
            bgfx::setState(0 | BGFX_STATE_RGB_WRITE);
            screenSpaceTile((F32)(x * tileSize), (F32)(y * tileSize), (F32)tileSize, (F32)tileSize, (F32)mRadianceSize, (F32)mRadianceSize);
            bgfx::submit(tempBRDFView->id, mGenerateBRDFShader->mProgram);
         }
      }

      mStage++;

      // Destroy temporary buffers.
      if (bgfx::isValid(tempBRDFBuffer))
         bgfx::destroyFrameBuffer(tempBRDFBuffer);
   }

   // ----------------------------------------------
   // CPU Cubemap Processor - Currently incomplete.
   // ----------------------------------------------

   CPUCubemapProcessor::CPUCubemapProcessor()
   {
      mStage            = 0;
      mSourceBuffer     = NULL;
      mRadianceBuffer   = NULL;
      mIrradianceBuffer = NULL;
   }

   CPUCubemapProcessor::~CPUCubemapProcessor()
   {
      if (mSourceBuffer != NULL)
         SAFE_DELETE_ARRAY(mSourceBuffer);

      if (mRadianceBuffer != NULL)
         SAFE_DELETE_ARRAY(mRadianceBuffer);

      if (mIrradianceBuffer != NULL)
         SAFE_DELETE_ARRAY(mIrradianceBuffer);
   }

   void CPUCubemapProcessor::init(bgfx::TextureHandle sourceCubemap, U32 sourceSize,
      bgfx::TextureHandle radianceCubemap, U32 radianceSize,
      bgfx::TextureHandle irradianceCubemap, U32 irradianceSize,
      bgfx::TextureHandle brdfTexture)
   {
      CubemapProcessor::init(sourceCubemap, sourceSize, radianceCubemap, radianceSize, irradianceCubemap, irradianceSize, brdfTexture);

      if (mSourceBuffer != NULL)
         SAFE_DELETE_ARRAY(mSourceBuffer);

      mSourceBuffer     = new U32[mSourceSize * mSourceSize];
      mRadianceBuffer   = new U32[mRadianceSize * mRadianceSize];
      mIrradianceBuffer = new U32[mIrradianceSize * mIrradianceSize];

      //bgfx::readTexture(mSourceCubemap, 5, mSourceBuffer);
      mStage = 1;
   }

   void CPUCubemapProcessor::process()
   {
      if (mStage == 1)
      {
         mStage = 2;
         return;
      }

      if (mStage == 2)
      {
         // Texture has been read back by now.
         for (U32 y = 0; y < mIrradianceSize; ++y)
         {
            for (U32 x = 0; x < mIrradianceSize; ++x)
            {
               Point3F sum = Point3F::Zero;
               Point3F N = texelCoordToVec(Point2F(x / mIrradianceSize, y / mIrradianceSize), 5);

               for (U32 n = 0; n < 512; n++)
               {
                  Point2F pt = Hammersley(n, 512);
                  Point3F H = ImportanceSampleGGX(pt, 1.0f, N);
                  Point3F V = N;
                  Point3F L = 2.0f * mDot(V, H) * H - V;

                  F32 ndotl = getMax(0.0f, mDot(N, L));

                  // Sample
                  U32 rawColor      = mSourceBuffer[(y * mIrradianceSize) + x];
                  Point3F sample    = Point3F((F32)BGFXCOLOR_GET_R(rawColor) / 255.0f, (F32)BGFXCOLOR_GET_R(rawColor) / 255.0f, (F32)BGFXCOLOR_GET_R(rawColor) / 255.0f);
                  sample            *= ndotl;
                  sum               += sample;
               }

               sum /= 512.0f;
               mIrradianceBuffer[(y * mIrradianceSize) + x] = BGFXCOLOR_RGBA((U8)(sum.x * 255.0f), (U8)(sum.y * 255.0f), (U8)(sum.z * 255.0f), 255);
            }
         }

         const bgfx::Memory* mem = bgfx::makeRef(mIrradianceBuffer, sizeof(U32) * mIrradianceSize * mIrradianceSize);
         bgfx::updateTextureCube(mIrradianceCubemap, 0, 0, 0, 0, mIrradianceSize, mIrradianceSize, mem);
         mStage = 3;
      }
   }

   bool CPUCubemapProcessor::isFinished()
   {
      return false;
   }
}
