#include "CNonDominatedSorting.h"

void CNonDominatedSorting::Cluster(std::vector<SMOIndividual *> &population, std::vector<std::vector<size_t>> &clusters)
{
    size_t popSize = population.size();
    std::vector<SSolution> solutions;
    solutions.reserve(popSize);
    for (size_t i = 0; i < popSize; ++i)
    {
        solutions.emplace_back(i);
    }

    // clusters == fronts
    clusters.push_back(std::vector<size_t>());

    // Domination relation between every pair of individuals in population is considered
    for (size_t p = 0; p < popSize; ++p)
    {
        for (size_t q = 0; q < popSize; ++q)
        {
            if (p != q)
            {
                if (population[q]->IsDominatedBy(population[p]))
                {
                    solutions[p].m_DominatedSolutions.push_back(&(solutions[q]));
                }
                else if (population[p]->IsDominatedBy(population[q]))
                {
                    solutions[p].m_DominationCounter += 1;
                }
            }
        }
        // Nondominated individuals are selected as the first front / cluster
        if (solutions[p].m_DominationCounter == 0)
        {
            clusters[0].push_back(p);
        }
    }

    size_t rank = 1;
    // If there are no nondominated individuals then there are no fronts / clusters and all individuals have the same rank
    while (!clusters[rank - 1].empty())
    {
        clusters.push_back(std::vector<size_t>());
        for (size_t solutionIdx: clusters[rank - 1])
        {
            // Individuals from the previous front / cluster are removed and new nondominated individuals are selected as the next front / cluster
            for (SSolution *dominatedSolution: solutions[solutionIdx].m_DominatedSolutions)
            {
                dominatedSolution->m_DominationCounter -= 1;
                if (dominatedSolution->m_DominationCounter == 0)
                {
                    clusters[rank].push_back(dominatedSolution->m_Idx);
                }
            }
        }
        ++rank;
    }

    // Remove last, which is empty
    clusters.pop_back();

    // Assign ranks to population
    for (size_t c = 0; c < clusters.size(); ++c)
    {
        for (size_t i = 0; i < clusters[c].size(); ++i)
        {
            population[clusters[c][i]]->m_Rank = c;
        }
    }
}

CNonDominatedSorting::SSolution::SSolution(size_t i)
        : m_Idx(i), m_DominationCounter(0)
{
}
