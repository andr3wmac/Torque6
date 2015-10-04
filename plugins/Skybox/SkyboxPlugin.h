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
#include <3d/rendering/renderable.h>
#endif

PLUGIN_FUNC(create)

class SimpleSkybox : public Rendering::RenderFeature
{
   private:
      typedef Rendering::RenderFeature Parent;

   protected:
      StringTableEntry           mTexturePath;
      bool                       mEnabled;
      bgfx::TextureHandle        mTexture;
      bgfx::ProgramHandle        mShader;
      bgfx::UniformHandle        mMatrixUniform;
      Graphics::ViewTableEntry*  mView;

   public:
      SimpleSkybox();

      virtual void onActivate();
      virtual void onDeactivate();
      virtual void preRender();
      virtual void render();
      virtual void postRender();

      void loadTexture(StringTableEntry path);

      static void initPersistFields();
      static bool setTexture(void* obj, const char* data) { static_cast<SimpleSkybox*>(obj)->loadTexture(Plugins::Link.StringTableLink->insert(data)); return false; }

      DECLARE_PLUGIN_CONOBJECT(SimpleSkybox);
};