/*******************************************************************************
Copyright (c) 2005-2009 David Williams

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would be
    appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source
    distribution. 	
*******************************************************************************/

#ifndef __AmbientOcclusionCalculator_H__
#define __AmbientOcclusionCalculator_H__

#include "Impl/RandomUnitVectors.h"
#include "Impl/RandomVectors.h"

#include "PolyVoxCore/Array.h"
#include "PolyVoxCore/Region.h"
#include "PolyVoxCore/Raycast.h"

//These two should not be here!
#include "PolyVoxCore/Material.h"
#include "PolyVoxCore/SimpleVolume.h"

#include <algorithm>

namespace PolyVox
{
	/**
	 * \file
	 * 
	 * Ambient occlusion
	 */
	
	template<typename IsVoxelTransparentCallback>
	class AmbientOcclusionCalculatorRaycastCallback
	{
	public:
		AmbientOcclusionCalculatorRaycastCallback(IsVoxelTransparentCallback isVoxelTransparentCallback) : mIsVoxelTransparentCallback(isVoxelTransparentCallback)
		{
		}

		bool operator()(const SimpleVolume<uint8_t>::Sampler& sampler)
		{
			uint8_t sample = sampler.getVoxel();
			bool func = mIsVoxelTransparentCallback(sample);
			return func;
		}

		IsVoxelTransparentCallback mIsVoxelTransparentCallback;
	};

	// NOTE: The callback needs to be a functor not a function. I haven't been
	// able to work the required template magic to get functions working as well.
	// 
	// Matt: If you make the function take a "IsVoxelTransparentCallback&&" then
	// it will forward it on. Works for functors, functions and lambdas.
	// This will be 'perfect forwarding' using 'universal references'
	// This will require C++11 rvalue references which is why I haven't made the
	// change yet.
	
	/// Calculate the ambient occlusion for the volume
	template<typename VolumeType, typename IsVoxelTransparentCallback>
	void calculateAmbientOcclusion(VolumeType* volInput, Array<3, uint8_t>* arrayResult, Region region, float fRayLength, uint8_t uNoOfSamplesPerOutputElement, IsVoxelTransparentCallback isVoxelTransparentCallback);
}

#include "PolyVoxCore/AmbientOcclusionCalculator.inl"

#endif //__AmbientOcclusionCalculator_H__
