(function () {
   var BuildLinks = App.Server.Root_GetNamedCallback("BuildLinks");
   var ResponseJson = App.Server.Root_GetNamedCallback("ResponseJson")

   var Post = function (in_request, in_response, in_next) {
      in_next();
   }
   App.Server.Root_AddRoute("/api/logout",
      {
         "POST": [Post, BuildLinks, ResponseJson]
      }
   );

})();