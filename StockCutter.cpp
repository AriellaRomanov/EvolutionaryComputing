#include "StockCutter.h"

StockCutter::StockCutter(const Problem& prob)
  : problem(prob), has_changed(true)
{}

StockCutter::StockCutter(const StockCutter& copy)
  : problem(copy.problem), fitness(copy.fitness), has_changed(copy.has_changed)
{}

Fitness StockCutter::CalculateFitness(const Parameters& params)
{
  CHECKPOINT("FitnessFunction");
  // Fitness is the amount of material left on the end,
  // so find the maximum x coordinate used in the graph.
  // Truly, we've mixed up the duties of the ShapePlacement
  // and the FitnessFunction. We ought to be checking for
  // interference and 'on material' in here instead.
  if (has_changed)
  {
    fitness.clear();
    auto penalty = GetPenalty(params);
    fitness.push_back(GetLengthFitness(penalty));
    fitness.push_back(GetWidthFitness(penalty));
  }
  return fitness;
}

FitType StockCutter::GetLengthFitness(const Penalty penalty)
{
  Length curr_length = 0;
  for (const auto& shp : problem.shapes)
  {
    for(const auto& c : shp->GetCoords())
    {
      if (GetX(c) > curr_length)
        curr_length = GetX(c);
    }
  }
  return problem.length - curr_length - penalty;
}

FitType StockCutter::GetWidthFitness(const Penalty penalty)
{
  Width curr_width = 0;
  for (const auto& shp : problem.shapes)
  {
    for(const auto& c : shp->GetCoords())
    {
      if (GetY(c) > curr_width)
        curr_width = GetY(c);
    }
  }
  return problem.width - curr_width - penalty;
}

long StockCutter::GetGeneCount() const
{
  return static_cast<long>(problem.shapes.size());
}

Gene StockCutter::GetGene(const long idx) const
{
  return std::unique_ptr<Shape>(new Shape(*problem.shapes.at(idx).get()));
}

void StockCutter::SetGene(const long idx, const Gene gene)
{
  problem.shapes.erase(problem.shapes.begin() + idx);
  problem.shapes.emplace(problem.shapes.begin() + idx, new Shape(*(Shape *)gene.get()));
}

void StockCutter::Mutate(std::default_random_engine& engine, const Parameters& params, const double mutation_rate)
{
  CHECKPOINT("Mutate");
  static double goal_stdev = GetParameter("mutateStdev", params, -1);
  
  for (auto& shp : problem.shapes)
  {
    auto mutate = uRandom(engine, 1, 100);
    if (mutate <= (mutation_rate * 100))
    {
      has_changed = true;
      auto x = shp->GetXOrigin();
      auto y = shp->GetYOrigin();
      auto d = shp->GetOrientation();

      if (goal_stdev < 0)
      {
        x = uRandom(engine, 0, static_cast<int>(problem.length));
        y = uRandom(engine, 0, static_cast<int>(problem.width));
        d = uRandom(engine, Dir::UP, Dir::LEFT);
      }
      else
      {
        x = nRandom(engine, x, goal_stdev, 0, static_cast<int>(problem.length));
        y = nRandom(engine, y, goal_stdev, 0, static_cast<int>(problem.width));
        d = uRandom(engine, Dir::UP, Dir::LEFT);
      }

      shp->SetOrigin(x, y, d);
    }
  }
}

