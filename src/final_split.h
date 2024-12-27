#include "ProjectDefs.h"
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

SplitBlock* findLargestBlock(std::vector<SplitBlock>& blocks)
{
  if (blocks.size() == 0)
    throw std::runtime_error("found zero sized vector");


  UInt max_block = 0;
  double max_weight = 0.0;
  for (UInt i=0; i < blocks.size(); ++i)
  {
    if (blocks[i].weight > max_weight)
    {
      max_block = i;
      max_weight = blocks[i].weight;
    }
  }

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


void splitUntilLoadBalanced(std::vector<std::vector<SplitBlock>> blocks_on_procs, UInt nprocs, double avg_weight_per_proc, double load_balance_factor)
{

  auto [most_overweight_proc, max_weight_per_proc ] = computeMostOverWeightProc(blocks_on_procs);
  while (max_weight_per_proc > avg_weight_per_proc * (1 + load_balance_factor))
  {
    // this is a trick to avoid having to find the largest_block in the flattened array
    SplitBlock* largest_block = findLargestBlock(blocks_on_procs[most_overweight_proc]);

    //TODO: maybe instead of splitting in half, split off the amount of work that would make this
    //      proc perfectly balanced?
    std::cout << "splitting block " << *largest_block << std::endl;
    auto [left_block, right_block] = splitBlock(*largest_block);
    *largest_block = left_block;

    std::vector<SplitBlock> split_blocks = flattenSplitBlocks(blocks_on_procs);
    split_blocks.push_back(right_block);

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
