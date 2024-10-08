#pragma once

#include "../../../../individual/MO/SMOIndividual.h"

class CNonDominatedSorting
{
public:

    void Cluster(std::vector<SMOIndividual *> &population, std::vector<std::vector<size_t>> &clusters);

private:

    struct SSolution
    {
        SSolution(size_t i);

        size_t m_Idx;
        std::vector<SSolution *> m_DominatedSolutions; // Pointers to individuals dominated by solution
        size_t m_DominationCounter; // Number of individuals that dominate solution
    };
};