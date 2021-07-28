(function () {
   var Get = function (in_request, in_response, in_next) {
      var databaseVersion = App.Database.Server_GET("DatabaseVersion");

      in_response.data = Object.assign(in_response.data ? in_response.data : {}, {
            "data": {
               "ServerVersion": "0.0.0.0",
               "DatabaseVersion": databaseVersion
            }
         }
      );
      in_next();
   }
   var BuildLinks = App.Server.Root_GetNamedCallback("BuildLinks");
   var ResponseJson = App.Server.Root_GetNamedCallback("ResponseJson")
   App.Server.Root_AddRoute("/api", {
         "GET": [Get, BuildLinks, ResponseJson]
      }
   );

})();