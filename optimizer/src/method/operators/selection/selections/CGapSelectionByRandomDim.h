#pragma once

#include "../../../individual/MO/SMOIndividual.h"
#include "ASelection.h"

class CGapSelectionByRandomDim : public ASelection
{
public:
    // TODO - if we have dynamic selection object, we can get rid of boolean "bntga" parameter, or make it a set of functions called by the method
    explicit CGapSelectionByRandomDim(int tournamentSize, bool bntga) : ASelection(tournamentSize), m_BNTGA(bntga)
    {};
    ~CGapSelectionByRandomDim() override = default;

    std::vector<std::pair<SMOIndividual*, SMOIndividual*>> Select(std::vector<SMOIndividual*>& parents, int objectiveNumber, int populationSize);
    // Added for CCoDE
    std::vector<SMOIndividual*> SelectForCoevolution(
        std::vector<SMOIndividual*>& archive,
        int objectiveNumber,
        int individualsCount,
        bool individualFitness = false);
private:
    bool m_BNTGA;

    std::vector<float> CalculateGapValues(
        std::vector<SMOIndividual*>& individuals,
        int objectiveNumber,
        bool individualFitness = false) const;

    size_t SelectParentIdxByTournament(const std::vector<SMOIndividual*>& individuals, const std::vector<float>& gapValues) const;
};
