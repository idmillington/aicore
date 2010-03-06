/*
 * Defines the classes used for state machines.
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
 * Holds an implementation of a finite state machine.
 */
#ifndef AICORE_SM_H
#define AICORE_SM_H

namespace aicore
{
    // Forward declaration (defined below)
    class StateMachineState;

    /**
     * Transitions map between state machines.
     */
    class Transition : public BaseTransition
    {
    public:

        /**
         * The transition returns a target state to transition to.
         */
        virtual StateMachineState * getTargetState() = 0;
    };

    /**
     * A mixin intended for use with a BasTransition derived class. We
     * assume that the trasition always leads to the same state. So a
     * target state member is added.
     */
    class FixedTargetTransitionMixin
    {
    public:

        /**
         * Holds the state into which the transition should flow.
         */
        StateMachineState * target;

        /**
         * The transition returns a target state to transition to.
         */
        virtual StateMachineState * getTargetState();
    };

    /**
     * This represents one internal state a character be in: such as
     * angry, or searching-for-ammo.
     */
    class StateMachineState
    {
    public:
        /**
         * Returns the first in a sequence of actions that should be
         * performed while the character is in this state.
         *
         * Note that this method should return one or more newly
         * created action instances, and the caller of this method
         * should be responsible for the deletion. In the default
         * implementation, it returns nothing.
         */
        virtual Action * getActions();

        /**
         * Returns the sequence of actions to perform when arriving in
         * this state.
         *
         * Note that this method should return one or more newly
         * created action instances, and the caller of this method
         * should be responsible for the deletion. In the default
         * implementation, it returns nothing.
         */
        virtual Action * getEntryActions();

        /**
         * Returns the sequence of actions to perform when leaving
         * this state.
         *
         * Note that this method should return one or more newly
         * created action instances, and the caller of this method
         * should be responsible for the deletion. In the default
         * implementation, it returns nothing.
         */
        virtual Action * getExitActions();

        /**
         * The first in the sequence of transitions that can leave
         * this state.
         */
        Transition * firstTransition;
    };

    /**
     * Encapsulates a single layer state machine (i.e. one without
     * hierarchical transitions, see HierarchicalStateMachine for
     * that.
     */
    class StateMachine
    {
    public:
        /**
         * Holds the initial state (a pointer into the 'state' array).
         */
        StateMachineState *initialState;

        /**
         * Holds the current state of the machine.
         */
        StateMachineState *currentState;

        /**
         * This method runs the state machine - it checks for
         * transitions, applies them and returns a list of actions.
         */
        virtual Action * update();
    };


}; // end of namespace

#endif // AICORE_SM_H
