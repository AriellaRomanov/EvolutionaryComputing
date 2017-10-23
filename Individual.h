#pragma once
#include "evAPI.h"
#include "Gene.h"

class Individual_base;
using Individual = std::shared_ptr<Individual_base>;

class Individual_base
{
  public:
    virtual Fitness CalculateFitness(const Parameters& params) = 0;
    virtual long GetGeneCount() const = 0;
    virtual Gene GetGene(const long idx) const = 0;
    virtual void SetGene(const long idx, const Gene gene) = 0;
    virtual void Mutate(std::default_random_engine& engine, const Parameters& params, const double mutation_rate) = 0;
    virtual Penalty GetPenalty(const Parameters& params) const = 0;
    virtual bool IsValid(const Parameters& params) const = 0;
    virtual void Repair(std::default_random_engine& engine, const Parameters& params) = 0;
    virtual void Randomize(std::default_random_engine& engine, const Parameters& params) = 0;
    virtual std::string GetSolutionString() const = 0;
    virtual bool ReadSolution(const std::string file_name) = 0;
};

using Population = std::vector<Individual>;
using ParetoFronts = std::vector<Population>;