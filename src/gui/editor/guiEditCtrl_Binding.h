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

ConsoleMethod(GuiEditCtrl, setRoot, void, 3, 3, "(GuiControl root) Sets the given control as root\n"
   "@return No return value.")
{
   GuiControl *ctrl;
   if (!Sim::findObject(argv[2], ctrl))
      return;
   object->setRoot(ctrl);
}


ConsoleMethod(GuiEditCtrl, addNewCtrl, void, 3, 3, "(GuiControl ctrl) Adds the given control to the control list\n"
   "@return No return value.")
{
   GuiControl *ctrl;
   if (!Sim::findObject(argv[2], ctrl))
      return;
   object->addNewControl(ctrl);
}
ConsoleMethod(GuiEditCtrl, addSelection, void, 3, 3, "(ctrlID) Adds the selected control.\n"
   "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->addSelection(id);
}
ConsoleMethod(GuiEditCtrl, removeSelection, void, 3, 3, "(ctrlID) Removes the selected control from list.\n"
   "@return No return value.")
{
   S32 id = dAtoi(argv[2]);
   object->removeSelection(id);
}

ConsoleMethod(GuiEditCtrl, clearSelection, void, 2, 2, "() Clear selected controls list.\n"
   "@return No return value.")
{
   object->clearSelection();
}
ConsoleMethod(GuiEditCtrl, select, void, 3, 3, "(GuiControl ctrl) Finds and selects given object\n"
   "@return No return value.")
{
   GuiControl *ctrl;

   if (!Sim::findObject(argv[2], ctrl))
      return;

   object->setSelection(ctrl, false);
}

ConsoleMethod(GuiEditCtrl, setCurrentAddSet, void, 3, 3, "(GuiControl ctrl) Set the current control set in which controls are added.\n"
   "@param ctrl The addset\n"
   "@return No return value.")
{
   GuiControl *addSet;

   if (!Sim::findObject(argv[2], addSet))
   {
      Con::printf("%s(): Invalid control: %s", argv[0], argv[2]);
      return;
   }
   object->setCurrentAddSet(addSet);
}

ConsoleMethod(GuiEditCtrl, getCurrentAddSet, S32, 2, 2, "()\n @return Returns the set to which new controls will be added")
{
   const GuiControl* add = object->getCurrentAddSet();
   return add ? add->getId() : 0;
}

ConsoleMethod(GuiEditCtrl, toggle, void, 2, 2, "() Toggle activation.\n"
   "@return No return value.")
{
   object->setEditMode(!object->mActive);
}

ConsoleMethod(GuiEditCtrl, justify, void, 3, 3, "(int mode) Sets justification mode of selection\n"
   "@return No return value.")
{
   object->justifySelection((GuiEditCtrl::Justification)dAtoi(argv[2]));
}

ConsoleMethod(GuiEditCtrl, bringToFront, void, 2, 2, "() Brings control to front\n"
   "@return No return value.")
{
   object->bringToFront();
}

ConsoleMethod(GuiEditCtrl, pushToBack, void, 2, 2, "() Sends control to back\n"
   "@return No return value.")
{
   object->pushToBack();
}

ConsoleMethod(GuiEditCtrl, deleteSelection, void, 2, 2, "Delete the selected text.\n"
   "@return No return value.")
{
   object->deleteSelection();
}

ConsoleMethod(GuiEditCtrl, moveSelection, void, 4, 4, "(int deltax, int deltay) Moves selection to given (relative to current position) point\n"
   "@param deltax,deltay The change in coordinates.\n"
   "@return No return value.")
{
   object->moveAndSnapSelection(Point2I(dAtoi(argv[2]), dAtoi(argv[3])));
}

ConsoleMethod(GuiEditCtrl, saveSelection, void, 3, 3, "(string fileName) Saves the current selection to given filename\n"
   "@return No return value.")
{
   object->saveSelection(argv[2]);
}

ConsoleMethod(GuiEditCtrl, loadSelection, void, 3, 3, "(string fileName) Loads from given filename\n"
   "@return No return value.")
{
   object->loadSelection(argv[2]);
}

ConsoleMethod(GuiEditCtrl, selectAll, void, 2, 2, "() Selects all controls in list\n"
   "@return No return value.")
{
   object->selectAll();
}


ConsoleMethod(GuiEditCtrl, getSelected, S32, 2, 2, "() Gets the GUI control(s) the editor is currently selecting\n"
   "@return Returns the ID of the control.")
{
   return object->getSelectedSet().getId();
}

ConsoleMethod(GuiEditCtrl, getTrash, S32, 2, 2, "() Gets the GUI controls(s) that are currently in the trash.\n"
   "@return Returns the ID of the control")
{
   return object->getTrash().getId();
}

