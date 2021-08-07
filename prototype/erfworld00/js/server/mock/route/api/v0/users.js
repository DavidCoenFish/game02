/*
https://christophermcdowell.dev/post/pagination-with-redis/
SET post:1 "Super interesting blog post about something."
SADD posts post:1 post:2 post:3 post:4 post:5 post:6 post:7 post:8 post:9 post:10
SSCAN posts 0 COUNT 2

App.Server.Root_MakeUUID()
Math.random()

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