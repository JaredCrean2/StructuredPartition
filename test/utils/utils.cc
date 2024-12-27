#include "utils.h"
#include "gtest/gtest.h"

void checkDecompositionValid(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks,
                             const std::vector<std::vector<SplitBlock>>& split_blocks)
{
#ifndef NDEBUG
  UInt idx = 0;
#endif
  std::vector<Array3D> element_usage_counts;
  for (auto& mesh_block : mesh_blocks)
  {
    assert(mesh_block->block_id == idx++);
    element_usage_counts.emplace_back(mesh_block->element_counts);
    element_usage_counts.back().set(0);
  }

  for (UInt proc=0; proc < split_blocks.size(); ++proc)
    for (const SplitBlock& split_block : split_blocks[proc])
    {
      Array3D& element_usage_count = element_usage_counts[split_block.meshblock->block_id];

      for (UInt i=0; i < split_block.element_counts[0]; ++i)
        for (UInt j=0; j < split_block.element_counts[1]; ++j)
          for (UInt k=0; k < split_block.element_counts[2]; ++k)
          {
            element_usage_count(i + split_block.mesh_offsets[0],
                                j + split_block.mesh_offsets[1],
                                k + split_block.mesh_offsets[2]) += 1;
          }
    }

  for (const Array3D& element_usage : element_usage_counts)
    for (UInt i=0; i < element_usage.extent(0); ++i)
      for (UInt j=0; j < element_usage.extent(1); ++j)
        for (UInt k=0; k < element_usage.extent(2); ++k)
          EXPECT_EQ(element_usage(i, j, k), 1);
}

void checkLoadBalance(const std::vector<std::vector<SplitBlock>>& blocks_on_procs, double load_balance_factor)
{
  double avg_weight_per_proc = 0.0;
  for (UInt proc=0; proc < blocks_on_procs.size(); ++proc)
    for (const SplitBlock& split_block : blocks_on_procs[proc])
      avg_weight_per_proc += split_block.weight;

  avg_weight_per_proc /= blocks_on_procs.size();

  for (UInt proc=0; proc < blocks_on_procs.size(); ++proc)
  {
    double weight_on_proc = 0.0;
    for (const SplitBlock& split_block : blocks_on_procs[proc])
      weight_on_proc += split_block.weight;

    EXPECT_TRUE(weight_on_proc <= avg_weight_per_proc * (1 + load_balance_factor));
  }
}