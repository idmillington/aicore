/*
 * Defines the state classes used for steering.
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
 * Holds a set of structures that represent the state of a character
 * moving in the game. The state can be as simple as a position, or as
 * complex as a linear and angular physical state. The classes in this
 * file are all 2.5-dimensional: they have three dimensional positions
 * but two dimensional orientations. This file also includes a class
 * to represent what a movement algorithm wants to do with a
 * character.
 */
#ifndef AICORE_LOCATION_H
#define AICORE_LOCATION_H

namespace aicore
{
    /**
     * SteeringOutput is a movement requested by the steering system.
     *
     * It consists of linear and angular components. The components
     * may be interpreted as forces and torques when output from a
     * full dynamic steering behaviour, or as velocity and rotation
     * when output from a kinematic steering behaviour. In the former
     * case, neither force nor torque take account of mass, and so
     * should be thought of as linear and angular acceleration.
     */
    struct SteeringOutput
    {
        /**
         * The linear component of the steering action.
         */
        Vector3 linear;

        /**
         * The angular component of the steering action.
         */
        real angular;

        /**
         * Creates a new steering action with zero linear and angular
         * changes.
         */
        SteeringOutput() : angular(0)
        {}

        /**
         * Creates a new steering action with the given linear and
         * angular changes.
         *
         * @param linear The initial linear change to give the
         * SteeringOutput.
         *
         * @param angular The initial angular change to give the
         * SteeringOutput.
         */
        SteeringOutput(const Vector3& linear, real angular = 0)
            : linear(linear), angular(angular)
        {}

        /**
         * Zeros the linear and angular changes of this steering action.
         */
        void clear()
        {
            linear.clear();
            angular = 0;
        }

        /**
         * Checks that the given steering action is equal to this.
         * SteeringOutputs are equal if their linear and angular
         * changes are equal.
         */
        bool operator == (const SteeringOutput& other) const
        {
            return linear == other.linear &&  angular == other.angular;
        }

        /**
         * Checks that the given steering action is unequal to this.
         * SteeringOutputs are unequal if either their linear or
         * angular changes are unequal.
         */
        bool operator != (const SteeringOutput& other) const
        {
            return linear != other.linear ||  angular != other.angular;
        }

		/**
		 * Returns the square of the magnitude of this steering output.
		 * This includes the angular component.
		 */
		real squareMagnitude()
		{
			return linear.squareMagnitude() + angular*angular;
		}

		/**
		* Returns the magnitude of this steering output.
		* This includes the angular component.
		 */
		real magnitude()
		{
			return real_sqrt(squareMagnitude());
		}
    };


    /**
     * Represents the location of a character or other object.
     *
     * A location consists of a position and orientation. Position is
     * stored as a vector, rotation is a planar rotation about the y
     * axis. This will be altered to be a quaternion in due course.
     */
    struct Location
    {
        /**
         * The position in 3 space.
         */
        Vector3 position;

        /**
         * The orientation, as a euler angle in radians around the
         * positive y axis (i.e. up) from the positive z axis.
         */
        real orientation;

        /**
         * Creates a new location with a 0 position and orientation.
         */
        Location() : orientation(0.0f)
        {}

        /**
         * Creates a location at the given position with no rotation.
         */
        Location(const Vector3& position)
            : position(position), orientation(0.0f)
        {}

        /**
         * Creates a location with the given position and orientation.
         */
        Location(const Vector3& position, real orientation)
            : position(position), orientation(orientation)
        {}

        /**
         * Creates a location with the position vector given as
         * components and the given orientation.
         */
        Location(real x, real y, real z, real orientation)
            : position(x, y, z), orientation(orientation)
        {}

        /**
         * Assignment operator.
         */
        Location& operator = (const Location& other)
        {
            position = other.position;
            orientation = other.orientation;
            return *this;
        }

        /**
         * Zeros the position and orientation.
         */
        void clear()
        {
            position.clear();
            orientation = 0.0f;
        }

        /**
         * Checks that the given location is equal to this. Locations
         * are equal if their positions and orientations are equal.
         */
        bool operator == (const Location& other) const
        {
            return position == other.position &&
                orientation == other.orientation;
        }

