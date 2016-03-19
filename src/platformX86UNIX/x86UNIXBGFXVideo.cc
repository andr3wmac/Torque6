//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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



#include "console/console.h"
#include "platform/event.h"
#include "platform/types.h"
#include "game/gameInterface.h"

#include "platformX86UNIX/platformX86UNIX.h"
#include "platformX86UNIX/x86UNIXBGFXVideo.h"
#include "platformX86UNIX/x86UNIXState.h"

#include <bx/timer.h>
#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>
#include <nanovg/nanovg.h>
#include <imgui/imgui.h>
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "rendering/rendering.h"
#include "sysgui/sysgui.h"
#include "plugins/plugins.h"
#include "scene/scene.h"

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_version.h>

bool bgfxInitialized = false;

//------------------------------------------------------------------------------
BGFXDevice::BGFXDevice()
{
   initDevice();
}

//------------------------------------------------------------------------------
BGFXDevice::~BGFXDevice()
{
}

//------------------------------------------------------------------------------
void BGFXDevice::addResolution(S32 width, S32 height, bool check)
{
   Point2I desktopSize = x86UNIXState->getDesktopSize();
   U32 desktopBpp = x86UNIXState->getDesktopBpp();

   // don't allow any resolution under this size
   if (width < 640 || height < 480)
      return;

   if (check)
   {
      // don't allow resolutions that exceed the current desktop size
      if (width > desktopSize.x || height > desktopSize.y)
         return;
   }

   //if (smCanSwitchBitDepth)
   //{
   //   // add both 16 and 32 bit resolutions
   //   mResolutionList.push_back(Resolution(width, height, 16));
    //  mResolutionList.push_back(Resolution(width, height, 32));
   //}
   //else
   //{
      // add just the desktop resolution
      mResolutionList.push_back(Resolution(width, height, desktopBpp));
   //}//
}

//------------------------------------------------------------------------------
void BGFXDevice::initDevice()
{
   mDeviceName = "BGFX";
   mFullScreenOnly = false;
}

//------------------------------------------------------------------------------
void BGFXDevice::loadResolutions()
{
   mResolutionList.clear();

   // X cannot switch bit depths on the fly.  In case this feature is
   // implemented someday, calling this function will let you take
   // advantage of it
   //if (Con::getBoolVariable("$pref::Unix::CanSwitchBitDepth"))
   //   smCanSwitchBitDepth = true;

   // add some default resolutions
   addResolution(640, 480);
   addResolution(800, 600);
   addResolution(1024, 768);
   addResolution(1152, 864);
   addResolution(1280, 1024);
   addResolution(1600, 1200);

   // specifying full screen should give us the resolutions that the
   // X server allows
   SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN);
   if (modes &&
      (modes != (SDL_Rect **)-1))
   {
      for (int i = 0; modes[i] != NULL; ++i)
      {
         // do we already have this mode?
         bool found = false;
         for (Vector<Resolution>::iterator iter = mResolutionList.begin();
              iter != mResolutionList.end();
              ++iter)
         {
            if (iter->w == modes[i]->w && iter->h == modes[i]->h)
            {
               found = true;
               break;
            }
         }
         if (!found)
            // don't check these resolutions because they should be OK
            // (and checking might drop resolutions that are higher than the
            // current desktop bpp)
            addResolution(modes[i]->w, modes[i]->h, false);
      }
   }
}

//------------------------------------------------------------------------------
bool BGFXDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   if (!setScreenMode(width, height, bpp, fullScreen))
   {
      Con::printf("Unable to set screen mode.");
      return false;
   }

   Con::setVariable( "$pref::Video::displayDevice", mDeviceName );

   return true;
}


//------------------------------------------------------------------------------
void BGFXDevice::shutdown()
{
   Plugins::destroy();
   Physics::destroy();
   Graphics::destroy();

   // Shutdown bgfx.
   bgfx::shutdown();
   bgfxInitialized = false;
}

