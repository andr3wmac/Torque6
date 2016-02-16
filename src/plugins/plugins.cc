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
#include "graphics/textureAsset.h"
#include "graphics/dgl.h"
#include "scene/scene.h"
#include "assets/assetManager.h"
#include "module/moduleManager.h"

#include "plugins_shared.h"
#include "plugins_Binding.h"
#include "platform/platformLibrary.h"

namespace Torque
{
   // Shared amongst plugins.
   PluginsWrapper         Plugins;
   EngineWrapper          Engine;
   PlatformWrapper        Platform;
   ConsoleWrapper         Con;
   SysGUIWrapper          SysGUI;
   NanoVGWrapper          NanoVG;
   SceneWrapper           Scene;
   PhysicsWrapper         Physics;
   RenderingWrapper       Rendering;
   GraphicsWrapper        Graphics;
   AssetDatabaseWrapper   AssetDatabaseLink;
   BGFXWrapper            bgfx;

   ModuleManager*         ModuleDatabaseLink;
   _StringTable*          StringTableLink;
   ResManager*            ResourceManager;
}

namespace Plugins
{
   // Used for internal management.
   Vector<Plugins::Plugin>             _pluginList;
   Vector<Plugins::PluginAPI*>         _pluginAPIs;
   Vector<Plugins::PluginAPIRequest>   _pluginAPIRequests;
   Vector<AbstractClassRep*>           _pluginConsoleClasses;

