#include <iostream>
#include <boost/program_options.hpp>
#include "sniffer.hpp"

int main(int argc, char *argv[]) {
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("endpoint,e", boost::program_options::value<std::string>(), "Endpoint to read")
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

    bool isVerbose = false;
    std::string *dev = NULL;
    
    if (vm.count("endpoint")) {
        dev = new std::string(vm["endpoint"].as<std::string>());
    }
    if (vm.count("verbose")) {
        isVerbose = true;
    }

    Sniffer sniffer(isVerbose);
    int result = sniffer.Run(dev);
    return result;
}

