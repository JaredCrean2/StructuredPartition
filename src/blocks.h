#include "ProjectDefs.h"
#include <memory>
#include <cassert>
#include <array>

#include <stdexcept>

namespace structured_part {

struct MeshBlock
{
  MeshBlock(Int block_id, UInt nx, UInt ny, UInt nz):
    block_id(block_id),
    element_counts{nx, ny, nz}
  {}

  Int block_id;
  std::array<UInt, 3> element_counts;
};


struct SplitBlock
{
  SplitBlock(std::shared_ptr<MeshBlock> meshblock, UInt nx, UInt ny, UInt nz,
             UInt mesh_offset_x, UInt mesh_offset_y, UInt mesh_offset_z):
    SplitBlock(meshblock, {nx, ny, nz}, {mesh_offset_x, mesh_offset_y, mesh_offset_z})
  {}

  SplitBlock(std::shared_ptr<MeshBlock> meshblock,
             const std::array<UInt, 3>& element_counts,
             const std::array<UInt, 3>& mesh_offsets):
  meshblock(meshblock),
  element_counts(element_counts),
  mesh_offsets(mesh_offsets)
  {
    for (UInt i=0; i < 3; ++i)
    {
      assert(mesh_offsets[i] + element_counts[i] <= meshblock->element_counts[i]);
    }
  }  
    
  std::shared_ptr<MeshBlock> meshblock;
  std::array<UInt, 3> element_counts;
  std::array<UInt, 3> mesh_offsets;
};

enum class SplitDirection
{
  I,
  J,
  K
};


// splits a block in the given direction.  Returns 2 blocks, one with elements [0, nelem) from the original
// block in the given direction, and one with [nelem, splitBlock->element_counts] in the given direction
std::pair<SplitBlock, SplitBlock> splitBlock(const SplitBlock& splitBlock, SplitDirection dir, UInt nelem);

}  // namespace