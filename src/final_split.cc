#include "final_split.h"
#include "assign_blocks_to_procs.h"
#include "pre_split.h"

namespace structured_part {

std::pair<UInt, double> computeMostOverWeightProc(const std::vector<std::vector<SplitBlock>>& blocks_on_procs)
{
  UInt most_overweight_proc = 0;
  double max_weight_per_proc = 0.0;
  for (UInt proc=0; proc < blocks_on_procs.size(); ++proc)
  {
    double weight_on_proc = 0.0;
    for (const SplitBlock& split_block : blocks_on_procs[proc])
    {
      weight_on_proc += split_block.weight;
    }

    if (weight_on_proc > max_weight_per_proc)
    {
      max_weight_per_proc = weight_on_proc;
      most_overweight_proc = proc;
    }
  }

  return std::make_pair(most_overweight_proc, max_weight_per_proc);
}

SplitBlock* findLargestBlock(std::vector<SplitBlock>& blocks, const std::map<std::shared_ptr<MeshBlock>, UInt>& block_split_counts, UInt max_splits_per_block)
{
  if (blocks.size() == 0)
    throw std::runtime_error("found zero sized vector");


  UInt max_block = -1;
  double max_weight = 0.0;
  for (UInt i=0; i < blocks.size(); ++i)
  {
    if (blocks[i].weight > max_weight && block_split_counts.at(blocks[i].meshblock) < max_splits_per_block)
    {
      max_block = i;
      max_weight = blocks[i].weight;
    }
  }

  if (max_block == UInt(-1))
    throw std::runtime_error("could not find a block to split");

  return &(blocks[max_block]);
}

std::vector<SplitBlock> flattenSplitBlocks(std::vector<std::vector<SplitBlock>> blocks_on_procs)
{
  std::vector<SplitBlock> split_blocks;
  for (UInt proc=0; proc < blocks_on_procs.size(); ++proc)
  {
    for (const SplitBlock& split_block : blocks_on_procs[proc])
    {
      split_blocks.push_back(split_block);
    }
  }

  return split_blocks;
}


void splitUntilLoadBalanced(std::vector<std::vector<SplitBlock>>& blocks_on_procs, UInt nprocs, double avg_weight_per_proc, double load_balance_factor)
{
  std::cout << "splitting until load balanced, avg weight per proc = " << avg_weight_per_proc << std::endl;

  // avoid spitting into too small pieces
  // The value is a little bit arbitrary
  constexpr double max_split_fraction = 0.8;

  //TODO: its unfortunate we have to use a map for this
  std::map<std::shared_ptr<MeshBlock>, UInt> block_split_counts;
  for (UInt i=0; i < blocks_on_procs.size(); ++i)
    for (const SplitBlock& split_block : blocks_on_procs[i])
      block_split_counts[split_block.meshblock]++;

  auto [most_overweight_proc, max_weight_per_proc ] = computeMostOverWeightProc(blocks_on_procs);
  while (max_weight_per_proc > avg_weight_per_proc * (1 + load_balance_factor))
  {
    // this is a trick to avoid having to find the largest_block in the flattened array
    SplitBlock* largest_block = findLargestBlock(blocks_on_procs[most_overweight_proc], block_split_counts, nprocs);

    double split_fraction = (max_weight_per_proc - avg_weight_per_proc) / largest_block->weight;
    split_fraction = std::min(split_fraction, max_split_fraction);

    auto [left_block, right_block] = splitBlock(*largest_block, split_fraction);
    *largest_block = left_block;

    std::vector<SplitBlock> split_blocks = flattenSplitBlocks(blocks_on_procs);
    split_blocks.push_back(right_block);

    block_split_counts[right_block.meshblock]++;

    blocks_on_procs = assignBlocksToProcs(split_blocks, nprocs);
    std::tie(most_overweight_proc, max_weight_per_proc) = computeMostOverWeightProc(blocks_on_procs); 
  }
}


std::vector<std::vector<SplitBlock>> finalSplit(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs, double load_balance_factor)
{
  double avg_weight_per_proc = 0.0;
  for (const auto& mesh_block : mesh_blocks)
    avg_weight_per_proc += mesh_block->weight;
  avg_weight_per_proc /= nprocs;

  
  std::vector<std::vector<SplitBlock>> blocks_on_procs = preSplit(mesh_blocks, nprocs);
  splitUntilLoadBalanced(blocks_on_procs, nprocs, avg_weight_per_proc, load_balance_factor);

  return blocks_on_procs;
}

}