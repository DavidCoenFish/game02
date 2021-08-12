/*
*/
(function () {
   var GetContainer = function (in_request, in_response, in_next) {
      var offset = "offset" in in_request.urlObject.searchMap ? in_request.urlObject.searchMap["offset"] : 0;
      var limit = "limit" in in_request.urlObject.searchMap ? in_request.urlObject.searchMap["limit"] : 25;


      //in_response.status(200);
      //in_response.json(data);
      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   //route.Use("GET", "/api/v0/users", GetContainer);
   //route.Use("POST", "/api/v0/users", PostContainer);
   //route.Use("GET", "/api/v0/users/:userid", GetContainerItem);
   //route.Use("PUT", "/api/v0/users/:userid", PutContainerItem);
   //route.Use("DELETE", "/api/v0/users/:userid", DeleteContainerItem);

})();