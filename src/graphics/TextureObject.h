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

#ifndef _TEXTURE_OBJECT_H_
#define _TEXTURE_OBJECT_H_

#ifndef _PLATFORM_H_
#include "platform/platform.h"
#endif

#ifndef _PLATFORMASSERT_H_
#include "platform/platformAssert.h"
#endif

#ifndef _TEXTURE_HANDLE_H_
#include "graphics/TextureHandle.h"
#endif

#ifndef _GBITMAP_H_
#include "graphics/gBitmap.h"
#endif

#ifndef BGFX_H_HEADER_GUARD
#include <bgfx.h>
#endif

//-----------------------------------------------------------------------------

class GBitmap;

//------------------------------------------------------------------------------

class TextureObject
{
    friend class TextureManager;
    friend class TextureDictionary;
    friend class TextureHandle;

private:
    TextureObject*  next;
    TextureObject*  prev;
    TextureObject*  hashNext;

    S32                 mRefCount;
    StringTableEntry    mTextureKey;
    GBitmap*            mpBitmap;
    U32                 mTextureWidth;
    U32                 mTextureHeight;
    U32                 mBitmapWidth;
    U32                 mBitmapHeight;
    bool                mClamp;

    U8*                 mTempBuf;
    bgfx::TextureHandle mBGFXTexture;
    U32                 mFlags;

    TextureHandle::TextureHandleType mHandleType;

public:
    TextureObject() :
        next( NULL ), prev( NULL ), hashNext( NULL ),
        mRefCount( 0 ),
        mTextureKey( NULL ),
        mpBitmap( NULL ),
        mTextureWidth( 0 ),
        mTextureHeight( 0 ),
        mBitmapWidth( 0 ),
        mBitmapHeight( 0 ),
        mClamp( false ),
        mHandleType( TextureHandle::InvalidTexture )
    {
       mTempBuf = NULL;
       mBGFXTexture.idx = bgfx::invalidHandle;
       mFlags = BGFX_TEXTURE_NONE;
    }

    inline StringTableEntry getTextureKey( void ) { return mTextureKey; }
    inline const GBitmap* getBitmap( void ) { return mpBitmap; }
    inline U32 getTextureWidth( void ) { return mTextureWidth; }
    inline U32 getTextureHeight( void ) { return mTextureHeight; }
    inline U32 getBitmapWidth( void ) { return mBitmapWidth; }
    inline U32 getBitmapHeight( void ) { return mBitmapHeight; }
    inline bool getClamp( void ) { return mClamp; }
    inline TextureHandle::TextureHandleType getHandleType( void ) { return mHandleType; }

    inline bgfx::TextureHandle getBGFXTexture( void ) { return mBGFXTexture; }
};

#endif // _TEXTURE_OBJECT_H_
