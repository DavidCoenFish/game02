(function () {
   function CheckValue(lhs, rhs, message) {
      if (lhs !== rhs) {
         App.Unittest.LogError(message + " " + String(lhs) + " != " + String(rhs));
      }
   }

   function CheckValueRange(in_low, in_high, in_actual, in_message) {
      if (false === ((in_low <= in_actual) && (in_actual <= in_high))) {
         App.Unittest.LogError(`${in_message} result ${in_actual} outside range ${in_low} ${in_high}`);
      }
   }


   function MakeRequest(in_method, in_url, in_headers, in_body) {
      return {
         "url": in_url,
         "method": in_method,
         "headers": in_headers ? in_headers : {},
         "body": in_body ? in_body : {}
      };
   }

   function MakeResponse() {
      var m_status = undefined;
      var m_content = undefined;
      var m_end = undefined;
      var m_headers = {};
      var result = {
         "status": function (in_status) {
            m_status = in_status;
            return result;
         },
         "getStatus": function () {
            return m_status;
         },
         "setHeader": function (in_key, in_value) {
            m_headers[in_key] = in_value;
            return result;
         },
         "getHeaders": function () {
            return m_headers;
         },
         "json": function (in_json) {
            m_content = JSON.stringify(in_json);
            m_end = true;
            return result;
         },
         "getContent": function () {
            return m_content;
         },
         "end": function () {
            m_end = true;
            return result;
         },
         "getEnd": function () {
            return m_end;
         }
      };
      return result;
   }

   function TestEndpointNotFound() {
      App.Unittest.LogInfo("=======================TestEndpointNotFound");
      var request = MakeRequest("GET", "");
      var response = MakeResponse();

      App.Server.Network_RequestListener(request, response);

      CheckValue(404, response.getStatus(), "status not found");
      CheckValue(true, response.getEnd(), "request ended");
   }

   function TestEndpointApi() {
      App.Unittest.LogInfo("=======================TestEndpointApi");
      var request = MakeRequest("GET", "/api");
      var response = MakeResponse();

      App.Server.Network_RequestListener(request, response);

      CheckValueRange(200, 299, response.getStatus(), "status ok");
      CheckValue(true, response.getEnd(), "request ended");
      App.Server.LogInfo(response.getContent());
   }

   function TestEndpointRecursiveImpl(in_visited, in_method, in_path, in_headers) {
      var visitedKey = `${in_method}:${in_path}`;
      if (visitedKey in in_visited) {
         return;
      }
      in_visited[visitedKey] = true;

      var request = MakeRequest(in_method, in_path, in_headers);
      var response = MakeResponse();

      App.Server.Network_RequestListener(request, response);
      var status = response.getStatus();
      if ((200 <= status) && (status <= 299)) {
         App.Unittest.LogInfo(`${in_method} ${in_path}`);
         var content = response.getContent();
         var result = JSON.parse(content);
         if (result._links !== undefined) {
            for (var key in result._links) {
               var data = result._links[key];
               TestEndpointRecursiveImpl(in_visited, data.method, data.href, in_headers);
            }
         }
      } else {
         App.Unittest.LogInfo(`error ${status} ${in_method} ${in_path}`);
      }
   }

   //no authorisation, try to visit every endpoint we can find
   function TestEndpointRecursive() {
      App.Unittest.LogInfo("=======================TestEndpointRecursive");
      TestEndpointRecursiveImpl({}, "GET", "/api");
   }

   function TestEndpointRecursiveAuthorised() {
      App.Unittest.LogInfo("=======================TestEndpointRecursiveAuthorised");

      var request = MakeRequest("POST", "/api/v0/actions/guest");
      var response = MakeResponse();

      App.Server.Network_RequestListener(request, response);
      var status = response.getStatus();
      if ((200 <= status) && (status <= 299)) {
         var headers = response.getHeaders(); //autho
         var visited = {
            "POST:/api/v0/actions/guest" : true,
            "POST:/api/v0/actions/logout": true,
         };
         TestEndpointRecursiveImpl(visited, "GET", "/api", headers);
      } else {
         App.Unittest.LogInfo(`error ${status} POST /api/v0/actions/guest`);
      }

   }

   function Main() {
      App.Unittest.LogInfo("Server");
      TestEndpointNotFound();
      TestEndpointApi();
      TestEndpointRecursive();
      TestEndpointRecursiveAuthorised();
   }

   App.RegisterOnLoad(Main);
})();