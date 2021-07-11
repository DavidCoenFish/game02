(function () {
   var mAuthoriseToken = undefined;
   var mStateData = {
      "clientVersion": "0.0.0.0",
      "userId": undefined,
      "language": undefined,
      "region": undefined,
      };
   var mStateFactory = {};
   var mState = undefined;
   var mLocaleCache = {
      "__id__":"en-au",
      "__language__": "English",
      "__region__": "Australia",
      "Progress": "Progress",
      "{0}": "{0}",
      "Client Version:[{0}]": "Client Version:[{0}]",
      "Database Version:[{0}]": "Database Version:[{0}]",
      // for this to work, after things are being pushed through mStateData, there could be a mStateData["error"]? 
      // then AsyncGetLocalisedText would need to resolve data earlier, else it resolves on callback and could have stale mStateData
      //"Error:[{0}]": "Error:[{0}]",
      //"Debug:[{0}]": "Debug:[{0}]",
   };
   //trying to accomodate change of language
   //what modules have been merged into the localeCache, <module name, language-region>
   var mLocaleModule = {
      "default" : "en-au"
   };
   // <modual name, callback chain>
   var mLocaleRequestCallback = {};

   App.Client.View_OnClick = function (screenDialogName, elementName, groupData, verb, subject, x, y) {
      if (mState) {
         mState.OnClick(screenDialogName, elementName, groupData, verb, subject, x, y);
      }
   };
   App.Client.View_OnDrag = function (screenDialogName, elementName, groupData, verb, subject, dx, dy, dragFinished) {
      if (mState) {
         mState.OnDrag(screenDialogName, elementName, groupData, verb, subject, dx, dy, dragFinished);
      }
   };
   App.Client.View_RegisterAsyncEnabled = function (callbackEnabled, callbackError, screenDialogName, elementName) {
      if (mState) {
         mState.RegisterAsyncEnabled(callbackEnabled, callbackError, screenDialogName, elementName);
      }
   };
   //App.Client.View_RemoveAsyncEnabled = function (callbackEnabled, callbackError) {
   //};
   App.Client.View_RegisterAsyncText = function (callbackText, callbackError, screenDialogName, elementName) {
      if (mState) {
         mState.RegisterAsyncText(callbackText, callbackError, screenDialogName, elementName);
      }
   };
   //App.Client.View_RemoveAsyncText = function (callbackText, callbackError) {
   //};
   App.Client.RegisterState = function (name, factory) {
      mStateFactory[name] = factory;
   }
   App.Client.SetState = function (name) {
      if (false === name in mStateFactory) {
         App.View.ConsoleError("State not found:" + String(name));
         return;
      }
      if (mState) {
         mState.Dtor();
         mState = undefined;
      }
      var defaultState = {
         OnClick: App.Unimplemented,
         OnDrag: App.Unimplemented,
         RegisterAsyncEnabled: App.Unimplemented,
         RegisterAsyncText: App.Unimplemented,
         Dtor: App.Unimplemented,
      };
      var factory = mStateFactory[name];
      mState = factory(defaultState, mStateData);
   }
   App.Client.SetAuthoriseToken = function (authoriseToken) {
      mAuthoriseToken = authoriseToken;
   }

   function ResolveLocalisedTextData(element) {
      var tokenArray = element.split(".");
      var result = "";
      var trace = mStateData;
      tokenArray.forEach(function (element, index, array) {
         if (element in trace) {
            trace = trace[element];
         }
      });
      if (trace !== mStateData) {
         result = String(trace);
      }
      return result;
   }

   //example data ["clientVersion", "someHashInStateData.anotherHash.someKey"]
   function ResolveLocalisedText(key, data) {
      var value = key;
      if (key in mLocaleCache) {
         value = mLocaleCache[key];
      }
      if (data && value) {
         var arrayValue = [];
         data.forEach(function (element, index, array) {
            arrayValue[index] = ResolveLocalisedTextData(element);
         });
         value.replace(/{(\d+)}/g, function (match, number) {
            return typeof arrayValue[number] !== "undefined" ? arrayValue[number] : match;
         });
      }
      return value;
   }
   App.Client.AsyncGetLocalisedText = function (in_callback, in_module, in_key, in_data) {
      var module = (undefined === in_module) ? "default" : in_module;
      var language = mStateData["language"];
      var region = mStateData["region"];
      var localeKey = language;
      if (region !== undefined) {
         localeKey += "-" + region;
      }
      if (module in mLocaleModule) {
         if (localeKey === mLocaleModule[module]) {
            if (module in mLocaleRequestCallback) {
               var currentQueue = mLocaleRequestCallback[module];
               var newQueue = function () {
                  in_callback(ResolveLocalisedText(in_key, in_data));
                  currentQueue();
               }
               mLocaleRequestCallback[module] = newQueue();
               return;
            } else {
               in_callback(ResolveLocalisedText(in_key, in_data));
               return;
            }
         }
      }

      mLocaleModule[module] = localeKey;
      //clear existing callbacks, as we could have a new language selected
      delete mLocaleRequestCallback[module];

      var mAttempt = 0;
      var GetLocaleModule = function ()
      {
         var endpoint = "db/locale/" + module + "/" + language;
         if (region) {
            endpoint += "/" + region;
         }
         var CallbackPass = function (body) {
            Object.assign(mLocaleCache, body);
            if (module in mLocaleRequestCallback) {
               var callback = mLocaleRequestCallback[module];
               callback();
            }
            delete mLocaleRequestCallback[module];
         }

         var CallbackError = function () {
            mAttempt++;
            App.View.CallbackError("Get locale module failed attempt:" + String(mAttempt) + " module:" + module + " language:" + language + " [region:]" + region );
            if (mAttempt < 5) {
               GetLocaleModule();
            } else {
               App.View.CallbackError("Get locale module failed giving up module:" + module + " language:" + language + " [region:]" + region);
            }
         }

         App.Network.Client_GET(
            CallbackPass,
            CallbackError,
            mAuthoriseToken,
            endpoint
            );
         return;
      }
      GetLocaleModule();
   }

   function Main() {
      SetState("boot");
      //App.View.Client_SetScreen("loading0", "templateLoading", null);
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