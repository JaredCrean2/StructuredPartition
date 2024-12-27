#ifndef STRUCTURED_PART_STRUCTURED_PART_H
#define STRUCTURED_PART_STRUCTURED_PART_H

#include "blocks.h"

namespace structured_part {

std::vector<std::vector<SplitBlock>> partitionMesh(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs, double load_balance_factor);

}

#endif
