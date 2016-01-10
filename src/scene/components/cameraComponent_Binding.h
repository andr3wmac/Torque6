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

namespace Scene
{
    ConsoleMethodGroupBeginWithDocs(CameraComponent, BaseComponent)

    ConsoleMethod(CameraComponent, lookAt, ConsoleVoid, 3, 3, (""))
    {
        Point3F look_at = Utility::mConvertStringToPoint3F(argv[2]);
        object->lookAt(look_at);
    }

    ConsoleMethod(CameraComponent, translate, ConsoleVoid, 3, 3, (""))
    {
        Point3F translation = Utility::mConvertStringToPoint3F(argv[2]);
        object->translate(translation);
    }

    ConsoleMethod(CameraComponent, pan, ConsoleVoid, 3, 3, (""))
    {
        Point3F pan = Utility::mConvertStringToPoint3F(argv[2]);
        object->pan(pan);
    }

    ConsoleMethod(CameraComponent, setPanVelocity, ConsoleVoid, 3, 3, (""))
    {
        Point3F panVel = Utility::mConvertStringToPoint3F(argv[2]);
        object->setPanVelocity(panVel);
    }

    ConsoleMethod(CameraComponent, setPosition, ConsoleVoid, 3, 3, (""))
    {
        Point3F position = Utility::mConvertStringToPoint3F(argv[2]);
        object->setPosition(position);
    }

    ConsoleMethod(CameraComponent, rotate, ConsoleVoid, 3, 3, (""))
    {
        Point3F rotation = Utility::mConvertStringToPoint3F(argv[2]);
        object->rotate(rotation);
    }

    ConsoleMethod(CameraComponent, bindMouse, ConsoleVoid, 3, 5, (""))
    {
        switch (argc)
        {
        case 3:
            object->setBindMouse(dAtob(argv[2]));
            break;

        case 4:
            object->setBindMouse(dAtob(argv[2]), dAtob(argv[3]));
            break;

        case 5:
            object->setBindMouse(dAtob(argv[2]), dAtob(argv[3]), dAtob(argv[4]));
            break;
        }
    }

    ConsoleMethodGroupEndWithDocs(CameraComponent)
}

namespace Scene{
   extern "C"{
      DLL_PUBLIC CameraComponent* CameraComponentCreateInstance()
      {
         return new CameraComponent();
      }
   }
}