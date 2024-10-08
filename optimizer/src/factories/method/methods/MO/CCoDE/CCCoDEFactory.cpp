#include "CCCoDEFactory.h"
#include "../../../operators/selection/CSelectionFactory.h"
#include "../../../operators/mutation/CMutationFactory.h"

CRankedTournament *CCCoDEFactory::rankedTournament = nullptr;
CGapSelectionByRandomDim *CCCoDEFactory::gapSelectionByRandomDim = nullptr;

CCCoDE *CCCoDEFactory::CreateCCoDE(SConfigMap *configMap, AProblem &problem, AInitialization *initialization,
                                   ACrossover *crossover, AMutation *mutation)
{
    rankedTournament = CSelectionFactory::CreateRankedTournamentSelection(configMap);
    gapSelectionByRandomDim = CSelectionFactory::CreateGapSelection(configMap, false);

    return new CCCoDE(
            problem,
            *initialization,
            *rankedTournament,
            *gapSelectionByRandomDim,
            *crossover,
            *mutation,
            configMap
    );
}

void CCCoDEFactory::DeleteObjects()
{
    delete rankedTournament;
    delete gapSelectionByRandomDim;
}
