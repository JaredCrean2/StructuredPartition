#include "gtest/gtest.h"

#include "pre_split.h"
#include "statistics.h"
#include "utils.h"


TEST(Presplit, Stats1Block100Procs)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 400, 400, 1)};
  
  for (UInt nprocs=1; nprocs < 100; ++nprocs)
  {
    std::cout << "\nnprocs = " << nprocs << std::endl;
    auto blocks_on_procs = preSplit(mesh_blocks, nprocs);
    checkDecompositionValid(mesh_blocks, blocks_on_procs);


    DecompStats stats = computeDecompStats(blocks_on_procs);
    std::cout << stats << std::endl;
    //printHistogram(std::cout, stats);
  }
}

TEST(Presplit, Stats4Blocks100Procs)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(1, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(2, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(3, 100, 100, 1)};
  
  for (UInt nprocs=1; nprocs < 100; ++nprocs)
  {
    std::cout << "\nnprocs = " << nprocs << std::endl;
    auto blocks_on_procs = preSplit(mesh_blocks, nprocs);
    checkDecompositionValid(mesh_blocks, blocks_on_procs);


    DecompStats stats = computeDecompStats(blocks_on_procs);
    std::cout << stats << std::endl;
    //printHistogram(std::cout, stats);
  }
}

TEST(Presplit, Stats4Blocks100ProcsOneSmallBlock)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 101, 100, 1),
                                                         std::make_shared<MeshBlock>(1, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(2, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(3, 10, 10, 1)};
  
  for (UInt nprocs=1; nprocs < 100; ++nprocs)
  {
    std::cout << "\nnprocs = " << nprocs << std::endl;
    auto blocks_on_procs = preSplit(mesh_blocks, nprocs);
    checkDecompositionValid(mesh_blocks, blocks_on_procs);

    DecompStats stats = computeDecompStats(blocks_on_procs);
    std::cout << stats << std::endl;
    //printHistogram(std::cout, stats);
  }
}