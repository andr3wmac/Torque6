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

#include "graphics/TextureManager.h"

#include "platform/platformAssert.h"
#include "platform/platform.h"
#include "collection/vector.h"
#include "io/resource/resourceManager.h"
#include "graphics/gBitmap.h"
#include "console/console.h"
#include "console/consoleInternal.h"
#include "console/consoleTypes.h"
#include "memory/safeDelete.h"
#include "math/mMath.h"

#include "TextureManager_Binding.h"

#include <nanovg/nanovg.h>
#include "graphics/dgl.h"
#include <bx/timer.h>

//---------------------------------------------------------------------------------------------------------------------

S32 TextureManager::mMasterTextureKeyIndex = 0;
TextureManager::ManagerState TextureManager::mManagerState = TextureManager::NotInitialized; 
bool TextureManager::mDGLRender = true;
bool TextureManager::mForce16BitTexture = false;
bool TextureManager::mAllowTextureCompression = false;
bool TextureManager::mDisableTextureSubImageUpdates = false;

//---------------------------------------------------------------------------------------------------------------------

#ifdef TORQUE_OS_IOS
#define EXT_ARRAY_SIZE 4
    static const char* extArray[EXT_ARRAY_SIZE] = { "", ".pvr", ".jpg", ".png"};
#else
#define EXT_ARRAY_SIZE 4
static const char* extArray[EXT_ARRAY_SIZE] = { "", ".jpg", ".png", ".dds"};
#endif

//---------------------------------------------------------------------------------------------------------------------

struct EventCallbackEntry
{
    TextureManager::TextureEventCallback callback;
    void *               userData;
    U32                  key;
};

static U32                        sgCurrCallbackKey = 0;

static Vector<EventCallbackEntry> sgEventCallbacks(__FILE__, __LINE__);

//--------------------------------------------------------------------------------------------------------------------