ConsoleMethod(GuiEditCtrl, getUndoManager, S32, 2, 2, "() Gets the Gui Editor's UndoManager object\n"
   "@return Returns the ID of the object.")
{
   return object->getUndoManager().getId();
}

ConsoleMethod(GuiEditCtrl, setSnapToGrid, void, 3, 3, "(gridsize) Set the size of the snap-to grid.\n"
   "@return No return value.")
{
   U32 gridsize = dAtoi(argv[2]);
   object->setSnapToGrid(gridsize);
}

extern "C"{
   DLL_PUBLIC GuiEditCtrl* GuiEditCtrlCreateInstance()
   {
      return new GuiEditCtrl();
   }

   DLL_PUBLIC void GuiEditCtrlSetRoot(GuiEditCtrl* ctrl, GuiControl* root)
   {
      ctrl->setRoot(root);
   }

   DLL_PUBLIC void GuiEditCtrlAddNewCtrl(GuiEditCtrl* ctrl, GuiControl* root)
   {
      ctrl->addNewControl(root);
   }

   DLL_PUBLIC void GuiEditCtrlAddSelection(GuiEditCtrl* ctrl, S32 ctrlID)
   {
      ctrl->addSelection(ctrlID);
   }

   DLL_PUBLIC void GuiEditCtrlRemoveSelection(GuiEditCtrl* ctrl, S32 ctrlID)
   {
      ctrl->removeSelection(ctrlID);
   }

   DLL_PUBLIC void GuiEditCtrlClearSelection(GuiEditCtrl* ctrl)
   {
      ctrl->clearSelection();
   }

   DLL_PUBLIC void GuiEditCtrlSelect(GuiEditCtrl* ctrl, GuiControl * selCtrl)
   {
      ctrl->setSelection(selCtrl);
   }

   DLL_PUBLIC void GuiEditCtrlSetCurrentAddSet(GuiEditCtrl* ctrl, GuiControl * selCtrl)
   {
      ctrl->setCurrentAddSet(selCtrl);
   }

   DLL_PUBLIC GuiControl* GuiEditCtrlGetCurrentAddSet(GuiEditCtrl* ctrl)
   {
      return const_cast<GuiControl*>(ctrl->getCurrentAddSet());
   }

   DLL_PUBLIC void GuiEditCtrlToggle(GuiEditCtrl* ctrl)
   {
      ctrl->setEditMode(!ctrl->mActive);
   }

   DLL_PUBLIC void GuiEditCtrlJustify(GuiEditCtrl* ctrl, S32 mode)
   {
      ctrl->justifySelection((GuiEditCtrl::Justification)mode);
   }

   DLL_PUBLIC void GuiEditCtrlBringToFront(GuiEditCtrl* ctrl)
   {
      ctrl->bringToFront();
   }

   DLL_PUBLIC void GuiEditCtrlPushToBack(GuiEditCtrl* ctrl)
   {
      ctrl->pushToBack();
   }

   DLL_PUBLIC void GuiEditCtrlDeleteSelection(GuiEditCtrl* ctrl)
   {
      ctrl->deleteSelection();
   }

   DLL_PUBLIC void GuiEditCtrlMoveSelection(GuiEditCtrl* ctrl, S32 deltaX, S32 deltaY)
   {
      ctrl->moveAndSnapSelection(Point2I(deltaX, deltaY));
   }

   DLL_PUBLIC void GuiEditCtrlSaveSelection(GuiEditCtrl* ctrl, const char* fileName)
   {
      ctrl->saveSelection(fileName);
   }

   DLL_PUBLIC void GuiEditCtrlLoadSelection(GuiEditCtrl* ctrl, const char* fileName)
   {
      ctrl->loadSelection(fileName);
   }

   DLL_PUBLIC void GuiEditCtrlSelectAll(GuiEditCtrl* ctrl)
   {
      ctrl->selectAll();
   }

   //CTODO this might fail
   DLL_PUBLIC SimSet* GuiEditCtrlGetSelected(GuiEditCtrl* ctrl)
   {
      return &const_cast<SimSet&>(ctrl->getSelectedSet());
   }

   DLL_PUBLIC SimGroup* GuiEditCtrlGetTrash(GuiEditCtrl* ctrl)
   {
      return &const_cast<SimGroup&>(ctrl->getTrash());
   }

   DLL_PUBLIC UndoManager* GuiEditCtrlGetUndoManager(GuiEditCtrl* ctrl)
   {
      return &const_cast<UndoManager&>(ctrl->getUndoManager());
   }

   DLL_PUBLIC void GuiEditCtrlSetSnapToGrid(GuiEditCtrl* ctrl, U32 gridSize)
   {
      ctrl->setSnapToGrid(gridSize);
   }
}