        /**
         * Checks that the given location is unequal to
         * this. Locations are unequal if either their positions or
         * orientations are unequal.
         */
        bool operator != (const Location& other) const
        {
            return position != other.position ||
                orientation != other.orientation;
        }

        /**
         * Perfoms a forward Euler integration of the Kinematic for
         * the given duration, applying the given steering velocity
         * and rotation.
         *
         * \note All of the integrate methods in this class are designed
         * to provide a simple mechanism for updating position. They are
         * not a substitute for a full physics engine that can correctly
         * resolve collisions and constraints.
         *
         * @param steer The velocity to apply over the integration.
         *
         * @param duration The number of simulation seconds to
         * integrate over.
         */
        void integrate(const SteeringOutput& steer, real duration);

        /**
         * Sets the orientation of this location so it points along
         * the given velocity vector.
         */
        void setOrientationFromVelocity(const Vector3& velocity);

        /**
         * Returns a unit vector in the direction of the current
         * orientation.
         */
        Vector3 getOrientationAsVector() const;

        /**
         * Fills the passed matrix with the Location's transformation.
        void retrieveTM(Matrix& m);
         */
    };

    /**
     * Represents the position and movement of a character or other object.
     *
     * Kinematic extends location to add the first derivative of both
     * position and orientation: velocity and rotation. Rotation
     * consists of angular velocity about the positive z axis (the
     * first derivative of orientation in the Location structure), this
     * will be altered to be a full 3D angular velocity in due course.
     */
    struct Kinematic : public Location
    {
        /**
         * The linear velocity.
         */
        Vector3 velocity;

        /**
         * The angular velocity.
         */
        real rotation;

        /**
         * Creates a new Kinematic with zeroed data.
         */
        Kinematic()
            : Location(), velocity(), rotation(0)
        {}

        /**
         * Creates a new Kinematic with the given linear components
         * and no rotation or orientation.
         *
         * @param position The position in space of the Kinematic.
         * @param velocity The linear velocity of the Kinematic.
         */
        Kinematic(const Vector3& position, const Vector3& velocity)
            : Location(position), velocity(velocity), rotation(0)
        {}

        /**
         * Creates a new Kinematic with the given location and linear
         * velocity. The Kinematic has no rotation.
         *
         * @param loc The location of the Kinematic.
         * @param velocity The linear velocity of the Kinematic.
         */
        Kinematic(const Location& loc, const Vector3& velocity)
            : Location(loc), velocity(velocity), rotation(0)
        {}

        /**
         * Creates a new Kinematic with the given location and no
         * linear or rotational velocity.
         *
         * @param loc The location of the Kinematic.
         */
        Kinematic(const Location& loc)
            : Location(loc), velocity(), rotation(0)
        {}

        /**
         * Creates a new Kinematic with the given position,
         * orientation, velocity and rotational velocity.
         *
         * @param position The position of the Kinematic.
         * @param orientation The orientation of the Kinematic.
         * @param velocity The linear velocity of the Kinematic.
         * @param avel The angular velocity of the Kinematic.
         */
        Kinematic(const Vector3& position, real orientation,
                  const Vector3& velocity, real avel)
            : Location(position, orientation),
              velocity(velocity), rotation(avel)
        {}

        /**
         * Zeros the location and velocity of this Kinematic.
         */
        void clear()
        {
            Location::clear();
            velocity.clear();
            rotation = 0.0f;
        }

        /**
         * Checks that the given Kinematic is equal to this.
         * Kinematics are equal if their locations, velocities and
         * rotations are equal.
         */
        bool operator == (const Kinematic& other) const
        {
            return position == other.position &&
                   orientation == other.orientation &&
                   velocity == other.velocity &&
                   rotation == other.rotation;
        }

        /**
         * Checks that the given Kinematic is unequal to this.
         * Kinematics are unequal if any of their locations,
         * velocities or rotations are unequal.
         */
        bool operator != (const Kinematic& other) const
        {
            return position != other.position ||
                   orientation != other.orientation ||
                   velocity != other.velocity ||
                   rotation != other.rotation;
        }

        /**
         * Checks that the given Kinematic is less than this.  A
         * Kinematic is less than another Kinematic if its position
         * along the x-axis is less than that of the other Kinematic.
         */
        bool operator < (const Kinematic& other) const
        {
            return position.x < other.position.x;
        }

