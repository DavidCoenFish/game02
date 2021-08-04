(function () {
   var m_stack = [];
   function AddMiddleware(in_path, in_method, in_calback) {
      m_stack

   }

   function RequestListener(in_request, in_response) {
      var callbackIndex = 0;
      var func = undefined;
      var next = function () {
         if (callbackIndex < m_arrayMiddleware.length) {
            func = m_arrayMiddleware[callbackIndex];
            callbackIndex += 1;
            if (typeof func === 'function') {
               func(in_request, in_response, next);
            }
         }
      }
      next();
   }

/*
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
*/
})();