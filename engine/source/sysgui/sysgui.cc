//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
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

#include "console/consoleTypes.h"
#include "sysgui.h"
#include "graphics/utilities.h"
#include "platform/event.h"

// Script bindings.
#include "sysgui_ScriptBinding.h"

// bgfx/bx
#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

#include <imgui/imgui.h>

namespace SysGUI
{
   bool enabled = false;
   bool mouseOverArea = false;

   // SysGUI Elements
   Vector<Element> elementList;
   S32 elementMaxID = 0;
   S32 elementSeek = -1;

   // Mouse
   Point2F mousePosition;
   bool mouseButtonOne = false;
   bool mouseButtonTwo = false;
   S32 mouseScroll = 0;

   // Keyboard
   Vector<char> keyboardQueue;
   U64 _keyboardLastInput = 0;

   // Init/Destroy
   void init()
   {
      imguiCreate();
   }

   void destroy()
   {
      imguiDestroy();
   }

   void setEnabled(bool val)
   {
      enabled = val;
   }

   void render()
   {
      if ( !enabled ) return;

      Point2I size = Platform::getWindowSize();

      // We keep a queue of keyboard charactes as they come in. 
      // imgui processes 1 character at a time.
      char inputChar = '\000';
      if ( keyboardQueue.size() > 0 )
      {
         inputChar = keyboardQueue[0];
         keyboardQueue.pop_front();
      }

      imguiBeginFrame(
         mousePosition.x, mousePosition.y, 
         (mouseButtonOne ? IMGUI_MBUT_LEFT  : 0) | (mouseButtonTwo ? IMGUI_MBUT_RIGHT : 0), 
         mouseScroll, 
         size.x, size.y, 
         inputChar, 
         Graphics::ViewTable::SysGUI); 

      mouseScroll = 0;

      U32 hideGroup = 0;
      for(S32 n = 0; n < elementList.size(); ++n)
      {
         Element* elem = &elementList[n];

         switch (elem->_type)
         {
            case Element::Type::BeginScrollArea:
               if ( elem->_hidden || hideGroup > 0 )
               {
                  hideGroup++;
                  break;
               }

               {
                  // Alignment
                  U32 x = elem->_align_right ? size.x - elem->_x - elem->_width : elem->_x;
                  U32 y = elem->_align_bottom ? size.y - elem->_y - elem->_height : elem->_y;
                  imguiBeginScrollArea(elem->_value_label.val, x, y, elem->_width, elem->_height, &elem->_value_int);
               }
               break;

            case Element::Type::EndScrollArea:
               if ( hideGroup > 0 ) 
               {
                  hideGroup--;
                  break;
               }
               imguiEndScrollArea();
               break;

            case Element::Type::Label:
               if ( elem->_hidden || hideGroup > 0 ) break;
               imguiLabel(elem->_value_label.val);
               break;

            case Element::Type::List:
               if ( elem->_hidden || hideGroup > 0 ) break;
               for(S32 i = 0; i < elem->_value_list.size(); ++i )
               {
                  bool isSelected = i == elem->_selected_list_item;

                  if ( isSelected )
                     imguiIndent();

                  if ( imguiItem(elem->_value_list[i].val) )
                  {
                     elem->_selected_list_item = i;

                     // ListItem Callback
                     if ( dStrlen(elem->_value_list[i].script) > 0 )
                        Con::evaluate(elem->_value_list[i].script, false);
                     if ( elem->_value_list[i].callback != NULL )
                        elem->_value_list[i].callback();

                     // List Callback
                     if ( dStrlen(elem->_value_script.val) > 0 )
                        Con::evaluate(elem->_value_script.val, false);
                     if ( elem->_value_callback != NULL )
                        elem->_value_callback();
                  }

                  if ( isSelected )
                     imguiUnindent();
               }
               break;

            case Element::Type::TextInput:
               if ( elem->_hidden || hideGroup > 0 ) break;
               imguiInput(elem->_value_label.val, elem->_value_text.val, 32);
               break;

            case Element::Type::Button:
               if ( elem->_hidden || hideGroup > 0 ) break;
               if ( imguiButton(elem->_value_label.val) )
               {
                  if ( dStrlen(elem->_value_script.val) > 0 )
                     Con::evaluate(elem->_value_script.val, false);
                  if ( elem->_value_callback != NULL )
                     elem->_value_callback();
               }
               break;

            case Element::Type::CheckBox:
               if ( elem->_hidden || hideGroup > 0 ) break;
               if ( imguiCheck(elem->_value_label.val, elem->_value_bool) )
                  elem->_value_bool = !elem->_value_bool;
               break;

            case Element::Type::Slider:
               if ( elem->_hidden || hideGroup > 0 ) break;
               imguiSlider(elem->_value_label.val, elem->_value_int, elem->_min, elem->_max);
               break;

            case Element::Type::Separator:
               if ( elem->_hidden || hideGroup > 0 ) break;
               imguiSeparatorLine();
               break;

            default:
               break;
         }
      }

      imguiEndFrame();
      mouseOverArea = imguiMouseOverArea();
   }

