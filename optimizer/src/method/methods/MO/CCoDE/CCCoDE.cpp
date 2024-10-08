#include "CCCoDE.h"
#include <cstring>
#include <algorithm>
#include <set>
#include "../../../../utils/logger/ErrorUtils.h"
#include "../../../../utils/random/CRandom.h"
#include "../utils/archive/ArchiveUtils.h"
#include "../utils/clustering/CNonDominatedSorting.h"

#define USE_HV = true

CCCoDE::CCCoDE(AProblem &evaluator,
               AInitialization &initialization,
               CRankedTournament &rankedTournament,
               CGapSelectionByRandomDim &gapSelection,
               ACrossover &crossover,
               AMutation &mutation,
               SConfigMap *configMap
) :
        AGeneticMethod(evaluator, initialization, crossover, mutation),
        m_RankedTournament(rankedTournament),
        m_GapSelection(gapSelection)
{
    configMap->TakeValue("GapSelectionPercent", m_GapSelectionPercent);
    ErrorUtils::OutOfScopeF("CCoDE", "GapSelectionPercent", m_GapSelectionPercent / 100.f);

    configMap->TakeValue("PopulationSize", m_PopulationSize);
    ErrorUtils::LowerThanZeroI("CCoDE", "PopulationSize", m_PopulationSize);

    if (m_Problem.GetEvalMode() == EEvalMode::SEPARATE_SPECIES)
    {
        m_PopulationSize = 2 * m_PopulationSize;
    }

    configMap->TakeValue("GenerationLimit", m_GenerationLimit);
    ErrorUtils::LowerThanZeroI("CCoDE", "GenerationLimit", m_GenerationLimit);

    configMap->TakeValue("CheckedCollaborationPartners", m_CheckedCollaborationPartners);
    ErrorUtils::LowerThanZeroI("CCoDE", "CheckedCollaborationPartners", m_CheckedCollaborationPartners);

    // Two populations are co-evolved
    ResetPopulations(m_Populations);
    ResetPopulations(m_NextPopulations);
}

void CCCoDE::RunOptimization()
{
    int generation = 0;

    InitialisePopulations();

    while (generation < m_GenerationLimit)
    {
        if (m_Problem.GetEvalMode() == EEvalMode::SEPARATE_SPECIES || (generation % 100) < (100 - m_GapSelectionPercent))
        {
            // Evolve populations with Pareto based tournament selection
            for (size_t i = 0; i < m_Populations.size(); ++i)
            {
                // Selection is performed on population + archive
                std::vector<SMOIndividual *> parentsVector;
                parentsVector.reserve(m_Archive.size() + m_Populations[i].size());
                parentsVector.insert(parentsVector.end(), m_Archive.begin(), m_Archive.end());
                parentsVector.insert(parentsVector.end(), m_Populations[i].begin(), m_Populations[i].end());

                // Nondominated sorting is performed on population to find ranks of individuals
                CNonDominatedSorting nonDominatedSorting;
                std::vector<std::vector<size_t>> combinedClusters;
                nonDominatedSorting.Cluster(parentsVector, combinedClusters);

                // Create next population
                for (size_t j = 0; j < m_PopulationSize; j += 2)
                {
                    auto *firstParent = m_RankedTournament.Select(m_Populations[i]);
                    auto *secondParent = m_RankedTournament.Select(m_Populations[i]);

                    CrossoverAndMutate(i, *firstParent, *secondParent);
                }
            }
        } else
        {
            // Evolve populations with Gap selection
            for (size_t i = 0; i < m_Populations.size(); ++i)
            {
                // Selection is performed on population + archive
                std::vector<SMOIndividual *> parentsVector;
                parentsVector.reserve(m_Archive.size() + m_Populations[i].size());
                parentsVector.insert(parentsVector.end(), m_Archive.begin(), m_Archive.end());
                parentsVector.insert(parentsVector.end(), m_Populations[i].begin(), m_Populations[i].end());

                // Selection is performed on population + archive
                const auto &parents = m_GapSelection.Select(
                        parentsVector,
                        m_Problem.GetProblemEncoding().m_objectivesNumber,
                        m_PopulationSize
                );
                // Create next population
                for (auto &parentPair: parents)
                {
                    CrossoverAndMutate(i, *parentPair.first, *parentPair.second);
                }
            }
        }

        // Evaluation of individuals against themselves - true selection coevolution
        EvaluatePopulationsAgainstThemselves();

        // Assignment of collaboration partners and evaluation for coevolution
        CoevolutionEvaluation(m_NextPopulations);

        // Archive management scheme
        ArchiveManagement(m_NextPopulations);

        // Move m_NextPopulations to m_Populations
        for (auto &population: m_Populations)
        {
            for (SMOIndividual *ind: population)
            {
                delete ind;
            }
        }
        m_Populations = m_NextPopulations;
        m_NextPopulations.clear();
        ResetPopulations(m_NextPopulations);

        ++generation;
    }

    // Log Pareto front (optimizer result) to file
    ArchiveUtils::LogParetoFront(m_Archive);
}

