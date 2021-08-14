/*
*/
(function () {
   //var GetContainer = function (in_request, in_response, in_next) {
   //   var offset = "offset" in in_request.urlObject.searchMap ? in_request.urlObject.searchMap["offset"] : 0;
   //   var limit = "limit" in in_request.urlObject.searchMap ? in_request.urlObject.searchMap["limit"] : 25;
   //}

   var m_itemName = "users";

   function PostContainer(in_request, in_response, in_next) {
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
      var keyUser = "users.data." + uuid;
      var keyUserSession = "sessions.data." + sessionUuid;

      App.Database.Server_SET("users.etag", App.Server.Root_MakeETag());
      App.Database.Server_SET(keyUser, data);
      App.Database.Server_SET(keyUserMap, uuid);
      App.Database.Server_SET("sessions.etag", App.Server.Root_MakeETag());
      App.Database.Server_SET(keyUserSession, uuid, "EX", App.Globals.databaseSessionTimeoutSeconds);

      var responseData = {
         "UUID": uuid,
         "Games": [],
         "IsGuest": false,
         "UserName": in_request.body.username,
         //"Password": in_request.body.password,
      };

      //what is the best was to get the Auth data back to client. short answer, don't roll your own auth, but wanted to mock something
      in_response.setHeader("Authorization", `${uuid} ${sessionUuid}`);
      App.Server.Root_MakeResponseCreated(in_request, in_response, responseData);

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }
   function GetContainerItem(in_request, in_response, in_next) {
      var keyUser = "users.data." + in_request.params.userid;
      data = App.Database.Server_GET(keyUser);
      if (!data) {
         App.Server.Root_MakeErrorResponseForbidden(in_request, in_response, `GET container item ${m_itemName}. item not found: ${String(in_request.params.userid)}`);
         return;
      }
      var responseData = {
         "UUID": data.UUID,
         "Games": data.Games,
         "IsGuest": data.IsGuest,
         "UserName": data.UserName,
         //"Password": in_request.body.password,
      };
      App.Server.Root_MakeResponseOk(in_request, in_response, responseData);
      in_response.setHeader("ETag", data.ETag);

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   //function PutContainerItem(in_request, in_response, in_next) {
   //   var keyUser = "users.data." + in_request.params.userid;
   //   data = App.Database.Server_GET(keyUser);
   //   if (!data) {
   //      App.Server.Root_MakeErrorResponseForbidden(in_request, in_response, `PUT container item ${m_itemName}. item not found: ${String(in_request.params.userid)}`);
   //      return;
   //   }
   //}

   function DeleteContainerItem(in_request, in_response, in_next) {
      var keyUser = "users.data." + in_request.params.userid;
      if (0 === App.Database.Server_DEL(keyUser)) {
         App.Server.Root_MakeErrorResponseForbidden(in_request, in_response, `DELETE container item ${m_itemName}. item not found: ${String(in_request.params.userid)}`);
         return;
      }
      App.Database.Server_SET("users.etag", App.Server.Root_MakeETag());

      App.Server.Root_MakeResponseOk(in_request, in_response, {});
      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   //route.Use("GET", "/api/v0/users", GetContainer);
   route.Use("POST", "/api/v0/users", PostContainer);
   route.Use("GET", "/api/v0/users/:userid", GetContainerItem);
   //route.Use("PUT", "/api/v0/users/:userid", PutContainerItem);
   route.Use("DELETE", "/api/v0/users/:userid", DeleteContainerItem);

})();