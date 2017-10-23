#pragma once
#include "Individual.h"
#include "SCProblem.h"

using Problem = SCProblem;

class StockCutter : public Individual_base
{
    Problem problem;
    Fitness fitness;
    bool has_changed;

    FitType GetLengthFitness(const Penalty penalty);
    FitType GetWidthFitness(const Penalty penalty);

  public:
    StockCutter(const Problem& prob);
    StockCutter(const StockCutter& copy);
    StockCutter(const Shapes& shapes);
    Fitness CalculateFitness(const Parameters& params);
    long GetGeneCount() const;
    Gene GetGene(const long idx) const;
    void SetGene(const long idx, const Gene gene);

    void Mutate(std::default_random_engine& engine, const Parameters& params, const double mutation_rate);
    Penalty GetPenalty(const Parameters& params) const;
    bool IsValid(const Parameters& params) const;
    void Repair(std::default_random_engine& engine, const Parameters& params);
    void Randomize(std::default_random_engine& engine, const Parameters& params);
    std::string GetSolutionString() const;
    bool ReadSolution(const std::string file_name);
};

Individual CreateNewIndividual(const Problem& prob);
