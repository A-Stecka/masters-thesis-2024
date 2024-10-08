#pragma once

#include "../../../../../problem/AProblem.h"
#include "../../../../../method/methods/MO/CCoDE/CCCoDE.h"

class CCCoDEFactory
{
public:
    static CCCoDE *CreateCCoDE(SConfigMap *configMap, AProblem &problem, AInitialization *initialization,
                               ACrossover *crossover, AMutation *mutation);
    static void DeleteObjects();
private:
    static CRankedTournament *rankedTournament;
    static CGapSelectionByRandomDim *gapSelectionByRandomDim;
};
