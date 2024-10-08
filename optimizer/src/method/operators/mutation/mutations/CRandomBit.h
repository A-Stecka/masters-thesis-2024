#pragma once

#include "../AMutation.h"
#include "../../../../problem/SProblemEncoding.h"

class CRandomBit : public AMutation
{
public:
    explicit CRandomBit(const char *name, float mutationProbability) : AMutation(name),
                                                                       m_MutationProbability(mutationProbability)
    {};

    ~CRandomBit() override = default;

    void Mutate(SProblemEncoding &problemEncoding, AIndividual &child) override;

private:
    float m_MutationProbability;
};
