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

#ifndef __PolyVox_Material_H__
#define __PolyVox_Material_H__

#include "Impl/TypeDef.h"

#include "PolyVoxCore/DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type

#include <cassert>

namespace PolyVox
{
	///This class represents a voxel storing only a material.
	////////////////////////////////////////////////////////////////////////////////
	/// Detailed description...
	///
	/// \sa Density, MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////

	// int32_t template parameter is a dummy, required as the compiler expects to be able to declare an
	// instance of VoxelType::DensityType without knowing that VoxelType doesn't actually have a density.
	template <typename Type>
	class Material
	{
	public:
		Material() : m_uMaterial(0) {}
		Material(Type uMaterial) : m_uMaterial(uMaterial) {}

		bool operator==(const Material& rhs) const
		{
			return (m_uMaterial == rhs.m_uMaterial);
		};

		bool operator!=(const Material& rhs) const
		{
			return !(*this == rhs);
		}

		/// \return The current material value of the voxel
		Type getMaterial() const { return m_uMaterial; }
		/**
		 * Set the material value of the voxel
		 * 
		 * \param uMaterial The material to set to
		 */
		void setMaterial(Type uMaterial) { m_uMaterial = uMaterial; }

	private:
		Type m_uMaterial;
	};

	typedef Material<uint8_t> Material8;
	typedef Material<uint16_t> Material16;

	template<typename Type>
	class DefaultIsQuadNeeded< Material<Type> >
	{
	public:
		bool operator()(Material<Type> back, Material<Type> front, uint32_t& materialToUse)
		{
			if((back.getMaterial() > 0) && (front.getMaterial() == 0))
			{
				materialToUse = static_cast<uint32_t>(back.getMaterial());
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}

#endif //__PolyVox_Material_H__
