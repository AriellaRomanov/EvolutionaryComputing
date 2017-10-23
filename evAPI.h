#pragma once
#include <iostream>
#include <functional>
#include <math.h>
#include <algorithm>
#include <map>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <vector>
#include <utility>
#include <set>
#include <random>
#include <memory>

// Uncomment the preprocessor command to
// unleash some debugging to standard output
//#define EV_DEBUG
#ifdef EV_DEBUG
#define CHECKPOINT(msg) { std::cout << msg << std::endl; }
#else
#define CHECKPOINT(msg) {}
#endif

using Parameters = std::map<std::string, std::string>;
using FitType = double;
using Fitness = std::vector<FitType>;
using Penalty = double;

// Reads a config file in a modified ini format and stores the key-pair
// values in the Parameters list
void ReadConfig(const std::string file_path, Parameters& params);

// Retrieves a parameter (as a string) from the supplied parameter list, returning the default value
// if no key is found
std::string GetParameter(const std::string key, const Parameters params, const std::string def_val);

// Retrieves a parameter (as a double) from the supplied parameter list, returning the default value
// if no key is found
double GetParameter(const std::string key, const Parameters params, const double def_val);

// Convert the fitness type to a string type
std::string FitnessToString(const Fitness& fitness);

// Writes the supplied message to the given log file
void WriteToLogFile(const std::string file_path, const std::string msg);
void WriteToLogFile(const std::string file_path, const Parameters& params);

std::vector<std::string> split(const std::string source, const std::string delim);

double NormalRandom(std::default_random_engine& engine, const double mean, const double stdev, const double min, const double max);
double UniformRandom(std::default_random_engine& engine, const double min, const double max);

// Generates random numbers between min and max
// Works for enumerations (uniform random)
template <typename T>
T uRandom(std::default_random_engine& engine, const T min, const T max)
{
  long _max = static_cast<double>(max);
  long _min = static_cast<double>(min);
  return static_cast<T>(UniformRandom(engine, _min, _max));
}

// Generates random numbers between min and max
// Works for enumerations (normal random)
template <typename T>
T nRandom(std::default_random_engine& engine, const double mean, const double stdev, const T min, const T max)
{
  long _max = static_cast<double>(max);
  long _min = static_cast<double>(min);
  return static_cast<T>(NormalRandom(engine, mean, stdev, _min, _max));
}