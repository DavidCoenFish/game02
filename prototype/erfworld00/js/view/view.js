(function () {
   var m_templateMap = {};
   App.View.Root_AddTemplate = function (in_templateName, in_factory) {
      m_templateMap[in_templateName] = in_factory;
   }
   App.View.Client_AddTemplate = function (in_templateName, in_factory) {
      m_templateMap[in_templateName] = in_factory;
   }
   var m_renderLayer = {};
   function GetRenderLayer(in_name) {
      if (in_name in m_renderLayer) {
         return m_renderLayer[in_name];
      }
      var div = document.createElement("div");
      div.innerHTML = in_name;
      //div.style.background = "red";
      div.style.position = "absolute";
      div.style.top = "0px";
      div.style.right = "0px";
      div.style.bottom = "0px";
      div.style.left = "0px";
      div.style.pointerEvents = "none";
      document.body.appendChild(div);
      m_renderLayer[in_name] = div;
      return div;
   }
   var m_screenMap = {};
   App.View.Client_AddScreen = function (in_templateName, in_name, in_IDataSource, in_takeFocusWhenReady, in_transitionHint, in_renderLayerHint) {
      var renderLayer = GetRenderLayer(in_renderLayerHint);
      var templateFactory = undefined;
      if (in_templateName in m_templateMap) {
         templateFactory = m_templateMap[in_templateName];
      }
      if (undefined === templateFactory) {
         App.View.LogError(`AddScreen template name not found:${in_templateName}`);
         return;
      }
      //in_document, in_rootElement, in_dataSource
      var screen = templateFactory(document, renderLayer, in_IDataSource, in_takeFocusWhenReady);
      m_screenMap[in_name] = screen;
   }
   App.View.Client_RemoveScreen = function (in_name) {
      var screen = undefined;
      if (in_name in m_screenMap) {
         screen = m_screenMap[in_name];
         delete m_screenMap[in_name];
      }
      if (undefined === screen) {
         App.View.View_LogError("RemoveScreen name not found:" + in_name);
         return;
      }
      screen.Dtor();
   }
   App.View.Client_AddRenderLayer = GetRenderLayer;

   //function Main() {
   //}
   //App.RegisterOnLoad(Main);

})();
