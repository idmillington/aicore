/*
 * Defines the general action class.
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
 * Holds the action management routines. Actions are the interface
 * between what the AI wants to do, and the game that allows them to
 * do it. Actions can be combined, sequenced, interrupted, and so on,
 * depending on the needs of the AI.
 *
 * The decision making algorithms in the remainder of the toolkit
 * generate these actions to get the game to take account of their
 * decisions.
 */
#ifndef AICORE_ACTION_H
#define AICORE_ACTION_H

namespace aicore
{
    /**
     * The action class is the base class for any request the AI makes
     * of the game. In some cases the AI is simple enough for this to
     * be uneccesary (in steering behaviours for example, the steering
     * output is often directly applied). In decision making, however,
     * it is more important.
     */
    class Action
    {
    public:
        /**
         * The relative priority of this action. This allows actions
         * to prempt others.
         */
        real priority;

        /**
         * Actions naturally come in sequences, so their are
         * implemented as a single linked list by default.
         */
        Action* next;

        /**
         * Retutns the last action in the list of actions.
         */
        Action* getLast();

        /**
         * Checks if this action can be interrupt others. By default
         * no actions can be interrupt.
         */
        virtual bool canInterrupt();

        /**
         * Asks the action to check if it be performed at the same
         * time as the given action. This relationship may not be
         * reflexible: both actions in a pair are asked, and things
         * only progress if both agree. By default an action cannot be
         * done with any other.
         */
        virtual bool canDoBoth(const Action * other) const;

        /**
         * Returns true if the action is done. Otherwise the manager
         * keeps scheduling the action. The default implementation is
         * always done.
         */
        virtual bool isComplete();

        /**
         * Requests that the action delete itself and its children.
         */
        virtual void deleteList();

        /**
         * Called to make the action do its stuff. This depends on the
         * type of action, and the default implementation does
         * nothing.
         */
        virtual void act();
    };

    /**
     * The action manager is a central marshalling system that can be
     * notified of actions that bits of AI wants to perform, and can
     * correctly and flexibly schedule them. It supports priorities in
     * actions, mixing actions together, and sequences of action.
     */
    class ActionManager
    {
    public:
        /**
         * Holds the highest priority value for all actions in the
         * active set.
         */
        real activePriority;

        /**
         * Holds the head of the action queue. This consists of
         * actions that have been scheduled, but are not yet being
         * performed.
         */
        Action * actionQueue;

        /**
         * Holds the list of actions that are currently being
         * performed.
         */
        Action * active;

    protected:
        /**
         * Runs all the active actions, deleting any that
         * complete. This is called automatically by the execute
         * function.
         */
        void runActive();

        /**
         * Allows any high-priority actions to interrupt the currently
         * active set. This is called automatically be the execute
         * method.
         */
        void checkInterrupts();

        /**
         * Checks through all the actions in the action queue, to see
         * if they can be performed at the same time as the actions
         * already in the active set. If so, they are scheduled. This
         * is called automatically be the execute method.
         */
        void addAllToActive();

    public:

        /**
         * Creates a new empty action manager.
         */
        ActionManager();

        /**
         * Adds the given action to the queue.
         */
        void scheduleAction(Action * newAction);

        /**
         * Runs the action manager, running the component actions in
         * turn. Note that the action manager deletes the action
         * objects it is done with.
         */
        void execute();
    };

    /**
     * Compund actions are made up of sub-actions. This is a base
     * class that adds the sub-action management code that then has
     * sematics added in its sub-classes.
     */
    class ActionCompound : public Action
    {
    public:
        Action * subActions;

        /**
         * Requests that the action delete itself and its children.
         */
        virtual void deleteList();

        /**
         * Compound actions are compatible, only if all their
         * components are compatible.
         */
        virtual bool canDoBoth(const Action * other) const;
    };

    /**
     * An action combination is a set of actions that can be performed
     * at the same time.
     */
    class ActionCombination : public ActionCompound
    {
    public:
        /**
         * Checks if this action can interrupt. The combination can
         * interrupt if any of its actions can.
         */
        virtual bool canInterrupt();

        /**
         * Returns true if all the sub-actions is done. Otherwise the
         * manager keeps scheduling the action.
         */
        virtual bool isComplete();

        /**
         * Called to make the action do its stuff. It calls all its
         * subactions.
         */
        virtual void act();
    };

    /**
     * An action combination is a set of actions that can be performed
     * at the same time.
     */
    class ActionSequence : public ActionCompound
    {
    public:
        /**
         * Checks if this action can interrupt. The combination can
         * interrupt if the first action can.
         */
        virtual bool canInterrupt();

        /**
         * Returns true if all the sub-actions are done. Otherwise the
         * manager keeps scheduling the action.
         */
        virtual bool isComplete();

        /**
         * Called to make the action do its stuff. It calls all its
         * subactions.
         */
        virtual void act();
    };

}; // end of namespace

#endif // AICORE_ACTION_H
