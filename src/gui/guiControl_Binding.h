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

ConsoleMethodGroupBeginWithDocs(GuiControl, SimGroup)

/*! Sets whether this control can serialize itself to the hard disk
    @param Flag setting
    @return No Return Value
*/
ConsoleMethodWithDocs( GuiControl, setCanSave, ConsoleVoid, 3,3, (bool canSave))
{
   object->setCanSave( dAtob( argv[2] ) );
}

/*! Check if point id in the control
    @param x Point x coordinate in parent coords
    @param y Point y coordinate in parent coords
    @return Returns true if the point is in the control, false otherwise
*/
ConsoleMethodWithDocs(GuiControl, pointInControl, ConsoleBool, 4,4, (int x, int y))
{
   Point2I kPoint(dAtoi(argv[2]), dAtoi(argv[3]));
   return object->pointInControl(kPoint);
}


// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=- //
/*! Adds the gui control
    @param controlId integer ID of the control to add
    @return No Return value
*/
ConsoleMethodWithDocs( GuiControl, addGuiControl, ConsoleVoid, 3, 3, (int controlId))
{

   GuiControl* ctrl = dynamic_cast<GuiControl* >(Sim::findObject(argv[2]));
   if(ctrl)
   {
      object->addObject(ctrl);
   }

}

//-----------------------------------------------------------------------------
//	Make Sure Child 1 is Ordered Just Under Child 2.
//-----------------------------------------------------------------------------
/*! uses simset reorder to push child 1 after child 2 - both must already be child controls of this control
*/
ConsoleMethodWithDocs(GuiControl, reorderChild, ConsoleVoid, 4,4,  (child1, child2))
{
   GuiControl* pControl = dynamic_cast<GuiControl*>(Sim::findObject(dAtoi(argv[2])));
   GuiControl* pTarget	 = dynamic_cast<GuiControl*>(Sim::findObject(dAtoi(argv[3])));

   if(pControl && pTarget)
   {
      object->reOrder(pControl,pTarget);
   }
}

/*! @return Returns the Id of the parent control
*/
ConsoleMethodWithDocs( GuiControl, getParent, ConsoleInt, 2, 2, ())
{

   GuiControl* pParent		= object->getParent();
   if(pParent)
   {
      return pParent->getId();
   }

   return 0;

}

/*! Use the setValue method to set the control specific value to value. Purpose and type varies by control type.
    @param value Some control specific value.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setValue, ConsoleVoid, 3, 3, ( value ))
{
   object->setScriptValue(argv[2]);
}

/*! Use the getValue method to get the control-specific 'value' for this control.
    @return Returns a control-specific specific value. Varies by control
*/
ConsoleMethodWithDocs( GuiControl, getValue, ConsoleString, 2, 2, ())
{
   return object->getScriptValue();
}

/*! Use the setActive method to (de)activate this control. Once active, a control can accept inputs. Controls automatically re-shade/skin themselves to reflect their active/inactive state.
    @param isActive A boolean value. f isActive is true, this control is activated, else it is set to inactive.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setActive, ConsoleVoid, 3, 3, ( isActive ))
{
   object->setActive(dAtob(argv[2]));
}

/*! Use the isActive method to determine if this control is active.
    An inactive control may visible, but will not accept inputs. It will also normally re-shade or re-skin itself to reflect its inactive state
    @return Returns true if this control is active.
*/
ConsoleMethodWithDocs( GuiControl, isActive, ConsoleBool, 2, 2, ())
{
   return object->isActive();
}

/*! Use the setVisible method to (un)hide this control.
    @param isVisible A boolean value. If true, the control will be made visible, otherwise the control will be hidden.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setVisible, ConsoleVoid, 3, 3, ( isVisible ))
{
   object->setVisible(dAtob(argv[2]));
}

/*! Use the makeFirstResponder method to force this control to become the first responder.
    @param isFirst A boolean value. If true, then this control become first reponder and at captures inputs before all other controls, excluding dialogs above this control.
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, makeFirstResponder, ConsoleVoid, 3, 3, ( isFirst ))
{
   object->makeFirstResponder(dAtob(argv[2]));
}

/*! Use the isVisible method to determine if this control is visible.
    This can return true, even if the entire control covered by another. This merely means that the control will render if not covered
    @return Returns true if the control is visible.
*/
ConsoleMethodWithDocs( GuiControl, isVisible, ConsoleBool, 2, 2, ())
{
   return object->isVisible();
}

