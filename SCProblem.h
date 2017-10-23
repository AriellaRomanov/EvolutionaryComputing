#pragma once
#include "evAPI.h"
#include "shape.h"

using Width = long;
using Length = long;

struct SCProblem
{
  SCProblem(const std::string file_name = "");
  SCProblem(const SCProblem& prob);
  void ReadProblemFile(const std::string file_name);
  void CalculateTotalLength();
  void AddToParameters(Parameters& params);

  Shapes shapes;
  Width width;
  Length length;
};