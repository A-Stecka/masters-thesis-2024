#include "CBinomialCX.h"
#include "../../../../utils/random/CRandom.h"

void CBinomialCX::Crossover(const SProblemEncoding &problemEncoding, AIndividual &firstParent,
                            AIndividual &secondParent, AIndividual &firstChild, AIndividual &secondChild)
{
    Crossover(problemEncoding, firstParent, secondParent, firstChild);
}

// Only ASSOCIATION type encoding sections are crossed over
void CBinomialCX::Crossover(const SProblemEncoding &problemEncoding, AIndividual &parent, AIndividual &mutant,
                            AIndividual &child)
{
    // Update every gene in child according to child = mutant if rand() < CR, parent otherwise
    size_t sectionStartIndex = 0;
    for (const auto &encodingSection: problemEncoding.m_Encoding)
    {
        const auto &sectionDesc = encodingSection.m_SectionDescription;
        const size_t sectionSize = sectionDesc.size();
        switch (encodingSection.m_SectionType)
        {
            case EEncodingType::ASSOCIATION:
            {
                for (size_t i = 0; i < sectionSize; ++i)
                {
                    size_t g = sectionStartIndex + i;
                    if (CRandom::GetFloat(0, 1) < m_CrossoverRate)
                    {
                        child.m_Genotype.m_FloatGenotype[g] = mutant.m_Genotype.m_FloatGenotype[g];
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