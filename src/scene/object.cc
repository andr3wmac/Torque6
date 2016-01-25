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

#include "object.h"
#include "console/consoleInternal.h"
#include "components/baseComponent.h"
#include "game/moveList.h"

#include <bx/fpumath.h>

#include "object_Binding.h"

namespace Scene
{
   IMPLEMENT_CO_NETOBJECT_V1(SceneObject);

   SceneObject::SceneObject()
      : mAddedToScene(false)
   {
      mGhosted = false;
      mNetFlags.set( Ghostable | ScopeAlways );

      mTemplateAssetID = StringTable->EmptyString;
      mTemplate = NULL;
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   SceneObject::~SceneObject()
   {
      if ( mTemplate != NULL )
         mTemplate->deleteObject();
   }

   void SceneObject::initPersistFields()
   {
      Parent::initPersistFields();

      addGroup("SceneObject");
         addProtectedField("Template", TypeAssetId, Offset(mTemplateAssetID, SceneObject), &setTemplateAsset, &defaultProtectedGetFn, "");
         addField("Position", TypePoint3F, Offset(mPosition, SceneObject), "");
         addField("Rotation", TypePoint3F, Offset(mRotation, SceneObject), "");
         addField("Scale", TypePoint3F, Offset(mScale, SceneObject), "");
      endGroup("SceneObject");

      addGroup("SceneObject: Networking");
         addProtectedField("Ghosted", TypeBool, Offset(mGhosted, SceneObject), &setGhosted, &defaultProtectedGetFn, "");
      endGroup("SceneObject: Networking");
   }

   bool SceneObject::onAdd()
   {
      if(!Parent::onAdd())
         return false;

      return true;
   }

   void SceneObject::onRemove()
   {
      Parent::onRemove();
   }

   void SceneObject::onGroupAdd()
   {
      mAddedToScene = true;

      for (S32 n = 0; n < mComponents.size(); ++n)
      {
         mComponents[n]->setOwnerObject(this);
         mComponents[n]->onAddToScene();
      }

      refresh();
   }

   void SceneObject::onGroupRemove()
   {
      mAddedToScene = false;

      for (S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->onRemoveFromScene();
   }

   void SceneObject::setGhosted( bool _value )
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

   void SceneObject::addComponent(BaseComponent* component)
   {
      component->setOwnerObject(this);
      mComponents.push_back(component);
      
      if (mAddedToScene)
         component->onAddToScene();
   }

   void SceneObject::removeComponent(BaseComponent* component)
   {
      for (U32 n = 0; n < mComponents.size(); ++n)
      {
         if (mComponents[n] == component)
         {
            mComponents.erase(n);
            component->onRemoveFromScene();
            component->unregisterObject();
            SAFE_DELETE(component);
         }
      }
   }

   void SceneObject::clearComponents()
   {
      mComponents.clear();
   }


   void SceneObject::setTemplateAsset( StringTableEntry assetID )
   {
      if ( mTemplate != NULL )
         return;

      mTemplateAssetID = StringTable->insert(assetID);
      AssetPtr<ObjectTemplateAsset> templateAsset;
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

   bool SceneObject::raycast(const Point3F& start, const Point3F& end, Point3F& hitPoint)
   {
      for (S32 n = 0; n < mComponents.size(); ++n)
      {
         if (mComponents[n]->raycast(start, end, hitPoint))
            return true;
      }

      return false;
   }

   void SceneObject::refresh()
   {
      // Calculate transformation.
      mTransformMatrix.createSRTMatrix(mScale.x, mScale.y, mScale.z,
         mRotation.x, mRotation.y, mRotation.z,
         mPosition.x, mPosition.y, mPosition.z);

      // Refresh components
      for (S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->refresh();

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
      mBoundingBox.transform(mTransformMatrix);

      //if ( isServerObject() )
      //   setMaskBits(TransformMask);
   }

   SimObject* SceneObject::findComponentByType(const char* pType)
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
      {
         if (  dStrcmp(mComponents[n]->getClassName(), pType) == 0 )
            return mComponents[n];
      }

      return NULL;
   }

   SimObject* SceneObject::findComponent(StringTableEntry internalName)
   {
      for (S32 n = 0; n < mComponents.size(); ++n)
      {
         if (dStrcmp(mComponents[n]->getInternalName(), internalName) == 0)
            return mComponents[n];
      }

      return NULL;
   }

   void SceneObject::writePacketData(GameConnection *conn, BitStream *stream)
   {
      // Components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->writePacketData(conn, stream);
   }

   void SceneObject::readPacketData(GameConnection *conn, BitStream *stream)
   {
      // Components
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->readPacketData(conn, stream);
   }

   U32 SceneObject::packUpdate(NetConnection* conn, U32 mask, BitStream* stream)
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

   void SceneObject::unpackUpdate(NetConnection* conn, BitStream* stream)
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

   void SceneObject::processMove( const Move *move )
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->processMove(move);
   }

   void SceneObject::interpolateMove( F32 delta )
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->interpolateMove(delta);
   }

   void SceneObject::advanceMove( F32 dt )
   {
      for(S32 n = 0; n < mComponents.size(); ++n)
         mComponents[n]->advanceMove(dt);
   }

   //-----------------------------------------------------------------------------

   void SceneObject::onTamlCustomWrite(TamlCustomNodes& customNodes)
   {
      // Call parent.
      Parent::onTamlCustomWrite(customNodes);

      // Finish early out if no components.
      const U32 componentCount = (U32)mComponents.size();
      if (componentCount == 0)
         return;

      // We don't write components if we're a templated entity.
      if (mTemplate != NULL)
         return;

      TamlCustomNode* pComponentNodes = customNodes.addNode("Components");

      for (S32 n = 0; n < mComponents.size(); ++n)
      {
         Scene::BaseComponent* component = mComponents[n];

         TamlCustomNode* pComponentNode = pComponentNodes->addNode(StringTable->insert(component->getClassName()), false);

         // Static Fields
         AbstractClassRep::FieldList fieldList = component->getFieldList();
         for (Vector<AbstractClassRep::Field>::iterator itr = fieldList.begin(); itr != fieldList.end(); itr++)
         {
            const AbstractClassRep::Field* f = itr;
            if (f->type == AbstractClassRep::StartGroupFieldType ||
               f->type == AbstractClassRep::DepricatedFieldType ||
               f->type == AbstractClassRep::EndGroupFieldType)
               continue;

            for (U32 j = 0; S32(j) < f->elementCount; j++)
            {
               const char *val = (*f->getDataFn)(component, Con::getData(f->type, (void *)(((const char *)component) + f->offset), j, f->table, f->flag));

               if (!val)
                  continue;

               pComponentNode->addField(StringTable->insert(f->pFieldname), StringTable->insert(val));
            }
         }

         // Dynamic Fields
         SimFieldDictionary* fieldDictionary = component->getFieldDictionary();
         for (SimFieldDictionaryIterator ditr(fieldDictionary); *ditr; ++ditr)
            pComponentNode->addField((*ditr)->slotName, (*ditr)->value);
      }
   }

   //-----------------------------------------------------------------------------

   void SceneObject::onTamlCustomRead(const TamlCustomNodes& customNodes)
   {
      // Call parent.
      Parent::onTamlCustomRead(customNodes);

      // Find custom node name.
      const TamlCustomNode* pResultsNode = customNodes.findNode("Components");

      // Finish if we don't have a results name.
      if (pResultsNode == NULL)
         return;

      // Fetch children component nodes.
      const TamlCustomNodeVector& componentNodes = pResultsNode->getChildren();

      // Iterate component nodes.
      for (TamlCustomNodeVector::const_iterator componentNodeItr = componentNodes.begin(); componentNodeItr != componentNodes.end(); ++componentNodeItr)
      {
         // Fetch component node.
         const TamlCustomNode* pComponentNode = *componentNodeItr;

         StringTableEntry componentType = pComponentNode->getNodeName();
         BaseComponent* newComponent = dynamic_cast<BaseComponent*>(Con::createObject(componentType));
            
         if (newComponent)
         {
            // Fetch field nodes.
            const TamlCustomFieldVector& fields = pComponentNode->getFields();

            // Iterate fields.
            for (TamlCustomFieldVector::const_iterator nodeFieldItr = fields.begin(); nodeFieldItr != fields.end(); ++nodeFieldItr)
            {
               // Fetch field.
               TamlCustomField* pField = *nodeFieldItr;

               // Set Field.
               newComponent->setDataField(pField->getFieldName(), NULL, pField->getFieldValue());
            }

            // Add Component to Entity
            newComponent->registerObject();
            addComponent(newComponent);
         }
      }
   }
}