void CCCoDE::Reset()
{
    for (auto &population: m_Populations)
    {
        for (auto &i: population)
        {
            delete i;
        }
    }
    m_Populations.clear();

    for (auto &nextPopulation: m_NextPopulations)
    {
        for (auto &i: nextPopulation)
        {
            delete i;
        }
    }
    m_NextPopulations.clear();

    for (auto &i: m_Archive)
    {
        delete i; // Individuals are deep-copied to archive
    }
    m_Archive.clear();

    // Two populations are co-evolved
    ResetPopulations(m_Populations);
    ResetPopulations(m_NextPopulations);
}

void CCCoDE::InitialisePopulations()
{
    // Initialise m_Populations
    SProblemEncoding &problemEncoding = m_Problem.GetProblemEncoding();
    EPopulationType populationType = EPopulationType::TASK_PRIORITIES;
    for (auto &population: m_Populations)
    {
        for (size_t i = 0; i < m_PopulationSize; ++i)
        {
            auto *newInd = m_Initialization.CreateCCoDEIndividual(problemEncoding, populationType);

            if (m_Problem.GetEvalMode() == EEvalMode::SEPARATE_SPECIES) // If coevolution individuals can be evaluated only once both populations are created
            {
                m_Problem.Evaluate(*newInd);
            }

            population.push_back(newInd);
        }
        populationType = EPopulationType::RESOURCE_ASSIGNMENTS;
    }

    // Evaluation of individuals against themselves - true selection coevolution
    EvaluatePopulationsAgainstThemselves();

    // Assignment of collaboration partners and evaluation for coevolution
    CoevolutionEvaluation(m_Populations);

    // Archive management scheme
    ArchiveManagement(m_Populations);
}

void CCCoDE::CrossoverAndMutate(size_t populationId, SMOIndividual &firstParent, SMOIndividual &secondParent)
{
    if (strcmp(m_Crossover.m_Name, "BinomialCX") == 0)
    {
        BinomialCrossoverAndMutate(populationId, firstParent);
        BinomialCrossoverAndMutate(populationId, secondParent);
    } else
    {
        SMOIndividual *r1 = nullptr, *r2 = nullptr;

        // Mutation of first parent individual
        ChooseMutationPartners(populationId, firstParent, r1, r2);
        m_Mutation.Mutate(m_Problem.GetProblemEncoding(), firstParent, r1->m_Genotype, r2->m_Genotype);

        // Mutation of second parent individual
        ChooseMutationPartners(populationId, secondParent, r1, r2);
        m_Mutation.Mutate(m_Problem.GetProblemEncoding(), secondParent, r1->m_Genotype, r2->m_Genotype);

        auto *firstChild = new SMOIndividual{firstParent};
        auto *secondChild = new SMOIndividual{secondParent};

        // Crossover of mutated parent individuals
        m_Crossover.Crossover(m_Problem.GetProblemEncoding(), firstParent, secondParent,
                              *firstChild, *secondChild);

        if (m_Problem.GetEvalMode() == EEvalMode::SEPARATE_SPECIES) // If coevolution individuals can be evaluated only once both populations are created
        {
            m_Problem.Evaluate(*firstChild);
            m_Problem.Evaluate(*secondChild);
        }

        m_NextPopulations[populationId].emplace_back(firstChild);
        m_NextPopulations[populationId].emplace_back(secondChild);
    }
}

void CCCoDE::BinomialCrossoverAndMutate(size_t populationId, SMOIndividual &parent)
{
    SMOIndividual *r1 = nullptr, *r2 = nullptr;
    auto *mutant = new SMOIndividual{parent};

    // Mutation of parent to create mutant individual
    ChooseMutationPartners(populationId, parent, r1, r2);
    m_Mutation.Mutate(m_Problem.GetProblemEncoding(), *mutant, r1->m_Genotype, r2->m_Genotype);

    auto *child = new SMOIndividual{parent};

    // Crossover of parent and mutant individuals
    m_Crossover.Crossover(m_Problem.GetProblemEncoding(), parent, *mutant, *child);

    if (m_Problem.GetEvalMode() == EEvalMode::SEPARATE_SPECIES) // If coevolution individuals can be evaluated only once both populations are created
    {
        m_Problem.Evaluate(*child);
    }

    m_NextPopulations[populationId].emplace_back(child);

    delete mutant;
}

