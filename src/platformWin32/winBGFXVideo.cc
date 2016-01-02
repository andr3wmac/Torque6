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

#include "platformWin32/platformWin32.h"
#include "platformWin32/winBGFXVideo.h"
#include "console/console.h"
#include "math/mPoint.h"
#include "platform/event.h"
#include "game/gameInterface.h"
#include "console/consoleInternal.h"
#include "console/ast.h"
#include "io/fileStream.h"

#include <bx/timer.h>
#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>
#include <nanovg/nanovg.h>
#include <imgui/imgui.h>
#include "graphics/dgl.h"
#include "graphics/shaders.h"
#include "rendering/common.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "sysgui/sysgui.h"
#include "plugins/plugins.h"


//------------------------------------------------------------------------------

// TODO: Relocate this?

struct BgfxCallback : public bgfx::CallbackI
{
   virtual ~BgfxCallback()
   {
      //
   }

   virtual void fatal(bgfx::Fatal::Enum _code, const char* _str) BX_OVERRIDE
   {
      //
   }

   virtual void traceVargs(const char* _filePath, uint16_t _line, const char* _format, va_list _argList) BX_OVERRIDE { }
   virtual uint32_t cacheReadSize(uint64_t _id) BX_OVERRIDE { return 0; }
   virtual bool cacheRead(uint64_t _id, void* _data, uint32_t _size) BX_OVERRIDE { return false; }
   virtual void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) BX_OVERRIDE { }

   virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t /*_size*/, bool _yflip) BX_OVERRIDE
   {
      //
   }

   virtual void captureBegin(uint32_t _width, uint32_t _height, uint32_t /*_pitch*/, bgfx::TextureFormat::Enum /*_format*/, bool _yflip) BX_OVERRIDE { }
   virtual void captureEnd() BX_OVERRIDE { }
   virtual void captureFrame(const void* _data, uint32_t /*_size*/) BX_OVERRIDE { }
};

BgfxCallback bgfxCallback;

//------------------------------------------------------------------------------

struct CardProfile
{
   const char *vendor;     // manufacturer
   const char *renderer;   // driver name

   bool safeMode;          // destroy rendering context for resolution change
   bool lockArray;         // allow compiled vertex arrays
   bool subImage;          // allow glTexSubImage*
   bool fogTexture;        // require bound texture for combine extension
   bool noEnvColor;        // no texture environment color
   bool clipHigh;          // clip high resolutions
   bool deleteContext;		// delete rendering context
   bool texCompress;			// allow texture compression
   bool interiorLock;		// lock arrays for Interior render
   bool skipFirstFog;		// skip first two-pass fogging (dumb 3Dfx hack)
   bool only16;				// inhibit 32-bit resolutions
   bool noArraysAlpha;	   // don't use glDrawArrays with a GL_ALPHA texture

   const char *proFile;		// explicit profile of graphic settings
};

struct OSCardProfile
{
   const char *vendor;     // manufacturer
   const char *renderer;   // driver name

   bool allowOpenGL;			// allow BGFX driver
   bool preferOpenGL;		// prefer BGFX driver
};

static Vector<CardProfile> sCardProfiles(__FILE__, __LINE__);
static Vector<OSCardProfile> sOSCardProfiles(__FILE__, __LINE__);

//------------------------------------------------------------------------------

struct ProcessorProfile
{
    U16 clock;  // clock range max
    U16 adjust; // CPU adjust
};

static U8 sNumProcessors = 4;
static ProcessorProfile sProcessorProfiles[] =
{
    {  400,  0 },
    {  600,  5 },
    {  800, 10 },
    { 1000, 15 },
};

struct SettingProfile
{
    U16 performance;        // metric range max
    const char *settings;   // default file
};

static U8 sNumSettings = 3;
static SettingProfile sSettingProfiles[] =
{
    {  33, "LowProfile.tsc" },
    {  66, "MediumProfile.tsc" },
    { 100, "HighProfile.tsc" },
};

//------------------------------------------------------------------------------

