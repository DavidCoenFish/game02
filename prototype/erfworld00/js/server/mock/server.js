(function () {
   /*
https://hackernoon.com/restful-api-design-step-by-step-guide-2f2c9f9fcdbf
/products?name=’ABC’
/products?type=’xyz’
/products?limit=25&offset=50  (default of limit 25?)
https://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
200 201 304 400 401 403 404 412 500

200 OK — This is most commonly used HTTP code to show that the operation performed is successful.
201 CREATED — This can be used when you use POST method to create a new resource.
//202 ACCEPTED — This can be used to acknowledge the request sent to the server.
304 NOT MODIFIED
400 BAD REQUEST — The request could not be understood by the server due to malformed syntax
401 UNAUTHORIZED — missing or bad authentication
403 FORBIDDEN - user authorized, but doens't have permision for the requested action
404 NOT FOUND —This can be used if you are looking for certain resource and it is not available in the system.
//409 CONFLICT
//410 GONE
412 PRECONDITION FAILED
500 INTERNAL SERVER ERROR — This should never be thrown explicitly but might occur if the system fails.
//502 BAD GATEWAY — This can be used if server received an invalid response from the upstream server.
{
  "error": {
    "message": "(#803) Some of the aliases you requested do not exist: products",
    "type": "OAuthException",
    "code": 803,
    "fbtrace_id": "FOXX2AhLh80"
  }
}
https://github.com/NationalBankBelgium/REST-API-Design-Guide/wiki
kebab style for url, camal for query, plurals, nouns not verbs... so /v1/host_sessions?excludeMetadata
security allowing, pass info in url or body, not the header

<!-- METHODS -->
GET //etag (changeid) &  last-modified time stamp
HEAD //can be issued against any resource to get the corresponding HTTP headers
POST //create and/or partial update
DELETE
OPTIONS //return a Allow header with allowed HTTP methods
PUT //full update

Google GData API: 10 different codes
200 201 304 400 401 403 404 409 410 500
Netflix: 9 different codes
200 201 304 400 401 403 404 412 500

//does each endpoint have the links to children endpoints? use OPTIONS to query what methods work on the endpoints?
// embed minimim data into resource for relations to other resources, client can resolve with self link if it needs more info from children
{
  "type": "foobar",
  "id": "9",
  "_links": {
      "self": { "href": "/orders" },
      "next": { "href": "/orders?page=2" },
      "ea:find": {
          "href": "/orders{?id}",
          "templated": true
      },
      "ea:admin": [{
          "href": "/admins/2",
          "title": "Fred"
      }, {
          "href": "/admins/5",
          "title": "Kate"
      }]
  },
  "cashier": {
    "type": "user",
    "id": "1",
    "links": {
      "self": "//example.com/users/1"
    }
  },
  "customer": {
    "type": "customer",
    "id": "12",
    "links": {
      "self": "//example.com/customers/12"
    }
  },
  "name" : "test",
  "notes" : "Lorem ipsum example long text",
  "store": {
    "type": "store",
    "id": "3",
    "links": {
      "self": "//example.com/stores/3"
    }
  }
}

<!-- resources [document, collection(server-managed directory of resources), store(client-managed resource repository)] -->
/api/v1 GET
/api/v1/users POST, (GET?)
/api/v1/users/<uuid> POST, GET, (PUT?), DELETE
/api/v1/games/
/api/v1/games/<uuid>
/api/v1/units
/api/v1/units/<uuid>
?/api/v1/lore/terrain/<uuid>//static extra data about terrain, flavour text, base stats
?/api/v1/lore/bestiary/<uuid> //static extra data about units, flavour text, base stats
?/api/v1/lore/ablities/<uuid>
?/api/v1/lore/dialog-trees/<uuid>
?/api/v1/locale/<uuid>?language=en,region=au //uuid is module of string table. or are locale strings auto resolved?
/api/v0/lore/<uuid> //terrain, units, abilities~ or do they all want to be different collections?

/api/v1/map/tiles?0,120,123,20,20   //map tiles (terrain)
/api/v1/map/instances?0,120,123,20,20 //instances of game units or gaea units on the map

?terrain and instances of units? or is /world split into /terrain and /instances
/api/v1/world?filter=static,dynamic&location=0,120,123,20,20  //mip,x,y,width,height
/api/v1/world/static?120,123,8,20,20
/api/v1/world/dynamic?120,123,8,20,20

? provide metadata about what to pass to create resources?
?/api/v1/template/guest-create  GET
?/api/v1/template/guest-update  GET
/api/v1/template/users-create  GET
/api/v1/template/users-update  GET
/api/v1/template/games-create GET
/api/v1/template/games-update GET
...

<!-- actions are not a resource, try to limit the number of actions. verbs are ok -->
/api/v1/actions/guest //make a temp user with session. if we just POST a user, need to also make a session ref that user
/api/v1/actions/login //make a session for given user
/api/v1/actions/logout //del seesion for user
/api/v1/actions/new-user //or just POST a user?
/api/v1/actions/promote-guest //or just PUT a user? difficult to have different allowed PUT for guest or not? removes timmer on user and created games
/api/v1/actions/game-endturn //given games/<uuid> process ./commands and update game state, 

/api/v1/actions/game-reroll //only for pregame game
/api/v1/actions/game-start //finish the pre game part of a game, the setting up start characters

was thinking that complicated actions (have a users' game array updated when game created) meant we needed an action, but that doesn't follow
complicated action could follow from simple rest enpoints. so you could still POST to /games and add a link for the user


<!-- how to confirm delete, we usually just call DELETE on resouce, but what abbout importaint stuff? or do confirm in ui?
/api/v1/actions/delete-user ->response 202 acknoledged with a new location of confirm + ETag
/api/v1/actions/delete-user-confirm
-->
     */

   App.Server.Root_GetServerVersion = function () {
      return "0.0.0.0"
   }

   function FilterMiddleware(in_route, in_method, in_request) {
      if (("" !== in_method) && (in_method !== in_request.method)) {
         return false;
      }
      if (("" !== in_route) && (in_route !== in_request.method)) {
         return false;
      }
      return true;
   }

   App.Server.Root_Use = function (in_path, in_method, in_callback) {
      m_middlewareStack.Use(in_path, in_method, in_callback);
   }
   App.Server.Root_MakeMiddlewareStack = function () {
      var m_stack = [];
      function Use(in_path, in_method, in_callback) {
         m_stack.push({
            "route": in_path ? in_path : "",
            "method": in_method ? in_method : "",
            "handle": in_callback,
         });
      }
      function RequestListener(in_request, in_response, in_next) {
         var callbackIndex = 0;
         var func = undefined;
         var data = undefined;
         var next = function () {
            if (callbackIndex <= m_stack.length) {
               if (callbackIndex < m_stack.length) {
                  data = m_stack[callbackIndex];
                  callbackIndex += 1;
                  //do we skip/ filter this middleware (on route or method)
                  if (false === FilterMiddleware(data.route, data.method, in_request)) {
                     next();
                     return;
                  }
                  func = data.handle;
               } else {
                  func = in_next;
                  callbackIndex += 1;
               }
               if (typeof func === 'function') {
                  func(in_request, in_response, next);
               }
            }
         }
         next();
      }
      return {
         "Use": Use, //in_path, in_method, in_callback(in_request, in_response, in_next)
         "RequestListener": RequestListener //in_request, in_response, in_next
      };
   }
   var m_middlewareStack = App.Server.Root_MakeMiddlewareStack();

   //allow easiy way to share state between mutiple src files without "include",
   var m_state = {};
   App.Server.Root_SetKeyValue = function (in_key, in_value) {
      m_state[in_key] = in_value;
   }
   App.Server.Root_GetKeyValue = function (in_key) {
      return m_state[in_key];
   }

   //https://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid
   App.Server.Root_MakeUUID = function () {
      var buf = new Uint32Array(4);
      window.crypto.getRandomValues(buf);
      var idx = -1;
      return 'xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx'.replace(/[xy]/g, function (c) {
         idx++;
         var r = (buf[idx >> 3] >> ((idx % 8) * 4)) & 15;
         var v = c === 'x' ? r : (r & 0x3 | 0x8);
         return v.toString(16);
      });
   }
   App.Server.Root_MakeETag = function () {
      //"17b1472d69d"
      var etag = Date.now().toString(16);
      return etag;
   }
   var m_serverETag = undefined;
   App.Server.Root_GetApiETag = function () {
      if (undefined === m_serverETag) {
         m_serverETag = App.Server.Root_MakeETag();
      }
      return m_serverETag;
   }

   function SetResponseOk(in_request, in_response, in_status, in_json) {
      in_response.status(in_status);
      in_response.json(in_json);
   }

   function SetResponseError(in_request, in_response, in_status, in_errorLogMessage) {
      in_response.status(in_status);
      in_response.end();
      //for security, don't return anything for body of error?
      App.Server.LogError(`${in_request.method} ${in_request.url} resulted in ${in_status} ${in_errorLogMessage}`);
   }

   App.Server.Root_MakeResponseOk = function (in_request, in_response, in_json) {
      SetResponseOk(in_request, in_response, 200, in_json);
   }
   App.Server.Root_MakeResponseCreated = function (in_request, in_response, in_json) {
      SetResponseOk(in_request, in_response, 201, in_json);
   }
   App.Server.Root_MakeErrorResponseNotModified = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 304, in_errorLogMessage);
   }
   App.Server.Root_MakeErrorResponseBadRequest = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 400, in_errorLogMessage);
   }
   App.Server.Root_MakeErrorResponseUnauthorized = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 401, in_errorLogMessage);
   }
   App.Server.Root_MakeErrorResponseForbidden = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 403, in_errorLogMessage);
   }
   App.Server.Root_MakeErrorResponseNotFound = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 404, in_errorLogMessage);
   }
   App.Server.Root_MakeErrorResponsePreconditionFailed = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 412, in_errorLogMessage);
   }
   App.Server.Root_MakeErrorResponseInternalServerError = function (in_request, in_response, in_errorLogMessage) {
      SetResponseError(in_request, in_response, 500, in_errorLogMessage);
   }

   var m_linkData = {};
   /*
{
  "_links": {
      "self": { "href": "/orders", "method": "GET" },
    */
   App.Server.Root_AddLinkData = function (in_path, in_method, in_name) {
      var pathTokens = in_path.split("/");
      if (0 < pathTokens.length) {
         var lastToken = pathTokens.pop();
         if (undefined === in_name) {
            in_name = lastToken;
         }
      }
      var rootPath = pathTokens.join("/");
      if (false === rootPath in m_linkData) {
         m_linkData[rootPath] = {}; 
      }
      m_linkData[rootPath][in_name] = {
         "href": in_path,
         "method": in_method,
      };

      if ("GET" === in_method && false === in_path in m_linkData) {
         m_linkData[in_path] = {
            "self": {
               "href": in_path,
               "method": in_method,
            }
         };
      }
      return;
   }

   App.Server.Root_AppendLinkDataToResponse = function (in_responseJson, in_path) {
      if (false === "_links" in in_responseJson) {
         in_responseJson["_links"] = {}
      }
      if (in_path in m_linkData) {
         in_responseJson._links = Object.assign(in_responseJson._links, m_linkData[in_path]);
      }
   }

   // request { "url", "headers", "method", "body" }
   // response { "status", "json", "end"}
   App.Server.Network_RequestListener = function (in_request, in_response) {
      m_middlewareStack.RequestListener(in_request, in_response);
   }

})();