#include "blocks.h"

namespace structured_part
{

std::pair<SplitBlock, SplitBlock> splitBlock(const SplitBlock& splitBlock, SplitDirection dir, UInt nelem)
{
  UInt dir_int = static_cast<UInt>(dir);
  if (nelem == 0 || nelem >= splitBlock.element_counts[dir_int])
  {
    throw std::runtime_error("invalid split");
  }

  std::array<UInt, 3> left_element_counts = splitBlock.element_counts;
  left_element_counts[dir_int] = nelem;
  std::array<UInt, 3> left_mesh_offsets = splitBlock.mesh_offsets;

  std::array<UInt, 3> right_element_counts = splitBlock.element_counts;
  right_element_counts[dir_int] = splitBlock.element_counts[dir_int] - nelem;
  std::array<UInt, 3> right_mesh_offsets = splitBlock.mesh_offsets;
  right_mesh_offsets[dir_int] += nelem;

  return std::make_pair(SplitBlock(splitBlock.meshblock, left_element_counts, left_mesh_offsets),
                        SplitBlock(splitBlock.meshblock, right_element_counts, right_mesh_offsets));
}
}