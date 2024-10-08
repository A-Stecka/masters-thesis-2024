#pragma once

#include <cstddef>
#include <vector>

struct SEncodingDescriptor
{
    float m_MinValue;
    float m_MaxValue;
};

enum class EEncodingType
{
    ASSOCIATION = 0,
    PERMUTATION,
    BINARY
};

struct SEncodingSection
{
    std::vector<SEncodingDescriptor> m_SectionDescription; // Min and max values for each gene
    EEncodingType m_SectionType; // Type of encoding used for this section
};

struct SProblemEncoding
{
    int m_objectivesNumber;
    std::vector<SEncodingSection> m_Encoding; // Entire genotype comprised of multiple sections
    std::vector<std::vector<float>> m_additionalProblemData;
};