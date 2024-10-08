#include "CRandomBit.h"
#include "utils/random/CRandom.h"

// Only ASSOCIATION type encoding sections are mutated
void CRandomBit::Mutate(SProblemEncoding& problemEncoding, AIndividual &child)
{
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
                    if (CRandom::GetFloat(0, 1) < m_MutationProbability)
                    {
                        float minValue = encodingSection.m_SectionDescription[i].m_MinValue;
                        float maxValue = encodingSection.m_SectionDescription[i].m_MaxValue;
                        child.m_Genotype.m_FloatGenotype[g] = CRandom::GetFloat(minValue, maxValue);
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
