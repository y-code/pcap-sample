import { CommonModule } from "@angular/common";
import { NgModule } from "@angular/core";
import { RouterModule } from "@angular/router";
import { ThreeJsSampleComponent } from "./threejs-sample.component";

@NgModule({
  imports: [
    CommonModule,
    RouterModule.forChild([
      {
        path: '',
        pathMatch: 'full',
        component: ThreeJsSampleComponent,
      },
    ]),
  ],
  declarations: [
    ThreeJsSampleComponent,
  ],
})
export class ThreeJsSampleModule {}
