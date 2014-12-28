#define TORQUE_PLUGIN
#define TORQUE_DEBUG

#include "TestTorque6DLL.h"
#include <sim/simObject.h>
#include <plugins/plugins_shared.h>

#define TEST_FUNC0 Link.Con.printf("worked0");
#define TEST_FUNC1 Link.Con.printf("worked1");
#define TEST_FUNC2 Link.Con.printf("worked2");

#define TEST(func_num) TEST_FUNC##func_num

namespace Plugins
{
   PluginLink Link;

   void initPlugin(PluginLink _link)
   {
      Link = _link;
      Link.Con.printf("Hello World from plugin init!");
      Link.Con.addCommand("Plugins", "testConsoleFunc", testConsoleFunc, "", 1, 1);

      TEST(0)

      Link.SysGUI.beginScrollArea("Plugin Window!", 200, 200, 200, 200);
      Link.SysGUI.label("HELLO FROM DLL!");
      Link.SysGUI.endScrollArea();

      //Plugins::Link.Con.registerClassRep(TestClass::getStaticClassRep());
   }

   void testConsoleFunc(SimObject *obj, S32 argc, const char *argv[])
   {
      Link.Con.printf("Hello World from testConsoleFunc");
      if ( obj )
         obj->dump();
   }

/*   IMPLEMENT_PLUGIN_CONOBJECT(TestClass);

   TestClass::TestClass()
   {

   }

   TestClass::~TestClass()
   {

   }
*/
}