void CCCoDE::ChooseMutationPartners(size_t populationId, SMOIndividual &child, SMOIndividual *&r1, SMOIndividual *&r2)
{
    if (strcmp(m_Mutation.m_Name, "DifferentialEvolution") == 0) // Selection of individuals r1, r2 is only performed for DE
    {
        size_t index1, index2;

        int i = 0;
        do
        {
            i++;
            index1 = CRandom::GetInt(0, m_Populations[populationId].size());
        } while (*m_Populations[populationId][index1] == child && i < m_NoOfTries);
        r1 = m_Populations[populationId][index1];

        i = 0;
        do
        {
            i++;
            index2 = CRandom::GetInt(0, m_Populations[populationId].size());
        } while ( (*m_Populations[populationId][index2] == *m_Populations[populationId][index1] ||
                   *m_Populations[populationId][index2] == child) && i < m_NoOfTries);
        r2 = m_Populations[populationId][index2];
    }
}

void CCCoDE::ArchiveManagement(std::vector<std::vector<SMOIndividual *>> &populations)
{
    for (auto &population : populations)
    {
        ArchiveUtils::CopyToArchiveWithFiltering(population, m_Archive);
    }
}

void CCCoDE::EvaluatePopulationsAgainstThemselves()
{
    if (std::set{EEvalMode::COEVOLUTION_SELECTION, EEvalMode::COEVOLUTION_SELECTION_NO_ARCHIVE, EEvalMode::COEVOLUTION_SELECTION_RANDOM}
        .count(m_Problem.GetEvalMode()))
    {
        for (auto &population: m_Populations)
        {
            for (auto ind: population)
            {
                m_Problem.EvaluateIndividualFitness(*ind);
            }
        }
    }
}


void CCCoDE::CoevolutionEvaluation(std::vector<std::vector<SMOIndividual*>> &populations)
{
    EEvalMode evalMode = m_Problem.GetEvalMode();

    if (evalMode != EEvalMode::SEPARATE_SPECIES) // If EA then do nothing
    {
        for (size_t i = 0; i < populations.size(); i++)
        {
            for (auto individual: populations[i])
            {
                std::vector<SMOIndividual *> collaborationPartners;
                auto &otherPopulation = i == 0 ? populations[1] : populations[0];

                switch (evalMode)
                {
                    case EEvalMode::COEVOLUTION_RANDOM: // Random coevolution of both populations
                    {
                        collaborationPartners.reserve(2 * m_CheckedCollaborationPartners + 1);

                        SelectCollaborationPartnersRandom(otherPopulation, collaborationPartners);
                        if (!m_Archive.empty())
                        {
                            SelectCollaborationPartnersRandom(m_Archive, collaborationPartners);
                        }
                        break;
                    }
                    case EEvalMode::COEVOLUTION_SELECTION: // Selection based coevolution of both populations
                    {
                        collaborationPartners.reserve(3 * m_CheckedCollaborationPartners + 1);

                        SelectCollaborationPartnersSelectionBased(otherPopulation, collaborationPartners, true);
                        if (!m_Archive.empty())
                        {
                            SelectCollaborationPartnersSelectionBased(m_Archive, collaborationPartners);
                        }
                        break;
                    }
                    case EEvalMode::COEVOLUTION_RANDOM_NO_ARCHIVE: // Random coevolution of both populations - archive not used in coevolution
                    {
                        collaborationPartners.reserve(3 * m_CheckedCollaborationPartners + 1);

                        SelectCollaborationPartnersRandom(otherPopulation, collaborationPartners);
                        break;
                    }
                    case EEvalMode::COEVOLUTION_SELECTION_NO_ARCHIVE: // Selection based coevolution of both populations - archive not used in coevolution
                    {
                        collaborationPartners.reserve(3 * m_CheckedCollaborationPartners + 1);

                        SelectCollaborationPartnersSelectionBased(otherPopulation, collaborationPartners, true);
                        break;
                    }
                    case EEvalMode::COEVOLUTION_SELECTION_RANDOM: // Selection based and random coevolution of both populations
                    {
                        collaborationPartners.reserve(3 * m_CheckedCollaborationPartners + 1);

                        SelectCollaborationPartnersSelectionBased(otherPopulation, collaborationPartners, true);
                        SelectCollaborationPartnersRandom(otherPopulation, collaborationPartners);
                        if (!m_Archive.empty())
                        {
                            SelectCollaborationPartnersSelectionBased(m_Archive, collaborationPartners);
                            SelectCollaborationPartnersRandom(m_Archive, collaborationPartners);
                        }
                        break;
                    }
                    default:
                    {
                        collaborationPartners.push_back(individual);
                        break;
                    }
                }

#ifdef USE_HV
                // Choose best collaboration partner according to HV
                CoevolutionSchemeHV(*individual, collaborationPartners);
#else
                // Choose best collaboration partner according to ED
                CoevolutionSchemeED(*individual, collaborationPartners);
#endif
            }
        }
    }
}

