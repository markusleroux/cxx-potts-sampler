#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "sampler.hpp"

static std::pair<Graph::Type, Parameters> parse_params(int argc, char **argv) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::options_description description("Program Options");

    // Declare arguments
    // clang-format off
    description.add_options()
        ("help,h", "Display this help message")
        (
            "temperature,T", po::value<long double>()->default_value(0.95),
            "Parameter B controls the strength of interactions; must be in the interval (0, 1)"
        )
        ("colours,q", po::value<int>()->required(), "Number of colours")
        ("delta,d", po::value<int>()->required(), "Maximum degree of graph")
        ("number,n", po::value<int>()->required(), "Number of vertices")
        ("type,t", po::value<Graph::Type>()->default_value(Graph::Type::CYCLE), "Type of graph");

    // parse arguments and save them in the variable map (vm)
    po::store(
        po::parse_command_line(argc, argv, description),
        vm
    );
    // clang-format on

    if (vm.count("help")) {
        std::cout << description << std::endl;
        exit(1);
    }

    Parameters params{.numNodes    = vm["number"].as<int>(),
                      .maxColours  = vm["colours"].as<int>(),
                      .maxDegree   = vm["delta"].as<int>(),
                      .temperature = vm["temperature"].as<long double>()};

    auto type = vm["type"].as<Graph::Type>();
    return {type, params};
}

int main(int argc, char **argv) {
    auto [type, params] = parse_params(argc, argv);

    //  check if parameters match conditions for theorem
    //  note that the algorithm still works if B is not in the correct interval,
    //  but there is no guarantee
    if (!params.verify()) {
        return 1;
    }

    auto graph                 = Graph(params.numNodes, type);
    std::vector<int> colouring = sample(params, graph);

    for (int v : colouring) {
        std::cout << v << ' ';
    }
    std::cout << std::endl;

    return 0;
}
