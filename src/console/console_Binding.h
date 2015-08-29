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

extern "C"{
   DLL_PUBLIC bool Globals_GetBool(const char* variable)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      return Con::getBoolVariable(globalVar);
   }

   DLL_PUBLIC void Globals_SetBool(const char* variable, bool val)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      Con::setBoolVariable(globalVar, val);
   }

   DLL_PUBLIC S32 Globals_GetInt(const char* variable)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      return Con::getIntVariable(globalVar);
   }

   DLL_PUBLIC void Globals_SetInt(const char* variable, S32 val)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      Con::setIntVariable(globalVar, val);
   }

   DLL_PUBLIC F32 Globals_GetFloat(const char* variable)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      return Con::getFloatVariable(globalVar);
   }

   DLL_PUBLIC void Globals_SetFloat(const char* variable, F32 val)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      Con::setFloatVariable(globalVar, val);
   }

   DLL_PUBLIC const char* Globals_GetString(const char* variable)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      return CInterface::GetMarshallableString(Con::getVariable(globalVar));
   }

   DLL_PUBLIC void Globals_SetString(const char* variable, const char* val)
   {
      char globalVar[80];
      strcpy(globalVar, "$");
      strcat(globalVar, variable);
      Con::setVariable(globalVar, val);
   }
}