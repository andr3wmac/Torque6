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

#ifndef _RENDERABLE_H_
#define _RENDERABLE_H_

#ifndef _CONSOLEINTERNAL_H_
#include "console/consoleInternal.h"
#endif

#ifndef _VECTOR_H_
#include "collection/vector.h"
#endif

#ifndef _SCENE_FEATURE_H_
#include "3d/scene/feature.h"
#endif

namespace Rendering
{
   class DLL_PUBLIC RenderFeature : public Scene::SceneFeature
   {
      private:
         typedef Scene::SceneFeature Parent;

      public:
         virtual void preRender() { }
         virtual void render() { }
         virtual void postRender() { }
         virtual void resize() { }

         virtual void onActivate();
         virtual void onDeactivate();

         DECLARE_CONOBJECT(RenderFeature);
   };

   class DLL_PUBLIC Renderable
   {
      private:
         // This just makes life easy
         typedef Vector<Renderable *>::iterator RenderableListIterator;

         /// Returns a reference to the list of all Renderable objects.
         static Vector<Renderable *>& getRenderList();

         typedef Vector<RenderFeature *>::iterator FeatureListIterator;
         static Vector<RenderFeature *>& getFeatureList();

         /// Indicates whether the object is currently processing ticks or not.
         bool mRender;

      protected:
         /// This method is called once every 32ms if isProcessingTicks returns true
         /// when called on the object
         virtual void preRender() = 0;
         virtual void render() = 0;
         virtual void postRender() = 0;
         virtual void resize() { }

      public:
         /// Constructor
         /// This will add the object to the process list
         Renderable();

         /// Destructor
         /// Remove this object from the process list
         ~Renderable();

         /// Is this object wanting to receive tick notifications
         /// @returns True if object wants tick notifications
         bool isRendering() const { return mRender; };

         /// Sets this object as either tick processing or not
         /// @param   tick     True if this object should process ticks
         void setRendering(bool tick = true);

         static void preRenderAll();
         static void renderAll();
         static void postRenderAll();
         static void resizeAll();

         static void addFeature(RenderFeature* feature);
         static void removeFeature(RenderFeature* feature);
   };
}

#endif