static void clearCardProfiles()
{
   while (sCardProfiles.size())
   {
      dFree((char *) sCardProfiles.last().vendor);
      dFree((char *) sCardProfiles.last().renderer);

        dFree((char *) sCardProfiles.last().proFile);

      sCardProfiles.decrement();
   }
}

static void clearOSCardProfiles()
{
   while (sOSCardProfiles.size())
   {
      dFree((char *) sOSCardProfiles.last().vendor);
      dFree((char *) sOSCardProfiles.last().renderer);

      sOSCardProfiles.decrement();
   }
}

static void execScript(const char *scriptFile)
{
   // execute the script
   FileStream str;

   if (!str.open(scriptFile, FileStream::Read))
      return;

   U32 size = str.getStreamSize();
   char *script = new char[size + 1];

   str.read(size, script);
   str.close();

   script[size] = 0;
   Con::executef(2, "eval", script);
   delete[] script;
}

static void profileSystem(const char *vendor, const char *renderer)
{
   //Con::executef(2, "exec", "scripts/CardProfiles.tsc");
   execScript("CardProfiles.tsc");

   const char *arch;
   OSVERSIONINFO OSVersionInfo;
   const char *os = NULL;
   char osProfiles[64];

   if (dStrstr(PlatformSystemInfo.processor.name,"AMD") != NULL)
      arch = "AMD";
   else
      arch = "Intel";

   dMemset( &OSVersionInfo, 0, sizeof( OSVERSIONINFO ) );
   OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
   if ( GetVersionEx( &OSVersionInfo )	)
   {
      if (OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
      {
         if (OSVersionInfo.dwMinorVersion == 0)
         {
            if (dStrcmp(arch, "Intel") == 0)
               os = "W95";
         }
         else if (OSVersionInfo.dwMinorVersion == 10)
         {
            if (OSVersionInfo.szCSDVersion[1] != 'A')
               os = "W98";
            else
               os = "W98SE";
         }
         else
         {
            os = "WME";
         }
      }
      else
      {
         if (OSVersionInfo.dwMajorVersion >= 5)
            os = "W2K";
      }
        
      if ( os != NULL )
      {
         dSprintf(osProfiles,64,"%s%sCardProfiles.tsc",arch,os);
         //Con::executef(2, "exec", osProfiles);
         execScript(osProfiles);
      }
   }

   const char *proFile = NULL;
   U32 i;

   for (i = 0; i < (U32)sCardProfiles.size(); ++i)
   {
      if (dStrstr(vendor, sCardProfiles[i].vendor) &&
         (!dStrcmp(sCardProfiles[i].renderer, "*") ||
            dStrstr(renderer, sCardProfiles[i].renderer)))
      {
         Con::setBoolVariable("$pref::Video::safeModeOn", sCardProfiles[i].safeMode);
         Con::setBoolVariable("$pref::OpenGL::disableEXTCompiledVertexArray", !sCardProfiles[i].lockArray);
         Con::setBoolVariable("$pref::OpenGL::disableSubImage", !sCardProfiles[i].subImage);
         Con::setBoolVariable("$pref::TS::fogTexture", sCardProfiles[i].fogTexture);
         Con::setBoolVariable("$pref::OpenGL::noEnvColor", sCardProfiles[i].noEnvColor);
         Con::setBoolVariable("$pref::Video::clipHigh", sCardProfiles[i].clipHigh);
         if (!sCardProfiles[i].deleteContext)
         {
            OSVERSIONINFO OSVersionInfo;

            // HACK: The Voodoo3/5 on W2K crash when deleting a rendering context
                // So we're not deleting it.
                // Oh, and the Voodoo3 returns a Banshee renderer string under W2K
            dMemset(&OSVersionInfo, 0, sizeof(OSVERSIONINFO));
            OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
            if (GetVersionEx(&OSVersionInfo) &&
               OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
               OSVersionInfo.dwMajorVersion == 5)
               Con::setBoolVariable("$pref::Video::deleteContext", false);
            else
               Con::setBoolVariable("$pref::Video::deleteContext", true);
         }
         else
            Con::setBoolVariable("$pref::Video::deleteContext", true);
         Con::setBoolVariable("$pref::OpenGL::disableARBTextureCompression", !sCardProfiles[i].texCompress);
         Con::setBoolVariable("$pref::Interior::lockArrays", sCardProfiles[i].interiorLock);
         Con::setBoolVariable("$pref::TS::skipFirstFog", sCardProfiles[i].skipFirstFog);
         Con::setBoolVariable("$pref::Video::only16", sCardProfiles[i].only16);
         Con::setBoolVariable("$pref::OpenGL::noDrawArraysAlpha", sCardProfiles[i].noArraysAlpha);

         proFile = sCardProfiles[i].proFile;

         break;
      }
   }

   // defaults
   U16 glProfile;

   if (!proFile)
   {
      // no driver GL profile -- make one via weighting GL extensions
      glProfile = 25;

      Con::setBoolVariable("$pref::Video::safeModeOn", true);
      Con::setBoolVariable("$pref::OpenGL::disableEXTCompiledVertexArray", false);
      Con::setBoolVariable("$pref::OpenGL::disableSubImage", false);
      Con::setBoolVariable("$pref::TS::fogTexture", false);
      Con::setBoolVariable("$pref::OpenGL::noEnvColor", false);
      Con::setBoolVariable("$pref::Video::clipHigh", false);
      Con::setBoolVariable("$pref::Video::deleteContext", true);
      Con::setBoolVariable("$pref::OpenGL::disableARBTextureCompression", false);
      Con::setBoolVariable("$pref::Interior::lockArrays", true);
      Con::setBoolVariable("$pref::TS::skipFirstFog", false);
      Con::setBoolVariable("$pref::Video::only16", false);
      Con::setBoolVariable("$pref::OpenGL::noDrawArraysAlpha", false);
   }

   Con::setVariable("$pref::Video::profiledVendor", vendor);
   Con::setVariable("$pref::Video::profiledRenderer", renderer);

   if (!Con::getBoolVariable("$DisableSystemProfiling") &&
      ( dStrcmp(vendor, Con::getVariable("$pref::Video::defaultsVendor")) ||
      dStrcmp(renderer, Con::getVariable("$pref::Video::defaultsRenderer")) ))
   {
      if (proFile)
      {
         char settings[64];

         dSprintf(settings,64,"%s.tsc",proFile);
         //Con::executef(2, "exec", settings);
         execScript(settings);
      }
      else
      {
         U16 adjust;

         // match clock with profile
         for (i = 0; i < sNumProcessors; ++i)
         {
            adjust = sProcessorProfiles[i].adjust;

            if (PlatformSystemInfo.processor.mhz < sProcessorProfiles[i].clock) break;
         }

         const char *settings;

         // match performance metric with profile
         for (i = 0; i < sNumSettings; ++i)
         {
            settings = sSettingProfiles[i].settings;

            if (glProfile+adjust <= sSettingProfiles[i].performance) break;
         }

         //Con::executef(2, "exec", settings);
         execScript(settings);
      }

      bool match = false;

      for (i = 0; i < (U32)sOSCardProfiles.size(); ++i)
      {
         if (dStrstr(vendor, sOSCardProfiles[i].vendor) &&
            (!dStrcmp(sOSCardProfiles[i].renderer, "*") ||
               dStrstr(renderer, sOSCardProfiles[i].renderer)))
         {
            Con::setBoolVariable("$pref::Video::allowOpenGL", sOSCardProfiles[i].allowOpenGL);
            Con::setBoolVariable("$pref::Video::preferOpenGL", sOSCardProfiles[i].preferOpenGL);

            match = true;

            break;
         }
      }

      if (!match)
      {
         Con::setBoolVariable("$pref::Video::allowOpenGL", true);
         Con::setBoolVariable("$pref::Video::preferOpenGL", true);
      }

      Con::setVariable("$pref::Video::defaultsVendor", vendor);
      Con::setVariable("$pref::Video::defaultsRenderer", renderer);
   }

   clearCardProfiles();
   clearOSCardProfiles();
}

//------------------------------------------------------------------------------
BGFXDevice::BGFXDevice()
{
   initDevice();
}


//------------------------------------------------------------------------------
void BGFXDevice::initDevice()
{
   // Set the device name:
   mDeviceName = "BGFX";

   // Let Parent enumerate valid resolutions.
   Parent::initDevice();
}


//------------------------------------------------------------------------------
bool BGFXDevice::activate( U32 width, U32 height, U32 bpp, bool fullScreen )
{
   bool needResurrect = false;

   // If the rendering context exists, delete it:
   if ( winState.hGLRC )
   {
      Con::printf( "Killing the texture manager..." );
      Game->textureKill();
      needResurrect = true;
      winState.hGLRC = NULL;
   }

   // [neo, 5/31/2007 - #3174]
   if( winState.appMenu )
   {
      DestroyMenu( winState.appMenu );

      winState.appMenu = NULL;
   }

   // If the window already exists, kill it so we can start fresh:
   /*if ( winState.appWindow )
   {
      if ( winState.appDC )
      {
         Con::printf( "Releasing the device context..." );
         ReleaseDC( winState.appWindow, winState.appDC );
         winState.appDC = NULL;
      }
   
      Con::printf( "Destroying the window..." );
      DestroyWindow( winState.appWindow );
      winState.appWindow = NULL;
   }*/

   // If BGFX library already loaded, shut it down and reload it:
   //if ( winState.hinstOpenGL )
   //   GL_Shutdown();

   //GL_Init( "opengl32", "glu32" );

   static bool onceAlready = false;
   bool profiled = false;

   if ( !mFullScreenOnly && fullScreen && !onceAlready )
   {	
      OSVERSIONINFO OSVersionInfo;

      // HACK: The Voodoo5 on W2K will only work if the initial rendering
      // context is windowed.  Can you believe this crap?
      dMemset( &OSVersionInfo, 0, sizeof( OSVERSIONINFO ) );
      OSVersionInfo.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
      if (GetVersionEx(&OSVersionInfo) &&
         OSVersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT &&
         OSVersionInfo.dwMajorVersion == 5)
      {

         //MIN_RESOLUTION defined in platformWin32/platformGL.h
         if (!setScreenMode(MIN_RESOLUTION_X, MIN_RESOLUTION_Y, bpp, false, true, false))
            return false;
         else
         {
            //const char* vendorString   = (const char*) glGetString( GL_VENDOR );
            //const char* rendererString = (const char*) glGetString( GL_RENDERER );

            // only do this for the first session
            //if (!Con::getBoolVariable("$DisableSystemProfiling") &&
            //     ( dStrcmp(vendorString, Con::getVariable("$pref::Video::profiledVendor")) ||
            //    dStrcmp(rendererString, Con::getVariable("$pref::Video::profiledRenderer")) ))
            //{
            //profileSystem(vendorString, rendererString);
            //    profiled = true;
            //}
         }
      }

      onceAlready = true;
   }
    // Set the resolution:
   if ( !setScreenMode( width, height, bpp, ( fullScreen || mFullScreenOnly ), true, false ) )
      return false;

   // Get original gamma ramp
   mRestoreGamma = GetDeviceGammaRamp(winState.appDC, mOriginalRamp);

   // Output some driver info to the console:
   const char* rendererString = bgfx::getRendererName(bgfx::getRendererType());
   if ( rendererString )
      Con::printf( "Current Renderer: %s", rendererString );

   if ( needResurrect )
   {
      // Reload the textures:
      Con::printf( "Resurrecting the texture manager..." );
      Game->textureResurrect();
   }

   Con::setVariable( "$pref::Video::displayDevice", mDeviceName );

   return true;
}


//------------------------------------------------------------------------------
void BGFXDevice::shutdown()
{
   Con::printf( "Shutting down the BGFX display device..." );

   if ( winState.hGLRC )
   {
      if (mRestoreGamma)
         SetDeviceGammaRamp(winState.appDC, mOriginalRamp);

      //Con::printf( "Making the GL rendering context not current..." );
      //dwglMakeCurrent( NULL, NULL );
      //  if ( Con::getBoolVariable("$pref::Video::deleteContext", true) )
      //{
      //   Con::printf( "Deleting the GL rendering context..." );
      //      dwglDeleteContext( winState.hGLRC );
      //}
      winState.hGLRC = NULL;
   }

   if ( winState.appDC )
   {
      Con::printf( "Releasing the device context..." );
      ReleaseDC( winState.appWindow, winState.appDC );
      winState.appDC = NULL;
   }

   if ( smIsFullScreen )
   {
      Con::printf( "Restoring the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel );
      ChangeDisplaySettings( NULL, 0 );
   }

   Scene::destroy();
   Rendering::destroy();
   Physics::destroy();
   Graphics::destroy();
   Plugins::destroy();
   SysGUI::destroy();

   // Shutdown bgfx.
   bgfx::shutdown();
}


//------------------------------------------------------------------------------
// This is the real workhorse function of the DisplayDevice...
//
bool BGFXDevice::setScreenMode( U32 width, U32 height, U32 bpp, bool fullScreen, bool forceIt, bool repaint )
{
   HWND curtain = NULL;
   char errorMessage[256];
   Resolution newRes( width, height, bpp );
   bool newFullScreen = fullScreen;
   bool safeModeOn = Con::getBoolVariable( "$pref::Video::safeModeOn" );

   if ( !newFullScreen && mFullScreenOnly )
   {
      Con::warnf( ConsoleLogEntry::General, "BGFXDevice::setScreenMode - device or desktop color depth does not allow windowed mode!" );
      newFullScreen = true;
   }

   if ( !newFullScreen && ( (S32)newRes.w >= winState.desktopClientWidth || (S32)newRes.h >= winState.desktopClientHeight ) )
   {
      Con::warnf( ConsoleLogEntry::General, "BGFXDevice::setScreenMode -- can't switch to resolution larger than desktop in windowed mode!" );
      S32 resIndex = 0;
      S32 bestScore = 0, thisScore = 0;
      for ( S32 i = 0; i < mResolutionList.size(); i++ )
      {
         if ( newRes == mResolutionList[i] )
         {
            resIndex = i;
            break;
         }
         else if( (S32)newRes.h <= winState.desktopClientHeight && (S32)newRes.w <= winState.desktopClientWidth )
         {
            thisScore = abs( S32( newRes.w ) - S32( mResolutionList[i].w ) )
               + abs( S32( newRes.h ) - S32( mResolutionList[i].h ) )
               + ( newRes.bpp == mResolutionList[i].bpp ? 0 : 1 );

            if ( !bestScore || ( thisScore < bestScore ) )
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }
      
   }

   //MIN_RESOLUTION defined in platformWin32/platformGL.h
   if ( newRes.w < MIN_RESOLUTION_X || newRes.h < MIN_RESOLUTION_Y )
   {
      Con::warnf( ConsoleLogEntry::General, "BGFXDevice::setScreenMode -- can't go smaller than 640x480!" );
      return false;
   }

   if ( newFullScreen )
   {
      if (newRes.bpp != 16 && mFullScreenOnly)
         newRes.bpp = 16;

      // Match the new resolution to one in the list:
      U32 resIndex = 0;
      U32 bestScore = 0, thisScore = 0;
      for ( int i = 0; i < mResolutionList.size(); i++ )
      {
         if ( newRes == mResolutionList[i] )
         {
            resIndex = i;
            break;
         }
         else
         {
            thisScore = abs( S32( newRes.w ) - S32( mResolutionList[i].w ) )
                      + abs( S32( newRes.h ) - S32( mResolutionList[i].h ) )
                      + ( newRes.bpp == mResolutionList[i].bpp ? 0 : 1 );

            if ( !bestScore || ( thisScore < bestScore ) )
            {
               bestScore = thisScore;
               resIndex = i;
            }
         }
      }

      newRes = mResolutionList[resIndex];
   }
   else
   {
      // Basically ignore the bit depth parameter:
      newRes.bpp = winState.desktopBitsPixel;
   }

   // Return if already at this resolution:
   if ( !forceIt && newRes == smCurrentRes && newFullScreen == smIsFullScreen )
      return true;

   Con::printf( "   Setting screen mode to %dx%dx%d (%s)...", newRes.w, newRes.h, newRes.bpp, ( newFullScreen ? "fs" : "w" ) );

   bool needResurrect = false;

   if ( ( newRes.bpp != smCurrentRes.bpp ) || ( safeModeOn && ( ( smIsFullScreen != newFullScreen ) || newFullScreen ) ) )
   {
      // Delete the rendering context:
      if ( winState.hGLRC )
      {
         if (!Video::smNeedResurrect)
            {
            Con::printf( "Killing the texture manager..." );
            Game->textureKill();
            needResurrect = true;
            }
         winState.hGLRC = NULL;
      }

      // Release the device context:
      if ( winState.appDC )
      {
         Con::printf( "Releasing the device context..." );
         ReleaseDC( winState.appWindow, winState.appDC );
         winState.appDC = NULL;
      }
      
      // [neo, 5/31/2007 - #3174]
      if( winState.appMenu )
      {
         DestroyMenu( winState.appMenu );

         winState.appMenu = NULL;
      }

      // Destroy the window:
      /*if ( winState.appWindow )
      {
         Con::printf( "Destroying the window..." );
         DestroyWindow( winState.appWindow );
         winState.appWindow = NULL;
      }*/
   }
   else if ( smIsFullScreen != newFullScreen )
   {
      // Change the window style:
      Con::printf( "Changing the window style..." );
      S32 windowStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
      if ( newFullScreen )
         windowStyle |= ( WS_MAXIMIZE | WS_VISIBLE);
      else
         windowStyle |= ( WS_OVERLAPPEDWINDOW );

      if ( winState.appWindow && !SetWindowLong( winState.appWindow, GWL_STYLE, windowStyle ) )
         Con::errorf( "SetWindowLong failed to change the window style!" );
   }

   if( winState.appWindow && !newFullScreen )
      ShowWindow( winState.appWindow, SW_NORMAL );
   else
      ShowWindow( winState.appWindow, SW_MAXIMIZE );


   U32 test;
   if ( newFullScreen )
   {
      // Change the display settings:
      DEVMODE devMode;
      dMemset( &devMode, 0, sizeof( devMode ) );
      devMode.dmSize       = sizeof( devMode );
      devMode.dmPelsWidth  = newRes.w;
      devMode.dmPelsHeight = newRes.h;
      devMode.dmBitsPerPel = newRes.bpp;
      devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

      Con::printf( "Changing the display settings to %dx%dx%d...", newRes.w, newRes.h, newRes.bpp );
      curtain = CreateCurtain( newRes.w, newRes.h );
      test = ChangeDisplaySettings( &devMode, CDS_FULLSCREEN );
      if ( test != DISP_CHANGE_SUCCESSFUL )
      {
         smIsFullScreen = false;
         Con::setBoolVariable( "$pref::Video::fullScreen", false );
         
         ChangeDisplaySettings( NULL, 0 );
         Con::errorf( ConsoleLogEntry::General, "BGFXDevice::setScreenMode - ChangeDisplaySettings failed." );
         switch( test )
         {
            case DISP_CHANGE_RESTART:
               Platform::AlertOK( "Display Change Failed", "You must restart your machine to get the specified mode." );
               break;

            case DISP_CHANGE_BADMODE:
               Platform::AlertOK( "Display Change Failed", "The specified mode is not supported by this device." );
               break;

            default:
               Platform::AlertOK( "Display Change Failed", "Hardware failed to switch to the specified mode." );
               break;
         };
         DestroyWindow( curtain );
         return false;
      }
      else
         smIsFullScreen = true;
   }
   else if ( smIsFullScreen )
   {
      Con::printf( "Changing to the desktop display settings (%dx%dx%d)...", winState.desktopWidth, winState.desktopHeight, winState.desktopBitsPixel );
      ChangeDisplaySettings( NULL, 0 );
      smIsFullScreen = false;
   }

   Con::setBoolVariable( "$pref::Video::fullScreen", smIsFullScreen );

   bool newWindow = false;
   if ( !winState.appWindow )
   {
      Con::printf( "   Creating a new %swindow...", ( fullScreen ? "full-screen " : "" ) );
      winState.appWindow = CreateOpenGLWindow( newRes.w, newRes.h, newFullScreen, true );
      if ( !winState.appWindow )
      {
         AssertFatal( false, "BGFXDevice::setScreenMode\nFailed to create a new window!" );
         return false;
      }
      newWindow = true;
   }

   // Move the window:
   if ( !newFullScreen )
   {
      // Adjust the window rect to compensate for the window style:
      RECT windowRect;
      windowRect.left = windowRect.top = 0;
      windowRect.right = newRes.w;
      windowRect.bottom = newRes.h;

      AdjustWindowRect( &windowRect, GetWindowLong( winState.appWindow, GWL_STYLE ), false );
      U32 adjWidth = windowRect.right - windowRect.left;
      U32 adjHeight = windowRect.bottom - windowRect.top;

      // Center the window on the desktop:
      U32 xPos = 0, yPos = 0;
      
      if(adjWidth < (U32)winState.desktopClientWidth)
         xPos = ( winState.desktopClientWidth - adjWidth ) / 2;

      if(adjHeight < (U32)winState.desktopClientHeight)
         yPos = ( winState.desktopClientHeight - adjHeight ) / 2;

      test = SetWindowPos( winState.appWindow, 0, xPos, yPos, adjWidth, adjHeight, SWP_NOZORDER );
      if ( !test )
      {
         dSprintf( errorMessage, 255, "BGFXDevice::setScreenMode\nSetWindowPos failed trying to move the window to (%d,%d) and size it to %dx%d.", xPos, yPos, newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }

      AnimateWindow( winState.appWindow, 90, AW_BLEND );
   }
   else if ( !newWindow )
   {
      // Move and size the window to take up the whole screen:
      if ( !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
        || !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER) )
      {
         dSprintf( errorMessage, 255, "BGFXDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }
   }

   bool newDeviceContext = false;
   if ( !winState.appDC )
   {
      // Get a new device context:
      Con::printf( "   Acquiring a new device context..." );
      winState.appDC = GetDC( winState.appWindow );
      if ( !winState.appDC )
      {
         AssertFatal( false, "BGFXDevice::setScreenMode\nGetDC failed to get a valid device context!" );
         return false;
      }
      newDeviceContext = true;
   }

   smCurrentRes = newRes;
   Platform::setWindowSize( newRes.w, newRes.h );
   char tempBuf[15];
   dSprintf( tempBuf, sizeof( tempBuf ), "%d %d %d", smCurrentRes.w, smCurrentRes.h, smCurrentRes.bpp );
   if( fullScreen )
      Con::setVariable( "$pref::Video::resolution", tempBuf );
   else
      Con::setVariable( "$pref::Video::windowedRes", tempBuf );

   if ( curtain )
      DestroyWindow( curtain );

   // Doesn't hurt to do this even it isn't necessary:
   ShowWindow( winState.appWindow, SW_SHOW );
   SetForegroundWindow( winState.appWindow );
   SetFocus( winState.appWindow );

   if (newFullScreen)
   {
      // Doesn't hurt to do this an extra time, and seems to help in some cases
      if ( !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOACTIVATE)
        || !SetWindowPos( winState.appWindow, HWND_TOPMOST, 0, 0, newRes.w, newRes.h, SWP_NOZORDER) )
      {
         dSprintf( errorMessage, 255, "BGFXDevice::setScreenMode\nSetWindowPos failed to move the window to (0,0) and size it to %dx%d.", newRes.w, newRes.h );
         AssertFatal( false, errorMessage );
         return false;
      }
   }

   Con::printf("");
   Con::printSeparator();
   Con::printf("Available Renderers:");

   bgfx::RendererType::Enum renderers[bgfx::RendererType::Count];
	U32 numRenderers = bgfx::getSupportedRenderers(renderers);
   for (U32 n = 0; n < numRenderers; ++n)
   {
      Con::printf("   %s", bgfx::getRendererName(renderers[n]));
   }

   bgfx::winSetHwnd(winState.appWindow);

   // Renderer Type Selection
   const char* renderer = Con::getVariable("pref::Video::Renderer");
   if (dStrcmp(renderer, "OpenGL") == 0)
      bgfx::init(bgfx::RendererType::OpenGL);
   else if (dStrcmp(renderer, "OpenGLES") == 0)
      bgfx::init(bgfx::RendererType::OpenGLES);
   else if (dStrcmp(renderer, "Direct3D9") == 0)
      bgfx::init(bgfx::RendererType::Direct3D9);
   else if (dStrcmp(renderer, "Direct3D11") == 0)
      bgfx::init(bgfx::RendererType::Direct3D11);
   else if (dStrcmp(renderer, "Direct3D12") == 0)
      bgfx::init(bgfx::RendererType::Direct3D12);
   else if (dStrcmp(renderer, "Metal") == 0)
      bgfx::init(bgfx::RendererType::Metal);
   else if (dStrcmp(renderer, "Vulkan") == 0)
      bgfx::init(bgfx::RendererType::Vulkan);
   else
      bgfx::init(); // Auto-select.

   bool vsyncEnabled = Con::getBoolVariable("pref::Video::VSync", true);
   bgfx::reset(width, height, vsyncEnabled ? BGFX_RESET_VSYNC : BGFX_RESET_NONE);

#ifdef TORQUE_DEBUG
   bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif

   Rendering::canvasWidth = width;
   Rendering::canvasHeight = height;

   SysGUI::init();
   Plugins::init();
   Graphics::init();
   Physics::init();
   Rendering::init();
   Scene::init();

   return true;
}


//------------------------------------------------------------------------------
void BGFXDevice::swapBuffers()
{
   // This tells bgfx we're done with this frame.
   //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
   //U64 startTime = bx::getHPCounter();

   bgfx::frame();

   //U64 endTime = bx::getHPCounter();
   //Con::printf("swapBuffers took: %d microseconds.", (U32)((endTime - startTime) / hpFreq));
}


//------------------------------------------------------------------------------
const char* BGFXDevice::getDriverInfo()
{
   return( "" );
}


//------------------------------------------------------------------------------
bool BGFXDevice::getGammaCorrection(F32 &g)
{
   U16 ramp[256*3];

   if (!GetDeviceGammaRamp(winState.appDC, ramp))
      return false;

   F32 csum = 0.0;
   U32 ccount = 0;

   for (U16 i = 0; i < 256; ++i)
   {
      if (i != 0 && ramp[i] != 0 && ramp[i] != 65535)
      {
         F64 b = (F64) i/256.0;
         F64 a = (F64) ramp[i]/65535.0;
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
   U16 ramp[256*3];

   for (U16 i = 0; i < 256; ++i)
      ramp[i] = (U16)((U16)mPow((F32) i/256.0f, g) * 65535.0f);
   dMemcpy(&ramp[256],ramp,256*sizeof(U16));
   dMemcpy(&ramp[512],ramp,256*sizeof(U16));

   return SetDeviceGammaRamp(winState.appDC, ramp);
}

//------------------------------------------------------------------------------
bool BGFXDevice::setVerticalSync( bool on )
{
   return false;
}

//------------------------------------------------------------------------------
DisplayDevice* BGFXDevice::create()
{
   BGFXDevice* newBGFXDevice = new BGFXDevice();
   if ( newBGFXDevice )
	{
		newBGFXDevice->mFullScreenOnly = false;
      return (DisplayDevice*) newBGFXDevice;
	}
   else
      return NULL;
}
