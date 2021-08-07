(function () {
   var Get = function (in_request, in_response, in_next) {
      var databaseVersion = App.Database.Server_GET("DatabaseVersion");

      var data = {
         "data": {
            "ServerVersion": App.Server.Root_GetServerVersion(),
            "DatabaseVersion": databaseVersion
         }
      };
      App.Server.Root_AppendLinkDataToResponse(data, "/api");
      App.Server.Root_MakeResponseOk(in_request, in_response, data);
      //follow convention of not calling next once we call response end/json?
      //in_next();
   }
   //var Options = function (in_request, in_response, in_next) {
   //   in_response.status(200);
   //   in_response.setHeader("Allow", "OPTIONS, GET");
   //   in_response.end();
   //}
   var route = App.Server.Root_GetKeyValue("Route");
   route.Use("GET", "/api", Get);

})();