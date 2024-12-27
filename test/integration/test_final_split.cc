#include "gtest/gtest.h"
#include "ProjectDefs.h"
#include "final_split.h"
#include "statistics.h"
#include "utils.h"

TEST(FinalSplit, Stats1Block100Procs)
{
  double load_balance_factor = 0.1;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 400, 400, 1)};
  
  for (UInt nprocs=1; nprocs < 100; ++nprocs)
  {
    std::cout << "\nnprocs = " << nprocs << std::endl;
    auto blocks_on_procs = finalSplit(mesh_blocks, nprocs, load_balance_factor);
    checkDecompositionValid(mesh_blocks, blocks_on_procs);
    checkLoadBalance(blocks_on_procs, load_balance_factor);


    DecompStats stats = computeDecompStats(blocks_on_procs);
    std::cout << stats << std::endl;
    //printHistogram(std::cout, stats);
  }
}


TEST(FinalSplit, Stats4Blocks100Procs)
{
  double load_balance_factor = 0.1;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(1, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(2, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(3, 100, 100, 1)};
  
  for (UInt nprocs=1; nprocs < 100; ++nprocs)
  {
    std::cout << "\nnprocs = " << nprocs << std::endl;
    auto blocks_on_procs = finalSplit(mesh_blocks, nprocs, load_balance_factor);
    checkDecompositionValid(mesh_blocks, blocks_on_procs);
    checkLoadBalance(blocks_on_procs, load_balance_factor);


    DecompStats stats = computeDecompStats(blocks_on_procs);
    std::cout << stats << std::endl;
    //printHistogram(std::cout, stats);
  }
}

TEST(FinalSplit, Stats4Blocks100ProcsOneSmallBlock)
{
  double load_balance_factor = 0.1;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 101, 100, 1),
                                                         std::make_shared<MeshBlock>(1, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(2, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(3, 10, 10, 1)};
  
  for (UInt nprocs=1; nprocs < 100; ++nprocs)
  {
    std::cout << "\nnprocs = " << nprocs << std::endl;
    auto blocks_on_procs = finalSplit(mesh_blocks, nprocs, load_balance_factor);
    checkDecompositionValid(mesh_blocks, blocks_on_procs);
    checkLoadBalance(blocks_on_procs, load_balance_factor);

    DecompStats stats = computeDecompStats(blocks_on_procs);
    std::cout << stats << std::endl;
    //printHistogram(std::cout, stats);
  }
}