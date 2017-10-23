#include "evAPI.h"

void ReadConfig(const std::string file_path, Parameters& params)
{
  CHECKPOINT("Config Read");

  std::ifstream file(file_path.c_str());
  if (file.is_open())
  {
    // get each line in the file
    std::string line;
    while (file >> line)
    {
      auto idx = line.find("=");
      if (idx != std::string::npos)
      {
        // split each line by the = character
        // and insert into the mapping
        auto key = line.substr(0, idx);
        auto val = line.substr(idx + 1, line.size());
        params.insert(std::make_pair(key, val));
      }
    }
  }
  else
    std::cout << "Error opening config file." << std::endl;
}

std::string GetParameter(const std::string key, const Parameters params, const std::string def_val)
{
  CHECKPOINT("GetParameter(" + key + ")");
  // find the key in the mapping
  auto itr = params.find(key);
  // if key exists, return the value
  if (itr != params.end())
    return itr->second;
  // else return the default
  return def_val;
}

double GetParameter(const std::string key, const Parameters params, const double def_val)
{
  CHECKPOINT("GetParameter(" + key + ")");
  // find the key in the mapping
  auto itr = params.find(key);
  // if key exists, return the value
  if (itr != params.end())
    return atof(itr->second.c_str());
  // else return the default
  return def_val;
}

std::string FitnessToString(const Fitness& fitness)
{
  std::string fit_desc = "{ ";
  for (long f = 0; f < static_cast<long>(fitness.size()); f++)
    fit_desc.append(std::to_string(fitness.at(f)) + " ");
  fit_desc.append("}");
  return fit_desc;
}

void WriteToLogFile(const std::string file_path, const std::string msg)
{
  // open file for appending
  std::ofstream file(file_path.c_str(), std::ios_base::app);
  if (file.is_open())
  {
    // write message and close
    file << msg << std::endl;
    file.close();
  }
}

void WriteToLogFile(const std::string file_path, const Parameters& params)
{
  WriteToLogFile(file_path, "Configuration Parameters");
  for (auto& itr : params)
    if (itr.first != "" || itr.second != "")
      WriteToLogFile(file_path, "  " + itr.first + "=" + itr.second);
}

std::vector<std::string> split(const std::string source, const std::string delim)
{
  std::vector<std::string> pieces;
  auto idx = source.find(delim);
  if (idx == std::string::npos)
    pieces.push_back(source);
  else
  {
    auto front = source.substr(0, idx);
    pieces.push_back(front);

    auto start = idx + delim.length();
    auto sub = source.substr(start, source.length() - start);

    auto sub_pieces = split(sub, delim);
    for (auto s : sub_pieces)
      pieces.push_back(s);
  }
  return pieces;
}

double NormalRandom(std::default_random_engine& engine, const double mean, const double stdev, const double min, const double max)
{
  std::normal_distribution<double> distribution(mean, stdev);
  double value = min;
  do
  {
    value = distribution(engine);
  } while (value < min || value > max);
  return value;
}

double UniformRandom(std::default_random_engine& engine, const double min, const double max)
{
  std::uniform_real_distribution<double> distribution(min, max);
  return distribution(engine);
}
