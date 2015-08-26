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
#include "graphics/TextureManager.h"
#include "gui/editor/guiFilterCtrl.h"
#include "gui/editor/guiFilterCtrl_Binding.h"
#include "platform/event.h"
#include "math/mMath.h"

IMPLEMENT_CONOBJECT(GuiFilterCtrl);

GuiFilterCtrl::GuiFilterCtrl()
{
   mControlPointRequest = 7;
   mFilter.setSize(7);
   identity();
}


void GuiFilterCtrl::initPersistFields()
{
   Parent::initPersistFields();
   addField("controlPoints", TypeS32, Offset(mControlPointRequest, GuiFilterCtrl));
   addField("filter", TypeF32Vector, Offset(mFilter, GuiFilterCtrl));
}

bool GuiFilterCtrl::onWake()
{
   if (!Parent::onWake())
      return false;

   if (U32(mControlPointRequest) != mFilter.size())
   {
      mFilter.setSize(mControlPointRequest);
      identity();
   }

   return true;
}


void GuiFilterCtrl::identity()
{
   S32 size = mFilter.size()-1;
   for (U32 i=0; S32(i) <= size; i++)
      mFilter[i] = (F32)i/(F32)size;
}


void GuiFilterCtrl::onMouseDown(const GuiEvent &event)
{
   mouseLock();
   setFirstResponder();

   Point2I p = globalToLocalCoord(event.mousePoint);

   // determine which knot (offset same as in onRender)
   F32 w = F32(mBounds.extent.x-4) / F32(mFilter.size()-1);
   F32 val = (F32(p.x) + (w / 2.f)) / w;
   mCurKnot = S32(val);

   mFilter[mCurKnot] = 1.0f - F32(getMin(getMax(0, p.y), mBounds.extent.y)/(F32)mBounds.extent.y);
   setUpdate();
}


void GuiFilterCtrl::onMouseDragged(const GuiEvent &event)
{
   mouseLock();
   setFirstResponder();

   Point2I p = globalToLocalCoord(event.mousePoint);
   mFilter[mCurKnot] = 1.0f - F32(getMin(getMax(0, p.y), mBounds.extent.y)/(F32)mBounds.extent.y);
   setUpdate();
}

void GuiFilterCtrl::onMouseUp(const GuiEvent &)
{
   mouseUnlock();
   if (mConsoleCommand[0])
      Con::evaluate(mConsoleCommand, false);
}

void GuiFilterCtrl::onPreRender()
{
   if(U32(mControlPointRequest) != mFilter.size())
   {
      mFilter.setSize(mControlPointRequest);
      identity();
      setUpdate();
   }
}

void GuiFilterCtrl::onRender(Point2I offset, const RectI &updateRect)
{
   Point2I pos = offset;
   Point2I ext = mBounds.extent;

   RectI r(pos, ext);
   dglDrawRectFill(r, ColorI(255,255,255));
   dglDrawRect(r, ColorI(0,0,0));

   // shrink by 2 pixels
   pos.x += 2;
   pos.y += 2;
   ext.x -= 4;
   ext.y -= 4;

#if defined(TORQUE_OS_IOS) || defined(TORQUE_OS_ANDROID) || defined(TORQUE_OS_EMSCRIPTEN)
	//this was the same drawing as dglDrawLine		<Mat>
	dglDrawLine( (pos.x), (pos.y+ext.y), (pos.x+ext.x), (pos.y), ColorI(255 *0.9, 255 *0.9, 255 *0.9, 255 *1) );

   // draw the curv
   glColor4f(0.4f, 0.4f, 0.4f, 1.0f);

	GLfloat verts[] = {
		0.0f, 0.0f,
		0.0f, 0.0f		
	};
	glVertexPointer(2, GL_FLOAT, 0, verts );	

	F32 scale = 1.0f/F32(ext.x);

	F32 index = 0;
	S32 y = (S32)(ext.y*(1.0f-mFilter.getValue(index)));
	verts[0] = pos.x;
	verts[1] = pos.y+y;
	
	//unreaveled, i = 0 above
	for (U32 i=1; S32(i) < ext.x; i++)
      {
         index = F32(i)*scale;
         y = (S32)(ext.y*(1.0f-mFilter.getValue(index)));
         //glVertex2i(pos.x+i, pos.y+y );
		  verts[2] = pos.x+i;
		  verts[3] = pos.y+y;

		  // need to draw a line between each point, and the previous point, so save this point for next time (we get the very first point outside this loop)
		  glDrawArrays(GL_LINES, 0, 2);
		  verts[0] = verts[2];
		  verts[1] = verts[3];
      }
#else
/*   
   // draw the identity line
   glColor3f(0.9f, 0.9f, 0.9f);
   glBegin(GL_LINES);
      glVertex2i(pos.x, pos.y+ext.y);
      glVertex2i(pos.x+ext.x, pos.y);
   glEnd();

   // draw the curve
   glColor3f(0.4f, 0.4f, 0.4f);
   glBegin(GL_LINE_STRIP);

      F32 scale = 1.0f/F32(ext.x);
      for (U32 i=0; S32(i) < ext.x; i++)
      {
         F32 index = F32(i)*scale;
         S32 y = (S32)(ext.y*(1.0f-mFilter.getValue(index)));
         glVertex2i(pos.x+i, pos.y+y );
      }
   glEnd();
   */
#endif

   // draw the knots
   for (U32 k=0; k < (U32)mFilter.size(); k++)
   {
      RectI r;
      r.point.x = (S32)(((F32)ext.x/(F32)(mFilter.size()-1)*(F32)k));
      r.point.y = (S32)(ext.y - ((F32)ext.y * mFilter[k]));
      r.point += pos + Point2I(-2,-2);
      r.extent = Point2I(5,5);

      dglDrawRectFill(r, ColorI(255,0,0));
   }

   renderChildControls(offset, updateRect);
}



//--------------------------------------
void Filter::set(S32 argc, const char *argv[])
{
   setSize(0);
   if (argc == 1)
   {  // in the form of one string "1.0 1.0 1.0"
      char list[1024];
      dStrcpy(list, *argv);    // strtok modifies the string so we need to copy it
      char *value = dStrtok(list, " ");
      while (value)
      {
         push_back(dAtof(value));
         value = dStrtok(NULL, " ");
      }
   }
   else
   {  // in the form of seperate strings "1.0" "1.0" "1.0"
      for (; argc ; argc--, argv++)
         push_back(dAtof(*argv));
   }
}


//--------------------------------------
F32 Filter::getValue(F32 x) const
{
   if (size() < 2)
      return 0.0f;

   x = mClampF(x, 0.0f, 1.0f);
   x *= F32(size()-1);

   F32 p0,p1,p2,p3;
   S32 i1 = (S32)mFloor(x);
   S32 i2 = i1+1;
   F32 dt = x - F32(i1);

   p1 = *(begin()+i1);
   p2 = *(begin()+i2);

   if (i1 == 0)
      p0 = p1 + (p1 - p2);
   else
      p0 = *(begin()+i1-1);

   if (i2 == S32(size()-1))
      p3 = p2 + (p2 - p1);
   else
      p3 = *(begin()+i2+1);

   return mClampF( mCatmullrom(dt, p0, p1, p2, p3), 0.0f, 1.0f );
}






