(function () {
   //https://stackoverflow.com/questions/736513/how-do-i-parse-a-url-into-hostname-and-path-in-javascript
   function Callback(in_request, in_response, in_next) {
      var href = in_request.url; //.replace("%20", " ").replace("%21", "!").replace("%23", "#").replace("%2B", "+")
      var match = href.match(/^(?:(https?\:)\/\/)?(([^:\/?#]*)(?:\:([0-9]+))?)([\/]{0,1}[^?#]*)(\?[^#]*|)(#.*|)$/);
      var result = {
         href: href,
         protocol: match ? match[1] : undefined,
         host: match ? match[2] : undefined,
         hostname: match ? match[3] : undefined,
         port: match ? match[4] : undefined,
         pathname: match ? match[5].replace("%20", " ").replace("%21", "!").replace("%23", "#").replace("%2B", "+") : undefined,
         search: match && match[6] ? match[6].substring(1) : undefined,
         hash: match && match[7] ? match[7].substring(1) : undefined
      }

      var searchMap = {};
      if (result.search) {
         var tokenPairs = result.search.split("&");
         for (var index = 0; index < tokenPairs.length; ++index) {
            var keyValue = tokenPairs[index].split("=");
            searchMap[keyValue[0]] = keyValue[1];
         }
      }
      result.searchMap = searchMap;

      in_request.urlObject = result;
      in_next();
   }


   App.Server.Root_Use("", "", Callback);
})();