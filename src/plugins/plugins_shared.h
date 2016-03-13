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
#define _PLUGINS_SHARED_H

#ifndef _CONSOLE_H_
#include <console/console.h>
#endif

#ifndef _RENDERING_H_
#include <rendering/rendering.h>
#endif

#ifndef _TEXTURE_OBJECT_H_
#include "graphics/TextureObject.h"
#endif

#ifndef _PROFILER_H_
#include "debug/profiler.h"
#endif

#ifndef NANOVG_H
#include <../common/nanovg/nanovg.h>
#endif

#ifndef _DEFERRED_SHADING_H_
#include "rendering/deferredShading/deferredShading.h"
#endif

// ----------------------------------------
//  Plugin Function Pointers
// ----------------------------------------
// A wrapper is created with function pointers for all the major
// engine functions. This file is shared between the engine and
// each plugin. PluginLink contains an instance of all the wrappers.
// There is a PluginLink variable in the Plugins namespace called Link.
// When the engine is initialized all the pointers in Link are defined.
// Link is defined as extern and implemented locally in the engine.
// When a plugin links against the DLL it will have access to Link and
// thus all the function pointers.
// A plugin can access engine functions with Torque::Function()

class MeshAsset;
class MaterialAsset;

namespace Scene
{
   class SceneCamera;
   class SceneObject;
}

namespace Plugins
{
   class PluginAPI;
   struct PluginAPIRequest;
}

namespace Torque
{
   struct EngineWrapper
   {
      Profiler* ProfilerLink;

      void (*mainLoop)();
      void (*resizeWindow)(int width, int height);
      void (*mouseMove)(int x, int y);
      void (*mouseButton)(bool down, bool left);
      void (*keyDown)(KeyCodes key);
      void (*keyUp)(KeyCodes key);
   };

   struct PlatformWrapper
   {
      StringTableEntry (*stripBasePath)(const char *path);
      char* (*makeFullPathName)(const char *path, char *buffer, U32 size, const char *cwd); // Defaults: cwd = NULL
      StringTableEntry (*makeRelativePathName)(const char *path, const char *to);
      bool (*pathCopy)(const char *fromName, const char *toName, bool nooverwrite);
      bool (*createPath)(const char *path);
   };

   struct ConsoleWrapper
   {
      void (*printf)(const char *_format, ...);
      void (*warnf)(const char *_format, ...);
      void (*errorf)(const char *_format, ...);

      const char* (*evaluate)(const char* string, bool echo, const char *fileName); // Defaults: echo = false, fileName = NULL
      const char* (*evaluatef)(const char* string, ...);

      void (*addCommandA)(const char *nsName, const char *name, StringCallback cb, const char *usage, S32 minArgs, S32 maxArgs);
      void (*addCommandB)(const char *nsName, const char *name, IntCallback cb, const char *usage, S32 minArgs, S32 maxArgs);
      void (*addCommandC)(const char *nsName, const char *name, FloatCallback cb, const char *usage, S32 minArgs, S32 maxArgs);
      void (*addCommandD)(const char *nsName, const char *name, VoidCallback cb, const char *usage, S32 minArgs, S32 maxArgs);
      void (*addCommandE)(const char *nsName, const char *name, BoolCallback cb, const char *usage, S32 minArgs, S32 maxArgs);

      void (*markCommandGroup)(const char * nsName, const char *name, const char* usage); // Defaults: usage = NULL
      void (*beginCommandGroup)(const char * nsName, const char *name, const char* usage);
      void (*endCommandGroup)(const char * nsName, const char *name);
      void (*addOverload)(const char * nsName, const char *name, const char *altUsage);

      void (*setData)(S32 type, void *dptr, S32 index, S32 argc, const char **argv, EnumTable *tbl, BitSet32 flag);
      const char* (*getData)(S32 type, void *dptr, S32 index, EnumTable *tbl, BitSet32 flag); // Defaults: *tbl = NULL, flag = 0
      Namespace* (*getNamespaceList)();
      Namespace* (*lookupNamespace)(const char *ns);
      bool (*classLinkNamespaces)(Namespace *parent, Namespace *child);
      void (*registerClassRep)(AbstractClassRep* in_pRep);
      ConsoleObject* (*createObject)(StringTableEntry typeName);

