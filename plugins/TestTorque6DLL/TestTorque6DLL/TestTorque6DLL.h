#ifndef _PLUGINS_SHARED_H
#include <plugins/plugins_shared.h>
#endif

#ifndef _SIM_OBJECT_H_
#include <sim/simObject.h>
#endif

#ifndef _CONSOLEOBJECT_H_
#include <console/consoleObject.h>
#endif

#define DECLARE_PLUGIN_CONOBJECT(className)                                                                         \
    static PluginClassRep<className> dynClassRep;                                                                   \
    static AbstractClassRep* getParentStaticClassRep();                                                             \
    static AbstractClassRep* getContainerChildStaticClassRep();                                                     \
    static AbstractClassRep* getStaticClassRep();                                                                   \
    static AbstractClassRep::WriteCustomTamlSchema getStaticWriteCustomTamlSchema();                                \
    virtual AbstractClassRep* getClassRep() const

#define IMPLEMENT_PLUGIN_CONOBJECT(className)                                                                       \
    AbstractClassRep* className::getClassRep() const { return &className::dynClassRep; }                            \
    AbstractClassRep* className::getStaticClassRep() { return &dynClassRep; }                                       \
    AbstractClassRep* className::getParentStaticClassRep() { return Parent::getStaticClassRep(); }                  \
    AbstractClassRep* className::getContainerChildStaticClassRep() { return NULL; }                                 \
    AbstractClassRep::WriteCustomTamlSchema className::getStaticWriteCustomTamlSchema() { return NULL; }            \
    PluginClassRep<className> className::dynClassRep(#className, 0, -1, 0, className::getParentStaticClassRep())

namespace Plugins
{
   void initPlugin(PluginLink _link);
   void testConsoleFunc(SimObject *obj, S32 argc, const char *argv[]);
/*
   class TestClass : public ConsoleObject
   {
      typedef ConsoleObject Parent;

      public:
         TestClass();
         ~TestClass();

         DECLARE_PLUGIN_CONOBJECT(TestClass);
   };
*/
}