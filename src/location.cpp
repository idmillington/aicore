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

#include <aicore/aicore.h>

namespace aicore
{
    /*
     * This is messy, but it saves a function call or code duplication.
     */
    #define SIMPLE_INTEGRATION(duration, velocity, rotation) \
        position.x += (velocity).x*duration; \
        position.y += (velocity).y*duration; \
        position.z += (velocity).z*duration; \
        orientation += (rotation)*duration; \
        orientation = real_mod_real(orientation, M_2PI);


    /*
     * Uses SIMPLE_INTEGRATION(duration), defined above.
     */
    void Location::integrate(const SteeringOutput& steer, real duration)
    {
        SIMPLE_INTEGRATION(duration, steer.linear, steer.angular);
    }

    void Location::setOrientationFromVelocity(const Vector3& velocity)
    {
        // If we haven't got any velocity, then we can do nothing.
        if (velocity.squareMagnitude() > 0) {
            orientation = real_atan2(velocity.x, velocity.z);
        }
    }

    Vector3 Location::getOrientationAsVector() const
    {
        return Vector3(real_sin(orientation),
                       0,
                       real_cos(orientation));
    }

    /*
     * Uses SIMPLE_INTEGRATION(duration), defined above.
     */
    void Kinematic::integrate(real duration)
    {
        SIMPLE_INTEGRATION(duration, velocity, rotation);
    }

    /*
     * Uses SIMPLE_INTEGRATION(duration), defined above.
     */
    void Kinematic::integrate(const SteeringOutput& steer,
                              real duration)
    {
        SIMPLE_INTEGRATION(duration, velocity, rotation);
        velocity.x += steer.linear.x*duration;
        velocity.y += steer.linear.y*duration;
        velocity.z += steer.linear.z*duration;
        rotation += steer.angular*duration;
    }

    /*
     * Uses SIMPLE_INTEGRATION(duration), defined above.
     */
    void Kinematic::integrate(const SteeringOutput& steer,
                              real drag,
                              real duration)
    {
        SIMPLE_INTEGRATION(duration, velocity, rotation);

        // Slowing velocity and rotational velocity
        drag = real_pow(drag, duration);
        velocity *= drag;
        rotation *= drag*drag;

        velocity.x += steer.linear.x*duration;
        velocity.y += steer.linear.y*duration;
        velocity.z += steer.linear.z*duration;
        rotation += steer.angular*duration;
    }

    /*
     * Uses SIMPLE_INTEGRATION(duration), defined above.
     */
    void Kinematic::integrate(const SteeringOutput& steer,
                              const SteeringOutput& drag,
                              real duration)
    {
        SIMPLE_INTEGRATION(duration, velocity, rotation);

        velocity.x *= real_pow(drag.linear.x, duration);
        velocity.y *= real_pow(drag.linear.y, duration);
        velocity.z *= real_pow(drag.linear.z, duration);
        rotation *= real_pow(drag.angular, duration);

        velocity.x += steer.linear.x*duration;
        velocity.y += steer.linear.y*duration;
        velocity.z += steer.linear.z*duration;
        rotation += steer.angular*duration;
    }



    /* Add and divide used in finding Kinematic means. */
    void Kinematic::operator += (const Kinematic& other)
    {
        position+=other.position;
        velocity+=other.velocity;
        rotation+=other.rotation;
        orientation+=other.orientation;
    }

    void Kinematic::operator -= (const Kinematic& other)
    {
        position-=other.position;
        velocity-=other.velocity;
        rotation-=other.rotation;
        orientation-=other.orientation;
    }

    void Kinematic::operator *= (real f)
    {
        position*=f;
        velocity*=f;
        rotation*=f;
        orientation*=f;
    }

    void Kinematic::trimMaxSpeed(real maxSpeed)
    {
        if (velocity.squareMagnitude() > maxSpeed*maxSpeed) {
            velocity.normalise();
            velocity *= maxSpeed;
        }
    }

    void Kinematic::setOrientationFromVelocity()
    {
        // If we haven't got any velocity, then we can do nothing.
        if (velocity.squareMagnitude() > 0) {
            orientation = real_atan2(velocity.x, velocity.z);
        }
    }

}; // end of namespace
