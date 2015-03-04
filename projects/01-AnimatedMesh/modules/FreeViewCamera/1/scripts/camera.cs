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
$panX = "0";
$panY = "0";
$panZ = "0";
$freeViewCam = 0;

function loadFreeViewCamera()
{
    new ActionMap(FreeViewControls);
    FreeViewControls.bind( keyboard, "w", moveForward );
    FreeViewControls.bind( keyboard, "s", moveBackward );
    FreeViewControls.bind( keyboard, "a", moveLeft );
    FreeViewControls.bind( keyboard, "d", moveRight );
    FreeViewControls.bind( keyboard, "q", moveUp );
    FreeViewControls.bind( keyboard, "e", moveDown );
    FreeViewControls.bind( keyboard, "r", rotateLeft );
    FreeViewControls.bind( keyboard, "f", rotateRight );
    FreeViewControls.push();

    $freeViewCam = Scene::getCamera("FreeViewCam");
    $freeViewCam.bindMouse(true);
    $freeViewCam.setPosition("0 50 -150");
    Scene::pushActiveCamera("FreeViewCam");
}

function setCameraPan()
{
    $freeViewCam.setPanVelocity($panX @ " " @ $panY @ " " @ $panZ);
}

function MoveForward( %val )
{
    if ( %val )
        $panZ = "-5";
    else
        $panZ = "0";

    setCameraPan();
}

function MoveBackward( %val )
{
    if ( %val )
        $panZ = "5";
    else
        $panZ = "0";

    setCameraPan();
}

function MoveLeft( %val )
{
    if ( %val )
        $panX = "5";
    else
        $panX = "0";

    setCameraPan();
}

function MoveRight( %val )
{
    if ( %val )
        $panX = "-5";
    else
        $panX = "0";

    setCameraPan();
}

function RotateLeft( %val )
{
    if ( %val )
        $freeViewCam.rotate("0 0.1 0");
}

function RotateRight( %val )
{
    if ( %val )
        $freeViewCam.rotate("0 -0.1 0");
}

function MoveUp( %val )
{
    if ( %val )
        $freeViewCam.translate("0 1 0");
}

function MoveDown( %val )
{
    if ( %val )
        $freeViewCam.translate("0 -1 0");
}
