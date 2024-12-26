#ifndef STRUCTURED_PART_BLOCKS_H
#define STRUCTURED_PART_BLOCKS_H

#include "ProjectDefs.h"
#include <memory>
#include <cassert>
#include <array>
#include "array_helpers.h"

#include <stdexcept>

namespace structured_part {

struct MeshBlock
{
  MeshBlock(Int block_id, UInt nx, UInt ny, UInt nz, double weight):
    block_id(block_id),
    element_counts{nx, ny, nz},
    weight(weight)
  {}

  MeshBlock(Int block_id, UInt nx, UInt ny, UInt nz):
    MeshBlock(block_id, nx, ny, nz, nx*ny*nz)
  {}

  Int block_id;
  std::array<UInt, 3> element_counts;
  double weight;
};

inline std::ostream& operator<<(std::ostream& os, const MeshBlock& block)
{
  os << "block " << block.block_id << ", dim = " << block.element_counts << ", weight = " << block.weight << std::endl;
  return os;
}


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
  mesh_offsets(mesh_offsets),
  weight(meshblock->weight*double(element_counts[0]*element_counts[1]*element_counts[2])/(meshblock->element_counts[0]*meshblock->element_counts[1]*meshblock->element_counts[2]))
  {
    for (UInt i=0; i < 3; ++i)
    {
      assert(mesh_offsets[i] + element_counts[i] <= meshblock->element_counts[i]);
    }
  }  
    
  std::shared_ptr<MeshBlock> meshblock;
  std::array<UInt, 3> element_counts;
  std::array<UInt, 3> mesh_offsets;
  double weight;
};

inline std::ostream& operator<<(std::ostream& os, const SplitBlock& block)
{
  os << "split block with dim = " << block.element_counts << ", offset into parent block = " << block.mesh_offsets << ", weight = " << block.weight << std::endl;
  return os;
}

inline bool operator!=(const SplitBlock& lhs, const SplitBlock& rhs)
{
  return lhs.meshblock != rhs.meshblock ||
         lhs.element_counts != rhs.element_counts ||
         lhs.mesh_offsets != rhs.mesh_offsets;
}

inline bool operator==(const SplitBlock& lhs, const SplitBlock& rhs)
{
  return !(lhs != rhs);
}



enum class SplitDirection
{
  I,
  J,
  K
};


// splits a block in the given direction.  Returns 2 blocks, one with elements [0, nelem) from the original
// block in the given direction, and one with [nelem, splitBlock->element_counts] in the given direction
std::pair<SplitBlock, SplitBlock> splitBlock(const SplitBlock& splitBlock, SplitDirection dir, UInt nelem);

// splits block in half along longest axis
std::pair<SplitBlock, SplitBlock> splitBlock(const SplitBlock& splitBlock);


}  // namespace

#endif