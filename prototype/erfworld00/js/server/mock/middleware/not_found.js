(function () {
   function Callback(in_request, in_response, in_next) {
      in_response.status(404);
      //in_response.json({ "message": "Resource not found." });
      in_response.end();
      //follow convention of not calling next once we call response end/json? there is more that writting response?
      //in_next();
   };

   App.Server.Root_Use("", "", Callback);
})();