FROM confluentinc/cp-kafka-connect:7.6.1

RUN confluent-hub install --no-prompt \
    --component-dir /usr/share/confluent-hub-components \
#    --worker-configs /etc/kafka/connect-distributed.properties:/etc/kafka/connect-standalone.properties:/etc/schema-registry/connect-avro-distributed.properties:/etc/schema-registry/connect-avro-standalone.properties:/etc/kafka-connect/kafka-connect.properties \
    confluentinc/kafka-connect-jdbc:latest

