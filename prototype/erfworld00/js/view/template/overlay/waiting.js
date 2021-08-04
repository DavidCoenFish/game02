(function () {
   function Factory(in_document, in_rootElement, in_dataSource) {
      var m_container = in_document.createElement("div");
      //m_container.style.background = "red";
      m_container.style.position = "absolute";
      m_container.style.left = "0px";
      m_container.style.right = "0px";
      m_container.style.top = "0px";
      m_container.style.bottom = "0px";
      m_container.style.display = "flex";
      m_container.style.flexDirection = "column";
      m_container.style.justifyContent = "center";
      m_container.style.alignItems = "center";
      in_rootElement.appendChild(m_container);

      var m_generatorText = undefined;
      {
         var child = in_document.createElement("div");
         m_container.appendChild(child);
         m_generatorText = in_dataSource.MakeGeneratorLocaleKey("{0}", undefined, ["ProgressText"], function (in_string) {
            child.innerHTML = in_string;
         });
      }
      var m_generatorState = undefined;
      {
         m_generatorState = in_dataSource.MakeGeneratorBool("ShowProgress", function (in_bool) {
            switch (in_bool) {
               default:
                  m_container.style.display = "none";
                  break;
               case true:
                  m_container.style.display = "flex";
                  break;
            }
         });
      }

      return {
         "Dtor": function () {
            if (m_generatorText) {
               m_generatorText.Dtor();
            }
            if (m_generatorState) {
               m_generatorState.Dtor();
            }
            in_rootElement.removeChild(m_container);
            in_dataSource.Dtor();
         }
      };
   };
   App.View.Root_AddTemplate("overlay_waiting", Factory);
})();
