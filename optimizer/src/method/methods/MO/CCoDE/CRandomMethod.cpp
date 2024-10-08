#include "CRandomMethod.h"
#include "../../../../utils/logger/ErrorUtils.h"
#include "../utils/archive/ArchiveUtils.h"

CRandomMethod::CRandomMethod(AProblem &evaluator, AInitialization &initialization, SConfigMap *configMap) :
        AMethod(evaluator, initialization)
{
    configMap->TakeValue("CheckedResults", m_CheckedResults);
    ErrorUtils::LowerThanZeroI("RandomMethod", "CheckedResults", m_CheckedResults);
}

void CRandomMethod::RunOptimization()
{
    for (int i = 0; i < m_CheckedResults; ++i)
    {
        auto *result = m_Initialization.CreateCCoDEIndividual(m_Problem.GetProblemEncoding(), EPopulationType::IGNORED);
        m_Problem.Evaluate(*result);
        ArchiveUtils::CopyToArchiveWithFiltering(result, m_Archive);
        delete result;
    }
    ArchiveUtils::LogParetoFront(m_Archive);
}

void CRandomMethod::Reset()
{
    for (auto &i: m_Archive)
    {
        delete i;
    }
    m_Archive.clear();
}