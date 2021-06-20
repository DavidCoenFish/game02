(function () {
   var mUserData
   App.Client.View_OnClick = function (screenDialogName, elementName, groupData, verb, subject, x, y) {
   };
   App.Client.View_OnDrag = function (screenDialogName, elementName, groupData, verb, subject, dx, dy, dragFinished) {
   };
   App.Client.View_RegisterAsyncEnabled = function (callbackEnabled, callbackError, screenDialogName, elementName) {
   };
   App.Client.View_RemoveAsyncEnabled = function (callbackEnabled, callbackError) {
   };
   App.Client.View_RegisterAsyncText = function (callbackText, callbackError, screenDialogName, elementName) {
   };
   App.Client.View_RemoveAsyncText = function (callbackText, callbackError) {
   };
   function Main() {
      App.View.Client_SetScreen("loading0", "templateLoading", null);
   }
   App.RegisterOnLoad(Main);
})();



//Erfworld00.Model = (function () {
//   var sScreenMap = {
//      "unimplemented": {
//         "template": "infoDialog",
//         "body": "this function has yet to be implemented",
//         "buttons": [
//            {
//               "name": "ok",
//               "body": "ok",
//               "verb": "pop"
//            }
//         ]
//      },
//      "mainMenu": {
//         "template": "verticalButtonStackScreen",
//         "body": "0.0.0.0",
//         "buttons": [
//            { "name": "new", "body": "new game", "verb": "push", "subject0": "new" },
//            { "name": "continue", "body": "continue", "enabled": "ContinueEnabled", "verb": "push", "subject0": "continue" },
//            { "name": "load", "body": "load", "enabled": "LoadEnabled", "verb": "push", "subject0": "load" },
//            { "name": "save", "body": "save", "enabled": "SaveEnabled", "verb": "push", "subject0": "save" },
//            { "name": "edit", "body": "edit", "enabled": "EditEnabled", "verb": "push", "subject0": "edit" },
//         ]
//      }
//   };
//   var mCurrentScreenName = "mainMenu";
//   var mScreenNameStack = [mCurrentScreenName];

//   var PushScreen = function (in_screenName) {
//      mCurrentScreenName = in_screenName;
//      mScreenNameStack.push(in_screenName);
//   }
//   var PopScreen = function () {
//      mScreenNameStack.pop();
//      mCurrentScreenName = mScreenNameStack[mScreenNameStack.length - 1];
//   }
//   var SetScreen = function (in_screenName) {
//      mScreenNameStack = [in_screenName];
//      mCurrentScreenName = in_screenName;
//   }

//   var GetCurrentScreenName = function () {
//      return mCurrentScreenName;
//   }
//   var GetScreenData = function (in_screenName) {
//      if (in_screenName in sScreenMap) {
//         return sScreenMap[in_screenName]
//      }
//      return sScreenMap["unimplemented"];
//   }
//   //take navigation functions (set screen, push dialog, pop dialog)
//   var OnScreenButton = function (in_onSetNewScreenCallback, in_stateData, in_screenName, in_buttonName, in_verb, in_subject0) {
//      console.log("OnScreenButton in_screenName:" + in_screenName + " in_buttonName:" + in_buttonName + " in_verb:" + in_verb + " in_subject0:" + in_subject0);
//      var setScreen = false;
//      switch (in_verb) {
//         default:
//            break;
//         case "set":
//            SetScreen(in_subject0);
//            setScreen = true;
//            break;
//         case "push":
//            PushScreen(in_subject0);
//            setScreen = true;
//            break;
//         case "pop":
//            PopScreen();
//            setScreen = true;
//            break;
//      }
//      if ((true === setScreen) && (in_onSetNewScreenCallback)) {
//         in_onSetNewScreenCallback();
//      }
//   }

//   //"mainMenuSaveEnabled", "mainMenuEditEnabled",
//   var GetBool = function (in_key) {
//      return false;
//   }

//   return {
//      "GetCurrentScreenName": GetCurrentScreenName,
//      "GetScreenData": GetScreenData,
//      "OnScreenButton": OnScreenButton,
//      "GetBool": GetBool,
//   }
//})();