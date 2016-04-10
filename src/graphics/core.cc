//-----------------------------------------------------------------------------
// Copyright (c) 2016 Andrew Mac
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

#include "graphics/core.h"
#include "graphics/dgl.h"
#include "lighting/lighting.h"
#include "rendering/rendering.h"
#include "scene/scene.h"
#include "sysgui/sysgui.h"

#include "core_Binding.h"

#include <bgfx/bgfx.h>
#include <debugdraw/debugdraw.h>
#include <imgui/imgui.h>
#include <nanovg/nanovg.h>

#include "bx/crtimpl.h"
#include "aviwriter.h"

namespace Graphics
{
   static bool sCaptureEnabled = false;

   struct BgfxCallback : public bgfx::CallbackI
   {
      virtual ~BgfxCallback()
      {
         //
      }

      virtual void fatal(bgfx::Fatal::Enum _code, const char* _str) BX_OVERRIDE
      {
         // Something unexpected happened, inform user and bail out.
         Con::errorf("Fatal error: 0x%08x: %s", _code, _str);

         // Must terminate, continuing will cause crash anyway.
         abort();
      }

      virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) BX_OVERRIDE
      {
         char buf[8192];
         dVsprintf(buf, sizeof(buf), _format, _argList);
         buf[dStrlen(buf) - 2] = '\0';
         Con::printf("%s", buf);
      }

      virtual uint32_t cacheReadSize(uint64_t _id) BX_OVERRIDE { return 0; }
      virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) BX_OVERRIDE { return false; }
      virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) BX_OVERRIDE { }

      virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t /*_size*/, bool _yflip) BX_OVERRIDE
      {
         GBitmap screenshot;
         screenshot.allocateBitmap(_width, _height);

         uint8_t* data = (uint8_t*)_data;
         for (U32 y = 0; y < _height; ++y)
         {
            for (U32 x = 0; x < _width; ++x)
            {
               // BGRA -> RGBA
               ColorI color(*(data + 2), *(data + 1), *data, *(data + 3));
               screenshot.setColor(x, y, color);
               data += 4;
            }
         }
         
         Platform::LocalTime lt;
         Platform::getLocalTime(lt);
         char nameBuffer[128];
         dSprintf(nameBuffer, sizeof(nameBuffer), "%d-%d-%d_%02d-%02d-%02d.png",
            lt.month + 1,
            lt.monthday,
            lt.year + 1900,
            lt.hour,
            lt.min,
            lt.sec);

         FileStream fws;
         if (fws.open(nameBuffer, FileStream::Write))
         {
            screenshot.writePNG(fws);
            fws.close();
         }
      }

      virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t /*_pitch*/, bgfx::TextureFormat::Enum /*_format*/, bool _yflip) BX_OVERRIDE
      {
         m_fileWriter = new bx::CrtFileWriter();
         m_aviWriter = new AviWriter(m_fileWriter);

         Platform::LocalTime lt;
         Platform::getLocalTime(lt);
         char nameBuffer[128];
         dSprintf(nameBuffer, sizeof(nameBuffer), "%d-%d-%d_%02d-%02d-%02d.avi",
            lt.month + 1,
            lt.monthday,
            lt.year + 1900,
            lt.hour,
            lt.min,
            lt.sec);

         if (!m_aviWriter->open(nameBuffer, _width, _height, 30, _yflip))
         {
            delete m_aviWriter;
            m_aviWriter = NULL;
         }
      }

      virtual void captureEnd() BX_OVERRIDE
      {
         if (NULL != m_aviWriter)
         {
            m_aviWriter->close();
            delete m_aviWriter;
            m_aviWriter = NULL;
         }

         if (NULL != m_fileWriter)
         {
            m_fileWriter->close();
            delete m_fileWriter;
            m_fileWriter = NULL;
         }
      }

      virtual void captureFrame(const void* _data, uint32_t /*_size*/) BX_OVERRIDE
      {
         if (NULL != m_aviWriter)
         {
            m_aviWriter->frame(_data);
         }
      }

      AviWriter*        m_aviWriter;
      bx::FileWriterI*  m_fileWriter;
   };
   static BgfxCallback sBGFXCallback;

   void init()
   {
      // Renderer Type Selection
      const char* renderer = Con::getVariable("pref::Video::Renderer");
      if (dStrcmp(renderer, "OpenGL") == 0)
         bgfx::init(bgfx::RendererType::OpenGL, 0, 0, &sBGFXCallback);
      else if (dStrcmp(renderer, "OpenGLES") == 0)
         bgfx::init(bgfx::RendererType::OpenGLES, 0, 0, &sBGFXCallback);
      else if (dStrcmp(renderer, "Direct3D9") == 0)
         bgfx::init(bgfx::RendererType::Direct3D9, 0, 0, &sBGFXCallback);
      else if (dStrcmp(renderer, "Direct3D11") == 0)
         bgfx::init(bgfx::RendererType::Direct3D11, 0, 0, &sBGFXCallback);
      else if (dStrcmp(renderer, "Direct3D12") == 0)
         bgfx::init(bgfx::RendererType::Direct3D12, 0, 0, &sBGFXCallback);
      else if (dStrcmp(renderer, "Metal") == 0)
         bgfx::init(bgfx::RendererType::Metal, 0, 0, &sBGFXCallback);
      else if (dStrcmp(renderer, "Vulkan") == 0)
         bgfx::init(bgfx::RendererType::Vulkan, 0, 0, &sBGFXCallback);
      else
         bgfx::init(bgfx::RendererType::Direct3D11, 0, 0, &sBGFXCallback); // Auto-select.

      reset();

#ifdef TORQUE_DEBUG
      bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif

      ddInit();
      dglInit();
      initUniforms();
      initUtilities();
      Lighting::init();
      Rendering::init();
      Scene::init();
      SysGUI::init();
   }

   void destroy()
   {
      SysGUI::destroy();
      Scene::destroy();
      Rendering::destroy();
      Lighting::destroy();
      destroyUniforms();
      destroyUtilities();
      dglDestroy();
      ddShutdown();

      // Shutdown bgfx.
      bgfx::shutdown();
   }

   void reset()
   {
      bool vsyncEnabled = Con::getBoolVariable("pref::Video::VSync", true);
      vsyncEnabled = false;

      U32 resetFlags = 0 
         | (vsyncEnabled ? BGFX_RESET_VSYNC : 0) 
         | (sCaptureEnabled ? BGFX_RESET_CAPTURE : 0);

      bgfx::reset(Rendering::windowWidth, Rendering::windowHeight, resetFlags);
   }

   void captureBegin()
   {
      sCaptureEnabled = true;
      reset();
   }

   void captureEnd()
   {
      sCaptureEnabled = false;
      reset();
   }

   void saveScreenshot()
   {
      bgfx::saveScreenShot("screenshot.bmp");
   }
}