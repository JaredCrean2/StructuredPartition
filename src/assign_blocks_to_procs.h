#ifndef STRUCTURED_PART_ASSIGN_BLOCKS_TO_PROC_H
#define STRUCTURED_PART_ASSIGN_BLOCKS_TO_PROC_H

#include "blocks.h"

namespace structured_part {

double computeTotalWeight(const std::vector<SplitBlock>& blocks);

UInt getProcWithMinWeightAndDifferentParent(const std::vector<std::vector<SplitBlock>>& blocks_on_proc, const std::shared_ptr<MeshBlock>& meshblock);

std::vector<std::vector<SplitBlock>> assignBlocksToProcs(std::vector<SplitBlock> split_blocks, UInt nprocs);

}

#endif