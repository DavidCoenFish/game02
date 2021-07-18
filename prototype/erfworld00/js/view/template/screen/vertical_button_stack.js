(function () {
   function Factory(in_document, in_rootElement, in_dataSource) {
      var m_container = in_document.createElement("div");
      m_container.style.position = "absolute";
      m_container.style.pointerEvents = "auto";
      m_container.style.left = "0px";
      m_container.style.right = "0px";
      m_container.style.top = "0px";
      m_container.style.bottom = "0px";
      m_container.style.display = "flex";
      m_container.style.flexDirection = "column";
      m_container.style.alignItems = "center";
      m_container.style.justifyContent = "center";
      in_rootElement.appendChild(m_container);

      var m_generatorArray = [];
      function AddButton(in_text, in_verb, in_object) {
         var child = in_document.createElement("button");
         child.innerHTML = in_text;
         child.style.width = "25em";
         child.onclick = function () {
            in_dataSource.OnButtonClick(in_verb, in_object);
         }
         m_container.appendChild(child);
      }

      var arrayButtonData = in_dataSource.GetValueArray();
      if (arrayButtonData) {
         for (let i = 0; i < arrayButtonData.length; i++) {
            var buttonData = arrayButtonData[i];
            AddButton(buttonData["text"], buttonData["verb"], buttonData["object"])
         }
      }

      return {
         "Dtor": function () {
            in_rootElement.removeChild(m_container);
            in_dataSource.Dtor();
         }
      };
   };
   App.View.Root_AddTemplate("verticalButtonStack", Factory);
})();
