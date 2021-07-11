(function () {
   function MakeMockLatencyDelayMillisecond() {
      var latency = App.Globals.mockNetworkLatencyMillisecond + (((Math.random() * 2.0) - 1.0) * App.Globals.mockNetworkLatencyMillisecondPlusMinus)
      return latency;
   }

   //http://www.sean-crist.com/professional/pages/language_region_codes/index.html
   App.Network.Client_GetUserDataAuthoriseToken = function (callbackPass, callbackError, userName, password) {
      setTimeout(function () {
         callbackPass({
            "authoriseToken": "dummy",
            "state": {
               "userId": 0, //"f552f056-2fbb-463b-84a9-18c422fd4289",
               "language": "en", //ISO 639-1
               "region": "au", //ISO 3166-1 alpha-2
               "dataVersion": "0.0.0.0", //database version
               //a minimum set of localisation keys to prep the loading screen
            },
            //"boot": {
            //   //"Progress": "Progress",
            //   //"{0}": "{0}",
            //   "Client Version:[{0}]": "Client Version:[{0}]",
            //   "Database Version:[{0}]": "Database Version:[{0}]",
            //   "Error:[{0}]": "Error:[{0}]",
            //   "Debug:[{0}]": "Debug:[{0}]",
            //}
         });
      }, MakeMockLatencyDelayMillisecond());
   };

   function MakeRequest(endpoint, authorizeToken, method, body){
      return {
         "url": endpoint,
         "headers": {
            "authorize_token": authorizeToken
         },
         "method": method,
         "body": body
      };
   };

   function MakeResponse(callbackPass, callbackError) {
      var mStatusCode = 0; //500; //Internal Server Error
      var mHeader = {};
      var mBody = undefined;
      function SetHeader(name, value) {
         mHeader[name] = value;
      }
      function WriteHead(statusCode, statusMessage, headers) {
         mStatusCode = statusCode;
         if (true === Array.isArray(headers)) {
            for (let index = 0; index < headers.length; index += 2) {
               mHeader[headers[index]] = headers[index + 1];
            }
         } else {
            const keys = Object.keys(headers);
            keys.forEach((key) => {
               mHeader[key] = headers[key];
            });
         }
      }
      function Write(chunk, encoding, callback) {
         if (mBody !== undefined) {
            mBody += chunk;
         } else {
            mBody = chunk;
         }
         if (callback) {
            callback();
         }
      }
      function End(data, encoding, callback) {
         if (data) {
            Write(data, encoding, callback);
         }
         if ((mStatusCode < 200) || (299 < mStatusCode)) {
            callbackError("response status:" + mStatusCode)
         } else {
            callbackPass(mBody);
         }
      }
      return {
         "setHeader": SetHeader,
         "writeHead": WriteHead,
         "write": Write,
         "end": End
      };
   }

   //[create]
   App.Network.Client_POST = function (callbackPass, callbackError, authorizeToken, endpoint, data) {
      var request = MakeRequest(endpoint, authorizeToken, "POST", data);
      var response = MakeResponse(callbackPass, callbackError);
      setTimeout(function () {
         App.Server.Network_RequestListener(request, response);
      }, MakeMockLatencyDelayMillisecond());
   };
   //[read]
   App.Network.Client_GET = function (callbackPass, callbackError, authorizeToken, endpoint) {
      var request = MakeRequest(endpoint, authorizeToken, "GET");
      var response = MakeResponse(callbackPass, callbackError);
      setTimeout(function () {
         App.Server.Network_RequestListener(request, response);
      }, MakeMockLatencyDelayMillisecond());
   };
   //[update]
   App.Network.Client_PUT = function (callbackPass, callbackError, authorizeToken, endpoint, data) {
      var request = MakeRequest(endpoint, authorizeToken, "PUT", data);
      var response = MakeResponse(callbackPass, callbackError);
      setTimeout(function () {
         App.Server.Network_RequestListener(request, response);
      }, MakeMockLatencyDelayMillisecond());
   };
   //[delete]
   App.Network.Client_DELETE = function (callbackPass, callbackError, authorizeToken, endpoint) {
      var request = MakeRequest(endpoint, authorizeToken, "DELETE");
      var response = MakeResponse(callbackPass, callbackError);
      setTimeout(function () {
         App.Server.Network_RequestListener(request, response);
      }, MakeMockLatencyDelayMillisecond());
   };
})();