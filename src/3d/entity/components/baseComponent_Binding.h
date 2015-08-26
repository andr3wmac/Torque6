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

#include "c-interface/c-interface.h"

namespace Scene
{
   ConsoleMethod(BaseComponent, setUniformVec4, ConsoleVoid, 4, 4, (""))
   {
      Point4F value;
      Con::setData(TypePoint4F, value, 0, 1, &argv[3]);

      object->setUniformVec4(argv[2], value);
   }

   extern "C"{
      DLL_PUBLIC BaseComponent* BaseComponentCreateInstance()
      {
         return new BaseComponent();
      }

      DLL_PUBLIC void BaseComponentGetPosition(BaseComponent* baseComponent, CInterface::Point3FParam* outPos)
      {
         *outPos = baseComponent->getPosition();
      }

      DLL_PUBLIC void BaseComponentSetPosition(BaseComponent* baseComponent, CInterface::Point3FParam pos)
      {
         baseComponent->setPosition(pos);
      }

      DLL_PUBLIC void BaseComponentGetRotation(BaseComponent* baseComponent, CInterface::Point3FParam* outRotation)
      {
         *outRotation = baseComponent->getRotation();
      }

      DLL_PUBLIC void BaseComponentSetRotation(BaseComponent* baseComponent, CInterface::Point3FParam rotation)
      {
         baseComponent->setRotation(rotation);
      }

      DLL_PUBLIC void BaseComponentGetScale(BaseComponent* baseComponent, CInterface::Point3FParam* outScale)
      {
         *outScale = baseComponent->getScale();
      }

      DLL_PUBLIC void BaseComponentSetScale(BaseComponent* baseComponent, CInterface::Point3FParam scale)
      {
         baseComponent->setScale(scale);
      }

      DLL_PUBLIC void BaseComponentSetUniformVec4(BaseComponent* baseComponent, const char* name, CInterface::Point4FParam value)
      {
         baseComponent->setUniformVec4(name, value);
      }
   }
}