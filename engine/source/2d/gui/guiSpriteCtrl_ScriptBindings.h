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

ConsoleMethodGroupBeginWithDocs(GuiSpriteCtrl, GuiControl)

/*! Gets whether the control is in static or dynamic (animated) mode.
    @return Returns whether the control is in static or dynamic (animated) mode.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, isStaticFrameProvider, ConsoleBool, 2, 2, ())
{
    return object->isStaticFrameProvider();
}

//------------------------------------------------------------------------------

/*! Gets whether the control is using a numerical or named image frame.
    @return Returns true when using a named frame, false when using a numerical index.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, isUsingNamedImageFrame, ConsoleBool, 2, 2, ())
{
    return object->isUsingNamedImageFrame();
}

//-----------------------------------------------------------------------------

/*! Sets the control image and optionally the frame.
    @param imageAssetId The image asset Id to display
    @param frame The numerical or named frame of the image to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setImage, ConsoleBool, 3, 4, (imageAssetId, [frame]))
{
    // Was a frame specified?
    if (argc >= 4)
    {
        // Was it a number or a string?
        if (!dIsalpha(*argv[3]))
        {
            // Fetch the numerical frame and set the image
            const U32 frame = argc >= 4 ? dAtoi(argv[3]) : 0;
            return object->setImage( argv[2], frame );
        }
        else
        {
            // Set the image and pass the named frame string
            return object->setImage( argv[2], argv[3] );
        }
    }
    else
    {
        // Frame was not specified, use default 0 and set the image
        const U32 frame = 0;
        return object->setImage( argv[2], frame );
    }
}

//------------------------------------------------------------------------------

/*! Gets current image asset Id.
    @return (string imageAssetId) The image being displayed.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getImage, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getImage() - Method invalid, not in static mode.");
        return StringTable->EmptyString;
    }

    // Get image.
    return object->getImage();
}

//-----------------------------------------------------------------------------

/*! Sets the image frame to use as the image.
    @param imageFrame The image frame to use as the image.
    @return No return value.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setImageFrame, ConsoleBool, 3, 3, (int imageFrame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::setImageFrame() - Method invalid, not in static mode.");
        return false;
    }

    return object->setImageFrame( dAtoi(argv[2]) );
}

//------------------------------------------------------------------------------

/*! Gets current numerical image frame.
    @return (int frame) The numerical frame currently being displayed.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getImageFrame, ConsoleInt, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getImageFrame() - Method invalid, not in static mode.");
        return -1;
    }

    // Are we using a named image frame?
    if ( object->isUsingNamedImageFrame() )
    {
        // Yes, so warn.
        Con::warnf("GuiSpriteCtrl::getImageFrame() - Method invalid, using a named image frame.");
        return -1;
    }

    // Get image frame.
    return object->getImageFrame();
}

//------------------------------------------------------------------------------

/*! Sets the image frame using a named string.
    @param frame The named frame to display
    @return Returns true on success.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, setNamedImageFrame, ConsoleBool, 3, 3, (frame))
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::setNamedImageFrame() - Method invalid, not in static mode.");
        return false;
    }

    // Set the numerical frame
    return object->setNamedImageFrame( argv[2] );
}

//------------------------------------------------------------------------------

/*! Gets the current named image frame.
    @return The current named image frame.
*/
ConsoleMethodWithDocs(GuiSpriteCtrl, getNamedImageFrame, ConsoleString, 2, 2, ())
{
    // Are we in static mode?
    if ( !object->isStaticFrameProvider() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getNamedImageFrame() - Method invalid, not in static mode.");
        return NULL;
    }

    // Are we using a named image frame?
    if ( !object->isUsingNamedImageFrame() )
    {
        // No, so warn.
        Con::warnf("GuiSpriteCtrl::getNamedImageFrame() - Method invalid, not using a named image frame.");
        return NULL;
    }

    return object->getNamedImageFrame();
}

ConsoleMethodGroupEndWithDocs(GuiSpriteCtrl)
