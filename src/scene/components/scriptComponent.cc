//-----------------------------------------------------------------------------
// Copyright (c) 2016 Andrew Mac
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

#include "console/consoleTypes.h"
#include "scriptComponent.h"

// Script bindings.
#include "scriptComponent_Binding.h"

namespace Scene
{
   IMPLEMENT_CONOBJECT(ScriptComponent);

   ScriptComponent::ScriptComponent()
   {

   }

   ScriptComponent::~ScriptComponent()
   {

   }

   void ScriptComponent::initPersistFields()
   {
      // Call parent.
      Parent::initPersistFields();

      addGroup("ScriptComponent");

         addProtectedField("ScriptFile", TypeAssetLooseFilePath, Offset(mScriptFile, ScriptComponent), &ScriptComponent::setScriptFile, &defaultProtectedGetFn, &defaultProtectedWriteFn, "");

      endGroup("ScriptComponent");
   }

   void ScriptComponent::setScriptFile(StringTableEntry scriptFile)
   {
      mScriptFile = scriptFile;

      FileStream scriptFileStream;
      Stream* scriptStream;

      char fullScriptPath[1024];
      Con::expandPath(fullScriptPath, sizeof(fullScriptPath), mScriptFile);
      Platform::makeFullPathName(fullScriptPath, fullScriptPath, sizeof(fullScriptPath), Platform::getCurrentDirectory());

      bool success = false;
      success = scriptFileStream.open(fullScriptPath, FileStream::Read);

      if (!success)
      {
         char msg[1024];
         dSprintf(msg, sizeof(msg), "Failed to open \"%s\".", fullScriptPath);
         printf(" Error : %s", msg);
         return;
      }

      scriptStream = &scriptFileStream;

      // Create a script buffer.
      const U32 size = scriptStream->getStreamSize();
      char* pScriptBuffer = new char[size + 1];

      // Read script.
      scriptStream->read(size, pScriptBuffer);
      scriptFileStream.close();
      pScriptBuffer[size] = 0;

      // Execute it.
      const S32 errorHash = Con::getIntVariable("$ScriptErrorHash");
      Con::evaluate(pScriptBuffer, false, mScriptFile);
      delete[] pScriptBuffer;

      // Did an error occur?
      if (errorHash != Con::getIntVariable("$ScriptErrorHash"))
         printf("An error occurred parsing the script '%s'.", mScriptFile);
   }

   void ScriptComponent::onAddToScene()
   {  
      Parent::onAddToScene();

      Con::executef(this, 2, "onAddToScene", Con::getIntArg(mOwnerObject->getId()));
   }

   void ScriptComponent::onRemoveFromScene()
   {
      Parent::onRemoveFromScene();

      Con::executef(this, 2, "onRemoveFromScene", Con::getIntArg(mOwnerObject->getId()));
   }

   void ScriptComponent::onSceneStart()
   {
      Parent::onSceneStart();

      Con::executef(this, 2, "onSceneStart", Con::getIntArg(mOwnerObject->getId()));
   }

   void ScriptComponent::onScenePlay() 
   {
      Parent::onScenePlay();

      Con::executef(this, 2, "onScenePlay", Con::getIntArg(mOwnerObject->getId()));
   }

   void ScriptComponent::onScenePause() 
   {
      Parent::onScenePause();

      Con::executef(this, 2, "onScenePause", Con::getIntArg(mOwnerObject->getId()));
   }

   void ScriptComponent::onSceneStop() 
   {
      Parent::onSceneStop();

      Con::executef(this, 2, "onSceneStop", Con::getIntArg(mOwnerObject->getId()));
   }

   void ScriptComponent::refresh()
   {
      Parent::refresh();

      Con::executef(this, 2, "refresh", Con::getIntArg(mOwnerObject->getId()));
   }
}
