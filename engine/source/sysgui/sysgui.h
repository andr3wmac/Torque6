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

#ifndef _SYSGUI_H_
#define _SYSGUI_H_

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

namespace SysGUI
{
   enum ElementType
   {
      BeginScrollArea,
      EndScrollArea,
      Label,
      CheckBox,
      Slider,
      TextInput,
      Separator,
      COUNT
   };

   struct Element
   {
      U32            _id;
      ElementType    _type;
      U32            _x;
      U32            _y;
      U32            _width;
      U32            _height;
      S32            _min;
      S32            _max;

      char           _value_label[100];
      char           _value_text[100];
      S32            _value_int;
      bool           _value_bool;
   };

   extern Vector<Element> elementList;
   extern S32 elementMaxID;

   // Init/Destroy
   void init();
   void destroy();
   void render();

   S32 addElement(Element elem);
   S32 getNewID();
   Element* getElementById(S32 id);

   // Controls
   S32 beginScrollArea(const char* title, U32 x, U32 y, U32 width, U32 height);
   S32 endScrollArea();
   S32 label(const char* label);
   S32 checkBox(const char* label, bool value);
   S32 slider(const char* label, S32 value, S32 min, S32 max);
   S32 textInput(const char* label, const char* text);
   S32 separator();

   bool processInputEvent(const InputEvent *event);

   char* getLabelValue(S32 id);
   void  setLabelValue(S32 id, const char* val);
   char* getTextValue(S32 id);
   void  setTextValue(S32 id, const char* val);
   S32   getIntValue(S32 id);
   void  setIntValue(S32 id, S32 val);
   bool  getBoolValue(S32 id);
   void  setBoolValue(S32 id, bool val);
}

#endif // _SYSGUI_H_