#pragma once

enum class EEvalMode
{
    COEVOLUTION_RANDOM = 0, // Individual evolved against its collaboration partners chosen randomly
    COEVOLUTION_SELECTION, // Individual evolved against its collaboration partners chosen with a selection operator
    COEVOLUTION_RANDOM_NO_ARCHIVE, // Individual evolved against its collaboration partners chosen randomly - archive not used in coevolution
    COEVOLUTION_SELECTION_NO_ARCHIVE, // Individual evolved against its collaboration partners chosen with a selection operator - archive not used in coevolution
    COEVOLUTION_SELECTION_RANDOM, // Individual evolved against its collaboration partners chosen with a selection operator and randomly
    NO_COEVOLUTION, // Coevolutionary code run without actual coevolutionary evaluation
    SEPARATE_SPECIES, // Individual evolved on its own
};

enum class EPopulationType
{
    TASK_PRIORITIES = 0,
    RESOURCE_ASSIGNMENTS,
    IGNORED // Population type is ignored - for single population methods
};