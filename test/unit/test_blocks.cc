#include "gtest/gtest.h"
#include "blocks.h"

using namespace structured_part;

namespace {

std::array<UInt, 3> make_array(const std::array<UInt, 3>& vals)  
{
  return vals;
}
}

TEST(MeshBlock, Fields)
{
  MeshBlock block(1, 3, 4, 5);
  EXPECT_EQ(block.block_id, 1);
  EXPECT_EQ(block.element_counts, make_array({3, 4, 5}));
  EXPECT_EQ(block.weight, 3*4*5);
}

TEST(SplitBlock, Fields)
{
   auto block = std::make_shared<MeshBlock>(1, 3, 4, 5);

   SplitBlock splitBlock(block, 1, 2, 3, 1, 1, 1);
   EXPECT_EQ(splitBlock.meshblock, block);
   EXPECT_EQ(splitBlock.element_counts, make_array({1, 2, 3}));
   EXPECT_EQ(splitBlock.mesh_offsets, make_array({1, 1, 1}));
}

TEST(SplitBlock, ConstructFromMeshBlock)
{
   auto block = std::make_shared<MeshBlock>(1, 3, 4, 5);

   SplitBlock splitBlock(block);
   EXPECT_EQ(splitBlock.meshblock, block);
   EXPECT_EQ(splitBlock.element_counts, block->element_counts);
   EXPECT_EQ(splitBlock.mesh_offsets, make_array({0, 0, 0}));
}

TEST(SplitBlock, EqualityOperatorMeshBlockNotSame)
{
  auto meshblock1 = std::make_shared<MeshBlock>(1, 3, 4, 5);
  auto meshblock2 = std::make_shared<MeshBlock>(1, 3, 4, 5);

  SplitBlock lhs(meshblock1, {1, 1, 1}, {0, 0, 0});
  SplitBlock rhs(meshblock2, {1, 1, 1}, {0, 0, 0});

  EXPECT_TRUE(lhs != rhs);
  EXPECT_FALSE(lhs == rhs);
}

TEST(SplitBlock, EqualityOperatorElementCountsNotSame)
{
  auto meshblock1 = std::make_shared<MeshBlock>(1, 3, 4, 5);

  SplitBlock lhs(meshblock1, {1, 1, 1}, {0, 0, 0});
  SplitBlock rhs(meshblock1, {1, 1, 2}, {0, 0, 0});

  EXPECT_TRUE(lhs != rhs);
  EXPECT_FALSE(lhs == rhs);
}

TEST(SplitBlock, EqualityOperatorMeshOffsetNotSame)
{
  auto meshblock1 = std::make_shared<MeshBlock>(1, 3, 4, 5);

  SplitBlock lhs(meshblock1, {1, 1, 1}, {0, 0, 0});
  SplitBlock rhs(meshblock1, {1, 1, 1}, {0, 1, 0});

  EXPECT_TRUE(lhs != rhs);
  EXPECT_FALSE(lhs == rhs);
}

// To test: for each direction
//   - 0, num_elem, middle value

TEST(SplitBlock, SplitBlockXZeroElements)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   EXPECT_ANY_THROW(splitBlock(split_block, SplitDirection::I, 0));
}

TEST(SplitBlock, SplitBlockX1Element)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   auto [left_block, right_block] = splitBlock(split_block, SplitDirection::I, 1);

   EXPECT_EQ(left_block.meshblock, block);
   EXPECT_EQ(left_block.element_counts, make_array({1, 5, 6}));
   EXPECT_EQ(left_block.mesh_offsets,   make_array({0, 0, 0}));
   EXPECT_NEAR(left_block.weight, block->weight * double(1*5*6)/(4*5*6), 1e-13);

   EXPECT_EQ(right_block.meshblock, block);
   EXPECT_EQ(right_block.element_counts, make_array({3, 5, 6}));
   EXPECT_EQ(right_block.mesh_offsets,   make_array({1, 0, 0}));
   EXPECT_NEAR(right_block.weight, block->weight*double(3*5*6)/(4*5*6), 1e-13);
}

