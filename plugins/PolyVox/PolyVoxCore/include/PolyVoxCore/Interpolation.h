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

#ifndef __PolyVox_Interpolation_H__
#define __PolyVox_Interpolation_H__

#include <cassert>

namespace PolyVox
{
	template <typename Type>
	Type lerp(
        const Type& v0,const Type& v1,
        const float x)
    {
        assert((x >= 0.0f) && (x <= 1.0f));

		//Interpolate along X
		Type v0_1 = v0 + x * (v1 - v0);

		return v0_1;
    }

	template <typename Type>
	Type bilerp(
        const Type& v00,const Type& v10,const Type& v01,const Type& v11,
        const float x, const float y)
    {
        assert((x >= 0.0f) && (y >= 0.0f) && 
            (x <= 1.0f) && (y <= 1.0f));

		// Linearly interpolate along x
		Type v00_10 = lerp(v00, v10, x);
		Type v01_11 = lerp(v01, v11, x);

		// And linearly interpolate the results along y
		Type v00_10__v01_11 = lerp(v00_10, v01_11, y);

		return v00_10__v01_11;
    }

	template <typename Type>
	Type trilerp(
        const Type& v000,const Type& v100,const Type& v010,const Type& v110,
        const Type& v001,const Type& v101,const Type& v011,const Type& v111,
        const float x, const float y, const float z)
    {
        assert((x >= 0.0f) && (y >= 0.0f) && (z >= 0.0f) && 
            (x <= 1.0f) && (y <= 1.0f) && (z <= 1.0f));

		// Bilinearly interpolate along Y
		Type v000_v100__v010_v110 = bilerp(v000, v100, v010, v110, x, y);
		Type v001_v101__v011_v111 = bilerp(v001, v101, v011, v111, x, y);

		// And linearly interpolate the results along z
		Type v000_v100__v010_v110____v001_v101__v011_v111 = lerp(v000_v100__v010_v110, v001_v101__v011_v111, z);
			
		return v000_v100__v010_v110____v001_v101__v011_v111;
    }
}

#endif //__PolyVox_Interpolation_H__
