#include "statistics.h"
#include <iomanip>
#include <cmath>

#include "pre_split.h"


namespace structured_part {

DecompStats computeDecompStats(const std::vector<std::vector<SplitBlock>>& blocks_per_proc)
{
  UInt nprocs = blocks_per_proc.size();

  DecompStats stats;
  for (UInt proc=0; proc < nprocs; ++proc)
  {
    double weight = computeTotalWeight(blocks_per_proc[proc]);

    stats.min_weight = std::min(stats.min_weight, weight);
    stats.max_weight = std::max(stats.max_weight, weight);
    stats.avg_weight_per_process += weight;
    stats.weight_per_process.push_back(weight);


    UInt num_blocks_on_proc = blocks_per_proc[proc].size();
    stats.num_blocks += num_blocks_on_proc;
    stats.blocks_per_proc.push_back(num_blocks_on_proc);
    stats.max_blocks_per_proc = std::max(stats.max_blocks_per_proc, num_blocks_on_proc);
    stats.min_blocks_per_proc = std::min(stats.min_blocks_per_proc, num_blocks_on_proc);
    stats.avg_blocks_per_proc += num_blocks_on_proc;

  }

  stats.avg_weight_per_process /= nprocs;
  stats.avg_blocks_per_proc /= nprocs;

  return stats;
}

std::ostream& operator<<(std::ostream& os, const DecompStats& stats)
{
  os << "decomp with " << stats.num_blocks << " sub-blocks" << std::endl;
  os << "min, max, avg weight = " << stats.min_weight << ", " << stats.max_weight << ", " << stats.avg_weight_per_process << std::endl;
  os << "min, max, avg blocks per proc " << stats.min_blocks_per_proc << ", " << stats.max_blocks_per_proc << ", " << stats.avg_blocks_per_proc << std::endl;
  os << "max load imbalance overage % = " << 100 * (stats.max_weight - stats.avg_weight_per_process)/stats.avg_weight_per_process;

  return os;
}

void printPerProcessStats(std::ostream& os, const DecompStats& stats)
{
  os << "weights per process, num_blocks = " << std::endl;
  for (UInt i=0; i < stats.weight_per_process.size(); ++i)
  {
    os << "rank " << i << ": " << stats.weight_per_process[i] << ", " << stats.blocks_per_proc[i] << "\n";    
  }  
}

void printHistogram(std::ostream& os, const DecompStats& stats, UInt width)
{
  for (UInt proc=0; proc < stats.weight_per_process.size(); ++proc)
  {
    UInt num_chars = std::round(width * stats.weight_per_process[proc] / double(stats.max_weight));
    std::string str(num_chars, '#');
    os << std::setw(6) << proc << ": " << str << "\n";
  }
}

}