      S32 TypeF32;
      S32 TypeS8;
      S32 TypeS32;
      S32 TypeS32Vector;
      S32 TypeBool;
      S32 TypeBoolVector;
      S32 TypeF32Vector;
      S32 TypeString;
      S32 TypeStringTableEntryVector;
      S32 TypeCaseString;
      S32 TypeFilename;
      S32 TypeEnum;
      S32 TypeFlag;
      S32 TypeSimObjectPtr;
      S32 TypeSimObjectName;
      S32 TypeSimObjectId;
      S32 TypePoint3F;
      S32 TypeAssetLooseFilePath;
      S32 TypeColorF;
      S32 TypeColorI;
   };

   struct SysGUIWrapper
   {
      S32 (*beginScrollArea)(const char* title, U32 x, U32 y, U32 width, U32 height);
      S32 (*endScrollArea)();
      S32 (*label)(const char* label);
      S32 (*list)(const char* script, void (*callback)(S32 id)); // Defaults: script = "", callback = NULL
      S32 (*checkBox)(const char* label, bool value);
      S32 (*slider)(const char* label, S32 value, S32 min, S32 max);
      S32 (*textInput)(const char* label, const char* text);
      S32 (*button)(const char* label, const char* script, void (*callback)(S32 id)); // Defaults: script = "", callback = NULL
      S32 (*separator)();
      S32 (*beginCollapse)(const char* label, const char* text, bool open);
      S32 (*endCollapse)();
      S32 (*colorWheel)(const char* label, ColorF color);
      S32 (*vector3)(const char* label, Point3F vec, const char* script, void (*callback)(S32 id));
      S32 (*image)(bgfx::TextureHandle*, const char* script, void (*callback)(S32 id));

      void (*addListValue)(S32 id, const char* val, const char* script, void (*callback)(S32 id)); // Defaults: script = "", callback = NULL
      const char* (*getListValue)(S32 id, S32 index);
      S32 (*getListSelected)(S32 id);
      void (*clearList)(S32 id);

      void                 (*setElementHidden)(S32 id, bool val);
      char*                (*getLabelValue)(S32 id);
      void                 (*setLabelValue)(S32 id, const char* val);
      char*                (*getTextValue)(S32 id);
      void                 (*setTextValue)(S32 id, const char* val);
      S32                  (*getIntValue)(S32 id);
      void                 (*setIntValue)(S32 id, S32 val);
      bool                 (*getBoolValue)(S32 id);
      void                 (*setBoolValue)(S32 id, bool val);
      ColorF               (*getColorValue)(S32 id);
      void                 (*setColorValue)(S32 id, ColorF val);
      Point3F              (*getVector3Value)(S32 id);
      void                 (*setVector3Value)(S32 id, Point3F val);
      bgfx::TextureHandle  (*getImageValue)(S32 id);
      void                 (*setImageValue)(S32 id, bgfx::TextureHandle val);

      void  (*alignLeft)(S32 id);
      void  (*alignRight)(S32 id);
      void  (*alignTop)(S32 id);
      void  (*alignBottom)(S32 id);

      void (*clearScrollArea)(S32 id);
      void (*seek)(S32 id);
      void (*clearSeek)();
   };

   struct NanoVGWrapper
   {
      void (*nvgSave)(NVGcontext* ctx);
      void (*nvgRestore)(NVGcontext* ctx);
      void (*nvgReset)(NVGcontext* ctx);

      NVGcolor (*nvgRGBA)(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
      NVGcolor (*nvgRGBAf)(float r, float g, float b, float a);

      void (*nvgBeginPath)(NVGcontext* ctx);
      void (*nvgMoveTo)(NVGcontext* ctx, float x, float y);
      void (*nvgBezierTo)(NVGcontext* ctx, float c1x, float c1y, float c2x, float c2y, float x, float y);

      void (*nvgCircle)(NVGcontext* ctx, float cx, float cy, float r);
      void (*nvgRect)(NVGcontext* ctx, float x, float y, float w, float h);
      void (*nvgRoundedRect)(NVGcontext* ctx, float x, float y, float w, float h, float r);

      NVGpaint (*nvgLinearGradient)(NVGcontext* ctx, float sx, float sy, float ex, float ey, NVGcolor icol, NVGcolor ocol);

      void (*nvgFill)(NVGcontext* ctx);
      void (*nvgFillColor)(NVGcontext* ctx, NVGcolor color);
      void (*nvgFillPaint)(NVGcontext* ctx, NVGpaint paint);

      void (*nvgStroke)(NVGcontext* ctx);
      void (*nvgStrokeColor)(NVGcontext* ctx, NVGcolor color);
      void (*nvgStrokeWidth)(NVGcontext* ctx, float size);

      void (*nvgFontFace)(NVGcontext* ctx, const char* font);
      void (*nvgFontSize)(NVGcontext* ctx, float size);
      float (*nvgText)(NVGcontext* ctx, float x, float y, const char* string, const char* end);
      void (*nvgTextAlign)(NVGcontext* ctx, int align);

      NVGpaint (*nvgImagePattern)(NVGcontext* ctx, float ox, float oy, float ex, float ey, float angle, int image, float alpha);
      void (*nvgImageSize)(NVGcontext* ctx, int image, int* w, int* h);
   };