//------------------------------------------------------------------------------
bool BGFXDevice::setScreenMode( U32 width, U32 height, U32 bpp,
   bool fullScreen, bool forceIt, bool repaint )
{
   // load resolutions, this is done lazily so that we can check the setting
   // of smCanSwitchBitDepth, which may be overridden by console
   if (mResolutionList.size()==0)
      loadResolutions();

   if (mResolutionList.size()==0)
   {
      Con::printf("No resolutions available!");
      return false;
   }

   if (bpp == 0)
   {
      // bpp comes in as "0" when it is set to "Default"
      bpp = x86UNIXState->getDesktopBpp();
   }

   if (height == 0 || width == 0)
   {
      // paranoia check.  set it to the default to prevent crashing
      width = 800;
      height = 600;
   }

   U32 desktopDepth = x86UNIXState->getDesktopBpp();
   // if we can't switch bit depths and the requested bpp is not equal to
   // the desktop bpp, set bpp to the desktop bpp
   bpp = desktopDepth;

   bool IsInList = false;

   Resolution NewResolution( width, height, bpp );

   // See if the desired resolution is in the list
   if ( mResolutionList.size() )
   {
      for ( int i = 0; i < mResolutionList.size(); i++ )
      {
         if ( width == mResolutionList[i].w
              && height == mResolutionList[i].h
              && bpp == mResolutionList[i].bpp )
         {
            IsInList = true;
            break;
         }
      }

      if ( !IsInList )
      {
         Con::printf( "Selected resolution not available: %d %d %d",
            width, height, bpp);
         return false;
      }
   }
   else
   {
      AssertFatal( false, "No resolution list found!!" );
   }

   // Here if we found a matching resolution in the list

   bool needResurrect = false;
   if (x86UNIXState->windowCreated())
   {
      Con::printf( "Killing the texture manager..." );
      Game->textureKill();
      needResurrect = true;
   }

   // set the new video mode
   if (SDL_SetVideoMode(width, height, bpp, 0) == NULL)
   {
      Con::printf("Unable to set SDL Video Mode: %s", SDL_GetError());
      return false;
   }

   if ( needResurrect )
   {
      // Reload the textures:
      Con::printf( "Resurrecting the texture manager..." );
      Game->textureResurrect();
   }

   // reset the window in platform state
   SDL_SysWMinfo sysinfo;
   SDL_VERSION(&sysinfo.version);
   if (SDL_GetWMInfo(&sysinfo) == 0)
   {
      Con::printf("Unable to set SDL Video Mode: %s", SDL_GetError());
      return false;
   }
   x86UNIXState->setWindow(sysinfo.info.x11.window);

   // set various other parameters
   x86UNIXState->setWindowCreated(true);
   smCurrentRes = NewResolution;
   Platform::setWindowSize ( width, height );
   smIsFullScreen = fullScreen;
   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );
   char tempBuf[15];
   dSprintf( tempBuf, sizeof( tempBuf ), "%d %d %d",
      smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp );
   Con::setVariable( "$pref::Video::resolution", tempBuf );

   // post a TORQUE_SETVIDEOMODE user event
   SDL_Event event;
   event.type = SDL_USEREVENT;
   event.user.code = TORQUE_SETVIDEOMODE;
   event.user.data1 = NULL;
   event.user.data2 = NULL;
   SDL_PushEvent(&event);

   // reset the caption
   SDL_WM_SetCaption(x86UNIXState->getWindowName(), NULL);

   // Initialize BGFX
   Con::printf("");
   Con::printSeparator();
   Con::printf("Available Renderers:");

   bgfx::RendererType::Enum renderers[bgfx::RendererType::Count];
   U32 numRenderers = bgfx::getSupportedRenderers(renderers);
   for (U32 n = 0; n < numRenderers; ++n)
   {
      Con::printf("   %s", bgfx::getRendererName(renderers[n]));
   }

   // TODO: preference based renderer choosing.
   if ( !bgfxInitialized )
   {
      bgfx::x11SetDisplayWindow(sysinfo.info.x11.display, sysinfo.info.x11.window);
      bgfx::init();
      bgfxInitialized = true;
   }
   bgfx::reset(width, height, BGFX_RESET_NONE);

//#ifdef TORQUE_DEBUG
   bgfx::setDebug(BGFX_DEBUG_TEXT);
//#endif

   Rendering::windowWidth = width;
   Rendering::windowHeight = height;

   Graphics::init();
   Physics::init();
   Plugins::init();

   // repaint
   if ( repaint )
      Con::evaluate( "resetCanvas();" );

   return true;
}

//------------------------------------------------------------------------------
void BGFXDevice::swapBuffers()
{
   //SDL_GL_SwapBuffers();
   bgfx::frame();
}

//------------------------------------------------------------------------------
const char* BGFXDevice::getDriverInfo()
{
   return( "" );
}

//------------------------------------------------------------------------------
bool BGFXDevice::getGammaCorrection(F32 &g)
{
   U16 redtable[256];
   U16 greentable[256];
   U16 bluetable[256];

   if (SDL_GetGammaRamp(redtable, greentable, bluetable) == -1)
   {
      Con::warnf("getGammaCorrection error: %s", SDL_GetError());
      return false;
   }

   F32 csum = 0.0;
   U32 ccount = 0;

   for (U16 i = 0; i < 256; ++i)
   {
      if (i != 0 && redtable[i] != 0 && redtable[i] != 65535)
      {
         F64 b = (F64) i/256.0;
         F64 a = (F64) redtable[i]/65535.0;
         F32 c = (F32) (mLog(a)/mLog(b));

         csum += c;
         ++ccount;
      }
   }
   g = csum/ccount;

   return true;
}

//------------------------------------------------------------------------------
bool BGFXDevice::setGammaCorrection(F32 g)
{
   U16 redtable[256];
   U16 greentable[256];
   U16 bluetable[256];

   for (U16 i = 0; i < 256; ++i)
      redtable[i] = static_cast<U16>(mPow((F32) i/256.0f, g) * 65535.0f);
   dMemcpy(greentable,redtable,256*sizeof(U16));
   dMemcpy(bluetable,redtable,256*sizeof(U16));

   S32 ok = SDL_SetGammaRamp(redtable, greentable, bluetable);
   if (ok == -1)
      Con::warnf("Error setting gamma correction: %s", SDL_GetError());

   return ok != -1;
}

//------------------------------------------------------------------------------
bool BGFXDevice::setVerticalSync( bool on )
{
   Con::printf("WARNING: BGFXDevice::setVerticalSync is unimplemented %s %d\n", __FILE__, __LINE__);
   return false;
}

//------------------------------------------------------------------------------
DisplayDevice* BGFXDevice::create()
{
   return new BGFXDevice();
}

