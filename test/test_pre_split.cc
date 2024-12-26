#include "gtest/gtest.h"
#include "pre_split.h"

using namespace structured_part;

namespace {

std::array<UInt, 3> make_array(const std::array<UInt, 3>& vals)  
{
  return vals;
}
}

TEST(Presplit, SplitSingleBlockOneProc)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 1, 1, 1);

  std::vector<SplitBlock> split_blocks = splitBlock(mesh_block, 1);
  EXPECT_EQ(split_blocks.size(), 1U);
  EXPECT_EQ(split_blocks[0].element_counts, mesh_block->element_counts);
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight);
}

TEST(Presplit, SplitSingleBlockOneProc2x2x2)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 2, 2, 2);

  std::vector<SplitBlock> split_blocks = splitBlock(mesh_block, 1);
  EXPECT_EQ(split_blocks.size(), 1U);
  EXPECT_EQ(split_blocks[0].element_counts, mesh_block->element_counts);
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight);
}

TEST(Presplit, SplitSingleBlockTwoProcs2x1x1)
{
  auto mesh_block = std::make_shared<MeshBlock>(0, 2, 1, 1);

  std::vector<SplitBlock> split_blocks = splitBlock(mesh_block, 2);
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

  std::vector<SplitBlock> split_blocks = splitBlock(mesh_block, 4);
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

  std::vector<SplitBlock> split_blocks = splitBlock(mesh_block, 3);
  EXPECT_EQ(split_blocks.size(), 3U);

  EXPECT_EQ(split_blocks[0].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[0].mesh_offsets, make_array({0, 0, 0}));
  EXPECT_EQ(split_blocks[0].weight, mesh_block->weight/4);

  EXPECT_EQ(split_blocks[1].element_counts, make_array({1, 2, 1}));
  EXPECT_EQ(split_blocks[1].mesh_offsets, make_array({1, 0, 0}));
  EXPECT_EQ(split_blocks[1].weight, mesh_block->weight/2);

  EXPECT_EQ(split_blocks[2].element_counts, make_array({1, 1, 1}));
  EXPECT_EQ(split_blocks[2].mesh_offsets, make_array({0, 1, 0}));
  EXPECT_EQ(split_blocks[2].weight, mesh_block->weight/4); 
}

