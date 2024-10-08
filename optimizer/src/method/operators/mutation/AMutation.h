
#pragma once

#include "../../../problem/SProblemEncoding.h"
#include "../../individual/AIndividual.h"

class AMutation
{
public:
    explicit AMutation(const char *name) : m_Name(name)
    {};

    virtual ~AMutation() = default;

    virtual void Mutate(SProblemEncoding &problemEncoding, AIndividual &child) = 0;

    // Added for Differential Evolution - other mutation operators do not use any other individuals
    virtual void Mutate(SProblemEncoding &problemEncoding, AIndividual &child, SGenotype &r1, SGenotype &r2)
    { Mutate(problemEncoding, child); };

    const char *m_Name; // Field added for Differential Evolution - if DE then selection of other individuals is needed
};
