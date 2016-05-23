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

#ifndef _SCENE_CORE_H_
#define _SCENE_CORE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _PHYSICS_H
#include <physics/physics.h>
#endif

#ifndef _NETCONNECTION_H_
#include "network/netConnection.h"
#endif

#ifndef _MPLANESET_H_
#include "math/mPlaneSet.h"
#endif

namespace Scene
{
   class SceneObject;

   // Init/Destroy
   void init();
   void destroy();

   // Running
   void play();
   bool isPlaying();
   void pause();
   void stop();

   void clear();
   void clearGhosted();
   void append(const char* filename);
   void load(const char* filename, bool append = false);
   void save(const char* filename);

   // Scene Functions
   SimGroup*      getSceneGroup();
   Box3F          getSceneBounds();

   // Scene Objects
   void                 addObject(SceneObject* obj, const char* name = "SceneObject");
   void                 deleteObject(SceneObject* obj);
   void                 removeObject(SceneObject* obj);
   SceneObject*         findObject(const char* name);
   Vector<SimObject*>   findComponentsByType(const char* pType);
   void                 refresh();
   SceneObject*         raycast(const Point3F& start, const Point3F& end);
   Vector<SceneObject*> boxSearch(const PlaneSetF& planes);

   // Networking
   void onCameraScopeQuery(NetConnection *cr, CameraScopeQuery *camInfo);

   // ----------------------------------------
   //   Preprocessors
   // ----------------------------------------

   class DLL_PUBLIC ScenePreprocessor
   {
      public:
         S32 priority;
         bool isFinished;

         ScenePreprocessor() : priority(0), isFinished(false) {}

         virtual void preprocess() { }
   };

   bool isPreprocessingActive(bool process = false);
   void addPreprocessor(ScenePreprocessor* preprocessor);
   bool removePreprocessor(ScenePreprocessor* preprocessor);
   Vector<ScenePreprocessor*>* getPreprocessorList();
   
}

#endif