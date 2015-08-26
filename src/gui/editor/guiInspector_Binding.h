//-----------------------------------------------------------------------------
// Copyright (c) 2013 GarageGames, LLC
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

ConsoleMethod(GuiInspector, inspect, void, 3, 3, "(obj) Goes through the object's fields and autogenerates editor boxes\n"
   "@return No return value.")
{
   SimObject * target = Sim::findObject(argv[2]);
   if (!target)
   {
      if (dAtoi(argv[2]) > 0)
         Con::warnf("%s::inspect(): invalid object: %s", argv[0], argv[2]);

      object->clearGroups();
      return;
   }

   object->inspectObject(target);
}


ConsoleMethod(GuiInspector, getInspectObject, const char*, 2, 2, "() - Returns currently inspected object\n"
   "@return The Object's ID as a string.")
{
   SimObject *pSimObject = object->getInspectObject();
   if (pSimObject != NULL)
      return pSimObject->getIdString();

   return "";
}

ConsoleMethod(GuiInspector, setName, void, 3, 3, "(NewObjectName) Set object name.\n"
   "@return No return value.")
{
   object->setName(argv[2]);
}

ConsoleMethod(GuiInspectorField, apply, void, 3, 3, "(newValue) Applies the given value to the field\n"
   "@return No return value.")
{
   object->setData(argv[2]);
}

ConsoleMethod(GuiInspectorDynamicGroup, addDynamicField, void, 2, 2, "obj.addDynamicField();")
{
   object->addDynamicField();
}

ConsoleMethod(GuiInspectorDynamicField, renameField, void, 3, 3, "field.renameField(newDynamicFieldName);")
{
   object->renameField(StringTable->insert(argv[2]));
}

ConsoleMethod(GuiInspectorDynamicGroup, inspectGroup, bool, 2, 2, "() Refreshes the dynamic fields in the inspector.\n"
   "@return Returns true on success.")
{
   return object->inspectGroup();
}

extern "C"{
   DLL_PUBLIC GuiInspector* GuiInspectorCreateInstance()
   {
      return new GuiInspector();
   }

   DLL_PUBLIC SimObject* GuiInspectorGetInspectObject(GuiInspector* inspector)
   {
      return inspector->getInspectObject();
   }

   DLL_PUBLIC void GuiInspectorSetName(GuiInspector* inspector, const char* newObjectName)
   {
      inspector->setName(newObjectName);
   }

   DLL_PUBLIC GuiInspectorField* GuiInspectorFieldCreateInstance()
   {
      return new GuiInspectorField();
   }

   DLL_PUBLIC void GuiInspectorFieldApply(GuiInspectorField* field, const char* newValue)
   {
      field->setData(newValue);
   }

   DLL_PUBLIC GuiInspectorGroup* GuiInspectorGroupCreateInstance()
   {
      return new GuiInspectorGroup();
   }

   DLL_PUBLIC GuiInspectorDynamicField* GuiInspectorDynamicFieldCreateInstance()
   {
      return new GuiInspectorDynamicField();
   }

   DLL_PUBLIC void GuiInspectorDynamicFieldRenameField(GuiInspectorDynamicField* field, const char* newName)
   {
      field->renameField(StringTable->insert(newName));
   }

   DLL_PUBLIC GuiInspectorDynamicGroup* GuiInspectorDynamicGroupCreateInstance()
   {
      return new GuiInspectorDynamicGroup();
   }

   DLL_PUBLIC void GuiInspectorDynamicGroupAddDynamicField(GuiInspectorDynamicGroup* group)
   {
      group->addDynamicField();
   }

   DLL_PUBLIC void GuiInspectorDynamicGroupInspectGroup(GuiInspectorDynamicGroup* group)
   {
      group->inspectGroup();
   }

   DLL_PUBLIC GuiInspectorDatablockField* GuiInspectorDatablockFieldCreateInstance()
   {
      return new GuiInspectorDatablockField();
   }

   DLL_PUBLIC void GuiInspectorInspect(GuiInspector* inspector, SimObject* obj)
   {
      if (obj)
         inspector->inspectObject(obj);
      else
         inspector->clearGroups();
   }
}