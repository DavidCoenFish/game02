(function () {

   //node: {path:"", callbackMap:{"method":func}, children{};
   var m_root = {
      "children": {}
   };

   function Use(in_method, in_path, in_callbackFuncOrArray, in_name) {
      App.Server.Root_AddLinkData(in_path, in_method, in_name);
      var pathArray = in_path.split("/");
      if ((0 < pathArray.length) && ("" === pathArray[0])) {
         pathArray.shift();
      }
      var trace = m_root;
      var tractPath = "";
      for (var index = 0; index < pathArray.length; ++index) {
         var pathSegment = pathArray[index];
         tractPath += "/" + pathSegment;
         if (pathSegment in trace.children) {
            trace = trace.children[pathSegment];
         } else {
            var newNode = {
               "pathSegment": pathSegment,
               "path": tractPath,
               "children": {}
            };
            if (":" === pathSegment[0]) {
               var paramName = pathSegment.substring(1);
               newNode["run"] = function (in_pathSegment, in_request) {
                  in_request.params[paramName] = in_pathSegment;
               }
            }
            trace.children[pathSegment] = newNode;
            trace = newNode;
         }
      }
      if (undefined !== trace) {
         if (false === "calbackMap" in trace) {
            trace["calbackMap"] = {};
         }
         var map = {};
         map[in_method] = in_callbackFuncOrArray;
         trace.calbackMap = Object.assign(trace.calbackMap, map);
      }
   }

   function Callback(in_request, in_response, in_next) {
      if (false === ("params" in in_request)) {
         in_request.params = {};
      }

      var routeArray = in_request.urlObject.pathname.split("/");
      while ((0 < routeArray.length) && ("" === routeArray[0])) {
         routeArray.shift();
      }

      var trace = m_root;
      for (var index = 0; index < routeArray.length; ++index) {
         if (undefined === trace) {
            break;
         }
         var pathSegment = routeArray[index];
         if (pathSegment in trace.children) {
            trace = trace.children[pathSegment];
         } else if ("run" in trace) {
            trace.run(in_request, in_response, pathSegment);
            trace = trace.children[trace.pathSegment];
         }
      }

      //if we find a route, call it else call next. 
      if (trace && trace.calbackMap && (in_request.method in trace.calbackMap)) {
         var callback = trace.calbackMap[in_request.method];
         if (typeof callback === 'function') {
            callback(in_request, in_response, in_next);
         } else if (Array.isArray(callback)) {
            var callbackIndex = 0;
            var func = undefined;
            var next = function () {
               if (callbackIndex <= callback.length) {
                  if (callbackIndex < callback.length) {
                     func = callback[callbackIndex];
                  } else {
                     func = in_next;
                  }
                  callbackIndex += 1;
                  if (typeof func === 'function') {
                     func(in_request, in_response, next);
                  }
               }
            }
            next();
         }
      } else {
         in_next();
      }
      return;
   };


   App.Server.Root_SetKeyValue("Route", {
      "Use": Use
   });
   App.Server.Root_Use("", "", Callback);
})();