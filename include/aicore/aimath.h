/*
 * The mathematics definition file.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */

/**
 * @file
 *
 * Holds the basic mathematical structures for the AI library.
 *
 * AI doesn't normally put a high burden on mathematical and vector
 * geometry routines. This file contains an implementation of a three
 * dimensional vector suitable for basic spatial querying. You
 * probably already have a vector implementation, along with other
 * vector mathematics (such as quaternions and matrices), that can be
 * used in place of this. It makes sense to reconfigure the code to
 * use the same code throughout your game.
 */
#ifndef AICORE_AIMATH_H
#define AICORE_AIMATH_H

namespace aicore
{
    /**
     * Holds a vector in 3 dimensions. This class has a whole series
     * of methods used to manipulate the vector.
     *
     * Four real data members are allocated for the vector. When you
     * are working with single precision floating point values on a
     * PC, for example, having four members allocated allows the
     * Vector to site on a 16-byte alignment boundary, for improved
     * memory performance. If you are using other types of real value
     * (IEEE extended precision floats, for example), then it makes no
     * sense to have this pad value here, so it can be safely removed.
     *
     * @note This class contains a lot of inline methods for basic
     * mathematics. The implementations are included in the header
     * file so that an intelligent compiler will inline them into the
     * compiled code.
     */
    class Vector3
    {
    public:
         /** Holds the value along the x axis. */
        real x;

        /** Holds the value along the y axis. */
        real y;

        /** Holds the value along the z axis. */
        real z;

    private:
        /** Padding to ensure 4 word alignment. */
        real _pad;

    public:
        /** The default constructor creates a zero vector. */
        Vector3() : x(0), y(0), z(0) {}

        /**
         * @name Default Directions
         *
         * This is a series of convenience unit-vectors for each of
         * the six cardinal directions in a standard right-handed
         * coordinate system. This assumes, as in the vast majority of
         * games that the plane of the screen is in the X,Y plane,
         * with the Y axis pointing up and the X axis pointing right.
         */
        /* @{ */
        const static Vector3 UP;
        const static Vector3 RIGHT;
        const static Vector3 OUT_OF_SCREEN;
        const static Vector3 DOWN;
        const static Vector3 LEFT;
        const static Vector3 INTO_SCREEN;
        /* @} */

        /**
         * @name Basis Directions
         *
         * These are convenience unit-vectors in the basis directions.
         */
        /* @{ */
        const static Vector3 X;
        const static Vector3 Y;
        const static Vector3 Z;
        /* @} */

        /** A zero length vector. */
        const static Vector3 ZERO;


        /**
         * The explicit constructor creates a vector with the given
         * components.
         */
        Vector3(const real x, const real y, const real z)
            : x(x), y(y), z(z) {}

        /** Creates a vector between the given two points. */
        Vector3(const Vector3& from, const Vector3& to)
        {
            x = to.x - from.x;
            y = to.y - from.y;
            z = to.z - from.z;
        }

        /** Adds the given vector to this. */
        void operator+=(const Vector3& v)
        {
            x += v.x;
            y += v.y;
            z += v.z;
        }

        /**
         * Returns the value of the given vector added to this.
         */
        Vector3 operator+(const Vector3& v) const
        {
            return Vector3(x+v.x, y+v.y, z+v.z);
        }

        /** Subtracts the given vector from this. */
        void operator-=(const Vector3& v)
        {
            x -= v.x;
            y -= v.y;
            z -= v.z;
        }

        /**
         * Returns the value of the given vector subtracted from this.
         */
        Vector3 operator-(const Vector3& v) const
        {
            return Vector3(x-v.x, y-v.y, z-v.z);
        }

        /** Multiplies this vector by the given scalar. */
        void operator*=(const real value)
        {
            x *= value;
            y *= value;
            z *= value;
        }

        /** Returns a copy of this vector scaled the given value. */
        Vector3 operator*(const real value) const
        {
            return Vector3(x*value, y*value, z*value);
        }

