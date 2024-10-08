#pragma once

#include "../../../AGeneticMethod.h"
#include "../../../configMap/SConfigMap.h"
#include "../../../operators/selection/selections/CRankedTournament.h"
#include "../../../operators/selection/selections/CGapSelectionByRandomDim.h"

// Class does not extend AMOGeneticMethod because two populations are used and a different Reset() method is needed
class CCCoDE : public AGeneticMethod
{
public:
    CCCoDE(
            AProblem &evaluator,
            AInitialization &initialization,
            CRankedTournament &rankedTournament,
            CGapSelectionByRandomDim &gapSelection,
            ACrossover &crossover,
            AMutation &mutation,
            SConfigMap *configMap
    );
    ~CCCoDE() override = default;

    void RunOptimization() override;
    void Reset() override;
private:
    std::vector<std::vector<SMOIndividual*>> m_Populations; // Population 0 optimizes tasks, population 1 optimizes resources in EEvalMode::CO_EVOLUTION
    std::vector<std::vector<SMOIndividual*>> m_NextPopulations; // Population 0 optimizes tasks, population 1 optimizes resources in EEvalMode::CO_EVOLUTION
    std::vector<SMOIndividual*> m_Archive;

    int m_GapSelectionPercent = 0;
    int m_CheckedCollaborationPartners = 0;
    int m_NoOfTries = 10; // Anti-inf.loop safeguard for clone prevention and DE mutation

    CRankedTournament &m_RankedTournament;
    CGapSelectionByRandomDim &m_GapSelection;

    void InitialisePopulations();

    void CrossoverAndMutate(size_t populationId, SMOIndividual &firstParent, SMOIndividual &secondParent);
    void BinomialCrossoverAndMutate(size_t populationId, SMOIndividual &parent);
    void ChooseMutationPartners(size_t populationId, SMOIndividual &child, SMOIndividual *&r1, SMOIndividual *&r2);

    void ArchiveManagement(std::vector<std::vector<SMOIndividual*>> &populations);


    void EvaluatePopulationsAgainstThemselves();
    void CoevolutionEvaluation(std::vector<std::vector<SMOIndividual*>> &populations);
    void CoevolutionSchemeHV(SMOIndividual &individual, std::vector<SMOIndividual*> &collaborationPartners);
    void CalculateHV(SMOIndividual &individual);
    void CoevolutionSchemeED(SMOIndividual &individual, std::vector<SMOIndividual*> &collaborationPartners);
    void CalculateED(SMOIndividual &individual);

    void SelectCollaborationPartnersRandom(
        std::vector<SMOIndividual*> &possiblePartners,
        std::vector<SMOIndividual*> &collaborationPartners);

    void SelectCollaborationPartnersSelectionBased(
        std::vector<SMOIndividual*> &possiblePartners,
        std::vector<SMOIndividual*> &collaborationPartners,
        bool individualFitness = false);

    void ResetPopulations(std::vector<std::vector<SMOIndividual*>> &populations);
};