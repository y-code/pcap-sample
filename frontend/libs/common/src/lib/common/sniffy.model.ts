interface SampleData {
  date: string,
  summary: string,
  temperatureC: number
}
export interface TrafficData {
  endpointVertices: EndpointVertexData[];
  protocolClientVertices: ProtocolClientVertexData[];
  protocolServerVertices: ProtocolServerVertexData[];
  endpointProtocolClientEdges: EndpointProtocolEdgeData[];
  endpointProtocolServerEdges: EndpointProtocolEdgeData[];
  trafficEdges: TrafficEdgeData[];
}
export interface EndpointVertexData {
  id: number;
  version: string,
  address: string;
}
export interface ProtocolClientVertexData {
  id: number;
  version: string,
  address: string;
  serverPort: string;
  ports: string[];
}
export interface ProtocolServerVertexData {
  id: number;
  version: string,
  address: string;
  port: string;
}
export interface EndpointProtocolEdgeData {
  id: number;
  startId: number;
  endId: number;
}
export interface TrafficEdgeData {
  id: number;
  startId: number;
  endId: number;
  upSize: number;
  downSize: number;
}
