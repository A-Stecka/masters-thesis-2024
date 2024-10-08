
#include <cstring>
#include "CProblemFactory.h"
#include "MSRCPSP/CMSRCPSP_Factory.h"
#include "TTP/CTTPFactory.h"
#include "CVRP/CCVRPFactory.h"
#include "TSP/CTSPFactory.h"

// Define the static method 'CreateProblem' in the 'CProblemFactory' class
// This method creates instances of different problem types based on the provided problem name
AProblem *CProblemFactory::CreateProblem(const char *problemName, const char *problemConfigurationPath)
{
    // Check if the problem name matches "MSRCPSP_CC_R" and create a true MSRCPSP Cooperative Co-evolution (random) problem instance if it does
    if (strcmp(problemName, "MSRCPSP_CC_R") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::COEVOLUTION_RANDOM);

    // Check if the problem name matches "MSRCPSP_CC_S" and create a true MSRCPSP Cooperative Co-evolution (selection based) problem instance if it does
    if (strcmp(problemName, "MSRCPSP_CC_S") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::COEVOLUTION_SELECTION);

    // Check if the problem name matches "MSRCPSP_CC_R_NA" and create a true MSRCPSP Cooperative Co-evolution (random, w/ archive) problem instance if it does
    if (strcmp(problemName, "MSRCPSP_CC_R_NA") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::COEVOLUTION_RANDOM_NO_ARCHIVE);

    // Check if the problem name matches "MSRCPSP_CC_S_NA" and create a true MSRCPSP Cooperative Co-evolution (selection based, w/ archive) problem instance if it does
    if (strcmp(problemName, "MSRCPSP_CC_S_NA") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::COEVOLUTION_SELECTION_NO_ARCHIVE);

    // Check if the problem name matches "MSRCPSP_CC_SR" and create a true MSRCPSP Cooperative Co-evolution (selection based and random) problem instance if it does
    if (strcmp(problemName, "MSRCPSP_CC_SR") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::COEVOLUTION_SELECTION_RANDOM);

    // Check if the problem name matches "MSRCPSP_NO_COEV" and create an MSRCPSP Cooperative Co-evolution problem instance without coevolutionary eval if it does
    if (strcmp(problemName, "MSRCPSP_NO_COEV") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::NO_COEVOLUTION);

    // Check if the problem name matches "MSRCPSP_SS" and create an MSRCPSP Cooperative Co-evolution problem instance with separate species if it does
    if (strcmp(problemName, "MSRCPSP_SS") == 0)
        return CMSRCPSP_Factory::CreateMSRCPSP_CC(problemConfigurationPath, EEvalMode::SEPARATE_SPECIES);

    if (strcmp(problemName, "MSRCPSP_TA") == 0) return CMSRCPSP_Factory::CreateMSRCPSP_TA(problemConfigurationPath, 5);
    if (strcmp(problemName, "MSRCPSP_TA2") == 0) return CMSRCPSP_Factory::CreateMSRCPSP_TA(problemConfigurationPath, 2);
    if (strcmp(problemName, "MSRCPSP_TO") == 0) return CMSRCPSP_Factory::CreateMSRCPSP_TO(problemConfigurationPath, 5);
    if (strcmp(problemName, "MSRCPSP_TO2") == 0) return CMSRCPSP_Factory::CreateMSRCPSP_TO(problemConfigurationPath, 2);
    if (strcmp(problemName, "TSP") == 0) return CTSPFactory::CreateTSP(problemConfigurationPath);

    if (strcmp(problemName, "TTP1") == 0) return CTTPFactory::CreateTTP1(problemConfigurationPath);
    if (strcmp(problemName, "TTP2") == 0) return CTTPFactory::CreateTTP2(problemConfigurationPath);
    if (strcmp(problemName, "CVRP") == 0) return CCVRPFactory::CreateCVRP(problemConfigurationPath);

    // If none of the above conditions are met, throw a runtime error indicating the problem name is not supported
    throw std::runtime_error("Problem name: " + std::string(problemName) + " not supported");
}

// Define the static method 'DeleteObjects' in the 'CProblemFactory' class
// This method is responsible for cleaning up and deleting objects created by the factory
void CProblemFactory::DeleteObjects()
{
    // Call the DeleteObjects method of the CMSRCPSP_TA_Factory to clean up its objects
    CMSRCPSP_Factory::DeleteObjects();

    // Call the DeleteObjects method of the CTSPFactory to clean up its objects
    CTSPFactory::DeleteObjects();

    // Call the DeleteObjects method of the CTTPFactory to clean up its objects
    CTTPFactory::DeleteObjects();

    // Call the DeleteObjects method of the CCVRPFactory to clean up its objects
    CCVRPFactory::DeleteObjects();
}
