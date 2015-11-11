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

ConsoleMethodGroupBeginWithDocs(AnimationComponent, BaseComponent)

// Nothing Yet

ConsoleMethodGroupEndWithDocs(AnimationComponent)

namespace Scene{
   extern "C" {
      DLL_PUBLIC AnimationComponent* AnimationComponentCreateInstance()
      {
         return new AnimationComponent();
      }

      DLL_PUBLIC F32 AnimationComponentGetSpeed(AnimationComponent* animationComponent)
      {
         return animationComponent->getSpeed();
      }

      DLL_PUBLIC void AnimationComponentSetSpeed(AnimationComponent* animationComponent, F32 val)
      {
         animationComponent->setSpeed(val);
      }

      DLL_PUBLIC MeshComponent* AnimationComponentGetTarget(AnimationComponent* animationComponent)
      {
         return animationComponent->getTarget();
      }

      DLL_PUBLIC void AnimationComponentSetTarget(AnimationComponent* animationComponent, MeshComponent* val)
      {
         animationComponent->setTarget(val);
      }

      DLL_PUBLIC const char* AnimationComponentGetMeshAsset(AnimationComponent* animationComponent)
      {
         return CInterface::GetMarshallableString(animationComponent->getMesh().getAssetId());
      }

      DLL_PUBLIC void AnimationComponentSetMeshAsset(AnimationComponent* animationComponent, const char* meshAssetId)
      {
         animationComponent->setMesh(meshAssetId);
      }
   }
}