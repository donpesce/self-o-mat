(window.webpackJsonp=window.webpackJsonp||[]).push([[42],{Jn9r:function(i,n,t){"use strict";t.r(n),t.d(n,"IonInfiniteScroll",function(){return r}),t.d(n,"IonInfiniteScrollContent",function(){return s});var e=t("B5Ai"),o=t("cBjU"),r=function(){function i(){this.thrPx=0,this.thrPc=0,this.didFire=!1,this.isBusy=!1,this.isLoading=!1,this.threshold="15%",this.disabled=!1,this.position="bottom"}return i.prototype.thresholdChanged=function(i){i.lastIndexOf("%")>-1?(this.thrPx=0,this.thrPc=parseFloat(i)/100):(this.thrPx=parseFloat(i),this.thrPc=0)},i.prototype.disabledChanged=function(i){this.disabled&&(this.isLoading=!1,this.isBusy=!1),this.enableScrollEvents(!i)},i.prototype.componentDidLoad=function(){return e.a(this,void 0,void 0,function(){var i,n,t=this;return e.c(this,function(e){switch(e.label){case 0:return(i=this.el.closest("ion-content"))?[4,i.componentOnReady()]:[3,3];case 1:return e.sent(),n=this,[4,i.getScrollElement()];case 2:n.scrollEl=e.sent(),e.label=3;case 3:return this.thresholdChanged(this.threshold),this.enableScrollEvents(!this.disabled),"top"===this.position&&this.queue.write(function(){t.scrollEl&&(t.scrollEl.scrollTop=t.scrollEl.scrollHeight-t.scrollEl.clientHeight)}),[2]}})})},i.prototype.componentDidUnload=function(){this.scrollEl=void 0},i.prototype.onScroll=function(){var i=this.scrollEl;if(!i||!this.canStart())return 1;var n=this.el.offsetHeight;if(0===n)return 2;var t=i.scrollTop,e=i.offsetHeight,o=0!==this.thrPc?e*this.thrPc:this.thrPx;if(("bottom"===this.position?i.scrollHeight-n-t-o-e:t-n-o)<0){if(!this.didFire)return this.isLoading=!0,this.didFire=!0,this.ionInfinite.emit(),3}else this.didFire=!1;return 4},i.prototype.complete=function(){var i=this,n=this.scrollEl;if(this.isLoading&&n&&(this.isLoading=!1,"top"===this.position)){this.isBusy=!0;var t=n.scrollHeight-n.scrollTop;requestAnimationFrame(function(){i.queue.read(function(){var e=n.scrollHeight-t;requestAnimationFrame(function(){i.queue.write(function(){n.scrollTop=e,i.isBusy=!1})})})})}},i.prototype.canStart=function(){return!(this.disabled||this.isBusy||!this.scrollEl||this.isLoading)},i.prototype.enableScrollEvents=function(i){this.scrollEl&&this.enableListener(this,"scroll",i,this.scrollEl)},i.prototype.hostData=function(){var i;return{class:(i={},i[""+this.mode]=!0,i["infinite-scroll-loading"]=this.isLoading,i["infinite-scroll-enabled"]=!this.disabled,i)}},Object.defineProperty(i,"is",{get:function(){return"ion-infinite-scroll"},enumerable:!0,configurable:!0}),Object.defineProperty(i,"properties",{get:function(){return{complete:{method:!0},disabled:{type:Boolean,attr:"disabled",watchCallbacks:["disabledChanged"]},el:{elementRef:!0},enableListener:{context:"enableListener"},isLoading:{state:!0},position:{type:String,attr:"position"},queue:{context:"queue"},threshold:{type:String,attr:"threshold",watchCallbacks:["thresholdChanged"]}}},enumerable:!0,configurable:!0}),Object.defineProperty(i,"events",{get:function(){return[{name:"ionInfinite",method:"ionInfinite",bubbles:!0,cancelable:!0,composed:!0}]},enumerable:!0,configurable:!0}),Object.defineProperty(i,"listeners",{get:function(){return[{name:"scroll",method:"onScroll",disabled:!0,passive:!0}]},enumerable:!0,configurable:!0}),Object.defineProperty(i,"style",{get:function(){return"ion-infinite-scroll{display:none;width:100%}.infinite-scroll-enabled{display:block}"},enumerable:!0,configurable:!0}),i}(),s=function(){function i(){}return i.prototype.componentDidLoad=function(){void 0===this.loadingSpinner&&(this.loadingSpinner=this.config.get("infiniteLoadingSpinner",this.config.get("spinner","ios"===this.mode?"lines":"crescent")))},i.prototype.hostData=function(){var i;return{class:(i={},i[""+this.mode]=!0,i["infinite-scroll-content-"+this.mode]=!0,i)}},i.prototype.render=function(){return Object(o.b)("div",{class:"infinite-loading"},this.loadingSpinner&&Object(o.b)("div",{class:"infinite-loading-spinner"},Object(o.b)("ion-spinner",{name:this.loadingSpinner})),this.loadingText&&Object(o.b)("div",{class:"infinite-loading-text",innerHTML:this.loadingText}))},Object.defineProperty(i,"is",{get:function(){return"ion-infinite-scroll-content"},enumerable:!0,configurable:!0}),Object.defineProperty(i,"properties",{get:function(){return{config:{context:"config"},loadingSpinner:{type:String,attr:"loading-spinner",mutable:!0},loadingText:{type:String,attr:"loading-text"}}},enumerable:!0,configurable:!0}),Object.defineProperty(i,"style",{get:function(){return"ion-infinite-scroll-content{display:-ms-flexbox;display:flex;-ms-flex-direction:column;flex-direction:column;-ms-flex-pack:center;justify-content:center;min-height:84px;text-align:center;-webkit-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none}.infinite-loading{margin-left:0;margin-right:0;margin-top:0;margin-bottom:32px;display:none;width:100%}.infinite-loading-text{margin-left:32px;margin-right:32px;margin-top:4px;margin-bottom:0}@supports ((-webkit-margin-start:0) or (margin-inline-start:0)) or (-webkit-margin-start:0){.infinite-loading-text{margin-left:unset;margin-right:unset;-webkit-margin-start:32px;margin-inline-start:32px;-webkit-margin-end:32px;margin-inline-end:32px}}.infinite-scroll-loading ion-infinite-scroll-content>.infinite-loading{display:block}.infinite-scroll-content-ios .infinite-loading-text{color:var(--ion-color-step-600,#666)}.infinite-scroll-content-ios .infinite-loading-spinner .spinner-crescent circle,.infinite-scroll-content-ios .infinite-loading-spinner .spinner-lines-ios line,.infinite-scroll-content-ios .infinite-loading-spinner .spinner-lines-small-ios line{stroke:var(--ion-color-step-600,#666)}.infinite-scroll-content-ios .infinite-loading-spinner .spinner-bubbles circle,.infinite-scroll-content-ios .infinite-loading-spinner .spinner-circles circle,.infinite-scroll-content-ios .infinite-loading-spinner .spinner-dots circle{fill:var(--ion-color-step-600,#666)}"},enumerable:!0,configurable:!0}),Object.defineProperty(i,"styleMode",{get:function(){return"ios"},enumerable:!0,configurable:!0}),i}()}}]);