   void init()
   {
      // Initialize all the function pointers.

      // Engine
      Torque::Engine.mainLoop = Platform::mainLoop;
      Torque::Engine.resizeWindow = Platform::resizeWindow;
      Torque::Engine.mouseMove = Platform::mouseMove;
      Torque::Engine.mouseButton = Platform::mouseButton;
      Torque::Engine.keyDown = Platform::keyDown;
      Torque::Engine.keyUp = Platform::keyUp;
      Torque::Engine.ProfilerLink = gProfiler;

      // Platform
      Torque::Platform.stripBasePath = Platform::stripBasePath;
      Torque::Platform.makeFullPathName = Platform::makeFullPathName;
      Torque::Platform.makeRelativePathName = Platform::makeRelativePathName;
      Torque::Platform.pathCopy = Platform::pathCopy;
      Torque::Platform.createPath = Platform::createPath;

      // Console
      Torque::Con.printf = Con::printf;
      Torque::Con.errorf = Con::errorf;
      Torque::Con.warnf = Con::warnf;
      Torque::Con.evaluate = Con::evaluate;
      Torque::Con.evaluatef = Con::evaluatef;
      Torque::Con.addCommand = Con::addCommand;
      Torque::Con.getData = Con::getData;
      Torque::Con.setData = Con::setData;
      Torque::Con.classLinkNamespaces = Con::classLinkNamespaces;
      Torque::Con.registerClassRep = AbstractClassRep::registerClassRep;
      Torque::Con.getNamespaceList = Con::getNamespaceList;
      Torque::Con.lookupNamespace = Con::lookupNamespace;
      Torque::Con.createObject = Con::createObject;

      // Console Types
      Torque::Con.TypeF32 = TypeF32;
      Torque::Con.TypeS8 = TypeS8;
      Torque::Con.TypeS32 = TypeS32;
      Torque::Con.TypeS32Vector = TypeS32Vector;
      Torque::Con.TypeBool = TypeBool;
      Torque::Con.TypeBoolVector = TypeBoolVector;
      Torque::Con.TypeF32Vector = TypeF32Vector;
      Torque::Con.TypeString = TypeString;
      Torque::Con.TypeStringTableEntryVector = TypeStringTableEntryVector;
      Torque::Con.TypeCaseString = TypeCaseString;
      Torque::Con.TypeFilename = TypeFilename;
      Torque::Con.TypeEnum = TypeEnum;
      Torque::Con.TypeFlag = TypeFlag;
      Torque::Con.TypeSimObjectPtr = TypeSimObjectPtr;
      Torque::Con.TypeSimObjectName = TypeSimObjectName;
      Torque::Con.TypeSimObjectId = TypeSimObjectId;
      Torque::Con.TypePoint3F = TypePoint3F;
      Torque::Con.TypeAssetLooseFilePath = TypeAssetLooseFilePath;
      Torque::Con.TypeColorF = TypeColorF;
      Torque::Con.TypeColorI = TypeColorI;

      // String Table
      Torque::StringTableLink = StringTable;

      // String Table
      Torque::ModuleDatabaseLink = &ModuleDatabase;

      // Resource Manager
      Torque::ResourceManager = ResourceManager;

      // SysGUI
      Torque::SysGUI.beginScrollArea = SysGUI::beginScrollArea;
      Torque::SysGUI.button = SysGUI::button;
      Torque::SysGUI.checkBox = SysGUI::checkBox;
      Torque::SysGUI.endScrollArea = SysGUI::endScrollArea;
      Torque::SysGUI.label = SysGUI::label;
      Torque::SysGUI.list = SysGUI::list;
      Torque::SysGUI.separator = SysGUI::separator;
      Torque::SysGUI.slider = SysGUI::slider;
      Torque::SysGUI.textInput = SysGUI::textInput;
      Torque::SysGUI.addListValue = SysGUI::addListValue;
      Torque::SysGUI.getListValue = SysGUI::getListValue;
      Torque::SysGUI.getListSelected = SysGUI::getListSelected;
      Torque::SysGUI.clearList = SysGUI::clearList;
      Torque::SysGUI.setElementHidden = SysGUI::setElementHidden;
      Torque::SysGUI.getLabelValue = SysGUI::getLabelValue;
      Torque::SysGUI.setLabelValue = SysGUI::setLabelValue;
      Torque::SysGUI.getTextValue = SysGUI::getTextValue;
      Torque::SysGUI.setTextValue = SysGUI::setTextValue;
      Torque::SysGUI.getIntValue = SysGUI::getIntValue;
      Torque::SysGUI.setIntValue = SysGUI::setIntValue;
      Torque::SysGUI.getBoolValue = SysGUI::getBoolValue;
      Torque::SysGUI.setBoolValue = SysGUI::setBoolValue;
      Torque::SysGUI.alignLeft = SysGUI::alignLeft;
      Torque::SysGUI.alignRight = SysGUI::alignRight;
      Torque::SysGUI.alignTop = SysGUI::alignTop;
      Torque::SysGUI.alignBottom = SysGUI::alignBottom;
      Torque::SysGUI.clearScrollArea = SysGUI::clearScrollArea;
      Torque::SysGUI.seek = SysGUI::seek;
      Torque::SysGUI.clearSeek = SysGUI::clearSeek;
      Torque::SysGUI.beginCollapse = SysGUI::beginCollapse;
      Torque::SysGUI.endCollapse = SysGUI::endCollapse;
      Torque::SysGUI.colorWheel = SysGUI::colorWheel;
      Torque::SysGUI.getColorValue = SysGUI::getColorValue;
      Torque::SysGUI.setColorValue = SysGUI::setColorValue;
      Torque::SysGUI.vector3 = SysGUI::vector3;
      Torque::SysGUI.getVector3Value = SysGUI::getVector3Value;
      Torque::SysGUI.setVector3Value = SysGUI::setVector3Value;
      Torque::SysGUI.image = SysGUI::image;
      Torque::SysGUI.getImageValue = SysGUI::getImageValue;
      Torque::SysGUI.setImageValue = SysGUI::setImageValue;

      // NanoVG
      Torque::NanoVG.nvgSave = nvgSave;
      Torque::NanoVG.nvgRestore = nvgRestore;
      Torque::NanoVG.nvgReset = nvgReset;
      Torque::NanoVG.nvgRGBA = nvgRGBA;
      Torque::NanoVG.nvgRGBAf = nvgRGBAf;
      Torque::NanoVG.nvgBeginPath = nvgBeginPath;
      Torque::NanoVG.nvgMoveTo = nvgMoveTo;
      Torque::NanoVG.nvgBezierTo = nvgBezierTo;
      Torque::NanoVG.nvgCircle = nvgCircle;
      Torque::NanoVG.nvgRect = nvgRect;
      Torque::NanoVG.nvgRoundedRect = nvgRoundedRect;
      Torque::NanoVG.nvgLinearGradient = nvgLinearGradient;
      Torque::NanoVG.nvgFill = nvgFill;
      Torque::NanoVG.nvgFillColor = nvgFillColor;
      Torque::NanoVG.nvgFillPaint = nvgFillPaint;
      Torque::NanoVG.nvgStroke = nvgStroke;
      Torque::NanoVG.nvgStrokeColor = nvgStrokeColor;
      Torque::NanoVG.nvgStrokeWidth = nvgStrokeWidth;
      Torque::NanoVG.nvgFontFace = nvgFontFace;
      Torque::NanoVG.nvgFontSize = nvgFontSize;
      Torque::NanoVG.nvgText = nvgText;
      Torque::NanoVG.nvgTextAlign = nvgTextAlign;
      Torque::NanoVG.nvgImagePattern = nvgImagePattern;
      Torque::NanoVG.nvgImageSize = nvgImageSize;

      // Scene
      Torque::Scene.getSceneGroup = Scene::getSceneGroup;
      Torque::Scene.raycast = Scene::raycast;
      Torque::Scene.addObject = Scene::addObject;
      Torque::Scene.deleteObject = Scene::deleteObject;
      Torque::Scene.removeObject = Scene::removeObject;
      Torque::Scene.getMaterialAsset = getMaterialAsset;
      Torque::Scene.getMeshAsset = getMeshAsset;
      Torque::Scene.createMeshAsset = createMeshAsset;
      Torque::Scene.createTextureAsset = createTextureAsset;
      Torque::Scene.createMaterialAsset = createMaterialAsset;
      Torque::Scene.createMaterialTemplate = createMaterialTemplate;
      Torque::Scene.clear = Scene::clear;
      Torque::Scene.append = Scene::append;
      Torque::Scene.load = Scene::load;
      Torque::Scene.save = Scene::save;
      Torque::Scene.refresh = Scene::refresh;

      // Physics
      Torque::Physics.pause = Physics::pause;
      Torque::Physics.resume = Physics::resume;

      // Rendering
      Torque::Rendering.canvasSizeChanged = &Rendering::canvasSizeChanged;
      Torque::Rendering.canvasWidth = &Rendering::canvasWidth;
      Torque::Rendering.canvasHeight = &Rendering::canvasHeight;
      Torque::Rendering.createRenderData = Rendering::createRenderData;
      Torque::Rendering.directionalLightDir = &Rendering::directionalLightDir;
      Torque::Rendering.directionalLightColor = &Rendering::directionalLightColor;
      Torque::Rendering.screenToWorld = Rendering::screenToWorld;
      Torque::Rendering.closestPointsOnTwoLines = Rendering::closestPointsOnTwoLines;
      Torque::Rendering.worldToScreen = Rendering::worldToScreen;
      Torque::Rendering.getLightList = Rendering::getLightList;
      Torque::Rendering.getActiveCamera = Rendering::getActiveCamera;
      Torque::Rendering.addRenderHook = Rendering::addRenderHook;
      Torque::Rendering.removeRenderHook = Rendering::removeRenderHook;

      // Graphics
      Torque::Graphics.PosUVNormalVertex = &Graphics::PosUVNormalVertex::ms_decl;
      Torque::Graphics.PosUVColorVertex = &Graphics::PosUVColorVertex::ms_decl;
      Torque::Graphics.cubeIB = &Graphics::cubeIB;
      Torque::Graphics.cubeVB = &Graphics::cubeVB;
      Torque::Graphics.loadTexture = TextureManager::loadTexture;
      Torque::Graphics.getTextureUniform = Graphics::Shader::getTextureUniform;
      Torque::Graphics.getUniformVec4 = Graphics::Shader::getUniformVec4;
      Torque::Graphics.getUniformMat4 = Graphics::Shader::getUniformMat4;
      Torque::Graphics.getShader = Graphics::getShader;
      Torque::Graphics.getDefaultShader = Graphics::getDefaultShader;
      Torque::Graphics.getShaderAsset = Graphics::getShaderAsset;
      Torque::Graphics.fullScreenQuad = fullScreenQuad;
      Torque::Graphics.dglScreenQuad = dglScreenQuad;
      Torque::Graphics.screenSpaceQuad = screenSpaceQuad;
      Torque::Graphics.dglGetNVGContext = dglGetNVGContext;
      Torque::Graphics.getView = Graphics::getView;
      Torque::Graphics.getTemporaryView = Graphics::getTemporaryView;
      Torque::Graphics.drawLine3D = drawLine3D;
      Torque::Graphics.drawCircle3D = drawCircle3D;
      Torque::Graphics.drawCone3D = drawCone3D;
      Torque::Graphics.drawBox3D = drawBox3D;
      Torque::Graphics.drawPlane3D = drawPlane3D;
      Torque::Graphics.drawBillboard = drawBillboard;

      // Graphics : dgl
      Torque::Graphics.dglClearBitmapModulation = dglClearBitmapModulation;
      Torque::Graphics.dglDrawBitmap = dglDrawBitmap;

      // Asset Database
      Torque::AssetDatabaseLink.findAssetType = Assets::findAssetType;
      Torque::AssetDatabaseLink.getDeclaredAssets = Assets::getDeclaredAssets;
      Torque::AssetDatabaseLink.addDeclaredAsset = Assets::addDeclaredAsset;
      Torque::AssetDatabaseLink.getAssetBase = Assets::getAssetBase;

      // bgfx
      Torque::bgfx.setViewClear = bgfx::setViewClear;
      Torque::bgfx.setViewRect = bgfx::setViewRect;
      Torque::bgfx.setViewTransform = bgfx::setViewTransform;
      Torque::bgfx.touch = bgfx::touch;
      Torque::bgfx.submit = bgfx::submit;
      Torque::bgfx.dbgTextPrintf = bgfx::dbgTextPrintf;
      Torque::bgfx.dbgTextClear = bgfx::dbgTextClear;
      Torque::bgfx.setTransform = bgfx::setTransform;
      Torque::bgfx.setTexture = bgfx::setTexture;
      Torque::bgfx.setTextureA = bgfx::setTexture;
      Torque::bgfx.setState = bgfx::setState;
      Torque::bgfx.setUniform = bgfx::setUniform;
      Torque::bgfx.makeRef = bgfx::makeRef;
      Torque::bgfx.createIndexBuffer = bgfx::createIndexBuffer;
      Torque::bgfx.destroyIndexBuffer = bgfx::destroyIndexBuffer;
      Torque::bgfx.createDynamicIndexBuffer = bgfx::createDynamicIndexBuffer;
      Torque::bgfx.updateDynamicIndexBuffer = bgfx::updateDynamicIndexBuffer;
      Torque::bgfx.destroyDynamicIndexBuffer = bgfx::destroyDynamicIndexBuffer;
      Torque::bgfx.createVertexBuffer = bgfx::createVertexBuffer;
      Torque::bgfx.destroyVertexBuffer = bgfx::destroyVertexBuffer;
      Torque::bgfx.createDynamicVertexBuffer = bgfx::createDynamicVertexBuffer;
      Torque::bgfx.updateDynamicVertexBuffer = bgfx::updateDynamicVertexBuffer;
      Torque::bgfx.destroyDynamicVertexBuffer = bgfx::destroyDynamicVertexBuffer;
      Torque::bgfx.createFrameBuffer = bgfx::createFrameBuffer;
      Torque::bgfx.destroyFrameBuffer = bgfx::destroyFrameBuffer;
      Torque::bgfx.createTexture2D = bgfx::createTexture2D;
      Torque::bgfx.updateTexture2D = bgfx::updateTexture2D;
      Torque::bgfx.createTextureCube = bgfx::createTextureCube;
      Torque::bgfx.destroyTexture = bgfx::destroyTexture;
      Torque::bgfx.setViewFrameBuffer = bgfx::setViewFrameBuffer;
      Torque::bgfx.alloc = bgfx::alloc;
      Torque::bgfx.copy = bgfx::copy;
      Torque::bgfx.blit = bgfx::blit;
      Torque::bgfx.blitA = bgfx::blit;
      Torque::bgfx.blitB = bgfx::blit;
      Torque::bgfx.blitC = bgfx::blit;

      // Plugin API
      Torque::Plugins.addPluginAPI = Plugins::addPluginAPI;
      Torque::Plugins.requestPluginAPI = Plugins::requestPluginAPI;
   }

