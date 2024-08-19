#include <iostream>
#include <boost/program_options.hpp>
#include "traffic-aggry.hpp"

int main(int argc, char **argv) {
    boost::program_options::options_description desc("Allow options");
    desc.add_options()
        ("help,h", "produce help message")
	("verbose,v", "produce human-readable outputs");

    boost::program_options::variables_map vm;
    try {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), vm);
	boost::program_options::notify(vm);
    } catch (const boost::program_options::error &e) {
        std::cerr << desc << std::endl;
	return 1;
    }

    if (vm.count("help")) {
        std::cout << desc << std::endl;
	return 0;
    }

    try {
        ypcap::TrafficAggry service;
        service.Process();
    } catch (std::exception const &ex) {
        std::cerr << "[ERROR] " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

