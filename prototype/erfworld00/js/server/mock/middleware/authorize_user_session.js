(function () {
   function Callback(in_request, in_response, in_next) {
      if (false === "Authorization" in in_request.headers) {
         App.Server.Root_MakeErrorResponseUnauthorized(in_request, in_response, "authorize user session. missing Authorization");
         //follow convention of not calling next once we call response end/json?
         //in_next();
         return;
      }

      let parts = in_request.headers.Authorization.trim().split(' ');
      in_request.sessionUuid = parts.pop()
      in_request.userUuid = parts.pop()

      var keyUserSession = "sessions.data." + in_request.sessionUuid;
      if (in_request.userUuid !== App.Database.Server_GET(keyUserSession)) {
         App.Server.Root_MakeErrorResponseUnauthorized(in_request, in_response, "authorize user session. session not found");
         //follow convention of not calling next once we call response end/json?
         //in_next();
         return;
      }

      in_next();
   };

   App.Server.Root_SetKeyValue("AuthorizeUserSession", Callback);
})();