#include <boost/program_options.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "sampler.hpp"

static std::optional<std::pair<Graph::Type, Parameters>> parse_params(int argc, char **argv) {
    namespace po = boost::program_options;

    po::variables_map vm;
    po::options_description description("Program Options");

    Graph::Type type;
    Parameters params;

    // Declare arguments
    // clang-format off
    description.add_options()
        ("help,h", "Display this help message")
        (
            "temperature,T", po::value<long double>(&params.temperature)->default_value(0.95L),
            "The strength of interactions; must be in the interval (0, 1)"
        )
        ("colours,q", po::value<int>(&params.maxColours)->default_value(7),             "Number of colours")
        ("delta,d",   po::value<int>(&params.maxDegree)->default_value(3),              "Maximum degree of graph")
        ("number,n",  po::value<int>(&params.numNodes)->default_value(10),              "Number of vertices")
        ("type,t",    po::value<Graph::Type>(&type)->default_value(Graph::Type::CYCLE), "Type of graph");

    // parse arguments and save them in the variable map (vm)
    po::store(
        po::parse_command_line(argc, argv, description),
        vm
    );

    if (vm.count("help")) {
        std::cout << description << std::endl;
        return std::nullopt;
    }

    po::notify(vm);
    // clang-format on

    return {
        {type, params}
    };
}

int main(int argc, char **argv) {
    auto paramsMb = parse_params(argc, argv);
    if (!paramsMb) {
        return 1;
    }

    //  check if parameters match conditions for theorem
    //  note that the algorithm still works if B is not in the correct interval,
    //  but there is no guarantee
    auto [type, params] = std::move(paramsMb.value());
    if (!params.verify()) {
        return 1;
    }

    auto graph                 = Graph(params.numNodes, type);
    std::vector<int> colouring = sample(params, graph);

    std::cout << "| ";
    for (int i{}; i < colouring.size(); ++i) {
        std::cout << i << ": " << colouring[i] << " | ";
    }
    std::cout << std::endl;

    return 0;
}
