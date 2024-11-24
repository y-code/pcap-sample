import { inject, Injectable } from "@angular/core";
import { HttpClient } from "@angular/common/http";
import { TrafficData } from "./sniffy.model";
import { Observable, of } from "rxjs";

@Injectable({ providedIn: 'root' })
export class SniffyService {
  private http = inject(HttpClient)

  getFlows$(): Observable<TrafficData> {
    return this.http.get<TrafficData>('http://localhost:5062/api/Sniffy/Graph');
    // return of({
    //   endpointVertices: [
    //     { id: 1, ip4: '123.234.0.1' },
    //     { id: 2, ip4: '123.234.0.2' },
    //     { id: 3, ip4: '123.234.0.3' },
    //     { id: 4, ip4: '123.234.0.4' },
    //     { id: 5, ip4: '123.234.0.5' },
    //   ],
    //   protocolVertices: [
    //     { id: 1000, ip4: '123.234.0.1', protocol: 'http' },
    //     { id: 1001, ip4: '123.234.0.1', protocol: 'https' },
    //     { id: 1002, ip4: '123.234.0.1', protocol: 'ssh' },
    //     { id: 1003, ip4: '123.234.0.1', protocol: 'postgresql' },
    //     { id: 2000, ip4: '123.234.0.2', protocol: 'http' },
    //     { id: 2001, ip4: '123.234.0.2', protocol: 'https' },
    //     { id: 2002, ip4: '123.234.0.2', protocol: 'postgresql' },
    //     { id: 3000, ip4: '123.234.0.3', protocol: 'http' },
    //     { id: 3001, ip4: '123.234.0.3', protocol: 'https' },
    //     { id: 3002, ip4: '123.234.0.3', protocol: 'ssh' },
    //     { id: 3003, ip4: '123.234.0.3', protocol: 'postgresql' },
    //     { id: 4000, ip4: '123.234.0.4', protocol: 'http' },
    //     { id: 4001, ip4: '123.234.0.4', protocol: 'https' },
    //     { id: 4002, ip4: '123.234.0.4', protocol: 'ssh' },
    //     { id: 4004, ip4: '123.234.0.4', protocol: 'kafka' },
    //     { id: 5000, ip4: '123.234.0.5', protocol: 'http' },
    //     { id: 5001, ip4: '123.234.0.5', protocol: 'https' },
    //     { id: 5002, ip4: '123.234.0.5', protocol: 'ssh' },
    //     { id: 5004, ip4: '123.234.0.5', protocol: 'kafka' },
    //   ],
    //   protocolEndpointEdges: [
    //     { id: 1001000, endpointId: 1, protocolId: 1000 },
    //     { id: 1001001, endpointId: 1, protocolId: 1001 },
    //     { id: 1001002, endpointId: 1, protocolId: 1002 },
    //     { id: 1001003, endpointId: 1, protocolId: 1003 },
    //     { id: 2001000, endpointId: 2, protocolId: 2000 },
    //     { id: 2001001, endpointId: 2, protocolId: 2001 },
    //     { id: 2001002, endpointId: 2, protocolId: 2002 },
    //     { id: 3001000, endpointId: 3, protocolId: 3000 },
    //     { id: 3001001, endpointId: 3, protocolId: 3001 },
    //     { id: 3001002, endpointId: 3, protocolId: 3002 },
    //     { id: 3001003, endpointId: 3, protocolId: 3003 },
    //     { id: 4001000, endpointId: 4, protocolId: 4000 },
    //     { id: 4001001, endpointId: 4, protocolId: 4001 },
    //     { id: 4001002, endpointId: 4, protocolId: 4002 },
    //     { id: 4001004, endpointId: 4, protocolId: 4004 },
    //     { id: 5001000, endpointId: 5, protocolId: 5000 },
    //     { id: 5001001, endpointId: 5, protocolId: 5001 },
    //     { id: 5001002, endpointId: 5, protocolId: 5002 },
    //     { id: 5001004, endpointId: 5, protocolId: 5004 },
    //   ],
    //   trafficEdges: [
    //     { id: 1000000000, srcId: 1000, dstId: 2000, size: 123 },
    //     { id: 1000000001, srcId: 1000, dstId: 4000, size: 123 },
    //     { id: 1000000002, srcId: 3000, dstId: 4000, size: 123 },
    //     { id: 1000000003, srcId: 3000, dstId: 5000, size: 123 },
    //     { id: 1000000004, srcId: 1001, dstId: 2001, size: 2123 },
    //     { id: 1000000005, srcId: 5001, dstId: 5001, size: 2230 },
    //     { id: 1000000006, srcId: 3001, dstId: 4001, size: 1000 },
    //     { id: 1000000007, srcId: 3001, dstId: 5001, size: 1230 },
    //     { id: 1000000008, srcId: 1002, dstId: 2002, size: 123 },
    //     { id: 1000000009, srcId: 1002, dstId: 4002, size: 123 },
    //     { id: 1000000010, srcId: 3002, dstId: 4002, size: 123 },
    //     { id: 1000000011, srcId: 3002, dstId: 5002, size: 123 },
    //     { id: 1000000015, srcId: 4004, dstId: 5004, size: 123 },
    //   ],
    // } as FlowsData);
  }
}
