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

#include "2d/core/ImageFrameProviderCore.h"

#ifndef _DGL_H_
#include "graphics/dgl.h"
#endif

#ifndef _CONSOLETYPES_H_
#include "console/consoleTypes.h"
#endif

#ifndef _STRINGBUFFER_H_
#include "string/stringBuffer.h"
#endif

// Debug Profiling.
#include "debug/profiler.h"

//-----------------------------------------------------------------------------

ImageFrameProviderCore::ImageFrameProviderCore() : mpImageAsset(NULL)
{
}

//-----------------------------------------------------------------------------

ImageFrameProviderCore::~ImageFrameProviderCore()
{
    // Reset the state.
    resetState();
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::allocateAssets( AssetPtr<ImageAsset>* pImageAssetPtr )
{
    // Sanity!
    AssertFatal( pImageAssetPtr != NULL, "ImageFrameProviderCore: Image-Asset pointer cannot be NULL." );

    // Reset the state.
    resetState();

    // Set asset pointers.
    mpImageAsset = pImageAssetPtr;

    // Set static/animation frame provider.
    mStaticProvider = mpImageAsset->notNull();
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::resetState( void )
{
    mSelfTick = false;

    mCurrentFrameIndex = 0;
    mLastFrameIndex = 0;
    mMaxFrameIndex = 0;
    mCurrentTime = 0.0f;
    mPausedTime = 0.0f;
    mAnimationTimeScale = 1.0f;
    mTotalIntegrationTime = 0.0f;
    mFrameIntegrationTime = 0.0f;
    mAnimationPaused = false;
    mAnimationFinished = true;

    clearAssets();
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::copyTo( ImageFrameProviderCore* pImageFrameProviderCore ) const
{
    // Sanity!
    AssertFatal(pImageFrameProviderCore != NULL, "ImageFrameProviderCore::copyTo - Copy object cannot be NULL.");

    // Set self ticking.
    pImageFrameProviderCore->mSelfTick = mSelfTick;

    // Static provider?
    if ( isStaticFrameProvider() )
    {
        // Yes, so use the image/frame if we have an asset.
        if ( mpImageAsset->notNull() )
        {
            // Named image frame?
            if ( !isUsingNamedImageFrame() )
                pImageFrameProviderCore->setImage( getImage(), getImageFrame() );
            else
                pImageFrameProviderCore->setImage( getImage(), getNamedImageFrame() );
        }
    }
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::update( const F32 elapsedTime )
{
    // Static provider?
    if ( isStaticFrameProvider() )
    {
        // Yes, so turn-off tick processing.
        setProcessTicks( false );

        return false;
    }

    // Turn-off tick processing.
    setProcessTicks( false );

    // Perform callback.
    onAnimationEnd();

    // Flag animation as just finished.
    return true;
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::processTick( void )
{
    // Update using tick period.
    update( Tickable::smTickSec );
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::validRender( void ) const
{
    // Are we in static mode?
    if ( isStaticFrameProvider() )
    {
        // Yes, so we must have an image asset and the frame must be in bounds.
        if (!isUsingNamedImageFrame())
            return mpImageAsset->notNull() && ( getImageFrame() < (*mpImageAsset)->getFrameCount() );
        else
            return mpImageAsset->notNull() && getNamedImageFrame() != StringTable->EmptyString && ( (*mpImageAsset)->containsFrame(getNamedImageFrame()) );
    }

    // No, so if the animation must be valid.
    return false;
}

//------------------------------------------------------------------------------

const ImageAsset::FrameArea& ImageFrameProviderCore::getProviderImageFrameArea( void ) const
{
    // If this does not have a valid render state, return a bad frame
    if (!validRender())
        return BadFrameArea;
    
    // If it is a static frame and it's not using named frames, get the image area based mImageFrame
    // If it is a static frame and it's using named frames, get the image area based on mImageNameFrame
    // Otherwise, get the current animation frame
    if (isStaticFrameProvider())
        return !isUsingNamedImageFrame() ? (*mpImageAsset)->getImageFrameArea(mImageFrame) : (*mpImageAsset)->getImageFrameArea(mNamedImageFrame);
    
    // If we got here for some reason, that's bad. So return a bad area frame
    return BadFrameArea;
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::render(
    const bool flipX,
    const bool flipY,
    const Vector2& vertexPos0,
    const Vector2& vertexPos1,
    const Vector2& vertexPos2,
    const Vector2& vertexPos3 ) const
{
    // Finish if we can't render.
    if ( !validRender() )
        return;

    // Fetch texel area.
    ImageAsset::FrameArea::TexelArea texelArea = getProviderImageFrameArea().mTexelArea;

    // Flip texture coordinates appropriately.
    texelArea.setFlip( flipX, flipY );

    // Fetch lower/upper texture coordinates.
    const Vector2& texLower = texelArea.mTexelLower;
    const Vector2& texUpper = texelArea.mTexelUpper;
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::renderGui( GuiControl& owner, Point2I offset, const RectI &updateRect ) const
{
    // Validate frame provider.
    if ( ( isStaticFrameProvider() && (mpImageAsset->isNull() || mImageFrame >= (*mpImageAsset)->getFrameCount()) ) ||
        ( !isStaticFrameProvider() ))
    {
        // Invalid so fetch the 'cannot render' proxy.
        //RenderProxy* pNoImageRenderProxy = Sim::findObject<RenderProxy>( CANNOT_RENDER_PROXY_NAME );

        // Check that the render proxy can render.
        //if ( pNoImageRenderProxy != NULL && pNoImageRenderProxy->validRender() )
        //{
            // Render using render-proxy.
        //    pNoImageRenderProxy->renderGui( owner, offset, updateRect );
        //}

        // Update control.
        owner.setUpdate();
    }
    else
    {
        // Valid, so calculate source region.
        const ImageAsset::FrameArea& frameArea = getProviderImageFrameArea();
        RectI sourceRegion( frameArea.mPixelArea.mPixelOffset, Point2I(frameArea.mPixelArea.mPixelWidth, frameArea.mPixelArea.mPixelHeight) );

        // Calculate destination region.
        RectI destinationRegion(offset, owner.mBounds.extent);

        // Render image.
        dglSetBitmapModulation( owner.mProfile->mFillColor );
        dglDrawBitmapStretchSR( getProviderTexture(), destinationRegion, sourceRegion );
        dglClearBitmapModulation();
    }

    // Render child controls.
    owner.renderChildControls(offset, updateRect);
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::setImage( const char* pImageAssetId, const U32 frame )
{
    // Finish if invalid image asset.
    if ( pImageAssetId == NULL )
        return false;

    // Set asset.
    mpImageAsset->setAssetId( pImageAssetId );

    // Set the image frame if the image asset was set.
    if ( mpImageAsset->notNull() )
        setImageFrame( frame );

    // Set as static provider.
    mStaticProvider = true;

    // Turn-off tick processing.
    setProcessTicks( false );

    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::setImage( const char* pImageAssetId, const char* pNamedFrame )
{
    // Finish if invalid image asset.
    if ( pImageAssetId == NULL )
        return false;
    
    // Set asset.
    mpImageAsset->setAssetId( pImageAssetId );
    
    // Set the image frame if the image asset was set.
    if ( mpImageAsset->notNull() )
        setNamedImageFrame( pNamedFrame );
    
    // Set as static provider.
    mStaticProvider = true;
    
    // Turn-off tick processing.
    setProcessTicks( false );
    
    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

bool ImageFrameProviderCore::setImageFrame( const U32 frame )
{
    // Check Existing Image.
    if ( mpImageAsset->isNull() )
    {
        // Warn.
        Con::warnf("ImageFrameProviderCore::setImageFrame() - Cannot set Frame without existing asset Id.");

        // Return Here.
        return false;
    }

    // Check Frame Validity.
    if ( frame >= (*mpImageAsset)->getFrameCount() )
    {
        // Warn.
        Con::warnf("ImageFrameProviderCore::setImageFrame() - Invalid Frame #%d for asset Id '%s'.", frame, mpImageAsset->getAssetId());
        // Return Here.
        return false;
    }

    // Set Frame.
    mImageFrame = frame;

    // Using a numerical frame index.
    mUsingNamedFrame = false;

    // Return Okay.
    return true;
}

//-----------------------------------------------------------------------------

bool ImageFrameProviderCore::setNamedImageFrame(const char* pNamedFrame)
{
    // Check Existing Image.
    if ( mpImageAsset->isNull() )
    {
        // Warn.
        Con::warnf("ImageFrameProviderCore::setNamedImageFrame() - Cannot set Frame without existing asset Id.");
        
        // Return Here.
        return false;
    }
    
    // Check Frame Validity.
    if ( pNamedFrame == StringTable->EmptyString )
    {
        // Warn.
        Con::warnf( "ImageFrameProviderCore::setNamedImageFrame() - Invalid Frame %s for asset Id '%s'.", pNamedFrame, mpImageAsset->getAssetId() );
        // Return Here.
        return false;
    }
    
    // Set Frame.
    mNamedImageFrame = StringTable->insert(pNamedFrame);
    mUsingNamedFrame = true;
    
    // Return Okay.
    return true;
}

//------------------------------------------------------------------------------

void ImageFrameProviderCore::clearAssets( void )
{
    if ( mpImageAsset != NULL )
        mpImageAsset->clear();

    // Reset remaining state.
    mImageFrame = 0;
    mNamedImageFrame = StringTable->EmptyString;
    mStaticProvider = true;
    setProcessTicks( false );
}

//-----------------------------------------------------------------------------

void ImageFrameProviderCore::onAssetRefreshed( AssetPtrBase* pAssetPtrBase )
{
    // Don't perform any action if the animation is not already playing.
    if ( mAnimationFinished )
        return;
}
