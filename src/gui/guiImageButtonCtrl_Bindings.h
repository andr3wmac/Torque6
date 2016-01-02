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

ConsoleMethodGroupBeginWithDocs(GuiImageButtonCtrl, GuiButtonCtrl)

/*! Sets the asset Id the button \up\ state.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiImageButtonCtrl, setNormalImage, ConsoleVoid, 3, 3, (imageAssetId))
{
   object->setNormalImage( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Sets the asset Id the button \hover\ state.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiImageButtonCtrl, setHoverImage, ConsoleVoid, 3, 3, (imageAssetId))
{
   object->setHoverImage( argv[2]  );
}

//-----------------------------------------------------------------------------

/*! Sets the asset Id the button \down\ state.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiImageButtonCtrl, setDownImage, ConsoleVoid, 3, 3, (imageAssetId))
{
   object->setDownImage( argv[2]  );
}

//-----------------------------------------------------------------------------

/*! Sets the asset Id the button \inactive\ state.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiImageButtonCtrl, setInactiveImage, ConsoleVoid, 3, 3, (imageAssetId))
{
   object->setInactiveImage( argv[2]  );
}

//-----------------------------------------------------------------------------

/*! Sets the asset Id the button \inactive\ state.
    @return No return value.
*/
ConsoleMethodWithDocs( GuiImageButtonCtrl, setActive, ConsoleVoid, 3, 3, (imageAssetId))
{
    bool flag = dAtob( argv[2] );
    object->setActive( flag  );
}

ConsoleMethodGroupEndWithDocs(GuiImageButtonCtrl)

extern "C"{
   DLL_PUBLIC GuiImageButtonCtrl* GuiImageButtonCtrlCreateInstance()
   {
      return new GuiImageButtonCtrl();
   }

   DLL_PUBLIC void GuiImageButtonCtrlSetNormalImage(GuiImageButtonCtrl* ctrl, const char* imageAssetId)
   {
      ctrl->setNormalImage(imageAssetId);
   }

   DLL_PUBLIC void GuiImageButtonCtrlSetHoverImage(GuiImageButtonCtrl* ctrl, const char* imageAssetId)
   {
      ctrl->setHoverImage(imageAssetId);
   }

   DLL_PUBLIC void GuiImageButtonCtrlSetDownImage(GuiImageButtonCtrl* ctrl, const char* imageAssetId)
   {
      ctrl->setDownImage(imageAssetId);
   }

   DLL_PUBLIC void GuiImageButtonCtrlSetInactiveImage(GuiImageButtonCtrl* ctrl, const char* imageAssetId)
   {
      ctrl->setInactiveImage(imageAssetId);
   }

   DLL_PUBLIC void GuiImageButtonCtrlSetActive(GuiImageButtonCtrl* ctrl, bool active)
   {
      ctrl->setActive(active);
   }

   DLL_PUBLIC char* GuiImageButtonCtrlGetNormalImage(GuiImageButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getNormalImage());
   }

   DLL_PUBLIC char* GuiImageButtonCtrlGetHoverImage(GuiImageButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getHoverImage());
   }

   DLL_PUBLIC char* GuiImageButtonCtrlGetDownImage(GuiImageButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getDownImage());
   }

   DLL_PUBLIC char* GuiImageButtonCtrlGetInactiveImage(GuiImageButtonCtrl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getInactiveImage());
   }
}