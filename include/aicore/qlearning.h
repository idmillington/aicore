/*
 * Defines the classes used for Q-learning.
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
 * Holds an implementation of a q-learning algorithm. It uses the
 * learning problem representation from the learning.h file.
 */
#ifndef AICORE_QLEARNING_H
#define AICORE_QLEARNING_H

namespace aicore
{

    /**
     * Encapsulates a q-learning algorithm to learn one
     * problem. Includes the data for the learning itself - unlike in
     * the book text, this is all in the same class, because they
     * share the same lifetime.
     */
    class QLearner
    {
    private:
        /**
         * Holds the stride through the qvalue array.
         */
        unsigned stride;

    protected:
        /**
         * References the problem we're trying to learn.
         */
        LearningProblem * problem;

        /**
         * Holds the alpha parameter: the learning rate
         */
        real alpha;

        /**
         * Holds the gamma parameter: the discount rate
         */
        real gamma;

        /**
         * Holds the rho parameter: exploration randomness
         */
        real rho;

        /**
         * Holds the nu parameter: length of walk
         */
        real nu;

        /**
         * Does one iteration of the learning algorithm based on the
         * given state, and returns the new state it has reached.
         */
        LearningProblemState*
        doLearningIteration(LearningProblemState * state);

        /**
         * Retrieves the q value associated with taking the given
         * action at the given state.
         */
        real getQValue(LearningProblemState* state,
                       LearningProblemAction* action);

        /**
         * Retrieves the q value associated with the best action from
         * the given state.
         */
        real getBestQValue(LearningProblemState* state);

        /**
         * Stores the given qvalue associated with taking the given
         * action at the given state.
         */
        void storeQValue(LearningProblemState* state,
                         LearningProblemAction* action,
                         real qvalue);

    public:
        /**
         * Holds the q-values. This is public so we can peek at its
         * values.
         */
        real *qvalues;

        /** Creates a new q-learning system to solve the given problem. */
        QLearner(LearningProblem * problem,
                 real alpha, real gamma, real rho, real nu);

        /** Deletes the internal store. */
        ~QLearner();

        /**
         * Performs the given number of iterations of learning.
         */
        void learn(unsigned iterations=1);

        /**
         * Based on the learning so far, this method returns the
         * algorithms recommendation for the best action in the given
         * state.
         */
        LearningProblemAction* getBestAction(LearningProblemState* state);
    };
}; // end of namespace

#endif // AICORE_QLEARNING_H
