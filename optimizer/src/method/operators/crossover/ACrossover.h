
#pragma once

#include <stdexcept>
#include <vector>
#include "../../../problem/SProblemEncoding.h"
#include "../../individual/AIndividual.h"

class ACrossover
{
public:
    explicit ACrossover(const char *name) : m_Name(name)
    {};

    virtual ~ACrossover() = default;

    virtual void Crossover(
            const SProblemEncoding &problemEncoding,
            AIndividual &firstParent,
            AIndividual &secondParent,
            AIndividual &firstChild,
            AIndividual &secondChild) = 0;

    // Added for Binomial Crossover - other crossover operators produce two children
    virtual void Crossover(
            const SProblemEncoding &problemEncoding,
            AIndividual &parent,
            AIndividual &mutant,
            AIndividual &child)
    { throw std::runtime_error("Method doesn't support Binomial Crossover"); };

    const char *m_Name; // Field added for Binomial Crossover - if BinomialCX then crossover produces only one child
};
