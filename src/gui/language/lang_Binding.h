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

ConsoleMethod(LangTable, addLanguage, S32, 3, 4, "(string filename, [string languageName]) Adds the specified language to a table from file\n"
   "@param filename Filename of file containing language data\n"
   "@param languageName (Optional) Name of language\n"
   "@return The size of the language table or -1 on failure")
{
   UTF8 scriptFilenameBuffer[1024];

   Con::expandPath((char*)scriptFilenameBuffer, sizeof(scriptFilenameBuffer), argv[2]);
   return object->addLanguage(scriptFilenameBuffer, argc == 4 ? (const UTF8*)argv[3] : NULL);
}

ConsoleMethod(LangTable, getString, const char *, 3, 3, "(string) ")
{
   char *ret;
   const char *str;

   if ((str = (const char*)object->getString(dAtoi(argv[2]))))
   {
      ret = Con::getReturnBuffer(dStrlen(str) + 1);
      dStrcpy(ret, str);
      return ret;
   }

   return "";
}

ConsoleMethod(LangTable, setDefaultLanguage, void, 3, 3, "(int language) Sets the default language to the one specified\n"
   "@param language ID of the desired language\n"
   "@return No return value.")
{
   object->setDefaultLanguage(dAtoi(argv[2]));
}

ConsoleMethod(LangTable, setCurrentLanguage, void, 3, 3, "(int language) Sets the current language to the one specified\n"
   "@param language ID of the desired language\n"
   "@return No return value.")
{
   object->setCurrentLanguage(dAtoi(argv[2]));
}

ConsoleMethod(LangTable, getCurrentLanguage, S32, 2, 2, "() @return Returns the ID of the currentlt used language")
{
   return object->getCurrentLanguage();
}

ConsoleMethod(LangTable, getLangName, const char *, 3, 3, "(int language) Returns the name of the langauge specified by the given ID\n"
   "@param language The ID of the desired language to check\n"
   "@return The name of the specified language, or the empty string on failure.")
{
   char *ret;
   const char *str;

   if ((str = (const char*)object->getLangName(dAtoi(argv[2]))))
   {
      ret = Con::getReturnBuffer(dStrlen(str) + 1);
      dStrcpy(ret, str);
      return ret;
   }

   return "";
}

ConsoleMethod(LangTable, getNumLang, S32, 2, 2, "() Returns the number of currently stored languages\n"
   "@return The number of laguages as an integer")
{
   return object->getNumLang();
}

extern "C" {
   DLL_PUBLIC LangTable* LangTableCreateInstance()
   {
      return new LangTable();
   }

   DLL_PUBLIC int LangTableAddLanguage(LangTable* langTable, const char* filename, const char* languageName)
   {
      UTF8 scriptFilenameBuffer[1024];

      Con::expandPath((char*)scriptFilenameBuffer, sizeof(scriptFilenameBuffer), filename);
      return langTable->addLanguage(scriptFilenameBuffer, (const UTF8*)languageName);
   }

   DLL_PUBLIC const char* LangTableGetString(LangTable* langTable, int langString)
   {
      char *ret;
      const char *str;

      if ((str = (const char*)langTable->getString(langString)))
      {
         ret = CInterface::GetMarshallableString(dStrlen(str) + 1);
         dStrcpy(ret, str);
         return ret;
      }

      return NULL;
   }

   DLL_PUBLIC void LangTableSetDefaultLanguage(LangTable* langTable, int language)
   {
      langTable->setDefaultLanguage(language);
   }

   DLL_PUBLIC void LangTableSetCurrentLanguage(LangTable* langTable, int language)
   {
      langTable->setCurrentLanguage(language);
   }

   DLL_PUBLIC int LangTableGetCurrentLanguage(LangTable* langTable)
   {
      return langTable->getCurrentLanguage();
   }

   DLL_PUBLIC const char* LangTableGetLangName(LangTable* langTable, int language)
   {
      char *ret;
      const char *str;

      if ((str = (const char*)langTable->getLangName(language)))
      {
         ret = CInterface::GetMarshallableString(dStrlen(str) + 1);
         dStrcpy(ret, str);
         return ret;
      }

      return NULL;
   }

   DLL_PUBLIC int LangTableGetNumLang(LangTable* langTable)
   {
      return langTable->getNumLang();
   }
}