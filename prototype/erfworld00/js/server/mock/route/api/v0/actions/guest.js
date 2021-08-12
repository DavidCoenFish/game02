/*

 ?users.map.[username] //<username, uuid>
 users.etag //
 users.data.[uuid] //{password, pref, games[uid]}

*/
(function () {
   var PostGuest = function (in_request, in_response, in_next) {

      //Sanity check. add some magic key to the in_request just to handshake guest resource creation, like etag of api or something?

      var uuid = App.Server.Root_MakeUUID();
      var sessionUuid = App.Server.Root_MakeUUID();
      var data = {
         "ETag": App.Server.Root_MakeETag(),
         "UUID": uuid,
         "Games": [],
         "IsGuest": true,
         //"UserName": "",
         //"Password": "",
      };
      var responseData = {
         "UUID": uuid,
         "Session": sessionUuid
      };
      var keyUser = "users.data." + uuid;
      var keyUserSession = "sessions.data." + sessionUuid;

      App.Database.Server_SET("users.etag", App.Server.Root_MakeETag());
      App.Database.Server_SET(keyUser, data, "EX", App.Globals.databaseGuestTimeoutSeconds);
      App.Database.Server_SET("sessions.etag", App.Server.Root_MakeETag());
      App.Database.Server_SET(keyUserSession, uuid, "EX", App.Globals.databaseSessionTimeoutSeconds);

      //what is the best was to get the Auth data back to client. short answer, don't roll your own auth, but wanted to mock something
      in_response.setHeader("Authorization", `${uuid} ${sessionUuid}`);

      App.Server.Root_MakeResponseOk(in_request, in_response, responseData);

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   route.Use("POST", "/api/v0/actions/guest", PostGuest);

})();