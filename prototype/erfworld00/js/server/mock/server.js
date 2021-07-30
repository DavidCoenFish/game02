(function () {
   /*
https://hackernoon.com/restful-api-design-step-by-step-guide-2f2c9f9fcdbf
/products?name=’ABC’
/products?type=’xyz’
/products?limit=25&offset=50  (default of limit 25?)
200 OK — This is most commonly used HTTP code to show that the operation performed is successful.
201 CREATED — This can be used when you use POST method to create a new resource.
202 ACCEPTED — This can be used to acknowledge the request sent to the server.
400 BAD REQUEST — This can be used when client side input validation fails.
401 UNAUTHORIZED / 403 FORBIDDEN— This can be used if the user or the system is not authorised to perform certain operation.
404 NOT FOUND— This can be used if you are looking for certain resource and it is not available in the system.
500 INTERNAL SERVER ERROR — This should never be thrown explicitly but might occur if the system fails.
502 BAD GATEWAY — This can be used if server received an invalid response from the upstream server.
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

/v1
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
/api/v1/users POST, GET
/api/v1/users/<uuid> POST, GET, (PUT?), DELETE
/api/v1/games/
/api/v1/games/<uuid>
/api/v1/games/<uuid>/commands
/api/v1/games/<uuid>/commands/<uuid>
/api/v1/games/<uuid>/units
/api/v1/games/<uuid>/units/<uuid>
/api/v1/games/<uuid>/status //resource flag when endturn process has completed?
/api/v1/gaea/<uuid> //units/things in the game world that don't belong to user
/api/v1/lore/terrain/<uuid>//static extra data about terrain, flavour text, base stats
/api/v1/lore/bestiary/<uuid> //static extra data about units, flavour text, base stats
/api/v1/lore/ablities/<uuid>
?/api/v1/lore/dialog-trees/<uuid>
?/api/v1/locale/<uuid>?language=en,region=au //uuid is module of string table. or are locale strings auto resolved?

/api/v1/map/tiles?0,120,123,20,20   //map tiles (terrain)
/api/v1/map/instances?0,120,123,20,20 //instances of game units or gaea units on the map


?terrain and instances of units? or is /world split into /terrain and /instances
/api/v1/world?filter=static,dynamic&location=0,120,123,20,20  //mip,x,y,width,height
/api/v1/world/static?120,123,8,20,20
/api/v1/world/dynamic?120,123,8,20,20

? provide metadata about what to pass to functions?
/api/v1/template/users-create  GET
/api/v1/template/users-update  GET
/api/v1/template/games-create GET
/api/v1/template/games-update GET
...

<!-- actions are not a resource, try to limit the number of actions. verbs are ok -->
/api/v1/actions/guest
/api/v1/actions/login
/api/v1/actions/logout
/api/v1/actions/promote-guest //or just POST a user?
/api/v1/actions/game-endturn //given games/<uuid> process ./commands and update game state, 

<!-- how to confirm delete, we usually just call DELETE on resouce, but what abbout importaint stuff? or do confirm in ui?
/api/v1/actions/delete-user ->resonspe 202 acknoledged with a new location of confirm + ETag
/api/v1/actions/delete-user-confirm
-->




/v0
what would a rest api look like, plus generating the links for each state in the response (Hypermedia as the Engine of Application State)
[POST, GET, PUT, DELETE] //create, read, update, delete...crud
get /api
   response {"DataVersion" : "0.0.0.0", "ServerVersion" : "0.0.0.0"}
   ////links
   //when not logged in, request has auth token with {sessionid, userid} not matching database temp key "user.[userid].session.[sessionid]"
   post login /api/login
   post login /api/guest
   //when logged in
   post logout /api/logout, request has auth token with {sessionid, userid} matching database temp key "user.[userid].session.[sessionid]"
   get api/map
   get api/data/terrain
   get api/data/bestiary

post /api/login
   response auth token? {sessionid, userid}
   ////links
   post /api/user/[userid]/newgame
   get /api/user/[userid]/game

post /api/user (create new user)
get /api/user/[userid]
   response {stats? options?}
   post /api/user/[userid]/delete
   ?post /api/user/[userid]/confirmdelete

get /api/user/[userid]/game
   resonse list of games for the user
   //// or is the links each game to continue
   get /api/user/[userid]/game/[gameid]

post /api/user/[userid]/newgame
   response {gameid}
   get /api/user/[userid]/game/[gameid]

get /api/user/[userid]/game/[gameid]
   ////links
   // just created
   /api/user/[userid]/game/[gameid]/seal
   /api/user/[userid]/game/[gameid]/reroll
   /api/user/[userid]/game/[gameid]/edit
   /api/user/[userid]/game/[gameid]/abandon
   // sealed game (post create), still alive, turn avaliable
   /api/user/[userid]/game/[gameid]/endturn
   /api/user/[userid]/game/[gameid]/unit {characters,buildings}
   /api/user/[userid]/game/[gameid]/abandon
   // sealed game, no turn avaliable
   /api/user/[userid]/game/[gameid]/abandon
   use abandon to increment score on user?

//requires session?
get /api/map/
//requires session
get /api/data/bestiary/
get /api/data/terrain/

say you had a set of games to continue, and you wanted to enforce max of 8 games (so options to replace, remove?)
don't get the new_game option if at max game slots
don't want client to construct uri, but for a table, how about template links

get /api/user/[userid]/game
   reponse [{gameid, date last used, nb of units, name and level fav unit, in use by session hint}]
   ////links [template on gameid]
   get /api/user/[userid]/game/[gameid]
   delete /api/user/[userid]/game/[gameid]




function get(path, ...middleware) {
    var req = {}; var res = {}; var next; var func;

    function next() {
        func = middleware.shift();
        if (typeof func === 'function') {
            func(req, res, next);
        }
    }

    next();
}

     */

   var m_namedCallbackMap = {};
   App.Server.Root_AddNamedCallback = function (in_name, in_callback) {
      m_namedCallbackMap[in_name] = in_callback;
   };
   App.Server.Root_GetNamedCallback = function (in_name) {
      return m_namedCallbackMap[in_name];
   };

   var m_routeRoot = { "path" : "/", "children": {} }; //{run:function(in_request, in_pathSegment), children:{}, callbackMap:{}}
   //have a mock inbuild route middleware to deal with url paths
   //route string "/foo/:foo_id/bar", callback map { "POST":[CreateCallback], "GET":[RequestCallback], "PUT":[Update..],"DELETE":[Delete...]}, [link array]
   App.Server.Root_AddRoute = function (in_routeString, in_callbackMap) {
      var routeArray = in_routeString.split("/");
      if ((0 < routeArray.length) && ("" === routeArray[0])) {
         routeArray.shift();
      }
      var tractRouteString = "";
      var trace = m_routeRoot;
      for (var index = 0; index < routeArray.length; ++index) {
         var pathSegment = routeArray[index];
         tractRouteString += "/" + pathSegment;
         if (pathSegment in trace.children) {
            trace = trace.children[pathSegment];
         } else {
            if (":" === pathSegment[0]) {
               var paramName = pathSegment.substring(1);
               var node1 = {
                  "run": function (in_request, in_response, in_pathSegment) {
                     in_request.params[paramName] = in_pathSegment;
                  },
                  "children": {}
               };
               trace.children[""] = node1;
               trace = node1;
            } else {
               var node2 = {
                  "children": {}
               };
               trace.children[pathSegment] = node2;
               trace = node2;
            }
            trace.path = tractRouteString;
         }
      }
      //append our in_callbackMap, do we replace or concat callback arrays?
      trace.callbackMap = Object.assign(trace.callbackMap ? trace.callbackMap : {}, in_callbackMap);
   };

   //request.params = { "foo_id" : value }
   // request { "url", "headers", "method", "body", "param"} ("onError"?)
   // response { "statusCode", "setHeader", "writeHead", "write", "end"} ("onError"?)
   App.Server.Network_RequestListener = function (in_request, in_response) {

      if (false === ("params" in in_request)) {
         in_request.params = {};
      }

      //our mock server is just has a hard coded router
      var routeArray = in_request.url.split("/");
      if ((0 < routeArray.length) && ("" === routeArray[0])) {
         routeArray.shift();
      }

      var trace = m_routeRoot;
      for (var index = 0; index < routeArray.length; ++index) {
         if (undefined === trace) {
            break;
         }
         var pathSegment = routeArray[index];
         if (pathSegment in trace.children) {
            trace = trace.children[pathSegment];
         } else if ("run" in trace) {
            trace.run(in_request, in_response, pathSegment);
            trace = trace.children[""];
         }
      }
      if ((undefined !== trace) && (undefined !== trace.callbackMap) && (in_request.method in trace.callbackMap)) {
         in_request.route = trace;
         var callbackArray = trace.callbackMap[in_request.method];
         var callbackIndex = 0;
         var func = undefined;
         var next = function () {
            if (callbackIndex < callbackArray.length) {
               func = callbackArray[callbackIndex];
               callbackIndex += 1;
               if (typeof func === 'function') {
                  func(in_request, in_response, next);
               }
            }
         }
         next();
      } else {
         in_response.status(404);
         in_response.end();
      }

   }
})();