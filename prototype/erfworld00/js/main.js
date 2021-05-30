function main() {
   console.log('js/main.js window load');
   view.Run(document.body, model);
}

if (window.attachEvent) {
   window.attachEvent('onload', main);
} else {
   if (window.onload) {
      var curronload = window.onload;
      var newonload = function (evt) {
         curronload(evt);
         main(evt);
      };
      window.onload = newonload;
   } else {
      window.onload = main;
   }
}
