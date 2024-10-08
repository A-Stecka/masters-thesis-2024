#pragma once

#include <string>
#include <stack>
#include "CTask.h"
#include "method/individual/AIndividual.h"

class CScheduler
{
public:
    const std::vector<std::vector<TResourceID>>& GetCapableResources() const
    { return m_CapableResources; };

    const std::vector<CTask> &GetTasks() const
    { return m_Tasks; };

    const std::vector<CResource> &GetResources() const
    { return m_Resources; };

    void SetInstanceName(const std::string& instanceName);
    void SetCapableResources(const std::vector<std::vector<TResourceID>>& capableResources);
    void SetTasks(const std::vector<CTask> &tasks);
    void SetResources(const std::vector<CResource> &resources);
    void InitUnassignedTasks();

    const CTask* GetTaskById(TTaskID taskId) const;
    const CResource* GetResourceById(TResourceID resourceId) const;

    void Assign(size_t taskIndex, TResourceID resourceId);
    void BuildTimestamps_TA();
    void BuildTimestamps_TO(std::vector<int>& tasksIndexes);

    void BuildTimestampForTask_TA(CTask &task); // Builds timestamp for one task (task-resource assignment encoding) and sets working time for resource
    void BuildTimestampForTask_TO(size_t taskIndex); // Assigns resource and builds timestamp for one task

    void BuildTimestamps_CC(std::vector<float>& genotype); // Builds timestamps for CCoDE encoding - added for CCoDE
    void AssignTask_CC(size_t taskIndex); // Checks all predecessors of task and builds timestamp for predecessors and task - added for CCoDE
    void BuildTimestampForTask_CC(size_t taskIndex); // Assigns resource and builds timestamp for one task - added for CCoDE

    void Clear();
    void Reset(); // Unassignes all resources from all tasks and unsets task order

    float EvaluateDuration();
    float EvaluateCost();
    float EvaluateAvgCashFlowDev();
    float EvaluateSkillOveruse();
    float EvaluateAvgUseOfResTime();
    void GetCapableResources(const CTask &task, std::vector<TResourceID> &resourceIds) const;
    TTime GetEarliestTime(const CTask &task) const;

    std::string GetInstanceName() const;
    float GetMaxDuration() const; // Tasks executed one after the other
    float GetMinDuration() const; // Tasks assigned uniformly to resources, shortest duration assumed for each task
    float GetMaxCost() const; // Tasks executed by most expensive resource
    float GetMinCost() const; // Tasks executed by least expensive (cheapest) resource
    float GetMaxAvgCashFlowDev() const;
    float GetMinAvgCashFlowDev() const;
    float GetMaxSkillOveruse() const;
    float GetMinSkillOveruse() const;
    float GetMaxAvgUseOfResTime() const;
    float GetMinAvgUseOfResTime() const;

private:
    std::string m_InstanceName;
    std::vector<std::vector<TResourceID>> m_CapableResources; // Resources capable of executing each task
    std::vector<CTask> m_Tasks;
    std::vector<CResource> m_Resources;

    std::vector<std::vector<size_t>> m_UnassignedTasksIndexes; // Tasks blocked by precedence (successors) for each task

    TTime GetBestCapableResourceId(size_t taskIndex, TTime earliestTime); // Chooses capable resource with earliest start time
    std::vector<size_t> GetTasksIndexes(std::vector<float>& priorities, size_t sectionSize); // Sorts task indexes by task priority
    void AssignTask(size_t taskIndex);
};
