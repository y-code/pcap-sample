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

TrafficAggry::TrafficAggry() {}

TrafficAggry::TrafficAggry(std::shared_ptr<ypcap::KafkaProducerFactory> producerFactory, std::string &topic)
        : mProducerFactory(producerFactory), mTopic(topic) {
    this->mProducer = this->mProducerFactory->open();
    RdKafka::Conf *topicConf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);
    this->mProducer->setupTopic(topic, topicConf);
}

TrafficAggry::~TrafficAggry() {
}

void TrafficAggry::flush() {
    while (!this->isFinished) {
        auto *newMap = new std::unordered_map<std::tuple<in_addr, in_addr, uint>, std::tuple<uint, uint, uint, std::unordered_set<uint>>, TrafficHeaderHash, TrafficHeaderEqual>{};

        std::unique_lock<std::mutex> lock(this->mtx);
        this->cv.wait_for(lock, this->flushInterval, [this] { return this->isFinished.load(); });

        auto *oldMap = this->map;
        this->map = newMap;

        lock.unlock();

        const auto now = std::chrono::system_clock::now();

        if (this->mProducer) {

            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            auto nanos = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count() % 1000000000;
            google::protobuf::Timestamp timestamp;
            timestamp.set_seconds(seconds);
            timestamp.set_nanos(nanos);

            // std::cout << "---" << std::endl;
            for (const auto& pair : *oldMap) {
                auto k = pair.first;
                auto v = pair.second;

                ypcap0::IpAddress up, down;
                up.set_version(ypcap0::IpAddress::v4);
                up.set_address(reinterpret_cast<const char *>(&(std::get<0>(k))), sizeof(std::get<0>(k)));
                down.set_version(ypcap0::IpAddress::v4);
                down.set_address(reinterpret_cast<const char *>(&(std::get<1>(k))), sizeof(std::get<1>(k)));

                ypcap0::TrafficMetric metric;
                *metric.mutable_timestamp() = timestamp;
                *metric.mutable_upaddress() = up;
                *metric.mutable_downaddress() = down;
                metric.set_upport(std::get<2>(k));
                metric.set_upsize(std::get<1>(v));
                metric.set_downsize(std::get<2>(v));
                for (const uint& downPort : std::get<3>(v))
                    metric.add_downports(downPort);

                std::string serialized;
                metric.SerializeToString(&serialized);

                char* base64 = new char[256];
                boost::beast::detail::base64::encode(base64, serialized.c_str(), serialized.length());
                std::string base64Serialized(base64);
                delete[] base64;

                std::cout << base64Serialized << std::endl;

                this->mProducer->produce(this->mTopic, base64Serialized);
            }

        } else {

            const std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
            const std::tm tm_timestamp = *std::localtime(&timestamp);

            for (const auto& pair : *oldMap) {
                auto k = pair.first;
                auto v = pair.second;

                char upAddr[INET_ADDRSTRLEN], downAddr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &std::get<0>(k), upAddr, INET_ADDRSTRLEN);
                inet_ntop(AF_INET, &std::get<1>(k), downAddr, INET_ADDRSTRLEN);
                std::cout << "{ ";
                std::cout << "\"timestamp\": \"" << std::put_time(&tm_timestamp, "%Y-%m-%dT%H:%M:%S") << "\", ";
                std::cout << "\"up\": \"" << upAddr << "\", ";
                std::cout << "\"down\": \"" << downAddr << "\", ";
                std::cout << "\"upPort\": \"" << std::get<2>(k) << "\", ";
                std::cout << "\"downPorts\": [ ";
                for (const uint &port : std::get<3>(v)) {
                    std::cout << port << ", ";
                }
                std::cout << "] ";
                std::cout << "\"count\": \"" << std::get<0>(v) << "\", ";
                std::cout << "\"upSize\": \"" << std::get<1>(v) << "\", ";
                std::cout << "\"downSize\": \"" << std::get<2>(v) << "\" ";
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
    map = new std::unordered_map<std::tuple<in_addr, in_addr, uint>, std::tuple<uint, uint, uint, std::unordered_set<uint>>, TrafficHeaderHash, TrafficHeaderEqual>{};

    while (std::getline(std::cin, line)) {
        const char *cline = line.c_str();
        char *deserialized = new char[256];
        boost::beast::detail::base64::decode(deserialized, cline, line.length());
        std::string sline(deserialized);
        ypcap0::IpPacket pkt;
        pkt.ParseFromString(sline);

        std::lock_guard<std::mutex> lock(this->mtx);

        in_addr ip_src = *(reinterpret_cast<const in_addr *>(pkt.srcaddress().address().c_str()));
        in_addr ip_dst = *(reinterpret_cast<const in_addr *>(pkt.dstaddress().address().c_str()));
        // char src_ip[INET_ADDRSTRLEN], dst_ip[INET_ADDRSTRLEN];
        // inet_ntop(AF_INET, &(ip_src), src_ip, INET_ADDRSTRLEN);
        // inet_ntop(AF_INET, &(ip_dst), dst_ip, INET_ADDRSTRLEN);

        if (pkt.srcport() < pkt.dstport()) {
            uint port = pkt.srcport();

            // auto &buffer = (*map)[{ pkt.upaddress().address(), pkt.downaddress().address(), port }];
            // auto &buffer = (*map)[{ std::string(src_ip), std::string(dst_ip), port }];
            auto &buffer = (*map)[{ ip_src, ip_dst, port }];

            std::get<0>(buffer)++;
            std::get<2>(buffer) += pkt.size();
            std::get<3>(buffer).insert(pkt.dstport());
        } else {
            uint port = pkt.dstport();

            auto &buffer = (*map)[{ ip_dst, ip_src, port }];

            std::get<0>(buffer)++;
            std::get<1>(buffer) += pkt.size();
            std::get<3>(buffer).insert(pkt.srcport());
        }

        this->cv.notify_one();

        delete[] deserialized;
    }

    isFinished = true;
    cv.notify_one();
    flusher.join();
}

} // namespace ypcap

