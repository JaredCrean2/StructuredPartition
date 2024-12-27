#ifndef STRUCTURED_PART_PRE_SPLIT_H
#define STRUCTURED_PART_PRE_SPLIT_H

#include "blocks.h"
#include <vector>

namespace structured_part {

double computeAvgWorkPerProc(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs);

UInt getMostUnderWeightBlock(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block);

UInt getMostOverWeightBlock(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block, UInt max_splits_per_block);

// computes a decomposition of roughly equally sized block with number of blocks <= num_split_blocks
std::array<UInt, 3> computeEvenlyDivisibleBlockGrid(const std::shared_ptr<MeshBlock>& input_block, UInt num_split_blocks);

std::array<std::vector<UInt>, 3> computeNumElementsPerBlock(const std::shared_ptr<MeshBlock>& input_block, const std::array<UInt, 3>& num_blocks_per_direction);

std::vector<SplitBlock> createSplitBlocks(const std::shared_ptr<MeshBlock>& input_block, const std::array<std::vector<UInt>, 3>& num_elem_per_block);

//void splitBlocksToFillRemainder(std::vector<SplitBlock>& new_blocks, int num_split_blocks);

//std::vector<SplitBlock> splitBlock(const std::shared_ptr<MeshBlock>& input_block, UInt num_split_blocks);

std::vector<SplitBlock> recursivelySplitBlock(const SplitBlock& input_block, UInt num_split_blocks);

std::vector<SplitBlock> recursivelySplitBlock(std::shared_ptr<MeshBlock> input_block, UInt num_split_blocks);

std::vector<SplitBlock> splitBlocks(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, const std::vector<UInt>& num_splits_per_block);

// returns a vector telling how many sub-blocks to split each block into
std::vector<UInt> computeNumSubBlocks(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs);

double computeTotalWeight(const std::vector<SplitBlock>& blocks);

UInt getProcWithMinWeightAndDifferentParent(const std::vector<std::vector<SplitBlock>>& blocks_on_proc, const std::shared_ptr<MeshBlock>& meshblock);

std::vector<std::vector<SplitBlock>> assignBlocksToProcs(std::vector<SplitBlock> split_blocks, UInt nprocs);

std::vector<std::vector<SplitBlock>> preSplit(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs);

} // namespace

#endif