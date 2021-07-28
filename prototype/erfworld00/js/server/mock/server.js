(function () {
   /*
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