#ifndef STRUCTURED_PART_TEST_UTILS_H
#define STRUCTURED_PART_TEST_UTILS_H

#include "ProjectDefs.h"
#include <array>
#include "blocks.h"

using namespace structured_part;

inline std::array<UInt, 3> make_array(const std::array<UInt, 3>& vals)  
{
  return vals;
}


class Array3D
{
  public:
    using value_type = int_least8_t;

    Array3D(const std::array<UInt, 3>& dims):
      m_vals(dims[0]*dims[1]*dims[2]),
      m_dims(dims)
    {}

    value_type& operator()(UInt i, UInt j, UInt k) { return m_vals[getIdx(i, j, k)]; }

    const value_type& operator()(UInt i, UInt j, UInt k) const { return m_vals[getIdx(i, j, k)]; }

    void set(value_type val)
    {
      for (UInt i=0; i < m_dims[0]; ++i)
        for (UInt j=0; j < m_dims[1]; ++j)
          for (UInt k=0; k < m_dims[2]; ++k)
            this->operator()(i, j, k) = val;
    }

    UInt extent(UInt i) const { return m_dims[i]; }

  private:
    UInt getIdx(UInt i, UInt j, UInt k) const
    {
      UInt idx = i * m_dims[1]*m_dims[2] + j*m_dims[2] + k;
      return idx;
    }

    std::vector<value_type> m_vals;
    std::array<UInt, 3> m_dims;
};

void checkDecompositionValid(const std::vector<std::shared_ptr<MeshBlock>>& mesh_blocks,
                             const std::vector<std::vector<SplitBlock>>& split_blocks);

void checkLoadBalance(const std::vector<std::vector<SplitBlock>>& blocks_on_procs, double load_balance_factor);

#endif