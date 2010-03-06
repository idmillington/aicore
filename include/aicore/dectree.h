/*
 * Defines the classes used for decision trees.
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
 * Holds classes for making decisions based on a decision
 * tree. Decision trees consist of a series of decisions, arranged so
 * that the results of one decision lead to another, until finally a
 * decision is reached at the end of the tree.
 */
#ifndef AICORE_DECTREE_H
#define AICORE_DECTREE_H

namespace aicore
{

    /**
     * A decision tree node is a base class for anything that makes a
     * decision.
     */
    class DecisionTreeNode
    {
    public:
        /**
         * The make decision method carries out a decision making
         * process and returns the new decision tree node that we've
         * reached in the tree.
         */
        virtual DecisionTreeNode* makeDecision() = 0;
    };

    /**
     * An action is a decision tree node at the end of the tree. It
     * simply returns itself as the result of the next decision.
     */
    class DecisionTreeAction : public DecisionTreeNode
    {
    public:
        /**
         * Makes the decision - in  this case there is no decision, so
         * this method returns the action back again..
         */
        virtual DecisionTreeNode* makeDecision()
        {
            return this;
        }
    };

    /**
     * Other than actions, the tree is made up of decisions, which
     * come up with some boolean result and choose a branch based on
     * that.
     */
    class Decision : public DecisionTreeNode
    {
    public:
        DecisionTreeNode* trueBranch;
        DecisionTreeNode* falseBranch;

        /**
         * This method actually does the checking for the decision.
         */
        virtual bool getBranch() = 0;

        /**
         * This is where the decision tree algorithm is located: it
         * recursively walks down the tree until it reaches the final
         * item to return (which is an action).
         */
        virtual DecisionTreeNode* makeDecision();
    };

    /**
     * This class represents a stable random decision. As long as the
     * decision is reached at each frame, the decision will be made
     * the same way each time. Otherwise the decision will be made at
     * random.
     */
    class RandomDecision : public Decision
    {
    public:
        /**
         * Holds the last decision that was made.
         */
        bool lastDecision;

        /**
         * Holds the number of the last frame at which the decision
         * was made.
         */
        unsigned lastDecisionFrame;

        /** Creates a new random decision. */
        RandomDecision();

        /**
         * Works out which branch to follow.
         */
        virtual bool getBranch();
    };

    /**
     * This class represents a stable random decision. As long as the
     * decision is reached at each frame, the decision will be made
     * the same way each time. Otherwise the decision will be made at
     * random.
     */
    class RandomDecisionWithTimeOut : public RandomDecision
    {
    public:
        /**
         * Holds the frame number that the current decision was made
         * on.
         */
        unsigned firstDecisionFrame;

        /**
         * Holds the number of frames to wait before timing out the
         * decision.
         */
        unsigned timeOutDuration;

        /** Creates a new random decision. */
        RandomDecisionWithTimeOut();

        /**
         * Works out which branch to follow.
         */
        virtual bool getBranch();
    };

}; // end of namespace

#endif // AICORE_DECTREE_H
