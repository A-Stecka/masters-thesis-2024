#pragma once

#include <stdint.h>
#include <vector>
#include "CSkill.h"

using TTime = int16_t;
#define TIME_MAX INT16_MAX;
using TResourceID = int16_t;

class CResource
{
public:

    CResource(TResourceID id, float salary, const std::vector<SSkill> &skills);

    TResourceID GetResourceID() const
    { return m_ID; }

    TTime GetFinish() const
    { return m_Finish; }

    void SetFinish(TTime finish)
    { m_Finish = finish; }

    TTime GetWorkingTime() const
    { return m_WorkingTime; }

    void SetWorkingTime(TTime workingTime)
    { m_WorkingTime = workingTime; }

    void AddWorkingTime(TTime workingTime)
    { m_WorkingTime += workingTime; }

    float GetSalary() const
    { return m_Salary; }

    const std::vector<SSkill> &GetSkills() const
    { return m_Skills; }

    // Returns true if resource has skill at specified or higher level
    bool HasSkill(const SSkill &querySkill) const;

    // Returns false if resource does not have skill
    bool GetSkillLevel(TSkillType skillType, TSkillLevel &skillLevel) const;

private:
    TResourceID m_ID;
    float m_Salary;
    std::vector<SSkill> m_Skills; // Only one entry per skill type - skill is known at only one level
    TTime m_Finish;
    TTime m_WorkingTime; // Used for AvgUseOfResources objective
};
