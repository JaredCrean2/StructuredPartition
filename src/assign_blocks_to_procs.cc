#include "assign_blocks_to_procs.h"

#include <limits>
#include <algorithm>

namespace structured_part {

double computeTotalWeight(const std::vector<SplitBlock>& blocks)
{
  double weight_on_proc = 0.0;
  for (const SplitBlock& block : blocks)
    weight_on_proc += block.weight;

  return weight_on_proc;
}

UInt getProcWithMinWeightAndDifferentParent(const std::vector<std::vector<SplitBlock>>& blocks_on_proc, const std::shared_ptr<MeshBlock>& meshblock)
{
  UInt nprocs = blocks_on_proc.size();
  UInt min_proc = -1;
  double min_weight = std::numeric_limits<double>::max();
  for (UInt i=0; i < nprocs; ++i)
  {
    bool already_have_meshblock = false;
    for (const SplitBlock& split_block: blocks_on_proc[i])
    {
      if (split_block.meshblock == meshblock)
      {
        already_have_meshblock = true;
        break;
      }
    }

    if (already_have_meshblock)
    {
      continue;
    }

    double weight_on_proc = computeTotalWeight(blocks_on_proc[i]);
    if (weight_on_proc < min_weight)
    {
      min_proc = i;
      min_weight = weight_on_proc;
    }
  }

  if (min_proc == UInt(-1))
  {
    throw std::runtime_error("unable to assign block to proc");
  }

  return min_proc;
}

std::vector<std::vector<SplitBlock>> assignBlocksToProcs(std::vector<SplitBlock> split_blocks, UInt nprocs)
{
  auto sortByWeight = [](const SplitBlock& lhs, const SplitBlock& rhs)
  {
    return lhs.weight < rhs.weight;
  };

  std::sort(split_blocks.begin(), split_blocks.end(), sortByWeight);
  
  std::vector<std::vector<SplitBlock>> blocks_on_proc(nprocs);
  while (!split_blocks.empty())
  {
    const SplitBlock& next_block = split_blocks.back();
    UInt min_proc = getProcWithMinWeightAndDifferentParent(blocks_on_proc, next_block.meshblock);
    blocks_on_proc[min_proc].push_back(next_block);
    split_blocks.pop_back();
  }

  return blocks_on_proc;
}

}