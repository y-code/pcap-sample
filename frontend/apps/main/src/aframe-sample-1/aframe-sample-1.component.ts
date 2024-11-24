import { Component, ViewChild } from "@angular/core";
import { Vector3 } from 'three';
import {utils, } from 'aframe';

@Component({
  selector: 'threejs-sample-1-aframe-sample-1',
  templateUrl: './aframe-sample-1.component.html',
  styleUrl: './aframe-sample-1.component.scss',
})
export class AFrameSample1Component {
  @ViewChild('ascene')
  aScene: any;

  boxSequence = 0;
  boxes: {id: number, position: Vector3}[] = [
    { id: this.boxSequence++, position: new Vector3(-1, 0.5, -3) },
  ];

  add() {utils.coordinates
    this.boxes = [
      ...this.boxes,
      { id: this.boxSequence++, position: new Vector3(-1 + this.boxSequence - 1, 0.5 + this.boxSequence - 1, -3) },
    ];
    console.log(this.aScene.nativeElement.object3D);
  }

  stringify = utils.coordinates.stringify;
}