   struct SceneWrapper
   {
      SimGroup* (*getSceneGroup)();
      Scene::SceneObject* (*raycast)(const Point3F& start, const Point3F& end);

      void (*addObject)(Scene::SceneObject* obj, const char* name); // Defaults: name = "SceneObject"
      void (*deleteObject)(Scene::SceneObject* obj);
      void (*removeObject)(Scene::SceneObject* obj);

      MaterialAsset* (*getMaterialAsset)(const char* id);
      MeshAsset* (*getMeshAsset)(const char* id);
      void (*createMeshAsset)(const char* name, const char* meshFile, const char* savePath);
      void (*createTextureAsset)(const char* name, const char* textureFile, const char* savePath);
      void (*createMaterialAsset)(const char* name, const char* templateFile, const char* savePath);
      void (*createMaterialTemplate)(const char* savePath);

      void (*clear)();
      void (*append)(const char* filename);
      void (*load)(const char* filename, bool append); // Defaults: append = false
      void (*save)(const char* filename);
      void (*refresh)();
   };

   struct PhysicsWrapper
   {
      void (*pause)();
      void (*resume)();
   };

   struct RenderingWrapper
   {
      bool*                         canvasSizeChanged;
      U32*                          canvasHeight; 
      U32*                          canvasWidth;
      Rendering::DirectionalLight*  directionalLight;

      Point2I (*worldToScreen)(Point3F worldPos);
      Point3F (*screenToWorld)(Point2I screenPos);
      bool (*closestPointsOnTwoLines)(Point3F& closestPointLine1, Point3F& closestPointLine2, Point3F linePoint1, Point3F lineVec1, Point3F linePoint2, Point3F lineVec2);
      Rendering::RenderData* (*createRenderData)();
      Vector<Rendering::LightData*> (*getLightList)();

      void (*addRenderHook)(Rendering::RenderHook* hook);
      bool (*removeRenderHook)(Rendering::RenderHook* hook);

      Rendering::RenderCamera* (*getActiveCamera)();
   };

   struct GraphicsWrapper
   {
      bgfx::VertexDecl* PosUVNormalVertex;
      bgfx::VertexDecl* PosUVColorVertex;

      bgfx::IndexBufferHandle* cubeIB;
      bgfx::VertexBufferHandle* cubeVB;

      TextureObject* (*loadTexture)(const char* pTextureKey, TextureHandle::TextureHandleType type, U32 flags, bool checkOnly, bool force16Bit );
      bgfx::UniformHandle (*getTextureUniform)(U32 slot);
      bgfx::UniformHandle (*getUniformVec4)(const char* name, U32 count);
      bgfx::UniformHandle (*getUniformMat4)(const char* name, U32 count);
      Graphics::Shader* (*getShader)(const char* vertexShaderPath, const char* fragmentShaderPath, bool forceRecompile, bool monitorFile); // Defaults: forceRecompile = false, monitorFile = true
      Graphics::Shader* (*getDefaultShader)(const char* vertexShaderPath, const char* fragmentShaderPath, bool forceRecompile, bool monitorFile); // Defaults: forceRecompile = false, monitorFile = true
      Graphics::ShaderAsset* (*getShaderAsset)(const char* id);

