# cxx-potts-sampler
A perfect sampling algorithm for the anti-ferromagnetic Potts model. The expected run-time of this algorithm on an n-vertex graph is provably O(n^2) when the number of colours (q) is more than twice the maximum degree (d) of the graph and the parameter B is in (1 - (q - 2d)/d, 1).

## Installation and Build Instructions

Download and build the source code from the repository:
```shell
git clone https://github.com/markusleroux/cxx-potts-sampler.git
cd cxx-potts-sampler/bin
cmake ..
cmake --build
```
The build process has been tested on OSX, but should work on any *nix system.

## Generating samples

Once built, information about the command line options is available under the --help (-h) flag. At the moment, specification of the graph structure from the
command line only supports a small number of types of graph. Two flags are used to specify the structure of the graph:
* --number (-n): the number of vertices in the graph
* --type (-t): the type of the graph (currently, only `cycle and `complete graph families are supported)

The parameters of th model may be controlled similarly. The following parameters are available:
* --parameter (-B): the parameter of the model (defaults to 0.5)
* --colours (-q): the number of colours to use
* --delta (-d): the maximum degree of any of the graphs in the family you wish to sample from

For example, with the sampler executable on the path, a sample from the model with 5 colours and parameters B = 0.95, Delta = 3 on the cyclic graph with 5
vertices can be generated as follows:
```shell
sampler -B 0.95 -q 5 -d 3 -n 5 -t cycle
```
