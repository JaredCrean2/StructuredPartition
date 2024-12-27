#include "gtest/gtest.h"
#include "pre_split.h"
#include "statistics.h"
#include "final_split.h"
#include "utils.h"

using namespace structured_part;


TEST(Presplit, SplitSingleBlockOneProc)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 1, 1, 1);

  std::vector<SplitBlock> split_blocks = recursivelySplitBlock(mesh_block, 1);
  EXPECT_EQ(split_blocks.size(), 1U);
  EXPECT_EQ(split_blocks[0].element_counts, mesh_block->element_counts);
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight);
}

TEST(Presplit, SplitSingleBlockOneProc2x2x2)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 2, 2, 2);

  std::vector<SplitBlock> split_blocks = recursivelySplitBlock(mesh_block, 1);
  EXPECT_EQ(split_blocks.size(), 1U);
  EXPECT_EQ(split_blocks[0].element_counts, mesh_block->element_counts);
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight);
}

TEST(Presplit, SplitSingleBlockTwoProcs2x1x1)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 2, 1, 1);

  std::vector<SplitBlock> split_blocks = recursivelySplitBlock(mesh_block, 2);
  EXPECT_EQ(split_blocks.size(), 2U);

  EXPECT_EQ(split_blocks[0].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight/2);

  EXPECT_EQ(split_blocks[1].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[1].mesh_offsets, make_array({1, 0, 0}));
  EXPECT_EQ(split_blocks[1].weight, mesh_block->weight/2);  
}

TEST(Presplit, SplitSingleBlockFourProcs2x2x1)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 2, 2, 1);

  std::vector<SplitBlock> split_blocks = recursivelySplitBlock(mesh_block, 4);
  EXPECT_EQ(split_blocks.size(), 4U);

  EXPECT_EQ(split_blocks[0].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight/4);

  EXPECT_EQ(split_blocks[1].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[1].mesh_offsets, make_array({0, 1, 0}));
  EXPECT_EQ(split_blocks[1].weight, mesh_block->weight/4);

  EXPECT_EQ(split_blocks[2].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[2].mesh_offsets, make_array({1, 0, 0}));
  EXPECT_EQ(split_blocks[2].weight, mesh_block->weight/4);

  EXPECT_EQ(split_blocks[3].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[3].mesh_offsets, make_array({1, 1, 0}));
  EXPECT_EQ(split_blocks[3].weight, mesh_block->weight/4);  
}


TEST(Presplit, SplitSingleBlockThreeProcs2x2x1)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 2, 2, 1);

  std::vector<SplitBlock> split_blocks = recursivelySplitBlock(mesh_block, 3);
  EXPECT_EQ(split_blocks.size(), 3U);

  std::cout << "split_blocks = \n" << split_blocks << std::endl;
  EXPECT_EQ(split_blocks[0].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight/4);

  EXPECT_EQ(split_blocks[1].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[1].mesh_offsets, make_array({0, 1, 0}));
  EXPECT_EQ(split_blocks[1].weight, mesh_block->weight/4);

  EXPECT_EQ(split_blocks[2].element_counts, make_array({1, 2, 1}));
  EXPECT_EQ(split_blocks[2].mesh_offsets, make_array({1, 0, 0}));
  EXPECT_EQ(split_blocks[2].weight, mesh_block->weight/2); 
}


//-----------------------------------------------------------------------------
// Test computeNumSubBlocks

TEST(Presplit, NumSubBlocksSingleBlock1Proc)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 2, 2, 1)};

  std::vector<UInt> num_sub_blocks = computeNumSubBlocks(mesh_blocks, 1);
  EXPECT_EQ(num_sub_blocks.size(), mesh_blocks.size());
  EXPECT_EQ(num_sub_blocks[0], 1);
}

TEST(Presplit, NumSubBlocksSingleBlock2Procs)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 2, 2, 1)};

  std::vector<UInt> num_sub_blocks = computeNumSubBlocks(mesh_blocks, 2);
  EXPECT_EQ(num_sub_blocks.size(), mesh_blocks.size());
  EXPECT_EQ(num_sub_blocks[0], 2);
}

TEST(Presplit, NumSubBlocks2Blocks2Procs)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 2, 2, 1),
                                                         std::make_shared<MeshBlock>(1, 2, 2, 1)};

  std::vector<UInt> num_sub_blocks = computeNumSubBlocks(mesh_blocks, 2);
  EXPECT_EQ(num_sub_blocks.size(), mesh_blocks.size());
  EXPECT_EQ(num_sub_blocks[0], 1);
  EXPECT_EQ(num_sub_blocks[1], 1);
}

TEST(Presplit, NumSubBlocks2Blocks2ProcsUneven)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1),
                                                         std::make_shared<MeshBlock>(1, 2, 2, 1)};

  std::vector<UInt> num_sub_blocks = computeNumSubBlocks(mesh_blocks, 2);
  EXPECT_EQ(num_sub_blocks.size(), mesh_blocks.size());
  EXPECT_EQ(num_sub_blocks[0], 2);
  EXPECT_EQ(num_sub_blocks[1], 1);
}

TEST(Presplit, NumSubBlocks2Blocks3ProcsUneven)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1),
                                                         std::make_shared<MeshBlock>(1, 2, 2, 1)};

  std::vector<UInt> num_sub_blocks = computeNumSubBlocks(mesh_blocks, 3);
  EXPECT_EQ(num_sub_blocks.size(), mesh_blocks.size());
  EXPECT_EQ(num_sub_blocks[0], 3);
  EXPECT_EQ(num_sub_blocks[1], 1);
}


