(window.webpackJsonp=window.webpackJsonp||[]).push([[15],{JLuJ:function(n,l,e){"use strict";e.r(l);var u=e("CcnG"),t=function(){return function(){}}(),i=e("pMnS"),o=e("Wxty"),a=e("oBZk"),c=e("ZZ/e"),r=e("gIcY"),b=e("Ip0R"),p=e("yTNM"),g=e("q2yH"),s=e("AytR"),d=function(){function n(n){this.apiService=n,this.pageTitle="Camera Settings"}return n.prototype.updateCameraSettings=function(n){if(!this.apiService.isUpdating)if(null!=this.settings){var l=g.xtech.selfomat.CameraSettings.encode(this.settings).finish(),e=g.xtech.selfomat.CameraSettings.encode(n).finish();l.toString()!==e.toString()&&(this.settings=n)}else this.settings=n},n.prototype.refresh=function(){var n=this;this.apiService.isUpdating||this.apiService.getCameraSettings().subscribe(function(l){return n.updateCameraSettings(l)})},n.prototype.ngOnInit=function(){var n=this;s.a.demo||setInterval(function(){n.refresh()},1e3),this.refresh()},n}(),f=u.mb({encapsulation:0,styles:[o.a],data:{}});function h(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,2,"ion-label",[],null,null,null,a.A,a.h)),u.nb(1,49152,null,0,c.N,[u.h,u.k],null,null),(n()(),u.Cb(2,0,["",""]))],null,function(n,l){n(l,2,0,l.parent.context.$implicit.name)})}function m(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,2,"ion-label",[["slot","start"]],null,null,null,a.A,a.h)),u.nb(1,49152,null,0,c.N,[u.h,u.k],null,null),(n()(),u.Cb(2,0,["",""]))],null,function(n,l){n(l,2,0,l.parent.context.$implicit.name)})}function v(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,6,"ion-toggle",[["slot","end"]],[[2,"ng-untouched",null],[2,"ng-touched",null],[2,"ng-pristine",null],[2,"ng-dirty",null],[2,"ng-valid",null],[2,"ng-invalid",null],[2,"ng-pending",null]],[[null,"ngModelChange"],[null,"ionChange"],[null,"ionBlur"]],function(n,l,e){var t=!0,i=n.component;return"ionBlur"===l&&(t=!1!==u.yb(n,2)._handleBlurEvent()&&t),"ionChange"===l&&(t=!1!==u.yb(n,2)._handleIonChange(e.target.checked)&&t),"ngModelChange"===l&&(t=!1!==(n.parent.context.$implicit.currentValue=e)&&t),"ionChange"===l&&(t=!1!==i.apiService.updateSetting(e,n.parent.context.$implicit)&&t),t},a.K,a.r)),u.nb(1,49152,null,0,c.Ab,[u.h,u.k],null,null),u.nb(2,16384,null,0,c.d,[u.k],null,null),u.zb(1024,null,r.b,function(n){return[n]},[c.d]),u.nb(4,671744,null,0,r.e,[[8,null],[8,null],[8,null],[6,r.b]],{model:[0,"model"]},{update:"ngModelChange"}),u.zb(2048,null,r.c,null,[r.e]),u.nb(6,16384,null,0,r.d,[[4,r.c]],null,null)],function(n,l){n(l,4,0,l.parent.context.$implicit.currentValue)},function(n,l){n(l,0,0,u.yb(l,6).ngClassUntouched,u.yb(l,6).ngClassTouched,u.yb(l,6).ngClassPristine,u.yb(l,6).ngClassDirty,u.yb(l,6).ngClassValid,u.yb(l,6).ngClassInvalid,u.yb(l,6).ngClassPending)})}function C(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,6,"ion-range",[["debounce","50"],["pin","false"],["slot","end"],["step","0.1"]],[[2,"ng-untouched",null],[2,"ng-touched",null],[2,"ng-pristine",null],[2,"ng-dirty",null],[2,"ng-valid",null],[2,"ng-invalid",null],[2,"ng-pending",null]],[[null,"ngModelChange"],[null,"ionChange"],[null,"ionBlur"]],function(n,l,e){var t=!0,i=n.component;return"ionBlur"===l&&(t=!1!==u.yb(n,2)._handleBlurEvent()&&t),"ionChange"===l&&(t=!1!==u.yb(n,2)._handleChangeEvent(e.target.value)&&t),"ngModelChange"===l&&(t=!1!==(n.parent.context.$implicit.currentValue=e)&&t),"ionChange"===l&&(t=!1!==i.apiService.updateSetting(e,n.parent.context.$implicit)&&t),t},a.D,a.k)),u.nb(1,49152,null,0,c.bb,[u.h,u.k],{debounce:[0,"debounce"],min:[1,"min"],max:[2,"max"],pin:[3,"pin"],step:[4,"step"]},null),u.nb(2,16384,null,0,c.Kb,[u.k],null,null),u.zb(1024,null,r.b,function(n){return[n]},[c.Kb]),u.nb(4,671744,null,0,r.e,[[8,null],[8,null],[8,null],[6,r.b]],{model:[0,"model"]},{update:"ngModelChange"}),u.zb(2048,null,r.c,null,[r.e]),u.nb(6,16384,null,0,r.d,[[4,r.c]],null,null)],function(n,l){n(l,1,0,"50",l.parent.context.$implicit.minValue,l.parent.context.$implicit.maxValue,"false","0.1"),n(l,4,0,l.parent.context.$implicit.currentValue)},function(n,l){n(l,0,0,u.yb(l,6).ngClassUntouched,u.yb(l,6).ngClassTouched,u.yb(l,6).ngClassPristine,u.yb(l,6).ngClassDirty,u.yb(l,6).ngClassValid,u.yb(l,6).ngClassInvalid,u.yb(l,6).ngClassPending)})}function x(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,2,"ion-select-option",[],null,null,null,a.E,a.m)),u.nb(1,49152,null,0,c.nb,[u.h,u.k],{value:[0,"value"]},null),(n()(),u.Cb(2,0,["",""]))],function(n,l){n(l,1,0,l.context.index)},function(n,l){n(l,2,0,l.context.$implicit)})}function y(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,8,"ion-select",[["interface","popover"],["slot","end"]],[[2,"ng-untouched",null],[2,"ng-touched",null],[2,"ng-pristine",null],[2,"ng-dirty",null],[2,"ng-valid",null],[2,"ng-invalid",null],[2,"ng-pending",null]],[[null,"ngModelChange"],[null,"ionChange"],[null,"ionBlur"]],function(n,l,e){var t=!0,i=n.component;return"ionBlur"===l&&(t=!1!==u.yb(n,2)._handleBlurEvent()&&t),"ionChange"===l&&(t=!1!==u.yb(n,2)._handleChangeEvent(e.target.value)&&t),"ngModelChange"===l&&(t=!1!==(n.parent.context.$implicit.currentIndex=e)&&t),"ionChange"===l&&(t=!1!==i.apiService.updateSetting(e,n.parent.context.$implicit)&&t),t},a.F,a.l)),u.nb(1,49152,null,0,c.mb,[u.h,u.k],{interface:[0,"interface"],value:[1,"value"]},null),u.nb(2,16384,null,0,c.Kb,[u.k],null,null),u.zb(1024,null,r.b,function(n){return[n]},[c.Kb]),u.nb(4,671744,null,0,r.e,[[8,null],[8,null],[8,null],[6,r.b]],{model:[0,"model"]},{update:"ngModelChange"}),u.zb(2048,null,r.c,null,[r.e]),u.nb(6,16384,null,0,r.d,[[4,r.c]],null,null),(n()(),u.fb(16777216,null,0,1,null,x)),u.nb(8,278528,null,0,b.h,[u.N,u.K,u.s],{ngForOf:[0,"ngForOf"]},null)],function(n,l){var e=l.component;n(l,1,0,"popover",l.parent.context.$implicit.currentIndex),n(l,4,0,l.parent.context.$implicit.currentIndex),n(l,8,0,e.apiService.getList(l.parent.context.$implicit).values)},function(n,l){n(l,0,0,u.yb(l,6).ngClassUntouched,u.yb(l,6).ngClassTouched,u.yb(l,6).ngClassPristine,u.yb(l,6).ngClassDirty,u.yb(l,6).ngClassValid,u.yb(l,6).ngClassInvalid,u.yb(l,6).ngClassPending)})}function S(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,2,"ion-note",[["slot","end"]],null,null,null,a.C,a.j)),u.nb(1,49152,null,0,c.X,[u.h,u.k],null,null),(n()(),u.Cb(2,0,["",""]))],null,function(n,l){n(l,2,0,l.parent.context.$implicit.value)})}function $(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,0,"input",[["type","file"]],[[8,"id",0],[8,"accept",0]],[[null,"change"]],function(n,l,e){var u=!0;return"change"===l&&(u=!1!==n.component.apiService.fileUpload(e,n.parent.context.$implicit)&&u),u},null,null))],null,function(n,l){n(l,0,0,u.qb(1,"input_",l.parent.context.index,""),u.qb(1,"",l.parent.context.$implicit.inputAccept,""))})}function k(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,15,"ion-item",[["lines","none"]],null,[[null,"click"]],function(n,l,e){var u=!0;return"click"===l&&(u=!1!==n.component.apiService.clickItem(e,n.context.$implicit,n.context.index)&&u),u},a.z,a.g)),u.nb(1,49152,null,0,c.H,[u.h,u.k],{detail:[0,"detail"],lines:[1,"lines"]},null),(n()(),u.fb(16777216,null,0,1,null,h)),u.nb(3,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null),(n()(),u.fb(16777216,null,0,1,null,m)),u.nb(5,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null),(n()(),u.fb(16777216,null,0,1,null,v)),u.nb(7,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null),(n()(),u.fb(16777216,null,0,1,null,C)),u.nb(9,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null),(n()(),u.fb(16777216,null,0,1,null,y)),u.nb(11,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null),(n()(),u.fb(16777216,null,0,1,null,S)),u.nb(13,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null),(n()(),u.fb(16777216,null,0,1,null,$)),u.nb(15,16384,null,0,b.i,[u.N,u.K],{ngIf:[0,"ngIf"]},null)],function(n,l){var e=l.component;n(l,1,0,e.apiService.isPost(l.context.$implicit)||e.apiService.isFileUpload(l.context.$implicit)||e.apiService.isLink(l.context.$implicit),"none"),n(l,3,0,!e.apiService.isNumber(l.context.$implicit.minValue)),n(l,5,0,e.apiService.isNumber(l.context.$implicit.minValue)),n(l,7,0,e.apiService.isBoolean(l.context.$implicit.currentValue)),n(l,9,0,e.apiService.isNumber(l.context.$implicit.minValue)),n(l,11,0,e.apiService.isList(l.context.$implicit)),n(l,13,0,e.apiService.isReadOnly(l.context.$implicit)),n(l,15,0,e.apiService.isFileUpload(l.context.$implicit))},null)}function I(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,6,"ion-header",[],null,null,null,a.x,a.e)),u.nb(1,49152,null,0,c.B,[u.h,u.k],null,null),(n()(),u.ob(2,0,null,0,4,"ion-toolbar",[],null,null,null,a.L,a.s)),u.nb(3,49152,null,0,c.Bb,[u.h,u.k],null,null),(n()(),u.ob(4,0,null,0,2,"ion-title",[],null,null,null,a.J,a.q)),u.nb(5,49152,null,0,c.zb,[u.h,u.k],null,null),(n()(),u.Cb(6,0,[" "," "])),(n()(),u.ob(7,0,null,null,5,"ion-content",[],null,null,null,a.u,a.b)),u.nb(8,49152,null,0,c.u,[u.h,u.k],null,null),(n()(),u.ob(9,0,null,0,3,"ion-list",[],null,null,null,a.B,a.i)),u.nb(10,49152,null,0,c.O,[u.h,u.k],null,null),(n()(),u.fb(16777216,null,0,1,null,k)),u.nb(12,278528,null,0,b.h,[u.N,u.K,u.s],{ngForOf:[0,"ngForOf"]},null)],function(n,l){var e=l.component;n(l,12,0,e.apiService.values(e.settings))},function(n,l){n(l,6,0,l.component.pageTitle)})}function w(n){return u.Db(0,[(n()(),u.ob(0,0,null,null,1,"app-tab1",[],null,null,null,I,f)),u.nb(1,114688,null,0,d,[p.a],null,null)],function(n,l){n(l,1,0)},null)}var D=u.kb("app-tab1",d,w,{},{},[]),N=e("ZYCi");e.d(l,"Tab1PageModuleNgFactory",function(){return B});var B=u.lb(t,[],function(n){return u.vb([u.wb(512,u.j,u.ab,[[8,[i.a,D]],[3,u.j],u.x]),u.wb(4608,b.k,b.j,[u.u,[2,b.q]]),u.wb(4608,c.c,c.c,[u.z,u.g]),u.wb(4608,c.Gb,c.Gb,[c.c,u.j,u.q,b.c]),u.wb(4608,c.Jb,c.Jb,[c.c,u.j,u.q,b.c]),u.wb(4608,r.g,r.g,[]),u.wb(1073742336,b.b,b.b,[]),u.wb(1073742336,c.Db,c.Db,[]),u.wb(1073742336,r.f,r.f,[]),u.wb(1073742336,r.a,r.a,[]),u.wb(1073742336,N.n,N.n,[[2,N.t],[2,N.m]]),u.wb(1073742336,t,t,[]),u.wb(1024,N.k,function(){return[[{path:"",component:d}]]},[])])})}}]);