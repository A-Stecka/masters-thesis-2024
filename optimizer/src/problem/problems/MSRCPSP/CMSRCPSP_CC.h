#pragma once

#include "CResource.h"
#include "CScheduler.h"
#include "../../EEvalMode.h"
#include "../../AProblem.h"
#include "../../../method/individual/SGenotype.h"

// Genotype encoding consists of sections: task priorities and task-resource assignments
// m_UpperBounds from CMSRCPSP_TA removed because task-resource assignments are represented as 0.f-1.f
class CMSRCPSP_CC : public AProblem
{
public:
    explicit CMSRCPSP_CC(CScheduler &scheduler, EEvalMode evalMode);
    ~CMSRCPSP_CC() override = default;

    EEvalMode &GetEvalMode() override;
    SProblemEncoding &GetProblemEncoding() override;

    void Evaluate(AIndividual& individual) override;
    void EvaluateIndividualFitness(AIndividual& individual) override;
    void LogSolution(AIndividual& individual) override;
private:
    void CreateProblemEncoding();

    // Selects resource for task based on float gene value for each task in assignment section of genotype
    TResourceID SelectResourceForTask(size_t genotypeIndex, const std::vector<float>& genotype);

    EEvalMode m_EvalMode; // Determines if individual should be evaluated against its collaboration partner
    std::vector<std::vector<TResourceID>> m_CapableResources; // Resources capable of executing each task
    SProblemEncoding m_ProblemEncoding;
    std::vector<float> m_MaxObjectiveValues;
    std::vector<float> m_MinObjectiveValues;

    CScheduler m_Scheduler;
};