      void (*fullScreenQuad)(F32 _textureWidth, F32 _textureHeight, F32 _z); // Defaults: _z = 0.0f
      void (*screenSpaceQuad)(F32 _x, F32 _y, F32 _width, F32 _height, F32 _targetWidth, F32 _targetHeight);
      void (*dglScreenQuad)(U32 _x, U32 _y, U32 _width, U32 _height);
      void (*drawLine3D)(U8 viewID, Point3F start, Point3F end, ColorI color, F32* transform); // Defaults: transform = NULL
      void (*drawBox3D)(U8 viewID, Box3F box, ColorI color, F32* transform); // Defaults: transform = NULL
      void (*drawCircle3D)(U8 viewID, Point3F position, F32 radius, U32 segments, ColorI color, F32* transform); // Defaults: transform = NULL
      void (*drawCone3D)(U8 viewID, Point3F position, F32 length, F32 radius, U32 segments, ColorI baseColor, ColorI tipColor, F32* transform); // Defaults: transform = NULL
      void (*drawPlane3D)(U8 viewID, Point3F position, F32 width, F32 height, ColorI color, F32* transform); // Defaults: transform = NULL
      void (*drawBillboard)(U8 viewID, TextureObject* texture, Point3F position, F32 width, F32 height, ColorI color, F32* transform); // Defaults: transform = NULL
      NVGcontext* (*dglGetNVGContext)();

      void (*dglClearBitmapModulation)();
      void (*dglDrawBitmap)(TextureObject* texture, const Point2I& in_rAt, const U32 in_flip); // Defaults: in_flip = 0

      Graphics::ViewTableEntry* (*getView)(const char* name, S32 priority, Rendering::RenderCamera* camera); // Defaults: camera = NULL
      Graphics::ViewTableEntry* (*getTemporaryView)(const char* name, S32 priority, Rendering::RenderCamera* camera); // Default: camera = NULL
   };

   struct AssetDatabaseWrapper
   {
      S32 (*findAssetType)( AssetQuery* pAssetQuery, const char* pAssetType, const bool assetQueryAsSource ); // Defaults: assetQueryAsSource = false
      Vector<const AssetDefinition*> (*getDeclaredAssets)();
      bool (*addDeclaredAsset)(ModuleDefinition* pModuleDefinition, const char* pAssetFilePath);
      AssetBase* (*getAssetBase)(const char* assetID);
   };

   struct BGFXWrapper
   {
      void (*dbgTextClear)(uint8_t _attr, bool _small); // Defaults: _attr = 0, _small = false
	   void (*dbgTextPrintf)(uint16_t _x, uint16_t _y, uint8_t _attr, const char* _format, ...);

      void (*setViewClear)(uint8_t _id, uint16_t _flags, uint32_t _rgba, float _depth, uint8_t _stencil); // Defaults: _depth = 1.0, _stencil = 0
      void (*setViewRect)(uint8_t _id, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height);
      void (*setViewTransform)(uint8_t _id, const void* _view, const void* _projL, uint8_t _flags, const void* _projR); // Defaults: _flags = BGFX_VIEW_STEREO, _projR = NULL

      uint32_t (*setTransform)(const void* _mtx, uint16_t _num);
      void (*setTexture)(uint8_t _stage, bgfx::UniformHandle _sampler, bgfx::TextureHandle _handle, uint32_t _flags); // Defaults: _flags = UINT32_MAX
      void (*setTextureA)(uint8_t _stage, bgfx::UniformHandle _sampler, bgfx::FrameBufferHandle _handle, uint8_t _attachment, uint32_t _flags); // Defaults: _attachment = 0, _flags = UINT32_MAX
      void (*setState)(uint64_t _state, uint32_t _rgba); // Defaults: _rgba = 0
      void (*setUniform)(bgfx::UniformHandle _handle, const void* _value, uint16_t _num); // Defaults: _num = 1