   bool processInputEvent(const InputEvent *event)
   {
      if ( !enabled || !mouseOverArea ) return false;

      mouseButtonOne = false;
      mouseButtonTwo = false;
      mouseScroll = 0;

      // Keyboard Input
      if ( event->deviceType == InputDeviceTypes::KeyboardDeviceType && event->ascii > 0 )
      {
         char c = event->ascii;

         if ( event->action == SI_MAKE )
         {
            keyboardQueue.push_back(c);
            _keyboardLastInput = bx::getHPCounter();
         }
         if ( event->action == SI_REPEAT )
         {
            U64 currentTime = bx::getHPCounter();
            if ( currentTime - _keyboardLastInput > 10000 )
            {
               keyboardQueue.push_back(c);
               _keyboardLastInput = currentTime;
            }
         }
      }

      // Mouse Input
      if ( event->deviceType == InputDeviceTypes::MouseDeviceType )
      {
         if ((event->action == SI_MAKE || event->action == SI_REPEAT) && (event->objType == SI_BUTTON))
         {
            if( event->objInst == KEY_BUTTON0 )
               mouseButtonOne = true;
            if( event->objInst == KEY_BUTTON1 )
               mouseButtonTwo = true;
         }

         // Scroll Wheel
         if ( event->objType == SI_ZAXIS )
         {
            if ( event->fValues[0] < 0.0f )
               mouseScroll = -1;
            else
               mouseScroll = 1;
         }
      }

      return true;
   }

   bool updateMousePosition(Point2F pt)
   {
      if ( !enabled ) return false;

      mousePosition = pt;
      return mouseOverArea;
   }

   // Element Handling
   void seek(S32 id)
   {
      elementSeek = -1;
      for ( S32 n = 0; n < elementList.size(); ++n )
      {
         if ( elementList[n]._id == id )
            elementSeek = n + 1;
      }
   }
   void clearSeek()
   {
      elementSeek = -1;
   }

   S32 addElement(Element elem)
   {
      elem._id = getNewID();
      if ( elementSeek < 0 )
         elementList.push_back(elem);
      else
      {
         elementList.insert(elementSeek);
         elementList[elementSeek] = elem;
      }
      return elem._id;
   }

   void removeElementById(S32 id)
   {
      S32 removal_index = -1;
      for ( S32 n = 0; n < elementList.size(); ++n )
      {
         if ( elementList[n]._id == id )
         {
            removal_index = n;
            break;
         }
      }

      if ( removal_index > -1 )
         elementList.erase(removal_index);
   }

   Element* getElementById(S32 id)
   {
      for ( S32 n = 0; n < elementList.size(); ++n )
      {
         if ( elementList[n]._id == id )
            return &elementList[n];
      }
      return NULL;
   }

   S32   getNewID()                                { return elementMaxID++; }
   void  setElementHidden(S32 id, bool val)        { getElementById(id)->_hidden = val; }
   char* getLabelValue(S32 id)                     { return getElementById(id)->_value_label.val; }
   void  setLabelValue(S32 id, const char* val)    { dStrcpy(getElementById(id)->_value_label.val, val); }
   char* getTextValue(S32 id)                      { return getElementById(id)->_value_text.val; }
   void  setTextValue(S32 id, const char* val)     { dStrcpy(getElementById(id)->_value_text.val, val); }
   S32   getIntValue(S32 id)                       { return getElementById(id)->_value_int; }
   void  setIntValue(S32 id, S32 val)              { getElementById(id)->_value_int = val; }
   bool  getBoolValue(S32 id)                      { return getElementById(id)->_value_bool; }
   void  setBoolValue(S32 id, bool val)            { getElementById(id)->_value_bool = val; }
   void  alignLeft(S32 id)                         { getElementById(id)->_align_right = false; }
   void  alignRight(S32 id)                        { getElementById(id)->_align_right = true; }
   void  alignTop(S32 id)                          { getElementById(id)->_align_bottom = false; }
   void  alignBottom(S32 id)                       { getElementById(id)->_align_bottom = true; }

