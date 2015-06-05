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
#include "plugins.h"
#include "graphics/core.h"
#include "platform/event.h"
#include "sysgui/sysgui.h"
#include "graphics/TextureManager.h"
#include "graphics/dgl.h"
#include "3d/scene/core.h"
#include "assets/assetManager.h"

#include "plugins_shared.h"
#include "plugins_ScriptBinding.h"
#include "platform/platformLibrary.h"

namespace Plugins
{
   // Shared amongst plugins.
   PluginLink Link;

   // Used for internal management.
   Vector<Plugin>             _pluginList;
   Vector<AbstractClassRep*>  _pluginConsoleClasses;
   Vector<PluginAPI*>         _pluginAPIs;
   Vector<PluginAPIRequest>   _pluginAPIRequests;

   void init()
   {
      // Initialize all the function pointers.

      // Console
      Link.Con.printf = Con::printf;
      Link.Con.errorf = Con::errorf;
      Link.Con.warnf = Con::warnf;
      Link.Con.addCommand = Con::addCommand;
      Link.Con.getData = Con::getData;
      Link.Con.classLinkNamespaces = Con::classLinkNamespaces;
      Link.Con.registerClassRep = AbstractClassRep::registerClassRep;
      Link.Con.lookupNamespace = Con::lookupNamespace;

      Link.Con.TypeF32 = TypeF32;
      Link.Con.TypeS8 = TypeS8;
      Link.Con.TypeS32 = TypeS32;
      Link.Con.TypeS32Vector = TypeS32Vector;
      Link.Con.TypeBool = TypeBool;
      Link.Con.TypeBoolVector = TypeBoolVector;
      Link.Con.TypeF32Vector = TypeF32Vector;
      Link.Con.TypeString = TypeString;
      Link.Con.TypeStringTableEntryVector = TypeStringTableEntryVector;
      Link.Con.TypeCaseString = TypeCaseString;
      Link.Con.TypeFilename = TypeFilename;
      Link.Con.TypeEnum = TypeEnum;
      Link.Con.TypeFlag = TypeFlag;
      Link.Con.TypeSimObjectPtr = TypeSimObjectPtr;
      Link.Con.TypeSimObjectName = TypeSimObjectName;
      Link.Con.TypeSimObjectId = TypeSimObjectId;
      Link.Con.TypePoint3F = TypePoint3F;

      // String Table
      Link.StringTableLink = StringTable;

      // Resource Manager
      Link.ResourceManager = ResourceManager;

      // SysGUI
      Link.SysGUI.beginScrollArea = SysGUI::beginScrollArea;
      Link.SysGUI.button = SysGUI::button;
      Link.SysGUI.checkBox = SysGUI::checkBox;
      Link.SysGUI.endScrollArea = SysGUI::endScrollArea;
      Link.SysGUI.label = SysGUI::label;
      Link.SysGUI.list = SysGUI::list;
      Link.SysGUI.separator = SysGUI::separator;
      Link.SysGUI.slider = SysGUI::slider;
      Link.SysGUI.textInput = SysGUI::textInput;
      Link.SysGUI.addListValue = SysGUI::addListValue;
      Link.SysGUI.getListValue = SysGUI::getListValue;
      Link.SysGUI.getListSelected = SysGUI::getListSelected;
      Link.SysGUI.clearList = SysGUI::clearList;
      Link.SysGUI.setElementHidden = SysGUI::setElementHidden;
      Link.SysGUI.getLabelValue = SysGUI::getLabelValue;
      Link.SysGUI.setLabelValue = SysGUI::setLabelValue;
      Link.SysGUI.getTextValue = SysGUI::getTextValue;
      Link.SysGUI.setTextValue = SysGUI::setTextValue;
      Link.SysGUI.getIntValue = SysGUI::getIntValue;
      Link.SysGUI.setIntValue = SysGUI::setIntValue;
      Link.SysGUI.getBoolValue = SysGUI::getBoolValue;
      Link.SysGUI.setBoolValue = SysGUI::setBoolValue;
      Link.SysGUI.alignLeft = SysGUI::alignLeft;
      Link.SysGUI.alignRight = SysGUI::alignRight;
      Link.SysGUI.alignTop = SysGUI::alignTop;
      Link.SysGUI.alignBottom = SysGUI::alignBottom;
      Link.SysGUI.clearScrollArea = SysGUI::clearScrollArea;
      Link.SysGUI.seek = SysGUI::seek;
      Link.SysGUI.clearSeek = SysGUI::clearSeek;
      Link.SysGUI.beginCollapse = SysGUI::beginCollapse;
      Link.SysGUI.endCollapse = SysGUI::endCollapse;
      Link.SysGUI.colorWheel = SysGUI::colorWheel;
      Link.SysGUI.getColorValue = SysGUI::getColorValue;
      Link.SysGUI.setColorValue = SysGUI::setColorValue;
      Link.SysGUI.vector3 = SysGUI::vector3;
      Link.SysGUI.getVector3Value = SysGUI::getVector3Value;
      Link.SysGUI.setVector3Value = SysGUI::setVector3Value;
      Link.SysGUI.image = SysGUI::image;
      Link.SysGUI.getImageValue = SysGUI::getImageValue;
      Link.SysGUI.setImageValue = SysGUI::setImageValue;

      // NanoVG
      Link.NanoVG.nvgSave = nvgSave;
      Link.NanoVG.nvgRestore = nvgRestore;
      Link.NanoVG.nvgReset = nvgReset;
      Link.NanoVG.nvgRGBA = nvgRGBA;
      Link.NanoVG.nvgRGBAf = nvgRGBAf;
      Link.NanoVG.nvgBeginPath = nvgBeginPath;
      Link.NanoVG.nvgMoveTo = nvgMoveTo;
      Link.NanoVG.nvgBezierTo = nvgBezierTo;
      Link.NanoVG.nvgCircle = nvgCircle;
      Link.NanoVG.nvgRect = nvgRect;
      Link.NanoVG.nvgRoundedRect = nvgRoundedRect;
      Link.NanoVG.nvgLinearGradient = nvgLinearGradient;
      Link.NanoVG.nvgFill = nvgFill;
      Link.NanoVG.nvgFillColor = nvgFillColor;
      Link.NanoVG.nvgFillPaint = nvgFillPaint;
      Link.NanoVG.nvgStroke = nvgStroke;
      Link.NanoVG.nvgStrokeColor = nvgStrokeColor;
      Link.NanoVG.nvgStrokeWidth = nvgStrokeWidth;
      Link.NanoVG.nvgFontFace = nvgFontFace;
      Link.NanoVG.nvgFontSize = nvgFontSize;
      Link.NanoVG.nvgText = nvgText;
      Link.NanoVG.nvgTextAlign = nvgTextAlign;
      Link.NanoVG.nvgImagePattern = nvgImagePattern;
      Link.NanoVG.nvgImageSize = nvgImageSize;

      // Scene
      Link.Scene.getActiveCamera = Scene::getActiveCamera;
      Link.Scene.addCamera = Scene::addCamera;
      Link.Scene.getCamera = Scene::getCamera;
      Link.Scene.pushActiveCamera = Scene::pushActiveCamera;
      Link.Scene.popActiveCamera = Scene::popActiveCamera;
      Link.Scene.getEntityGroup = Scene::getEntityGroup;
      Link.Scene.raycast = Scene::raycast;
      Link.Scene.directionalLightDir = &Scene::directionalLightDir;
      Link.Scene.directionalLightColor = &Scene::directionalLightColor;
      Link.Scene.directionalLightAmbient = &Scene::directionalLightAmbient;
      Link.Scene.setDirectionalLight = Scene::setDirectionalLight;
      Link.Scene.addEntity = Scene::addEntity;
      Link.Scene.removeEntity = Scene::removeEntity;
      Link.Scene.getMaterialAsset = getMaterialAsset;
      Link.Scene.getMeshAsset = getMeshAsset;

      // Physics
      Link.Physics.pause = Physics::pause;
      Link.Physics.resume = Physics::resume;

      // Rendering
      Link.Rendering.canvasSizeChanged = &Rendering::canvasSizeChanged;
      Link.Rendering.canvasWidth = &Rendering::canvasWidth;
      Link.Rendering.canvasHeight = &Rendering::canvasHeight;
      Link.Rendering.createRenderData = Rendering::createRenderData;
      Link.Rendering.viewMatrix = Rendering::viewMatrix;
      Link.Rendering.projectionMatrix = Rendering::projectionMatrix;
      Link.Rendering.screenToWorld = Rendering::screenToWorld;
      Link.Rendering.worldToScreen = Rendering::worldToScreen;
      Link.Rendering.getDeferredRendering = Rendering::getDeferredRendering;

      // Graphics
      Link.Graphics.PosUVNormalVertex = &Graphics::PosUVNormalVertex::ms_decl;
      Link.Graphics.PosUVColorVertex = &Graphics::PosUVColorVertex::ms_decl;
      Link.Graphics.cubeIB = &Graphics::cubeIB;
      Link.Graphics.cubeVB = &Graphics::cubeVB;
      Link.Graphics.loadTexture = TextureManager::loadTexture;
      Link.Graphics.getTextureUniform = Graphics::Shader::getTextureUniform;
      Link.Graphics.getUniformVec4 = Graphics::Shader::getUniformVec4;
      Link.Graphics.getUniformMat4 = Graphics::Shader::getUniformMat4;
      Link.Graphics.getShader = Graphics::getShader;
      Link.Graphics.getShaderAsset = Graphics::getShaderAsset;
      Link.Graphics.fullScreenQuad = fullScreenQuad;
      Link.Graphics.dglScreenQuad = dglScreenQuad;
      Link.Graphics.screenSpaceQuad = screenSpaceQuad;
      Link.Graphics.drawLine3D = drawLine3D;
      Link.Graphics.drawBox3D = drawBox3D;
      Link.Graphics.dglGetNVGContext = dglGetNVGContext;
      Link.Graphics.getView = Graphics::getView;

      // Asset Database
      Link.AssetDatabaseLink.findAssetType = Assets::findAssetType;

      // bgfx
      Link.bgfx.setViewClear = bgfx::setViewClear;
      Link.bgfx.setViewRect = bgfx::setViewRect;
      Link.bgfx.setViewTransform = bgfx::setViewTransform;
      Link.bgfx.submit = bgfx::submit;
      Link.bgfx.dbgTextPrintf = bgfx::dbgTextPrintf;
      Link.bgfx.dbgTextClear = bgfx::dbgTextClear;
      Link.bgfx.setProgram = bgfx::setProgram;
      Link.bgfx.setTransform = bgfx::setTransform;
      Link.bgfx.setTexture = bgfx::setTexture;
      Link.bgfx.setState = bgfx::setState;
      Link.bgfx.setUniform = bgfx::setUniform;
      Link.bgfx.makeRef = bgfx::makeRef;
      Link.bgfx.createIndexBuffer = bgfx::createIndexBuffer;
      Link.bgfx.destroyIndexBuffer = bgfx::destroyIndexBuffer;
      Link.bgfx.createDynamicIndexBuffer = bgfx::createDynamicIndexBuffer;
      Link.bgfx.updateDynamicIndexBuffer = bgfx::updateDynamicIndexBuffer;
      Link.bgfx.destroyDynamicIndexBuffer = bgfx::destroyDynamicIndexBuffer;
      Link.bgfx.createVertexBuffer = bgfx::createVertexBuffer;
      Link.bgfx.destroyVertexBuffer = bgfx::destroyVertexBuffer;
      Link.bgfx.createDynamicVertexBuffer = bgfx::createDynamicVertexBuffer;
      Link.bgfx.updateDynamicVertexBuffer = bgfx::updateDynamicVertexBuffer;
      Link.bgfx.destroyDynamicVertexBuffer = bgfx::destroyDynamicVertexBuffer;
      Link.bgfx.createFrameBuffer = bgfx::createFrameBuffer;
      Link.bgfx.destroyFrameBuffer = bgfx::destroyFrameBuffer;
      Link.bgfx.createTexture2D = bgfx::createTexture2D;
      Link.bgfx.updateTexture2D = bgfx::updateTexture2D;
      Link.bgfx.destroyTexture = bgfx::destroyTexture;
      Link.bgfx.setViewFrameBuffer = bgfx::setViewFrameBuffer;
      Link.bgfx.alloc = bgfx::alloc;
      Link.bgfx.copy = bgfx::copy;

      // Plugin API
      Link.addPluginAPI = addPluginAPI;
      Link.requestPluginAPI = requestPluginAPI;
   }

