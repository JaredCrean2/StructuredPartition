#include "ProjectDefs.h"
#include <iostream>
#include <array>
#include <vector>

namespace structured_part {

template <typename T, size_t N>
std::ostream& operator<<(std::ostream& os, const std::array<T, N>& arr)
{
  for (UInt i=0; i < N; ++i)
    os << arr[i] << ",";
  return os;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr)
{
  for (UInt i=0; i < arr.size(); ++i)
    os << arr[i] << ",";
  return os;
}

}