U32 TextureManager::registerEventCallback(TextureEventCallback callback, void *userData)
{
    sgEventCallbacks.increment();
    sgEventCallbacks.last().callback = callback;
    sgEventCallbacks.last().userData = userData;
    sgEventCallbacks.last().key      = sgCurrCallbackKey++;

    return sgEventCallbacks.last().key;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::unregisterEventCallback(const U32 callbackKey)
{
    for (S32 i = 0; i < sgEventCallbacks.size(); i++)
    {
        if (sgEventCallbacks[i].key == callbackKey)
        {
            sgEventCallbacks.erase(i);
            return;
        }
    }
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::postTextureEvent(const TextureEventCode eventCode)
{
    for (S32 i = 0; i < sgEventCallbacks.size(); i++)
    {
        (sgEventCallbacks[i].callback)(eventCode, sgEventCallbacks[i].userData);
    }
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::create()
{
    AssertISV(mManagerState == NotInitialized, "TextureManager::create() - already created!");

    TextureDictionary::create();

    Con::addVariable("$pref::OpenGL::force16BitTexture", TypeBool, &TextureManager::mForce16BitTexture);
    Con::addVariable("$pref::OpenGL::allowTextureCompression", TypeBool, &TextureManager::mAllowTextureCompression);
    Con::addVariable("$pref::OpenGL::disableTextureSubImageUpdates", TypeBool, &TextureManager::mDisableTextureSubImageUpdates);

    // Flag as alive.
    mManagerState = Alive;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::destroy()
{
    AssertISV(mManagerState != NotInitialized, "TextureManager::destroy - nothing to destroy!");

    // Destroy the texture dictionary.
    TextureDictionary::destroy();

    // Reset state.
    mMasterTextureKeyIndex = 0;

    // Flag as not initialized.
    mManagerState = NotInitialized;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::killManager()
{
    // Finish if already dead.
    if ( mManagerState == Dead )
        return;

    // Flag as dead.
    mManagerState = Dead;

    // Post zombie event.
    postTextureEvent(BeginZombification);

    TextureObject* probe = TextureDictionary::TextureObjectChain;
    while (probe) 
    {
        // TODO: Should we delete all the BGFX textures?
        probe = probe->next;
    }
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::resurrectManager( void )
{
    // Finish if not dead.
    if (mManagerState != Dead)
        return;

    // Flag as resurrecting.
    mManagerState = Resurrecting;

    // Post begin resurrection event.
    postTextureEvent(BeginResurrection);

    // Resurrect textures.
    TextureObject* probe = TextureDictionary::TextureObjectChain;
    while (probe) 
    {
        switch( probe->mHandleType )
        {
            case TextureHandle::BitmapTexture:
                {
                    // Sanity!
                    AssertISV( probe->mTextureKey != NULL && probe->mTextureKey != StringTable->EmptyString, "Encountered a bitmap texture that didn't specify its bitmap." );

                    // Load the bitmap.
                    GBitmap* pBitmap = loadBitmap( probe->mTextureKey );

                    // Sanity!
                    AssertISV(pBitmap != NULL, "Error resurrecting the texture cache.\n""Possible cause: a bitmap was deleted during the course of gameplay.");

                    // Register texture.
                    TextureObject* pTextureObject;
                    pTextureObject = registerTexture(probe->mTextureKey, pBitmap, probe->mHandleType, probe->mClamp);

                    // Sanity!
                    AssertFatal(pTextureObject == probe, "A new texture was returned during resurrection.");

                } break;

            case TextureHandle::BitmapKeepTexture:
                {
                    // Sanity!
                    AssertISV( probe->mpBitmap != NULL, "Encountered no bitmap for a texture that should keep it." );

                    // Create texture.
                    createBGFXTexture(probe);

                } break;

            default:
                // Sanity!
                AssertISV( false, "Unknown texture type encountered during texture resurrection." );
        }

        // Next texture.
        probe = probe->next;
    }

    // Post end resurrection event.
    postTextureEvent(EndResurrection);

    // Flag as alive.
    mManagerState = Alive;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::flush()
{
    killManager();
    resurrectManager();
}

//--------------------------------------------------------------------------------------------------------------------

StringTableEntry TextureManager::getUniqueTextureKey( void )
{
    char textureKeyBuffer[32];
    dSprintf( textureKeyBuffer, sizeof(textureKeyBuffer), "GeneratedKey_%d", mMasterTextureKeyIndex++ );
    return StringTable->insert( textureKeyBuffer );
}

//--------------------------------------------------------------------------------------------------------------------

GBitmap* TextureManager::createPowerOfTwoBitmap(GBitmap* pBitmap)
{    
    // Sanity!
    AssertISV( pBitmap->getFormat() != GBitmap::Palettized, "Paletted bitmaps are not supported." );

    // Finish if already a power-of-two in dimension.
    if (isPow2(pBitmap->getWidth()) && isPow2(pBitmap->getHeight()))
        return pBitmap;


    U32 width = pBitmap->getWidth();
    U32 height = pBitmap->getHeight();

    U32 newWidth  = getNextPow2(pBitmap->getWidth());
    U32 newHeight = getNextPow2(pBitmap->getHeight());

    GBitmap* pReturn = new GBitmap(newWidth, newHeight, false, pBitmap->getFormat());

    for (U32 i = 0; i < height; i++) 
    {
        U8*       pDest = (U8*)pReturn->getAddress(0, i);
        const U8* pSrc  = (const U8*)pBitmap->getAddress(0, i);

        dMemcpy(pDest, pSrc, width * pBitmap->bytesPerPixel);

        pDest += width * pBitmap->bytesPerPixel;
        // set the src pixel to the last pixel in the row
        const U8 *pSrcPixel = pDest - pBitmap->bytesPerPixel;

        for(U32 j = width; j < newWidth; j++)
            for(U32 k = 0; k < pBitmap->bytesPerPixel; k++)
                *pDest++ = pSrcPixel[k];
    }

    for(U32 i = height; i < newHeight; i++)
    {
        U8* pDest = (U8*)pReturn->getAddress(0, i);
        U8* pSrc = (U8*)pReturn->getAddress(0, height-1);
        dMemcpy(pDest, pSrc, newWidth * pBitmap->bytesPerPixel);
    }

    return pReturn;
}

//---------------------------------------------------------------------------------------------------------------------

void TextureManager::freeTexture( TextureObject* pTextureObject )
{
    if ( pTextureObject->mpBitmap != NULL )
        delete pTextureObject->mpBitmap;

    TextureDictionary::remove(pTextureObject);
    SAFE_DELETE( pTextureObject );
}

//-----------------------------------------------------------------------------

void TextureManager::refresh( TextureObject* pTextureObject )
{
    // Finish if refresh not appropriate.
    if (!(mDGLRender || mManagerState == Resurrecting))
        return;

    //U64 hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
    //U64 startTime = bx::getHPCounter();

    // Sanity!
    AssertISV( pTextureObject->mpBitmap != 0, "Refreshing texture but no bitmap available." );

    // Fetch bitmaps.
    GBitmap* pSourceBitmap = pTextureObject->mpBitmap;
    GBitmap* pNewBitmap = createPowerOfTwoBitmap(pSourceBitmap);
   
    U8 *bits = (U8*)pNewBitmap->getBits();

    // Are we forcing to 16-bit?
    if( pSourceBitmap->mForce16Bit )
    {
        // Yes, so generate a 16-bit texture.
        //GLint GLformat;
        //GLint GLdata_type;

        //U16* pBitmap16 = create16BitBitmap( pNewBitmap, pNewBitmap->getWritableBits(), pNewBitmap->getFormat(), 
        //                                        &GLformat, &GLdata_type,
        //                                        pNewBitmap->getWidth(), pNewBitmap->getHeight() );

        /*glTexImage2D(GL_TEXTURE_2D, 
                        0,
                        GLformat,
                        pNewBitmap->getWidth(), pNewBitmap->getHeight(), 
                        0,
                        GLformat, 
                        GLdata_type,
                        pBitmap16
                    );
                    */
        //copy new texture_data into pBits
        //delete [] pBitmap16;
    }
    else
    {
       if ( pNewBitmap->getFormat() == GBitmap::RGBA )
       {
          // Convert to BGRA
          U32 count = pNewBitmap->getWidth() * pNewBitmap->getHeight();
          pTextureObject->mTempBuf = new U8[count * 4];
          const U8* bits = pNewBitmap->getBits(0);
          
		    U32 bytesPerPixel = 4;
		    U32 pitch = pNewBitmap->getWidth() * bytesPerPixel;

          // TODO: We don't need to generate mips for literally every texture.
          pTextureObject->mBGFXTexture = getMipMappedTexture(pTextureObject->mTextureKey, pNewBitmap->getWidth(), pNewBitmap->getHeight(), bits, pTextureObject->mFlags);
       }

       // TODO: Finish texture loading in all its glorious forms.
       if ( pNewBitmap->getFormat() == GBitmap::Alpha )
          pTextureObject->mBGFXTexture = bgfx::createTexture2D(pNewBitmap->getWidth(), pNewBitmap->getHeight(), 0, bgfx::TextureFormat::R8, pTextureObject->mFlags);

       if ( pNewBitmap->getFormat() == GBitmap::RGB )
       {
          // Convert to BGRA
          U32 count = pNewBitmap->getWidth() * pNewBitmap->getHeight();
          pTextureObject->mTempBuf = new U8[count * 4];
          const U8* bits = pNewBitmap->getBits(0);
          
		    U32 bytesPerPixel = 4;
		    U32 pitch = pNewBitmap->getWidth() * bytesPerPixel;

          U8* rgba_data = new U8[pNewBitmap->getHeight() * pitch];
          swizzleRGBtoRGBA(pNewBitmap->getWidth(), pNewBitmap->getHeight(), bits, rgba_data);
          pTextureObject->mBGFXTexture = getMipMappedTexture(pTextureObject->mTextureKey, pNewBitmap->getWidth(), pNewBitmap->getHeight(), rgba_data);
          SAFE_DELETE(rgba_data);
       }

       if ( pNewBitmap->getFormat() == GBitmap::DDS )
       {
          Con::printf("Asking to load DDS!");
          const bgfx::Memory* mem = NULL;
			 mem = bgfx::alloc(pNewBitmap->getByteSize());
          dMemcpy(mem->data, pNewBitmap->getBits(0), pNewBitmap->getByteSize());
          pTextureObject->mBGFXTexture = bgfx::createTexture(mem,
             pTextureObject->mFlags,
             0,
             NULL);
       }

       if ( pTextureObject->mBGFXTexture.idx == bgfx::invalidHandle )
          Con::printf("Failed to load BGFX texture.");
       else
       {
          // This loads the texture into NanoVG (it doesn't create a second copy on the GPU)
          // This is nessicary in order to use textures in NanoVG calls.
          nvgCreateImageBGFX(dglGetNVGContext(), pNewBitmap->getWidth(), pNewBitmap->getHeight(), NVG_TEXTURE_RGBA, pTextureObject->mBGFXTexture);
       }
    }

    if(pNewBitmap != pSourceBitmap)
        delete pNewBitmap;

    //U64 endTime = bx::getHPCounter();
    //Con::printf("TEXTURE REFRESH TOOK: %d microseconds. (1 microsecond = 0.001 milliseconds)", (U32)((endTime - startTime) / hpFreq));
}

void TextureManager::swizzleRGBtoBGRA(U32 width, U32 height, const U8* src, U8* dest)
{
   U32 count = width * height * 3;
   U32 dest_pos = 0;
   for (U32 n = 0; n < count; n += 3)
   {
      dest[dest_pos] = src[n + 2]; // B
      dest[dest_pos + 1] = src[n + 1]; // G
      dest[dest_pos + 2] = src[n]; // R
      dest[dest_pos + 3] = 255; // A
      dest_pos += 4;
   }
}

void TextureManager::swizzleRGBtoRGBA(U32 width, U32 height, const U8* src, U8* dest)
{
   U32 count = width * height * 3;
   U32 dest_pos = 0;
   for (U32 n = 0; n < count; n += 3)
   {
      dest[dest_pos] = src[n]; // R
      dest[dest_pos + 1] = src[n + 1]; // G
      dest[dest_pos + 2] = src[n + 2]; // B
      dest[dest_pos + 3] = 255; // A
      dest_pos += 4;
   }
}

const bgfx::Memory* TextureManager::generateMipMappedTexture(U32 _numMips, U32 _width, U32 _height, const U8* _src, bool _swizzleToBGRA)
{
   //Con::printf("Generating Mips for %d x %d Texture.", _width, _height);

   // Allocate more than we'll need
   U32 byte_count = 0;
   U8* out_data = new U8[_width * _height * 4 * 2];
   U8* data = new U8[_width * _height * 4];
   dMemcpy(data, _src, _width * _height * 4);

   if ( _swizzleToBGRA )
      bgfx::imageSwizzleBgra8(_width, _height, _width * 4, _src, out_data);
   else
      dMemcpy(out_data, _src, _width * _height * 4);
   
   byte_count += _width * _height * 4;

   U32 width = _width;
   U32 height = _height;
   U32 pitch = _width * 4;

   // Generate mip maps.
   for (U32 i = 0; i < (_numMips - 1); ++i)
   {
      bgfx::imageRgba8Downsample2x2(width, height, pitch, data, data);

      if ( width > 1 ) width >>= 1;
      if ( height > 1 ) height >>= 1;
      pitch = width*4;

      if ( _swizzleToBGRA )
         bgfx::imageSwizzleBgra8(width, height, width * 4, data, &out_data[byte_count]);
      else
         dMemcpy(&out_data[byte_count], data, width * height * 4);

      byte_count += width * height * 4;
   }

   const bgfx::Memory* mem = NULL;
   mem = bgfx::alloc(byte_count);
   dMemcpy(mem->data, out_data, byte_count);

   SAFE_DELETE(data);
   SAFE_DELETE(out_data);

   return mem;
}

static const U32 TextureCacheVersion = 100;

bgfx::TextureHandle TextureManager::getMipMappedTexture(StringTableEntry _textureKey, U32 _width, U32 _height, const U8* _src, U32 _flags, bool _swizzleToBGRA)
{
   U32 numMips = 1 + mFloor(mLog2(_width > _height ? _width : _height));

   // Caching system.
   char cachedFilename[256];
   dSprintf(cachedFilename, 256, "%s.bin", _textureKey);
   StringTableEntry cachedPath = Platform::getCachedFilePath(cachedFilename);

   const bgfx::Memory* mem = NULL;
   FileStream stream;
   if (stream.open(cachedPath, FileStream::ReadWrite))
   {
      U32 version;
      stream.read(&version);
      U32 mips;
      stream.read(&mips);
      U32 size;
      stream.read(&size);

      if ((version == TextureCacheVersion) && (mips == numMips) && (size < (_width * _height * 8)))
      {
         mem = bgfx::alloc(size);
         stream.read(size, mem->data);
      }

      stream.close();
   }

   if ( mem == NULL )
   {
      Con::printf("Generating texture mip maps..");
      mem = generateMipMappedTexture(numMips, _width, _height, _src, _swizzleToBGRA);

      Platform::createPath(cachedPath);
      if (stream.open(cachedPath, FileStream::Write))
      {
         stream.write(TextureCacheVersion);
         stream.write(numMips);
         stream.write(mem->size);
         stream.write(mem->size, mem->data);
         stream.close();
      }
   }

   bgfx::TextureHandle result = BGFX_INVALID_HANDLE;
   result = bgfx::createTexture2D(_width
      , _height
      , numMips
      , bgfx::TextureFormat::BGRA8
      , _flags
      , mem
      );

   return result;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::refresh( const char *textureName )
{
    // Finish if no texture name specified.
    AssertFatal( textureName != NULL, "Texture Manager:  Cannot refresh a NULL texture name." );

    // Find the texture object.
    TextureObject* pTextureObject = TextureDictionary::find( textureName );

    // Finish if no texture for this texture name.
    if ( pTextureObject == NULL )
        return;

    // Finish if the texture object is a kept bitmap.
    if ( pTextureObject->getHandleType() == TextureHandle::BitmapKeepTexture )
        return;

    // Load the bitmap.
    GBitmap* pBitmap = loadBitmap( pTextureObject->mTextureKey );

    // Finish if bitmap could not be loaded.
    if ( pBitmap == NULL )
        return;

    // Register texture.
    TextureObject* pNewTextureObject;
    pNewTextureObject = registerTexture(pTextureObject->mTextureKey, pBitmap, pTextureObject->mHandleType, pTextureObject->mClamp);

    // Sanity!
    AssertFatal(pNewTextureObject == pTextureObject, "A new texture was returned during refresh.");
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::createBGFXTexture( TextureObject* pTextureObject )
{
    // Finish if not appropriate.
    if (!(mDGLRender || mManagerState == Resurrecting))
        return;

    // Sanity!
    AssertISV( pTextureObject->mHandleType != TextureHandle::InvalidTexture, "Invalid texture type." );
    AssertISV( pTextureObject->mpBitmap != NULL, "Bitmap cannot be NULL." );

    // Refresh the texture.
    refresh( pTextureObject );
}

//--------------------------------------------------------------------------------------------------------------------

TextureObject* TextureManager::registerTexture(const char* pTextureKey, GBitmap* pNewBitmap, TextureHandle::TextureHandleType type, U32 flags)
{
    // Sanity!
    AssertISV( type != TextureHandle::InvalidTexture, "Invalid texture type." );

    TextureObject* pTextureObject = NULL;

    // Fetch texture key.
    StringTableEntry textureKey = StringTable->insert(pTextureKey);

    if(pTextureKey)
        pTextureObject = TextureDictionary::find(textureKey, type, flags);

    if( pTextureObject )
    {
        // Remove bitmap if we have a different existing one.
        if ( pTextureObject->mpBitmap != NULL && pTextureObject->mpBitmap != pNewBitmap)
        {
            delete pTextureObject->mpBitmap;
            pTextureObject->mpBitmap = NULL;
        }
    }
    else
    {
        // Create new texture object.
        pTextureObject = new TextureObject();
        pTextureObject->mTextureKey = textureKey;
        pTextureObject->mHandleType = type;

        TextureDictionary::insert(pTextureObject);
    }

    pTextureObject->mpBitmap           = pNewBitmap;
    pTextureObject->mBitmapWidth       = pNewBitmap->getWidth();
    pTextureObject->mBitmapHeight      = pNewBitmap->getHeight();
    pTextureObject->mTextureWidth      = getNextPow2(pNewBitmap->getWidth());
    pTextureObject->mTextureHeight     = getNextPow2(pNewBitmap->getHeight());
    pTextureObject->mFlags             = flags;

    createBGFXTexture(pTextureObject);

    // Delete bitmap if we're not keeping it.
    if ( pTextureObject->mHandleType != TextureHandle::BitmapKeepTexture ) 
    {
        delete pTextureObject->mpBitmap;
        pTextureObject->mpBitmap = NULL;
    }

    return pTextureObject;
}

//--------------------------------------------------------------------------------------------------------------------

TextureObject *TextureManager::loadTexture(const char* pTextureKey, TextureHandle::TextureHandleType type, U32 flags, bool checkOnly, bool force16Bit )
{
    // Sanity!
    AssertISV( type != TextureHandle::InvalidTexture, "Invalid texture type." );

    // Finish if texture key is invalid.
    if( pTextureKey == NULL || *pTextureKey == 0)
        return NULL;

    // Fetch texture key.
    StringTableEntry textureKey = StringTable->insert(pTextureKey);

    TextureObject *ret = TextureDictionary::find(textureKey, type, flags);

    GBitmap *bmp = NULL;

    if( ret == NULL )
    {
        // Ok, no hit - is it in the current dir? If so then let's grab it
        // and use it.
        bmp = loadBitmap(textureKey, false);

        if(bmp)
        {
            bmp->mForce16Bit = force16Bit;
            return registerTexture(textureKey, bmp, type, flags);
        }
    }

    if(ret)
        return ret;

    // If we're just checking, fail out so we eventually get around to
    // loading a real bitmap.
    if(checkOnly)
        return NULL;

    // Ok, no success so let's try actually loading a texture.
    bmp = loadBitmap(textureKey);

    if(!bmp)
    {
        Con::warnf("Could not locate texture: %s", textureKey);
        return NULL;
    }
    bmp->mForce16Bit = force16Bit;

    return registerTexture(textureKey, bmp, type, flags);
}

//--------------------------------------------------------------------------------------------------------------------

GBitmap *TextureManager::loadBitmap( const char* pTextureKey, bool recurse, bool nocompression )
{
    char fileNameBuffer[512];
    Con::expandPath( fileNameBuffer, sizeof(fileNameBuffer), pTextureKey );
    GBitmap *bmp = NULL;

    // Loop through the supported extensions to find the file.
    U32 len = dStrlen(fileNameBuffer);
    for (U32 i = 0; i < EXT_ARRAY_SIZE && bmp == NULL; i++)
    {
#if defined(TORQUE_OS_IOS)
        // check to see if requested no-compression...
        if (nocompression && (dStrncmp( extArray[i], ".pvr", 4 ) == 0)) {
            continue;
        }
#endif
        dStrcpy(fileNameBuffer + len, extArray[i]);

        //ResourceManager->loadInstanceThreaded(fileNameBuffer);

        bmp = (GBitmap*)ResourceManager->loadInstance(fileNameBuffer);

        if ( bmp != NULL && (bmp->getWidth() > MaximumProductSupportedTextureWidth || bmp->getHeight() > MaximumProductSupportedTextureHeight) )
        {
            Con::warnf( "TextureManager::loadBitmap() - Cannot load bitmap '%s' as its dimensions exceed the maximum product-supported texture dimension.", fileNameBuffer );
            delete bmp;
            return NULL;
        }
    }

    return bmp;
}

//--------------------------------------------------------------------------------------------------------------------

void TextureManager::dumpMetrics( void )
{
    S32 textureResidentCount = 0;
    S32 textureResidentSize = 0;
    S32 textureResidentWasteSize = 0;
    S32 bitmapResidentSize = 0;

    Con::printSeparator();
    Con::printBlankLine();
    Con::printf( "Dumping texture manager metrics:" );

    TextureObject* pProbe = TextureDictionary::TextureObjectChain;
    while (pProbe != NULL) 
    {
        // TODO: Restore this.
        pProbe = pProbe->next;
    }

    Con::printSeparator();
    Con::printf( "Texture managed metrics unavailable." );
    Con::printSeparator();
}