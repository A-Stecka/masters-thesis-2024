#include "CRandomMethodFactory.h"

CRandomMethod *CRandomMethodFactory::CreateRandomMethod(SConfigMap *configMap, AProblem &problem,
                                                        AInitialization *initialization)
{
    return new CRandomMethod(problem, *initialization, configMap);
}
