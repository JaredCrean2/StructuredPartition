This is a package for partitioning structured multi-block meshes, based
loosely on:

"CGNS Parallel Decomposition: A Workflow for using CGNS in Parallel HPC Analyses" by Gregory Sjaardema, HDF5 User Group 2023

The mesh is partitioned such that each processor has one or more rectangular
blocks of elements.
No processor has more than one rectangular block of elements from a given mesh block.
This facilitates using simple mesh data structures (for example, if
there were 2 or more rectangular blocks from the same mesh block on
a single processor, the mesh data structured would have to deal with
the possibility that they might be adjacent to each other)

# Building
The only dependencies for this package are a C++17 compiler, and GoogleTest if you wish to build the tests.  To build:

```
git clone https://github.com/JaredCrean2/StructuredPartition.git
cd ./structured_partition
mkdir -v ./build
cd ./build
../do_cmake.sh  # customize this as needed for your environment
make
make install
```

To run the tests

```
# while in the build directory
./tests/unit_tests
./tests/integration_tests
```

The unit tests should run nearly instantly, the integration tests take
less than 5 seconds to run on my (rather old) machine.

# Usage

In your code, the usage pattern is

```
#include "structured_part.h"

std::vector<std::shared_ptr<structured_part::MeshBlock>> mesh_blocks;
int nprocs = ;  // number of processors you want to partition the mesh
                // over
double load_balance_factor = 0.1;  // maximum load imbalance allowed
                                   // ie. no processor will have more than
                                   // (1 + load_balance_factor) work
                                   // than the average processor
int nblocks = ; // however many blocks you have here

for (int i=0; i < nblocks; ++i)
{
  size_t nx = , ny = , nz = ;  // get the number of elements in block i
  int block_id = ; // get the block id of your mesh block.
                   // structured_partition never uses this, but it will
                   // enable you to relate the split blocks back to the
                   // original mesh block in your mesh data structure
  mesh_blocks.push_back(std::make_shared<structured_part::MeshBlock>(block_id, nx, ny, nz);
}

std::vector<std::vector<structured_part::SplitBlock>> blocks_on_procs = structured_part::partitionMesh(mesh_blocks, nprocs, load_balance_factor);

// blocks_on_procs is a vector with length nprocs.
// blocks_on_procs[i] is a vector of SplitBlocks giving rectangular
// subsets of mesh blocks assigned to processor i

// SplitBlock::meshblock gives the std::shared_ptr<MeshBlock> of
// the MeshBlock this SplitBlock is a subset of.
// SplitBlock::element_counts is a std::array of length 3 giving
// the number of elements in the x, y, and z direction of the
// rectangular subset.
// SplitBlock::mesh_offsets gives the location of the lower-left corner
// (i.e. element {0, 0, 0}) of the rectangular subset in the MeshBlock.
// For a given element {i, j, k} in the rectangular subset, the
// indices in the original MeshBlock can be computed as
// {i, j, k} + SplitBlock::mesh_offsets.

```

