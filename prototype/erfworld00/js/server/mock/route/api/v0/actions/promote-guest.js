/*
need to remove the timeout on any resource created with the guest user (minus the session) which may include games, units, commands

need to check provided username and password

*/
(function () {
   var PromoteGuest = function (in_request, in_response, in_next) {
      App.Server.Root_MakeErrorResponseInternalServerError(in_request, in_response, "promote guest. unimplemented");
      //in_response.status(200);
      //in_response.json(data);
      //follow convention of not calling next once we call response end/json?
      //in_next();
   }

   var route = App.Server.Root_GetKeyValue("Route");
   var AuthorizeUserSession = App.Server.Root_GetKeyValue("AuthorizeUserSession");
   route.Use("POST", "/api/v0/actions/promote-guest", [AuthorizeUserSession, PromoteGuest]);


})();