TEST(SplitBlock, SplitBlockXAllElements)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   EXPECT_ANY_THROW(splitBlock(split_block, SplitDirection::I, split_block.element_counts[0]));
}


TEST(SplitBlock, SplitBlockYZeroElements)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   EXPECT_ANY_THROW(splitBlock(split_block, SplitDirection::J, 0));
}

TEST(SplitBlock, SplitBlockY1Element)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   auto [left_block, right_block] = splitBlock(split_block, SplitDirection::J, 2);

   EXPECT_EQ(left_block.meshblock, block);
   EXPECT_EQ(left_block.element_counts, make_array({4, 2, 6}));
   EXPECT_EQ(left_block.mesh_offsets,   make_array({0, 0, 0}));

   EXPECT_EQ(right_block.meshblock, block);
   EXPECT_EQ(right_block.element_counts, make_array({4, 3, 6}));
   EXPECT_EQ(right_block.mesh_offsets,   make_array({0, 2, 0}));
}

TEST(SplitBlock, SplitBlockYAllElements)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   EXPECT_ANY_THROW(splitBlock(split_block, SplitDirection::J, split_block.element_counts[1]));
}


TEST(SplitBlock, SplitBlockZZeroElements)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   EXPECT_ANY_THROW(splitBlock(split_block, SplitDirection::K, 0));
}

TEST(SplitBlock, SplitBlockZ1Element)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   auto [left_block, right_block] = splitBlock(split_block, SplitDirection::K, 4);

   EXPECT_EQ(left_block.meshblock, block);
   EXPECT_EQ(left_block.element_counts, make_array({4, 5, 4}));
   EXPECT_EQ(left_block.mesh_offsets,   make_array({0, 0, 0}));

   EXPECT_EQ(right_block.meshblock, block);
   EXPECT_EQ(right_block.element_counts, make_array({4, 5, 2}));
   EXPECT_EQ(right_block.mesh_offsets,   make_array({0, 0, 4}));
}

TEST(SplitBlock, SplitBlockZAllElements)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});

   EXPECT_ANY_THROW(splitBlock(split_block, SplitDirection::K, split_block.element_counts[2]));
}

TEST(SplitBlock, SplitBlockByWeightMidPoint)
{
   auto block = std::make_shared<MeshBlock>(1, 4, 5, 6);
   SplitBlock split_block(block, {4, 5, 6}, {0, 0, 0});   
   auto [left_block, right_block] = splitBlock(split_block, 0.5);

   EXPECT_EQ(left_block.meshblock, block);
   EXPECT_EQ(left_block.element_counts, make_array({4, 5, 3}));
   EXPECT_EQ(left_block.mesh_offsets, make_array({0, 0, 0}));

   EXPECT_EQ(right_block.meshblock, block);
   EXPECT_EQ(right_block.element_counts, make_array({4, 5, 3}));
   EXPECT_EQ(right_block.mesh_offsets, make_array({0, 0, 3}));
}

TEST(SplitBlock, RoundSmallerBlockDown)
{
  // This rounding behavior is important when recursively dividing
  // blocks because sometimes the remainder block can be too large.
  // One example is if a 400x400x1 block gets split 57 ways.  If
  // the remainder block is 8x400x1 it is larger than any of the 56
  // blocks in the main block, causing load imblanace.  Better to have
  // it be 7x400x1
  auto block = std::make_shared<MeshBlock>(1, 400, 400, 1);
  SplitBlock split_block(block);  
  double weight = 56.0/57;
  auto [left_block, right_block] = splitBlock(split_block, weight);
  EXPECT_EQ(left_block.element_counts[0], 393);
  EXPECT_EQ(right_block.element_counts[0], 7);  
}