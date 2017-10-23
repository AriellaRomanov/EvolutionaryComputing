#include "Population.h"

Fitness GetAverageFitness(const Parameters& params, const Population& population)
{
  CHECKPOINT("GetAverageFitness");
  Fitness avg_fit;
  // sum them all up
  for (long p = 0; p < static_cast<long>(population.size()); p++)
  {
    auto fit = population.at(p)->CalculateFitness(params);
    for (long f = 0; f < static_cast<long>(fit.size()); f++)
    {
      if (p == 0)
        avg_fit.push_back(fit.at(f));
      else
        avg_fit.at(f) += fit.at(f);
    }
  }
  // divide into the average
  for (long i = 0; i < static_cast<long>(avg_fit.size()); i++)
    avg_fit.at(i) /= static_cast<double>(population.size());
  
  // return result
  return avg_fit;
}

Fitness GetBestFitness(const Parameters& params, const Population& population)
{
  CHECKPOINT("GetBestFitness");
  Fitness best_fit = population.at(0)->CalculateFitness(params);
  for (long p = 1; p < static_cast<long>(population.size()); p++)
  {
    auto fitness = population.at(p)->CalculateFitness(params);
    for (long f = 0; f < static_cast<long>(fitness.size()); f++)
      if (fitness.at(f) > best_fit.at(f))
        best_fit.at(f) = fitness.at(f);
  }
  return best_fit;
}


Population InitializePopulation(std::default_random_engine& engine, const Parameters& params, Problem& prob)
{
  CHECKPOINT("InitializePopulation");
  static auto seed_count = GetParameter("popSeedCount", params, 0);
  static long pop_size = GetParameter("muSize", params, 100);
  
  Population population;
  for (long i = 0; i < seed_count; i++)
  {
    auto file = GetParameter("seedFile" + std::to_string(i), params, "");
    if (file != "")
    {
      auto indi = CreateNewIndividual(prob);
      bool read = indi->ReadSolution(file);
      if (!read)
        indi->Randomize(engine, params);
      population.push_back(indi);
    }
  }

  while (static_cast<long>(population.size()) < pop_size)
  {
    auto indi = CreateNewIndividual(prob);
    indi->Randomize(engine, params);
    population.push_back(indi);
  }

  return population;
}

double ModifyMutationRate(const Parameters& params, Population& population)
{
  static long same = 0;
  static Fitness last_avg;

  auto _avg = GetAverageFitness(params, population);
  if (_avg == last_avg)
    same++;
  else
  {
    same = 0;
    last_avg = _avg;
  }

  static double base_rate = GetParameter("childMutate", params, 0.15);
  static double step = GetParameter("mutationStep", params, 0.05);
  double mutation = base_rate;
  for (long i = 0; i < same; i++)
  {
    mutation += step;
    step *= 0.75;
  }

  if (mutation < 0)
    mutation = 0;
  if (mutation > 1)
    mutation = 1;

  return mutation;
}

ParetoFronts CreateParetoFronts(const Parameters& params, const Population& population)
{
  ParetoFronts fronts;
  for (long i = 0; i < static_cast<long>(population.size()); i++)
    PlaceInParetoFront(params, fronts, population.at(i), 0);

  return fronts;
}

void PlaceInParetoFront(const Parameters& params, ParetoFronts& fronts, Individual indi, const long max_front)
{
  bool placed = false;

  // find the highest front that they are not dominated in
  for (long frt = max_front; frt < static_cast<long>(fronts.size()) && !placed; frt++)
  {
    bool dominated = false;
    // check each being at this front for any that dominate
    for (long p = 0; p < static_cast<long>(fronts.at(frt).size()) && !dominated; p++)
      dominated = Dominates(params, fronts.at(frt).at(p), indi);

    // if we were not dominated, add ourselves to this front and then make any we dominate move down
    if (!dominated)
    {
      placed = true;
      fronts.at(frt).emplace_back(indi);
      for (long p = 0; p < static_cast<long>(fronts.at(frt).size()); p++)
      {
        if (Dominates(params, indi, fronts.at(frt).at(p)))
        {
          // remove them from the front and make them move.
          auto weakling = fronts.at(frt).at(p);
          fronts.at(frt).erase(fronts.at(frt).begin() + p);
          PlaceInParetoFront(params, fronts, weakling, frt + 1);
          p--;
        }
      }
    }
  }

  // if completely dominated, create a new front at the rear
  if (!placed)
  {
    fronts.emplace_back();
    fronts.back().emplace_back(indi);
  }
}

bool Dominates(const Parameters& params, const Individual lhs, const Individual rhs)
{
  auto lFit = lhs->CalculateFitness(params);
  auto rFit = rhs->CalculateFitness(params);

  long better = 0;
  for (long f = 0; f < static_cast<long>(lFit.size()); f++)
  {
    if (lFit.at(f) > rFit.at(f))
      better++;
    if (lFit.at(f) < rFit.at(f))
      return false;
  }
  return (better > 0);
}

bool Dominates(const Parameters& params, const Population lhs, const Population rhs)
{
  auto lPer = PercentDomination(params, lhs, rhs);
  auto rPer = PercentDomination(params, rhs, lhs);
  return (lPer > rPer);
}

double PercentDomination(const Parameters& params, const Population lhs, const Population rhs)
{
  long count = 0;
  for (long l = 0; l < static_cast<long>(lhs.size()); l++)
  {
    bool has_dominated = false;
    for (long r = 0; r < static_cast<long>(rhs.size()) && !has_dominated; r++)
    {
      if (Dominates(params, lhs.at(l), rhs.at(r)))
      {
        has_dominated = true;
        count++;
      }
    }
  }
  return count / static_cast<double>(lhs.size());
}