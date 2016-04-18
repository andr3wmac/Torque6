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

#ifndef _BASE_COMPONENT_H_
#define _BASE_COMPONENT_H_

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _VERTEXLAYOUTS_H_
#include "graphics/core.h"
#endif

#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif

#ifndef _SHADERS_H_
#include "graphics/shaders.h"
#endif

#ifndef _MESH_ASSET_H_
#include "mesh/meshAsset.h"
#endif

#ifndef _RENDERING_H_
#include "rendering/rendering.h"
#endif

#ifndef _SCENE_OBJECT_H_
#include "scene/object.h"
#endif

class NetConnection;

namespace Scene 
{
   class SceneObject;

   class DLL_PUBLIC BaseComponent : public SimObject
   {
      private:
         typedef SimObject Parent;

      protected:
         MatrixF     mTransformMatrix;
         Point3F     mWorldPosition;
         Box3F       mBoundingBox;

         // Any component that uses a shader will include these uniforms.
         Rendering::UniformSet mUniforms;

      public:
         Transform            mTransform;
         Scene::SceneObject*  mOwnerObject;
         const char*          mTypeString;

         BaseComponent();

         void setUniformVec4(const char* name, Point4F value);

         // Scene Events.
         virtual void onAddToScene()      {}
         virtual void onRemoveFromScene() {}
         virtual void onScenePlay()       {}
         virtual void onScenePause()      {}
         virtual void onSceneStop()       {}

         virtual void setOwnerObject( Scene::SceneObject* owner ) { mOwnerObject = owner; }
         virtual bool raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint);
         virtual void refresh();

         virtual Box3F getBoundingBox();

         virtual MatrixF getTransform()                  { return mTransformMatrix; }
         virtual Point3F getWorldPosition()              { return mWorldPosition; }
         virtual void    setWorldPosition(Point3F pos)   { mWorldPosition = pos; }

         // Ticking passed down from Owner
         virtual void processMove( const Move *move ) { }
         virtual void interpolateMove( F32 delta )    { }
         virtual void advanceMove( F32 dt )           { }

         // Networking passed down from Owner
         virtual void writePacketData(GameConnection *conn, BitStream *stream)      { }
         virtual void readPacketData (GameConnection *conn, BitStream *stream)      { }
         virtual U32  packUpdate(NetConnection* conn, U32 mask, BitStream* stream)  { return 0; }
         virtual void unpackUpdate(NetConnection* conn, BitStream* stream)          { }

         static void initPersistFields();

         static bool setPositionFn(void* obj, const char* data);
         static const char* getPositionFn(void* obj, const char* data);
         static bool setRotationFn(void* obj, const char* data);
         static const char* getRotationFn(void* obj, const char* data);
         static bool setScaleFn(void* obj, const char* data);
         static const char* getScaleFn(void* obj, const char* data);

         DECLARE_CONOBJECT(BaseComponent);
   };
}

#endif _BASE_COMPONENT_H_