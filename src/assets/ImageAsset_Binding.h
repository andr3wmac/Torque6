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

ConsoleMethodGroupBeginWithDocs(ImageAsset, AssetBase)

/*! Sets the image file (bitmap file).
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setImageFile, ConsoleVoid, 3, 3, (ImageFile))
{
    object->setImageFile( argv[2] );
}

//-----------------------------------------------------------------------------

/*! Gets the image file.
    @return Returns the bitmap image file.
*/
ConsoleMethodWithDocs(ImageAsset, getImageFile, ConsoleString, 2, 2, ())
{
    return object->getImageFile();
}

//------------------------------------------------------------------------------

/*! Sets the filter mode.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setFilterMode, ConsoleVoid, 3, 3, (mode))
{
    // Fetch Texture Filter Mode.
    const ImageAsset::TextureFilterMode filterMode = ImageAsset::getFilterModeEnum( argv[2] );

    // Valid Filter?
    if ( filterMode == ImageAsset::FILTER_INVALID )
    {
        // Warn.
        Con::warnf("ImageAsset::setFilterMode() - Invalid Filter Mode Specified! (%s)", argv[2] );
        // Finish Here.
        return;
    }

    // Set Filter Mode.
    object->setFilterMode( filterMode );
}

//------------------------------------------------------------------------------

/*! Gets the filter mode.
    @return The filter mode.
*/
ConsoleMethodWithDocs(ImageAsset, getFilterMode, ConsoleString, 2, 2, ())
{
    return ImageAsset::getFilterModeDescription( object->getFilterMode() );
}

//-----------------------------------------------------------------------------

/*! Sets whether 16-bit image is forced or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setForce16Bit, ConsoleVoid, 3, 3, (force16Bit?))
{
    object->setForce16Bit( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether 16-bit image is forced or not.
    @return Whether 16-bit image is forced or not.
*/
ConsoleMethodWithDocs(ImageAsset, getForce16Bit, ConsoleBool, 2, 2, ())
{
    return object->getForce16Bit();
}

//-----------------------------------------------------------------------------