      uint32_t (*touch)(uint8_t _id);
      uint32_t (*submit)(uint8_t _id, bgfx::ProgramHandle _handle, int32_t _depth, bool _preserveState); // Defaults: _depth = 0, _preserveState = false
      void (*blit)(uint8_t _id, bgfx::TextureHandle _dst, uint16_t _dstX, uint16_t _dstY, bgfx::TextureHandle _src, uint16_t _srcX, uint16_t _srcY, uint16_t _width, uint16_t _height); // Defaults: _srcX = 0, _srcY = 0, _width = UINT16_MAX, _height = UINT16_MAX
      void (*blitA)(uint8_t _id, bgfx::TextureHandle _dst, uint16_t _dstX, uint16_t _dstY, bgfx::FrameBufferHandle _src, uint8_t _attachment, uint16_t _srcX, uint16_t _srcY, uint16_t _width, uint16_t _height); // Defaults: _attachment = 0, _srcX = 0, _srcY = 0, _width = UINT16_MAX, _height = UINT16_MAX
      void (*blitB)(uint8_t _id, bgfx::TextureHandle _dst, uint8_t _dstMip, uint16_t _dstX, uint16_t _dstY, uint16_t _dstZ, bgfx::TextureHandle _src, uint8_t _srcMip, uint16_t _srcX, uint16_t _srcY, uint16_t _srcZ, uint16_t _width, uint16_t _height, uint16_t _depth); // Defaults: _srcMip = 0, _srcX = 0, _srcY = 0, _srcZ = 0, _width = UINT16_MAX, _height = UINT16_MAX, _depth = UINT16_MAX
      void (*blitC)(uint8_t _id, bgfx::TextureHandle _dst, uint8_t _dstMip, uint16_t _dstX, uint16_t _dstY, uint16_t _dstZ, bgfx::FrameBufferHandle _src, uint8_t _attachment, uint8_t _srcMip, uint16_t _srcX, uint16_t _srcY, uint16_t _srcZ, uint16_t _width, uint16_t _height, uint16_t _depth); // Defaults: _attachment = 0, _srcMip = 0, _srcX = 0, _srcY = 0, _srcZ = 0, _width = UINT16_MAX, _height = UINT16_MAX, _depth = UINT16_MAX
      const bgfx::Memory* (*makeRef)(const void* _data, uint32_t _size, bgfx::ReleaseFn _releaseFn, void* _userData); // Defaults: _releaseFn = NULL, _userData = NULL

   	bgfx::IndexBufferHandle (*createIndexBuffer)(const bgfx::Memory* _mem, uint16_t _flags); // Defaults: _flags = BGFX_BUFFER_NONE
	   void (*destroyIndexBuffer)(bgfx::IndexBufferHandle _handle);

      bgfx::DynamicIndexBufferHandle (*createDynamicIndexBuffer)(const bgfx::Memory* _mem, uint16_t _flags); // Defaults: _flags = BGFX_BUFFER_NONE
      void (*updateDynamicIndexBuffer)(bgfx::DynamicIndexBufferHandle _handle, uint32_t _startIndex, const bgfx::Memory* _mem);
      void (*destroyDynamicIndexBuffer)(bgfx::DynamicIndexBufferHandle _handle);

	   bgfx::VertexBufferHandle (*createVertexBuffer)(const bgfx::Memory* _mem, const bgfx::VertexDecl& _decl, uint16_t _flags); // Defaults: _flags = BGFX_BUFFER_NONE
	   void (*destroyVertexBuffer)(bgfx::VertexBufferHandle _handle);

      bgfx::DynamicVertexBufferHandle (*createDynamicVertexBuffer)(const bgfx::Memory* _mem, const bgfx::VertexDecl& _decl, uint16_t _flags); // Defaults: _flags = BGFX_BUFFER_NONE
      void (*updateDynamicVertexBuffer)(bgfx::DynamicVertexBufferHandle _handle, uint32_t _startVertex, const bgfx::Memory* _mem);
      void (*destroyDynamicVertexBuffer)(bgfx::DynamicVertexBufferHandle _handle);

      bgfx::FrameBufferHandle (*createFrameBuffer)(uint8_t _num, const bgfx::TextureHandle* _handles, bool _destroyTextures); // Defaults: _destroyTextures = false
      void (*destroyFrameBuffer)(bgfx::FrameBufferHandle _handle);

      bgfx::TextureHandle (*createTexture2D)(uint16_t _width, uint16_t _height, uint8_t _numMips, bgfx::TextureFormat::Enum _format, uint32_t _flags, const bgfx::Memory* _mem); // Defaults: _flags = BGFX_TEXTURE_NONE, _mem = NULL
      bgfx::TextureHandle (*createTextureCube)(uint16_t _size, uint8_t _numMips, bgfx::TextureFormat::Enum _format, uint32_t _flags, const bgfx::Memory* _mem); // Defaults: _flags  = BGFX_TEXTURE_NONE, _mem = NULL
      void (*updateTexture2D)(bgfx::TextureHandle _handle, uint8_t _mip, uint16_t _x, uint16_t _y, uint16_t _width, uint16_t _height, const bgfx::Memory* _mem, uint16_t _pitch); // Defaults: _pitch = UINT16_MAX
      void (*destroyTexture)(bgfx::TextureHandle _handle);

      void (*setViewFrameBuffer)(uint8_t _id, bgfx::FrameBufferHandle _handle);

      const bgfx::Memory* (*alloc)(uint32_t _size);
      const bgfx::Memory* (*copy)(const void* _data, uint32_t _size);
   };

