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

#include "entity.h"
#include "console/consoleInternal.h"
#include "components/baseComponent.h"
#include "game/moveList.h"

#include <bx/fpumath.h>

#include "entity_Binding.h"

namespace Scene
{
   IMPLEMENT_CO_NETOBJECT_V1(SceneEntity);

   SceneEntity::SceneEntity()
   {
      mGhosted = false;
      mNetFlags.set( Ghostable | ScopeAlways );

      mTemplateAssetID = StringTable->EmptyString;
      mTemplate = NULL;
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   SceneEntity::~SceneEntity()
   {
      if ( mTemplate != NULL )
         mTemplate->deleteObject();
   }

   void SceneEntity::initPersistFields()
   {
      Parent::initPersistFields();

      addGroup("SceneEntity");
         addProtectedField("Template", TypeAssetId, Offset(mTemplateAssetID, SceneEntity), &setTemplateAsset, &defaultProtectedGetFn, "");
         addField("Position", TypePoint3F, Offset(mPosition, SceneEntity), "");
         addField("Rotation", TypePoint3F, Offset(mRotation, SceneEntity), "");
         addField("Scale", TypePoint3F, Offset(mScale, SceneEntity), "");
      endGroup("SceneEntity");

      addGroup("SceneEntity: Networking");
         addProtectedField("Ghosted", TypeBool, Offset(mGhosted, SceneEntity), &setGhosted, &defaultProtectedGetFn, "");
      endGroup("SceneEntity: Networking");
   }

   bool SceneEntity::onAdd()
   {
      if(!Parent::onAdd())
         return false;

      return true;
   }

   void SceneEntity::onRemove()
   {
      Parent::onRemove();
   }

   void SceneEntity::onGroupAdd()
   {
      for (S32 n = 0; n < mComponents.size(); ++n)
      {
         mComponents[n]->setOwnerEntity(this);
         mComponents[n]->onAddToScene();
      }

      refresh();
   }

   void SceneEntity::onGroupRemove()
   {
      for (S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->onRemoveFromScene();
   }

   void SceneEntity::setGhosted( bool _value )
   {
      if ( _value ) 
      {
         mGhosted = true;
         mNetFlags.set( Ghostable | ScopeAlways );
      } else {
         mGhosted = false;
         mNetFlags.set( Ghostable | ScopeAlways );
      }

      if ( isServerObject() )
         setMaskBits(GhostedMask);
   }

   void SceneEntity::addComponent(BaseComponent* component)
   {
      mComponents.push_back(component);
   }

   void SceneEntity::removeComponent(BaseComponent* component)
   {
      //
   }

   void SceneEntity::clearComponents()
   {
      mComponents.clear();
   }


   void SceneEntity::setTemplateAsset( StringTableEntry assetID )
   {
      if ( mTemplate != NULL )
         return;

      mTemplateAssetID = StringTable->insert(assetID);
      AssetPtr<EntityTemplateAsset> templateAsset;
      templateAsset.setAssetId(mTemplateAssetID);
      if ( !templateAsset.isNull() )
      {
         mTemplate = templateAsset->getInstance();

         // We keep a vector of BaseComponent pointers for quick access.
         clearComponents();
         for(S32 n = 0; n < mTemplate->size(); ++n)
         {
            BaseComponent* component = static_cast<BaseComponent*>(mTemplate->at(n));
            addComponent(component);
         }
      }

      if ( isServerObject() )
         setMaskBits(TemplateMask);
   }

   void SceneEntity::refresh()
   {
      // Calculate bounding box based on component bounding boxes.
      Box3F newBoundingBox;
      newBoundingBox.set(Point3F(0, 0, 0));
      for (S32 n = 0; n < mComponents.size(); ++n)
      {
         if ( n == 0 )
            newBoundingBox = mComponents[n]->getBoundingBox();
         else
            newBoundingBox.intersect(mComponents[n]->getBoundingBox());
      }
      mBoundingBox = newBoundingBox;

      // Refresh components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->refresh();

      //if ( isServerObject() )
      //   setMaskBits(TransformMask);
   }

   SimObject* SceneEntity::findComponentByType(const char* pType)
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
      {
         if (  dStrcmp(mComponents[n]->mTypeString, pType) == 0 )
            return mComponents[n];
      }

      return NULL;
   }

   void SceneEntity::writePacketData(GameConnection *conn, BitStream *stream)
   {
      // Components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->writePacketData(conn, stream);
   }

   void SceneEntity::readPacketData(GameConnection *conn, BitStream *stream)
   {
      // Components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->readPacketData(conn, stream);
   }

   U32 SceneEntity::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
   {
      // Ghosted Update.
      if ( stream->writeFlag(GhostedMask && mask) )
         stream->writeFlag(mGhosted);

      // If we're not ghosted we don't send anything else.
      if ( !mGhosted )
         return 0;

      // Template Update.
      if ( stream->writeFlag(TemplateMask && mask) ) 
         stream->writeString(mTemplateAssetID);

      // Transform Update.
      if ( stream->writeFlag(TransformMask && mask) )
      {
         stream->writePoint3F(mPosition);
         stream->writePoint3F(mRotation);
         stream->writePoint3F(mScale);
      }

      // Components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->packUpdate(conn, mask, stream);

      return 0;
   }

   void SceneEntity::unpackUpdate(NetConnection* conn, BitStream* stream)
   {
      // Ghosted Update.
      if ( stream->readFlag() )
         setGhosted(stream->readFlag());

      if ( !mGhosted )
         return;

      // Template Update.
      if ( stream->readFlag() )
      {
         char strBuf[256];
         stream->readString(strBuf);
         setTemplateAsset(strBuf);
      }

      // Transform Update.
      if ( stream->readFlag() )
      {
         stream->readPoint3F(&mPosition);
         stream->readPoint3F(&mRotation);
         stream->readPoint3F(&mScale);
         refresh();
      }

      // Components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->unpackUpdate(conn, stream);
   }

   void SceneEntity::processMove( const Move *move )
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->processMove(move);
   }

   void SceneEntity::interpolateMove( F32 delta )
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->interpolateMove(delta);
   }

   void SceneEntity::advanceMove( F32 dt )
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->advanceMove(dt);
   }
}