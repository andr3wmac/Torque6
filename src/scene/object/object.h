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

#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _GAMEOBJECT_H_
#include "game/GameObject.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _OBJECT_TEMPLATE_H_
#include "scene/object/objectTemplate.h"
#endif

#ifndef _OBJECT_TEMPLATE_ASSET_H_
#include "scene/object/objectTemplateAsset.h"
#endif

namespace Scene 
{
   class BaseComponent;

   class DLL_PUBLIC SceneObject : public GameObject
   {
      private:
         typedef SimObject Parent;
         bool mAddedToScene;

      public:
         SceneObject();
         ~SceneObject();

         // For now, these are public. 
         ObjectTemplate*         mTemplate;
         StringTableEntry        mTemplateAssetID;
         Vector<BaseComponent*>  mComponents;

         Box3F    mBoundingBox;
         F32      mTransformMatrix[16];
         Point3F  mPosition;
         Point3F  mRotation;
         Point3F  mScale;

         // GameObject
         virtual void processMove( const Move *move );
         virtual void interpolateMove( F32 delta );
         virtual void advanceMove( F32 dt );

         bool raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint);
         virtual void refresh();

         static void initPersistFields();

         virtual bool onAdd();
         virtual void onRemove();
         virtual void onGroupAdd();
         virtual void onGroupRemove();

         DECLARE_CONOBJECT(SceneObject);

         SimObject* findComponentByType(const char* pType);
         SimObject* findComponent(StringTableEntry internalName) { return mTemplate->findObjectByInternalName(internalName); }

      protected:
         virtual void onTamlCustomWrite(TamlCustomNodes& customNodes);
         virtual void onTamlCustomRead(const TamlCustomNodes& customNodes);
         static bool setTemplateAsset( void* obj, const char* data ) { static_cast<SceneObject*>(obj)->setTemplateAsset(data); return false; }

      // Networking
      public:
         bool mGhosted;

         enum SceneObjectMasks
         {
            GhostedMask       = BIT( 0 ),
            TemplateMask      = BIT( 1 ),
            TransformMask     = BIT( 2 ),
            ComponentMask     = BIT( 3 ),
            NextFreeMask      = BIT( 4 )
         };

         void writePacketData(GameConnection *conn, BitStream *stream);
         void readPacketData (GameConnection *conn, BitStream *stream);
         U32  packUpdate(NetConnection* conn, U32 mask, BitStream* stream);
         void unpackUpdate(NetConnection* conn, BitStream* stream);
         void setGhosted(bool _value);
         void setTemplateAsset(StringTableEntry assetID);

         void addComponent(BaseComponent* component);
         void removeComponent(BaseComponent* component);
         void clearComponents();

      protected:
         static bool setGhosted(void* obj, const char* data) 
         { 
            bool value;
            Con::setData(TypeBool, &value, 0, 1, &data);
            static_cast<SceneObject*>(obj)->setGhosted(value);
            return false; 
         }
   };
}

#endif