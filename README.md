# Sniffy - PCAP Sandbox

## Network Traffic Visualization with Three.js

This is a visualization of network traffic.

Monitoring network packets, the Sniffy tool aggregates the data to see communications between endpoints by protocol.

Every network endpoint in the diagram has an IP address and communication protocols are connected in thick lines. The protocols are displayed with their significant port number.

![Sniffy Network Traffic](https://github.com/y-code/pcap-sample/raw/main/docs/img/sniffy-network-traffic.gif)

## Development

### 1. Build Custom Kafka Connect Value Converter

When Kafka Connect JDBC Sink reads the sniffy protobuf data, it needs a custom value converter to read and deserialize the data from a topic and map it to the database shcema.

Build it with the command below.

```
docker run --rm -it --name java-dev -v $HOME/.m2/:/root/.m2/ -v ${REPO_ROOT}:/tmp/ -w /tmp/kafka-connect-sink maven:3.9.9-amazoncorretto-8-debian bash -c "apt-get update -y && apt-get install -y protobuf-compiler && mvn clean package"
```

### 2. Run Docker Containers

Let's run four required docker containers by docker compose. It will start Kafka, Kafka Connect, Kafdrop, and PostgreSQL servers in the containers.

[NOTE] For the Kafka Connect container, the docker builder will run to install Kafka Connect's JDBC sink plugin library using `confluent-hub` utility.

```
docker compose --project-directory ./ypcap-middleware up -d
```

### 3. Initialize Database

To create the `sniffy` database and tables in it, run the following Entity Framework command.

```
dotnet ef database update --project ./sniffy-webapi/Ycode.Sniffy.Domain
```

NOTE: The Kafka Connect Sink can also create the required table, but it cannot create the database.

### 4. Set up JDBC Sink in Kafka Connect

Set up a JDBC sink with a custom value converter, which was built in the step 1 above, by sending a sink configuration to the Kafka Connect API.

```
curl -X POST -H "Content-Type: application/json" --data @${REPO_ROOT}/ypcap-middleware/kafka-connect/plugins/ypcap-metrics-sink.json http://localhost:8083/connectors
```

If the sink setup was successful, the command below will show _RUNNING_ status.

```
curl -X GET http://localhost:8083/connectors/ypcap-metrics-sink/status
```

### 5. Install Protobuf Compiler

```
apt-get install protobuf-compiler
```
or
```
brew install protobuf
```

### 6. Build Sniffy

```
cmake -B build
cmake --build build
```

### 7. Run Sniffy

Now, it is ready to run Sniffy agent.

```
sudo ./build/sniffy | ./build/traffic-aggry -k -v
```

The summary of the data sent to the Kafka topic can be monitored on Kafdrop http://localhost:9000/topic/traffic.

### 8. Install PLpgSQL Stored Procedure for Data Analysis with Apache AGE

```
docker exec -it postgresql psql -U postgres -d sniffy -a -f /var/lib/postgresql/data/ddl/init.sql
docker exec -it postgresql psql -U postgres -d sniffy -a -f /var/lib/postgresql/data/ddl/create_graph_from_table.sql
```

### 9. Run Data Analysis with Apache AGE

```
docker exec -it postgresql psql -U postgres -d sniffy -a -c "SET search_path = ag_catalog, \"\$user\", public; CALL create_graph_from_table('graph_1', 'traffic_metrics');"
```

`graph_1` can be modified, but it should match `GraphSchema` in `sniffy-webapi/Ycode.Sniffy.WebApi/appsettings.json` file.

### 10. Run Web Services for Visualization with Three.js

Run the backend first.

```
dotnet run --project ./sniffy-webapi/Ycode.Sniffy.WebApi 
```

In another terminal, run the frontend development server.

```
cd $REPO_ROOT/frontend
npx nx serve main
```

Now, open http://localhost:4200/ on browser.
