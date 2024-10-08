#pragma once

#include "../AMutation.h"
#include "../../../../problem/SProblemEncoding.h"

class CDifferentialEvolution : public AMutation
{
public:
    explicit CDifferentialEvolution(const char *name, float mutationFactor) : AMutation(name),
                                                                              m_MutationFactor(mutationFactor)
    {};

    ~CDifferentialEvolution() override = default;

    void Mutate(SProblemEncoding &problemEncoding, AIndividual &child) override;

    void Mutate(SProblemEncoding &problemEncoding, AIndividual &child, SGenotype &r1, SGenotype &r2) override;

private:
    float m_MutationFactor;
};
