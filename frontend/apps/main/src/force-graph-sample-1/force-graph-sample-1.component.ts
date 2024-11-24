import { Component, ElementRef, inject, ViewChild } from "@angular/core";
import ForceGraph3D from '3d-force-graph';
import { CSS2DObject, CSS2DRenderer } from "three/examples/jsm/Addons";
import { Renderer } from "three";
import { tap } from "rxjs";
import { ProtocolClientVertexData, ProtocolServerVertexData, SniffyService } from '@sample-1/common';
import { ForceLink, SimulationNodeDatum, SimulationLinkDatum } from 'd3-force';

@Component({
  selector: 'threejs-sample-1-force-graph-sample-1',
  templateUrl: './force-graph-sample-1.component.html',
  styleUrl: './force-graph-sample-1.component.scss',
})
export class ForceGraphSample1Component {
  common = inject(SniffyService);

  flow$ = this.common.getFlows$();

  private _forceGraphView?: ElementRef;
  @ViewChild('forceGraphView')
  set forceGraphView(ref: ElementRef) {
    if (this._forceGraphView)
      return;

    this._forceGraphView = ref;

    const graph = ForceGraph3D({
      extraRenderers: [new CSS2DRenderer() as unknown as Renderer]
    })(this._forceGraphView.nativeElement)
      .linkWidth(link => (link as { upSize: number|undefined }).upSize == null ? 10 : 1)
      .linkDirectionalArrowLength(link => (link as { upSize: number|undefined }).upSize == null ? 1 : 10)
      .width(1000).height(800)
      .forceEngine('d3');

    // (graph.d3Force('link') as ForceLink<SimulationNodeDatum, SimulationLinkDatum<SimulationNodeDatum>>)
    //   .strength(link => 1 / ((link as unknown as {size: number|undefined}).size ?? 1));

    this.flow$.pipe(
      tap(data => graph
        .linkLabel(link => ''+((link as {upSize: number}).upSize == null ? '' : `[up:${(link as {upSize: number}).upSize}, down:${(link as {downSize: number}).downSize}]`))
        .graphData({
          nodes: [
            ...data.endpointVertices.map(x => ({...x})),
            ...data.protocolClientVertices.map(x => ({...x, protocol: x.serverPort})),
            ...data.protocolServerVertices.map(x => ({...x, protocol: x.port})),
          ],
          links: [
            ...data.endpointProtocolClientEdges.map(x => ({ index: x.id, source: x.startId, target: x.endId })),
            ...data.endpointProtocolServerEdges.map(x => ({ index: x.id, source: x.startId, target: x.endId })),
            ...data.trafficEdges.map(x => ({ id: x.id, source: x.startId, target: x.endId, upSize: x.upSize, downSize: x.downSize })),
          ],
        })
        .enableNodeDrag(false)
        .nodeAutoColorBy('protocol')
        .nodeThreeObject(node => {
          const data = node as (ProtocolClientVertexData | ProtocolServerVertexData) & { protocol: number };
          const nodeEl = document.createElement('div');
          nodeEl.innerHTML = '<div>'+(data.protocol ? '<div class="protocol">'+data.protocol+'</div>' : '<div>'+data.address+'</div>')+'</div>';
          nodeEl.style.color = '#aaaaaa';//node.color;
          nodeEl.className = 'node-label';
          return new CSS2DObject(nodeEl);
        })
        .nodeThreeObjectExtend(true)
      ),
    ).subscribe();
  }

  ngOnInit(): void {;
  }
}
