#pragma once
#include "Population.h"

Population ParentSelection(std::default_random_engine& engine, const Parameters& params, Population& population);
Population SurvivalSelection(std::default_random_engine& engine, const Parameters& params, Population& population);
Population RandomSubset(std::default_random_engine& engine, const Parameters& params, Population& population, const long count);
Population Truncation(std::default_random_engine& engine, const Parameters& params, Population& population, const long count);
Population FPS(std::default_random_engine& engine, const Parameters& params, Population& population, const long count);
Population KTourney(std::default_random_engine& engine, const Parameters& params, Population& population, const long count);
Population KTourneyRep(std::default_random_engine& engine, const Parameters& params, Population& population, const long count);
Population KTourneyBase(std::default_random_engine& engine, const Parameters& params, Population& population, const long count, const long k_count, const double p, const bool replace);