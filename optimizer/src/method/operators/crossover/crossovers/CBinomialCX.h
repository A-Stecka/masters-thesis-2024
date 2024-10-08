#pragma once

#include "../ACrossover.h"

class CBinomialCX : public ACrossover
{
public:
    explicit CBinomialCX(const char *name, float crossoverRate) : ACrossover(name), m_CrossoverRate(crossoverRate)
    {};

    ~CBinomialCX() override = default;

    void Crossover(
            const SProblemEncoding &problemEncoding,
            AIndividual &firstParent,
            AIndividual &secondParent,
            AIndividual &firstChild,
            AIndividual &secondChild) override;

    void Crossover(
            const SProblemEncoding &problemEncoding,
            AIndividual &parent,
            AIndividual &mutant,
            AIndividual &child) override;

private:
    float m_CrossoverRate;
};