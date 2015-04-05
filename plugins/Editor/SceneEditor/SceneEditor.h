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
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

class SceneEditorCamera : public Scene::SceneCamera
{
   private:
      typedef Scene::SceneCamera Parent;

      Point2I lastMousePosition;
      Point3F translateDirection;
      Point2F mouseDirection;

   public:
      SceneEditorCamera();;

      void onMouseDownEvent(const GuiEvent &event);
      void onMouseDraggedEvent(const GuiEvent &event);

      DECLARE_PLUGIN_CONOBJECT(SceneEditorCamera);
};

class SceneEditor : public virtual Tickable
{
   private:
      SceneEditorCamera mCamera;
      
   public:
      SimGroup*            mSceneGroup;
      Scene::SceneEntity*  mSelectedEntity;
      S32                  sceneEditorArea;
      S32                  sceneOverviewArea;
      S32                  sceneOverviewList;
      S32                  entityInspectorArea;
      Vector<S32>          entityInspectorElements;

      SceneEditor();
      ~SceneEditor();

      void enable();
      void disable();
      void render();

      

      void selectEntity(Scene::SceneEntity* entity);

      virtual void processTick();
      virtual void advanceTime(F32 delta);
      virtual void interpolateTick(F32 delta);
};

extern SceneEditor sceneEditor;
void clickOverviewList();

//extern 
//void refreshSelectionBox();
//void selectEntity(Scene::SceneEntity* entity);