/*! Use the isAwake method to determine if this control is awake.
    @return Returns true if this control is awake and ready to display
*/
ConsoleMethodWithDocs( GuiControl, isAwake, ConsoleBool, 2, 2, ())
{
   return object->isAwake();
}

/*! Sets the currently used from for the GuiControl
    @param p The profile you wish to set the control to use
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setProfile, ConsoleVoid, 3, 3, (GuiControlProfile p))
{
   GuiControlProfile * profile;

   if(Sim::findObject(argv[2], profile))
      object->setControlProfile(profile);
}

/*! Resizes the control to the given dimensions
*/
ConsoleMethodWithDocs( GuiControl, resize, ConsoleVoid, 6, 6, (int x, int y, int w, int h))
{
   Point2I newPos(dAtoi(argv[2]), dAtoi(argv[3]));
   Point2I newExt(dAtoi(argv[4]), dAtoi(argv[5]));
   object->resize(newPos, newExt);
}

/*! @return A string set up as \<pos.x> <pos.y>\
*/
ConsoleMethodWithDocs( GuiControl, getPosition, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &pos = object->getPosition();
   dSprintf(retBuffer, 64, "%d %d", pos.x, pos.y);
   return retBuffer;
}
/*! @return Returns center of control, as space seperated ints
*/
ConsoleMethodWithDocs( GuiControl, getCenter, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I pos = object->getPosition();
   const Point2I ext = object->getExtent();
   Point2I center(pos.x + ext.x/2, pos.y + ext.y/2);
   dSprintf(retBuffer, 64, "%d %d", center.x, center.y);
   return retBuffer;
}

/*! Sets control position, by center - coords are local not global
    @return No Return value.
*/
ConsoleMethodWithDocs( GuiControl, setCenter, ConsoleVoid, 4, 4, (int x, int y))
{
   const Point2I ext = object->getExtent();
   Point2I newpos(dAtoi(argv[2])-ext.x/2, dAtoi(argv[3])-ext.y/2);
   object->setPosition(newpos);
}

/*! @return Returns center of control, as space seperated ints
*/
ConsoleMethodWithDocs( GuiControl, getGlobalCenter, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I tl(0,0);
   Point2I pos		 =	object->localToGlobalCoord(tl);
   const Point2I ext = object->getExtent();
   Point2I center(pos.x + ext.x/2, pos.y + ext.y/2);
   dSprintf(retBuffer, 64, "%d %d", center.x, center.y);
   return retBuffer;
}

/*! @return Returns the control's position converted to global coordinates (position as space-separted integers)
*/
ConsoleMethodWithDocs( GuiControl, getGlobalPosition, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I pos(0,0);
   Point2I gPos	=	object->localToGlobalCoord(pos);

   dSprintf(retBuffer, 64, "%d %d", gPos.x, gPos.y);
   return retBuffer;
}
/*! Sets the control's position in global space
    @return No return value
*/
ConsoleMethodWithDocs( GuiControl, setPositionGlobal, ConsoleVoid, 4, 4, (int x, int y))
{
   //see if we can turn the x/y into ints directly, 
   Point2I gPos(dAtoi(argv[2]), dAtoi(argv[3]));
   Point2I lPosOffset	=	object->globalToLocalCoord(gPos);
   lPosOffset.x += object->mBounds.point.x;
   lPosOffset.y += object->mBounds.point.y;

   object->mBounds.set(lPosOffset,object->mBounds.extent);
}

/*! Sets the current control position in local space
    @return No Return Value.
*/
ConsoleMethodWithDocs( GuiControl, setPosition, ConsoleVoid, 4, 4, (int x, int y))
{
   //see if we can turn the x/y into ints directly, 
   Point2I lPos(dAtoi(argv[2]), dAtoi(argv[3]));
   object->mBounds.set(lPos,object->mBounds.extent);
}

