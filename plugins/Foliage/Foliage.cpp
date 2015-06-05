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

#include <plugins/plugins_shared.h>
#include "Foliage.h"
#include <sim/simObject.h>
#include <3d/rendering/common.h>
#include <graphics/core.h>
#include <bx/fpumath.h>

F32                              mTransformMatrix[16];
bgfx::ProgramHandle              mShader;
Rendering::RenderData*           mRenderData = NULL;
bgfx::TextureHandle              mTexture = BGFX_INVALID_HANDLE;
Vector<Rendering::InstanceData>  mInstanceData;
Vector<Rendering::TextureData>   mTextureData;

using namespace Plugins;

// Called when the plugin is loaded.
void create(PluginLink _link)
{
   bx::mtxIdentity(mTransformMatrix);
   Link.Con.addCommand("Foliage", "load", loadModel, "", 2, 2);
}

void destroy()
{

}

void loadModel(SimObject *obj, S32 argc, const char *argv[])
{
   // Load skybox texture.
   MeshAsset* mesh = Link.Scene.getMeshAsset(argv[1]);
   if ( mesh )
   {
      Link.Con.printf("Found Mesh Successfully!");

      // Load Shader
      Graphics::ShaderAsset* particleShaderAsset = Plugins::Link.Graphics.getShaderAsset("Foliage:grassShader");
      if ( particleShaderAsset )
         mShader = particleShaderAsset->getProgram();

      // Load Texture
      TextureObject* texture_obj = Plugins::Link.Graphics.loadTexture("grass01_diffuse.png", TextureHandle::BitmapKeepTexture, false, false, false);
      if ( texture_obj )
         mTexture = texture_obj->getBGFXTexture();

      if ( mRenderData == NULL )
         mRenderData = Plugins::Link.Rendering.createRenderData();

      mRenderData->indexBuffer = mesh->getIndexBuffer(0);
      mRenderData->vertexBuffer = mesh->getVertexBuffer(0);

      // Render in Forward (for now) with our custom terrain shader.
      mRenderData->shader = mShader;
      mRenderData->view = Plugins::Link.Graphics.getView("TransparencyBuffer", "", false);
      mRenderData->state = 0
            | BGFX_STATE_RGB_WRITE
            | BGFX_STATE_ALPHA_WRITE
            | BGFX_STATE_DEPTH_TEST_LESS
            | BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_ONE, BGFX_STATE_BLEND_ONE)
				| BGFX_STATE_BLEND_INDEPENDENT;
      mRenderData->stateRGBA = 0
            | BGFX_STATE_BLEND_FUNC_RT_1(BGFX_STATE_BLEND_ZERO, BGFX_STATE_BLEND_INV_SRC_ALPHA);

      // Transform of emitter.
      mRenderData->transformTable = &mTransformMatrix[0];
      mRenderData->transformCount = 1;

      // Instances
      mInstanceData.clear();
      for(S32 x = -100; x < 100; ++x)
      {
         for(S32 y = -100; y < 100; ++y)
         {
            Rendering::InstanceData grassInstance;

            F32 instScale = mRandF(0.5f, 1.5f);
            F32 instRot = mRandF(0.0f, 3.14f);

            F32 instMat[16];
            bx::mtxSRT(instMat, instScale, instScale, instScale, 1.57f, instRot, 0.0f, x * 20.0f, 0.0f, y * 20.0f);

            grassInstance.i_data0.set(instMat[0],  instMat[1],  instMat[2],  instMat[3]);
            grassInstance.i_data1.set(instMat[4],  instMat[5],  instMat[6],  instMat[7]);
            grassInstance.i_data2.set(instMat[8],  instMat[9],  instMat[10], instMat[11]);
            grassInstance.i_data3.set(instMat[12], instMat[13], instMat[14], instMat[15]);

            mInstanceData.push_back(grassInstance);
         }
      }
      mRenderData->instances = &mInstanceData;

      // Textures
      mTextureData.clear();
      mRenderData->textures = &mTextureData;
      Rendering::TextureData* texture = mRenderData->addTexture();
      texture->handle = mTexture;
      texture->uniform = Plugins::Link.Graphics.getTextureUniform(0);
   }
}