import { Component, ElementRef, OnInit, ViewChild } from "@angular/core";
import * as Three from 'three';
import { OrbitControls, GLTFLoader, WebGL } from 'three/examples/jsm/Addons';

@Component({
  selector: 'threejs-sample-1-home',
  templateUrl: './threejs-sample.component.html',
  styleUrl: './threejs-sample.component.scss',
})
export class ThreeJsSampleComponent implements OnInit {
  readonly scene: Three.Scene;
  readonly camera: Three.PerspectiveCamera;
  readonly renderer: Three.WebGLRenderer;

  private _threeView?: ElementRef;
  @ViewChild('three')
  set threeView(ref: ElementRef) {
    if (this._threeView) return;

    this._threeView = ref;

    if (!WebGL.isWebGL2Available()) {
      this._threeView.nativeElement.appendChild(WebGL.getWebGL2ErrorMessage());
      return;
    }

    this._threeView.nativeElement.appendChild(this.renderer.domElement)
    this.onThreeViewInit();
  }

  private _cube?: Three.Mesh;
  private get cube(): Three.Mesh {
    if (this._cube)
      return this._cube;

    const geometry = new Three.BoxGeometry(10, 10, 10);
    const material = new Three.MeshBasicMaterial({ color: 0x00ff00 });
    this._cube = new Three.Mesh( geometry, material );
    return this._cube;
  }

  private _line?: Three.Line;
  private get line(): Three.Line {
    if (this._line)
      return this._line;

    const geometry = new Three.BufferGeometry().setFromPoints([
      new Three.Vector3(-10, 0, 0),
      new Three.Vector3(0, 10, 0),
      new Three.Vector3(10, 0, 0),
    ]);
    const material = new Three.LineBasicMaterial({ color: 0xff0000, linewidth: 10 });
    this._line = new Three.Line(geometry, material);
    return this._line;
  }

  constructor() {
    this.scene = new Three.Scene();
    this.camera = new Three.PerspectiveCamera(75, 600 / 600, 0.1, 1000);
    this.renderer = new Three.WebGLRenderer();
    this.renderer.setSize(600, 600);
  }

  ngOnInit(): void {
  }

  private onThreeViewInit(): void {
    this.scene.add(this.cube);
    this.scene.add(this.line);

    this.camera.position.set(0, 0, 30);
    this.camera.lookAt(0, 0, 0);

    this.renderer.setAnimationLoop(() => this.animate())
  }

  private animate(): void {
    this.cube.rotation.x += 0.01;
    this.cube.rotation.y += 0.01;
    this.renderer.render(this.scene, this.camera);
  }
}
