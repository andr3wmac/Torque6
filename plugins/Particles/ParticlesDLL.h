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

extern "C" 
{
   PLUGIN_FUNC void create(Plugins::PluginLink _link);
   PLUGIN_FUNC void destroy();
   PLUGIN_FUNC void processTick();
}

struct PosUVColorVertex
{
   F32 m_x;
	F32 m_y;
	F32 m_z;
	F32 m_u;
	F32 m_v;
	U32 m_abgr;
};

extern bool                      particlesEnabled;
extern bgfx::ProgramHandle       particleShader;
extern Rendering::RenderData*    particleRenderData;
extern bgfx::VertexBufferHandle  particleVB;
extern bgfx::IndexBufferHandle   particleIB;
extern bgfx::TextureHandle       particleTexture;

void enableParticles(SimObject *obj, S32 argc, const char *argv[]);
void disableParticles(SimObject *obj, S32 argc, const char *argv[]);
void refresh();

class SimObject2Rep;

class SimObject2 : public SimObject
{
   typedef SimObject Parent;

public:    
   SimObject2(){};

   static SimObject2Rep dynClassRep;
   static AbstractClassRep* getParentStaticClassRep();                                                             
   static AbstractClassRep* getContainerChildStaticClassRep();                                                     
   static AbstractClassRep* getStaticClassRep();                                                                   
   static AbstractClassRep::WriteCustomTamlSchema getStaticWriteCustomTamlSchema();                                
   virtual AbstractClassRep* getClassRep() const;
};

class SimObject2Rep : public AbstractClassRep
{
public:
   SimObject2Rep(const char *name, S32 netClassGroupMask, S32 netClassType, S32 netEventDir, AbstractClassRep *parent)
   {
      // name is a static compiler string so no need to worry about copying or deleting
      mClassName = name;

      // Clean up mClassId
      for (U32 i = 0; i < NetClassGroupsCount; i++)
         mClassId[i] = -1;

      // Set properties for this ACR
      mClassType = netClassType;
      mClassGroupMask = netClassGroupMask;
      mNetEventDir = netEventDir;
      parentClass = parent;

      // Finally, register ourselves.
      //registerClassRep(this);
   };

   ~SimObject2Rep(){

   }

   void registerClass()
   {
      Plugins::Link.Con.registerClassRep(this);
      init();
   }

   virtual AbstractClassRep* getContainerChildClass(const bool recurse)
   {
      // Fetch container children type.
      AbstractClassRep* pChildren = SimObject2::getContainerChildStaticClassRep();
      if (!recurse || pChildren != NULL)
         return pChildren;

      // Fetch parent type.
      AbstractClassRep* pParent = SimObject2::getParentStaticClassRep();
      if (pParent == NULL)
         return NULL;

      // Get parent container children.
      return pParent->getContainerChildClass(recurse);
   }

   virtual WriteCustomTamlSchema getCustomTamlSchema(void)
   {
      return SimObject2::getStaticWriteCustomTamlSchema();
   }

   /// Perform class specific initialization tasks.
   ///
   /// Link namespaces, call initPersistFields() and consoleInit().
   void init() const
   {
      // Get handle to our parent class, if any, and ourselves (we are our parent's child).
      AbstractClassRep *parent = SimObject2::getParentStaticClassRep();
      AbstractClassRep *child = SimObject2::getStaticClassRep();

      // If we got reps, then link those namespaces! (To get proper inheritance.)
      if (parent && child)
         Plugins::Link.Con.classLinkNamespaces(parent->getNameSpace(), child->getNameSpace());

      // Finally, do any class specific initialization...
      SimObject2::initPersistFields();
      SimObject2::consoleInit();
   }

   /// Wrap constructor.
   ConsoleObject* create() const { return new SimObject2; }
};
