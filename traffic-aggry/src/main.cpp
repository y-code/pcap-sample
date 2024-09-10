#include <iostream>
#include <boost/program_options.hpp>
#include "traffic-aggry.hpp"

int main(int argc, char **argv) {
    boost::program_options::options_description desc("Allow options");
    desc.add_options()
        ("help,h", "produce help message")
        ("verbose,v", "produce human-readable outputs")
        ("kafka,k", "output to a kafka topic")
        ("kafka-broker,b", boost::program_options::value<std::string>(), "Kafka broker host and port concatenated by colon")
        ("kafka-topic,t", boost::program_options::value<std::string>(), "Kafka topic");

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

    bool useKafka = false;
    if (vm.count("kafka")) {
        useKafka = true;
    }
    std::string broker = "localhost:9093";
    if (vm.count("kafka-broker")) {
        broker = vm["kafka-broker"].as<std::string>();
    }

    std::string topic = "traffic";
    if (vm.count("kafka-topic")) {
        topic = vm["kafka-topic"].as<std::string>();
    }

    std::unique_ptr<ypcap::TrafficAggry> service;
    try {
        if (useKafka) {
            auto producerFactory = std::make_shared<ypcap::KafkaProducerFactory>(broker);
            service = make_unique<ypcap::TrafficAggry>(producerFactory, topic);
            service->Process();
        } else {
            service = std::make_unique<ypcap::TrafficAggry>();
            service->Process();
        }
    } catch (std::exception const &ex) {
        std::cerr << "[ERROR] " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}

