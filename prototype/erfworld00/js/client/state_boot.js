(function () {
   function Factory(in_defaultState, in_stateData) {
      var mAttempt = 0;
      function CallbackPass(in_userData) {
         //App.View.Client_SetScreen("Loading");
         App.Client.SetAuthoriseToken(in_userData["authoriseToken"]);
         App.Client.SetLocaleModuleData("boot", in_userData["boot"]);
         Object.assign(in_stateData, in_userData["state"]);
         App.View.Client_OnGetUserData();
      }
      function CallbackError() {
         mAttempt++;
         App.View.CallbackError("Authorise failed attempt:" + String(mAttempt));
         if (mAttempt < 5) {
            Main();
         } else {
            App.View.CallbackError("Authorise failed giving up");
         }
      }
      /*
      want to update progress ({0}) => some nb as time passes
      want to not have every state deal with "version" element text request 
      state could just modify state data, such as increment a state progress var, but wat updates that text?
      */
      function RegisterAsyncText(callbackText, callbackError, screenDialogName, elementName) {
         //Progress
         if ("Progress" === elementName) {
         } else {
            App.Client.AsyncGetLocalisedText(
         }
      }
      function Main() {
         App.View.Client_SetScreen("Loading");
         App.Network.Client_GetUserDataAuthoriseToken(
            CallbackPass,
            CallbackError,
            "mock",
            "mock"
         );
      }
      Main();
      return Object.assign(
         in_defaultState,
         {
            //OnClick: App.Unimplemented,
            //OnDrag: App.Unimplemented,
            ////RegisterAsyncEnabled: App.Unimplemented,
            //"RegisterAsyncText": RegisterAsyncText,
            "Dtor": function () {
               App.View.Client_SetScreen(App.View.Client_GetDefaultTemplateName());
            }
         }
      );
   };
   App.Client.AddState("Default", Factory);
})();
