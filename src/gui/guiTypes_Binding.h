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

extern "C" {
   DLL_PUBLIC GuiControlProfile* GuiControlProfileCreateInstance()
   {
      return new GuiControlProfile();
   }

   DLL_PUBLIC bool GuiControlProfileGetTab(GuiControlProfile* controlProfile)
   {
      return controlProfile->mTabable;
   }

   DLL_PUBLIC void GuiControlProfileSetTab(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mTabable = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetCanKeyFocus(GuiControlProfile* controlProfile)
   {
      return controlProfile->mCanKeyFocus;
   }

   DLL_PUBLIC void GuiControlProfileSetCanKeyFocus(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mCanKeyFocus = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetMouseOverSelected(GuiControlProfile* controlProfile)
   {
      return controlProfile->mMouseOverSelected;
   }

   DLL_PUBLIC void GuiControlProfileSetMouseOverSelected(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mMouseOverSelected = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetModal(GuiControlProfile* controlProfile)
   {
      return controlProfile->mModal;
   }

   DLL_PUBLIC void GuiControlProfileSetModal(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mModal = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetOpaque(GuiControlProfile* controlProfile)
   {
      return controlProfile->mOpaque;
   }

   DLL_PUBLIC void GuiControlProfileSetOpaque(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mOpaque = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFillColor(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFillColor);
   }

   DLL_PUBLIC void GuiControlProfileSetFillColor(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFillColor = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFillColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFillColorHL);
   }

   DLL_PUBLIC void GuiControlProfileSetFillColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFillColorHL = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFillColorNA(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFillColorNA);
   }

   DLL_PUBLIC void GuiControlProfileSetFillColorNA(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFillColorNA = value;
   }

   DLL_PUBLIC int GuiControlProfileGetBorder(GuiControlProfile* controlProfile)
   {
      return controlProfile->mBorder;
   }

   DLL_PUBLIC void GuiControlProfileSetBorder(GuiControlProfile* controlProfile, int value)
   {
      controlProfile->mBorder = value;
   }

   DLL_PUBLIC int GuiControlProfileGetBorderThickness(GuiControlProfile* controlProfile)
   {
      return controlProfile->mBorderThickness;
   }

   DLL_PUBLIC void GuiControlProfileSetBorderThickness(GuiControlProfile* controlProfile, int value)
   {
      controlProfile->mBorderThickness = value;
   }

   DLL_PUBLIC void GuiControlProfileGetBorderColor(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mBorderColor);
   }

   DLL_PUBLIC void GuiControlProfileSetBorderColor(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mBorderColor = value;
   }

   DLL_PUBLIC void GuiControlProfileGetBorderColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mBorderColorHL);
   }

   DLL_PUBLIC void GuiControlProfileSetBorderColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mBorderColorHL = value;
   }

   DLL_PUBLIC void GuiControlProfileGetBorderColorNA(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mBorderColorNA);
   }

   DLL_PUBLIC void GuiControlProfileSetBorderColorNA(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mBorderColorNA = value;
   }

   DLL_PUBLIC void GuiControlProfileGetBevelColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mBevelColorHL);
   }

   DLL_PUBLIC void GuiControlProfileSetBevelColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mBevelColorHL = value;
   }

   DLL_PUBLIC void GuiControlProfileGetBevelColorLL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mBevelColorLL);
   }

   DLL_PUBLIC void GuiControlProfileSetBevelColorLL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mBevelColorLL = value;
   }

   DLL_PUBLIC const char* GuiControlProfileGetFontType(GuiControlProfile* controlProfile)
   {
      return CInterface::GetMarshallableString(controlProfile->mFontType);
   }

   DLL_PUBLIC void GuiControlProfileSetFontType(GuiControlProfile* controlProfile, const char* value)
   {
      controlProfile->mFontType = value;
   }

   DLL_PUBLIC int GuiControlProfileGetFontSize(GuiControlProfile* controlProfile)
   {
      return controlProfile->mFontSize;
   }

   DLL_PUBLIC void GuiControlProfileSetFontSize(GuiControlProfile* controlProfile, int value)
   {
      controlProfile->mFontSize = value;
   }

   DLL_PUBLIC int GuiControlProfileGetFontCharset(GuiControlProfile* controlProfile)
   {
      return controlProfile->mFontCharset;
   }

   DLL_PUBLIC void GuiControlProfileSetFontCharset(GuiControlProfile* controlProfile, int value)
   {
      controlProfile->mFontCharset = (FontCharset)value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColors(GuiControlProfile* controlProfile, int index, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[index]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColors(GuiControlProfile* controlProfile, int index, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[index] = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColor(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[GuiControlProfile::BaseColor]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColor(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[GuiControlProfile::BaseColor] = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[GuiControlProfile::ColorHL]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColorHL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[GuiControlProfile::ColorHL] = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColorNA(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[GuiControlProfile::ColorNA]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColorNA(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[GuiControlProfile::ColorNA] = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColorSEL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[GuiControlProfile::ColorSEL]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColorSEL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[GuiControlProfile::ColorSEL] = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColorLink(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[GuiControlProfile::ColorUser0]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColorLink(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[GuiControlProfile::ColorUser0] = value;
   }

   DLL_PUBLIC void GuiControlProfileGetFontColorLinkHL(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mFontColors[GuiControlProfile::ColorUser1]);
   }

   DLL_PUBLIC void GuiControlProfileSetFontColorLinkHL(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mFontColors[GuiControlProfile::ColorUser1] = value;
   }

   DLL_PUBLIC int GuiControlProfileGetJustify(GuiControlProfile* controlProfile)
   {
      return controlProfile->mAlignment;
   }

   DLL_PUBLIC void GuiControlProfileSetJustify(GuiControlProfile* controlProfile, int value)
   {
      controlProfile->mAlignment = (GuiControlProfile::AlignmentType)value;
   }

   DLL_PUBLIC Point2I GuiControlProfileGetTextOffset(GuiControlProfile* controlProfile)
   {
      return controlProfile->mTextOffset;
   }

   DLL_PUBLIC void GuiControlProfileSetTextOffset(GuiControlProfile* controlProfile, CInterface::Point2IParam value)
   {
      controlProfile->mTextOffset = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetAutoSizeWidth(GuiControlProfile* controlProfile)
   {
      return controlProfile->mAutoSizeWidth;
   }

   DLL_PUBLIC void GuiControlProfileSetAutoSizeWidth(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mAutoSizeWidth = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetAutoSizeHeight(GuiControlProfile* controlProfile)
   {
      return controlProfile->mAutoSizeHeight;
   }

   DLL_PUBLIC void GuiControlProfileSetAutoSizeHeight(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mAutoSizeHeight = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetReturnTab(GuiControlProfile* controlProfile)
   {
      return controlProfile->mReturnTab;
   }

   DLL_PUBLIC void GuiControlProfileSetReturnTab(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mReturnTab = value;
   }

   DLL_PUBLIC bool GuiControlProfileGetNumbersOnly(GuiControlProfile* controlProfile)
   {
      return controlProfile->mNumbersOnly;
   }

   DLL_PUBLIC void GuiControlProfileSetNumbersOnly(GuiControlProfile* controlProfile, bool value)
   {
      controlProfile->mNumbersOnly = value;
   }

   DLL_PUBLIC void GuiControlProfileGetCursorColor(GuiControlProfile* controlProfile, CInterface::ColorParam* outColor)
   {
      *outColor = CInterface::ColorParam(controlProfile->mCursorColor);
   }

   DLL_PUBLIC void GuiControlProfileSetCursorColor(GuiControlProfile* controlProfile, CInterface::ColorParam value)
   {
      controlProfile->mCursorColor = value;
   }

   DLL_PUBLIC const char* GuiControlProfileGetBitmap(GuiControlProfile* controlProfile)
   {
      return CInterface::GetMarshallableString(controlProfile->mBitmapName);
   }

   DLL_PUBLIC void GuiControlProfileSetBitmap(GuiControlProfile* controlProfile, const char* value)
   {
      controlProfile->mBitmapName = value;
   }

   DLL_PUBLIC AudioAsset* GuiControlProfileGetSoundButtonDown(GuiControlProfile* controlProfile)
   {
      return controlProfile->mSoundButtonDown;
   }

   DLL_PUBLIC void GuiControlProfileSetSoundButtonDown(GuiControlProfile* controlProfile, AudioAsset* value)
   {
      controlProfile->mSoundButtonDown = value->getAssetId();
   }

   DLL_PUBLIC AudioAsset* GuiControlProfileGetSoundButtonOver(GuiControlProfile* controlProfile)
   {
      return controlProfile->mSoundButtonOver;
   }

   DLL_PUBLIC void GuiControlProfileSetSoundButtonOver(GuiControlProfile* controlProfile, AudioAsset* value)
   {
      controlProfile->mSoundButtonOver = value->getAssetId();
   }

   DLL_PUBLIC GuiControlProfile* GuiControlProfileGetProfileForChildren(GuiControlProfile* controlProfile)
   {
      return controlProfile->mProfileForChildren;
   }

   DLL_PUBLIC void GuiControlProfileSetProfileForChildren(GuiControlProfile* controlProfile, GuiControlProfile* value)
   {
      controlProfile->mProfileForChildren = value;
   }   
   
   DLL_PUBLIC GuiCursor* GuiCursorCreateInstance()
   {
      return new GuiCursor();
   }

   DLL_PUBLIC void GuiCursorGetHotSpot(GuiCursor* guiCursor, CInterface::Point2IParam* outPoint)
   {
      *outPoint = CInterface::Point2IParam(guiCursor->getHotSpot());
   }

   DLL_PUBLIC void GuiCursorSetHotSpot(GuiCursor* guiCursor, CInterface::Point2IParam value)
   {
      guiCursor->setHotSpot(value);
   }

   DLL_PUBLIC void GuiCursorGetRenderOffset(GuiCursor* guiCursor, CInterface::Point2FParam* outPoint)
   {
      *outPoint = CInterface::Point2FParam(guiCursor->getRenderOffset());
   }

   DLL_PUBLIC void GuiCursorSetRenderOffset(GuiCursor* guiCursor, CInterface::Point2FParam value)
   {
      guiCursor->setRenderOffset(value);
   }

   DLL_PUBLIC const char* GuiCursorGetBitmapName(GuiCursor* guiCursor)
   {
      return CInterface::GetMarshallableString(guiCursor->getBitmapName());
   }

   DLL_PUBLIC void GuiCursorSetBitmapName(GuiCursor* guiCursor, const char* value)
   {
      guiCursor->setBitmapName(value);
   }
}