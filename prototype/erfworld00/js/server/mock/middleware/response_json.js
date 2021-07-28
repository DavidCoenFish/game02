/*
 expect a JSON object in_response.data
 */
(function () {
   function Callback(in_request, in_response, in_next) {
      if (false === ("data" in in_response)) {
         return in_response.status(500).json({ message: 'Internal Server Error' });
      }
      in_response.status(200).json(in_response.data).end();
      in_next();
   };

   App.Server.Root_AddNamedCallback("ResponseJson", Callback);
})();