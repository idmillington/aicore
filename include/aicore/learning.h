/*
 * Defines the classes used for learning.
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
 * Holds the general structure of a learning problem, suitable for
 * many different kinds of learning algorithm to tackle. A learning
 * problem represents the state of the game world and the actions that
 * can be taken to modify it. It may therefore be better for you to
 * not use the classes in this file, but to directly build in similar
 * interfaces into your game, so that the learning algorithms can
 * work directly with the game code.
 */
#ifndef AICORE_LEARNING_H
#define AICORE_LEARNING_H

namespace aicore
{
    /**
     * A state in a reinforcement problem will be indexed by number,
     * so all we provide is a value and a slot for more data. If you
     * know the problem in advance, this can be far more concrete.
     */
    struct LearningProblemState
    {
        /** Holds the index number of this state. */
        unsigned index;

        /** Holds additional data used by the state. */
        void* data;
    };

    /**
     * An action moves between states of the problem. In our case we
     * have an aicore::Action instance on board, but in your
     * application it would be better to customise this class.
     */
    struct LearningProblemAction
    {
        /** Holds the index number of this action. */
        unsigned index;

        /**
         * An action that corresponds to this problem action. This
         * could be used to actually cause a character to perform the
         * real world correlate of this problem-specific action.
         */
        Action * action;

        /**
         * Actions can be returned as a linked list - this points to
         * the next item in the list.
         */
        LearningProblemAction * next;

        /**
         * Retrieves the number of actions in the list.
         */
        unsigned getCount();

        /**
         * Retrieves the action at the given position in the list
         * (this item being at zero). If the position is beyond the
         * end of the list then the end item is returned.
         */
        LearningProblemAction * getAtPositionInList(unsigned pos);
    };

    /**
     * This is the result of taking an action - a new world state, and
     * a reward.
     */
    struct LearningProblemActionResult
    {
        LearningProblemState* state;
        real reward;
    };

    /**
     * This is a class that you need to implement in order to
     * represent the problem you're trying to solve to the learning
     * system. Note that this problem assumes that the problem has a
     * finite and easily numerable set of states. If this isn't true
     * then some major recoding would be required.
     */
    class LearningProblem
    {
    public:
        /** Returns the number of states in the problem. */
        virtual unsigned getStateCount() = 0;

        /** Returns the maximum number of actions in the problem. */
        virtual unsigned getActionCount() = 0;

        /** Returns the state with the given index. */
        virtual LearningProblemState* getState(unsigned index) = 0;

        /** Returns a random state at which to start learning. */
        virtual LearningProblemState* getRandomState() = 0;

        /**
         * Returns an initial state at which to start learning. The
         * default implementation is to start at a random state.
         */
        virtual LearningProblemState* getInitialState();

        /** Returns the list of actions that are valid from the given state. */
        virtual LearningProblemAction*
            getActions(LearningProblemState* state) = 0;

        /**
         * Returns the result of taking the given action from the
         * given state.
         */
        virtual LearningProblemActionResult getResult(
            LearningProblemState* state,
            LearningProblemAction* action
            ) = 0;
    };

    /**
     * This is a concrete class that implements the LearningProblem
     * interface with a set of states in an array, each containing a
     * the same number of actions. It is mostly suitable for
     * relatively toy problems.
     */
    class ArrayBasedLearningProblem : public LearningProblem
    {
    protected:
        /** Holds the number of states in the problem. */
        unsigned stateCount;

        /** Holds the number of actions per state. */
        unsigned actionsPerState;

        /** Holds the state objects */
        LearningProblemState *states;

        /** Holds the actions */
        LearningProblemAction *actions;

        /**
         * Holds the transition information: this tells the problem
         * which state and action combinations map to which
         * others. The array starts with each action for the first
         * state - indicating the index of the state that is reached
         * by taking the action. It then continues for the second
         * state and so on. A value of 0xffffff in this array
         * indicates that the action isn't valid for that state.
         */
        unsigned *destination;

        /**
         * Holds the reward matrix - in the same format as destination
         * above, but this time giving the reward for each action from
         * each state.
         */
        real *rewards;

    public:
        /**
         * Creates a new problem with the given data.
         */
        ArrayBasedLearningProblem::ArrayBasedLearningProblem(
            unsigned stateCount,
            unsigned actionsPerState,
            unsigned *destination,
            real *reward);

        /** Virtual destructor. */
        virtual ~ArrayBasedLearningProblem();

        /** Returns the number of states in the problem. */
        virtual unsigned getStateCount();

        /** Returns the number of states in the problem. */
        virtual unsigned getActionCount();

        /** Returns the state with the given index. */
        virtual LearningProblemState* getState(unsigned index);

        /** Returns a random state at which to start learning. */
        virtual LearningProblemState* getRandomState();

        /** Returns the list of actions that are valid from the given state. */
        virtual LearningProblemAction*
            getActions(LearningProblemState* state);

        /**
         * Returns the result of taking the given action from the
         * given state.
         */
        virtual LearningProblemActionResult getResult(
            LearningProblemState* state,
            LearningProblemAction* action
            );
    };

}; // end of namespace

#endif // AICORE_LEARNING_H
