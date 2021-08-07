/*
database... need to search user to login, rather than uuid to identify, 
user name -> base64 or something that could use as key?
 //users.sort //sorted list, <score, uuid> if you need to paganate the users?
 users.map.[username] //<username, uuid>
 users.timestamp //
 users.changeid
 users.data.[uuid] //{password, pref, games[uid]}

 games.[uuid]
 ?games.[uuid].commands.[id] {data}, or game just has link to commands
 commands.[uuid]
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
   //route.Use("GET", "/api/v0/games", GetContainer);

})();