   void destroy()
   {

   }

   bool load(const char* path)
   {
      Plugins::Plugin* p = new Plugins::Plugin();
      if (p->load(path))
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
   void addPluginAPI(Plugins::PluginAPI* api)
   {
      // Check if it already existed.
      for (S32 n = 0; n < _pluginAPIs.size(); ++n)
      {
         if (dStrcmp(_pluginAPIs[n]->pluginName, api->pluginName) == 0)
            return;
      }

      // Do we have a pending request for it? Fufill them all now.
      Vector<Plugins::PluginAPIRequest> new_request_list;
      for (S32 n = 0; n < _pluginAPIRequests.size(); ++n)
      {
         if (dStrcmp(_pluginAPIRequests[n].pluginName, api->pluginName) == 0)
         {
            _pluginAPIRequests[n].requestCallback(api);
         }
         else {
            new_request_list.push_back(_pluginAPIRequests[n]);
         }
      }
      _pluginAPIRequests = new_request_list;

      // Add to list.
      _pluginAPIs.push_back(api);
   }

   // PluginAPI uses a request system so it works independant of the order
   // plugins are loaded.
   void requestPluginAPI(const char* name, void(*requestCallback)(Plugins::PluginAPI* api))
   {
      for (S32 n = 0; n < _pluginAPIs.size(); ++n)
      {
         if (dStrcmp(_pluginAPIs[n]->pluginName, name) == 0)
         {
            requestCallback(_pluginAPIs[n]);
            return;
         }
      }

      // If we didn't find one then store the request for later.
      Plugins::PluginAPIRequest req;
      dStrcpy(req.pluginName, name);
      req.requestCallback = requestCallback;
      _pluginAPIRequests.push_back(req);
   }

// ----------------------------------------
//  Plugin Class
// ----------------------------------------

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
      
      StringTableEntry dirName = Platform::stripFileName(mPath);
      StringTableEntry fileName = Platform::stripDirectory(mPath);
      
      mHInst = openLibrary(fileName, dirName);
      if ( !mHInst ) return false;

      // Register plugin console classes
      for(S32 n = 0; n < _pluginConsoleClasses.size(); ++n)
         _pluginConsoleClasses[n]->registerClass();
      _pluginConsoleClasses.clear();

      // Create/Destroy function pointers.
      _create  = (Plugin::createFunc)getLibraryFunc(mHInst, "create");
      _destroy = (Plugin::destroyFunc)getLibraryFunc(mHInst, "destroy");

      // Call the create function and establish the 
      if ( _create )
         _create();

      // Tick Functions
      _interpolateTick  = (Plugin::interpolateTickFunc)getLibraryFunc(mHInst, "interpolateTick");
      _processTick      = (Plugin::processTickFunc)getLibraryFunc(mHInst, "processTick");
      _advanceTime      = (Plugin::advanceTimeFunc)getLibraryFunc(mHInst, "advanceTime");
      setProcessTicks(_interpolateTick || _processTick || _advanceTime);

      // Render Functions
      //_preRender  = (Plugin::preRenderFunc)getLibraryFunc(mHInst, "preRender");
      //_render     = (Plugin::renderFunc)getLibraryFunc(mHInst, "render");
      //_postRender = (Plugin::postRenderFunc)getLibraryFunc(mHInst, "postRender");
      //setRendering(_preRender || _render || _postRender);

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
}      
