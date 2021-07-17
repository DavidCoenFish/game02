(function () {
   function Factory() {
      var mAttempt = 0;
      function CallbackPass(in_userData) {
         //"AuthoriseToken"
         App.Client.Client_SetAuthoriseToken(in_userData["AuthoriseToken"]);

         //"StateData"
         var stateData = in_userData["StateData"];
         if (undefined !== stateData) {
            for (const property in stateData) {
               App.Client.Client_SetDataValue(property, stateData[property]);
            }
         }

         //"ViewTemplate"
         //var viewTemplate = in_userData["ViewTemplate"];

         //ClientState
         //var clientState = in_userData["ClientState"];

         //"SetState"
         var stateName = in_userData["SetState"];
         if (undefined !== stateName) {
            App.Client.Client_SetState(stateName);
         }
      }
      function CallbackError() {
         mAttempt++;
         App.Client.LogError("Authorise failed attempt:" + String(mAttempt));
         if (mAttempt < 5) {
            Main();
         } else {
            App.Client.LogError("Authorise failed giving up");
         }
      }
      function Main() {
         App.Network.Client_AsyncGetAuthoriseTokenAndBootstrapData(
            CallbackPass,
            CallbackError,
            "mock_username",
            "mock_password"
         );
      }
      var m_screenName = "boot";
      //App.View.Client_AddScreen("boot", m_screenName, App.Client.Client_MakeDefaultDataSource());
      Main();
      return {
         "Dtor": function () {
            //App.View.Client_RemoveScreen(m_screenName);
         }
      }
   };
   App.Client.Root_AddStateFactory("boot", Factory);
})();
