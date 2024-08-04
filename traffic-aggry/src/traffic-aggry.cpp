#include "traffic-aggry.hpp"

void ypcap::TrafficAggry::Process() {
    std::string line;

    while (std::getline(std::cin, line)) {
        std::cout << "[Aggry] " << line << std::endl;
    }
}

