#pragma once

#include <vector>
#include <cmath>
#include <iostream>

class SGenotype
{
public:
    SGenotype() = default;

    SGenotype(const SGenotype &other) : m_FloatGenotype(other.m_FloatGenotype), m_IntGenotype(other.m_IntGenotype),
                                        m_BoolGenotype(other.m_BoolGenotype)
    {};

    // Added for CCoDE - only float genotype is considered because only float genotype is used
    bool operator == (const SGenotype &other) const
    {
        if (m_FloatGenotype.size() == other.m_FloatGenotype.size())
        {
            for (size_t i = 0; i < m_FloatGenotype.size(); i++)
            {
                if (fabs(m_FloatGenotype[i] - other.m_FloatGenotype[i]) > 1e-9)
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    // TODO - why three different genotypes?
    std::vector<float> m_FloatGenotype;
    std::vector<int> m_IntGenotype;
    std::vector<bool> m_BoolGenotype;
};
