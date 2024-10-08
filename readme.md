# Master's Thesis - CCoDE and MODE methods
Original methods for solving the Multi-Skill Resource Constrained Project Scheduling Problem developed for my master's thesis
-

The solution to the  Multi-Skill Resource Constrained Project Scheduling Problem (MS-RCPSP) is a schedule in which each task has specific start and end times, and each task is assigned a resource that performs it. Each task is carried out for a certain amount of time — its duration. Preemption of tasks is not allowed, meaning that once a task is started, the resource performing it is occupied for the entire duration of the task and cannot interrupt its execution. Additionally, there may be precedence constraints between tasks, meaning a task may have a set of predecessors whose completion is required before it can begin.

In the MS-RCPSP problem, time is viewed as a set of discrete time points and a resource can only perform one task at a time. Resources cannot collaborate on tasks either — each task can only be performed by one resource. One of the differences compared to the classic Resource Constrained Project Scheduling Problem (RCPSP) is that resources are human resources and, as such, they receive compensation for the work performed. Different resources have different hourly wage rates, which reflect varying hourly rates for people with different levels of experience or who are employed in different roles.

Another difference from the classic RCPSP problem is that performing tasks requires specific skills, and for this reason, not every resource can perform every task. Each resource has certain skills, and for each type of skill, a proficiency level is defined. The consideration of skills required to perform tasks means that a resource can only execute a task if the resource possesses the skill required for the completion of the task at or above the proficiency level needed for this task.

To solve the MS-RCPSP problem as a multi-objective problem (two objectives are considered at the same time) two original methods have been proposed: **Cooperative Coevolutionary Differential Evolution** and **Multi-Objective Differential Evolution**. The multi-objective approach is based on the Pareto-dominance relation. Each proposed method is based on differential evolution, therefore they both use real number representation.

This repository is forked from [iMOPSE](https://github.com/imopse/iMOPSE). The iMOPSE library was used as the basis for the implementation of the proposed method.
-
The main method code can be found in [optimizer/src/method/methods/MO/CCoDE](https://github.com/A-Stecka/masters-thesis-2024/tree/main/optimizer/src/method/methods/MO/CCoDE).

An original problem representation — MSRCPSP_CC — has been implemented and can be found in [optimizer/src/problem/problems/MSRCPSP](https://github.com/A-Stecka/masters-thesis-2024/tree/main/optimizer/src/problem/problems/MSRCPSP).

Two additional operators have been implemented: the mutation operator characteristic of differential evolution and the Binomial Crossover. They can be found in [optimizer/src/method/operators/mutation/mutations](https://github.com/A-Stecka/masters-thesis-2024/tree/main/optimizer/src/method/operators/mutation/mutations) and [optimizer/src/method/operators/crossover/crossovers](https://github.com/A-Stecka/masters-thesis-2024/tree/main/optimizer/src/method/operators/crossover/crossovers), respectively.
