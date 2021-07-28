/*
 expect in the headers of the request under a user authorise endpoint
 /api/user/:user_id/...
 
 */
(function () {
   function Callback(in_request, in_response, in_next) {
      if (false === "authorize_token" in in_request.headers) {//
         return in_response.status(401).json({ message: 'Missing Authorization Header' });
      }

      if (false === "userid" in in_request.params) {
         return in_response.status(500).json({ message: 'Internal Server Error' });
      }

      var userId = in_request.params["userid"]
      var userAuthorise = JSON.parse(in_request.headers["authorize_token"]);
      if (userId !== userAuthorise.userId) {
         return in_response.status(401).json({ message: 'Incorrect userID' });
      }

      var key = `user.${userId}.session.${userAuthorise.session}`;
      if (0 === App.Database.Server_EXISTS(key)) {
         return in_response.status(401).json({ message: 'Invalid session' });
      }

      in_next();
   };

   App.Server.Root_AddNamedCallback("UserAuthorise", Callback);
})();