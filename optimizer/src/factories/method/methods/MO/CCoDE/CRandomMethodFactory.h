#pragma once

#include "../../../../../problem/AProblem.h"
#include "../../../../../method/methods/MO/CCoDE/CRandomMethod.h"

class CRandomMethodFactory
{
public:
    static CRandomMethod *CreateRandomMethod(SConfigMap *configMap, AProblem &problem, AInitialization *initialization);
};
