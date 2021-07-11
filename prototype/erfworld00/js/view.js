(function () {
   var mTemplateMap = {};
   var mScreen = undefined;
   var mDialogStack = [];
   var mDefaultTemplateName;

   App.View.Client_SetScreen = function (templateName, name, data) {
      if (false === templateName in mTemplateMap) {
         App.View.Client_ConsoleError("Template for screen not found name:" + name + " templateName:" + templateName);
         return;
      }
      var factory = mTemplateMap[templateName];
      if (mScreen) {
         mScreen();
      }
      mScreen = factory(document, document.body, name, data);
      return;
   };
   App.View.Client_PushDialog = function (templateName, name, data) {
      if (false === templateName in mTemplateMap) {
         App.View.Client_ConsoleError("Template for dialog not found name:" + name + " templateName:" + templateName);
         return;
      }
      var factory = mTemplateMap[templateName];
      var dialog = factory(document, document.body, name, data);
      if (dialog) {
         mDialogStack.push(dialog)
      }
      return;
   };
   App.View.Client_PopDialog = function () {
      var dialog = mDialogStack.pop();
      if (dialog) {
         dialog();
      }
      return;
   };
   App.View.Client_ClearAllDialog = function () {
      while (0 < mDialogStack.length()) {
         var dialog = mDialogStack.pop();
         if (dialog) {
            dialog();
         }
      }
   };
   App.View.Client_GetDefaultTemplateName = function () {
      return mDefaultTemplateName;
   };
   App.View.Client_OnGetUserData = function () {
      //
      var button = document.createElement("button");


   }
   App.View.ConsoleError = function (message) {
      console.error(message);
   };
   App.View.ConsoleDebug = function (message) {
      console.log(message);
   };
   App.View.SetDefaultTemplateName = function (defaultTemplateName) {
      mDefaultTemplateName = defaultTemplateName;
      return;
   };
   App.View.AddTemplate = function (templateName, factory) {
      mTemplateMap[templateName] = factory;
   };
   function Main() {
      //make debug/error console
   }
   App.RegisterOnLoad(Main);

})();


//Erfworld00.View = (function () {
//   var mCurrentCreatedElements = [];
//   var mStateData = {};

//   var CleanUpCreatedElements = function () {
//      mCurrentCreatedElements.slice().reverse().forEach(function (item) {
//         if (item) {
//            item.remove();
//         }
//      });
//      mCurrentCreatedElements = [];
//   }
//   var AddCreatedElements = function (in_element) {
//      mCurrentCreatedElements.push(in_element);
//   }
//   var CleanUpStateData = function () {
//      mStateData = {};
//   }

//   var AddElement = function (in_parent, in_body, in_type) {
//      var div = document.createElement(in_type);
//      AddCreatedElements(div);
//      if (in_body) {
//         div.innerHTML = Erfworld00.Locale.Resove(in_body);
//      }
//      in_parent.appendChild(div);
//      return div;
//   }

//   var AddButton = function (in_model, in_parent, in_body, in_enabledName, in_rootElement, in_screenName, in_buttonName, in_buttonVerb, in_buttonSubject0) {
//      var button = AddElement(in_parent, in_body, "button");
//      if (in_enabledName) {
//         button.disabled = (false === in_model.GetBool(in_enabledName));
//      }
//      var onSetNewScreenCallback = function () {
//         Run(in_rootElement, in_model); 
//      }
//      button.addEventListener("click", function () {
//         in_model.OnScreenButton(onSetNewScreenCallback, mStateData, in_screenName, in_buttonName, in_buttonVerb, in_buttonSubject0);
//      });
//   }


//   var DefaultTemplateFunction = function (in_rootElement, in_model, in_screenName, in_screenData) {
//      if ("body" in in_screenData) {
//         AddElement(in_rootElement, in_screenData.body, "div");
//      }
//      if ("buttons" in in_screenData) {
//         in_screenData.buttons.forEach(function (item) {
//            AddButton(in_model, buttonContainer, item.body, item.enabled, in_rootElement, in_screenName, item.name, item.verb, item.subject0);
//         });
//      }
//   }

//   var VerticalButtonStackScreenTemplateFunction = function (in_rootElement, in_model, in_screenName, in_screenData) {
//      var container = AddElement(in_rootElement, undefined, "div");
//      container.style.display = "flex";
//      container.style.flexDirection = "column";
//      container.style.height = "100%";

//      var buttonContainer = AddElement(container, undefined, "div");
//      buttonContainer.style.display = "flex";
//      buttonContainer.style.flex = 1;
//      buttonContainer.style.flexDirection = "column";
//      buttonContainer.style.justifyContent = "center";
//      buttonContainer.style.alignSelf = "center";

//      if ("buttons" in in_screenData) {
//         in_screenData.buttons.forEach(function (item) {
//            AddButton(in_model, buttonContainer, item.body, item.enabled, in_rootElement, in_screenName, item.name, item.verb, item.subject0);
//         });
//      }

//      if ("body" in in_screenData) {
//         var footer = AddElement(container, in_screenData.body, "footer");
//         footer.style.textAlign = "center";
//      }
//   }

//   var InfoDialogTemplateFunction = function (in_rootElement, in_model, in_screenName, in_screenData) {
//      var container = AddElement(in_rootElement, undefined, "div");
//      container.style.display = "flex";
//      container.style.flexDirection = "column";
//      container.style.justifyContent = "center";
//      container.style.height = "100%";

//      var dialogContainer = AddElement(container, in_screenData.body, "div");
//      //dialogContainer.style.flex = 1;
//      dialogContainer.style.alignSelf = "center";
//      dialogContainer.style.background = "var(--background-color-softer)";
//      dialogContainer.style.padding = "1em 1em 0 1em";

//      var buttonContainer = AddElement(dialogContainer, undefined, "div");
//      buttonContainer.style.display = "flex";
//      buttonContainer.style.flexDirection = "row";
//      buttonContainer.style.justifyContent = "center";
//      buttonContainer.style.margin = "1em";

//      if ("buttons" in in_screenData) {
//         in_screenData.buttons.forEach(function (item) {
//            AddButton(in_model, buttonContainer, item.body, item.enabled, in_rootElement, in_screenName, item.name, item.verb, item.subject0);
//         })
//      }
//   }

//   var sTemplateMap = {
//      "default": DefaultTemplateFunction
//      ,"verticalButtonStackScreen": VerticalButtonStackScreenTemplateFunction
//      ,"infoDialog": InfoDialogTemplateFunction
//   };

//   var Run = function (in_rootElement, in_model) {
//      var screenName = in_model.GetCurrentScreenName();
//      var screenData = in_model.GetScreenData(screenName);
//      var templateName;
//      if ("template" in screenData) {
//         templateName = screenData.template;
//      }
//      var templateFunction = undefined;
//      if (templateName in sTemplateMap) {
//         templateFunction = sTemplateMap[templateName];
//      } else {
//         templateFunction = sTemplateMap["default"];
//      }
//      CleanUpCreatedElements();
//      CleanUpStateData();
//      if (templateFunction) {
//         templateFunction(in_rootElement, in_model, screenName, screenData);
//      }
//   }

//   return {
//      "Run": Run
//   }
//})();