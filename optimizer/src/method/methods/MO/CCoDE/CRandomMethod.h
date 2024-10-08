#pragma once

#include "../../../AMethod.h"
#include "../../../configMap/SConfigMap.h"

class CRandomMethod : public AMethod
{
public:
    CRandomMethod(AProblem &evaluator, AInitialization &initialization, SConfigMap *configMap);
    ~CRandomMethod() override = default;

    void RunOptimization() override;
    void Reset() override;

private:
    int m_CheckedResults = 0;
    std::vector<SMOIndividual*> m_Archive;
};