// ----------------------------------------------------------------------------
// Test assignBlocksToProcs


TEST(Presplit, AssignBlocksToProcs1Block1Proc)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1)};
  std::vector<SplitBlock> split_blocks = {SplitBlock(mesh_blocks[0], mesh_blocks[0]->element_counts, {0, 0, 0})};
  UInt nprocs = 1;

  std::vector<std::vector<SplitBlock>> blocks_on_procs = assignBlocksToProcs(split_blocks, nprocs);
  EXPECT_EQ(blocks_on_procs.size(), nprocs);
  EXPECT_EQ(blocks_on_procs[0].size(), 1);
  EXPECT_EQ(blocks_on_procs[0][0], split_blocks[0]);
}

TEST(Presplit, AssignBlocksToProcs1Block2Proc)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1)};
  std::vector<SplitBlock> split_blocks = {SplitBlock(mesh_blocks[0], mesh_blocks[0]->element_counts, {0, 0, 0})};
  UInt nprocs = 2;

  std::vector<std::vector<SplitBlock>> blocks_on_procs = assignBlocksToProcs(split_blocks, nprocs);
  EXPECT_EQ(blocks_on_procs.size(), nprocs);
  EXPECT_EQ(blocks_on_procs[0].size(), 1);
  EXPECT_EQ(blocks_on_procs[0][0], split_blocks[0]);

  EXPECT_EQ(blocks_on_procs[1].size(), 0);
}

TEST(Presplit, AssignBlocksToProcs2Block2Proc)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1)};
  std::vector<SplitBlock> split_blocks = {SplitBlock(mesh_blocks[0], {6, 10, 1}, {0, 0, 0}),
                                          SplitBlock(mesh_blocks[0], {4, 10, 1}, {6, 0, 0})};
  UInt nprocs = 2;

  std::vector<std::vector<SplitBlock>> blocks_on_procs = assignBlocksToProcs(split_blocks, nprocs);
  EXPECT_EQ(blocks_on_procs.size(), nprocs);
  EXPECT_EQ(blocks_on_procs[0].size(), 1);
  EXPECT_EQ(blocks_on_procs[0][0], split_blocks[0]);

  EXPECT_EQ(blocks_on_procs[1].size(), 1);
  EXPECT_EQ(blocks_on_procs[1][0], split_blocks[1]);
}

TEST(Presplit, AssignBlocksToProcs3Block2Proc)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1),
                                                         std::make_shared<MeshBlock>(1, 10, 10, 1)};

  std::vector<SplitBlock> split_blocks = {SplitBlock(mesh_blocks[0], {5, 10, 1}, {0, 0, 0}),
                                          SplitBlock(mesh_blocks[0], {2, 10, 1}, {5, 0, 0}),
                                          SplitBlock(mesh_blocks[1], {3, 10, 1}, {7, 0, 0})};
  UInt nprocs = 2;

  std::vector<std::vector<SplitBlock>> blocks_on_procs = assignBlocksToProcs(split_blocks, nprocs);
  EXPECT_EQ(blocks_on_procs.size(), nprocs);
  EXPECT_EQ(blocks_on_procs[0].size(), 1);
  EXPECT_EQ(blocks_on_procs[0][0], split_blocks[0]);

  EXPECT_EQ(blocks_on_procs[1].size(), 2);
  EXPECT_EQ(blocks_on_procs[1][0], split_blocks[2]);
  EXPECT_EQ(blocks_on_procs[1][1], split_blocks[1]);
}

TEST(Presplit, AssignBlocksToProcs3Block2ProcThrow)
{
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 10, 10, 1)};

  std::vector<SplitBlock> split_blocks = {SplitBlock(mesh_blocks[0], {5, 10, 1}, {0, 0, 0}),
                                          SplitBlock(mesh_blocks[0], {2, 10, 1}, {5, 0, 0}),
                                          SplitBlock(mesh_blocks[0], {3, 10, 1}, {7, 0, 0})};
  UInt nprocs = 2;
  EXPECT_ANY_THROW(assignBlocksToProcs(split_blocks, nprocs));
}


//-----------------------------------------------------------------------------
// Test: end-to-end decomposition

TEST(Presplit, StatsSingleBlock)
{
  UInt nprocs = 7;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 100, 100, 1)};
  auto blocks_on_procs = preSplit(mesh_blocks, nprocs);
  checkDecompositionValid(mesh_blocks, blocks_on_procs);

  DecompStats stats = computeDecompStats(blocks_on_procs);
  std::cout << stats << std::endl;
  printHistogram(std::cout, stats);
}

TEST(Presplit, Stats4Blocks)
{
  UInt nprocs = 7;
  std::vector<std::shared_ptr<MeshBlock>> mesh_blocks = {std::make_shared<MeshBlock>(0, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(1, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(2, 100, 100, 1),
                                                         std::make_shared<MeshBlock>(3, 100, 100, 1)};
  
  auto blocks_on_procs = preSplit(mesh_blocks, nprocs);
  checkDecompositionValid(mesh_blocks, blocks_on_procs);

  DecompStats stats = computeDecompStats(blocks_on_procs);
  std::cout << stats << std::endl;
  printPerProcessStats(std::cout, stats);
  printHistogram(std::cout, stats);
}