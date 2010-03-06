/*
 * The basic decision tree demo.
 *
 * Part of the Artificial Intelligence for Games system.
 *
 * Copyright (c) Ian Millington 2003-2006. All Rights Reserved.
 *
 * This software is distributed under licence. Use of this software
 * implies agreement with all terms and conditions of the accompanying
 * software licence.
 */
#include <stdio.h>
#include <string.h>
#include <aicore/aicore.h>


/**
 * This is a custom decision implementation that asks the user for the
 * result of the decision.
 */
class InteractiveDecision : public aicore::Decision
{
public:
    /**
     * This will hold the question to ask.
     */
    char *question;

    virtual bool getBranch();
};

bool InteractiveDecision::getBranch()
{
    char buffer[3];

    // Ask the user the question
    printf("%s [Y/N]\n", question);

    // Get a response from the user
    fgets(buffer, 3, stdin);
    if (buffer[0] == 'y' || buffer[0] == 'Y') {
        printf("Recursing to trueNode\n");
        return true;
    } else {
        printf("Recursing to falseNode\n");
        return false;
    }
}

/**
 * This is a very simple action that just holds some text to display.
 */
class DemoAction : public aicore::DecisionTreeAction
{
public:
    char *actionText;
};

/**
 * The main entry point, we set up our decision tree here, and then
 * run it.
 */
int main(int argc, char** argv)
{
    // Holds our list of decisions
    InteractiveDecision decisions[7];

    // Holds our list of actions
    DemoAction actions[5];

    // Set up the tree
    decisions[0].question = "Q1. Can you see any enemies? (Yes->Q3, No->Q2)";
    decisions[0].trueBranch = &decisions[2];
    decisions[0].falseBranch = &decisions[1];

    decisions[1].question = "Q2. Is your health okay? (Yes->A1, No->A2)";
    decisions[1].trueBranch = &actions[0];
    decisions[1].falseBranch = &actions[1];

    decisions[2].question = "Q3. Is the enemy close by? (Yes->Q4, No->Q5)";
    decisions[2].trueBranch = &decisions[3];
    decisions[2].falseBranch = &decisions[4];

    decisions[3].question = "Q4. Is the enemy dangerous? (Yes->Q6, No->A3)";
    decisions[3].trueBranch = &decisions[5];
    decisions[3].falseBranch = &actions[2];

    decisions[4].question = "Q5. Is your health okay? (Yes->A3, No->A4)";
    decisions[4].trueBranch = &actions[2];
    decisions[4].falseBranch = &actions[3];

    decisions[5].question = "Q6. Are you in cover? (Yes->A3, No->Q7)";
    decisions[5].trueBranch = &actions[2];
    decisions[5].falseBranch = &decisions[6];

    decisions[6].question = "Q7. Is there cover nearby? (Yes->A5, No->A3)";
    decisions[6].trueBranch = &actions[4];
    decisions[6].falseBranch = &actions[2];

    actions[0].actionText = "A1. Seek out enemies.";
    actions[1].actionText = "A2. Seek out health packs.";
    actions[2].actionText = "A3. Attack enemy.";
    actions[3].actionText = "A4. Run away from enemy.";
    actions[4].actionText = "A5. Head for cover.";

    // Find the decision
    printf("AI4G: Decision Tree Demo (Ctrl+C to exit)\n");
    aicore::DecisionTreeNode *node = decisions[0].makeDecision();
    printf("%s\n", ((DemoAction*)node)->actionText);

    // We're fine
    return 0;
}
