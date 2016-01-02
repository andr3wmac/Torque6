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

#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

#ifndef _RENDERABLE_H_
#include <rendering/renderable.h>
#endif

class ScatterSky : public Rendering::RenderFeature
{
   private:
      typedef Rendering::RenderFeature Parent;

   protected:
      bool                       mEnabled;
      bgfx::TextureHandle        mTexture;
      bgfx::ProgramHandle        mShader;
      bgfx::UniformHandle        mMatrixUniform;
      Graphics::ViewTableEntry*  mView;
		bool								mGenerateSkyCube;
		bool								mSkyCubeReady;
		bgfx::ProgramHandle        mGenerateSkyCubeShader;
      bgfx::UniformHandle        mCubeParamsUniform;
      bgfx::UniformHandle        mSkyParams1Uniform;
      bgfx::UniformHandle        mSkyParams2Uniform;
      bgfx::UniformHandle        mSkyParams3Uniform;
      bgfx::UniformHandle        mSkyParams4Uniform;
      bgfx::UniformHandle        mSkyParams5Uniform;

      F32                        mIntensity;
      F32                        mSunBrightness;
      F32                        mSurfaceHeight;
      F32                        mScatterStrength;
      F32                        mMieBrightness;
      F32                        mMieDistribution;
      F32                        mMieCollectionPower;
      F32                        mMieStrength;
      F32                        mRayleighBrightness;
      F32                        mRayleighCollectionPower;
      F32                        mRayleighStrength;
      F32                        mStepCount;
      ColorF                     mAirColor;

      Graphics::ViewTableEntry*  mTempSkyCubeCopyView;
      Graphics::ViewTableEntry*  mTempSkyCubeView[6];
      bgfx::FrameBufferHandle    mTempSkyCubeBuffers[6];
      bgfx::TextureHandle        mTempSkyCubeTextures[6];

   public:
		ScatterSky();

      virtual void onActivate();
      virtual void onDeactivate();
      virtual void refresh();
      virtual void preRender();
      virtual void render();
      virtual void postRender();
		
      void generateSkyCubeBegin();
		void generateSkyCube();
      void generateSkyCubeEnd();

      static void initPersistFields();

      DECLARE_PLUGIN_CONOBJECT(ScatterSky);
};