   // Element Creation Functions
   S32 beginScrollArea(const char* title, U32 x, U32 y, U32 width, U32 height)
   {
      Element elem;
      elem._type = Element::Type::BeginScrollArea;
      elem._x = x;
      elem._y = y;
      elem._height = height;
      elem._width = width;
      elem._value_int = 0;
      elem._align_right = false;

      dStrcpy(elem._value_label.val, title);

      return addElement(elem);
   }

   S32 endScrollArea()
   {
      Element elem;
      elem._type = Element::Type::EndScrollArea;
      return addElement(elem);
   }

   void clearScrollArea(S32 id)
   {
      Vector<SysGUI::Element> newElementList;

      U32 clearGroup = 0;
      for(S32 n = 0; n < elementList.size(); ++n)
      {
         Element* elem = &elementList[n];

         switch (elem->_type)
         {
            case Element::Type::BeginScrollArea:
               if ( elem->_id == id )
               {
                  clearGroup++;
                  newElementList.push_back(*elem);
                  break;
               }

               if ( clearGroup == 0 )
                  newElementList.push_back(*elem);
               break;

            case Element::Type::EndScrollArea:
               if ( clearGroup > 0 ) 
                  clearGroup--;

               if ( clearGroup == 0 )
                  newElementList.push_back(*elem);
               break;

            default:
               if ( clearGroup == 0 )
                  newElementList.push_back(*elem);
               break;
         }
      }

      elementList.clear();
      elementList = newElementList;
   }

   S32 label(const char* label)
   {
      Element elem;
      elem._type = Element::Type::Label;

      dStrcpy(elem._value_label.val, label);

      return addElement(elem);
   }

   S32 textInput(const char* label, const char* text)
   {
      Element elem;
      elem._type = Element::Type::TextInput;

      dStrcpy(elem._value_label.val, label);
      dStrcpy(elem._value_text.val, text);

      return addElement(elem);
   }

   S32 button(const char* label, const char* script, void (*callback)())
   {
      Element elem;
      elem._type = Element::Type::Button;

      dStrcpy(elem._value_label.val, label);
      dStrcpy(elem._value_script.val, script);
      elem._value_callback = callback;

      return addElement(elem);
   }

   S32 slider(const char* label, S32 value, S32 min, S32 max)
   {
      Element elem;
      elem._type = Element::Type::Slider;
      elem._value_int = value;
      elem._min = min;
      elem._max = max;

      dStrcpy(elem._value_label.val, label);

      return addElement(elem);
   }

   S32 checkBox(const char* label, bool value)
   {
      Element elem;
      elem._type = Element::Type::CheckBox;
      elem._value_bool = value;

      dStrcpy(elem._value_label.val, label);

      return addElement(elem);
   }

   S32 separator()
   {
      Element elem;
      elem._type = Element::Type::Separator;
      return addElement(elem);
   }

   S32 list(const char* script, void (*callback)())
   {
      Element elem;
      elem._type = Element::Type::List;
      elem._selected_list_item = -1;
      dStrcpy(elem._value_script.val, script);
      elem._value_callback = callback;
      return addElement(elem);
   }

   void addListValue(S32 id, const char* val, const char* script, void (*callback)())
   {
      Element* elem = getElementById(id);
      
      Element::ListItem item;
      dStrcpy(item.val, val);
      dStrcpy(item.script, script);
      item.callback = callback;
      elem->_value_list.push_back(item);
   }

   void clearList(S32 id)
   {
      Element* elem = getElementById(id);
      elem->_value_list.clear();
   }

   const char* getListValue(S32 id, S32 index)
   {
      Element* elem = getElementById(id);
      if ( index < elem->_value_list.size() )
         return elem->_value_list[index].val;
      return "";
   }

   S32 getListSelected(S32 id)
   {
      Element* elem = getElementById(id);
      return elem->_selected_list_item;
   }
}      