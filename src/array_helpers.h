#ifndef STRUCTURED_PART_ARRAY_HELPERS_H
#define STRUCTURED_PART_ARRAY_HELPERS_H
#include "ProjectDefs.h"
#include <iostream>
#include <array>
#include <vector>

namespace structured_part {


template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& arr);

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

template <typename T, size_t N>
T prod(const std::array<T, N>& arr)
{
  T val = 1;
  for (UInt i=0; i < N; ++i)
    val *= arr[i];

  return val;
}

}

#endif