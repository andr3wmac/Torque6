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

#include "console/console.h"
#include "console/consoleTypes.h"
#include "graphics/dgl.h"
#include <bgfx.h>
#include <bx/fpumath.h>
#include <imgui/imgui.h>

#include "gui/guiCurveCtrl.h"

IMPLEMENT_CONOBJECT(GuiCurveCtrl);

GuiCurveCtrl::GuiCurveCtrl(void)
{
	mColor.set(255, 255, 255, 255);
	mThickness = 5.0f;
}

void GuiCurveCtrl::initPersistFields()
{
	Parent::initPersistFields();
	addGroup("GuiCurveCtrl");	

		addField("start",		TypePoint2I, Offset(mStart, GuiCurveCtrl));
		addField("end",			TypePoint2I, Offset(mEnd, GuiCurveCtrl));
		addField("controlA",	TypePoint2I, Offset(mControlPointA, GuiCurveCtrl));
		addField("controlB",	TypePoint2I, Offset(mControlPointB, GuiCurveCtrl));
		addField("color",		TypeColorI,		Offset(mColor, GuiCurveCtrl));
		addField("thickness",	TypeF32,		Offset(mThickness, GuiCurveCtrl));

	endGroup("GuiCurveCtrl");		
}

bool GuiCurveCtrl::onWake()
{
   if (! Parent::onWake())
      return false;
   setActive(true);
   return true;
}

void GuiCurveCtrl::onSleep()
{
   Parent::onSleep();
}

//-------------------------------------
void GuiCurveCtrl::inspectPostApply()
{
   // if the extent is set to (0,0) in the gui editor and appy hit, this control will
   // set it's extent to be exactly the size of the bitmap (if present)
   Parent::inspectPostApply();
}

void GuiCurveCtrl::onRender(Point2I offset, const RectI &updateRect)
{
	NVGcontext* vg = dglGetNVGContext();
	if (vg)
	{
		nvgBeginPath(vg);
		nvgMoveTo(vg, (F32)(offset.x + mStart.x), (F32)(offset.y + mStart.y));
		nvgBezierTo(vg, (F32)(offset.x + mControlPointA.x), (F32)(offset.y + mControlPointA.y), 
                      (F32)(offset.x + mControlPointB.x), (F32)(offset.y + mControlPointB.y), 
                      (F32)(offset.x + mEnd.x), (F32)(offset.y + mEnd.y));
		nvgStrokeColor(vg, nvgRGBA(mColor.red, mColor.green, mColor.blue, mColor.alpha));
		nvgStrokeWidth(vg, mThickness);
		nvgStroke(vg);
	}

	if (mProfile->mBorder)
	{
		RectI rect(offset.x, offset.y, mBounds.extent.x, mBounds.extent.y);
		dglDrawRect(rect, mProfile->mBorderColor);
	}

	renderChildControls(offset, updateRect);
}