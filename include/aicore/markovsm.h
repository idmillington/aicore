/*
 * Defines the classes used for markov state machines.
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
 * Holds a state machine implementation that updates a series of priority
 * or numerical values based on a Markov process.
 */
#ifndef AICORE_MARKOVSM_H
#define AICORE_MARKOVSM_H

namespace aicore
{
    /**
     * Markov transitions consist of a matrix that is multiplied by
     * the current state vector. They can also contain actions like a
     * regular transition.
     */
    class MarkovTransition : public BaseTransition
    {
    public:
        /**
         * Returns the matrix associated with the transition. This
         * will be multiplied by the state vector to get the new state
         * vector.
         *
         * @return The matrix as a linear array in row-major order
         * (i.e. its number of elements is the square of the size of
         * the state vector). The matrix will be pre-multiplied with
         * the state vector.
         */
        virtual real* getMatrix()=0;
    };

    /**
     * A markov transition with a fixed and explicit matrix.
     */
    class FixedMarkovTransition : public MarkovTransition
    {
    public:
        /**
         * The matrix associated with this transition.
         */
        real* matrix;

        /**
         * Returns the matrix defined in this instance.
         *
         * @return The matrix as a linear array in row-major order
         * (i.e. its number of elements is the square of the size of
         * the state vector. The matrix will be pre-multiplied with
         * the state vector.
         */
        virtual real* getMatrix();
    };

    /**
     * The markov state machine is responsible for keeping track of
     * the current array of states and modifying them under influence
     * of transitions.
     */
    class MarkovStateMachine
    {
        /**
         * This helper function does the state vector update.
         */
        void updateStateVector(real * matrix);

    public:
        /**
         * Holds the values in the state vector. Note that there is no
         * separate set of initial values. Unlike a regular state
         * machine, it doesn't make sense for a markov state machine
         * to be in an indefinite initial state. It needs to be set up
         * with starting numbers.
         */
        real *stateVector;

        /**
         * Holds the number of values in the state vector.
         */
        unsigned stateVectorSize;

        /**
         * Unlike other state machines the transitions are a feature
         * of the machine itself.
         */
        MarkovTransition *firstTransition;

        /**
         * Holds the transition that will fire after a while if no
         * other transitions fire.
         */
        MarkovTransition *defaultTransition;

        /**
         * Holds the number of frames of inactivity before the default
         * transition fires.
         */
        unsigned framesToDefault;

        /**
         * Keeps track of the number of frames since a transition fired.
         */
        unsigned framesPassed;

        /**
         * This method runs the state machine - it checks for
         * transitions, applies them and returns a list of actions.
         */
        virtual Action * update();
    };


}; // end of namespace

#endif // AICORE_MARKOVSM_H
