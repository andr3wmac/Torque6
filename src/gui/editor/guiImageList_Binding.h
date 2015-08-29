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

ConsoleMethod(GuiImageList, getImage, const char *, 3, 3, "(int index) Get a path to the texture at the specified index\n"
   "@param index The index of the desired image\n"
   "@return The ID of the image or -1 on failure")
{
   return object->GetTexturePath(dAtoi(argv[2]));
}


ConsoleMethod(GuiImageList, clear, bool, 2, 2, "() Clears the imagelist\n"
   "@return Returns true on success, and false otherwise")
{
   return object->Clear();
}

ConsoleMethod(GuiImageList, count, S32, 2, 2, "Gets the number of images in the list\n"
   "@return Return number of images as an integer")
{
   return object->Count();
}


ConsoleMethod(GuiImageList, remove, bool, 3, 3, "(index) Removes an image from the list by index\n"
   "@param index The index of the image to remove\n"
   "@return Returns true on success, false otherwise")
{
   return object->FreeTextureEntry(dAtoi(argv[2]));
}

ConsoleMethod(GuiImageList, getIndex, S32, 3, 3, "(char* path) Retrieves the imageindex of a specified texture in the list\n"
   "@param path Thge path of the image file to retrieve the indexs of\n"
   "@return The index of the image as an integer or -1 for failure")
{
   return object->IndexFromPath(argv[2]);
}


ConsoleMethod(GuiImageList, insert, S32, 3, 3, "(image path) Insert an image into imagelist\n"
   "@param path The path of the image file\n"
   "@return returns the image index or -1 for failure")
{
   return object->Insert(argv[2]);
}

extern "C"{
   DLL_PUBLIC GuiImageList* GuiImageListCreateInstance()
   {
      return new GuiImageList();
   }

   DLL_PUBLIC const char* GuiImageListGetImage(GuiImageList* list, S32 index)
   {
      return CInterface::GetMarshallableString(list->GetTexturePath(index));
   }

   DLL_PUBLIC bool GuiImageListClear(GuiImageList* list)
   {
      return list->Clear();
   }

   DLL_PUBLIC S32 GuiImageListCount(GuiImageList* list)
   {
      return list->Count();
   }

   DLL_PUBLIC bool GuiImageListRemove(GuiImageList* list, S32 index)
   {
      return list->FreeTextureEntry(index);
   }

   DLL_PUBLIC S32 GuiImageListGetIndex(GuiImageList* list, const char* path)
   {
      return list->IndexFromPath(path);
   }

   DLL_PUBLIC S32 GuiImageListInsert(GuiImageList* list, const char* path)
   {
      return list->Insert(path);
   }
}