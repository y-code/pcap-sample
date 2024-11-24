import { NgModule } from "@angular/core";
import { ForceGraphSample1Component } from "./force-graph-sample-1.component";
import { CommonModule } from "@angular/common";
import { RouterModule } from "@angular/router";

@NgModule({
  imports: [
    CommonModule,
    RouterModule.forChild([
      {
        path: '',
        pathMatch: 'full',
        component: ForceGraphSample1Component,
      }
    ])
  ],
  declarations: [
    ForceGraphSample1Component,
  ]
})
export class ForceGraphSample1Module {}
