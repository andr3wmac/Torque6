//-----------------------------------------------------------------------------
// Copyright (c) 2014 Andrew Mac
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

#include <imgui.h>

namespace SysGUI
{
   Vector<Element> elementList;
   S32 elementMaxID = 0;

   // Init/Destroy
   void init()
   {
      FileObject imguiFont;
      imguiFont.readMemory("fonts/droidsans.ttf");
      imguiCreate(imguiFont.getBuffer());
   }

   void destroy()
   {

   }

   void render()
   {
      Point2I size = Platform::getWindowSize();
      imguiBeginFrame(0, 0, 0, 0, size.x, size.y, '\000', Graphics::ViewTable::EditorGUI); 

      for(U32 n = 0; n < elementList.size(); ++n)
      {
         Element* elem = &elementList[n];
         switch (elem->_type)
         {
            case ElementType::BeginScrollArea:
               imguiBeginScrollArea(elem->_value_label, elem->_x, elem->_y, elem->_width, elem->_height, &elem->_value_int);
               break;

            case ElementType::EndScrollArea:
               imguiEndScrollArea();
               break;

            case ElementType::Label:
               imguiLabel(elem->_value_label);
               break;

            case ElementType::TextInput:
               imguiInput(elem->_value_label, elem->_value_text, 10);
               break;

            case ElementType::CheckBox:
               imguiCheck(elem->_value_label, elem->_value_bool);
               break;

            case ElementType::Slider:
               imguiSlider(elem->_value_label, elem->_value_int, elem->_min, elem->_max);
               break;

            case ElementType::Separator:
               imguiSeparatorLine();
               break;

            default:
               break;
         }
      }

      imguiEndFrame();
   }

   bool processInputEvent(const InputEvent *event)
   {
      if ( event->deviceType == InputDeviceTypes::KeyboardDeviceType )
      {
         
      }

      return false;
   }

   S32 addElement(Element elem)
   {
      elem._id = getNewID();
      elementList.push_back(elem);
      return elem._id;
   }

   S32 getNewID() { return elementMaxID++; }

   S32 beginScrollArea(const char* title, U32 x, U32 y, U32 width, U32 height)
   {
      Element elem;
      elem._type = ElementType::BeginScrollArea;
      elem._x = x;
      elem._y = y;
      elem._height = height;
      elem._width = width;
      elem._value_int = 0;

      dStrcpy(elem._value_label, title);

      return addElement(elem);
   }

   S32 endScrollArea()
   {
      Element elem;
      elem._type = ElementType::EndScrollArea;
      return addElement(elem);
   }

   S32 label(const char* label)
   {
      Element elem;
      elem._type = ElementType::Label;

      dStrcpy(elem._value_label, label);

      return addElement(elem);
   }

   S32 textInput(const char* label, const char* text)
   {
      Element elem;
      elem._type = ElementType::TextInput;

      dStrcpy(elem._value_label, label);
      dStrcpy(elem._value_text, text);

      return addElement(elem);
   }

   S32 slider(const char* label, S32 value, S32 min, S32 max)
   {
      Element elem;
      elem._type = ElementType::Slider;
      elem._value_int = value;
      elem._min = min;
      elem._max = max;

      dStrcpy(elem._value_label, label);

      return addElement(elem);
   }

   S32 checkBox(const char* label, bool value)
   {
      Element elem;
      elem._type = ElementType::CheckBox;
      elem._value_bool = value;

      dStrcpy(elem._value_label, label);

      return addElement(elem);
   }

   S32 separator()
   {
      Element elem;
      elem._type = ElementType::Separator;
      return addElement(elem);
   }

   Element* getElementById(S32 id)
   {
      for ( U32 n = 0; n < elementList.size(); ++n )
      {
         if ( elementList[n]._id == id )
            return &elementList[n];
      }
      return NULL;
   }

   char* getLabelValue(S32 id)                     { return getElementById(id)->_value_label; }
   void  setLabelValue(S32 id, const char* val)    { dStrcpy(getElementById(id)->_value_label, val); }
   char* getTextValue(S32 id)                      { return getElementById(id)->_value_text; }
   void  setTextValue(S32 id, const char* val)     { dStrcpy(getElementById(id)->_value_text, val); }
   S32   getIntValue(S32 id)                       { return getElementById(id)->_value_int; }
   void  setIntValue(S32 id, S32 val)              { getElementById(id)->_value_int = val; }
   bool  getBoolValue(S32 id)                      { return getElementById(id)->_value_bool; }
   void  setBoolValue(S32 id, bool val)            { getElementById(id)->_value_bool = val; }
}      