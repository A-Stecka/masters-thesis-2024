#include "ParetoReader.h"
#include "ParetoWriter.h"
#include "ConfigResults.h"
#include "InstancesOrder.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

std::vector<std::string> SplitLineBySpace(const std::string &line)
{
    std::vector<std::string> words;
    size_t begin_pos = 0;
    size_t pos = 0;
    while ((pos = line.find(" ", begin_pos)) != std::string::npos)
    {
        words.push_back(line.substr(begin_pos, pos - begin_pos));
        begin_pos = pos + 1;
    }
    words.push_back(line.substr(begin_pos, line.length() - begin_pos));
    return words;
}

int main(int argc, char *argv[])
{
    std::vector<ConfigData *> configsToAnalyze;

    for (int a = 1; a < argc; a++)
    {
        const char *experimentDirPath = argv[a];

        ParetoReader paretoReader;
        std::filesystem::path path = experimentDirPath;
        std::string name = path.filename().string();

        auto *newConfig = new ConfigData(name, experimentDirPath);
        configsToAnalyze.emplace_back(newConfig);
        paretoReader.ReadConfigParetos(newConfig->configPath.c_str(), newConfig->configResults, "d6");

        bool allContains = true;
        for (const ConfigData *config: configsToAnalyze)
        {
            if (!config->configResults.Contains("d6"))
            {
                allContains = false;
                std::cout << "Missing: " << name << " results for config: " << config->configName << std::endl;
                break;
            }
        }


        //++instanceCounter;

        if (!allContains)
        {
            return 0;
        }
    }

    ParetoFront trueParetoFront;

    ParetoWriter paretoWriter;
    //////////////////////////////////////////////////////////////////////////
    // Calculate true Pareto and write
    for (ConfigData *config: configsToAnalyze)
    {
        config->configMergedPareto = config->configResults.GetMergedParetoFronts("d6");
        paretoWriter.WriteParetoToCSV("./", config->configName + "_" + "_merged",config->configMergedPareto);
        trueParetoFront = trueParetoFront.Merge(config->configMergedPareto);
    }
    paretoWriter.WriteParetoToCSV("./",  + "all_merged", trueParetoFront);
    //////////////////////////////////////////////////////////////////////////
    // or
    //////////////////////////////////////////////////////////////////////////
    // Read Pareto front from the file
//		paretoReader.ReadParetoFromCSV(trueParetoDir, selInstName + "_merged", trueParetoFront);
    //////////////////////////////////////////////////////////////////////////

    size_t trueParetoFrontSize = trueParetoFront.solutions.size();
    std::cout << "TPFS:" << trueParetoFrontSize << std::endl;

    if (trueParetoFrontSize == 0)
    {
        std::cout << "TPFS == 0, shutting down!" << std::endl;
        return 0;
    }

    // Normalize points
    {
        size_t sol1Size = trueParetoFront.solutions[0].size();

        // Initialize Min and Max values
        std::vector<float> minValues(sol1Size, 0.f);
        std::vector<float> maxValues(sol1Size, 0.f);
        for (size_t v = 0; v < sol1Size; ++v)
        {
            maxValues[v] = minValues[v] = trueParetoFront.solutions[0][v];
        }

        // Find Min and Max in TrueParetoFront
        for (size_t i = 1; i < trueParetoFrontSize; ++i)
        {
            const std::vector<float> &sol = trueParetoFront.solutions[i];
            for (size_t v = 0; v < sol1Size; ++v)
            {
                float evalValue = sol[v];
                minValues[v] = fminf(minValues[v], evalValue);
                maxValues[v] = fmaxf(maxValues[v], evalValue);
            }
        }

        // Update Pareto data
        if (!trueParetoFront.NormalizeByMinMax(minValues, maxValues))
        {
            std::cout << "Error while trying to normalize TrueParetoFront!" << std::endl;
        }
        // Update config data
        for (ConfigData *config: configsToAnalyze)
        {
            if (!config->configResults.NormalizeByMinMax("d6", minValues, maxValues))
            {
                std::cout << "Error while trying to normalize Config: " << config->configName << "!" << std::endl;
            }
            if (!config->configMergedPareto.NormalizeByMinMax(minValues, maxValues))
            {
                std::cout << "Error while trying to normalize ConfigMergedPareto: " << config->configName << "!"
                          << std::endl;
            }
        }
    }

    // Evaluate
    //std::cout << "Evaluate" << std::endl;
    {
        for (ConfigData *config: configsToAnalyze)
        {
            //ParetoMetrics avgParetoMetrics = config.configResults.EvaluateByTrueParetoFront_IGD(selInstName, trueParetoFront);
            ParetoMetrics avgParetoMetrics = config->configResults.EvaluateByTrueParetoFront("d6", trueParetoFront);
            //config.configResultsDump.push_back(selInstName + ";" + avgParetoMetrics.ToString());
            size_t runsCount = config->configResults.GetParetoCountForInstance("d6");
            size_t mergedSize = config->configMergedPareto.solutions.size();
            size_t mergedNonDominated = config->configMergedPareto.GetNumberOfNonDominatedBy(trueParetoFront);
            std::cout << config->configName << ";runs:" << runsCount << ";MPFS:" << mergedSize << ";MND:"
                      << mergedNonDominated << ";" << avgParetoMetrics.ToString() << std::endl;
        }
    }
    return 0;
}