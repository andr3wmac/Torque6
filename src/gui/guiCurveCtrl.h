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

#ifndef _GuiCurveCtrl_H_
#define _GuiCurveCtrl_H_

#ifndef _GUICONTROL_H_
#include "gui/guiControl.h"
#endif
#ifndef _TEXTURE_MANAGER_H_
#include "graphics/TextureManager.h"
#endif


/// Renders a bitmap.
class GuiCurveCtrl : public GuiControl
{
private:
   typedef GuiControl Parent;

protected:
   ColorI   mColor;
   F32      mThickness;
   Point2I  mStart;
   Point2I  mEnd;
   Point2I  mControlPointA;
   Point2I  mControlPointB;

public:
   //creation methods
   DECLARE_CONOBJECT(GuiCurveCtrl);
   GuiCurveCtrl();
   static void initPersistFields();

   //Parental methods
   bool onWake();
   void onSleep();
   void inspectPostApply();

   ColorI getColor() { return mColor; }
   void setColor(ColorI color) { mColor = color; }
   F32 getThickness() { return mThickness; }
   void setThickness(F32 thickness) { mThickness = thickness; }
   Point2I getStart() { return mStart; }
   void setStart(Point2I point) { mStart = point; }
   Point2I getEnd() { return mEnd; }
   void setEnd(Point2I point) { mEnd = point; }
   Point2I getControlPointA() { return mControlPointA; }
   void setControlPointA(Point2I point) { mControlPointA = point; }
   Point2I getControlPointB() { return mControlPointB; }
   void setControlPointB(Point2I point) { mControlPointB = point; }

   //Luma:	ability to specify source rect for image UVs
   void setSourceRect(U32 x, U32 y, U32 width, U32 height);
   void setUseSourceRect(bool bUse);

   void onRender(Point2I offset, const RectI &updateRect);
};

#endif
