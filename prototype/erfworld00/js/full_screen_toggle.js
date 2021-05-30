function full_screen_toggle() {
   console.log('js/full_screen_toggle.js window load');

   var button = document.createElement("button");
   button.innerHTML = "Fullscreen";
   button.style.zIndex = "1";
   button.style.position = "absolute";
   button.style.left = "50%";
   button.style.marginLeft = "-8em";
   button.style.width = "16em";

   document.body.appendChild(button);

   button.addEventListener("click", function () {
      if (document.fullscreenElement) {
         document.exitFullscreen();
      } else {
         document.documentElement.requestFullscreen();
      }
   });

   document.addEventListener('fullscreenchange', (event) => {
      if (document.fullscreenElement) {
         console.log("fullscreenchange with fullscreenElement");
         button.style.display = "none";
      } else {
         console.log("fullscreenchange null fullscreenElement");
         //delete button.style.display;
         button.style.display = "inline";
      }
   });
}

if (window.attachEvent) {
   window.attachEvent('onload', full_screen_toggle);
} else {
   if (window.onload) {
      var curronload = window.onload;
      var newonload = function (evt) {
         curronload(evt);
         full_screen_toggle(evt);
      };
      window.onload = newonload;
   } else {
      window.onload = full_screen_toggle;
   }
}
