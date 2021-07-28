(function () {
   function MakeMockLatencyDelayMillisecond() {
      var latency = App.Globals.mockNetworkLatencyMillisecond + (((Math.random() * 2.0) - 1.0) * App.Globals.mockNetworkLatencyMillisecondPlusMinus)
      return latency;
   }

   //http://www.sean-crist.com/professional/pages/language_region_codes/index.html
   App.Network.Client_AsyncGetAuthoriseTokenAndBootstrapData = function (callbackPass, callbackError, userName, password) {
      setTimeout(function () {
         callbackPass({
            "AuthoriseToken": "dummy",
            "StateData": {
               "UserId": 0, //"f552f056-2fbb-463b-84a9-18c422fd4289",
               "Language": "en", //ISO 639-1
               "Region": "au", //ISO 3166-1 alpha-2
               "DatabaseVersion": "0.0.0.0", //database version
               "ServerVersion": "0.0.0.0"
            },
            //ViewTemplate
            //ClientState
            "SetState": "Launch"
         });
      }, MakeMockLatencyDelayMillisecond());
   };

   function MakeRequest(endpoint, authorizeToken, method, body){
      return {
         "url": endpoint,
         "headers": {
            "authorize_token": authorizeToken //{"userId":,"session":}?
         },
         "method": method,
         "body": body
      };
   };

   function MakeResponse(callbackPass, callbackError) {
      var mStatusCode = 0; //500; //Internal Server Error
      var mHeader = {};
      var mBody = undefined;
      var mThis = undefined;
      function Status(statusCode) {
         mStatusCode = statusCode;
         return mThis;
      }
      function Json(data) {
         SetHeader('Content-Type', 'application/json');
         mBody = JSON.stringify(data);
         return mThis;
      }
      function SetHeader(name, value) {
         mHeader[name] = value;
         return mThis;
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
         return mThis;
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
         return mThis;
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
         return mThis;
      }
      //will self referencing cause memory bloat?
      mThis = {
         "status": Status,
         "json": Json,
         "setHeader": SetHeader,
         "writeHead": WriteHead,
         "write": Write,
         "end": End
      };
      return mThis;
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