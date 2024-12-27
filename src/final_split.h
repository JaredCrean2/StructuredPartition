#ifndef STRUCTURED_PART_FINAL_SPLIT_H
#define STRUCTURED_PART_FINAL_SPLIT_H

#include "ProjectDefs.h"
#include "blocks.h"
#include <map>

namespace structured_part {

std::pair<UInt, double> computeMostOverWeightProc(const std::vector<std::vector<SplitBlock>>& blocks_on_procs);

SplitBlock* findLargestBlock(std::vector<SplitBlock>& blocks, const std::map<std::shared_ptr<MeshBlock>, UInt>& block_split_counts, UInt max_splits_per_block);

void splitUntilLoadBalanced(std::vector<std::vector<SplitBlock>>& blocks_on_procs, UInt nprocs, double avg_weight_per_proc, double load_balance_factor);

std::vector<std::vector<SplitBlock>> finalSplit(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs, double load_balance_factor);


}

#endif
