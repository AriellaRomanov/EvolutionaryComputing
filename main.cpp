#include "Population.h"
#include "SubsetSelection.h"
#include "Recombination.h"
#include "Counter.h"

void WriteToSolutionFile(const std::string file_path, const Population& population);
void WriteToLogFile(const std::string file_path, const long evals, const Parameters& params, const Population& population);

int main(int argc, char** argv)
{
  /*
  ** CHECK FOR PROGRAM PARAMETERS
  */
  if (argc < 2)
  {
    // Config filepath must be provided as a command-line argument
    std::cout << "Expected configuration filepath." << std::endl;
    return 0;
  }
  
  // Read in all parameters available in config file
  Parameters params;
  ReadConfig(argv[1], params);

  // We allow the problem file either as a command-line argument
  // or as a config file parameter; preference given to command-line
  std::string prob_file = (argc >= 3) ? argv[2] : GetParameter("probFile", params, "");
  if (prob_file == "")
  {
    // If the parameter does not exist in either location,
    // we cannot continue
    std::cout << "Missing problem file." << std::endl;
    return 0;
  }

  // Mark in the parameter list the problem file we will
  // actually be using (in case of conflict between
  // parameters and command-line)
  params["probFile"] = prob_file;

  // Extract other necessary parameters from config
  // file, using the supplied default if it does
  // not exist
  std::string log_file = GetParameter("logFile", params, "Logs/log.txt");
  std::string sol_file = GetParameter("solFile", params, "Solutions/solution.txt");
  std::string vis_file = GetParameter("visFile", params, "Visualizers/visualizer.txt");
  std::string algorithm = GetParameter("Algorithm", params, "EA");

  // If the seed parameter does not exist, we need to generate a seed.
  // We also need to mark the seed that we did use in the execution.
  long seed = GetParameter("seed", params, time(NULL));
  params["seed"] = std::to_string(seed);

  // Initialize the randomness
  std::default_random_engine engine(seed);
  CHECKPOINT("srand(" + std::to_string(seed) + ")");

  /*
  **  END PROGRAM PARAMETERS
  */

  /*
  **  START PROBLEM SET
  */

  // Retrieve the problem from the given file
  // and mark in the parameters the data, then
  // write to log file
  Problem problem(prob_file);
  problem.AddToParameters(params);
  WriteToLogFile(log_file, params);

  /*
  **  END PROBLEM SET
  */

  /*
  **  START EXECUTION
  */
  CHECKPOINT("Starting execution");

  if (algorithm == "Test")
  {
    auto indi = CreateNewIndividual(problem);
    std::cout << indi->GetGeneCount() << std::endl;
    for (long g = 0; g < indi->GetGeneCount(); g++)
      std::cout << indi->GetGene(g)->AsString() << std::endl;
  }
  else
  {
    /*
    **  GETTING EA PARAMETERS
    */

    CHECKPOINT("Getting EA Parameters");
    long num_runs = GetParameter("numRuns", params, 30);
    long front_gens = GetParameter("frontChangeGens", params, 1);
    long parent_count = GetParameter("lambdaCount", params, 1) * 2;
    long max_evals = GetParameter("evalMax", params, parent_count / 2);
    auto surv_strat = GetParameter("survivalStrategy", params, "Comma");
    double penalty = GetParameter("fitPenalty", params, 0);

    CHECKPOINT("Starting runs");
    Population best_sol;
    for (long run = 0; run < num_runs; run++)
    {
      WriteToLogFile(log_file, "\nRun " + std::to_string(run));
      std::cout << "Run " + std::to_string(run) << std::endl;
      // Start with a random population
      auto population = InitializePopulation(engine, params, problem);
      
      // Initialize our trackers for this run
      long front_tracker = 0;
      Population best_front;
      long eval_count = 0;
      long counter = 0;
      do
      {
        auto parents = ParentSelection(engine, params, population);
        if (surv_strat == "Comma")
          population.clear();
        
        CHECKPOINT("ChildGeneration");
        auto mutate_rate = ModifyMutationRate(params, population);
        for (long i = 0; i < parent_count / 2 && ((2 * i) + 1) < static_cast<long>(parents.size()); i++)
        {
          auto child = Recombine(params, problem, parents.at(2 * i), parents.at((2 * i) + 1));
          child->Mutate(engine, params, mutate_rate);
          if (penalty == 0 && !child->IsValid(params))
            child->Repair(engine, params);

          child->CalculateFitness(params);
          eval_count++;

          population.emplace_back(child);
        }
        population = SurvivalSelection(engine, params, population);

        // Gather data about this generation
        auto fronts = CreateParetoFronts(params, population);
        bool same = true;
        if (fronts.at(0).size() != best_front.size())
          same = false;
        else
        {
          for (long f = 0; f < static_cast<long>(best_front.size()) && same; f++)
          {
            bool found = false;
            for (long i = 0; i < static_cast<long>(best_front.size()) && !found; i++)
              found = (fronts.at(0).at(f) == best_front.at(i));
            if (!found)
              same = false;
          }
        }
        if (same)
          front_tracker++;
        else
        {
          best_front = fronts.at(0);
          front_tracker = 0;
        }

        WriteToLogFile(log_file, eval_count, params, population);
        if (++counter >= 10)
        {
          counter = 0;
          std::cout << "  " << eval_count << " evaluations completed." << std::endl;
        }

      } while (front_tracker < front_gens && (max_evals > 0 && eval_count < max_evals));

      if (max_evals > 0 && eval_count < max_evals)
        WriteToLogFile(log_file, max_evals, params, population);
    
      // Get the best solution for this run
      // Organize the population into fronts
      auto fronts = CreateParetoFronts(params, population);
      auto sol = fronts.at(0);
      for (long i = 1; i < static_cast<long>(fronts.size()); i++)
      {
        if (Dominates(params, fronts.at(i), sol))
          sol = fronts.at(i);
      }

      // Compare it to the best solution for all the runs
      if (run == 0)
        best_sol = sol;
      else if (Dominates(params, sol, best_sol))
        best_sol = sol;
    }
    
    /*
    **  WRITE TO SOLUTION FILE
    */

    // Consider that we may not always find a solution,
    // but we should write it down if we do.
    WriteToSolutionFile(sol_file, best_sol);
    WriteToSolutionFile(vis_file, best_sol);
    /*
    **  END WRITE TO SOLUTION FILE
    */
  }

  /*
  **  END EXECUTION
  */

  return 0;
}

void WriteToSolutionFile(const std::string file_path, const Population& population)
{
  std::ofstream file(file_path.c_str(), std::ios_base::out);
  if (file.is_open())
  {
    // write message and close
    file << population.size() << std::endl;
    for (long i = 0; i < static_cast<long>(population.size()); i++)
      file << population.at(i)->GetSolutionString() << std::endl;
    file.close();
  }
}

void WriteToLogFile(const std::string file_path, const long evals, const Parameters& params, const Population& population)
{
  auto avg = GetAverageFitness(params, population);
  auto best = GetBestFitness(params, population);
  std::string msg = std::to_string(evals);
  for (long i = 0; i < static_cast<long>(avg.size()); i++)
    msg.append("\t" + std::to_string(avg.at(i)) + "\t" + std::to_string(best.at(i)));
  WriteToLogFile(file_path, msg);
}