   void destroy()
   {

   }

   bool load(const char* path)
   {
      Plugin* p = new Plugin();
      if ( p->load(path) )
      {
         _pluginList.push_back(*p);
         return true;
      }

      delete p;
      Con::errorf("[PLUGIN] Could not load plugin: %s", path);
      //Con::errorf("[PLUGIN] Error: %s", dlerror());
      return false;
   }

   // PluginAPI is just a way for plugins to pass structs of function pointers
   // to each for cross plugin communication.
   void addPluginAPI(PluginAPI* api)
   {
      // Check if it already existed.
      for ( U32 n = 0; n < _pluginAPIs.size(); ++n )
      {
         if ( dStrcmp(_pluginAPIs[n]->pluginName, api->pluginName) == 0 )
            return;
      }

      // Do we have a pending request for it? Fufill them all now.
      Vector<PluginAPIRequest> new_request_list;
      for ( U32 n = 0; n < _pluginAPIRequests.size(); ++n )
      {
         if ( dStrcmp(_pluginAPIRequests[n].pluginName, api->pluginName) == 0 )
         {
            _pluginAPIRequests[n].requestCallback(api);
         } else {
            new_request_list.push_back(_pluginAPIRequests[n]);
         }
      }
      _pluginAPIRequests = new_request_list;

      // Add to list.
      _pluginAPIs.push_back(api);
   }