/*! Get the width and height of the control.
    @return The height and width as a string with space-separated integers
*/
ConsoleMethodWithDocs( GuiControl, getExtent, ConsoleString, 2, 2, (...))
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &ext = object->getExtent();
   dSprintf(retBuffer, 64, "%d %d", ext.x, ext.y);
   return retBuffer;
}

/*! Sets the width & height of the control.
    @return No Return Value.
*/
ConsoleMethodWithDocs( GuiControl, setExtent, ConsoleVoid, 4, 4, (int width, int height))
{
   Point2I kExt(dAtoi(argv[2]), dAtoi(argv[3]));
   object->setExtent(kExt);
}

/*! Get the minimum allowed size of the control.
    @return Returns the minimum extent as a string with space separated point values <width> <height>
*/
ConsoleMethodWithDocs( GuiControl, getMinExtent, ConsoleString, 2, 2, ())
{
   char *retBuffer = Con::getReturnBuffer(64);
   const Point2I &minExt = object->getMinExtent();
   dSprintf(retBuffer, 64, "%d %d", minExt.x, minExt.y);
   return retBuffer;
}

/*! Searches for the control at the given point 
    @return Returns the Id of the control at the point
*/
ConsoleMethodWithDocs( GuiControl, findHitControl, ConsoleInt, 4, 4, (int x, int y))
{
   Point2I pos(dAtoi(argv[2]), dAtoi(argv[3]));
   GuiControl* hit = object->findHitControl(pos);
   return hit ? hit->getId() : 0;
}

/*! Sets this control as the first responder
*/
ConsoleMethodWithDocs(GuiControl, setFirstResponder, ConsoleVoid, 2, 2, ())
{
   object->setFirstResponder();
}

ConsoleMethodGroupEndWithDocs(GuiControl)

