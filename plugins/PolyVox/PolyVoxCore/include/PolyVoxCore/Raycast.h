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

#ifndef __PolyVox_Raycast_H__
#define __PolyVox_Raycast_H__

#include "PolyVoxCore/Vector.h"

namespace PolyVox
{
	namespace RaycastResults
	{
		/**
		 * The results of a raycast
		 */
		enum RaycastResult
		{
			Completed, ///< If the ray passed through the volume without being interupted
			Interupted ///< If the ray was interupted while travelling
		};
	}
	typedef RaycastResults::RaycastResult RaycastResult;

	/// OUT OF DATE SINCE UNCLASSING
	////////////////////////////////////////////////////////////////////////////////
	/// \file Raycast.h
	///
	/// The principle behind raycasting is to fire a 'ray' through the volume and determine
	/// what (if anything) that ray hits. This simple test can be used for the purpose of
	/// picking, visibility checks, lighting calculations, or numerous other applications.
	///
	/// A ray is a stright line in space define by a start point and a direction vector.
	/// The length of the direction vector represents the length of the ray. When you call a
	/// Raycast object's execute() method it will iterate over each voxel which lies on the ray,
	/// starting from the defined start point. It will examine each voxel and terminate
	/// either when it encounters a solid voxel or when it reaches the end of the ray. If a
	/// solid voxel is encountered then its position is stored in the intersectionVoxel field
	/// of the RaycastResult structure and the intersectionFound flag is set to true, otherwise
	/// the intersectionFound flag is set to false.
	///
	/// **Important Note:** These has been confusion in the past with people not realising
	/// that the length of the direction vector is important. Most graphics API can provide
	/// a camera position and view direction for picking purposes, but the view direction is
	/// usually normalised (i.e. of length one). If you use this view direction directly you
	/// will only iterate over a single voxel and won't find what you are looking for. Instead
	/// you must scale the direction vector so that it's length represents the maximum distance
	/// over which you want the ray to be cast.
	///
	/// The following code snippet shows how the class is used:
	/// \code
	/// Vector3DFloat start(rayOrigin.x(), rayOrigin.y(), rayOrigin.z());
	/// Vector3DFloat direction(rayDir.x(), rayDir.y(), rayDir.z());
	/// direction.normalise();
	/// direction *= 1000.0f; //Casts ray of length 1000
	/// 
	/// RaycastResult raycastResult;
	/// Raycast<Material8> raycast(m_pPolyVoxVolume, start, direction, raycastResult);
	/// raycast.execute();
	/// 
	/// if(raycastResult.foundIntersection)
	/// {
	/// 	//...
	/// }
	/// \endcode
	///
	/// Some further notes, the Raycast uses full 26-connectivity, which basically means it 
	/// will examine every voxel the ray touches, even if it just passes through the corner.
	/// Also, it peforms a simple binary test against a voxel's threshold, rather than making
	/// use of it's density. Therefore it will work best in conjunction with one of the 'cubic'
	/// surace extractors. It's behaviour with the Marching Cubes surface extractor has not
	/// been tested yet.
	////////////////////////////////////////////////////////////////////////////////

	template<typename VolumeType, typename Callback>
	RaycastResult raycastWithEndpoints(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dEnd, Callback& callback);

	template<typename VolumeType, typename Callback>
	RaycastResult raycastWithDirection(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, Callback& callback);
}

#include "PolyVoxCore/Raycast.inl"

#endif //__PolyVox_Raycast_H__
