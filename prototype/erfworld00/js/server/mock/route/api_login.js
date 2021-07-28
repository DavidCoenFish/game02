(function () {
   var BuildLinks = App.Server.Root_GetNamedCallback("BuildLinks");
   var ResponseJson = App.Server.Root_GetNamedCallback("ResponseJson")

   var Post = function (in_request, in_response, in_next) {
      in_next();
   }
   App.Server.Root_AddRoute("/api/login",
      {
         "POST": [Post, BuildLinks, ResponseJson]
      }
   );

   var GetTemplate = function (in_request, in_response, in_next) {
      in_response.data = Object.assign(in_response.data ? in_response.data : {}, {
         "username": "username",
         "password": "password"
      });
      in_next();
   }
   App.Server.Root_AddRoute("/api/login/template",
      {
         "GET": [GetTemplate, BuildLinks, ResponseJson]
      }
   );


})();