   // PluginAPI uses a request system so it works independant of the order
   // plugins are loaded.
   void requestPluginAPI(const char* name, void (*requestCallback)(PluginAPI* api))
   {
      for ( U32 n = 0; n < _pluginAPIs.size(); ++n )
      {
         if ( dStrcmp(_pluginAPIs[n]->pluginName, name) == 0 )
         {
            requestCallback(_pluginAPIs[n]);
            return;
         }
      }

      // If we didn't find one then store the request for later.
      PluginAPIRequest req;
      dStrcpy(req.pluginName, name);
      req.requestCallback = requestCallback;
      _pluginAPIRequests.push_back(req);
   }

// --------------------

   Plugin::Plugin()
   {
      mLoaded = false;
      mHInst = NULL;
   }

   Plugin::~Plugin()
   {
      if ( !mLoaded ) return;
      mHInst = NULL;
   }

   bool Plugin::load(const char* path)
   {
      unload();

      // Expand path and load library.
      Con::expandPath( mPath, sizeof(mPath), path );
      mHInst = openLibrary(mPath);
      if ( !mHInst ) return false;

      // Register plugin console classes
      for(S32 n = 0; n < Plugins::_pluginConsoleClasses.size(); ++n)
         Plugins::_pluginConsoleClasses[n]->registerClass();
      Plugins::_pluginConsoleClasses.clear();

      // Create/Destroy function pointers.
      _create = (Plugin::createFunc)getLibraryFunc(mHInst, "create");
      _destroy = (Plugin::destroyFunc)getLibraryFunc(mHInst, "destroy");

      // Call the create function and establish the link.
      if ( _create )
         _create();

      // Tick Functions
      _interpolateTick = (Plugin::interpolateTickFunc)getLibraryFunc(mHInst, "interpolateTick");
      _processTick = (Plugin::processTickFunc)getLibraryFunc(mHInst, "processTick");
      _advanceTime = (Plugin::advanceTimeFunc)getLibraryFunc(mHInst, "advanceTime");
      setProcessTicks(_interpolateTick || _processTick || _advanceTime);

      // Render Functions
      _preRender = (Plugin::preRenderFunc)getLibraryFunc(mHInst, "preRender");
      _render = (Plugin::renderFunc)getLibraryFunc(mHInst, "render");
      _postRender = (Plugin::postRenderFunc)getLibraryFunc(mHInst, "postRender");
      setRendering(_preRender || _render || _postRender);

      mLoaded = true;
      return true;
   }

   void Plugin::unload()
   {
      if ( mHInst ) 
      {
         if ( _destroy )
            _destroy();

         closeLibrary(mHInst);
      }

      mLoaded = false;
   }

   void Plugin::interpolateTick( F32 delta )
   {
      if ( !mLoaded || !_interpolateTick ) return;
      _interpolateTick(delta);
   }

   void Plugin::processTick()
   {
      if ( !mLoaded || !_processTick ) return;
      _processTick();
   }

   void Plugin::advanceTime( F32 timeDelta )
   {
      if ( !mLoaded || !_advanceTime ) return;
      _advanceTime(timeDelta);   
   }

   void Plugin::preRender()
   {
      if ( !mLoaded || !_preRender ) return;
      _preRender();   
   }

   void Plugin::render()
   {
      if ( !mLoaded || !_render ) return;
      _render();   
   }

   void Plugin::postRender()
   {
      if ( !mLoaded || !_postRender ) return;
      _postRender();   
   }
}      