   struct PluginsWrapper
   {
      void(*addPluginAPI)(Plugins::PluginAPI* api);
      void (*requestPluginAPI)(const char* name, void (*requestCallback)(Plugins::PluginAPI* api));
   };

   extern DLL_PUBLIC PluginsWrapper         Plugins;
   extern DLL_PUBLIC EngineWrapper          Engine;
   extern DLL_PUBLIC PlatformWrapper        Platform;
   extern DLL_PUBLIC ConsoleWrapper         Con;
   extern DLL_PUBLIC SysGUIWrapper          SysGUI;
   extern DLL_PUBLIC NanoVGWrapper          NanoVG;
   extern DLL_PUBLIC SceneWrapper           Scene;
   extern DLL_PUBLIC PhysicsWrapper         Physics;
   extern DLL_PUBLIC RenderingWrapper       Rendering;
   extern DLL_PUBLIC GraphicsWrapper        Graphics;
   extern DLL_PUBLIC AssetDatabaseWrapper   AssetDatabaseLink;
   extern DLL_PUBLIC BGFXWrapper            bgfx;
      
   extern DLL_PUBLIC ModuleManager*         ModuleDatabaseLink;
   extern DLL_PUBLIC _StringTable*          StringTableLink;
   extern DLL_PUBLIC ResManager*            ResourceManager;
}

// ----------------------------------------
//  Plugin Macros
// ----------------------------------------
//   Engine Side:
//
//     - PLUGIN_FUNC_PTR:
//         Defines a function pointer that can optionally be filled by a plugin.
//         Example: PLUGIN_FUNC_PTR(interpolateTick, F32 delta)
//
//     - IMPLEMENT_PLUGIN_CONOBJECT(className):
//         Plugin equivilant of IMPLEMENT_CONOBJECT
//
//     - DECLARE_PLUGIN_CONOBJECT(className):
//         Plugin equivilant of DECLARE_CONOBJECT
//
//   Plugin Side:
//
//     - PLUGIN_FUNC:
//         Opposite to PLUGIN_FUNC_PTR. Defines the function on the plugin side.
//         Example: PLUGIN_FUNC(interpolateTick, F32 delta)

namespace Plugins
{
   extern DLL_PUBLIC Vector<AbstractClassRep*> _pluginConsoleClasses;
}

#define PLUGIN_FUNC_PTR(name, ...) \
   typedef void (*name##Func)(__VA_ARGS__); \
   name##Func _##name;

#ifdef __GNUC__
   #define PLUGIN_FUNC(name, ...) \
   extern "C" { __attribute__ ((dllexport)) void name (__VA_ARGS__); }
#else
   #define PLUGIN_FUNC(name, ...) \
   extern "C" { __declspec(dllexport) void name##(__VA_ARGS__); }
#endif

#define IMPLEMENT_PLUGIN_CONOBJECT(className)                                                                       \
    AbstractClassRep* className::getClassRep() const { return &className::dynClassRep; }                            \
    AbstractClassRep* className::getStaticClassRep() { return &dynClassRep; }                                       \
    AbstractClassRep* className::getParentStaticClassRep() { return Parent::getStaticClassRep(); }                  \
    AbstractClassRep* className::getContainerChildStaticClassRep() { return NULL; }                                 \
    AbstractClassRep::WriteCustomTamlSchema className::getStaticWriteCustomTamlSchema() { return NULL; }            \
    className::className##Rep className::dynClassRep(#className, 0, -1, 0, className::getParentStaticClassRep())

#define DECLARE_PLUGIN_CONOBJECT(className)                                                                                         \
    class className##Rep;                                                                                                           \
    static className##Rep dynClassRep;                                                                                              \
    static AbstractClassRep* getParentStaticClassRep();                                                                             \
    static AbstractClassRep* getContainerChildStaticClassRep();                                                                     \
    static AbstractClassRep* getStaticClassRep();                                                                                   \
    static AbstractClassRep::WriteCustomTamlSchema getStaticWriteCustomTamlSchema();                                                \
    virtual AbstractClassRep* getClassRep() const;                                                                                  \
    class className##Rep : public AbstractClassRep {                                                                                \
      public:                                                                                                                       \
         className##Rep(const char *name, S32 netClassGroupMask, S32 netClassType, S32 netEventDir, AbstractClassRep *parent) {     \
            mClassName = name;                                                                                                      \
            for (U32 i = 0; i < NetClassGroupsCount; i++) mClassId[i] = -1;                                                         \
            mClassType = netClassType;                                                                                              \
            mClassGroupMask = netClassGroupMask;                                                                                    \
            mNetEventDir = netEventDir;                                                                                             \
            parentClass = parent;                                                                                                   \
            Plugins::_pluginConsoleClasses.push_back(this);                                                                         \
      }                                                                                                                             \
      void registerClass() {                                                                                                        \
         Torque::Con.registerClassRep( this );                                                                                \
         mNamespace = Torque::Con.lookupNamespace( Torque::StringTableLink->insert( getClassName() ) );                 \
         mNamespace->mClassRep = this;                                                                                              \
         sg_tempFieldList.setSize(0);                                                                                               \
         init();                                                                                                                    \
         if ( sg_tempFieldList.size() != 0 )                                                                                        \
            mFieldList = sg_tempFieldList;                                                                                          \
         sg_tempFieldList.clear();                                                                                                  \
      }                                                                                                                             \
      virtual AbstractClassRep* getContainerChildClass(const bool recurse) {                                                        \
         AbstractClassRep* pChildren = className::getContainerChildStaticClassRep();                                                \
         if (!recurse || pChildren != NULL) return pChildren;                                                                       \
         AbstractClassRep* pParent = className::getParentStaticClassRep();                                                          \
         if (pParent == NULL) return NULL;                                                                                          \
         return pParent->getContainerChildClass(recurse);                                                                           \
      }                                                                                                                             \
      virtual WriteCustomTamlSchema getCustomTamlSchema(void) { return className::getStaticWriteCustomTamlSchema(); }               \
      void init() const {                                                                                                           \
         AbstractClassRep *parent = className::getParentStaticClassRep();                                                           \
         AbstractClassRep *child = className::getStaticClassRep();                                                                  \
         if (parent && child) Torque::Con.classLinkNamespaces(parent->getNameSpace(), child->getNameSpace());                 \
         className::initPersistFields();                                                                                            \
         className::consoleInit();                                                                                                  \
      }                                                                                                                             \
      ConsoleObject* create() const { return new className; }                                                                       \
   };                                                                                           

class PluginConsoleConstructor
{
public:
   StringCallback sc;   ///< A function/method that returns a string.
   IntCallback ic;      ///< A function/method that returns an int.
   FloatCallback fc;    ///< A function/method that returns a float.
   VoidCallback vc;     ///< A function/method that returns nothing.
   BoolCallback bc;     ///< A function/method that returns a bool.
   bool group;          ///< Indicates that this is a group marker.
   bool overload;       ///< Indicates that this is an overload marker.
   bool ns;             ///< Indicates that this is a namespace marker.