void CCCoDE::CoevolutionSchemeHV(SMOIndividual &individual, std::vector<SMOIndividual *> &collaborationPartners)
{
    auto bestPartner = collaborationPartners[0];

    // Evaluate schedules for every collaboration partner and choose partner with the best HV value
    for (auto partner: collaborationPartners)
    {
        // Assign collaboration partner and evaluate schedule
        individual.m_CollaborationPartner = partner;
        m_Problem.Evaluate(individual);

        // Calculate HV
        CalculateHV(individual);

        // Check best HV
        if (individual.m_HV > bestPartner->m_HV)
        {
            bestPartner = partner;
            bestPartner->m_HV = individual.m_HV;
        }
    }

    // Assign best collaboration partner to individual and evaluate schedule
    individual.m_CollaborationPartner = bestPartner;
    m_Problem.Evaluate(individual);

    // Reset m_HV for all checked collaboration partners
    for (auto partner: collaborationPartners)
    {
        partner->ResetHV();
    }
}

void CCCoDE::CalculateHV(SMOIndividual &individual)
{
    // For normalized evaluation NadirPoint = { 1.f, 1.f }
    individual.m_HV = (1.f - individual.m_NormalizedEvaluation[0]) * (1.f - individual.m_NormalizedEvaluation[1]);
}

void CCCoDE::CoevolutionSchemeED(SMOIndividual &individual, std::vector<SMOIndividual *> &collaborationPartners)
{
    auto bestPartner = collaborationPartners[0];

    // Evaluate schedules for every collaboration partner and choose partner with the best HV value
    for (auto partner: collaborationPartners)
    {
        // Assign collaboration partner and evaluate schedule
        individual.m_CollaborationPartner = partner;
        m_Problem.Evaluate(individual);

        // Calculate ED
        CalculateED(individual);

        // Check best ED
        if (individual.m_ED < bestPartner->m_ED)
        {
            bestPartner = partner;
            bestPartner->m_ED = individual.m_ED;
        }
    }

    // Assign best collaboration partner to individual and evaluate schedule
    individual.m_CollaborationPartner = bestPartner;
    m_Problem.Evaluate(individual);

    // Reset m_HV for all checked collaboration partners
    for (auto partner: collaborationPartners)
    {
        partner->ResetED();
    }
}

void CCCoDE::CalculateED(SMOIndividual &individual)
{
    // For normalized evaluation PerfectPoint = { 0.f, 0.f }
    individual.m_ED = sqrt(pow(individual.m_NormalizedEvaluation[0], 2) + pow(individual.m_NormalizedEvaluation[1], 2));
}

void CCCoDE::SelectCollaborationPartnersRandom(std::vector<SMOIndividual*> &possiblePartners, std::vector<SMOIndividual *> &collaborationPartners)
{
    // Randomly select individuals from possiblePartners
    for (int i = 0; i < m_CheckedCollaborationPartners; i++)
    {
        collaborationPartners.push_back(possiblePartners[CRandom::GetInt(0, possiblePartners.size())]);
    }
}

void CCCoDE::SelectCollaborationPartnersSelectionBased(std::vector<SMOIndividual*> &possiblePartners, std::vector<SMOIndividual *> &collaborationPartners,
                                                       bool individualFitness)
{
    // Select individuals from possiblePartners using GapSelection
    const auto &individuals = m_GapSelection.SelectForCoevolution(
        possiblePartners,
        m_Problem.GetProblemEncoding().m_objectivesNumber,
        m_CheckedCollaborationPartners,
        individualFitness
    );
    collaborationPartners.insert(collaborationPartners.end(), individuals.begin(), individuals.end());
}

void CCCoDE::ResetPopulations(std::vector<std::vector<SMOIndividual *>> &populations)
{
    if (m_Problem.GetEvalMode() == EEvalMode::SEPARATE_SPECIES)
    {
        populations.reserve(1);
        populations.emplace_back();
        populations[0].reserve(m_PopulationSize);
    } else
    {
        populations.reserve(2);
        populations.emplace_back();
        populations.emplace_back();
        for (auto &population: populations)
        {
            population.reserve(m_PopulationSize);
        }
    }
}
