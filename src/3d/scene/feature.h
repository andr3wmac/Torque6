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

#ifndef _SCENE_FEATURE_H_
#define _SCENE_FEATURE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _ASSET_PTR_H_
#include "assets/assetPtr.h"
#endif

#ifndef _MESH_ASSET_H_
#include "3d/scene/object/meshAsset.h"
#endif

#ifndef _TICKABLE_H_
#include "platform/Tickable.h"
#endif

#ifndef _GUITYPES_H_
#include <gui/guiTypes.h>
#endif

namespace Scene
{
   class DLL_PUBLIC SceneFeature : public SimObject
   {
      private:
         typedef SimObject Parent;

      protected:
         bool mActive;

      public:
         const char* mName;

         SceneFeature();
         ~SceneFeature();
		  
         void setActive(bool val);

         virtual void onActivate();
         virtual void onDeactivate();

         static void initPersistFields();
         static bool setActive(void* obj, const char* data) 
         { 
            bool value;
            Con::setData(TypeBool, &value, 0, 1, &data);
            static_cast<SceneFeature*>(obj)->setActive(value);
            return false; 
         }

         DECLARE_CONOBJECT(SceneFeature);
   };
}

#endif