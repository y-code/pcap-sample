#include <iostream>
#include <unordered_map>
#include <boost/beast/core/detail/base64.hpp>
#include "pcap.h"
#include "traffic-aggry.hpp"
#include "kafka-producer.hpp"

bool operator==(const in_addr &lhs, const in_addr &rhs) {
    return lhs.s_addr == rhs.s_addr;
}

template<>
struct std::hash<in_addr> {
    std::size_t operator()(const in_addr &n) const {
        return std::hash<unsigned long int>{}(n.s_addr);
    }
};

namespace ypcap {

TrafficAggry::TrafficAggry() {
    this->mProducerFactory = std::make_unique<ypcap::KafkaProducerFactory>("localhost:9093");
    this->mProducer = this->mProducerFactory->open();
    RdKafka::Conf *topicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    std::string topic = "traffic";
    this->mProducer->setupTopic(topic, topicConf);
}

TrafficAggry::~TrafficAggry() {
}

void TrafficAggry::flush() {
    while (!this->isFinished) {
        auto *newMap = new std::unordered_map<std::tuple<in_addr, in_addr, uint>, std::tuple<uint, uint>, TrafficHeaderHash, TrafficHeaderEqual>{};

        std::unique_lock<std::mutex> lock(this->mtx);
        this->cv.wait_for(lock, this->flushInterval, [this] { return this->isFinished.load(); });

        auto *oldMap = this->map;
        this->map = newMap;

        lock.unlock();

        const auto now = std::chrono::system_clock::now();
        const std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
        const std::tm tm_timestamp = *std::localtime(&timestamp);

        if (true) {
            // std::cout << "---" << std::endl;
            for (const auto& pair : *oldMap) {
                auto k = pair.first;
                auto v = pair.second;

                ypcap0::IpAddress src, dst;
                src.set_version(ypcap0::IpAddress::v4);
                src.set_address(reinterpret_cast<const char *>(&(std::get<0>(k))), sizeof(std::get<0>(k)));
                dst.set_version(ypcap0::IpAddress::v4);
                dst.set_address(reinterpret_cast<const char *>(&(std::get<1>(k))), sizeof(std::get<1>(k)));

                ypcap0::TrafficMetric metric;
                *metric.mutable_srcaddress() = src;
                *metric.mutable_dstaddress() = dst;
                metric.set_port(std::get<2>(k));
                metric.set_count(std::get<0>(v));
                metric.set_size(std::get<1>(v));

                std::string serialized;
                metric.SerializeToString(&serialized);

                char* base64 = new char[256];
                boost::beast::detail::base64::encode(base64, serialized.c_str(), serialized.length());
                std::string base64Serialized(base64);
                delete[] base64;

                std::cout << base64Serialized << std::endl;

                this->mProducer->produce("traffic", base64Serialized);
            }
        } else {
            for (const auto& pair : *oldMap) {
                auto k = pair.first;
                auto v = pair.second;
                char src[INET_ADDRSTRLEN], dst[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &std::get<0>(k), src, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &std::get<1>(k), dst, INET_ADDRSTRLEN);
                std::cout << "{ ";
                std::cout << "\"timestamp\": \"" << std::put_time(&tm_timestamp, "%Y-%m-%dT%H:%M:%S") << "\", ";
                std::cout << "\"src\": \"" << src << "\", ";
                std::cout << "\"dst\": \"" << dst << "\", ";
                std::cout << "\"port\": \"" << std::get<2>(k) << "\", ";
                std::cout << "\"count\": \"" << std::get<0>(v) << "\", ";
                std::cout << "\"size\": \"" << std::get<1>(v) << "\" ";
                std::cout << "}" << std::endl;
            }
        }

        delete oldMap;
    }

    std::unique_lock<std::mutex> lock(this->mtx);
}

void TrafficAggry::Process() {
    std::thread flusher(&TrafficAggry::flush, this);

    std::string line;
    map = new std::unordered_map<std::tuple<in_addr, in_addr, uint>, std::tuple<uint, uint>, TrafficHeaderHash, TrafficHeaderEqual>{};

    while (std::getline(std::cin, line)) {
        const char *cline = line.c_str();
        char *deserialized = new char[256];
        boost::beast::detail::base64::decode(deserialized, cline, line.length());
        std::string sline(deserialized);
        ypcap0::IpPacket pkt;
        pkt.ParseFromString(sline);
        uint port = pkt.srcport() < pkt.dstport() ? pkt.srcport() : pkt.dstport();

        std::lock_guard<std::mutex> lock(this->mtx);

        in_addr ip_src = *(reinterpret_cast<const in_addr *>(pkt.srcaddress().address().c_str()));
        in_addr ip_dst = *(reinterpret_cast<const in_addr *>(pkt.dstaddress().address().c_str()));
//        char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
//        inet_ntop(AF_INET, &(ip_src), src_ip, INET_ADDRSTRLEN);
//        inet_ntop(AF_INET, &(ip_dst), dst_ip, INET_ADDRSTRLEN);

        //auto &buffer = (*map)[{ pkt.srcaddress().address(), pkt.dstaddress().address(), port }];
//        auto &buffer = (*map)[{ std::string(src_ip), std::string(dst_ip), port }];
        auto &buffer = (*map)[{ ip_src, ip_dst, port }];

        std::get<0>(buffer)++;
        std::get<1>(buffer) += pkt.size();

        this->cv.notify_one();

        delete[] deserialized;
    }

    isFinished = true;
    cv.notify_one();
    flusher.join();
}

} // namespace ypcap

