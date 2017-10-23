#include "Recombination.h"

Individual Recombine(const Parameters& params, const Problem& problem, const Individual& mom, const Individual& dad)
{
  CHECKPOINT("Recombine");
  static long n = GetParameter("nPtCrossOver", params, 1);
  return NPointCrossOver(mom, dad, n, problem);
}

Individual NPointCrossOver(const Individual& mom, const Individual& dad, const long n, const Problem& problem)
{
  CHECKPOINT("NPointCrossOver");
  
  // N-Point Crossover
  auto gene_count = mom->GetGeneCount();
  long alleles = gene_count / (n + 1);
  alleles = (alleles <= 0) ? 1 : alleles;
  auto child = CreateNewIndividual(problem);

  for (long j = 0; j < n + 1; j++)
  {
    auto parent = (j % 2 == 0) ? mom : dad;
    for (long i = 0; i < alleles && ((j * alleles) + i) < gene_count; i++)
    {
      long idx = (j * alleles) + i;
      child->SetGene(idx, parent->GetGene(idx));
    }
  }
  return child;
}