#include "model.hpp"

#include <boost/program_options.hpp>

#include <memory>

static bool checkParameters(int q, int Delta, long double B) {
    bool failed = false;

    if (Delta < 3) {
        std::cout << "Delta must be greater than 2." << std::endl;
        failed |= true;
    }

    if (q <= 2 * Delta) {
        std::cout << "The number of colours q must be greater than 2 * Delta." << std::endl;
        failed |= true;
    }

    if (B >= 1) {
        std::cout << "B must be less than 1." << std::endl;
        failed |= true;
    }

    if (const long double min_B = 1 - static_cast<long double>(q - 2 * Delta) / Delta; B <= min_B) {
        std::cout << "B, Delta and q must satisfy B > 1 - (q - 2 * Delta) / Delta, i.e. B > ";
        std::cout << std::to_string(min_B) << std::endl;
        failed |= true;
    }

    return failed;
}


int main(int argc, char **argv) {
    // boost::program_options will handle command line parsing
    namespace po = boost::program_options;

    po::variables_map vm;
    po::options_description description("Allowed Options");

    // Declare arguments
    description.add_options()
        ("help,h", "Display this help message")
        ("temperature,T",
             po::value<long double>()->default_value((long double)0.95),
            "Parameter B controls the strength of interactions; must be in the "
            "interval (0, 1)"
        )
        ("colours,q", po::value<int>()->required(), "Number of colours")
        ("delta,d", po::value<int>()->required(), "Maximum degree of graph")
        ("number,n", po::value<int>()->required(), "Number of vertices")
        ("type,t", po::value<std::string>()->default_value("cycle"), "Type of graph");

    // parse arguments and save them in the variable map (vm)
    po::store(
        po::parse_command_line(argc, argv, description),
        vm
    );

    if (vm.count("help")) {
        std::cout << description << "\n";
        return 1;
    }

    const int n = vm["number"].as<int>();
    const int q = vm["colours"].as<int>();
    const int Delta = vm["delta"].as<int>();
    const long double B = vm["temperature"].as<long double>();
    const std::string type = vm["type"].as<std::string>();

    //  check if parameters match conditions for theorem
    //  note that the algorithm still works if B is not in the correct interval,
    //  but there is no guarantee
    if (!checkParameters(q, Delta, B)) {
        return 1;
    }

    // Is there a better way to do struct initialization in a shared_ptr?
    std::shared_ptr<const Parameters> params = std::shared_ptr<const Parameters>(new Parameters{n, q, Delta, B});

    Model m(std::move(params), type);
    m.sample();
    std::cout << m;
    return 0;
}

