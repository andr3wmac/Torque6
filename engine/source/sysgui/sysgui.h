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

#ifndef _SYSGUI_H_
#define _SYSGUI_H_

#ifndef _EVENT_H_
#include "platform/event.h"
#endif

#ifndef _COLOR_H_
#include "graphics/color.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

#ifndef _GRAPHICS_CORE_H_
#include "graphics/core.h"
#endif

namespace SysGUI
{
   struct Element
   {
      struct Text
      {
         char val[256];
         char prevVal[256];
      };

      struct ListItem
      {
         char val[256];
         char script[256];
         void (*callback)(S32 id);
      };

      enum Type
      {
         BeginScrollArea,
         EndScrollArea,
         Label,
         CheckBox,
         Slider,
         TextInput,
         Separator,
         List,
         Button,
         BeginCollapse,
         EndCollapse,
         ColorWheel,
         Vector3,
         Image,
         COUNT
      };

      Element()
      {
         _value_image.idx = bgfx::invalidHandle;
         _hidden = false;
         _align_right = false;
         _align_bottom = false;
      }

      bool                 _hidden;
      U32                  _id;
      Type                 _type;
      U32                  _x;
      U32                  _y;
      U32                  _width;
      U32                  _height;
      S32                  _min;
      S32                  _max;

      bool                 _align_right;
      bool                 _align_bottom;
      
      Text                 _value_label;
      Text                 _value_text0;
      Text                 _value_text1;
      Text                 _value_text2;
      S32                  _value_int;
      bool                 _value_bool;
      Vector<ListItem>     _value_list;
      ColorF               _value_color;
      Point3F              _value_vector3;
      bgfx::TextureHandle  _value_image;
      S32                  _selected_list_item;
      Text                 _value_script;
      void                 (*_value_callback)(S32 id);
   };

   extern Graphics::ViewTableEntry* v_SysGUI;

   extern Vector<Element>  elementList;
   extern S32              elementMaxID;
   extern S32              elementSeek;

   extern Point2F mousePosition;
   extern bool    mouseButtonOne;
   extern bool    mouseButtonTwo;
   extern S32     mouseScroll;

   extern Vector<char>  keyboardQueue;
   extern U64           _keyboardLastInput;

   // 
   void init();
   void destroy();
   void setEnabled(bool val);
   void render();

   S32 addElement(Element elem);
   void removeElementById(S32 id);
   S32 getNewID();
   Element* getElementById(S32 id);

   // Controls
   S32 beginScrollArea(const char* title, U32 x, U32 y, U32 width, U32 height);
   S32 endScrollArea();
   void clearScrollArea(S32 id);

   S32 label(const char* label);
   S32 list(const char* script = NULL, void (*callback)(S32 id) = NULL);
   S32 checkBox(const char* label, bool value);
   S32 slider(const char* label, S32 value, S32 min, S32 max);
   S32 textInput(const char* label, const char* text);
   S32 button(const char* label, const char* script = NULL, void (*callback)(S32 id) = NULL);
   S32 separator();
   S32 beginCollapse(const char* label, const char* text, bool open);
   S32 endCollapse();

   S32 colorWheel(const char* label, ColorF color);
   S32 vector3(const char* label, Point3F vec, const char* script = NULL, void (*callback)(S32 id) = NULL);
   S32 image(bgfx::TextureHandle*, const char* script = NULL, void (*callback)(S32 id) = NULL);

   bool processInputEvent(const InputEvent *event);
   bool updateMousePosition(Point2F pt);

   void  addListValue(S32 id, const char* val, const char* script = NULL, void (*callback)(S32 id) = NULL);
   const char* getListValue(S32 id, S32 index);
   S32 getListSelected(S32 id);
   void  clearList(S32 id);

   void                 setElementHidden(S32 id, bool val);
   char*                getLabelValue(S32 id);
   void                 setLabelValue(S32 id, const char* val);
   char*                getTextValue(S32 id);
   void                 setTextValue(S32 id, const char* val);
   S32                  getIntValue(S32 id);
   void                 setIntValue(S32 id, S32 val);
   bool                 getBoolValue(S32 id);
   void                 setBoolValue(S32 id, bool val);
   ColorF               getColorValue(S32 id);
   void                 setColorValue(S32 id, ColorF val);
   Point3F              getVector3Value(S32 id);
   void                 setVector3Value(S32 id, Point3F val);
   bgfx::TextureHandle  getImageValue(S32 id);
   void                 setImageValue(S32 id, bgfx::TextureHandle val);

   void  alignLeft(S32 id);
   void  alignRight(S32 id);
   void  alignTop(S32 id);
   void  alignBottom(S32 id);

   void seek(S32 id);
   void clearSeek();

   class ElementCallbackEvent : public SimEvent
   {
      void (*_value_callback)(S32 id);
      S32 id; 

      public:
         ElementCallbackEvent(void (*callback)(S32 id), S32 _id)
         {
            _value_callback = callback;
            id = _id;
         }

         virtual void process(SimObject *object)
         {
            _value_callback(id);
         }
   };

   class ElementScriptEvent : public SimEvent
   {
      char _value_script[256];

      public:
         ElementScriptEvent(char script[256])
         {
            dStrcpy(_value_script, script);
         }

         virtual void process(SimObject *object)
         {
            Con::evaluate(_value_script, false);
         }
   };
}

#endif // _SYSGUI_H_