void StockCutter::Repair(std::default_random_engine& engine, const Parameters& params)
{
  CHECKPOINT("Repair");
  has_changed = true;

  // Capture which shapes need to be moved
  std::vector<long> replace_shapes;
  std::vector<long> good_shapes;
  Coords graph;
  for (long i = 0; i < static_cast<long>(problem.shapes.size()); i++)
  {
    auto coords = problem.shapes.at(i)->GetCoords();
    bool valid = true;
    for (auto coord : coords)
    {
      auto x_good = (GetX(coord) >= 0) && (GetX(coord) < problem.length);
      auto y_good = (GetY(coord) >= 0) && (GetY(coord) < problem.width);
      auto taken = (graph.find(coord) != graph.end());
      valid = valid && (!taken && x_good && y_good);
    }

    if (valid)
    {
      good_shapes.emplace_back(i);
      for (auto coord : coords)
        graph.insert(coord);
    }
    else
    {
      replace_shapes.emplace_back(i);
      for (long j = 0; j < i; j++)
      {
        auto j_coords = problem.shapes.at(j)->GetCoords();
        bool match = false;
        for (auto coord : j_coords)
          match = match || (coords.find(coord) != coords.end());
        
        if (match)
        {
          replace_shapes.emplace_back(j);
          for (auto coord : j_coords)
            graph.erase(coord);
        }
      }
    }
  }

  // Move the shapes that need to be moved to a new place
  Coords mutations;
  for (long i = 0; i < static_cast<long>(replace_shapes.size()); i++)
  {
    long idx = replace_shapes.at(i);

    bool valid = true;
    long place_attempts = 0;
    do
    {
      auto org_x = uRandom(engine, static_cast<Length>(0), problem.length);
      auto org_y = uRandom(engine, static_cast<Width>(0), problem.width);
      auto rot = uRandom(engine, Dir::UP, Dir::LEFT);
      problem.shapes.at(idx)->SetOrigin(org_x, org_y, rot);

      auto coords = problem.shapes.at(idx)->GetCoords();
      for (auto c : coords)
      {
        auto x = GetX(c);
        auto x_good = (x >= 0) && (x < problem.length);
        auto y = GetY(c);
        auto y_good = (y >= 0) && (y < problem.width);
        auto taken = graph.find(c) != graph.end();
        auto m_taken = mutations.find(c) != mutations.end();

        valid = valid && (!taken && !m_taken && x_good && y_good);
      }

    } while (!valid && ++place_attempts < 100);

    if (valid)
    {
      auto coords = problem.shapes.at(idx)->GetCoords();
      for (auto coord : coords)
        mutations.insert(coord);
    }
    else
    {
      static long swap_count = 0;
      // Clearly not working. So, get it in the bounds, and then 
      // move anything it intersects with
      do
      {
        valid = true;
        auto org_x = uRandom(engine, static_cast<Length>(0), problem.length);
        auto org_y = uRandom(engine, static_cast<Width>(0), problem.width);
        auto rot = uRandom(engine, Dir::UP, Dir::LEFT);
        problem.shapes.at(idx)->SetOrigin(org_x, org_y, rot);

        auto coords = problem.shapes.at(idx)->GetCoords();
        for (auto c : coords)
        {
          auto x_good = (GetX(c) >= 0) && (GetX(c) < problem.length);
          auto y_good = (GetY(c) >= 0) && (GetY(c) < problem.width);
          valid = valid && (x_good && y_good);
        }
  
      } while (!valid);

      auto coords = problem.shapes.at(idx)->GetCoords();
      for (long j = 0; j < static_cast<long>(good_shapes.size()); j++)
      {
        long s_idx = good_shapes.at(j);
        auto s_coords = problem.shapes.at(s_idx)->GetCoords();
        bool match = false;
        for (auto c : s_coords)
          match = match || coords.find(c) != coords.end();

        if (match)
        {
          for (auto c : s_coords)
            graph.erase(c);
          good_shapes.erase(good_shapes.begin() + j--);
          replace_shapes.emplace_back(s_idx);
        }
      }

      i = -1;
      mutations.clear();

      if (++swap_count >= 250)
      {
        // just abort and create a new set of random shapes
        swap_count = 0;
        Randomize(engine, params);
        return;
      }
    }
  }
}

Penalty StockCutter::GetPenalty(const Parameters& params) const
{
  CHECKPOINT("GetPenalty");
  static Penalty penalty = GetParameter("fitPenalty", params, 0);

  Coords graph;
  double tot_penalty = 0;
  for (const auto& shp : problem.shapes)
  {
    for(const auto& c : shp->GetCoords())
    {
      auto itr = graph.find(c);
      auto x = GetX(c);
      auto y = GetY(c);
      if (itr != graph.end() || x < 0 || x >= problem.length || y < 0 || y >= problem.width)
        tot_penalty += penalty;
      graph.insert(c);
    }
  }

  return tot_penalty;
}

