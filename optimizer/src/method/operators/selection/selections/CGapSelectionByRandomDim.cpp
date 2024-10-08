#include <algorithm>
#include <cfloat>
#include "CGapSelectionByRandomDim.h"
#include "../../../../utils/random/CRandom.h"

std::vector<std::pair<SMOIndividual*, SMOIndividual*>> CGapSelectionByRandomDim::Select(std::vector<SMOIndividual*>& parents, int objectiveNumber, int populationSize)
{
    // Backup in case of 1 solution in archive
    if (parents.size() < 2)
    {
        return { {parents[0], parents[0]} };
    }

    std::vector<std::pair<SMOIndividual*, SMOIndividual*>> selectedParents;
    selectedParents.reserve(populationSize / 2);

    // Calculate gap sizes for parent individuals
    std::vector<float> gapValues = CalculateGapValues(parents, objectiveNumber);

    for (size_t i = 0; i < populationSize; i += 2)
    {
        // First parent is selected using tournament selection based on gap sizes
        size_t firstParentIdx = SelectParentIdxByTournament(parents, gapValues);
        size_t secondParentIdx = 0;
        if (m_BNTGA)
        {
            if (firstParentIdx == 0)
            {
                secondParentIdx = 1;
            }
            else if (firstParentIdx == parents.size() - 1)
            {
                secondParentIdx = parents.size() - 2;
            }
            else
            {
                secondParentIdx = firstParentIdx + (CRandom::GetInt(0, 2) == 0 ? 1 : -1);
            }
            parents[firstParentIdx]->OnSelected();
            parents[secondParentIdx]->OnSelected();
        }
        else
        {
            // Second parent is selected as a random neighbour of first parent or...
            secondParentIdx = firstParentIdx + (CRandom::GetInt(0, 2) == 0 ? 1 : -1);
            // ... using tournament selection based on gap values
            if (secondParentIdx < 0 || secondParentIdx >= parents.size())
            {
                secondParentIdx = SelectParentIdxByTournament(parents, gapValues);
            }
        }
        selectedParents.emplace_back(parents[firstParentIdx], parents[secondParentIdx]);
    }
    return selectedParents;
}

std::vector<SMOIndividual *> CGapSelectionByRandomDim::SelectForCoevolution(std::vector<SMOIndividual *> &archive,
                                                                            int objectiveNumber,
                                                                            int individualsCount,
                                                                            bool individualFitness)
{
    std::vector<SMOIndividual*> selectedIndividuals;

    // Backup in case of empty archive
    if (archive.size() < 2)
    {
        // selectedIndividuals.insert(selectedIndividuals.end(), archive.begin(), archive.end());
        return selectedIndividuals;
    }

    selectedIndividuals.reserve(individualsCount);

    // Calculate gap sizes for parent individuals
    std::vector<float> gapValues = CalculateGapValues(archive, objectiveNumber, individualFitness);

    // Select number of individuals equal to individuals count usin tournament selection based on gap values
    while (selectedIndividuals.size() < individualsCount)
    {
        selectedIndividuals.push_back(archive[SelectParentIdxByTournament(archive, gapValues)]);
    }
    return selectedIndividuals;
}

std::vector<float> CGapSelectionByRandomDim::CalculateGapValues(std::vector<SMOIndividual*>& individuals,
                                                                int objectiveNumber,
                                                                bool individualFitness) const
{
    // Parents are sorted based on normalized eval values for a random objective
    size_t objectiveId = CRandom::GetInt(0, objectiveNumber);
    std::sort(individuals.begin(), individuals.end(), [objectiveId, individualFitness](const SMOIndividual* a, const SMOIndividual* b) -> bool
    {
        if (individualFitness)
        {
            return a->m_NormalizedIndividualEvaluation[objectiveId] < b->m_NormalizedIndividualEvaluation[objectiveId];
        }
        return a->m_NormalizedEvaluation[objectiveId] < b->m_NormalizedEvaluation[objectiveId];
    });

    // Calculate Gaps
    size_t frontSize = individuals.size();
    std::vector<float> gapValues(frontSize, 0.f);

    // The first and last individuals have gap sizes set to inf. - "start" and "end" of front
    gapValues[0] = FLT_MAX;
    gapValues[frontSize - 1] = FLT_MAX;

    // Gaps are calculated as the difference between normalized eval value of individual and its two neighbours - max gap size is chosen
    for (size_t i = 1; i < frontSize - 1; ++i)
    {
        if (individualFitness)
        {
            float iValue = individuals[i]->m_NormalizedIndividualEvaluation[objectiveId];
            gapValues[i] = fmaxf(iValue - individuals[i - 1]->m_NormalizedIndividualEvaluation[objectiveId],
                                 individuals[i + 1]->m_NormalizedIndividualEvaluation[objectiveId] - iValue);
        } else
        {
            float iValue = individuals[i]->m_NormalizedEvaluation[objectiveId];
            gapValues[i] = fmaxf(iValue - individuals[i - 1]->m_NormalizedEvaluation[objectiveId],
                                 individuals[i + 1]->m_NormalizedEvaluation[objectiveId] - iValue);
        }
    }

    if (m_BNTGA)
    {
        for (size_t i = 0; i < frontSize; ++i)
        {
            size_t selected = individuals[i]->GetSelected();
            // Gap Balanced
            gapValues[i] = (gapValues[i] / (selected + 1));
            // Gap Balanced norm2 Lambda
            //gapValues[i] = (gapValues[i] / sqrtf(maxGap)) + sqrtf(log((float)globalSelections) / (selected + 1));
            // Gap Balanced norm Lambda
            //gapValues[i] = (gapValues[i] / (selected + 1)) + lambda * sqrtf(log((float)globalSelections * maxSpan) / (selected + 1));
            // Gap Balanced norm2 Lambda
            //gapValues[i] = (gapValues[i] / (selected + 1)) + lambda * sqrtf(log((float)globalSelections) * maxSpan / (selected + 1));
            // Gap Balanced Lambda 1
            //gapValues[i] = (gapValues[i] / (selected + 1)) + lambda * sqrtf(log((float)globalSelections) / (selected + 1));
        }
    }

    return gapValues;
}

size_t CGapSelectionByRandomDim::SelectParentIdxByTournament(const std::vector<SMOIndividual*>& individuals, const std::vector<float>& gapValues) const
{
    size_t popSize = individuals.size();
    size_t parentIdx = CRandom::GetInt(0, popSize);
    float bestGap = gapValues[parentIdx];
    for (size_t i = 1; i < m_TournamentSize / 100.f * popSize; ++i)
    {
        size_t randomIdx = CRandom::GetInt(0, popSize);
        if (gapValues[randomIdx] > bestGap)
        {
            bestGap = gapValues[randomIdx];
            parentIdx = randomIdx;
        }
    }
    return parentIdx;
}