(function () {
   function Factory() {

      function CreateNewGame() {
         //create a new game
         // on success, goto pregame edit, 
         // on fail, return to launch
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
         CreateNewGame();
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
   App.Client.Root_AddStateFactory("NewGame", Factory);
}) ();