bool StockCutter::IsValid(const Parameters& params __attribute__((unused))) const
{
  CHECKPOINT("IsValid");
  
  Coords graph;
  for (const auto& shp : problem.shapes)
  {
    auto coords = shp->GetCoords();
    for (auto c : coords)
    {
      if (GetX(c) >= problem.length || GetX(c) < 0)
        return false;
      if (GetY(c) >= problem.width || GetY(c) < 0)
        return false;
      if (graph.find(c) != graph.end())
        return false;
    }

    for (auto c : coords)
      graph.insert(c);
  }

  return true;
}

void StockCutter::Randomize(std::default_random_engine& engine, const Parameters& params __attribute__((unused)))
{
  CHECKPOINT("RandomPlaceShapes");
  has_changed = true;
  // This will hold all the coordinates that a
  // shape has been placed in already
  Coords graph;

  for (long i = 0; i < static_cast<long>(problem.shapes.size()); i++)
  {
    // For each shape, keep trying to place it.
    // This ought to be re-thought. We are just hoping that
    // we don't get unlucky enough to end up with a shape
    // that physically cannot fit anywhere on the material
    // without fault.
    bool placed = false;
    long attempts = 0;
    long max_attempts = problem.length * problem.width * static_cast<long>(Dir::NUM_DIRS);
    do
    {
      // Place the shape at a random positon and direction
      problem.shapes.at(i)->SetOrigin(uRandom(engine, static_cast<Length>(0), problem.length),
                              uRandom(engine, static_cast<Width>(0), problem.width),
                              uRandom(engine, Dir::UP, Dir::LEFT));
      
      // Get all the coordinate pairs that the shape
      // takes up in the position/rotation
      auto coords = problem.shapes.at(i)->GetCoords();

      // for all coordinates, check to see if we
      // already have a shape sitting there
      bool interference = false;
      for (auto spot : coords)
      {
        // if we already have placed some other
        // shape in this position, it's no good
        bool found = (graph.count(spot) != 0);
        interference = (interference || found);
      }

      if (!interference)
      {
        // we also need to check that the shape is fully
        // on the material
        bool on_material = true;
        for (auto spot : coords)
        {
          // both the x and y coordinates must be correct
          bool good_x = (GetX(spot) >= 0 && GetX(spot) < problem.length);
          bool good_y = (GetY(spot) >= 0 && GetY(spot) < problem.width);
          on_material = on_material && good_x && good_y;
        }
          
        if (on_material)
        {
          // insert all the coordinates that this shape
          // takes up in the overall graph, and mark
          // that we have placed this shape
          placed = true;
          for (auto spot : coords)
            graph.insert(spot);
        }
      }
    }
    while (!placed && ++attempts <= max_attempts);

    if (!placed)
    {
      // we were unable to place the shape,
      // so clear out the graph and start over
      graph.clear();
      i = -1;
    }
  }
}

std::string StockCutter::GetSolutionString() const
{
  std::string solution = "";
  for (const auto& shp : problem.shapes)
    solution.append(shp->GetOriginString() + "\t");
  return solution;
}

bool StockCutter::ReadSolution(const std::string file_name)
{
  std::ifstream file(file_name.c_str());
  if (!file.is_open())
    return false;
  else
  {
    has_changed = true;
    long counter = 0;
    std::string line;
    while (counter < GetGeneCount() && getline(file, line))
    {
      auto gene = GetGene(counter);
      gene->SetValue(line);
      SetGene(counter++, std::move(gene));
    }
    file.close();

    return (counter == GetGeneCount());
  }
}

Individual CreateNewIndividual(const Problem& prob)
{
  return Individual(new StockCutter(prob));
}
