/*
https://christophermcdowell.dev/post/pagination-with-redis/
SET post:1 "Super interesting blog post about something."
SADD posts post:1 post:2 post:3 post:4 post:5 post:6 post:7 post:8 post:9 post:10
SSCAN posts 0 COUNT 2

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
   route.Use("GET", "/v0/api/users", GetContainer);
   route.Use("POST", "/v0/api/users", PostContainer);
   route.Use("GET", "/v0/api/users/:userid", GetContainerItem);
   route.Use("PUT", "/v0/api/users/:userid", PutContainerItem);
   route.Use("DELETE", "/v0/api/users/:userid", DeleteContainerItem);

})();