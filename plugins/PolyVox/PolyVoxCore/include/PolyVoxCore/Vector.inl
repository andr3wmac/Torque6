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
    //-------------------------- Constructors, etc ---------------------------------
    /**
    Creates a Vector object and initialises it with given values.
    \param x x component to set.
    \param y y component to set.
    */
    template <uint32_t Size,typename Type>
        Vector<Size,Type>::Vector(Type x, Type y)
    {
		m_tElements[0] = x;
		m_tElements[1] = y;

    }

	/**
	Creates a Vector3D object and initialises it with given values.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	*/
	template <uint32_t Size,typename Type>
		Vector<Size,Type>::Vector(Type x, Type y, Type z)
	{
		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;

	}

	/**
	Creates a Vector3D object and initialises it with given values.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	\param w w component to set.
	*/
	template <uint32_t Size,typename Type>
		Vector<Size,Type>::Vector(Type x, Type y, Type z, Type w)
	{
		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
		m_tElements[3] = w;
	}

	/**
	Creates a Vector object but does not initialise it.
	*/
	template <uint32_t Size, typename Type>
		Vector<Size, Type>::Vector(void)
	{
	}

    /**
    Copy constructor builds object based on object passed as parameter.
    \param vector A reference to the Vector to be copied.
    */
    template <uint32_t Size, typename Type>
        Vector<Size, Type>::Vector(const Vector<Size, Type>& vector)
    {
		std::memcpy(m_tElements, vector.m_tElements, sizeof(Type) * Size);
    }

	/**
	This copy constructor allows casting between vectors with different data types.
	It is now possible to use code such as:
	
	Vector3DDouble v3dDouble(1.0,2.0,3.0);
	Vector3DFloat v3dFloat = static_cast<Vector3DFloat>(v3dDouble); //Casting

	\param vector A reference to the Vector to be copied.
	*/
	template <uint32_t Size, typename Type>
		template <typename CastType>
		Vector<Size, Type>::Vector(const Vector<Size, CastType>& vector)
	{
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] = static_cast<Type>(vector.getElement(ct));
		}
	}

    /**
    Destroys the Vector.
    */
    template <uint32_t Size, typename Type>
        Vector<Size, Type>::~Vector(void)
    {
    }

    /**
    Assignment operator copies each element of first Vector to the second.
    \param rhs Vector to assign to.
    \return A reference to the result to allow chaining.
    */
    template <uint32_t Size, typename Type>
        Vector<Size, Type>& Vector<Size, Type>::operator=(const Vector<Size, Type>& rhs)
    {
        if(this == &rhs)
		{
			return *this;
		}
        std::memcpy(m_tElements, rhs.m_tElements, sizeof(Type) * Size);
        return *this;
    }

    /**
    Checks whether two Vectors are equal.
    \param rhs The Vector to compare to.
    \return true if the Vectors match.
    \see operator!=
    */
    template <uint32_t Size, typename Type>
        inline bool Vector<Size, Type>::operator==(const Vector<Size, Type> &rhs) const
    {
		bool equal = true;
        for(uint32_t ct = 0; ct < Size; ++ct)
		{
			if(m_tElements[ct] != rhs.m_tElements[ct])
			{
				equal = false;
				break;
			}
		}
		return equal;
    }

	/**
    Checks whether two Vectors are not equal.
    \param rhs The Vector to compare to.
    \return true if the Vectors do not match.
    \see operator==
    */
    template <uint32_t Size, typename Type>
        inline bool Vector<Size, Type>::operator!=(const Vector<Size, Type> &rhs) const
    {
		return !(*this == rhs); //Just call equality operator and invert the result.
    }

	/**
    Checks whether this vector is less than the parameter. The metric is
	meaningless but it allows Vectors to me used as key in sdt::map, etc.
    \param rhs The Vector to compare to.
    \return true if this is less than the parameter
    \see operator!=
    */
    template <uint32_t Size, typename Type>
        inline bool Vector<Size, Type>::operator<(const Vector<Size, Type> &rhs) const
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			if (m_tElements[ct] < rhs.m_tElements[ct])
				return true;
			if (rhs.m_tElements[ct] < m_tElements[ct])
				return false;
		}
		return false;
    }    

    /**
    Addition operator adds corresponding elements of the two Vectors.
    \param rhs Vector to add
    \return The resulting Vector.
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type>& Vector<Size, Type>::operator+=(const Vector<Size, Type>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] += rhs.m_tElements[ct];
		}
        return *this;
    }

	/**
    Subtraction operator subtracts corresponding elements of one Vector from the other.
    \param rhs Vector to subtract
    \return The resulting Vector.
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type>& Vector<Size, Type>::operator-=(const Vector<Size, Type>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] -= rhs.m_tElements[ct];
		}
        return *this;
    }

	/**
    Multiplication operator multiplies corresponding elements of the two Vectors.
    \param rhs Vector to multiply by
    \return The resulting Vector.
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type>& Vector<Size, Type>::operator*=(const Vector<Size, Type>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] *= rhs.m_tElements[ct];
		}
        return *this;
    }

	/**
    Division operator divides corresponding elements of one Vector by the other.
    \param rhs Vector to divide by
    \return The resulting Vector.
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type>& Vector<Size, Type>::operator/=(const Vector<Size, Type>& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= rhs.m_tElements[ct];
		}
        return *this;
    }

    /**
    Multiplication operator multiplies each element of the Vector by a number.
    \param rhs the number the Vector is multiplied by.
    \return The resulting Vector.
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type>& Vector<Size, Type>::operator*=(const Type& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] *= rhs;
		}
        return *this;
    }

    /**
	Division operator divides each element of the Vector by a number.
	\param rhs the number the Vector is divided by.
	\return The resulting Vector.
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type>& Vector<Size, Type>::operator/=(const Type& rhs)
    {
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= rhs;
		}
        return *this;
    }

	/**
    Addition operator adds corresponding elements of the two Vectors.
	\param lhs Vector to add to.
    \param rhs Vector to add.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename Type>
	    Vector<Size,Type> operator+(const Vector<Size,Type>& lhs, const Vector<Size,Type>& rhs)
	{
		Vector<Size,Type> result = lhs;
		result += rhs;
		return result;
	}

	/**
    Subtraction operator subtracts corresponding elements of one Vector from the other.
	\param lhs Vector to subtract from.
    \param rhs Vector to subtract.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename Type>
	    Vector<Size,Type> operator-(const Vector<Size,Type>& lhs, const Vector<Size,Type>& rhs)
	{
		Vector<Size,Type> result = lhs;
		result -= rhs;
		return result;
	}

	/**
    Multiplication operator mulitplies corresponding elements of the two Vectors.
	\param lhs Vector to multiply.
    \param rhs Vector to multiply by.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename Type>
	    Vector<Size,Type> operator*(const Vector<Size,Type>& lhs, const Vector<Size,Type>& rhs)
	{
		Vector<Size,Type> result = lhs;
		result *= rhs;
		return result;
	}

	/**
    Division operator divides corresponding elements of one Vector by the other.
	\param lhs Vector to divide.
    \param rhs Vector to divide by.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename Type>
	    Vector<Size,Type> operator/(const Vector<Size,Type>& lhs, const Vector<Size,Type>& rhs)
	{
		Vector<Size,Type> result = lhs;
		result /= rhs;
		return result;
	}

	/**
    Multiplication operator multiplies each element of the Vector by a number.
	\param lhs the Vector to multiply.
    \param rhs the number the Vector is multiplied by.
    \return The resulting Vector.
    */
	template <uint32_t Size,typename Type>
	    Vector<Size,Type> operator*(const Vector<Size,Type>& lhs, const Type& rhs)
	{
		Vector<Size,Type> result = lhs;
		result *= rhs;
		return result;
	}

	/**
	Division operator divides each element of the Vector by a number.
	\param lhs the Vector to divide.
	\param rhs the number the Vector is divided by.
	\return The resulting Vector.
    */
	template <uint32_t Size,typename Type>
	    Vector<Size,Type> operator/(const Vector<Size,Type>& lhs, const Type& rhs)
	{
		Vector<Size,Type> result = lhs;
		result /= rhs;
		return result;
	}

    /**
    Enables the Vector to be used intuitively with output streams such as cout.
    \param os The output stream to write to.
    \param vector The Vector to write to the stream.
    \return A reference to the output stream to allow chaining.
    */
    template <uint32_t Size, typename Type>
        std::ostream& operator<<(std::ostream& os, const Vector<Size, Type>& vector)
    {
        os << "(";
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			os << vector.getElement(ct);
			if(ct < (Size-1))
			{
				os << ",";
			}
		}
		os << ")";
        return os;
    }		

	/**
	Returns the element at the given position.
	\param index The index of the element to return.
	\return The element.
	*/
	template <uint32_t Size, typename Type>
		inline Type Vector<Size, Type>::getElement(uint32_t index) const
	{
		return m_tElements[index];
	}

    /**
    \return A const reference to the X component of a 1, 2, 3, or 4 dimensional Vector.
    */
    template <uint32_t Size, typename Type>
        inline Type Vector<Size, Type>::getX(void) const
    {
        return m_tElements[0];
    }	

	/**
	\return A const reference to the Y component of a 2, 3, or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename Type>
        inline Type Vector<Size, Type>::getY(void) const
    {
        return m_tElements[1];
    }	

	/**
	\return A const reference to the Z component of a 3 or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename Type>
        inline Type Vector<Size, Type>::getZ(void) const
    {
        return m_tElements[2];
    }	

	/**
	\return A const reference to the W component of a 4 dimensional Vector.
	*/
	template <uint32_t Size, typename Type>
		inline Type Vector<Size, Type>::getW(void) const
	{
		return m_tElements[3];
	}  

	/**
	\param index The index of the element to set.
	\param tValue The new value for the element.
	*/
	template <uint32_t Size, typename Type>
		inline void Vector<Size, Type>::setElement(uint32_t index, Type tValue)
	{
		m_tElements[index] = tValue;
	}

	/**
    Sets several elements of a vector at once.
    \param x x component to set.
    \param y y component to set.
    */
    template <uint32_t Size,typename Type>
        inline void Vector<Size,Type>::setElements(Type x, Type y)
    {
		m_tElements[0] = x;
		m_tElements[1] = y;

    }

	/**
	Sets several elements of a vector at once.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	*/
	template <uint32_t Size,typename Type>
		inline void Vector<Size,Type>::setElements(Type x, Type y, Type z)
	{
		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;

	}

	/**
	Sets several elements of a vector at once.
	\param x x component to set.
	\param y y component to set.
	\param z z component to set.
	\param w w component to set.
	*/
	template <uint32_t Size,typename Type>
		inline void Vector<Size,Type>::setElements(Type x, Type y, Type z, Type w)
	{
		m_tElements[0] = x;
		m_tElements[1] = y;
		m_tElements[2] = z;
		m_tElements[3] = w;
	}

	/**
	\param tX The new value for the X component of a 1, 2, 3, or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename Type>
        inline void Vector<Size, Type>::setX(Type tX)
    {
        m_tElements[0] = tX;
    }

	/**
	\param tY The new value for the Y component of a 2, 3, or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename Type>
        inline void Vector<Size, Type>::setY(Type tY)
    {
        m_tElements[1] = tY;
    }

	/**
	\param tZ The new value for the Z component of a 3 or 4 dimensional Vector.
	*/
    template <uint32_t Size, typename Type>
        inline void Vector<Size, Type>::setZ(Type tZ)
    {
        m_tElements[2] = tZ;
    }

	/**
	\param tW The new value for the W component of a 4 dimensional Vector.
	*/
	template <uint32_t Size, typename Type>
        inline void Vector<Size, Type>::setW(Type tW)
    {
        m_tElements[3] = tW;
    }

	/**
	\note This function does not make much sense on integer Vectors.
    \return Length of the Vector.
    */
    template <uint32_t Size, typename Type>
        inline double Vector<Size, Type>::length(void) const
    {
        return sqrt(lengthSquared());
    }

    /**
    \return Squared length of the Vector.
    */
    template <uint32_t Size, typename Type>
        inline double Vector<Size, Type>::lengthSquared(void) const
    {
		double result = 0.0f;
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			result += m_tElements[ct] * m_tElements[ct];
		}
		return result;
    }

    /**
    This function is commutative, such that a.angleTo(b) == b.angleTo(a). The angle
    returned is in radians and varies between 0 and 3.14(pi). It is always positive.

	\note This function does not make much sense on integer Vectors.

    \param vector The Vector to find the angle to.
    \return The angle between them in radians.
    */
    template <uint32_t Size, typename Type>
        inline double Vector<Size, Type>::angleTo(const Vector<Size, Type>& vector) const
    {
        return acos(dot(vector) / (vector.length() * this->length()));
    }

    /**
    This function is used to calculate the cross product of two Vectors.
    The cross product is the Vector which is perpendicular to the two
    given Vectors. It is worth remembering that, unlike the dot product,
    it is not commutative. E.g a.b != b.a. The cross product obeys the 
	right-hand rule such that if the two vectors are given by the index
	finger and middle finger respectively then the cross product is given
	by the thumb.
    \param vector The vector to cross with this
    \return The value of the cross product.
    \see dot()
    */
    template <uint32_t Size, typename Type>
        inline Vector<Size, Type> Vector<Size, Type>::cross(const Vector<Size, Type>& vector) const
    {
        Type i = vector.getZ() * this->getY() - vector.getY() * this->getZ();
        Type j = vector.getX() * this->getZ() - vector.getZ() * this->getX();
        Type k = vector.getY() * this->getX() - vector.getX() * this->getY();
        return Vector<Size, Type>(i,j,k);
    }

    /**
    Calculates the dot product of the Vector and the parameter.
    This function is commutative, such that a.dot(b) == b.dot(a).
    \param rhs The Vector to find the dot product with.
    \return The value of the dot product.
    \see cross()
    */
    template <uint32_t Size, typename Type>
    inline Type Vector<Size, Type>::dot(const Vector<Size, Type>& rhs) const
    {
        Type dotProduct = static_cast<Type>(0);
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			dotProduct += m_tElements[ct] * rhs.m_tElements[ct];
		}
		return dotProduct;
    }

    /**
    Divides the i, j, and k components by the length to give a Vector of length 1.0.

	\note This function does not make much sense on integer Vectors.
    */
    template <uint32_t Size, typename Type>
        inline void Vector<Size, Type>::normalise(void)
    {
        Type tLength = static_cast<Type>(this->length());
		//FIXME - throw div by zero exception?
		if(tLength < 0.0001f)
		{
			return;
		}
		for(uint32_t ct = 0; ct < Size; ++ct)
		{
			m_tElements[ct] /= tLength;
		}
    }
}//namespace PolyVox
