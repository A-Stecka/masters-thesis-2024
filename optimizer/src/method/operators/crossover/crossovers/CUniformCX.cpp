#include "CUniformCX.h"
#include "../../../../utils/random/CRandom.h"

// Only ASSOCIATION type encoding sections are crossed over
void CUniformCX::Crossover(const SProblemEncoding& problemEncoding, AIndividual &firstParent, AIndividual &secondParent,
                           AIndividual &firstChild, AIndividual &secondChild)
{
    if (CRandom::GetFloat(0.0f, 1.0f) < m_CrossoverProbability)
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
                        if (CRandom::GetFloat(0.0f, 1.0f) < 0.5f)
                        {
                            firstChild.m_Genotype.m_FloatGenotype[g] = secondParent.m_Genotype.m_FloatGenotype[g];
                        }
                        // Check explicitly both sides ??
                        if (CRandom::GetFloat(0.0f, 1.0f) < 0.5f)
                        {
                            secondChild.m_Genotype.m_FloatGenotype[g] = firstParent.m_Genotype.m_FloatGenotype[g];
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
}
