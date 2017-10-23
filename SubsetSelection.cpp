#include "SubsetSelection.h"

Population ParentSelection(std::default_random_engine& engine, const Parameters& params, Population& population)
{
  static long parent_count = GetParameter("lambdaCount", params, 1) * 2;
  static std::string pSel = GetParameter("parentSel", params, "FPS");
  if (pSel == "FPS")
    return FPS(engine, params, population, parent_count);
  else
    return KTourneyRep(engine, params, population, parent_count);
}

Population SurvivalSelection(std::default_random_engine& engine, const Parameters& params, Population& population)
{
  static long pop_count = GetParameter("muSize", params, 1);
  static std::string sSel = GetParameter("survivalSel", params, "Truncation");
  if (sSel == "KTourney")
    return KTourney(engine, params, population, pop_count);
  else if (sSel == "FPS")
    return FPS(engine, params, population, pop_count);
  else
    return Truncation(engine, params, population, pop_count);
}

Population RandomSubset(std::default_random_engine& engine, const Parameters& params __attribute__((unused)), Population& population, const long count)
{
  CHECKPOINT("Running RandomSubset");
  Population subset;
  int pop_count = population.size();
  for (long i = 0; i < count; i++)
  {
    auto idx = uRandom(engine, 0, pop_count);
    subset.emplace_back(population.at(idx));
  }
  return subset;
}

Population Truncation(std::default_random_engine& engine, const Parameters& params, Population& population, const long count)
{
  CHECKPOINT("Running Truncation");
  // organize the population
  auto fronts = CreateParetoFronts(params, population);
  // Add the strongest ones to the new population
  Population subset;
  for (long frt = 0; frt < static_cast<long>(fronts.size()) && static_cast<long>(subset.size()) < count; frt++)
  {
    while (static_cast<long>(fronts.at(frt).size()) > 0 && static_cast<long>(subset.size()) < count)
    {
      // select one of the current best ones at random
      long idx = uRandom(engine, 0, static_cast<int>(fronts.at(frt).size()) - 1);
      subset.emplace_back(fronts.at(frt).at(idx));
      fronts.at(frt).erase(fronts.at(frt).begin() + idx);
    }
  }
  return subset;
}

Population FPS(std::default_random_engine& engine, const Parameters& params __attribute__((unused)), Population& population, const long count)
{
  CHECKPOINT("Running FPS");

  // organize the population
  auto fronts = CreateParetoFronts(params, population);

  // Create the roulette wheel
  CHECKPOINT("Running FPS: making wheel");
  Population wheel;
  for (long frt = 0; frt < static_cast<long>(fronts.size()); frt++)
  {
    // get the number of times individuals at this level should appear on the wheel
    long level = static_cast<long>(fronts.size()) - frt;
    for (long i = 0; i < static_cast<long>(fronts.at(frt).size()); i++)
    {
      // insert each individual that many times
      for (long c = 0; c < level; c++)
        wheel.emplace_back(fronts.at(frt).at(i));
    }
  }
  CHECKPOINT("Running FPS: wheel size: " + std::to_string(wheel.size()));

  // Roll wheel parent_count number of times
  Population subset;
  for (long i = 0; i < count; i++)
  {
    CHECKPOINT("Running FPS: rolling wheel (" + std::to_string(i) + " of " + std::to_string(count) + ")");
    auto idx = uRandom(engine, 0, static_cast<int>(wheel.size()) - 1);
    subset.emplace_back(wheel.at(idx));
  }
  return subset;
}

Population KTourney(std::default_random_engine& engine, const Parameters& params, Population& population, const long count)
{
  CHECKPOINT("Running KTourney\n");
  static long k_count = GetParameter("KTourney_k", params, 5);
  static double p = GetParameter("KTourney_p", params, 0.5);
  return KTourneyBase(engine, params, population, count, k_count, p, false);
}

Population KTourneyRep(std::default_random_engine& engine, const Parameters& params, Population& population, const long count)
{
  CHECKPOINT("Running KTourneyRep");
  static long k_count = GetParameter("KTourneyRep_k", params, 5);
  static double p = GetParameter("KTourneyRep_p", params, 0.5);
  return KTourneyBase(engine, params, population, count, k_count, p, true);
}

Population KTourneyBase(std::default_random_engine& engine, const Parameters& params, Population& population, const long count, const long k_count, const double p, const bool replace)
{
  CHECKPOINT("KTourneyBase");
  Population subset;
  while (static_cast<long>(subset.size()) < count && static_cast<long>(population.size()) > 0)
  {
    Population tourney;

    // Select k random items
    while (static_cast<long>(tourney.size()) != k_count && 
          static_cast<long>(population.size()) > 0)
    {
      auto idx = uRandom(engine, 0, static_cast<int>(population.size()));
      tourney.emplace_back(population.at(idx));
      population.erase(population.begin() + idx);
    }

    // organize the tournament population
    auto fronts = CreateParetoFronts(params, tourney);
    // select them with probability
    for (long frt = 0; frt < static_cast<long>(fronts.size()); frt++)
    {
      auto prob = p * pow(1 - p, frt) * 100;
      for (long p = 0; p < static_cast<long>(fronts.at(frt).size()); p++)
      {
        auto house = uRandom(engine, 1, 100);
        if (prob <= house)
        {
          subset.emplace_back(fronts.at(frt).at(p));
          if (!replace)
            fronts.at(frt).erase(fronts.at(frt).begin() + p);
          break;
        }
      }
    }

    // put the rest back in the population
    for (long frt = 0; frt < static_cast<long>(fronts.size()); frt++)
      for (long p = 0; p < static_cast<long>(fronts.at(frt).size()); p++)
        population.emplace_back(fronts.at(frt).at(p));
  }

  return subset;
}