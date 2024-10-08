#pragma once

#include <vector>
#include "SGenotype.h"
#include "../../problem/EEvalMode.h"

struct AIndividual
{
public:
    SGenotype m_Genotype;
    std::vector<float> m_Evaluation;
    std::vector<float> m_NormalizedEvaluation;
    std::vector<float> m_IndividualEvaluation; // Added for CCoDE
    std::vector<float> m_NormalizedIndividualEvaluation; // Added for CCoDE
    AIndividual *m_CollaborationPartner; // Pointer to collaboration partner - added for CCoDE
    EPopulationType m_PopulationType; // Type of population (optimizes RESOURCE_ASSIGNMENTS or TASK_PRIORITIES; if not relevant then IGNORED) - added for CCoDE

    // Added for CCoDE - checks if every evaluation value is approx. equal with 1e-9 precision
    bool operator == (const AIndividual &other) const
    {
        if (m_Evaluation.size() == other.m_Evaluation.size())
        {
            for (size_t i = 0; i < m_Evaluation.size(); i++)
            {
                if (fabs(m_Evaluation[i] - other.m_Evaluation[i]) > 1e-9)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

protected:
    AIndividual() : m_CollaborationPartner(nullptr), m_PopulationType(EPopulationType::IGNORED)
    {};

    AIndividual(SGenotype& genotype, std::vector<float>& evaluation, std::vector<float>& normalizedEvaluation)
        : m_Genotype(genotype)
        , m_Evaluation(evaluation)
        , m_NormalizedEvaluation(normalizedEvaluation)
        , m_IndividualEvaluation(evaluation)
        , m_NormalizedIndividualEvaluation(normalizedEvaluation)
        , m_CollaborationPartner(nullptr)
        , m_PopulationType(EPopulationType::IGNORED)
    {};

    AIndividual(const AIndividual& other)
        : m_Genotype(other.m_Genotype)
        , m_Evaluation(other.m_Evaluation)
        , m_NormalizedEvaluation(other.m_NormalizedEvaluation)
        , m_IndividualEvaluation(other.m_IndividualEvaluation)
        , m_NormalizedIndividualEvaluation(other.m_NormalizedIndividualEvaluation)
        , m_CollaborationPartner(nullptr)
        , m_PopulationType(other.m_PopulationType)
    {};

    // Constructor added for CCoDE
    AIndividual(SGenotype &genotype, std::vector<float> &evaluation,std::vector<float> &normalizedEvaluation,
                EPopulationType populationType)
        : m_Genotype(genotype)
        , m_Evaluation(evaluation)
        , m_NormalizedEvaluation(normalizedEvaluation)
        , m_IndividualEvaluation(evaluation)
        , m_NormalizedIndividualEvaluation(normalizedEvaluation)
        , m_CollaborationPartner(nullptr)
        , m_PopulationType(populationType)
    {};
};
