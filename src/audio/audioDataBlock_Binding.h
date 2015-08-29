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

#include "audioDataBlock.h"

extern "C" {
   DLL_PUBLIC AudioEnvironment* AudioEnvironmentCreateInstance()
   {
      return new AudioEnvironment();
   }

   DLL_PUBLIC bool AudioEnvironmentGetUseRoom(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mUseRoom;
   }

   DLL_PUBLIC void AudioEnvironmentSetUseRoom(AudioEnvironment* audioEnvironment, bool useRoom)
   {
      audioEnvironment->mUseRoom = useRoom;
   }

   DLL_PUBLIC S32 AudioEnvironmentGetRoom(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mRoom;
   }

   DLL_PUBLIC void AudioEnvironmentSetRoom(AudioEnvironment* audioEnvironment, S32 room)
   {
      audioEnvironment->mRoom = room;
   }

   DLL_PUBLIC S32 AudioEnvironmentGetRoomHF(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mRoomHF;
   }

   DLL_PUBLIC void AudioEnvironmentSetRoomHF(AudioEnvironment* audioEnvironment, S32 room)
   {
      audioEnvironment->mRoomHF = room;
   }

   DLL_PUBLIC S32 AudioEnvironmentGetReflections(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mReflections;
   }

   DLL_PUBLIC void AudioEnvironmentSetReflections(AudioEnvironment* audioEnvironment, S32 reflections)
   {
      audioEnvironment->mReflections = reflections;
   }

   DLL_PUBLIC S32 AudioEnvironmentGetReverb(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mReverb;
   }

   DLL_PUBLIC void AudioEnvironmentSetReverb(AudioEnvironment* audioEnvironment, S32 reverb)
   {
      audioEnvironment->mReverb = reverb;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetRoomRolloffFactor(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mRoomRolloffFactor;
   }

   DLL_PUBLIC void AudioEnvironmentSetRoomRolloffFactor(AudioEnvironment* audioEnvironment, F32 factor)
   {
      audioEnvironment->mRoomRolloffFactor = factor;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetDecayTime(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mDecayTime;
   }

   DLL_PUBLIC void AudioEnvironmentSetDecayTime(AudioEnvironment* audioEnvironment, F32 time)
   {
      audioEnvironment->mDecayTime = time;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetDecayHFRatio(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mDecayHFRatio;
   }

   DLL_PUBLIC void AudioEnvironmentSetDecayHFRatio(AudioEnvironment* audioEnvironment, F32 ratio)
   {
      audioEnvironment->mDecayHFRatio = ratio;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetReflectionsDelay(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mReflectionsDelay;
   }

   DLL_PUBLIC void AudioEnvironmentSetReflectionsDelay(AudioEnvironment* audioEnvironment, F32 delay)
   {
      audioEnvironment->mReflectionsDelay = delay;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetReverbDelay(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mReverbDelay;
   }

   DLL_PUBLIC void AudioEnvironmentSetReverbDelay(AudioEnvironment* audioEnvironment, F32 delay)
   {
      audioEnvironment->mReverbDelay = delay;
   }

   DLL_PUBLIC S32 AudioEnvironmentGetRoomVolume(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mRoomVolume;
   }

   DLL_PUBLIC void AudioEnvironmentSetRoomVolume(AudioEnvironment* audioEnvironment, F32 volume)
   {
      audioEnvironment->mRoomVolume = volume;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetEffectVolume(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mEffectVolume;
   }

   DLL_PUBLIC void AudioEnvironmentSetEffectVolume(AudioEnvironment* audioEnvironment, F32 volume)
   {
      audioEnvironment->mEffectVolume = volume;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetDamping(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mDamping;
   }

   DLL_PUBLIC void AudioEnvironmentSetDamping(AudioEnvironment* audioEnvironment, F32 damping)
   {
      audioEnvironment->mDamping = damping;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetEnvironmentSize(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mEnvironmentSize;
   }

   DLL_PUBLIC void AudioEnvironmentSetEnvironmentSize(AudioEnvironment* audioEnvironment, F32 size)
   {
      audioEnvironment->mEnvironmentSize = size;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetEnvironmentDiffusion(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mEnvironmentDiffusion;
   }

   DLL_PUBLIC void AudioEnvironmentSetEnvironmentDiffusion(AudioEnvironment* audioEnvironment, F32 diffusion)
   {
      audioEnvironment->mEnvironmentDiffusion = diffusion;
   }

   DLL_PUBLIC F32 AudioEnvironmentGetAirAbsorption(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mAirAbsorption;
   }

   DLL_PUBLIC void AudioEnvironmentSetAirAbsorption(AudioEnvironment* audioEnvironment, F32 absorption)
   {
      audioEnvironment->mAirAbsorption = absorption;
   }

   DLL_PUBLIC S32 AudioEnvironmentGetFlags(AudioEnvironment* audioEnvironment)
   {
      return audioEnvironment->mFlags;
   }

   DLL_PUBLIC void AudioEnvironmentSetFlags(AudioEnvironment* audioEnvironment, S32 flags)
   {
      audioEnvironment->mFlags = flags;
   }

   DLL_PUBLIC AudioSampleEnvironment* AudioSampleEnvironmentCreateInstance()
   {
      return new AudioSampleEnvironment();
   }

   DLL_PUBLIC S32 AudioSampleEnvironmentGetDirect(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mDirect;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetDirect(AudioSampleEnvironment* audioEnvironment, S32 direct)
   {
      audioEnvironment->mDirect = direct;
   }

   DLL_PUBLIC S32 AudioSampleEnvironmentGetDirectHF(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mDirectHF;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetDirectHF(AudioSampleEnvironment* audioEnvironment, S32 directHF)
   {
      audioEnvironment->mDirectHF = directHF;
   }

   DLL_PUBLIC S32 AudioSampleEnvironmentGetRoom(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mRoom;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetRoom(AudioSampleEnvironment* audioEnvironment, S32 room)
   {
      audioEnvironment->mRoom = room;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetObstruction(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mObstruction;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetObstruction(AudioSampleEnvironment* audioEnvironment, F32 obstruction)
   {
      audioEnvironment->mObstruction = obstruction;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetObstructionLFRatio(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mObstructionLFRatio;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetObstructionLFRatio(AudioSampleEnvironment* audioEnvironment, F32 ratio)
   {
      audioEnvironment->mObstructionLFRatio = ratio;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetOcclusion(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mOcclusion;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetOcclusion(AudioSampleEnvironment* audioEnvironment, F32 occlusion)
   {
      audioEnvironment->mOcclusion = occlusion;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetOcclusionLFRatio(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mOcclusionLFRatio;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetOcclusionLFRatio(AudioSampleEnvironment* audioEnvironment, F32 ratio)
   {
      audioEnvironment->mOcclusionLFRatio = ratio;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetOcclusionRoomRatio(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mOcclusionRoomRatio;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetOcclusionRoomRatio(AudioSampleEnvironment* audioEnvironment, F32 ratio)
   {
      audioEnvironment->mOcclusionRoomRatio = ratio;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetRoomRolloff(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mRoomRolloff;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetRoomRolloff(AudioSampleEnvironment* audioEnvironment, F32 rolloff)
   {
      audioEnvironment->mRoomRolloff = rolloff;
   }

   DLL_PUBLIC F32 AudioSampleEnvironmentGetAirAbsorption(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mAirAbsorption;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetAirAbsorption(AudioSampleEnvironment* audioEnvironment, F32 absorption)
   {
      audioEnvironment->mAirAbsorption = absorption;
   }

   DLL_PUBLIC S32 AudioSampleEnvironmentGetOutsideVolumeHF(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mOutsideVolumeHF;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetOutsideVolumeHF(AudioSampleEnvironment* audioEnvironment, S32 volume)
   {
      audioEnvironment->mOutsideVolumeHF = volume;
   }

   DLL_PUBLIC S32 AudioSampleEnvironmentGetFlags(AudioSampleEnvironment* audioEnvironment)
   {
      return audioEnvironment->mFlags;
   }

   DLL_PUBLIC void AudioSampleEnvironmentSetFlags(AudioSampleEnvironment* audioEnvironment, S32 flags)
   {
      audioEnvironment->mFlags = flags;
   }
}