/*! Sets whether CELL row order should be used or not.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellRowOrder, ConsoleVoid, 3, 3, (rowOrder?))
{
    object->setCellRowOrder( dAtob(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets whether CELL row order should be used or not.
    @return Whether CELL row order should be used or not.
*/
ConsoleMethodWithDocs(ImageAsset, getCellRowOrder, ConsoleBool, 2, 2, ())
{
    return object->getCellRowOrder();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL offset X.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellOffsetX, ConsoleVoid, 3, 3, (offsetX))
{
    object->setCellOffsetX( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL offset X.
    @return The CELL offset X.
*/
ConsoleMethodWithDocs(ImageAsset, getCellOffsetX, ConsoleInt, 2, 2, ())
{
    return object->getCellOffsetX();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL offset Y.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellOffsetY, ConsoleVoid, 3, 3, (offsetY))
{
    object->setCellOffsetY( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL offset Y.
    @return The CELL offset Y.
*/
ConsoleMethodWithDocs(ImageAsset, getCellOffsetY, ConsoleInt, 2, 2, ())
{
    return object->getCellOffsetY();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL stride X.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellStrideX, ConsoleVoid, 3, 3, (strideX))
{
    object->setCellStrideX( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL stride X.
    @return The CELL stride X.
*/
ConsoleMethodWithDocs(ImageAsset, getCellStrideX, ConsoleInt, 2, 2, ())
{
    return object->getCellStrideX();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL stride Y.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellStrideY, ConsoleVoid, 3, 3, (strideY))
{
    object->setCellStrideY( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL stride Y.
    @return The CELL stride Y.
*/
ConsoleMethodWithDocs(ImageAsset, getCellStrideY, ConsoleInt, 2, 2, ())
{
    return object->getCellStrideY();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL count X.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellCountX, ConsoleVoid, 3, 3, (countX))
{
    object->setCellCountX( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL count X.
    @return The CELL count X.
*/
ConsoleMethodWithDocs(ImageAsset, getCellCountX, ConsoleInt, 2, 2, ())
{
    return object->getCellCountX();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL count Y.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellCountY, ConsoleVoid, 3, 3, (countY))
{
    object->setCellCountY( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL count Y.
    @return The CELL count Y.
*/
ConsoleMethodWithDocs(ImageAsset, getCellCountY, ConsoleInt, 2, 2, ())
{
    return object->getCellCountY();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL width.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellWidth, ConsoleVoid, 3, 3, (Width))
{
    object->setCellWidth( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL width.
    @return The CELL width.
*/
ConsoleMethodWithDocs(ImageAsset, getCellWidth, ConsoleInt, 2, 2, ())
{
    return object->getCellWidth();
}

//-----------------------------------------------------------------------------

/*! Sets the CELL height.
    @return No return value.
*/
ConsoleMethodWithDocs(ImageAsset, setCellHeight, ConsoleVoid, 3, 3, (Height))
{
    object->setCellHeight( dAtoi(argv[2]) );
}

//-----------------------------------------------------------------------------

/*! Gets the CELL height.
    @return The CELL height.
*/
ConsoleMethodWithDocs(ImageAsset, getCellHeight, ConsoleInt, 2, 2, ())
{
    return object->getCellHeight();
}

//-----------------------------------------------------------------------------

/*! Gets the image width.
    @return The image width.
*/
ConsoleMethodWithDocs(ImageAsset, getImageWidth, ConsoleInt, 2, 2, ())
{
    return object->getImageWidth();
}

//-----------------------------------------------------------------------------

/*! Gets the image height.
    @return The image width.
*/
ConsoleMethodWithDocs(ImageAsset, getImageHeight, ConsoleInt, 2, 2, ())
{
    return object->getImageHeight();
}

//-----------------------------------------------------------------------------

/*! Gets the image size.
    @return The image size.
*/
ConsoleMethodWithDocs(ImageAsset, getImageSize, ConsoleString, 2, 2, ())
{
    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", object->getImageWidth(), object->getImageHeight());

    // Return Buffer.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Gets whether the image has a power-of-two dimensions or not.
    @return Whether the image has a power-of-two dimensions or not.
*/
ConsoleMethodWithDocs(ImageAsset, getIsImagePOT, ConsoleBool, 2, 2, ())
{
    return isPow2( object->getImageWidth() ) && isPow2( object->getImageHeight() );
}

//-----------------------------------------------------------------------------

/*! Gets the frame count.
    @return The frame count.
*/
ConsoleMethodWithDocs(ImageAsset, getFrameCount, ConsoleInt, 2, 2, ())
{
    return object->getFrameCount(); 
}

//-----------------------------------------------------------------------------

/*! Gets the specified frames size.
    @param frame The frame number to use.
    @return The specified frames size.
*/
ConsoleMethodWithDocs(ImageAsset, getFrameSize, ConsoleString, 3, 3, (int frame))
{
    // Fetch Frame.
    const S32 frame = dAtoi(argv[2]);

    // Check Frame.
    if ( frame < 0 || frame >= (S32)object->getFrameCount() )
    {
        // Warn.
        Con::warnf("ImageAsset::getFrameSize() - Invalid Frame; Allowed range is 0 to %d", object->getFrameCount()-1 );
        // Finish Here.
        return NULL;
    }

    // Fetch Selected Frame Pixel Area.
    const ImageAsset::FrameArea::PixelArea& framePixelArea = object->getImageFrameArea( frame ).mPixelArea;

    // Create Returnable Buffer.
    char* pBuffer = Con::getReturnBuffer(32);

    // Format Buffer.
    dSprintf(pBuffer, 32, "%d %d", framePixelArea.mPixelWidth, framePixelArea.mPixelHeight );

    // Return Velocity.
    return pBuffer;
}

//-----------------------------------------------------------------------------

/*! Clears all explicit cells.
    The image asset stays in explicit mode however with no explicit cells a single full-frame cell becomes default.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, clearExplicitCells, ConsoleBool, 2, 2, ())
{
    return object->clearExplicitCells();
}

//-----------------------------------------------------------------------------

/*! Add an explicit cell.
    @param cellOffsetX The offset in the X axis to the top-left of the cell.
    @param cellOffsetY The offset in the Y axis to the top-left of the cell.
    @param cellWidth The width of the cell.
    @param cellHeight The height of the cell.
    @param cellName The name of the cell's region.
    The image asset must be in explicit mode for this operation to succeed.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, addExplicitCell, ConsoleBool, 7, 7, (int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, string cellName))
{
    // Fetch offsets.
    const S32 cellOffsetX = dAtoi( argv[2] );
    const S32 cellOffsetY = dAtoi( argv[3] );

    // Fetch dimensions.
    const S32 cellWidth = dAtoi( argv[4] );
    const S32 cellHeight = dAtoi (argv[5] );

    return object->addExplicitCell( cellOffsetX, cellOffsetY, cellWidth, cellHeight, argv[6] );
}

//-----------------------------------------------------------------------------

/*! Insert an explicit cell at the specified index.
    @param cellIndex The zero-based index to insert the cell.  This will work when no cells are present.  If the index is beyond the cell count then the cell is simply added.
    @param cellOffsetX The offset in the X axis to the top-left of the cell.
    @param cellOffsetY The offset in the Y axis to the top-left of the cell.
    @param cellWidth The width of the cell.
    @param cellHeight The height of the cell.
    @param cellName The name of the cell's region.
    The image asset must be in explicit mode for this operation to succeed.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, insertExplicitCell, ConsoleBool, 8, 8, (int cellIndex, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, string cellName))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi( argv[2] );

    // Fetch offsets.
    const S32 cellOffsetX = dAtoi( argv[3] );
    const S32 cellOffsetY = dAtoi( argv[4] );

    // Fetch dimensions.
    const S32 cellWidth = dAtoi( argv[5] );
    const S32 cellHeight = dAtoi (argv[6] );

    return object->insertExplicitCell( cellIndex, cellOffsetX, cellOffsetY, cellWidth, cellHeight, argv[7] );
}


//-----------------------------------------------------------------------------

/*! Remove an explicit cell from the specified index.
    @param cellIndex The zero-based index to remove the cell from.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, removeExplicitCell, ConsoleBool, 7, 7, (int cellIndex))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi( argv[2] );

    return object->removeExplicitCell( cellIndex );
}

//-----------------------------------------------------------------------------

/*! Set an explicit cell at the specified index.
    @param cellIndex The zero-based index to set the cell.
    @param cellOffsetX The offset in the X axis to the top-left of the cell.
    @param cellOffsetY The offset in the Y axis to the top-left of the cell.
    @param cellWidth The width of the cell.
    @param cellHeight The height of the cell.
    @param cellName The name of the cell's region.
    The image asset must be in explicit mode for this operation to succeed.
    @return Whether the operation was successful or not.
*/
ConsoleMethodWithDocs(ImageAsset, setExplicitCell, ConsoleBool, 8, 8, (int cellIndex, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, string cellName))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi( argv[2] );

    // Fetch offsets.
    const S32 cellOffsetX = dAtoi( argv[3] );
    const S32 cellOffsetY = dAtoi( argv[4] );

    // Fetch dimensions.
    const S32 cellWidth = dAtoi( argv[5] );
    const S32 cellHeight = dAtoi (argv[6] );

    return object->setExplicitCell( cellIndex, cellOffsetX, cellOffsetY, cellWidth, cellHeight, argv[7] );
}

//-----------------------------------------------------------------------------

/*! Gets the explicit cell count.
    @return The explicit cell count.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellCount, ConsoleInt, 2, 2, ())
{
    return object->getExplicitCellCount();
}

//-----------------------------------------------------------------------------

/*! Gets the CELL offset in Explicit Mode.
    @param cell The cell index or cell name to use to find the specific offset.
    @return The specified CELL width.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellOffset, ConsoleString, 3, 3, (cell))
{
    // Was it a number or a string?
    if (!dIsalpha(*argv[2]))
    {
        // Using cell index.
        const S32 cellIndex = dAtoi(argv[2]);
        
        return object->getExplicitCellOffset(cellIndex).scriptThis();
    }
    else
    {
        // Using cell name.
        ImageAsset::FrameArea& frameRegion = object->getCellByName(argv[2]);
        
        const Vector2 offset = frameRegion.mPixelArea.mPixelOffset;
        
        return offset.scriptThis();
    }
}

//-----------------------------------------------------------------------------

/*! Gets the CELL width in Explicit Mode.
    @param cell The cell index or cell name to use to find the specific width.
    @return The specified CELL width.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellWidth, ConsoleInt, 3, 3, (cell))
{
    S32 cellIndex;
    
    // Was it a number or a string?
    if (!dIsalpha(*argv[2]))
    {
        // Using cell index.
        cellIndex = dAtoi(argv[2]);
        
        return object->getExplicitCellWidth(cellIndex);
    }
    else
    {
        // Using cell name.
        ImageAsset::FrameArea& frameRegion = object->getCellByName(argv[2]);
        
        return frameRegion.mPixelArea.mPixelWidth;
    }
}

//-----------------------------------------------------------------------------

/*! Gets the CELL height in Explicit Mode.
    @param cell The cell index or cell name to use to find the specific height.
    @return The specified CELL height.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellHeight, ConsoleInt, 3, 3, (cell))
{
    S32 cellIndex;
    
    // Was it a number or a string?
    if (!dIsalpha(*argv[2]))
    {
        // Using cell index.
        cellIndex = dAtoi(argv[2]);
        
        return object->getExplicitCellHeight(cellIndex);
    }
    else
    {
        // Using cell name.
        ImageAsset::FrameArea& frameRegion = object->getCellByName(argv[2]);
        
        return frameRegion.mPixelArea.mPixelHeight;
    }
}

//-----------------------------------------------------------------------------

/*! Gets the CELL region name in Explicit Mode.
    @param cell The cell index to use to find the specific name.
    @return The specified CELL region name.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellName, ConsoleString, 3, 3, (cell))
{
    // Fetch cell index.
    const S32 cellIndex = dAtoi(argv[2]);

    return object->getExplicitCellName(cellIndex);
}

//-----------------------------------------------------------------------------

/*! Gets the CELL index number in Explicit Mode.
    @param cellName The cell name to use to find the specific index.
    @return The specified CELL index number.
*/
ConsoleMethodWithDocs(ImageAsset, getExplicitCellIndex, ConsoleInt, 3, 3, (cellName))
{
    return object->getExplicitCellIndex( argv[2] );
}

ConsoleMethodGroupEndWithDocs(ImageAsset)


extern "C"{
   DLL_PUBLIC ImageAsset* ImageAssetCreateInstance()
   {
      return new ImageAsset();
   }

   DLL_PUBLIC const char* ImageAssetGetImageFile(ImageAsset* imageAsset)
   {
      return CInterface::GetMarshallableString(imageAsset->getImageFile());
   }

   DLL_PUBLIC void ImageAssetSetImageFile(ImageAsset* imageAsset, const char* imageFile)
   {
      imageAsset->setImageFile(imageFile);
   }

   DLL_PUBLIC const char* ImageAssetGetFilterMode(ImageAsset* imageAsset)
   {
      return CInterface::GetMarshallableString(ImageAsset::getFilterModeDescription(imageAsset->getFilterMode()));
   }

   DLL_PUBLIC void ImageAssetSetFilterMode(ImageAsset* imageAsset, const char* filterMode)
   {
      // Fetch Texture Filter Mode.
      const ImageAsset::TextureFilterMode enumFilterMode = ImageAsset::getFilterModeEnum(filterMode);

      // Valid Filter?
      if (enumFilterMode == ImageAsset::FILTER_INVALID)
      {
         // Warn.
         Con::warnf("ImageAsset::setFilterMode() - Invalid Filter Mode Specified! (%s)", filterMode);
         // Finish Here.
         return;
      }

      // Set Filter Mode.
      imageAsset->setFilterMode(enumFilterMode);
   }

   DLL_PUBLIC bool ImageAssetGetExplicitMode(ImageAsset* imageAsset)
   {
      return imageAsset->getExplicitMode();
   }

   DLL_PUBLIC void ImageAssetSetExplicitMode(ImageAsset* imageAsset, bool value)
   {
      imageAsset->setExplicitMode(value);
   }

   DLL_PUBLIC bool ImageAssetGetForce16Bit(ImageAsset* imageAsset)
   {
      return imageAsset->getForce16Bit();
   }

   DLL_PUBLIC void ImageAssetSetForce16Bit(ImageAsset* imageAsset, bool value)
   {
      imageAsset->setForce16Bit(value);
   }

   DLL_PUBLIC bool ImageAssetGetCellRowOrder(ImageAsset* imageAsset)
   {
      return imageAsset->getCellRowOrder();
   }

   DLL_PUBLIC void ImageAssetSetCellRowOrder(ImageAsset* imageAsset, bool value)
   {
      imageAsset->setCellRowOrder(value);
   }

   DLL_PUBLIC int ImageAssetGetCellOffsetX(ImageAsset* imageAsset)
   {
      return imageAsset->getCellOffsetX();
   }

   DLL_PUBLIC void ImageAssetSetCellOffsetX(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellOffsetX(value);
   }

   DLL_PUBLIC int ImageAssetGetCellOffsetY(ImageAsset* imageAsset)
   {
      return imageAsset->getCellOffsetY();
   }

   DLL_PUBLIC void ImageAssetSetCellOffsetY(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellOffsetY(value);
   }

   DLL_PUBLIC int ImageAssetGetCellStrideX(ImageAsset* imageAsset)
   {
      return imageAsset->getCellStrideX();
   }

   DLL_PUBLIC void ImageAssetSetCellStrideX(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellStrideX(value);
   }

   DLL_PUBLIC int ImageAssetGetCellStrideY(ImageAsset* imageAsset)
   {
      return imageAsset->getCellStrideY();
   }

   DLL_PUBLIC void ImageAssetSetCellStrideY(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellStrideY(value);
   }

   DLL_PUBLIC int ImageAssetGetCellCountX(ImageAsset* imageAsset)
   {
      return imageAsset->getCellCountX();
   }

   DLL_PUBLIC void ImageAssetSetCellCountX(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellCountX(value);
   }

   DLL_PUBLIC int ImageAssetGetCellCountY(ImageAsset* imageAsset)
   {
      return imageAsset->getCellCountY();
   }

   DLL_PUBLIC void ImageAssetSetCellCountY(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellCountY(value);
   }

   DLL_PUBLIC int ImageAssetGetCellWidth(ImageAsset* imageAsset)
   {
      return imageAsset->getCellWidth();
   }

   DLL_PUBLIC void ImageAssetSetCellWidth(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellWidth(value);
   }

   DLL_PUBLIC int ImageAssetGetCellHeight(ImageAsset* imageAsset)
   {
      return imageAsset->getCellHeight();
   }

   DLL_PUBLIC void ImageAssetSetCellHeight(ImageAsset* imageAsset, int value)
   {
      imageAsset->setCellHeight(value);
   }

   DLL_PUBLIC int ImageAssetGetImageWidth(ImageAsset* imageAsset)
   {
      return imageAsset->getImageWidth();
   }

   DLL_PUBLIC int ImageAssetGetImageHeight(ImageAsset* imageAsset)
   {
      return imageAsset->getImageHeight();
   }

   DLL_PUBLIC const char* ImageAssetGetImageSize(ImageAsset* imageAsset)
   {
      // Create Returnable Buffer.
      char* pBuffer = CInterface::GetMarshallableString(32);

      // Format Buffer.
      dSprintf(pBuffer, 32, "%d %d", imageAsset->getImageWidth(), imageAsset->getImageHeight());

      // Return Buffer.
      return pBuffer;
   }

   DLL_PUBLIC bool ImageAssetGetIsImagePOT(ImageAsset* imageAsset)
   {
      return isPow2(imageAsset->getImageWidth()) && isPow2(imageAsset->getImageHeight());
   }

   DLL_PUBLIC int ImageAssetGetFrameCount(ImageAsset* imageAsset)
   {
      return imageAsset->getFrameCount();
   }

   DLL_PUBLIC const char* ImageAssetGetFrameSize(ImageAsset* imageAsset, int frame)
   {
      // Check Frame.
      if (frame < 0 || frame >= (S32)imageAsset->getFrameCount())
      {
         // Warn.
         Con::warnf("ImageAsset::getFrameSize() - Invalid Frame; Allowed range is 0 to %d", imageAsset->getFrameCount() - 1);
         // Finish Here.
         return nullptr;
      }

      // Fetch Selected Frame Pixel Area.
      const ImageAsset::FrameArea::PixelArea& framePixelArea = imageAsset->getImageFrameArea(frame).mPixelArea;

      // Create Returnable Buffer.
      char* pBuffer = CInterface::GetMarshallableString(32);

      // Format Buffer.
      dSprintf(pBuffer, 32, "%d %d", framePixelArea.mPixelWidth, framePixelArea.mPixelHeight);

      // Return Velocity.
      return pBuffer;
   }

   DLL_PUBLIC bool ImageAssetClearExplicitCells(ImageAsset* imageAsset)
   {
      return imageAsset->clearExplicitCells();
   }

   DLL_PUBLIC bool ImageAssetAddExplicitCell(ImageAsset* imageAsset, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, const char* cellName)
   {
      return imageAsset->addExplicitCell(cellOffsetX, cellOffsetY, cellWidth, cellHeight, cellName);
   }

   DLL_PUBLIC bool ImageAssetInsertExplicitCell(ImageAsset* imageAsset, int cellIndex, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, const char* cellName)
   {
      return imageAsset->insertExplicitCell(cellIndex, cellOffsetX, cellOffsetY, cellWidth, cellHeight, cellName);
   }

   DLL_PUBLIC bool ImageAssetRemoveExplicitCell(ImageAsset* imageAsset, int cellIdx)
   {
      return imageAsset->removeExplicitCell(cellIdx);
   }

   DLL_PUBLIC bool ImageAssetSetExplicitCell(ImageAsset* imageAsset, int cellIndex, int cellOffsetX, int cellOffsetY, int cellWidth, int cellHeight, const char* cellName)
   {
      return imageAsset->setExplicitCell(cellIndex, cellOffsetX, cellOffsetY, cellWidth, cellHeight, cellName);
   }

   DLL_PUBLIC int ImageAssetGetExplicitCellCount(ImageAsset* imageAsset)
   {
      return imageAsset->getExplicitCellCount();
   }

   DLL_PUBLIC const char* ImageAssetGetExplicitCellOffset(ImageAsset* imageAsset, int cellIdx)
   {
      char* pBuffer = CInterface::GetMarshallableString(32);
      dSprintf(pBuffer, 32, "%.5g %.5g", imageAsset->getExplicitCellOffset(cellIdx).x, imageAsset->getExplicitCellOffset(cellIdx).y);
      return pBuffer;
   }

   DLL_PUBLIC const char* ImageAssetGetExplicitCellOffsetByName(ImageAsset* imageAsset, const char* cellName)
   {
      // Using cell name.
      ImageAsset::FrameArea& frameRegion = imageAsset->getCellByName(cellName);

      const Vector2 offset = frameRegion.mPixelArea.mPixelOffset;

      char* pBuffer = CInterface::GetMarshallableString(32);
      dSprintf(pBuffer, 32, "%.5g %.5g", offset.x, offset.y);
      return pBuffer;
   }

   DLL_PUBLIC int ImageAssetGetExplicitCellWidth(ImageAsset* imageAsset, int cellIdx)
   {
      return imageAsset->getExplicitCellWidth(cellIdx);
   }

   DLL_PUBLIC int ImageAssetGetExplicitCellWidthByName(ImageAsset* imageAsset, const char* cellName)
   {
      // Using cell name.
      ImageAsset::FrameArea& frameRegion = imageAsset->getCellByName(cellName);

      return frameRegion.mPixelArea.mPixelWidth;
   }

   DLL_PUBLIC int ImageAssetGetExplicitCellHeight(ImageAsset* imageAsset, int cellIdx)
   {
      return imageAsset->getExplicitCellHeight(cellIdx);
   }

   DLL_PUBLIC int ImageAssetGetExplicitCellHeightByName(ImageAsset* imageAsset, const char* cellName)
   {
      // Using cell name.
      ImageAsset::FrameArea& frameRegion = imageAsset->getCellByName(cellName);

      return frameRegion.mPixelArea.mPixelHeight;
   }

   DLL_PUBLIC const char* ImageAssetGetExplicitCellName(ImageAsset* imageAsset, int cellIdx)
   {
      return CInterface::GetMarshallableString(imageAsset->getExplicitCellName(cellIdx));
   }

   DLL_PUBLIC int ImageAssetGetExplicitCellIndex(ImageAsset* imageAsset, const char* cellName)
   {
      return imageAsset->getExplicitCellIndex(cellName);
   }
}