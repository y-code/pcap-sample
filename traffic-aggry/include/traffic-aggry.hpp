#ifndef H_TRAFFIC_AGGRY
#define H_TRAFFIC_AGGRY

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <memory>
#include <netinet/ip.h>
#include "packet.pb.h"
#include "kafka-producer.hpp"

namespace ypcap {

struct TrafficHeaderHash {
    template<typename T1, typename T2, typename T3>
    std::size_t operator () (const std::tuple<T1, T2, T3> &x) const {
        std::size_t hash1 = std::hash<T1>{}(std::get<0>(x));
        std::size_t hash2 = std::hash<T2>{}(std::get<1>(x));
        std::size_t hash3 = std::hash<T3>{}(std::get<2>(x));
        return hash1 ^ (hash2 << 1) ^ (hash3 << 2);
    }
};

struct TrafficHeaderEqual {
    template<typename T1, typename T2, typename T3>
    bool operator () (const std::tuple<T1, T2, T3> &x, const std::tuple<T1, T2, T3> &y) const {
        return std::get<0>(x) == std::get<0>(y)
            && std::get<1>(x) == std::get<1>(y)
            && std::get<2>(x) == std::get<2>(y);
    }
};

class TrafficAggry {

public:
    TrafficAggry();
    ~TrafficAggry();
    void Process();

private:
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<bool> isFinished{false};
    std::unordered_map<std::tuple<in_addr, in_addr, uint>, std::tuple<uint, uint>, TrafficHeaderHash, TrafficHeaderEqual> *map;
    std::chrono::milliseconds flushInterval{1000};
    std::unique_ptr<ypcap::KafkaProducerFactory> mProducerFactory;
    std::unique_ptr<ypcap::KafkaProducer> mProducer;

    void flush();
};

}

#endif

