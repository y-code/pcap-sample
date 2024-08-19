package ycode.ypcap;

import org.apache.kafka.connect.errors.DataException;
import org.apache.kafka.connect.storage.Converter;
import org.apache.kafka.connect.data.Schema;
import org.apache.kafka.connect.data.SchemaAndValue;
import org.apache.kafka.connect.data.Struct;
import org.apache.kafka.connect.data.SchemaBuilder;

import java.util.Map;
import java.util.Base64;

import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Message;

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
                .field("src_version", Schema.INT16_SCHEMA)
                .field("src_address", Schema.BYTES_SCHEMA)
                .field("dst_version", Schema.INT16_SCHEMA)
                .field("dst_address", Schema.BYTES_SCHEMA)
                .field("port", Schema.INT32_SCHEMA)
                .field("count", Schema.INT32_SCHEMA)
                .field("size", Schema.INT32_SCHEMA)
                .build();
            Struct struct = new Struct(schema);
            if (message.getSrcAddress().getVersion() == Packet.IpAddress.AddressVersion.v4)
                struct.put("src_version", (short)4);
            else if (message.getSrcAddress().getVersion() == Packet.IpAddress.AddressVersion.v6)
                struct.put("src_version", (short)6);
            else
                struct.put("src_version", (short)0);
            struct.put("src_address", message.getSrcAddress().getAddress().toByteArray());
            if (message.getDstAddress().getVersion() == Packet.IpAddress.AddressVersion.v4)
                struct.put("dst_version", (short)4);
            else if (message.getDstAddress().getVersion() == Packet.IpAddress.AddressVersion.v6)
                struct.put("dst_version", (short)6);
            else
                struct.put("dst_version", (short)0);
            struct.put("dst_address", message.getDstAddress().getAddress().toByteArray());
            struct.put("port", message.getPort());
            struct.put("count", message.getCount());
            struct.put("size", message.getSize());

            return new SchemaAndValue(struct.schema(), struct);
        } catch (InvalidProtocolBufferException e) {
            throw new DataException("Error deserializing a protobuf message", e);
        }
    }
}
