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

ConsoleMethod(MessageVector, clear, void, 2, 2, "() Clear the message vector.\n"
   "@return No return value")
{
   object->clear();
}

ConsoleMethod(MessageVector, pushBackLine, void, 4, 4, "( msg [ , tag ] ) Use the pushBackLine method to push a line onto the back of the list.\n"
   "@param msg The text to add to this control.\n"
   "@param tag An optional tag to tag this line with. If not tag is supplied, a tag of 0 is used.\n"
   "@return No return value.\n"
   "@sa popBackLine, popFrontLine, insertLine, pushFrontLine")
{
   U32 tag = 0;
   if (argc == 4)
      tag = dAtoi(argv[3]);

   object->pushBackLine(argv[2], tag);
}

ConsoleMethod(MessageVector, popBackLine, bool, 2, 2, "() Use the popBackLine method to pop a line from the back of the list; destroys the line.\n"
   "@return No return value.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   if (object->getNumLines() == 0) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::popBackLine(): underflow");
      return false;
   }

   object->popBackLine();
   return true;
}

ConsoleMethod(MessageVector, pushFrontLine, void, 3, 4, "( msg [ , tag ] ) Use the pushFrontLine method to push a line onto the front of the vector.\n"
   "@param msg The text to add to this control.\n"
   "@param tag An optional tag to tag this line with. If not tag is supplied, a tag of 0 is used.\n"
   "@return No return value.\n"
   "@sa popBackLine, popFrontLine, insertLine, pushBackLine")
{
   U32 tag = 0;
   if (argc == 4)
      tag = dAtoi(argv[3]);

   object->pushFrontLine(argv[2], tag);
}

ConsoleMethod(MessageVector, popFrontLine, bool, 2, 2, "() Use the popFrontLine method to pop a line from the front of the vector, destroying the line.\n"
   "@return No return value.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   if (object->getNumLines() == 0) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::popFrontLine(): underflow");
      return false;
   }

   object->popFrontLine();
   return true;
}


ConsoleMethod(MessageVector, insertLine, bool, 4, 5, "( pos , msg [ , tag ] ) Use the insertLine method to insert a new line into the vector at the specified position. An optional tag may also be applied.\n"
   "@param pos The line at which to insert the new text.\n"
   "@param msg The text to add to this control.\n"
   "@param tag An optional tag to tag this line with. If not tag is supplied, a tag of 0 is used.\n"
   "@return No return value.\n"
   "@sa pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos > object->getNumLines())
      return false;

   S32 tag = 0;
   if (argc == 5)
      tag = dAtoi(argv[4]);

   object->insertLine(pos, argv[3], tag);
   return true;
}

ConsoleMethod(MessageVector, deleteLine, bool, 3, 3, "( lineIndex ) Delete the line at the specified position.\n"
   "@param lineIndex The line to delete in this vector.\n"
   "@return No return value.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos >= object->getNumLines())
      return false;

   object->deleteLine(pos);
   return true;
}

ConsoleMethod(MessageVector, dump, void, 3, 4, "( filename [ , header ] ) Dump the message vector to a file, optionally prefixing the file with a header.\n"
   "@param filename The file to dump this vector to.\n"
   "@param header An optional string containing data to dump to the new file prior to dumping the vector.\n"
   "@return No return value")
{

   if (argc == 4)
      object->dump(argv[2], argv[3]);
   else
      object->dump(argv[2]);
}

ConsoleMethod(MessageVector, getNumLines, S32, 2, 2, "() Use the getNumLines method to get the number of lines in the vector.\n"
   "@return Returns an integer value equal to the line count for this vector.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   return object->getNumLines();
}

ConsoleMethod(MessageVector, getLineTextByTag, const char*, 3, 3, "( tag ) Use the getLineTextByTag method to scan through the lines in the vector, returning the first line that has a matching tag.\n"
   "@param tag An special marker that may have been used when creating lines in the vector.\n"
   "@return Returns the contents of the first line found with a matching tag or NULL indicating no match.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 tag = dAtoi(argv[2]);

   for (U32 i = 0; i < object->getNumLines(); i++)
      if (object->getLine(i).messageTag == tag)
         return object->getLine(i).message;
   return "";
}

ConsoleMethod(MessageVector, getLineIndexByTag, S32, 3, 3, "( tag ) Scan through the vector, returning the line number of the first line that matches the specified tag; else returns -1 if no match was found.\n"
   "@param tag A special marker, possibly embedded in one or more lines in the vector.\n"
   "@return Returns the line number of the first line found with the specified tag, otherwise returns -1.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 tag = dAtoi(argv[2]);

   for (U32 i = 0; i < object->getNumLines(); i++)
      if (object->getLine(i).messageTag == tag)
         return i;
   return -1;
}

