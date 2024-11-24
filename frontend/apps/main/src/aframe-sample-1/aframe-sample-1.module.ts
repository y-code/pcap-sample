import { CUSTOM_ELEMENTS_SCHEMA, NgModule } from "@angular/core";
import { AFrameSample1Component } from "./aframe-sample-1.component";
import { CommonModule } from "@angular/common";
import { RouterModule } from "@angular/router";

@NgModule({
  imports: [
    CommonModule,
    RouterModule.forChild([
      {
        path: '',
        pathMatch: 'full',
        component: AFrameSample1Component,
      },
    ]),
  ],
  declarations: [
    AFrameSample1Component,
  ],
  schemas: [ CUSTOM_ELEMENTS_SCHEMA ],
})
export class AFrameSample1Module {}