        /** Checks if the two vectors have identical components. */
        bool operator==(const Vector3& other) const
        {
            return x == other.x &&
                y == other.y &&
                z == other.z;
        }

        /** Checks if the two vectors have non-identical components. */
        bool operator!=(const Vector3& other) const
        {
            return !(*this == other);
        }

        /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a < b) does not imply (b >= a).
         */
        bool operator<(const Vector3& other) const
        {
            return x < other.x && y < other.y && z < other.z;
        }

        /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a < b) does not imply (b >= a).
         */
        bool operator>(const Vector3& other) const
        {
            return x > other.x && y > other.y && z > other.z;
        }

        /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a <= b) does not imply (b > a).
         */
        bool operator<=(const Vector3& other) const
        {
            return x <= other.x && y <= other.y && z <= other.z;
        }

        /**
         * Checks if this vector is component-by-component less than
         * the other.
         *
         * @note This does not behave like a single-value comparison:
         * !(a <= b) does not imply (b > a).
         */
        bool operator>=(const Vector3& other) const
        {
            return x >= other.x && y >= other.y && z >= other.z;
        }

        /**
         * Calculates and returns a component-wise product of this
         * vector with the given vector.
         */
        Vector3 componentProduct(const Vector3 &vector) const
        {
            return Vector3(x * vector.x, y * vector.y, z * vector.z);
        }

        /**
         * Performs a component-wise product with the given vector and
         * sets this vector to its result.
         */
        void componentProductUpdate(const Vector3 &vector)
        {
            x *= vector.x;
            y *= vector.y;
            z *= vector.z;
        }

        /**
         * Calculates and returns the vector product of this vector
         * with the given vector.
         */
        Vector3 vectorProduct(const Vector3 &vector) const
        {
            return Vector3(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }

        /**
         * Updates this vector to be the vector product of its current
         * value and the given vector.
         */
        void operator %=(const Vector3 &vector)
        {
            *this = vectorProduct(vector);
        }

        /**
         * Calculates and returns the vector product of this vector
         * with the given vector.
         */
        Vector3 operator%(const Vector3 &vector) const
        {
            return Vector3(y*vector.z-z*vector.y,
                           z*vector.x-x*vector.z,
                           x*vector.y-y*vector.x);
        }

        /**
         * Calculates and returns the scalar product of this vector
         * with the given vector.
         */
        real scalarProduct(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }

        /**
         * Calculates and returns the scalar product of this vector
         * with the given vector.
         */
        real operator*(const Vector3 &vector) const
        {
            return x*vector.x + y*vector.y + z*vector.z;
        }

        /**
         * Adds the given vector to this, scaled by the given amount.
         */
        void addScaledVector(const Vector3& vector, real scale)
        {
            x += vector.x * scale;
            y += vector.y * scale;
            z += vector.z * scale;
        }

        /** Gets the magnitude of this vector. */
        real magnitude() const
        {
            return real_sqrt(x*x+y*y+z*z);
        }

        /** Gets the squared magnitude of this vector. */
        real squareMagnitude() const
        {
            return x*x+y*y+z*z;
        }

        /** Turns a non-zero vector into a vector of unit length. */
        void normalise()
        {
            real l = magnitude();
            if (l > 0)
            {
                (*this) *= ((real)1)/l;
            }
        }

		/** Returns a unit vector in the direction of this vector. */
		Vector3 unit() const
		{
			Vector3 result = *this;
			result.normalise();
			return result;
		}

        void setMagnitude(real magnitude)
        {
            normalise();
            (*this) *= magnitude;
        }

        /** Zero all the components of the vector. */
        void clear()
        {
            x = y = z = 0;
        }

        /** Flips all the components of the vector. */
        void invert()
        {
            x = -x;
            y = -y;
            x = -z;
        }


        /** Checks if the vector is zero */
        bool isZero() const
        {
            return x == 0 && y == 0 && z == 0;
        }

        /** Finds this distance from this point to that given. */
        real distance(const Vector3 &other) const
        {
            return Vector3(*this, other).magnitude();
        }
    };

}; // end of namespace

#endif // AICORE_AIMATH_H
