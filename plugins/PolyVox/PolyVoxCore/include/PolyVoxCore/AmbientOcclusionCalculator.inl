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

namespace PolyVox
{
	/**
	 * \param volInput The volume to calculate the ambient occlusion for
	 * \param[out] arrayResult The output of the calculator
	 * \param region The region of the volume for which the occlusion should be calculated
	 * \param fRayLength The length for each test ray
	 * \param uNoOfSamplesPerOutputElement The number of samples to calculate the occlusion
	 * \param isVoxelTransparentCallback A callback which takes a \a VoxelType and returns a \a bool whether the voxel is transparent
	 */
	template<typename VolumeType, typename IsVoxelTransparentCallback>
	void calculateAmbientOcclusion(VolumeType* volInput, Array<3, uint8_t>* arrayResult, Region region, float fRayLength, uint8_t uNoOfSamplesPerOutputElement, IsVoxelTransparentCallback isVoxelTransparentCallback)
	{
		typename VolumeType::Sampler m_sampVolume(volInput);

		uint16_t uRandomUnitVectorIndex = 0;
		uint16_t uRandomVectorIndex = 0;
		uint16_t uIndexIncreament;

		//Make sure that the size of the volume is an exact multiple of the size of the array.
		assert(volInput->getWidth() % arrayResult->getDimension(0) == 0);
		assert(volInput->getHeight() % arrayResult->getDimension(1) == 0);
		assert(volInput->getDepth() % arrayResult->getDimension(2) == 0);

		//Our initial indices. It doesn't matter exactly what we set here, but the code below makes 
		//sure they are different for different regions which helps reduce tiling patterns in the results.
		uRandomUnitVectorIndex += region.getLowerCorner().getX() + region.getLowerCorner().getY() + region.getLowerCorner().getZ();
		uRandomVectorIndex += region.getLowerCorner().getX() + region.getLowerCorner().getY() + region.getLowerCorner().getZ();

		//This value helps us jump around in the array a bit more, so the
		//nth 'random' value isn't always followed by the n+1th 'random' value.
		uIndexIncreament = 1;

		const int iRatioX = volInput->getWidth()  / arrayResult->getDimension(0);
		const int iRatioY = volInput->getHeight() / arrayResult->getDimension(1);
		const int iRatioZ = volInput->getDepth()  / arrayResult->getDimension(2);

		const float fRatioX = iRatioX;
		const float fRatioY = iRatioY;
		const float fRatioZ = iRatioZ;
		const Vector3DFloat v3dRatio(fRatioX, fRatioY, fRatioZ);

		const float fHalfRatioX = fRatioX * 0.5f;
		const float fHalfRatioY = fRatioY * 0.5f;
		const float fHalfRatioZ = fRatioZ * 0.5f;
		const Vector3DFloat v3dHalfRatio(fHalfRatioX, fHalfRatioY, fHalfRatioZ);

		const Vector3DFloat v3dOffset(0.5f,0.5f,0.5f);

		//This loop iterates over the bottom-lower-left voxel in each of the cells in the output array
		for(uint16_t z = region.getLowerCorner().getZ(); z <= region.getUpperCorner().getZ(); z += iRatioZ)
		{
			for(uint16_t y = region.getLowerCorner().getY(); y <= region.getUpperCorner().getY(); y += iRatioY)
			{
				for(uint16_t x = region.getLowerCorner().getX(); x <= region.getUpperCorner().getX(); x += iRatioX)
				{
					//Compute a start position corresponding to 
					//the centre of the cell in the output array.
					Vector3DFloat v3dStart(x, y, z);
					v3dStart -= v3dOffset;
					v3dStart += v3dHalfRatio;

					//Keep track of how many rays did not hit anything
					uint8_t uVisibleDirections = 0;

					for(int ct = 0; ct < uNoOfSamplesPerOutputElement; ct++)
					{						
						//We take a random vector with components going from -1 to 1 and scale it to go from -halfRatio to +halfRatio.
						//This jitter value moves our sample point from the centre of the array cell to somewhere else in the array cell
						Vector3DFloat v3dJitter = randomVectors[(uRandomVectorIndex += (++uIndexIncreament)) % 1019]; //Prime number helps avoid repetition on successive loops.
						v3dJitter *= v3dHalfRatio;
						const Vector3DFloat v3dRayStart = v3dStart + v3dJitter;

						Vector3DFloat v3dRayDirection = randomUnitVectors[(uRandomUnitVectorIndex += (++uIndexIncreament)) % 1021]; //Different prime number.
						v3dRayDirection *= fRayLength;
						
						AmbientOcclusionCalculatorRaycastCallback<IsVoxelTransparentCallback> ambientOcclusionCalculatorRaycastCallback(isVoxelTransparentCallback);
						RaycastResult result = raycastWithDirection(volInput, v3dRayStart, v3dRayDirection, ambientOcclusionCalculatorRaycastCallback);

						if(result == RaycastResults::Completed)
						{
							++uVisibleDirections;
						}
					}

					float fVisibility;
					if(uNoOfSamplesPerOutputElement == 0)
					{
						//The user might request zero samples (I've done this in the past while debugging - I don't want to
						//wait for ambient occlusion but I do want as valid result for rendering). Avoid the divide by zero.
						fVisibility = 1.0f;
					}
					else
					{
						fVisibility = static_cast<float>(uVisibleDirections) / static_cast<float>(uNoOfSamplesPerOutputElement);
						assert((fVisibility >= 0.0f) && (fVisibility <= 1.0f));
					}

					(*arrayResult)[z / iRatioZ][y / iRatioY][x / iRatioX] = static_cast<uint8_t>(255.0f * fVisibility);
				}
			}
		}
	}
}
