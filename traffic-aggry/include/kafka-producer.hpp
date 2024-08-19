#ifndef H_KAFKA_PRODUCER
#define H_KAFKA_PRODUCER

#include <unordered_map>
#include <memory>
#include <librdkafka/rdkafkacpp.h>

namespace ypcap {

class KafkaProducer {
    private:
        RdKafka::Producer *mProducer;
        std::unordered_map<std::string, std::unique_ptr<RdKafka::Topic>> mTopics;

    public:
        KafkaProducer(RdKafka::Producer *producer);
        ~KafkaProducer();
        RdKafka::Producer *client() { return this->mProducer; }
        void setupTopic(std::string &name, RdKafka::Conf *conf);
        void produce(std::string topic, std::string message);
};

class DeliveryReportCb : public RdKafka::DeliveryReportCb {
    public:
        void dr_cb (RdKafka::Message &message) override;
};

class KafkaProducerFactory {
    private:
        std::string mBroker;

    public:
        KafkaProducerFactory(const std::string &broker);
        std::unique_ptr<KafkaProducer> open();
};

} // namespace ypcap

#endif // H_KAFKA_PRODUCER

