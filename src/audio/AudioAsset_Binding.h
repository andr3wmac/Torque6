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

extern "C"{
   DLL_PUBLIC AudioAsset* AudioAssetCreateInstance()
   {
      return new AudioAsset();
   }

   DLL_PUBLIC const char* AudioAssetGetAudioFile(AudioAsset* audioAsset)
   {
      return CInterface::GetMarshallableString(audioAsset->getAudioFile());
   }

   DLL_PUBLIC void AudioAssetSetAudioFile(AudioAsset* audioAsset, const char* val)
   {
      audioAsset->setAudioFile(val);
   }

   DLL_PUBLIC float AudioAssetGetVolume(AudioAsset* audioAsset)
   {
      return audioAsset->getVolume();
   }

   DLL_PUBLIC void AudioAssetSetVolume(AudioAsset* audioAsset, float val)
   {
      audioAsset->setLooping(val);
   }

   DLL_PUBLIC int AudioAssetGetVolumeChannel(AudioAsset* audioAsset)
   {
      return audioAsset->getVolumeChannel();
   }

   DLL_PUBLIC void AudioAssetSetVolumeChannel(AudioAsset* audioAsset, int val)
   {
      audioAsset->setVolumeChannel(val);
   }

   DLL_PUBLIC bool AudioAssetGetLooping(AudioAsset* audioAsset)
   {
      return audioAsset->getLooping();
   }

   DLL_PUBLIC void AudioAssetSetLooping(AudioAsset* audioAsset, bool val)
   {
      audioAsset->setLooping(val);
   }

   DLL_PUBLIC bool AudioAssetGetStreaming(AudioAsset* audioAsset)
   {
      return audioAsset->getStreaming();
   }

   DLL_PUBLIC void AudioAssetSetStreaming(AudioAsset* audioAsset, bool val)
   {
      audioAsset->setStreaming(val);
   }
}