(function () {
   var m_stateFactory = {};
   App.Client.Root_AddStateFactory = function (in_stateName, in_factory) {
      m_stateFactory[in_stateName] = in_factory;
   }
   App.Client.Client_AddStateFactory = function (in_stateName, in_factory) {
      m_stateFactory[in_stateName] = in_factory;
   }
   var m_state = undefined;
   App.Client.Client_SetState = function (in_name) {
      var stateFactory = undefined;
      if (in_name in m_stateFactory) {
         stateFactory = m_stateFactory[in_name];
      }
      if (undefined === stateFactory) {
         App.Client.LogError(`SetState state not found:${in_name}`);
         return;
      }
      if (undefined !== m_state) {
         m_state.Dtor();
         m_state = undefined;
      }
      m_state = stateFactory();
   }

   var m_stateData = {
      "ClientVersion": "0.0.0.0",
      "Language": "en",
      "Region":"au",
   };
   App.Client.Root_SetData = function (in_data) {
      m_stateData = Object.Assign(m_stateData, in_data);
   }
   var m_stateDataCallback = {};
   App.Client.Client_SetDataValue = function (in_key, in_value) {
      if (in_key in m_stateData) {
         if (in_value === m_stateData[in_key]) {
            return;
         }
      }
      m_stateData[in_key] = in_value;
      if (in_key in m_stateDataCallback) {
         var callbackArray = m_stateDataCallback[in_key];
         for (let i = 0; i < callbackArray.length; i++) {
            var callback = callbackArray[i];
            callback(in_value);
         }
      }
   }
   App.Client.Client_GetDataValue = function (in_key) {
      return m_stateData[in_key];
   }
   App.Client.Client_AddDataChangeCallback = function (in_key, in_callback) {
      if (in_key in m_stateDataCallback) {
         m_stateDataCallback[in_key].push(in_callback);
      } else {
         m_stateDataCallback[in_key] = [in_callback];
      }
   }
   App.Client.Client_RemoveDataChangeCallback = function (in_key, in_callback) {
      if (in_key in m_stateDataCallback) {
         var callbackArray = m_stateDataCallback[in_key];
         const index = callbackArray.indexOf(5);
         if (-1 < index) {
            callbackArray.splice(index, 1);
         }
      }
   }

   var s_defaultModule = "default";
   var m_localeCache = {
      "Version {0} {1} {2}": "Version {0} {1} {2}"
   };
   //trying to accomodate change of language
   //what modules have been merged into the localeCache, <module name, language-region>
   var m_localeModule = {};
   m_localeModule[s_defaultModule] = "en-au";
   // <modual name, callback chain>
   var m_localeRequestCallback = {};

   function ResolveLocalisedText(in_callback, in_key, in_arrayDataKeys) {
      var value = in_key;
      if (in_key in m_localeCache) {
         value = m_localeCache[in_key];
      }
      if (value && in_arrayDataKeys) {
         var arrayValue = [];
         in_arrayDataKeys.forEach(function (element, index, array) {
            arrayValue[index] = App.Client.Client_GetDataValue(element);
         });
         value = value.replace(/{(\d+)}/g, function (match, number) {
            return typeof arrayValue[number] !== "undefined" ? arrayValue[number] : "";
         });
      }
      in_callback(value);
   }

   // of data keys to be converted to text to fill in format blanks of text({0}, {1}, {2}, ...)
   App.Client.Client_AsyncGetLocalisedText = function (in_callback, in_module, in_key, in_arrayDataKeys) {
      var languageRegion = App.Client.Client_GetDataValue("LanguageRegion");
      var module = in_module ? in_module : s_defaultModule;
      if ((module in m_localeModule) && (languageRegion === m_localeModule[module])) {
         ResolveLocalisedText(in_callback, in_key, in_arrayDataKeys);
         return;
      }
      if (module in m_localeRequestCallback) {
         var value = m_localeRequestCallback[module];
         m_localeRequestCallback[module] = function () {
            ResolveLocalisedText(in_callback, in_key, in_arrayDataKeys);
            value();
            return;
         };
      }

      m_localeModule[module] = languageRegion;
      //clear existing callbacks, as we could have a new language selected
      delete m_localeRequestCallback[module];

      var attempt = 0;
      var language = App.Client.Client_GetDataValue("Language");
      var region = App.Client.Client_GetDataValue("Region");
      var GetLocaleModule = function () {
         var endpoint = "db/locale/" + module + "/" + language;
         if (region) {
            endpoint += "/" + region;
         }
         var CallbackPass = function (body) {
            Object.assign(m_localeCache, body);
            if (module in m_localeRequestCallback) {
               var callback = m_localeRequestCallback[module];
               callback();
            }
            delete m_localeRequestCallback[module];
         }

         var CallbackError = function () {
            attempt++;
            App.Client.LogWarning(`Get locale module failed attempt:${attempt} module:${module} language:${language} [region:${region}]`);
            if (attempt < 5) {
               GetLocaleModule();
            } else {
               App.Client.LogError(`Get locale module failed attempt:${attempt} module:${module} language:${language} [region:${region}]`);
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

   var m_authoriseToken = undefined;
   App.Client.Client_SetAuthoriseToken = function (in_authoriseToken) {
      m_authoriseToken = in_authoriseToken;
   }

   function MakeGeneratorLocaleKey(in_localeKey, in_module, in_dataKeyArray, in_callback) {
      var currentTextValue = "";
      function GenerateLocaleCallback() {
         function Callback(in_value) {
            if (in_value === currentTextValue) {
               return;
            }
            currentTextValue = in_value;
            in_callback(in_value);
         }
         App.Client.Client_AsyncGetLocalisedText(Callback, in_module, in_localeKey, in_dataKeyArray);
      }
      App.Client.Client_AddDataChangeCallback("LanguageRegion", GenerateLocaleCallback);
      if (undefined !== in_dataKeyArray) {
         for (let i = 0; i < in_dataKeyArray.length; i++) {
            var dataKey = in_dataKeyArray[i];
            App.Client.Client_AddDataChangeCallback(dataKey, GenerateLocaleCallback);
         }
      }
      GenerateLocaleCallback();
      return {
         "Dtor": function () {
            App.Client.Client_RemoveDataChangeCallback("LanguageRegion", GenerateLocaleCallback);
            if (undefined !== in_dataKeyArray) {
               for (let i = 0; i < in_dataKeyArray.length; i++) {
                  var dataKey = in_dataKeyArray[i];
                  App.Client.Client_RemoveDataChangeCallback(dataKey, GenerateLocaleCallback);
               }
            }
         },
      };
   }
   App.Client.Client_MakeDefaultDataSource = function () {
      return {
         "Dtor": function () {
         },
         "MakeGeneratorLocaleKey": MakeGeneratorLocaleKey
      };
   }

   function CallbackLocaleKey() {
      var oldValue = String(App.Client.Client_GetDataValue("LanguageRegion"));
      var language = String(App.Client.Client_GetDataValue("Language"));
      var region = App.Client.Client_GetDataValue("Region");
      var newValue = language;
      if (typeof (region) !== "undefined" && region) {
         newValue += "-" + region;
      }
      if (oldValue !== newValue) {
         App.Client.Client_SetDataValue("LanguageRegion", newValue);
      }
   }

   function Main() {
      App.Client.Client_AddDataChangeCallback("Language", CallbackLocaleKey);
      App.Client.Client_AddDataChangeCallback("Region", CallbackLocaleKey);
      CallbackLocaleKey();

      App.View.Client_AddRenderLayer("base");
      App.View.Client_AddRenderLayer("base_overlay");
      App.View.Client_AddRenderLayer("base_transition");
      App.View.Client_AddRenderLayer("dialog_under");
      App.View.Client_AddRenderLayer("dialog");
      App.View.Client_AddRenderLayer("dialog_transition");
      App.View.Client_AddRenderLayer("top_overlay");

      App.View.Client_AddScreen("overlay_version", "overlay_version", App.Client.Client_MakeDefaultDataSource(), false, undefined, "top_overlay");

      App.Client.Client_SetState("boot");
   }
   App.RegisterOnLoad(Main);

})();
