import { Route } from '@angular/router';

export const appRoutes: Route[] = [
  {
    path: '',
    pathMatch: 'full',
    redirectTo: 'force-graph-sample-1',
  },
  {
    path: 'threejs-sample',
    loadChildren: () => import('../threejs-sample/threejs-sample.module').then(m => m.ThreeJsSampleModule),
  },
  {
    path: 'aframe-sample-1',
    loadChildren: () => import('../aframe-sample-1/aframe-sample-1.module').then(m => m.AFrameSample1Module),
  },
  {
    path: 'force-graph-sample-1',
    loadChildren: () => import('../force-graph-sample-1/force-graph-sample-1.module').then(m => m.ForceGraphSample1Module),
  },
];
