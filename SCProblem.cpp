#include "SCProblem.h"

SCProblem::SCProblem(const std::string file_name)
  : width(0), length(0)
{
  ReadProblemFile(file_name);
}

SCProblem::SCProblem(const SCProblem& prob)
  : width(prob.width), length(prob.length)
{
  for (auto& ptr : prob.shapes)
    shapes.emplace_back(new Shape(*ptr.get()));
}

void SCProblem::ReadProblemFile(const std::string file_name)
{
  CHECKPOINT("ReadProblemSet(" + file_name + ")");
  
  // Clear out any old shapes, in case this ever gets run
  // twice in an execution.
  shapes.clear();
  width = 0;
  length = 0;

  std::ifstream file(file_name.c_str());
  if (file.is_open())
  {
    // We need to read through each line in the file
    std::string line;
    while (getline(file, line))
    {
      // The first line is two numbers for the width and the
      // number of shapes to follow. We assume if we have not
      // initialized the width yet, we are on the first line.
      // Not very elegant.
      if (width == 0)
      {
        auto idx = line.find(' ');
        if (idx != std::string::npos)
          width = atoi(line.substr(0, idx).c_str());
        CHECKPOINT("width " + std::to_string(width));
      }
      else
      {
        // All other lines are shapes.
        shapes.emplace_back(new Shape(line));
      }
    }

    file.close();
  }
  CalculateTotalLength();
}

void SCProblem::CalculateTotalLength()
{
  // Maximum length is the sum of the lengths of
  // all shapes in the Upwards rotation
  CHECKPOINT("CalculateTotalLength()");
  length = 0;
  for (const auto& shp : shapes)
    length += ((shp->GetXDimension() > shp->GetYDimension()) ? shp->GetXDimension() : shp->GetYDimension());
}

void SCProblem::AddToParameters(Parameters& params)
{
  params.insert(std::make_pair("maxWidth", std::to_string(width)));
  params.insert(std::make_pair("maxLength", std::to_string(length)));
}