   S32 mina, maxa;
   const char *usage;         ///< Usage string.
   const char *funcName;      ///< Function name.
   const char *className;     ///< Class name.

   PluginConsoleConstructor()
   {
      sc = 0; 
      fc = 0; 
      vc = 0; 
      bc = 0; 
      ic = 0;
      group = false;
      ns = false;
   }

   void init(const char *cName, const char *fName, const char *usg, S32 minArgs, S32 maxArgs)
   {
      mina = minArgs;
      maxa = maxArgs;
      funcName = fName;
      usage = usg;
      className = cName;

      Torque::Con.printf("Adding Plugin Command: %s::%s", className, funcName);

      if (sc)
         Torque::Con.addCommandA(className, funcName, sc, usage, mina, maxa);
      else if (ic)
         Torque::Con.addCommandB(className, funcName, ic, usage, mina, maxa);
      else if (fc)
         Torque::Con.addCommandC(className, funcName, fc, usage, mina, maxa);
      else if (vc)
         Torque::Con.addCommandD(className, funcName, vc, usage, mina, maxa);
      else if (bc)
         Torque::Con.addCommandE(className, funcName, bc, usage, mina, maxa);
      else if (group)
         Torque::Con.markCommandGroup(className, funcName, usage);
      else if (overload)
         Torque::Con.addOverload(className, funcName, usage);
      else if (ns)
      {
         Namespace* foundNameSpace = Namespace::find(Torque::StringTableLink->insert(className));
         if (foundNameSpace)
            foundNameSpace->mUsage = usage;
      }
      else
         AssertAlwaysFatal("Found a ConsoleConstructor with an indeterminate type!");
   }

