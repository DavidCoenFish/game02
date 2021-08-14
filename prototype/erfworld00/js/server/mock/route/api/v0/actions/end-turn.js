/*
*/
(function () {
   var PostEndturn = function (in_request, in_response, in_next) {
      App.Server.Root_MakeErrorResponseInternalServerError(in_request, in_response, "end turn. unimplemented");

      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   var AuthorizeUserSession = App.Server.Root_GetKeyValue("AuthorizeUserSession");
   route.Use("POST", "/api/v0/actions/end-turn", [AuthorizeUserSession, PostEndturn]);

})();