ConsoleMethod(MessageVector, getLineText, const char*, 3, 3, "( index ) Use the getLineIndex method to get the text at a specified line.\n"
   "@param index The index in the vector from which to retrieve a line of text.\n"
   "@return Returns the text at the specified line, or NULL indicating a bad index.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos >= object->getNumLines()) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::getLineText(con): out of bounds line");
      return "";
   }

   return object->getLine(pos).message;
}

ConsoleMethod(MessageVector, getLineTag, S32, 3, 3, "(line) Use the getLineTag method to retrieve the tag for the specified line.\n"
   "@param line Line to search for tag in.\n"
   "@return Returns a tag value or 0 indicating no tag found.\n"
   "@sa insertLine, pushBackLine, pushFrontLine")
{
   U32 pos = U32(dAtoi(argv[2]));
   if (pos >= object->getNumLines()) {
      Con::errorf(ConsoleLogEntry::General, "MessageVector::getLineTag(con): out of bounds line");
      return 0;
   }

   return object->getLine(pos).messageTag;
}

extern "C" {
   DLL_PUBLIC MessageVector* MessageVectorCreateInstance()
   {
      return new MessageVector();
   }

   DLL_PUBLIC void MessageVectorClear(MessageVector* messageVector)
   {
      messageVector->clear();
   }

   DLL_PUBLIC void MessageVectorPushBackLine(MessageVector* messageVector, const char* msg, int tag)
   {
      messageVector->pushBackLine(msg, tag);
   }

   DLL_PUBLIC bool MessageVectorPopBackLine(MessageVector* messageVector)
   {
      if (messageVector->getNumLines() <= 0) {
         Con::errorf(ConsoleLogEntry::General, "MessageVector::popBackLine(): underflow");
         return false;
      }

      messageVector->popBackLine();
      return true;
   }

   DLL_PUBLIC void MessageVectorPushFrontLine(MessageVector* messageVector, const char* msg, int tag)
   {
      messageVector->pushFrontLine(msg, tag);
   }

   DLL_PUBLIC bool MessageVectorPopFrontLine(MessageVector* messageVector)
   {
      if (messageVector->getNumLines() <= 0) {
         Con::errorf(ConsoleLogEntry::General, "MessageVector::popFrontLine(): underflow");
         return false;
      }

      messageVector->popFrontLine();
      return true;
   }

   DLL_PUBLIC bool MessageVectorInsertLine(MessageVector* messageVector, int pos, const char* msg, int tag)
   {
      messageVector->insertLine(pos, msg, tag);
      return true;
   }

   DLL_PUBLIC bool MessageVectorDeleteLine(MessageVector* messageVector, U32 lineIndex)
   {
      if (lineIndex >= messageVector->getNumLines())
         return false;

      messageVector->deleteLine(lineIndex);
      return true;
   }

   DLL_PUBLIC void MessageVectorDump(MessageVector* messageVector, const char* filename, const char* header)
   {
      messageVector->dump(filename, header);
   }

   DLL_PUBLIC int MessageVectorGetNumLines(MessageVector* messageVector)
   {
      return messageVector->getNumLines();
   }

   DLL_PUBLIC const char* MessageVectorGetLineTextByTag(MessageVector* messageVector, int tag)
   {
      for (U32 i = 0; i < messageVector->getNumLines(); i++)
         if (messageVector->getLine(i).messageTag == tag)
            return CInterface::GetMarshallableString(messageVector->getLine(i).message);
      return NULL;
   }

   DLL_PUBLIC int MessageVectorGetLineIndexByTag(MessageVector* messageVector, int tag)
   {
      for (U32 i = 0; i < messageVector->getNumLines(); i++)
         if (messageVector->getLine(i).messageTag == tag)
            return i;
      return -1;
   }

   DLL_PUBLIC const char* MessageVectorGetLineText(MessageVector* messageVector, U32 index)
   {
      if (index >= messageVector->getNumLines()) {
         Con::errorf(ConsoleLogEntry::General, "MessageVector::getLineText(con): out of bounds line");
         return nullptr;
      }

      return CInterface::GetMarshallableString(messageVector->getLine(index).message);
   }

   DLL_PUBLIC int MessageVectorGetLineTag(MessageVector* messageVector, U32 index)
   {
      if (index >= messageVector->getNumLines()) {
         Con::errorf(ConsoleLogEntry::General, "MessageVector::getLineText(con): out of bounds line");
         return -1;
      }

      return messageVector->getLine(index).messageTag;
   }
}