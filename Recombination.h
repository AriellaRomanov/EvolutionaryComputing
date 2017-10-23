#pragma once
#include "StockCutter.h"

Individual Recombine(const Parameters& params, const Problem& problem, const Individual& mom, const Individual& dad);
Individual NPointCrossOver(const Individual& mom, const Individual& dad, const long n, const Problem& problem);
