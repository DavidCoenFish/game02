(function () {
   function Factory(in_document, in_rootElement, in_dataSource) {
      var m_container = in_document.createElement("div");
      //m_container.style.background = "red";
      m_container.style.position = "absolute";
      m_container.style.left = "0px";
      m_container.style.right = "0px";
      m_container.style.bottom = "0px";
      m_container.style.display = "flex";
      //m_container.style.flexDirection = "column-reverse";
      m_container.style.flexDirection = "row";
      //m_container.style.alignItems = "center";
      m_container.style.justifyContent = "center";
      in_rootElement.appendChild(m_container);

      var m_generator = undefined;
      function AddElement(in_localeKey, in_dataKeyArray) {
         var child = in_document.createElement("div");
         child.innerHTML = in_name;
         //child.style.background = "blue";
         child.style.fontSize = "0.5em"
         child.style.opacity = 0.5;
         child.style.margin = "0.5em";
         m_container.appendChild(child);

         //so how do i make 
         m_generator = in_dataSource.MakeGeneratorLocaleKey(in_localeKey, undefined, in_dataKeyArray, function (in_string) {
            child.innerHTML = in_string;
         });
      }
      AddElement("version __0__ __1__ __2__", ["DatabaseVersion", "ServerVersion", "ClientVersion"]);

      return {
         "Dtor": function () {
            if (m_generator) {
               m_generator.Dtor();
            }
            in_rootElement.removeChild(m_container);
         }
      };
   };
   App.View.Root_AddTemplate("overlay_version", Factory);
})();
