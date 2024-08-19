#include <iostream>
#include <stdexcept>
#include "kafka-producer.hpp"

namespace ypcap {

KafkaProducerFactory::KafkaProducerFactory(const std::string &broker)
    : mBroker(broker) {}

std::unique_ptr<KafkaProducer> KafkaProducerFactory::open() {
    RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

    std::string err;
    if (conf->set("bootstrap.servers", this->mBroker, err) != RdKafka::Conf::CONF_OK) {
        throw std::runtime_error("Failed setting bootstrap.servers: " + err);
    }

    DeliveryReportCb deliveryReport;
    conf->set("dr_cb", &deliveryReport, err);
    RdKafka::Producer *producer = RdKafka::Producer::create(conf, err);
    if (!producer) {
        throw std::runtime_error("Failed creating a Kafka producer: " + err);
    }

    return std::make_unique<KafkaProducer>(producer);
}

void DeliveryReportCb::dr_cb (RdKafka::Message &message) {
    if (message.err()) {
        std::cerr << "Failed deliverying a message: " << message.errstr() << std::endl;
    } else {
        std::cout << "Completed deliverying a message to Topic " << message.topic_name() << std::endl;
    }
}

KafkaProducer::KafkaProducer(RdKafka::Producer *producer)
    : mProducer(producer), mTopics(std::unordered_map<std::string, std::unique_ptr<RdKafka::Topic>>{}) {}

KafkaProducer::~KafkaProducer() {
    if (mProducer) {
        // TODO: close
    }
}

void KafkaProducer::setupTopic(std::string &name, RdKafka::Conf *conf) {
    std::string err;
    RdKafka::Topic *topic = RdKafka::Topic::create(this->mProducer, name, conf, err);
    if (!topic) {
        throw std::runtime_error("Failed creating Topic " + name + ": " + err);
    }
    this->mTopics[name] = std::unique_ptr<RdKafka::Topic>(topic);
}

void KafkaProducer::produce(std::string topic, std::string message) {
    auto objTopic = this->mTopics.find(topic);
    if (objTopic == this->mTopics.end()) {
        throw std::runtime_error("Topic " + topic + " has not been configured.");
    }

    RdKafka::ErrorCode err = this->mProducer->produce(
            objTopic->second.get(),
            RdKafka::Topic::PARTITION_UA,
            RdKafka::Producer::RK_MSG_COPY,
            const_cast<char *>(message.c_str()),
            message.size(),
            nullptr,
            nullptr);
    if (err != RdKafka::ERR_NO_ERROR) {
        throw std::runtime_error("Failed producing a message \"" + message + "\"");
    }
}

} // namespace ypcap

