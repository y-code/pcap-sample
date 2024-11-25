# Sniffy - PCAP Sandbox

## Network Traffic Visualization with Three.js

This is a visualization of network traffic.

Monitoring network packets, the Sniffy tool aggregates the data to see communications between endpoints by protocol.

Every network endpoint in the diagram has an IP address and communication protocols are connected in thick lines. The protocols are displayed with their significant port number.

![Sniffy Network Traffic](https://github.com/y-code/pcap-sample/raw/main/docs/img/sniffy-network-traffic.gif)

## Development

### 1. Build Custom Kafka Connect Sink

This Kafka Connect Sink reads the sniffy protobuf data from a topic and store it in the PostgreSQL database.

Build it with the command below.

```
docker run --rm -it --name java-dev -v $HOME/.m2/:/root/.m2/ -v $REPO_ROOT:/tmp/ -w /tmp/kafka-connect-sink maven:3.9.9-amazoncorretto-8-debian bash -c "apt-get update -y && apt-get install -y protobuf-compiler && mvn clean package"
```

### 2. Build Kafka Connect Docker Image

To pre-install Kafka Connect's JDBC library using `confluent-hub` utility, build a custom Kafka Connect docker image.

```
docker build -t ycode/kafka-connect:7.6.1 -f ./ypcap-middleware/kafka-connect/Dockerfile .
```

The docker image tag name can be whatever, but it just needs to match the kafka-connect service's image tag in the docker-compose.yml.

### 3. Run Docker Containers

Let's run four required docker containers by docker compose. It will start Kafka, Kafka Connect, Kafdrop, and PostgreSQL servers in the containers.

```
docker compose --project-directory ./ypcap-middleware up -d
```

### 4. Initialize Database

To create the `sniffy` database and tables in it, run the following Entity Framework command.

```
dotnet ef database update --project ./sniffy-webapi/Ycode.Sniffy.Domain
```

NOTE: The Kafka Connect Sink can also create the required table, but it cannot create the database.

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
