#include "pre_split.h"
#include <numeric>
#include <limits>
#include <algorithm>
#include <cmath>
#include "assign_blocks_to_procs.h"


namespace structured_part {

double computeAvgWorkPerProc(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs)
{
  auto sum_work = [](double sum, const std::shared_ptr<MeshBlock>& meshblock)
  {
    return sum + meshblock->weight;
  };

  return std::accumulate(mesh_blocks.begin(), mesh_blocks.end(), 0.0, sum_work) / nprocs;
}

UInt getMostUnderWeightBlock(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block)
{
  UInt block_most_under_weight = -1;
  double min_weight = std::numeric_limits<double>::max();
  for (UInt i=0; i < mesh_blocks.size(); ++i)
  {
    double weight_per_split_block = mesh_blocks[i]->weight / num_splits_per_block[i];

    if (num_splits_per_block[i] > 1 && weight_per_split_block < min_weight)
    {
      min_weight = weight_per_split_block;
      block_most_under_weight = i;
    }
  }

  if (block_most_under_weight == UInt(-1))
  {
    throw std::runtime_error("could not reduce the number of splits in any block");
  }

  return block_most_under_weight;
}

UInt getMostOverWeightBlock(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block, UInt max_splits_per_block)
{
  UInt block_most_under_weight = -1;
  double max_weight = std::numeric_limits<double>::min();
  for (UInt i=0; i < mesh_blocks.size(); ++i)
  {
    double weight_per_split_block = mesh_blocks[i]->weight / num_splits_per_block[i];

    if (weight_per_split_block > max_weight && num_splits_per_block[i] < max_splits_per_block)
    {
      max_weight = weight_per_split_block;
      block_most_under_weight = i;
    }
  }

  if (block_most_under_weight == UInt(-1))
  {
    throw std::runtime_error("could not reduce the number of splits in any block");
  }

  return block_most_under_weight;
}

// computes a decomposition of roughly equally sized block with number of blocks <= num_split_blocks
std::array<UInt, 3> computeEvenlyDivisibleBlockGrid(const SplitBlock& input_block, UInt num_split_blocks)
{
  std::array<UInt, 3> num_blocks_per_direction = {1, 1, 1};
  std::array<double, 3> num_elems_per_directions = {static_cast<double>(input_block.element_counts[0]),
                                                    static_cast<double>(input_block.element_counts[1]),
                                                    static_cast<double>(input_block.element_counts[2])};

  while (num_blocks_per_direction[0] * num_blocks_per_direction[1] * num_blocks_per_direction[2] < num_split_blocks)
  {
    UInt max_direction = 0;
    double max_elements_per_direction = num_elems_per_directions[max_direction];
    for (UInt i=1; i < 3; ++i)
    {
      // this also enforces the condition that num_blocks_per_direction[i] < num_elems_per_direction[i] in the original block
      // (if there exists a solution)
      if (num_elems_per_directions[i] > max_elements_per_direction)
      {
        max_direction = i;
        max_elements_per_direction = num_elems_per_directions[i];
      }
    }

    std::array<UInt, 3> new_num_blocks_per_direction = num_blocks_per_direction;
    new_num_blocks_per_direction[max_direction]++;
    UInt new_num_blocks = new_num_blocks_per_direction[0] * new_num_blocks_per_direction[1] * new_num_blocks_per_direction[2];
    if (new_num_blocks > num_split_blocks)
    {
      break;
    }

    num_blocks_per_direction = new_num_blocks_per_direction;
    num_elems_per_directions[max_direction] = double(input_block.element_counts[max_direction]) / num_blocks_per_direction[max_direction];
  }

  return num_blocks_per_direction;
}

std::array<std::vector<UInt>, 3> computeNumElementsPerBlock(const SplitBlock& input_block, 
                                                            const std::array<UInt, 3>& num_blocks_per_direction)
{
  std::array<std::vector<UInt>, 3> num_elem_per_block;
  for (UInt d=0; d < 3; ++d)
  {
    UInt max_num_elements_per_direction = input_block.element_counts[d] / num_blocks_per_direction[d];
    UInt remainder = input_block.element_counts[d] - (num_blocks_per_direction[d] - 1)*max_num_elements_per_direction;
    for (UInt i=0; i < num_blocks_per_direction[d] - 1; ++i)
      num_elem_per_block[d].push_back(max_num_elements_per_direction);

    num_elem_per_block[d].push_back(remainder);

    UInt idx = 0;
    while (num_elem_per_block[d][num_blocks_per_direction[d]-1] > max_num_elements_per_direction)
    {
      num_elem_per_block[d][idx]++;
      num_elem_per_block[d][num_blocks_per_direction[d]-1]--;
      idx = (idx + 1) % (num_blocks_per_direction[d] - 1);
    }
  }

  return num_elem_per_block;  
}

std::vector<SplitBlock> createSplitBlocks(const SplitBlock& input_block, 
                                          const std::array<std::vector<UInt>, 3>& num_elem_per_block)
{
  std::array<UInt, 3> num_blocks_per_direction = {num_elem_per_block[0].size(), num_elem_per_block[1].size(), num_elem_per_block[2].size()};

  std::vector<SplitBlock> new_blocks;
  std::array<UInt, 3> block_offset = input_block.mesh_offsets;
  for (UInt i=0; i < num_blocks_per_direction[0]; ++i)
  {
    block_offset[1] = input_block.mesh_offsets[1];
    for (UInt j=0; j < num_blocks_per_direction[1]; ++j)
    {
      block_offset[2] = input_block.mesh_offsets[2];
      for (UInt k=0; k < num_blocks_per_direction[2]; ++k)
      {
        std::array<UInt, 3> elems_in_block = {num_elem_per_block[0][i], num_elem_per_block[1][j], num_elem_per_block[2][k]};
        new_blocks.emplace_back(input_block.meshblock, elems_in_block, block_offset);

        block_offset[2] += num_elem_per_block[2][k];
      }
      block_offset[1] += num_elem_per_block[1][j];
    }

    block_offset[0] += num_elem_per_block[0][i];
  }

  return new_blocks;
}


// split the block into num_split_blocks, using a recursive approach to deal with non-evenly divisible
// number of blocks
std::vector<SplitBlock> recursivelySplitBlock(const SplitBlock& input_block, UInt num_split_blocks)
{
  std::array<UInt, 3> num_blocks_per_direction = computeEvenlyDivisibleBlockGrid(input_block, num_split_blocks);
  UInt num_blocks_in_grid = prod(num_blocks_per_direction);
  UInt num_remainder_blocks = num_split_blocks - num_blocks_in_grid;

  if (num_remainder_blocks > 0)
  {
    double weight_fraction = double(num_blocks_in_grid) / num_split_blocks;
    auto [main_block, remainder_block] = splitBlock(input_block, weight_fraction);

    num_blocks_per_direction = computeEvenlyDivisibleBlockGrid(main_block, num_blocks_in_grid);

    if (prod(num_blocks_per_direction) != num_blocks_in_grid)
      throw std::runtime_error("dividing block changed number of blocks in grid");

    std::array<std::vector<UInt>, 3> num_elem_per_block = computeNumElementsPerBlock(main_block, num_blocks_per_direction);
    std::vector<SplitBlock> new_blocks = createSplitBlocks(main_block, num_elem_per_block);

    std::vector<SplitBlock> remainder_blocks;
    if (num_remainder_blocks > 1)
    {
      remainder_blocks = recursivelySplitBlock(remainder_block, num_remainder_blocks);
    } else
    {
      remainder_blocks = {remainder_block};
    }

    for (const SplitBlock& split_block : remainder_blocks)
      new_blocks.push_back(split_block);

    return new_blocks;
  } else
  {
    std::array<std::vector<UInt>, 3> num_elem_per_block = computeNumElementsPerBlock(input_block, num_blocks_per_direction);
    return createSplitBlocks(input_block, num_elem_per_block);
  }
}

std::vector<SplitBlock> recursivelySplitBlock(std::shared_ptr<MeshBlock> input_block, UInt num_split_blocks)
{
  return recursivelySplitBlock(SplitBlock(input_block), num_split_blocks);
}


std::vector<SplitBlock> splitBlocks(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block)
{
  std::vector<SplitBlock> split_blocks;
  for (UInt i=0; i < mesh_blocks.size(); ++i)
  {
    SplitBlock block(mesh_blocks[i]);
    std::vector<SplitBlock> new_blocks = recursivelySplitBlock(block, num_splits_per_block[i]);
    for (const auto& new_block : new_blocks)
      split_blocks.push_back(new_block);
  }

  return split_blocks;
}

// returns a vector telling how many sub-blocks to split each block into
std::vector<UInt> computeNumSubBlocks(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs)
{

  double avg_weight_per_proc = computeAvgWorkPerProc(mesh_blocks, nprocs);

  std::vector<UInt> num_splits_per_block(mesh_blocks.size(), 0);
  UInt num_splits = 0;  // num splits is the number of sub-blocks to split 
                        // a given block into, not the number of cuts to make

  for (UInt i=0; i < mesh_blocks.size(); ++i)
  {
    num_splits_per_block[i] = std::max(std::ceil(mesh_blocks[i]->weight / avg_weight_per_proc), 1.0);
    num_splits_per_block[i] = std::min(num_splits_per_block[i], nprocs);
    num_splits += num_splits_per_block[i];
  }

  // adjust splits so there are at least as many sub-blocks as procs
  while (num_splits < nprocs)
  {
    UInt block_most_under_weight = getMostOverWeightBlock(mesh_blocks, num_splits_per_block, nprocs);
    num_splits_per_block[block_most_under_weight]++;
    num_splits++;
  }

  return num_splits_per_block;
}


std::vector<std::vector<SplitBlock>> preSplit(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs)
{
  std::vector<UInt> num_splits_per_block = computeNumSubBlocks(mesh_blocks, nprocs);
  std::vector<SplitBlock> split_blocks = splitBlocks(mesh_blocks, num_splits_per_block);
  auto blocks_on_procs = assignBlocksToProcs(split_blocks, nprocs);

  return blocks_on_procs;
}

}