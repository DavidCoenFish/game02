(function () {
   function Factory() {

      var m_screenName = "launch";
      var m_arrayButtonData = [
         {
            "text": "New Game",
            "object": "NewGame",
         },
         {
            "text": "Continue",
            "object": "ContinueGame",
         },
         {
            "text": "Editor",
            "object": "Editor",
         },
         {
            "text": "Debug save database",
         },
         {
            "text": "Debug load database",
         },
      ];
      function GetValueArray() {
         return m_arrayButtonData;
      }
      ////or move all this into a new game state
      //function NewGame() {
      //   //start progress dialog? overlay?
      //   //request on server a new game
      //   //

      //}
      function OnButtonClick(in_verb, in_object) {
         App.Client.LogInfo(`OnButtonClick ${in_verb} ${in_object}`);
         switch (in_object) {
            default:
               App.Client.Client_SetState(in_object);
               break;
         //   case "NewGame":
         //      NewGame();
         //      break;
         //   case "Continue":
         //      break;
         //   case "Editor":
         //      break;
         }
      }

      var dataSource = {
         "Dtor": function () { },
         "GetValueArray": GetValueArray,
         "OnButtonClick": OnButtonClick,  //verb, object
         "MakeGeneratorLocaleKey": App.Client.Client_MakeGeneratorLocaleKey
      };
      App.View.Client_AddScreen("verticalButtonStack", m_screenName, dataSource, true, undefined, "base");

      return {
         "Dtor": function () {
            App.View.Client_RemoveScreen(m_screenName);
         }
      }
   };
   App.Client.Root_AddStateFactory("Launch", Factory);
}) ();
