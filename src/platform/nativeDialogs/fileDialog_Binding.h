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
#include "platform/nativeDialogs/fileDialog.h"
#include "console/console.h"

#include "c-interface/c-interface.h"

ConsoleMethodGroupBeginWithDocs(FileDialog, SimObject)

/*! Executes the dialog
    @return Returns true on success and false otherwise
*/
ConsoleMethodWithDocs( FileDialog, Execute, ConsoleBool, 2, 2, ())
{
   return object->Execute();
}

ConsoleMethodGroupEndWithDocs(FileDialog)

extern "C"{
   DLL_PUBLIC FileDialog* FileDialogCreateInstance()
   {
      return new FileDialog();
   }

   DLL_PUBLIC void FileDialogSetDefaultPath(FileDialog* fileDialog, const char* value)
   {
      FileDialog::setDefaultPath(fileDialog, value);
   }

   DLL_PUBLIC const char* FileDialogGetDefaultPath(FileDialog* fileDialog)
   {
      return CInterface::GetMarshallableString(fileDialog->getData().mDefaultPath);
   }

   DLL_PUBLIC void FileDialogSetDefaultFile(FileDialog* fileDialog, const char* value)
   {
      FileDialog::setDefaultFile(fileDialog, value);
   }

   DLL_PUBLIC const char* FileDialogGetDefaultFile(FileDialog* fileDialog)
   {
      return CInterface::GetMarshallableString(fileDialog->getData().mDefaultFile);
   }

   DLL_PUBLIC void FileDialogSetFileName(FileDialog* fileDialog, const char* value)
   {
      FileDialog::setFile(fileDialog, value);
   }

   DLL_PUBLIC const char* FileDialogGetFileName(FileDialog* fileDialog)
   {
      return CInterface::GetMarshallableString(fileDialog->getData().mFile);
   }

   DLL_PUBLIC void FileDialogSetFilters(FileDialog* fileDialog, const char* value)
   {
      FileDialog::setFilters(fileDialog, value);
   }

   DLL_PUBLIC const char* FileDialogGetFilters(FileDialog* fileDialog)
   {
      return CInterface::GetMarshallableString(fileDialog->getData().mFilters);
   }

   DLL_PUBLIC void FileDialogSetTitle(FileDialog* fileDialog, const char* value)
   {
      fileDialog->getData().mTitle = value;
   }

   DLL_PUBLIC const char* FileDialogGetTitle(FileDialog* fileDialog)
   {
      return CInterface::GetMarshallableString(fileDialog->getData().mTitle);
   }

   DLL_PUBLIC void FileDialogSetChangePath(FileDialog* fileDialog, bool value)
   {
      if (value)
         fileDialog->getData().mStyle |= FileDialogData::FDS_CHANGEPATH;
      else
         fileDialog->getData().mStyle &= ~FileDialogData::FDS_CHANGEPATH;
   }

   DLL_PUBLIC bool FileDialogGetChangePath(FileDialog* fileDialog)
   {
      return fileDialog->getData().mStyle & FileDialogData::FDS_CHANGEPATH;
   }

   DLL_PUBLIC bool FileDialogExecute(FileDialog* fileDialog)
   {
      return fileDialog->Execute();
   }

   DLL_PUBLIC OpenFileDialog* OpenFileDialogCreateInstance()
   {
      return new OpenFileDialog();
   }

   DLL_PUBLIC void OpenFileDialogSetMustExist(OpenFileDialog* openFileDialog, bool value)
   {
      if (value)
         openFileDialog->getData().mStyle |= FileDialogData::FDS_MUSTEXIST;
      else
         openFileDialog->getData().mStyle &= ~FileDialogData::FDS_MUSTEXIST;
   }

   DLL_PUBLIC bool OpenFileDialogGetMustExist(OpenFileDialog* openFileDialog)
   {
      return openFileDialog->getData().mStyle & FileDialogData::FDS_MUSTEXIST;
   }

   DLL_PUBLIC void OpenFileDialogSetMultipleFiles(OpenFileDialog* openFileDialog, bool value)
   {
      if (value)
         openFileDialog->getData().mStyle |= FileDialogData::FDS_MULTIPLEFILES;
      else
         openFileDialog->getData().mStyle &= ~FileDialogData::FDS_MULTIPLEFILES;
   }

   DLL_PUBLIC bool OpenFileDialogGetMultipleFiles(OpenFileDialog* openFileDialog)
   {
      return openFileDialog->getData().mStyle & FileDialogData::FDS_MULTIPLEFILES;
   }

   DLL_PUBLIC OpenFolderDialog* OpenFolderDialogCreateInstance()
   {
      return new OpenFolderDialog();
   }

   DLL_PUBLIC void OpenFolderDialogSetFileMustExist(OpenFolderDialog* openFolderDialog, const char* value)
   {
      openFolderDialog->mMustExistInDir = value;
   }

   DLL_PUBLIC const char* OpenFolderDialogGetFileMustExist(OpenFolderDialog* openFolderDialog)
   {
      return CInterface::GetMarshallableString(openFolderDialog->mMustExistInDir);
   }

   DLL_PUBLIC SaveFileDialog* SaveFileDialogCreateInstance()
   {
      return new SaveFileDialog();
   }

   DLL_PUBLIC void SaveFileDialogSetOverwritePrompt(SaveFileDialog* saveFileDialog, bool value)
   {
      if (value)
         saveFileDialog->getData().mStyle |= FileDialogData::FDS_OVERWRITEPROMPT;
      else
         saveFileDialog->getData().mStyle &= ~FileDialogData::FDS_OVERWRITEPROMPT;
   }

   DLL_PUBLIC bool SaveFileDialogGetOverwritePrompt(SaveFileDialog* saveFileDialog)
   {
      return saveFileDialog->getData().mStyle & FileDialogData::FDS_OVERWRITEPROMPT;
   }
}