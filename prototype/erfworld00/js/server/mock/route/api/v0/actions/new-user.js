/*

 users.map.[username] //<username, uuid>
 users.etag //
 users.data.[user uuid] //{password, pref, games[uid]}
 users.sessions.[session uuid]

*/
(function () {
   var PostNewUser = function (in_request, in_response, in_next) {
      //does the body have what we expect
      if ((false === "username" in in_request.body) ||
         (false === "password" in in_request.body) ||
         ("" === in_request.body.username) ||
         ("" === in_request.body.password) ||
         (64 < in_request.username.length) ||
         (in_request.username.password.length < 8)) {

         App.Server.Root_MakeErrorResponseBadRequest(in_request, in_response, "new user. username or password missing or invalid");
         return;
      }
      var keyUserMap = "users.map." + in_request.body.username; //username is json packable, how bad could it be? or we can base64 it or something
      if (0 !== App.Database.Server_EXISTS(keyUserMap)) {
         App.Server.Root_MakeErrorResponseForbidden(in_request, in_response, "new user. username not avaliable");
         return;
      }

      var uuid = Api.Server.Root_MakeUUID();
      var sessionUuid = Api.Server.Root_MakeUUID();
      var data = {
         "ETag": App.Server.Root_MakeETag(),
         "UUID": uuid,
         "Games": [],
         "IsGuest": false,
         "UserName": in_request.body.username,
         "Password": in_request.body.password,
      };
      var responseData = {
         "UUID": uuid,
         "Session": sessionUuid 
      };
      var keyUser = "users.data." + uuid;
      var keyUserSession = "users.sessions." + sessionUuid;

      App.Database.Server_SET("users.etag", App.Server.Root_MakeETag());
      App.Database.Server_SET(keyUser, data);
      App.Database.Server_SET(keyUserMap, uuid);
      App.Database.Server_SET(keyUserSession, uuid, "EX", App.Globals.databaseSessionTimeoutSeconds);

      //what is the best was to get the Auth data back to client. short answer, don't roll your own auth, but wanted to mock something
      in_response.setHeader("Authorization", `${uuid} ${sessionUuid}`);

      App.Server.Root_MakeResponseOk(in_request, in_response, responseData);

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   route.Use("POST", "/api/v0/actions/new-user", PostNewUser);

})();