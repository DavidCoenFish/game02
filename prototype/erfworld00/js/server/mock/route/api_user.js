(function () {
   var Post = function (in_request, in_response, in_next) {
      in_next();
   }
   var BuildLinks = App.Server.Root_GetNamedCallback("BuildLinks");
   var ResponseJson = App.Server.Root_GetNamedCallback("ResponseJson")
   App.Server.Root_AddRoute("/api/user",
      {
         "POST": [Post, BuildLinks, ResponseJson]
      }
   );

})();