extern "C"{
   DLL_PUBLIC GuiControl* GuiControlCreateInstance()
   {
      return new GuiControl();
   }

   DLL_PUBLIC void GuiControlSetCanSave(GuiControl* ctrl, bool canSave)
   {
      ctrl->setCanSave(canSave);
   }

   DLL_PUBLIC bool GuiControlPointInControl(GuiControl* ctrl, CInterface::Point2IParam point)
   {
      return ctrl->pointInControl(point);
   }

   DLL_PUBLIC void GuiControlAddGuiControl(GuiControl* ctrl, GuiControl* guiCtrl)
   {
      ctrl->addObject(guiCtrl);
   }

   DLL_PUBLIC void GuiControlReorderChild(GuiControl* ctrl, GuiControl* child1, GuiControl* child2)
   {
      if (child1 && child2)
      {
         ctrl->reOrder(child1, child2);
      }
   }

   DLL_PUBLIC GuiControl* GuiControlGetParent(GuiControl* ctrl)
   {
      return ctrl->getParent();
   }

   DLL_PUBLIC void GuiControlSetValue(GuiControl* ctrl, const char* value)
   {
      ctrl->setScriptValue(value);
   }

   DLL_PUBLIC char* GuiControlGetValue(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getScriptValue());
   }

   DLL_PUBLIC void GuiControlSetActive(GuiControl* ctrl, bool isActive)
   {
      ctrl->setActive(isActive);
   }

   DLL_PUBLIC bool GuiControlIsActive(GuiControl* ctrl)
   {
      return ctrl->isActive();
   }

   DLL_PUBLIC void GuiControlSetVisible(GuiControl* ctrl, bool isVisible)
   {
      ctrl->setVisible(isVisible);
   }

   DLL_PUBLIC bool GuiControlIsVisible(GuiControl* ctrl)
   {
      return ctrl->isVisible();
   }

   DLL_PUBLIC void GuiControlMakeFirstResponder(GuiControl* ctrl, bool isFirst)
   {
      ctrl->makeFirstResponder(isFirst);
   }

   DLL_PUBLIC bool GuiControlIsAwake(GuiControl* ctrl)
   {
      return ctrl->isAwake();
   }

   DLL_PUBLIC void GuiControlSetProfile(GuiControl* ctrl, GuiControlProfile* profile)
   {
      if (profile)
         ctrl->setControlProfile(profile);
   }

   DLL_PUBLIC void GuiControlResize(GuiControl* ctrl, CInterface::Point2IParam newPos, CInterface::Point2IParam newExt)
   {
      ctrl->resize(newPos, newExt);
   }

   DLL_PUBLIC void GuiControlGetPosition(GuiControl* ctrl, CInterface::Point2IParam* outPos)
   {
      *outPos = ctrl->getPosition();
   }

   DLL_PUBLIC void GuiControlGetCenter(GuiControl* ctrl, CInterface::Point2IParam* outPos)
   {
      const Point2I pos = ctrl->getPosition();
      const Point2I ext = ctrl->getExtent();
      *outPos = Point2I(pos.x + ext.x / 2, pos.y + ext.y / 2);
   }

   DLL_PUBLIC void GuiControlSetCenter(GuiControl* ctrl, CInterface::Point2IParam newCenter)
   {
      const Point2I ext = ctrl->getExtent();
      Point2I newpos(newCenter.x - ext.x / 2, newCenter.y - ext.y / 2);
      ctrl->setPosition(newpos);
   }

   DLL_PUBLIC void GuiControlGetGlobalCenter(GuiControl* ctrl, CInterface::Point2IParam* outPos)
   {
      const Point2I tl(0, 0);
      Point2I pos = ctrl->localToGlobalCoord(tl);
      const Point2I ext = ctrl->getExtent();
      *outPos = Point2I(pos.x + ext.x / 2, pos.y + ext.y / 2);
   }

   DLL_PUBLIC void GuiControlGetGlobalPosition(GuiControl* ctrl, CInterface::Point2IParam* outPos)
   {
      const Point2I pos(0, 0);
      *outPos = ctrl->localToGlobalCoord(pos);
   }

   DLL_PUBLIC void GuiControlSetPositionGlobal(GuiControl* ctrl, CInterface::Point2IParam pos)
   {
      Point2I lPosOffset = ctrl->globalToLocalCoord(pos);
      lPosOffset.x += ctrl->mBounds.point.x;
      lPosOffset.y += ctrl->mBounds.point.y;

      ctrl->mBounds.set(lPosOffset, ctrl->mBounds.extent);
   }

   DLL_PUBLIC void GuiControlSetPosition(GuiControl* ctrl, CInterface::Point2IParam pos)
   {
      ctrl->mBounds.set(pos, ctrl->mBounds.extent);
   }

   DLL_PUBLIC void GuiControlGetExtent(GuiControl* ctrl, CInterface::Point2IParam* outExt)
   {
      *outExt = ctrl->getExtent();
   }

   DLL_PUBLIC void GuiControlSetExtent(GuiControl* ctrl, CInterface::Point2IParam ext)
   {
      ctrl->setExtent(ext);
   }

   DLL_PUBLIC void GuiControlGetMinExtent(GuiControl* ctrl, CInterface::Point2IParam* outExt)
   {
      *outExt = ctrl->getMinExtent();
   }

   DLL_PUBLIC void GuiControlSetMinExtent(GuiControl* ctrl, CInterface::Point2IParam ext)
   {
      ctrl->setMinExtent(ext);
   }

   DLL_PUBLIC GuiControl* GuiControlFindHitControl(GuiControl* ctrl, CInterface::Point2IParam pos)
   {
      return ctrl->findHitControl(pos);
   }

   DLL_PUBLIC void GuiControlSetFirstResponder(GuiControl* ctrl)
   {
      ctrl->setFirstResponder();
   }

   DLL_PUBLIC bool GuiControlGetIsContainer(GuiControl* ctrl)
   {
      return ctrl->mIsContainer;
   }

   DLL_PUBLIC void GuiControlSetIsContainer(GuiControl* ctrl, bool container)
   {
      ctrl->mIsContainer = container;
   }

   DLL_PUBLIC GuiControlProfile* GuiControlGetProfile(GuiControl* ctrl)
   {
      return ctrl->mProfile;
   }

   DLL_PUBLIC S32 GuiControlGetHorizSizing(GuiControl* ctrl)
   {
      return ctrl->getHorizSizing();
   }

   DLL_PUBLIC void GuiControlSetHorizSizing(GuiControl* ctrl, S32 sizing)
   {
      ctrl->setHorizSizing(sizing);
   }

   DLL_PUBLIC S32 GuiControlGetVertSizing(GuiControl* ctrl)
   {
      return ctrl->getVertSizing();
   }

   DLL_PUBLIC void GuiControlSetVertSizing(GuiControl* ctrl, S32 sizing)
   {
      ctrl->setVertSizing(sizing);
   }

   DLL_PUBLIC bool GuiControlGetCanSave(GuiControl* ctrl)
   {
      return ctrl->getCanSave();
   }

   DLL_PUBLIC bool GuiControlGetVisible(GuiControl* ctrl)
   {
      return ctrl->isVisible();
   }

   DLL_PUBLIC char* GuiControlGetVariable(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getConsoleVariable());
   }

   DLL_PUBLIC void GuiControlSetVariable(GuiControl* ctrl, const char* variable)
   {
      ctrl->setConsoleVariable(variable);
   }

   DLL_PUBLIC char* GuiControlGetCommand(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getConsoleCommand());
   }

   DLL_PUBLIC void GuiControlSetCommand(GuiControl* ctrl, const char* command)
   {
      ctrl->setConsoleCommand(command);
   }

   DLL_PUBLIC char* GuiControlGetAltCommand(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getAltConsoleCommand());
   }

   DLL_PUBLIC void GuiControlSetAltCommand(GuiControl* ctrl, const char* command)
   {
      ctrl->setAltConsoleCommand(command);
   }

   DLL_PUBLIC char* GuiControlGetAccelerator(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getAcceleratorKey());
   }

   DLL_PUBLIC void GuiControlSetAccelerator(GuiControl* ctrl, const char* acceleratorKey)
   {
      ctrl->setAcceleratorKey(acceleratorKey);
   }

   DLL_PUBLIC bool GuiControlGetActive(GuiControl* ctrl)
   {
      return ctrl->isActive();
   }

   DLL_PUBLIC GuiControlProfile* GuiControlGetTooltipProfile(GuiControl* ctrl)
   {
      return ctrl->mTooltipProfile;
   }

   DLL_PUBLIC void GuiControlSetTooltipProfile(GuiControl* ctrl, GuiControlProfile* profile)
   {
      ctrl->mTooltipProfile = profile;
   }

   DLL_PUBLIC char* GuiControlGetTooltip(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->getTooltip());
   }

   DLL_PUBLIC void GuiControlSetTooltip(GuiControl* ctrl, const char* tooltip)
   {
      ctrl->setTooltip(tooltip);
   }

   DLL_PUBLIC S32 GuiControlGetTooltipWidth(GuiControl* ctrl)
   {
      return ctrl->mTooltipWidth;
   }

   DLL_PUBLIC void GuiControlSetTooltipWidth(GuiControl* ctrl, S32 width)
   {
      ctrl->mTooltipWidth = width;
   }

   DLL_PUBLIC S32 GuiControlGetHoverTime(GuiControl* ctrl)
   {
      return ctrl->mTipHoverTime;
   }

   DLL_PUBLIC void GuiControlSetHoverTime(GuiControl* ctrl, S32 time)
   {
      ctrl->mTipHoverTime = time;
   }

   DLL_PUBLIC char* GuiControlGetLangTableMod(GuiControl* ctrl)
   {
      return CInterface::GetMarshallableString(ctrl->mLangTableName);
   }

   DLL_PUBLIC void GuiControlSetLangTableMod(GuiControl* ctrl, const char* langTableName)
   {
      ctrl->mLangTableName = langTableName;
   }
}