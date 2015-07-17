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

ConsoleNamespaceFunction( SysGUI, setEnabled, ConsoleVoid, 2, 2, (""))
{
   SysGUI::setEnabled(dAtob(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, beginScrollArea, ConsoleInt, 6, 6, (""))
{
   return SysGUI::beginScrollArea(argv[1], dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]), dAtoi(argv[5]));
}

ConsoleNamespaceFunction( SysGUI, endScrollArea, ConsoleInt, 1, 1, (""))
{
   return SysGUI::endScrollArea();
}

ConsoleNamespaceFunction( SysGUI, beginCollapse, ConsoleInt, 4, 4, (""))
{
   return SysGUI::beginCollapse(argv[1], argv[2], dAtob(argv[3]));
}

ConsoleNamespaceFunction( SysGUI, endCollapse, ConsoleInt, 1, 1, (""))
{
   return SysGUI::endCollapse();
}

ConsoleNamespaceFunction( SysGUI, colorWheel, ConsoleInt, 3, 3, (""))
{
   ColorF color;
   Con::setData(TypeColorF, &color, 0, 1, &argv[2]);

   return SysGUI::colorWheel(argv[1], color);
}

ConsoleNamespaceFunction( SysGUI, separator, ConsoleInt, 1, 1, (""))
{
   return SysGUI::separator();
}

ConsoleNamespaceFunction( SysGUI, vector3, ConsoleInt, 2, 2, (""))
{
   Point3F vec;
   Con::setData(TypePoint3F, &vec, 0, 1, &argv[2]);

   return SysGUI::vector3(argv[1], vec);
}

ConsoleNamespaceFunction( SysGUI, label, ConsoleInt, 2, 2, (""))
{
   return SysGUI::label(argv[1]);
}

ConsoleNamespaceFunction( SysGUI, textInput, ConsoleInt, 3, 3, (""))
{
   return SysGUI::textInput(argv[1], argv[2]);
}

ConsoleNamespaceFunction( SysGUI, button, ConsoleInt, 3, 3, (""))
{
   return SysGUI::button(argv[1], argv[2]);
}

ConsoleNamespaceFunction( SysGUI, checkBox, ConsoleInt, 3, 3, (""))
{
   return SysGUI::checkBox(argv[1], dAtob(argv[2]));
}

ConsoleNamespaceFunction( SysGUI, slider, ConsoleInt, 5, 5, (""))
{
   return SysGUI::slider(argv[1], dAtoi(argv[2]), dAtoi(argv[3]), dAtoi(argv[4]));
}

ConsoleNamespaceFunction( SysGUI, getLabelValue, ConsoleString, 2, 2, (""))
{
   return SysGUI::getLabelValue(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, setLabelValue, ConsoleVoid, 3, 3, (""))
{
   SysGUI::setLabelValue(dAtoi(argv[1]), argv[2]);
}

ConsoleNamespaceFunction( SysGUI, getTextValue, ConsoleString, 2, 2, (""))
{
   return SysGUI::getTextValue(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, setTextValue, ConsoleVoid, 3, 3, (""))
{
   SysGUI::setTextValue(dAtoi(argv[1]), argv[2]);
}

ConsoleNamespaceFunction( SysGUI, getIntValue, ConsoleInt, 2, 2, (""))
{
   return SysGUI::getIntValue(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, getBoolValue, ConsoleBool, 2, 2, (""))
{
   return SysGUI::getBoolValue(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, alignLeft, ConsoleVoid, 2, 2, (""))
{
   SysGUI::alignLeft(dAtoi(argv[1]));
}
ConsoleNamespaceFunction( SysGUI, alignRight, ConsoleVoid, 2, 2, (""))
{
   SysGUI::alignRight(dAtoi(argv[1]));
}
ConsoleNamespaceFunction( SysGUI, alignTop, ConsoleVoid, 2, 2, (""))
{
   SysGUI::alignTop(dAtoi(argv[1]));
}
ConsoleNamespaceFunction( SysGUI, alignBottom, ConsoleVoid, 2, 2, (""))
{
   SysGUI::alignBottom(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, list, ConsoleInt, 1, 1, (""))
{
   return SysGUI::list();
}

ConsoleNamespaceFunction( SysGUI, addListValue, ConsoleVoid, 3, 3, (""))
{
   SysGUI::addListValue(dAtoi(argv[1]), argv[2]);
}

ConsoleNamespaceFunction( SysGUI, getListValue, ConsoleString, 3, 3, (""))
{
   return SysGUI::getListValue(dAtoi(argv[1]), dAtoi(argv[2]));
}

ConsoleNamespaceFunction( SysGUI, getListSelected, ConsoleInt, 2, 2, (""))
{
   return SysGUI::getListSelected(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, clearList, ConsoleVoid, 2, 2, (""))
{
   SysGUI::clearList(dAtoi(argv[1]));
}

ConsoleNamespaceFunction( SysGUI, setElementHidden, ConsoleVoid, 3, 3, (""))
{
   SysGUI::setElementHidden(dAtoi(argv[1]), dAtob(argv[2]));
}