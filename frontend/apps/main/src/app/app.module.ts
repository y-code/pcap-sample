import { NgModule } from "@angular/core";
import { RouterModule } from "@angular/router";
import { NgbCollapseModule } from '@ng-bootstrap/ng-bootstrap';
import { AppComponent } from "./app.component";
import { BrowserModule } from "@angular/platform-browser";
import { appRoutes } from "./app.routes";
import { provideHttpClient } from "@angular/common/http";

@NgModule({
  imports: [
    BrowserModule,
    RouterModule.forRoot(appRoutes),
    NgbCollapseModule,
  ],
  declarations: [
    AppComponent,
  ],
  providers: [
    provideHttpClient(),
  ],
  bootstrap: [AppComponent],
})
export class AppModule {}
