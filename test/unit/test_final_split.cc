#include "gtest/gtest.h"
#include "final_split.h"
#include "statistics.h"
#include "utils.h"

TEST(FinalSplit, StatsSingleBlock)
{
  UInt nprocs = 7;
  double load_balance_factor = 0.1;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 100, 100, 1)};
  auto blocks_on_procs = finalSplit(mesh_blocks, nprocs, load_balance_factor);
  checkDecompositionValid(mesh_blocks, blocks_on_procs);
  checkLoadBalance(blocks_on_procs, load_balance_factor);

  DecompStats stats = computeDecompStats(blocks_on_procs);
  std::cout << stats << std::endl;
  printPerProcessStats(std::cout, stats);
  printHistogram(std::cout, stats);
}

TEST(FinalSplit, Stats4Blocks)
{
  double load_balance_factor = 0.1;
  UInt nprocs = 7;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(1, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(2, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(3, 100, 100, 1)};
  
  auto blocks_on_procs = finalSplit(mesh_blocks, nprocs, load_balance_factor);
  checkDecompositionValid(mesh_blocks, blocks_on_procs);
  checkLoadBalance(blocks_on_procs, load_balance_factor);

  DecompStats stats = computeDecompStats(blocks_on_procs);
  std::cout << stats << std::endl;
  printPerProcessStats(std::cout, stats);
  printHistogram(std::cout, stats);
}