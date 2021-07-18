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
      var m_progressCount = 0;
      function UpdateProgress() {
         progressText = ".";
         for (let i = 0; i < m_progressCount; ++i) {
            progressText += ".";
         }
         m_progressCount = (m_progressCount + 1) % 4;
         App.Client.Client_SetDataValue("progressText", progressText);
      }
      var m_progressInterval = setInterval(UpdateProgress, 500);

      function Main() {
         App.Network.Client_AsyncGetAuthoriseTokenAndBootstrapData(
            CallbackPass,
            CallbackError,
            "mock_username",
            "mock_password"
         );
         UpdateProgress();
      }
      var m_screenName = "boot";
      App.View.Client_AddScreen("progress", m_screenName, App.Client.Client_MakeDefaultDataSource(), true, undefined, "base");

      Main();
      return {
         "Dtor": function () {
            App.View.Client_RemoveScreen(m_screenName);
            clearInterval(m_progressInterval);
         }
      }
   };
   App.Client.Root_AddStateFactory("Boot", Factory);
})();
