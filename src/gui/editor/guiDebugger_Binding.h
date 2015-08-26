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

ConsoleMethod(DbgFileView, setCurrentLine, void, 4, 4, "(int line, bool selected)"
   "Set the current highlighted line.\n"
   "@return No return value.")
{
   object->setCurrentLine(dAtoi(argv[2]), dAtob(argv[3]));
}

ConsoleMethod(DbgFileView, getCurrentLine, const char *, 2, 2, "()"
   "Get the currently executing file and line, if any.\n\n"
   "@returns A string containing the file, a tab, and then the line number."
   " Use getField() with this.")
{
   S32 lineNum;
   const char *file = object->getCurrentLine(lineNum);
   char* ret = Con::getReturnBuffer(256);
   dSprintf(ret, 256 * sizeof(char), "%s\t%d", file, lineNum);
   return ret;
}

ConsoleMethod(DbgFileView, open, bool, 3, 3, "(string filename)"
   "Open a file for viewing.\n\n"
   "@return Returns true on success.\n"
   "@note This loads the file from the local system.")
{
   return object->openFile(argv[2]);
}

ConsoleMethod(DbgFileView, clearBreakPositions, void, 2, 2, "()"
   "Clear all break points in the current file.\n"
   "@return No return value.")
{
   object->clearBreakPositions();
}

ConsoleMethod(DbgFileView, setBreakPosition, void, 3, 3, "(int line)"
   "Set a breakpoint at the specified line.\n"
   "@return No return value.")
{
   object->setBreakPosition(dAtoi(argv[2]));
}

ConsoleMethod(DbgFileView, setBreak, void, 3, 3, "(int line)"
   "Set a breakpoint at the specified line.\n"
   "@return No return value.")
{
   object->setBreakPointStatus(dAtoi(argv[2]), true);
}

ConsoleMethod(DbgFileView, removeBreak, void, 3, 3, "(int line)"
   "Remove a breakpoint from the specified line.\n"
   "@return No return value.")
{
   object->setBreakPointStatus(dAtoi(argv[2]), false);
}

ConsoleMethod(DbgFileView, findString, bool, 3, 3, "(string findThis)"
   "Find the specified string in the currently viewed file and "
   "scroll it into view.\n"
   "@return Returns true on success\n")
{
   return object->findString(argv[2]);
}

extern "C"{
   DLL_PUBLIC DbgFileView* DbgFileViewCreateInstance()
   {
      return new DbgFileView();
   }

   DLL_PUBLIC void DbgFileViewSetCurrentLine(DbgFileView* view, S32 line, bool selected)
   {
      view->setCurrentLine(line, selected);
   }

   //CTODO fieldlist
   DLL_PUBLIC const char* DbgFileViewGetCurrentLine(DbgFileView* view, S32 line, bool selected)
   {
      S32 lineNum;
      const char *file = view->getCurrentLine(lineNum);
      char* ret = Con::getReturnBuffer(256);
      dSprintf(ret, 256 * sizeof(char), "%s\t%d", file, lineNum);
      return CInterface::GetMarshallableString(ret);
   }

   DLL_PUBLIC bool DbgFileViewOpen(DbgFileView* view, const char* fileName)
   {
      return view->openFile(fileName);
   }

   DLL_PUBLIC void DbgFileViewClearBreakPositions(DbgFileView* view)
   {
      view->clearBreakPositions();
   }

   DLL_PUBLIC void DbgFileViewSetBreakPosition(DbgFileView* view, S32 line)
   {
      view->setBreakPosition(line);
   }

   DLL_PUBLIC void DbgFileViewSetBreak(DbgFileView* view, S32 line)
   {
      view->setBreakPointStatus(line, true);
   }

   DLL_PUBLIC void DbgFileViewRemoveBreak(DbgFileView* view, S32 line)
   {
      view->setBreakPointStatus(line, false);
   }

   DLL_PUBLIC bool DbgFileViewFindString(DbgFileView* view, const char* findThis)
   {
      return view->findString(findThis);
   }
}