/*
 * Defines the base classes used for different state machines.
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
 * Holds the base classes for different state machine
 * implementations. State machines have similarities regardless of the
 * specifics of the techniques that they use. The classes in this file
 * are then extended and used by other types of state machines.
 */
#ifndef AICORE_BASESM_H
#define AICORE_BASESM_H

namespace aicore
{
    /**
     * The base transition is used for any kind of state machine. It
     * doesn't force a representation for the states or their
     * transitions, but does give values for the actions to be carried
     * out and the triggering.
     */
    class BaseTransition
    {
    public:
        /**
         * The transition needs to decide if it can be triggered or
         * not. This will depend on the sub-class of transition we're
         * dealing with.
         */
        virtual bool isTriggered() = 0;

        /**
         * The transition can also optionally return a list of actions
         * that need to be performed during the transition.
         *
         * Note that this method should return one or more newly
         * created action instances, and the caller of this method
         * should be responsible for the deletion. In the default
         * implementation, it returns nothing.
         */
        virtual Action * getActions();

        /**
         * Points to the next transition in the sequence. Transitions
         * are arranged in a singly linked list.
         */
        BaseTransition * next;
    };


    /**
     * @name Types of Condition
     *
     * Conditions can be used to trigger transitions (or anything
     * else).
     */
    /* @{ */

    /**
     * The condition interface offsets the problem of whether
     * transitions should fire by having a separate set of condition
     * instances that can be combined together with boolean operators.
     */
    class Condition
    {
    public:
        /**
         * Performs the test for this condition.
         */
        virtual bool test() = 0;
    };

    /**
     * A condition that checks if a watched integer matches a
     * specified value.
     */
    class IntegerMatchCondition : public Condition
    {
    public:
        /**
         * A pointer to the integer value we should try to match.
         */
        int *watch;

        /**
         * The target value for the integer. If this is matched, then
         * the condition will be true.
         */
        int target;

        /**
         * Checks if the target matches the watch value.
         */
        virtual bool test()
        {
            return (*watch == target);
        }
    };

    /**
     * A mixin intended for use with a base transition derived
     * class. Uses an external condition instances to determine if the
     * transition is triggered.
     */
    class ConditionalTransitionMixin
    {
    public:
        /**
         * Holds the condition used to determine if the transition
         * should trigger.
         */
        Condition * condition;

        /**
         * Returns true if the transition wants to fire.
         */
        virtual bool isTriggered();
    };

    /* @} */

}; // end of namespace

#endif // AICORE_SM_H
