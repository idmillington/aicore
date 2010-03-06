/*
 * The random decision tree demo.
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
    char *text;

    virtual bool getBranch();
};

bool InteractiveDecision::getBranch()
{
    char buffer[3];

    // Ask the user the text
    printf("%s [Y/N]\n", text);

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
 * This is a simple wrapper class that adds a text field and a mechanism
 * for reporting a random decision.
 **/
class ReportingRandomDecision : public aicore::RandomDecision
{
public:
    char *text;

    virtual bool getBranch();
};

bool ReportingRandomDecision::getBranch()
{
    printf("Considering random decision:\n%s\n", text);
    printf("Current state: %d, Last time %d\n",
           lastDecision, lastDecisionFrame);

    bool result = aicore::RandomDecision::getBranch();

    if (result) {
        printf("Random decision returned true.\n");
        printf("Recursing to trueNode\n");
    } else {
        printf("Random decision returned false.\n");
        printf("Recursing to falseNode\n");
    }

    return result;
}

/**
 * This is a simple wrapper class that adds a text field and a mechanism
 * for reporting a random decision.
 **/
class ReportingRandomDecisionWithTimeOut
    : public aicore::RandomDecisionWithTimeOut
{
public:
    char *text;

    virtual bool getBranch();
};

bool ReportingRandomDecisionWithTimeOut::getBranch()
{
    printf("Considering random decision (with time out):\n%s\n", text);
    printf("Current state: %d, First time %d, Last time %d\n",
           lastDecision, firstDecisionFrame, lastDecisionFrame);

    bool result = aicore::RandomDecisionWithTimeOut::getBranch();

    if (result) {
        printf("Random decision returned true.\n");
        printf("Recursing to trueNode\n");
    } else {
        printf("Random decision returned false.\n");
        printf("Recursing to falseNode\n");
    }

    return result;
}

/**
 * This is a very simple action that just holds some text to display.
 */
class DemoAction : public aicore::DecisionTreeAction
{
public:
    char *text;
};

/**
 * The main entry point, we set up our decision tree here, and then
 * run it.
 */
int main(int argc, char** argv)
{
    // Holds our list of decisions
    InteractiveDecision decisions[7];
    ReportingRandomDecision rDecision;
    ReportingRandomDecisionWithTimeOut rtoDecision;

    // Holds our list of actions
    DemoAction actions[7];

    // Set up the tree
    decisions[0].text = "Q1. Can you see any enemies? (Yes->Q3, No->Q2)";
    decisions[0].trueBranch = &decisions[2];
    decisions[0].falseBranch = &decisions[1];

    decisions[1].text = "Q2. Is your health okay? (Yes->RDT, No->A2)";
    decisions[1].trueBranch = &rtoDecision;
    decisions[1].falseBranch = &actions[1];

    decisions[2].text = "Q3. Is the enemy close by? (Yes->Q4, No->Q5)";
    decisions[2].trueBranch = &decisions[3];
    decisions[2].falseBranch = &decisions[4];

    decisions[3].text = "Q4. Is the enemy dangerous? (Yes->Q6, No->RD)";
    decisions[3].trueBranch = &decisions[5];
    decisions[3].falseBranch = &rDecision;

    decisions[4].text = "Q5. Is your health okay? (Yes->A3, No->A4)";
    decisions[4].trueBranch = &actions[2];
    decisions[4].falseBranch = &actions[3];

    decisions[5].text = "Q6. Are you in cover? (Yes->A3, No->Q7)";
    decisions[5].trueBranch = &actions[2];
    decisions[5].falseBranch = &decisions[6];

    decisions[6].text = "Q7. Is there cover nearby? (Yes->A5, No->A3)";
    decisions[6].trueBranch = &actions[4];
    decisions[6].falseBranch = &actions[2];

    rtoDecision.text = "RDT: Shall I search for enemies or powerups?";
    rtoDecision.trueBranch = &actions[0];
    rtoDecision.falseBranch = &actions[5];
    rtoDecision.timeOutDuration = 4;
    rtoDecision.firstDecisionFrame = 0;

    rDecision.text = "RD: Shall I attack or taunt?";
    rDecision.trueBranch = &actions[2];
    rDecision.falseBranch = &actions[6];

    actions[0].text = "A1. Seek out enemies.";
    actions[1].text = "A2. Seek out health packs.";
    actions[2].text = "A3. Attack enemy.";
    actions[3].text = "A4. Run away from enemy.";
    actions[4].text = "A5. Head for cover.";
    actions[5].text = "A6. Seek out powerups.";
    actions[6].text = "A7. Taunt enemy.";

    // Initialise the timers
    aicore::TimingData::init();

    // Loop until the user Ctrl+C's the program
    printf("AI4G: Random Decision Tree Demo (Ctrl+C to exit)\n");
    while (true) {
        // Update the timer and display the frame
        aicore::TimingData::get().update();
        printf("Running frame %d\n", aicore::TimingData::get().frameNumber);

        // Find the decision
        aicore::DecisionTreeNode *node = decisions[0].makeDecision();
        printf("%s\n", ((DemoAction*)node)->text);

        // Space out the frames
        printf("\n\n");
    }

    // We're done with the timers
    aicore::TimingData::deinit();

    // We're fine
    return 0;
}
