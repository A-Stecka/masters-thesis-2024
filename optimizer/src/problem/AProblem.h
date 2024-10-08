#pragma once

#include "SProblemEncoding.h"
#include "../method/individual/SGenotype.h"
#include "../method/individual/AIndividual.h"

class AProblem
{
public:
    virtual ~AProblem() = default;

    // Added for CCoDE
    virtual EEvalMode &GetEvalMode()
    { throw std::runtime_error("Method doesn't support multiple evaluation modes"); };
    virtual SProblemEncoding &GetProblemEncoding() = 0;
    virtual void Evaluate(AIndividual& individual) = 0;
    virtual void EvaluateIndividualFitness(AIndividual& individual)
    { throw std::runtime_error("Method doesn't support individual fitness evaluation - use Evaluate() instead"); };
    virtual void LogSolution(AIndividual& individual) = 0;
};