        /**
         * Sets the value of this Kinematic to the given location.
         * The velocity components of the Kinematic are left
         * unchanged.
         *
         * @param other The Location to set the Kinematic to.
         */
        Kinematic& operator = (const Location& other)
        {
            orientation = other.orientation;
            position = other.position;
            return *this;
        }

        /**
         * Copies (by assignment) all the attributes of the given
         * Kinematic.
         *
         * @param other Reference to Kinematic to copy.
         */
        Kinematic& operator = (const Kinematic& other)
        {
            orientation = other.orientation;
            position = other.position;
            velocity = other.velocity;
            rotation = other.rotation;
            return *this;
        }

        /**
         * Modifies the value of this Kinematic by adding the given
         * Kinematic.  Additions are performed by component.
         */
        void operator += (const Kinematic&);

        /**
         * Modifies the value of this Kinematic by subtracting the
         * given Kinematic.  Subtractions are performed by component.
         */
        void operator -= (const Kinematic&);

        /**
         * Scales the Kinematic by the given value.  All components
         * are scaled, including rotations and orientations, this is
         * normally not what you want. To scale only the linear
         * components, use the *= operator on the position and
         * velocity components individually.
         *
         * @param f The scaling factor.
         */
        void operator *= (real f);

        /**
         * Performs a forward Euler integration of the Kinematic for
         * the given duration.  This applies the Kinematic's velocity
         * and rotation to its position and orientation.
         *
         * \note All of the integrate methods in this class are
         * designed to provide a simple mechanism for updating
         * position. They are not a substitute for a full physics
         * engine that can correctly resolve collisions and
         * constraints.
         *
         * @param duration The number of simulation seconds to
         * integrate over.
         */
        void integrate(real duration);

        /**
         * Perfoms a forward Euler integration of the Kinematic for
         * the given duration, applying the given acceleration.
         * Because the integration is Euler, all the acceleration is
         * applied to the velocity at the end of the time step.
         *
         * \note All of the integrate methods in this class are designed
         * to provide a simple mechanism for updating position. They are
         * not a substitute for a full physics engine that can correctly
         * resolve collisions and constraints.
         *
         * @param steer The acceleration to apply over the
         * integration.  @param duration The number of simulation
         * seconds to integrate over.
         */
        void integrate(const SteeringOutput& steer, real duration);

        /**
         * Perfoms a forward Euler integration of the Kinematic for
         * the given duration, applying the given acceleration and
         * drag.  Because the integration is Euler, all the
         * acceleration is applied to the velocity at the end of the
         * time step.
         *
         * \note All of the integrate methods in this class are designed
         * to provide a simple mechanism for updating position. They are
         * not a substitute for a full physics engine that can correctly
         * resolve collisions and constraints.
         *
         * @param steer The acceleration to apply over the integration.
         *
         * @param drag The isotropic drag to apply to both velocity
         * and rotation. This should be a value between 0 (complete
         * drag) and 1 (no drag).
         *
         * @param duration The number of simulation seconds to
         * integrate over.
         */
        void integrate(const SteeringOutput& steer,
                       real drag, real duration);

        /**
         * Perfoms a forward Euler integration of the Kinematic for the given
         * duration, applying the given acceleration and drag.
         * Because the integration is Euler, all the acceleration is applied to
         * the velocity at the end of the time step.
         *
         * \note All of the integrate methods in this class are designed
         * to provide a simple mechanism for updating position. They are
         * not a substitute for a full physics engine that can correctly
         * resolve collisions and constraints.
         *
         * @param steer The acceleration to apply over the integration.
         *
         * @param drag The anisotropic drag to apply. The force
         * component of the SteeringOutput is interpreted as linear drag
         * coefficients in each direction, and torque component is
         * interpreted as the rotational drag.
         *
         * @param duration The number of simulation seconds to
         * integrate over.
         */
        void integrate(const SteeringOutput& steer,
                       const SteeringOutput& drag,
                       real duration);

        /**
         * Trims the speed of the kinematic to be no more than that
         * given.
         */
        void trimMaxSpeed(real speed);

        /**
         * Sets the orientation of this location so it points along
         * its own velocity vector.
         */
        void setOrientationFromVelocity();
    };


}; // end of namespace

#endif // AICORE_LOCATION_H
