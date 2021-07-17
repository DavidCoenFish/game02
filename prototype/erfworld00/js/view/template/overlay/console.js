(function () {
   function Factory(in_document, in_rootElement) {
   };
   var templateName = "Default";
   App.View.SetDefaultTemplateName(templateName);
   App.View.AddTemplate(templateName, Factory);
})();
