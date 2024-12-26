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

UInt getMostOverWeightBlock(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block)
{
  UInt block_most_under_weight = -1;
  double max_weight = std::numeric_limits<double>::min();
  for (UInt i=0; i < mesh_blocks.size(); ++i)
  {
    double weight_per_split_block = mesh_blocks[i]->weight / num_splits_per_block[i];

    if (weight_per_split_block > max_weight)  //TODO: this could potentially split one block into more than nproc pieces
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

/*
// returns a factor of the given integer as close to sqrt(integer) as possible
UInt factor_int(UInt integer)
{
  UInt factor = std::ceil(std::sqrt(integer));
  for (; factor <= integer; ++factor)
  {
    if (integer % factor == 0)
    {
      return factor;
    }
  }

  return -1;  // unreachable
}
*/

// computes a decomposition of roughly equally sized block with number of blocks <= num_split_blocks
std::array<UInt, 3> computeEvenlyDivisibleBlockGrid(const std::shared_ptr<MeshBlock>& input_block, UInt num_split_blocks)
{
  std::array<UInt, 3> num_blocks_per_direction = {1, 1, 1};
  std::array<double, 3> num_elems_per_directions = {static_cast<double>(input_block->element_counts[0]),
                                                    static_cast<double>(input_block->element_counts[1]),
                                                    static_cast<double>(input_block->element_counts[2])};

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
    num_elems_per_directions[max_direction] = double(input_block->element_counts[max_direction]) / num_blocks_per_direction[max_direction];
  }

  return num_blocks_per_direction;
}

std::array<std::vector<UInt>, 3> computeNumElementsPerBlock(const std::shared_ptr<MeshBlock>& input_block, const std::array<UInt, 3>& num_blocks_per_direction)
{
  std::array<std::vector<UInt>, 3> num_elem_per_block;
  for (UInt d=0; d < 3; ++d)
  {
    UInt max_num_elements_per_direction = input_block->element_counts[d] / num_blocks_per_direction[d];
    UInt remainder = input_block->element_counts[d] - (num_blocks_per_direction[d] - 1)*max_num_elements_per_direction;
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

std::vector<SplitBlock> createSplitBlocks(const std::shared_ptr<MeshBlock>& input_block, const std::array<std::vector<UInt>, 3>& num_elem_per_block)
{
  std::array<UInt, 3> num_blocks_per_direction = {num_elem_per_block[0].size(), num_elem_per_block[1].size(), num_elem_per_block[2].size()};

  std::vector<SplitBlock> new_blocks;
  std::array<UInt, 3> block_offset = {0, 0, 0};
  for (UInt i=0; i < num_blocks_per_direction[0]; ++i)
  {
    block_offset[1] = 0;
    for (UInt j=0; j < num_blocks_per_direction[1]; ++j)
    {
      block_offset[2] = 0;
      for (UInt k=0; k < num_blocks_per_direction[2]; ++k)
      {
        std::array<UInt, 3> elems_in_block = {num_elem_per_block[0][i], num_elem_per_block[1][j], num_elem_per_block[2][k]};
        new_blocks.emplace_back(input_block, elems_in_block, block_offset);

        block_offset[2] += num_elem_per_block[2][k];
      }
      block_offset[1] += num_elem_per_block[1][j];
    }

    block_offset[0] += num_elem_per_block[0][i];
  }

  return new_blocks;
}

void splitBlocksToFillRemainder(std::vector<SplitBlock>& new_blocks, int num_split_blocks)
{
  while (new_blocks.size() < num_split_blocks)
  {
    UInt max_block_idx = 0;
    UInt max_block_size = 0;
    for (UInt i=0; i < new_blocks.size(); ++i)
    {
      UInt block_size = new_blocks[i].element_counts[0] * new_blocks[i].element_counts[1] * new_blocks[i].element_counts[2];
      if (block_size > max_block_size)
      {
        max_block_idx = i;
        max_block_size = block_size;
      }
    }

    auto [left_block, right_block] = splitBlock(new_blocks[max_block_idx]);

    new_blocks[max_block_idx] = left_block;
    new_blocks.push_back(right_block);
  }  
}


std::vector<SplitBlock> splitBlock(const std::shared_ptr<MeshBlock>& input_block, UInt num_split_blocks)
{
  // the difficulty here is that if num_split_blocks is a prime number, then the only
  // possible decomposition with equal areas is n x 1 x 1, which is bad for parallel
  // communication.  Instead, break the block into two pieces and divide the remaining splits between them
  // TODO: this only works for 2D.  For 3D, we need factors close to the cube root of n

  std::array<UInt, 3> num_blocks_per_direction = computeEvenlyDivisibleBlockGrid(input_block, num_split_blocks);
  std::array<std::vector<UInt>, 3> num_elem_per_block = computeNumElementsPerBlock(input_block, num_blocks_per_direction);
  
  std::vector<SplitBlock> new_blocks = createSplitBlocks(input_block, num_elem_per_block);
  splitBlocksToFillRemainder(new_blocks, num_split_blocks);

  return new_blocks;
}


std::vector<SplitBlock> splitBlocks(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block)
{
  std::vector<SplitBlock> split_blocks;
  for (UInt i=0; i < mesh_blocks.size(); ++i)
  {
    std::vector<SplitBlock> new_blocks = splitBlock(mesh_blocks[i], num_splits_per_block[i]);
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
    num_splits += num_splits_per_block[i];
  }

  // adjust splits so there are at least as many sub-blocks as procs
  while (num_splits < nprocs)
  {
    if (num_splits > nprocs)
    {
      //TODO: we could just sort the blocks by avg work and choose the first n
      UInt block_most_under_weight = getMostUnderWeightBlock(mesh_blocks, num_splits_per_block);
      assert(num_splits_per_block[block_most_under_weight] > 1);
      num_splits_per_block[block_most_under_weight]--;
      num_splits--;
    } else
    {
      UInt block_most_under_weight = getMostOverWeightBlock(mesh_blocks, num_splits_per_block);
      num_splits_per_block[block_most_under_weight]++;
      num_splits++;
    }
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