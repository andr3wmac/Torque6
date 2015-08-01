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

#ifndef _SCENEENTITY_H_
#define _SCENEENTITY_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _GAMEOBJECT_H_
#include "game/GameObject.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _ENTITY_TEMPLATE_H_
#include "3d/entity/entityTemplate.h"
#endif

#ifndef _ENTITY_TEMPLATE_ASSET_H_
#include "3d/entity/entityTemplateAsset.h"
#endif

namespace Scene 
{
   class BaseComponent;

   class DLL_PUBLIC SceneEntity : public GameObject
   {
      private:
         typedef SimObject Parent;

      public:
         SceneEntity();
         ~SceneEntity();

         // For now, these are public. 
         EntityTemplate*         mTemplate;
         StringTableEntry        mTemplateAssetID;
         Vector<BaseComponent*>  mComponents;

         Box3F    mBoundingBox;
         Point3F  mPosition;
         Point3F  mRotation;
         Point3F  mScale;

         // GameObject
         virtual void processMove( const Move *move );
         virtual void interpolateMove( F32 delta );
         virtual void advanceMove( F32 dt );

         void refresh();

         static void initPersistFields();

         virtual bool onAdd();
         virtual void onRemove();
         virtual void onGroupAdd();
         virtual void onGroupRemove();

         DECLARE_CONOBJECT(SceneEntity);

         SimObject* findComponentByType(const char* pType);
         SimObject* findComponent(StringTableEntry internalName) { return mTemplate->findObjectByInternalName(internalName); }

      protected:
         static bool setTemplateAsset( void* obj, const char* data ) { static_cast<SceneEntity*>(obj)->setTemplateAsset(data); return false; }

      // Networking
      public:
         bool mGhosted;

         enum SceneEntityMasks
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

      protected:
         static bool setGhosted(void* obj, const char* data) { static_cast<SceneEntity*>(obj)->setGhosted(data); return false; }
   };
}

#endif