package ycode.ypcap;

import org.apache.kafka.connect.errors.DataException;
import org.apache.kafka.connect.storage.Converter;
import org.apache.kafka.connect.data.Schema;
import org.apache.kafka.connect.data.SchemaAndValue;
import org.apache.kafka.connect.data.Struct;
import org.apache.kafka.connect.data.SchemaBuilder;

import java.util.Map;
import java.util.Base64;
import java.util.Date;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Message;
import com.google.protobuf.Timestamp;

import ypcap0.Packet;

public class ProtobufBase64Converter implements Converter {
    @Override
    public void configure(Map<String, ?> configs, boolean isKey) {
    }

    @Override
    public byte[] fromConnectData(String topic, Schema schema, Object value) {
        throw new UnsupportedOperationException("Serialization not supported");
    }

    @Override
    public SchemaAndValue toConnectData(String topic, byte[] value) {
        byte[] decodedBytes = Base64.getDecoder().decode(value);

        try {
            Packet.TrafficMetric message = Packet.TrafficMetric.parseFrom(decodedBytes);

            Schema schema = SchemaBuilder.struct()
                .field("timestamp", SchemaBuilder.int64().name("org.apache.kafka.connect.data.Timestamp").build())
                .field("up_version", Schema.INT16_SCHEMA)
                .field("up_address", Schema.BYTES_SCHEMA)
                .field("down_version", Schema.INT16_SCHEMA)
                .field("down_address", Schema.BYTES_SCHEMA)
                .field("up_port", Schema.INT32_SCHEMA)
                .field("down_ports", SchemaBuilder.array(Schema.INT32_SCHEMA).build())
                .field("up_size", Schema.INT32_SCHEMA)
                .field("down_size", Schema.INT32_SCHEMA)
                .build();
            Struct struct = new Struct(schema);
            struct.put("timestamp", new Date(message.getTimestamp().getSeconds() * 1000 + message.getTimestamp().getNanos() / 1000000));
            if (message.getUpAddress().getVersion() == Packet.IpAddress.AddressVersion.v4)
                struct.put("up_version", (short)4);
            else if (message.getUpAddress().getVersion() == Packet.IpAddress.AddressVersion.v6)
                struct.put("up_version", (short)6);
            else
                struct.put("up_version", (short)0);
            struct.put("up_address", message.getUpAddress().getAddress().toByteArray());
            if (message.getDownAddress().getVersion() == Packet.IpAddress.AddressVersion.v4)
                struct.put("down_version", (short)4);
            else if (message.getDownAddress().getVersion() == Packet.IpAddress.AddressVersion.v6)
                struct.put("down_version", (short)6);
            else
                struct.put("down_version", (short)0);
            struct.put("down_address", message.getDownAddress().getAddress().toByteArray());
            struct.put("up_port", message.getUpPort());
            struct.put("down_ports", message.getDownPortsList());
            struct.put("up_size", message.getUpSize());
            struct.put("down_size", message.getDownSize());

            return new SchemaAndValue(struct.schema(), struct);
        } catch (InvalidProtocolBufferException e) {
            throw new DataException("Error deserializing a protobuf message", e);
        }
    }
}
