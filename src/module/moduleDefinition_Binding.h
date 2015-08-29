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

#include "c-interface/c-interface.h"

ConsoleMethodGroupBeginWithDocs(ModuleDefinition, SimSet)

/*! Saves the module definition to the file it was loaded from (if any).
    @return (bool success) Whether the module definition was saved or not.
*/
ConsoleMethodWithDocs(ModuleDefinition, save, ConsoleBool, 2, 2, ())
{
    // Save.
    return object->save();
}

//-----------------------------------------------------------------------------

/*! Gets the module manager which this module definition is registered with (if any).
    @return (moduleManager) The module manager which this module definition is registered with (zero if not registered).
*/
ConsoleMethodWithDocs(ModuleDefinition, getModuleManager, ConsoleInt, 2, 2, ())
{
    // Fetch module manager.
    ModuleManager* pModuleManager = object->getModuleManager();

    return pModuleManager != NULL ? pModuleManager->getId() : 0;
}

//-----------------------------------------------------------------------------

/*! Gets the number of module dependencies this module definition has.
    @return (int count) The number of module dependencies this module definition has.
*/
ConsoleMethodWithDocs(ModuleDefinition, getDependencyCount, ConsoleInt, 2, 2, ())
{
    // Get module dependency count.
    return object->getDependencyCount();
}

//-----------------------------------------------------------------------------

/*! Gets the module dependency at the specified index.
    @param dependencyIndex The module dependency index.
    @return (module-dependency) The module dependency at the specified index.
*/
ConsoleMethodWithDocs(ModuleDefinition, getDependency, ConsoleString, 3, 3, (int dependencyIndex))
{
    // Fetch dependency index.
    const U32 dependencyIndex = (U32)dAtoi(argv[2]);

    // Get module dependency.
    ModuleDefinition::ModuleDependency dependency;
    if ( object->getDependency( dependencyIndex, dependency ) == false )
        return StringTable->EmptyString;

    // Format module dependency.
    char* pReturnBuffer = Con::getReturnBuffer( 256 );
    dSprintf( pReturnBuffer, 256, "%s %d", dependency.mModuleId, dependency.mVersionId );

    return pReturnBuffer;
}

//-----------------------------------------------------------------------------

/*! Adds the specified moduleId and vesionId as a dependency.
    @param moduleId The module Id to add as a dependency.
    @param versionId The version Id to add as a dependency.  Using zero indicates any version.
    @return (bool success) Whether the module dependency was added or not.
*/
ConsoleMethodWithDocs(ModuleDefinition, addDependency, ConsoleBool, 4, 4, (moduleId, versionId))
{
    // Fetch module Id.
    const char* pModuleId = argv[2];

    // Fetch version Id.
    const U32 versionId = (U32)dAtoi(argv[3]);

    // Add dependency.
    return object->addDependency( pModuleId, versionId );
}

//-----------------------------------------------------------------------------

/*! Removes the specified moduleId as a dependency.
    @param moduleId The module Id to remove as a dependency.
    @return (bool success) Whether the module dependency was removed or not.
*/
ConsoleMethodWithDocs(ModuleDefinition, removeDependency, ConsoleBool, 3, 3, (moduleId))
{
    // Fetch module Id.
    const char* pModuleId = argv[2];

    // Remove dependency.
    return object->removeDependency( pModuleId );
}

ConsoleMethodGroupEndWithDocs(ModuleDefinition)

