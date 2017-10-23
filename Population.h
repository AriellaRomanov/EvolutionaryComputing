#pragma once
#include "StockCutter.h"

Fitness GetAverageFitness(const Parameters& params, const Population& population);
Fitness GetBestFitness(const Parameters& params, const Population& population);

Population InitializePopulation(std::default_random_engine& engine, const Parameters& params, Problem& prob);

double ModifyMutationRate(const Parameters& params, Population& population);

ParetoFronts CreateParetoFronts(const Parameters& params, const Population& population);
void PlaceInParetoFront(const Parameters& params, ParetoFronts& fronts, Individual indi, const long max_front);

bool Dominates(const Parameters& params, const Individual lhs, const Individual rhs);
bool Dominates(const Parameters& params, const Population lhs, const Population rhs);
double PercentDomination(const Parameters& params, const Population lhs, const Population rhs);
