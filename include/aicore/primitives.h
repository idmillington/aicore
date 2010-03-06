/*
 * The definitions for global routines and helpers.
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
 * Holds structures representing primitive objects in a game world. This is
 * mostly for the benefit of simple implementations of movement algorithms.
 */
#ifndef AICORE_PRIMITIVES_H
#define AICORE_PRIMITIVES_H

/**
 * Holds all classes and functions for AI-Core.
 *
 * There is a single namespace in which the whole library lives. This
 * allows you to import the aicore.h header without fear of naming
 * clashes, then either do:
 *
 * <code>using namespace aicore;</code>
 *
 * If you are convinced that there will be no clashes, or access elements of
 * the code using the <code>aicore::</code> prefix.
 */
namespace aicore
{
	/**
	 * A spherical primitive.
	 */
	struct Sphere
	{
		/**
		 * The position of the geometric centre of the sphere.
		 */
		Vector3 position;

		/**
		 * The radius of the sphere.
		 */
		real radius;
	};

}; // end of namespace

#endif // AICORE_CORE_H
