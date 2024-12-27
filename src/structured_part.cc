#include "structured_part.h"
#include "final_split.h"


namespace structured_part {

std::vector<std::vector<SplitBlock>> partitionMesh(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks, UInt nprocs, double load_balance_factor)
{
  return finalSplit(mesh_blocks, nprocs, load_balance_factor);
}

}