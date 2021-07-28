/*
expect in_request.route = {
   "children" {
      "" : {
         "path" : "",
         "callbackMap" : {
            "Method<CREATE,GET,PUT,DELETE>" : ...
         }
      }
   }
   //"path" : "",
   //"callbackMap"
};
expect a JSON object in_response.data

 */
(function () {
   function Callback(in_request, in_response, in_next) {
      if (("route" in in_request) && ("children" in in_request.route)) {
         if (false === ("data" in in_response)) {
            in_response.data = {};
         }
         var links = [];
         for (var key in in_request.route.children) {
            var child = in_request.route.children[key];
            var methodHint = "";
            if ("callbackMap" in child) {
               if ("POST" in child.callbackMap) {
                  methodHint += "C";
               }
               if ("GET" in child.callbackMap) {
                  methodHint += "R";
               }
               if ("PUT" in child.callbackMap) {
                  methodHint += "U";
               }
               if ("DELETE" in child.callbackMap) {
                  methodHint += "D";
               }
            }
            if ("" !== methodHint) {
               links.push([methodHint, child.path]);
            }
         }
         if (0 < links.length) {
            in_response.data.links = links;
         }
      }

      in_next();
   };

   App.Server.Root_AddNamedCallback("BuildLinks", Callback);
})();