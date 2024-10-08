#include "CDifferentialEvolution.h"
#include "../../../../utils/random/CRandom.h"

void CDifferentialEvolution::Mutate(SProblemEncoding &problemEncoding, AIndividual &child)
{
    throw std::runtime_error("Method doesn't support Differential Evolution");
}

// Only ASSOCIATION type encoding sections are mutated
void CDifferentialEvolution::Mutate(SProblemEncoding &problemEncoding, AIndividual &child, SGenotype &r1, SGenotype &r2)
{
    // Update every gene in child according to child = child + F ∗ (other1 − other2)
    size_t sectionStartIndex = 0;
    for (const auto& encodingSection : problemEncoding.m_Encoding)
    {
        const auto& sectionDesc = encodingSection.m_SectionDescription;
        const size_t sectionSize = sectionDesc.size();
        switch (encodingSection.m_SectionType)
        {
            case EEncodingType::ASSOCIATION:
            {
                for (size_t i = 0; i < sectionSize; ++i)
                {
                    size_t g = sectionStartIndex + i;
                    child.m_Genotype.m_FloatGenotype[g] += m_MutationFactor * (r1.m_FloatGenotype[g] - r2.m_FloatGenotype[g]);
                    if (child.m_Genotype.m_FloatGenotype[g] < sectionDesc[i].m_MinValue || child.m_Genotype.m_FloatGenotype[g] >= sectionDesc[i].m_MaxValue)
                    {
                        child.m_Genotype.m_FloatGenotype[g] = CRandom::GetFloat(sectionDesc[i].m_MaxValue, sectionDesc[i].m_MinValue);
                    }
                }
                break;
            }
            case EEncodingType::PERMUTATION: case EEncodingType::BINARY: // Encoding types not supported - sections ignored
                break;
        }
        sectionStartIndex += sectionSize;
    }
}
