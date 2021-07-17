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
      "ClientVersion":"0.0.0.0"
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

   App.Client.Client_AsyncGetLocalisedText = function (in_callback, in_module, in_key, in_arrayDataKeys) { // of data keys to be converted to text to fill in format blanks of text(__0__, __1__, __2__, ...)
   }
   var m_authoriseToken = undefined;
   App.Client.Client_SetAuthoriseToken = function (in_authoriseToken) {
      m_authoriseToken = in_authoriseToken;
   }

   function MakeGeneratorLocaleKey(in_localeKey, in_module, in_dataKeyArray, in_callback) {
      return {
         "Dtor": function () {
            App.Client.Client_RemoveDataChangeCallback();
         },
      };
   }
   App.Client.Client_MakeDefaultDataSource = function () {
      function Callback() {
      }
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