   PluginConsoleConstructor(const char *className, const char *funcName, StringCallback sfunc, const char* usage, S32 minArgs, S32 maxArgs)
   {
      sc = sfunc;
      init(className, funcName, usage, minArgs, maxArgs);
      
   }
   PluginConsoleConstructor(const char *className, const char *funcName, IntCallback    ifunc, const char* usage, S32 minArgs, S32 maxArgs)
   {
      ic = ifunc;
      init(className, funcName, usage, minArgs, maxArgs);
      
   }
   PluginConsoleConstructor(const char *className, const char *funcName, FloatCallback  ffunc, const char* usage, S32 minArgs, S32 maxArgs)
   {
      fc = ffunc;
      init(className, funcName, usage, minArgs, maxArgs);
   }
   PluginConsoleConstructor(const char *className, const char *funcName, VoidCallback   vfunc, const char* usage, S32 minArgs, S32 maxArgs)
   {
      vc = vfunc;
      init(className, funcName, usage, minArgs, maxArgs);
   }
   PluginConsoleConstructor(const char *className, const char *funcName, BoolCallback   bfunc, const char* usage, S32 minArgs, S32 maxArgs)
   {
      bc = bfunc;
      init(className, funcName, usage, minArgs, maxArgs);
      
   }
   PluginConsoleConstructor(const char *className, const char *groupName, const char* aUsage)
   {
      group = true;

      static char * lastUsage = NULL;
      if (aUsage)
         lastUsage = (char *)aUsage;

      usage = lastUsage;

      init(className, groupName, usage, -1, -2);
   }
   PluginConsoleConstructor(const char *className, const char *usage)
   {
      ns = true;
      init(className, NULL, usage, -1, -2);
   }
};

#  define PluginConsoleMethod(className,name,returnType,minArgs,maxArgs,usage1)                                                  \
      static inline returnType c##className##name(className *, S32, const char **argv);                                          \
      static returnType c##className##name##caster(SimObject *object, S32 argc, const char **argv) {                             \
         AssertFatal( dynamic_cast<className*>( object ), "Object passed to " #name " is not a " #className "!" );               \
         conmethod_return_##returnType ) c##className##name(static_cast<className*>(object),argc,argv);                          \
      };                                                                                                                         \
      static PluginConsoleConstructor className##name##obj(#className,#name,c##className##name##caster,usage1,minArgs,maxArgs);  \
      static inline returnType c##className##name(className *object, S32 argc, const char **argv)

#  define PluginConsoleMethodWithDocs(className,name,returnType,minArgs,maxArgs,argString)                                          \
      static inline returnType c##className##name(className *, S32, const char **argv);                                             \
      static returnType c##className##name##caster(SimObject *object, S32 argc, const char **argv) {                                \
         AssertFatal( dynamic_cast<className*>( object ), "Object passed to " #name " is not a " #className "!" );                  \
         conmethod_return_##returnType ) c##className##name(static_cast<className*>(object),argc,argv);                             \
      };                                                                                                                            \
	  static PluginConsoleConstructor className##name##obj(#className,#name,c##className##name##caster,#argString,minArgs,maxArgs);  \
      static inline returnType c##className##name(className *object, S32 argc, const char **argv)

#  define PluginConsoleStaticMethod(className,name,returnType,minArgs,maxArgs,usage1)                 \
      static inline returnType c##className##name(S32, const char **);                                \
      static returnType c##className##name##caster(SimObject *object, S32 argc, const char **argv) {  \
         conmethod_return_##returnType ) c##className##name(argc,argv);                               \
      };                                                                                              \
      static PluginConsoleConstructor                                                                 \
         className##name##obj(#className,#name,c##className##name##caster,usage1,minArgs,maxArgs);    \
      static inline returnType c##className##name(S32 argc, const char **argv)

#  define PluginConsoleStaticMethodWithDocs(className,name,returnType,minArgs,maxArgs,argString)      \
      static inline returnType c##className##name(S32, const char **);                                \
      static returnType c##className##name##caster(SimObject *object, S32 argc, const char **argv) {  \
         conmethod_return_##returnType ) c##className##name(argc,argv);                               \
      };                                                                                              \
      static PluginConsoleConstructor                                                                 \
	  className##name##obj(#className,#name,c##className##name##caster,#argString,minArgs,maxArgs);    \
      static inline returnType c##className##name(S32 argc, const char **argv)

#  define PluginConsoleMethodGroupEnd(className, groupName) \
      static PluginConsoleConstructor className##groupName##__GroupEnd(#className,#groupName,NULL);

#endif