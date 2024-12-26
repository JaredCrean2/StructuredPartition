#ifndef STRUCTURED_PART_STATS_H
#define STRUCTURED_PART_STATS_H

#include "blocks.h"
#include <limits>

namespace structured_part {

struct DecompStats
{
  UInt num_blocks = 0.0;
  UInt max_blocks_per_proc = 0.0;
  UInt min_blocks_per_proc = std::numeric_limits<UInt>::max();
  double avg_blocks_per_proc = 0.0;
  std::vector<UInt> blocks_per_proc;

  double min_weight = std::numeric_limits<double>::max();;
  double max_weight = std::numeric_limits<double>::min();
  double avg_weight_per_process = 0.0;
  std::vector<double> weight_per_process;
};

DecompStats computeDecompStats(const std::vector<std::vector<SplitBlock>>& blocks_per_proc);

std::ostream& operator<<(std::ostream& os, const DecompStats& stats);

void printPerProcessStats(std::ostream& os, const DecompStats& stats);

void printHistogram(std::ostream& os, const DecompStats& stats, UInt width=64);

}

#endif