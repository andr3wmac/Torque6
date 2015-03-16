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

#include "entity_ScriptBinding.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(SceneEntity);

   SceneEntity::SceneEntity()
   {
      mTemplatePath = StringTable->EmptyString;
      mTemplate = NULL;
      mScale.set(1.0f, 1.0f, 1.0f);
      mPosition.set(0.0f, 0.0f, 0.0f);
      mRotation.set(0.0f, 0.0f, 0.0f);
   }

   SceneEntity::~SceneEntity()
   {
      SAFE_DELETE(mTemplate);
   }

   void SceneEntity::initPersistFields()
   {
       // Call parent.
       Parent::initPersistFields();

       addProtectedField("template", TypeString, Offset(mTemplatePath, SceneEntity), &setTemplate, &defaultProtectedGetFn, "");
       addField("position", TypePoint3F, Offset(mPosition, SceneEntity), "");
       addField("rotation", TypePoint3F, Offset(mRotation, SceneEntity), "");
       addField("scale", TypePoint3F, Offset(mScale, SceneEntity), "");
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
      if ( mTemplate == NULL ) return;

      for(S32 n = 0; n < mTemplate->size(); ++n)
      {
         BaseComponent* component = static_cast<BaseComponent*>(mTemplate->at(n));
         if ( component )
         {
            component->setOwnerEntity(this);
            component->onAddToScene();
         }
      }

      refresh();
   }

   void SceneEntity::onGroupRemove()
   {
      if ( mTemplate == NULL ) return;

      for(S32 n = 0; n < mTemplate->size(); ++n)
      {
         BaseComponent* component = static_cast<BaseComponent*>(mTemplate->at(n));
         if ( component )
            component->onRemoveFromScene();
      }
   }

   void SceneEntity::setTemplate( const char* pTemplatePath )
   {
      mTemplatePath = StringTable->insert(pTemplatePath);

      Taml taml;
      mTemplate = taml.read<Scene::EntityTemplate>( pTemplatePath );
   }

   void SceneEntity::refresh()
   {
      if ( mTemplate == NULL ) return;

      // Refresh Bounding Box
      Box3F newBoundingBox;
      newBoundingBox.set(Point3F(0, 0, 0));

      for(S32 n = 0; n < mTemplate->size(); ++n)
      {
         BaseComponent* component = static_cast<BaseComponent*>(mTemplate->at(n));
         if ( component )
            newBoundingBox.intersect(component->getBoundingBox());
      }

      newBoundingBox.minExtents = (newBoundingBox.minExtents * mScale) + mPosition;
      newBoundingBox.maxExtents = (newBoundingBox.maxExtents * mScale) + mPosition;
      mBoundingBox = newBoundingBox;

      // Refresh components
      for(S32 n = 0; n < mTemplate->size(); ++n)
      {
         BaseComponent* component = static_cast<BaseComponent*>(mTemplate->at(n));
         if ( component )
            component->refresh();
      }
   }

   SimObject* SceneEntity::findComponentByType(const char* pType)
   {
      if ( mTemplate == NULL ) return NULL;

      for(S32 n = 0; n < mTemplate->size(); ++n)
      {
         BaseComponent* component = static_cast<BaseComponent*>(mTemplate->at(n));
         if ( component && dStrcmp(component->mTypeString, pType) == 0 )
            return component;
      }

      return NULL;
   }
}