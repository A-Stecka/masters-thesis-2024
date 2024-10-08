#include <stdexcept>
#include "CMSRCPSP_CC.h"
#include "../../../utils/logger/CExperimentLogger.h"

CMSRCPSP_CC::CMSRCPSP_CC(CScheduler &scheduler, EEvalMode evalMode) : m_Scheduler(scheduler), m_EvalMode(evalMode)
{
    CreateProblemEncoding();
    m_Scheduler.InitUnassignedTasks();

    // Only two objectives are considered
    m_MaxObjectiveValues = {
            m_Scheduler.GetMaxDuration(),
            m_Scheduler.GetMaxCost(),
            -1,
            -1,
            -1
    };

    // Only two objectives are considered
    m_MinObjectiveValues = {
            m_Scheduler.GetMinDuration(),
            m_Scheduler.GetMinCost(),
            -1,
            -1,
            -1
    };
}

EEvalMode &CMSRCPSP_CC::GetEvalMode()
{
    return m_EvalMode;
}

SProblemEncoding &CMSRCPSP_CC::GetProblemEncoding()
{
    return m_ProblemEncoding;
}

// For MSRCPSP_CC collaboration partner has to be assigned before final evaluation
void CMSRCPSP_CC::Evaluate(AIndividual &individual)
{
    size_t sectionSize = individual.m_Genotype.m_FloatGenotype.size() / 2;

    switch (m_EvalMode)
    {
        // Co-evolutionary approach with two separate populations
        case EEvalMode::COEVOLUTION_RANDOM:
        case EEvalMode::COEVOLUTION_SELECTION:
        case EEvalMode::COEVOLUTION_RANDOM_NO_ARCHIVE:
        case EEvalMode::COEVOLUTION_SELECTION_NO_ARCHIVE:
        case EEvalMode::COEVOLUTION_SELECTION_RANDOM:
        {
            if (individual.m_CollaborationPartner == nullptr)
            {
                throw std::runtime_error("Unset collaboration partner for COEVOLUTION evaluation mode");
            }

            if (individual.m_PopulationType == EPopulationType::RESOURCE_ASSIGNMENTS)
            {
                // Get task priority section from collaboration partner of individual
                for (size_t i = 0; i < sectionSize; i++)
                {
                    individual.m_Genotype.m_FloatGenotype[i] = individual.m_CollaborationPartner->m_Genotype.m_FloatGenotype[i];
                }
            } else
            {
                // Get task-resource assignment section from collaboration partner of individual
                for (size_t i = sectionSize; i < 2 * sectionSize; i++)
                {
                    individual.m_Genotype.m_FloatGenotype[i] = individual.m_CollaborationPartner->m_Genotype.m_FloatGenotype[i];
                }
            }
            break;
        }
        // Using co-evolutionary method on a single species without collaboration with second population
        default:
        {
            break;
        }
    }

    m_Scheduler.Reset();

    // Assign resources to every task
    for (size_t i = sectionSize; i < 2 * sectionSize; ++i)
    {
        TResourceID selectedResourceID = SelectResourceForTask(i, individual.m_Genotype.m_FloatGenotype);
        m_Scheduler.Assign(i % sectionSize, selectedResourceID);
    }

    // Assign start times to every task
    m_Scheduler.BuildTimestamps_CC(individual.m_Genotype.m_FloatGenotype);

    // We assume this is 2 dim problem
    individual.m_Evaluation =
            {
                    m_Scheduler.EvaluateDuration(),
                    m_Scheduler.EvaluateCost(),
            };

    // Normalize
    for (int i = 0; i < m_ProblemEncoding.m_objectivesNumber; i++)
    {
        individual.m_NormalizedEvaluation[i] = (individual.m_Evaluation[i] - m_MinObjectiveValues[i]) /
                                               (m_MaxObjectiveValues[i] - m_MinObjectiveValues[i]);
    }
}

// For MSRCPSP_CC individual can be evaluated against itself
void CMSRCPSP_CC::EvaluateIndividualFitness(AIndividual &individual)
{
    size_t sectionSize = individual.m_Genotype.m_FloatGenotype.size() / 2;

    m_Scheduler.Reset();

    // Assign resources to every task
    for (size_t i = sectionSize; i < 2 * sectionSize; ++i)
    {
        TResourceID selectedResourceID = SelectResourceForTask(i, individual.m_Genotype.m_FloatGenotype);
        m_Scheduler.Assign(i % sectionSize, selectedResourceID);
    }

    // Assign start times to every task
    m_Scheduler.BuildTimestamps_CC(individual.m_Genotype.m_FloatGenotype);

    // We assume this is 2 dim problem
    individual.m_IndividualEvaluation =
            {
                    m_Scheduler.EvaluateDuration(),
                    m_Scheduler.EvaluateCost(),
            };

    // Normalize
    for (int i = 0; i < m_ProblemEncoding.m_objectivesNumber; i++)
    {
        individual.m_NormalizedIndividualEvaluation[i] = (individual.m_IndividualEvaluation[i] - m_MinObjectiveValues[i]) /
                                                         (m_MaxObjectiveValues[i] - m_MinObjectiveValues[i]);
    }
}

void CMSRCPSP_CC::LogSolution(AIndividual &individual)
{
    Evaluate(individual);
    CExperimentLogger::WriteSchedulerToFile(m_Scheduler, individual);
}

void CMSRCPSP_CC::CreateProblemEncoding()
{
    m_CapableResources.clear();

    const std::vector<CTask> &tasks = m_Scheduler.GetTasks();
    m_CapableResources.reserve(tasks.size());

    for (const CTask &task: tasks)
    {
        std::vector<TResourceID> capableResourceIds;
        m_Scheduler.GetCapableResources(task, capableResourceIds);
        m_CapableResources.push_back(capableResourceIds);
    }

    // Section of genotype containing priorities for each task represented as real values
    SEncodingSection taskPrioritySection = SEncodingSection
            {
                    std::vector<SEncodingDescriptor>(tasks.size(), SEncodingDescriptor{
                            (float) 0, (float) 1
                    }),
                    EEncodingType::ASSOCIATION
            };

    // Section of genotype containing resource assignments for each task also represented as real values
    SEncodingSection resourceAssignmentSection = SEncodingSection
            {
                    std::vector<SEncodingDescriptor>(tasks.size(), SEncodingDescriptor{
                            (float) 0, (float) 1
                    }),
                    EEncodingType::ASSOCIATION
            };

    // Only two objectives are considered
    m_ProblemEncoding = SProblemEncoding{2, {taskPrioritySection, resourceAssignmentSection}};
}

TResourceID CMSRCPSP_CC::SelectResourceForTask(size_t genotypeIndex, const std::vector<float> &genotype)
{
    // Genotype contains two sections of the same size
    size_t taskIndex = genotypeIndex % m_ProblemEncoding.m_Encoding[0].m_SectionDescription.size();
    size_t capableResourcesSize = m_CapableResources[taskIndex].size();

    int selectedResource = (int)(genotype[genotypeIndex] * ((float)capableResourcesSize - 1e-9));
    return m_CapableResources[taskIndex][selectedResource];
}
