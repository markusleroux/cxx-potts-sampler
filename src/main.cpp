#include "Model.h"
#include <boost/program_options.hpp>

static bool checkParameters(unsigned q, unsigned Delta, long double B) {
	return (q > 2 * Delta) && (Delta >= 3) && (B > 1 - (long double) (q - 2 * Delta) / Delta) && (B < 1);
}

int main(int argc, char **argv) {
	namespace po = boost::program_options;

	po::variables_map vm;
	po::options_description description("Allowed Options");

	// Declare arguments
	description.add_options()
			("help,h", "Display this help message")
			("parameter,B", po::value<long double>()->default_value((long double) 0.5), "Parameter B")
			("colours,q", po::value<unsigned int>()->required(), "Number of colours")
			("delta,d", po::value<unsigned int>()->required(), "Maximum degree of graph")
			("number,n", po::value<unsigned int>()->required(), "Number of vertices")
			("type,t", po::value<std::string>()->default_value("cycle"), "Type of graph");

	// parse arguments and save them in the variable map (vm)
	po::store(po::parse_command_line(argc, argv, description), vm);

	if (vm.count("help")) {
		std::cout << description << "\n";
		return 1;
	}

	unsigned n = vm["number"].as<unsigned int>();
	unsigned q = vm["colours"].as<unsigned int>();
	unsigned Delta = vm["delta"].as<unsigned int>();
	long double B = vm["parameter"].as<long double>();


	if (not checkParameters(q, Delta, B)) {
		std::cout << "Parameters don't meet conditions of theorem.\n";
		std::cout << "Try setting B in the interval (" << std::to_string(1 - (long double) (q - 2 * Delta) / Delta)
		          << ")\n";
		return 0;
	}

	try {
		Model m = Model::genericModelConstructor(vm["type"].as<std::string>(), n, q, Delta, B);
		m.sample();
		std::cout << m;
	} catch (std::exception &e) {
		std::cout << e.what() << "\n";
		return 0;
	}
}