extern "C" {
   DLL_PUBLIC ModuleDefinition* ModuleDefinitionCreateInstance()
   {
      return new ModuleDefinition();
   }

   DLL_PUBLIC const char* ModuleDefinitionGetModuleId(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleId());
   }

   DLL_PUBLIC void ModuleDefinitionSetModuleId(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setModuleId(value);
   }

   DLL_PUBLIC int ModuleDefinitionGetVersionId(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getVersionId();
   }

   DLL_PUBLIC void ModuleDefinitionSetVersionId(ModuleDefinition* moduleDefinition, int value)
   {
      moduleDefinition->setVersionId(value);
   }

   DLL_PUBLIC int ModuleDefinitionGetBuildId(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getBuildId();
   }

   DLL_PUBLIC void ModuleDefinitionSetBuildId(ModuleDefinition* moduleDefinition, int value)
   {
      moduleDefinition->setBuildId(value);
   }

   DLL_PUBLIC bool ModuleDefinitionGetEnabled(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getEnabled();
   }

   DLL_PUBLIC void ModuleDefinitionSetEnabled(ModuleDefinition* moduleDefinition, bool value)
   {
      moduleDefinition->setEnabled(value);
   }

   DLL_PUBLIC bool ModuleDefinitionGetSynchronized(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getSynchronized();
   }

   DLL_PUBLIC void ModuleDefinitionSetSynchronized(ModuleDefinition* moduleDefinition, bool value)
   {
      moduleDefinition->setSynchronized(value);
   }

   DLL_PUBLIC bool ModuleDefinitionGetDeprecated(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getDeprecated();
   }

   DLL_PUBLIC void ModuleDefinitionSetDeprecated(ModuleDefinition* moduleDefinition, bool value)
   {
      moduleDefinition->setDeprecated(value);
   }

   DLL_PUBLIC bool ModuleDefinitionGetCriticalMerge(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getCriticalMerge();
   }

   DLL_PUBLIC void ModuleDefinitionSetCriticalMerge(ModuleDefinition* moduleDefinition, bool value)
   {
      moduleDefinition->setCriticalMerge(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetDescription(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleDescription());
   }

   DLL_PUBLIC void ModuleDefinitionSetDescription(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setModuleDescription(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetAuthor(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getAuthor());
   }

   DLL_PUBLIC void ModuleDefinitionSetAuthor(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setAuthor(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetGroup(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleGroup());
   }

   DLL_PUBLIC void ModuleDefinitionSetGroup(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setModuleGroup(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetType(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleType());
   }

   DLL_PUBLIC void ModuleDefinitionSetType(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setModuleType(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetDependencies(ModuleDefinition* moduleDefinition)
   {
      return NULL;
      /*if (IsAlive())
         throw gcnew NotImplementedException("Need to use a typeModuleDependencyVector");*/
      //return gcnew String(moduleDefinition->getDependencies());
   }

   DLL_PUBLIC void ModuleDefinitionSetDependencies(ModuleDefinition* moduleDefinition, const char* value)
   {
      /*throw gcnew NotImplementedException("Need to use a typeModuleDependencyVector");*/
      //moduleDefinition->setDependencies(_value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetScriptFile(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getScriptFile());
   }

   DLL_PUBLIC void ModuleDefinitionSetScriptFile(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setScriptFile(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetCreateFunction(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getCreateFunction());
   }

   DLL_PUBLIC void ModuleDefinitionSetCreateFunction(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setCreateFunction(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetDestroyFunction(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getDestroyFunction());
   }

   DLL_PUBLIC void ModuleDefinitionSetDestroyFunction(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setDestroyFunction(value);
   }

   DLL_PUBLIC const char* ModuleDefinitionGetAssetTagsManifest(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getAssetTagsManifest());
   }

   DLL_PUBLIC void ModuleDefinitionSetAssetTagsManifest(ModuleDefinition* moduleDefinition, const char* value)
   {
      moduleDefinition->setAssetTagsManifest(value);
   }

   DLL_PUBLIC int ModuleDefinitionGetScopeSet(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getScopeSet();
   }

   DLL_PUBLIC const char* ModuleDefinitionGetModulePath(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModulePath());
   }

   DLL_PUBLIC const char* ModuleDefinitionGetModuleFile(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleFile());
   }

   DLL_PUBLIC const char* ModuleDefinitionGetModuleFilePath(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleFilePath());
   }

   DLL_PUBLIC const char* ModuleDefinitionGetModuleScriptFilePath(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getModuleScriptFilePath());
   }

   DLL_PUBLIC const char* ModuleDefinitionGetSignature(ModuleDefinition* moduleDefinition)
   {
      return CInterface::GetMarshallableString(moduleDefinition->getSignature());
   }

   DLL_PUBLIC bool ModuleDefinitionSave(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->save();
   }

   DLL_PUBLIC ModuleManager* ModuleDefinitionGetModuleManager(ModuleDefinition* moduleDefinition)
   {
       return moduleDefinition->getModuleManager();
   }

   DLL_PUBLIC int ModuleDefinitionGetDependencyCount(ModuleDefinition* moduleDefinition)
   {
      return moduleDefinition->getDependencyCount();
   }

   DLL_PUBLIC const char* ModuleDefinitionGetDependency(ModuleDefinition* moduleDefinition, int dependencyIndex)
   {
      // Get module dependency.
      ModuleDefinition::ModuleDependency dependency;
      if (moduleDefinition->getDependency(dependencyIndex, dependency) == false)
         return StringTable->EmptyString;

      // Format module dependency.
      char* pReturnBuffer = CInterface::GetMarshallableString(256);
      dSprintf(pReturnBuffer, 256, "%s %d", dependency.mModuleId, dependency.mVersionId);

      return pReturnBuffer;
   }

   DLL_PUBLIC bool ModuleDefinitionAddDependency(ModuleDefinition* moduleDefinition, const char* moduleId, int versionId)
   {
      return moduleDefinition->addDependency(moduleId, versionId);
   }

   DLL_PUBLIC bool ModuleDefinitionRemoveDependency(ModuleDefinition* moduleDefinition, const char* moduleId)
   {
      return moduleDefinition->removeDependency(moduleId);
   }
}