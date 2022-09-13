#include <iostream>
#include <cmath>
#include <ctime>
using namespace std;

int main() {
    //Hyperfocus Constants
        //Progression Percent Increase Per HyperFocus Token in Decimal Form
        float hyperFocusIncreasePerToken = 0.003f;

        //Chance Increase Per HyperFocus Token in Decimal Form
        float hyperFocusChanceIncreasePerToken = 0.04f;

        //Starting and Max Hyperfocus Tokens
        int startingTokens = 0;
        int maxTokens = 6;

    //Stakeout Constants
        //Stakeout Progression Percent Increase
        float stakeOutBonus = 0.01f;

    //Base Game Constants
        //Base Repair Speed (can be used for prove thyself, this simulator assumes one person so divide result by number of people)
        float baseRepairSpeed = 1.0f;

        //Repair Progression Percent in Decimal Form
        float baseRepairGreat = 0.01f;

        //Base Skill Check Chance in Decimal Form
        float baseSkillCheckChance = 0.08f;

        //Toolbox Skill Check Chance in Decimal Form
        float toolboxSkillCheckChance = 0.40f;

        //Starting Gen Charges and Charges to Finish Gen
        float startingGenCharges = 0;
        float genBaseCharges = 90.0f;

    //Run Settings
        int sampleSize = 1000;

        //Toolbox Settings for Run
        bool usingToolbox = true;
            //Percent Speed Increase in Decimal Form (ie 50% faster is 0.50)
            float toolboxSpeedIncrease = 0.50f;

            //Toolbox Charges
            float toolboxCharges = 52.0f;

            //Toolbox Charge Modifier (ie streetwise 15% is 1.15)
            float toolboxChargeModifier = 1.00f;

        //Percent Speed Increase in Decimal Form (ie pentimento 30% slower is -0.30)
        float repairSpeedIncrease = 0.00f;

        //Perks Being Used
        bool usingStakeOut = true;
        bool usingHyperFocus = true;

    //Random Number Generator
    srand(time(0));

    //Variables to hold stats
    double averageTime = 0;
    int averageSkillChecks = 0;

    //Run while loop for every sample
    int sampleSizeCounter = sampleSize;
    while (sampleSizeCounter-- > 0) {
        //Charges until gen is done
        float genRemaining = genBaseCharges - startingGenCharges;

        //Effective charges in toolbox (assumes toolboxChargeModifier stays constant)
        float toolboxChargesLeft = toolboxCharges * toolboxChargeModifier;

        //HyperFocus tokens
        float currTokens = startingTokens;

        //Keep track of time and skillchecks
        float seconds = 0.0f;
        int skillChecks = 0;

        //Variable to keep track of when toolbox runs out
        bool toolboxVar = usingToolbox;

        //Loop runs in one second intervals
        while (genRemaining > 0.0f) {
            //If the toolbox is not out of charges
            if (toolboxVar) {
                //How many charges would be depleted in one second
                float chargesToDeplete = (baseRepairSpeed * (1.0f + repairSpeedIncrease + toolboxSpeedIncrease));

                //If the toolbox would run out of charges before the interval is up
                if (chargesToDeplete > toolboxChargesLeft) {
                    //How much of the interval was spent with a toolbox
                    float percentToolbox = toolboxChargesLeft / chargesToDeplete;
                    float percentNormal = 1 - percentToolbox;

                    //Handle the percent that was done with a toolbox
                    seconds += percentToolbox;
                    genRemaining -= baseRepairSpeed * (1.0f + repairSpeedIncrease + toolboxSpeedIncrease) * percentToolbox;

                    //If the gen was completed need to fix seconds since we overcounted
                    if (genRemaining < 0.0f) {
                        //Use how many charges we went over divided by charges done in one second to find how much we went over
                        seconds += (genRemaining / (baseRepairSpeed * (1.0f + repairSpeedIncrease + toolboxSpeedIncrease) * percentToolbox));
                    }
                    //Otherwise, move on to doing by hand
                    else {
                        //Handle the percent done by hand
                        seconds += percentNormal;
                        genRemaining -= baseRepairSpeed * (1.0f + repairSpeedIncrease) * percentNormal;

                        //If the gen was completed need to fix seconds since we overcounted
                        if (genRemaining < 0.0f) {
                            //Use how many charges we went over divided by charges done in one second to find how much we went over
                            seconds += (genRemaining / (baseRepairSpeed * (1.0f + repairSpeedIncrease) * percentNormal));
                        }
                    }

                    //Toolbox has either been spent or the gen is completed, for simplicity just mark as spent and while loop will handle completion
                    toolboxVar = false;
                }
                //Otherwise, toolbox will not run out of charges.
                else {
                    //Handle amount of gen that was done
                    genRemaining -= baseRepairSpeed * (1.0f + repairSpeedIncrease + toolboxSpeedIncrease);

                    //Subtract charges
                    toolboxChargesLeft -= chargesToDeplete;

                    //One second has past
                    seconds += 1.0f;

                    //If the gen was completed need to fix seconds since we overcounted
                    if (genRemaining < 0.0f) {
                        //Use how many charges we went over divided by charges done in one second to find how much we went over
                        seconds += (genRemaining / (baseRepairSpeed * (1.0f + repairSpeedIncrease + toolboxSpeedIncrease)));
                    }
                }
            }
            //Otherwise, gen was done by hand
            else {
                //Handle amount done by hand
                seconds += 1.0f;
                genRemaining -= baseRepairSpeed * (1.0f + repairSpeedIncrease);

                //If the gen was completed need to fix seconds since we overcounted
                if (genRemaining < 0.0f) {
                    //Use how many charges we went over divided by charges done in one second to find how much we went over
                    seconds += (genRemaining / (baseRepairSpeed * (1.0f + repairSpeedIncrease)));
                }
            }

            //Find base skill check chance
            float skillCheckChance;
            if (toolboxVar) {
                skillCheckChance = toolboxSkillCheckChance;
            } else {
                skillCheckChance = baseSkillCheckChance;
            }

            //Add the chance increase from hyperfocus
            if (usingHyperFocus) {
                skillCheckChance += currTokens * hyperFocusChanceIncreasePerToken;
            }

            //Find a random integer using the chance as a bound (slight rounding errors happen here)
            int num = (rand() % (int)roundf(1 / skillCheckChance));

            //If a skill check occured
            if (num == 0) {
                //Add to the skill check counter
                skillChecks++;

                //Subtract base great value
                genRemaining -= baseRepairGreat * genBaseCharges;

                //Subtract bonus from stakeout
                if (usingStakeOut) {
                    genRemaining -= stakeOutBonus * genBaseCharges;
                }

                //Subtract bonus from stakeout
                if (usingHyperFocus) {
                    genRemaining -= currTokens * hyperFocusIncreasePerToken * genBaseCharges;
                }

                //Add a token
                    //This program assumes hyperfocus gives you the token after calculating the bonus not before
                if (currTokens < maxTokens) {
                    currTokens++;
                }
            }
        }

        //Once the gen is finished, add to the totals
        averageTime += seconds;
        averageSkillChecks += skillChecks;
    }

    //Output the information
    cout << "Sample Size: " << sampleSize << endl;
    cout << "Average Time to Complete: " << averageTime / sampleSize << endl;
    cout << "Average Skill Checks: " << averageSkillChecks / sampleSize << endl;

    return 0;
}
