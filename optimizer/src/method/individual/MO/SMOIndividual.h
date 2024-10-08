#pragma once

#include <vector>
#include <cstddef>
#include "../AIndividual.h"

class SMOIndividual : public AIndividual
{
public:
    SMOIndividual(SGenotype& genotype, std::vector<float>& evaluation, std::vector<float>& normalizedEvaluation)
        : AIndividual(genotype, evaluation, normalizedEvaluation)
    {};

    SMOIndividual(const SMOIndividual& other)
        : AIndividual(other)
        , m_MetaInfo(other.m_MetaInfo)
    {};

    // Constructor added for CCoDE
    SMOIndividual(SGenotype &genotype, std::vector<float>& evaluation, std::vector<float>& normalizedEvaluation,
                  EPopulationType populationType)
        : AIndividual(genotype, evaluation, normalizedEvaluation, populationType)
    {}
    
    bool IsDominatedBy(const SMOIndividual* otherSolution) const
    {
        // If this has any better value
        for (int i = 0; i < m_NormalizedEvaluation.size(); ++i)
        {
            if (m_NormalizedEvaluation[i] < otherSolution->m_NormalizedEvaluation[i])
            {
                return false;
            }
        }

        // Now we are sure we have worse or equal values

        // If other has at least one better value
        for (int i = 0; i < m_NormalizedEvaluation.size(); ++i)
        {
            if (otherSolution->m_NormalizedEvaluation[i] < m_NormalizedEvaluation[i])
            {
                return true;
            }
        }

        // If equal
        return false;
    }

    bool IsDuplicateEvalValue(const SMOIndividual* otherSolution) const
    {
        for (int i = 0; i < m_NormalizedEvaluation.size(); ++i)
        {
            if (otherSolution->m_NormalizedEvaluation[i] != m_NormalizedEvaluation[i])
            {
                return false;
            }
        }

        return true;
    }

    size_t GetSelected() const
    {
        return m_Selected;
    }

    void OnSelected()
    {
        m_Selected += 1;
    }

    void ResetHV()
    {
        m_HV = -1.0f;
    }

    void ResetED()
    {
        m_ED = -1.0;
    }


    size_t m_Rank = 0;
    float m_CrowdingDistance = 0.0f;
    float m_HV = -1.0f;
    double m_ED = -1.0;
    std::vector<float> m_MetaInfo;
private:
    size_t m_Selected = 0; // No of times individual has been selected in GapSelectionByRandomDim
};