/*

 users.map.[username] //<username, uuid>
 users.etag //
 users.data.[uuid] //{password, pref, games[uid]}

*/
(function () {
   var PostLogin = function (in_request, in_response, in_next) {
      //does the body have what we expect
      if ((false === "username" in in_request.body) ||
         (false === "password" in in_request.body) ||
         ("" === in_request.body.username) ||
         ("" === in_request.body.password) ||
         (64 < in_request.username.length) ||
         (in_request.username.password.length < 8)) {

         App.Server.Root_MakeErrorResponseBadRequest(in_request, in_response, "login. username or password missing or invalid");
         //follow convention of not calling next once we call response end/json?
         //in_next();
         return;
      }
      var keyUserMap = "users.map." + in_request.body.username; //it is json packable, how bad could it be? or we can base64 it or something
      if (0 === App.Database.Server_EXISTS(keyUserMap)) {
         App.Server.Root_MakeErrorResponseForbidden(in_request, in_response, "login. username not found");
         //follow convention of not calling next once we call response end/json?
         //in_next();
         return;
      }
      var uuid = App.Database.Server_GET(keyUserMap);
      var keyUser = "users.data." + uuid;
      var userData = App.Database.Server_GET(keyUser);
      if ((null === userData) ||
         (in_request.body.password !== userData.Password)) {
         App.Server.Root_MakeErrorResponseForbidden(in_request, in_response, "login. password missmatch");
         //follow convention of not calling next once we call response end/json?
         //in_next();
         return;
      }

      var sessionUuid = Api.Server.Root_MakeUUID();
      var keyUserSession = "sessions.data." + sessionUuid;
      var responseData = {
         "UUID": uuid,
         "Session": sessionUuid
      };

      App.Database.Server_SET("sessions.etag", App.Server.Root_MakeETag());
      App.Database.Server_SET(keyUserSession, uuid, "EX", App.Globals.databaseSessionTimeoutSeconds);

      //what is the best was to get the Auth data back to client. short answer, don't roll your own auth, but wanted to mock something
      in_response.setHeader("Authorization", `${uuid} ${sessionUuid}`);

      App.Server.Root_MakeResponseOk(in_request, in_response, responseData);

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   route.Use("POST", "/api/v0/actions/login", PostLogin);

})();