# cxx-potts-sampler
A perfect sampling algorithm for the anti-ferromagnetic Potts model. The expected run-time of this algorithm on an n-vertex graph is provably O(n^2) when the number of colours (q) is more than twice the maximum degree (d) of the graph and the parameter B is in (1 - (q - 2d)/d, 1).

## Dependencies

The project is configured using CMake and built using ninja and clang. The library depends on Boost for dynamic bitset. If building the CLI tool (`-DBUILD_CLI`), Boost program options are also required. If building the tests, Catch2 is required.

Dependencies available on the building system will be used if available. Otherwise, dependencies are vendored using CMake's FetchContent mechanism. To avoid downloading and building the dependencies if they are not found on the host, pass `-DFETCH_CONTENT_FULLY_DISCONNECTED=ON` to the configuration step.


## Installation and Build Instructions

Download and build the source code from the repository:
```bash
git clone https://github.com/markusleroux/cxx-potts-sampler.git
cd cxx-potts-sampler
mkdir build
cmake -B build -S . --preset=release
cmake --build build
```

## CLI

The CLI can be built by passing `-DBUILD_CLI` to the configure stage. Once built, information about the command line options is available under the --help (-h) flag.

For example, the following generates a sample from the model with 5 colours, temperature 0.95, maximum degree (delta) 3 on the cyclic graph with 5 vertices:
```bash
potts-sampler --temperature 0.95 --colours 7 --vertices 10 --type cycle
```

## TODO
- [ ] visualize graphs with colourings
- [ ] control the seed
- [ ] make graph a concept class to allow user types?
- [ ] multi-thread at a per-epoch granularity
- [ ] improve test coverage and test across a suite of compilers, build generators and dependency versions
