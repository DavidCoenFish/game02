/*
*/
(function () {
   var PostLogout = function (in_request, in_response, in_next) {
      //see middleware authorize_user_session.js to ensure we have a Authorization. sets in_request.userUuid, in_request.sessionUuid

      var keyUserSession = "users.sessions." + in_request.sessionUuid;

      App.Database.Server_DEL(keyUserSession);

      in_response.removeHeader("Authorization");

      App.Server.Root_MakeResponseOk(in_request, in_response, {});

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   var AuthorizeUserSession = App.Server.Root_GetKeyValue("AuthorizeUserSession");
   route.Use("POST", "/api/v0/actions/logout", [AuthorizeUserSession, PostLogout]);

})();