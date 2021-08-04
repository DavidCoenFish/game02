(function () {
   function CheckValue(lhs, rhs, message) {
      if (lhs !== rhs) {
         App.Unittest.LogError(message + " " + String(lhs) + " != " + String(rhs));
         return false;
      }
   }

   function MakeRequest(in_method, in_url) {
      return {
         "url": in_url,
         "method": in_method
      };
   }

   function MakeResponse() {
      var m_status = undefined;
      var m_content = undefined;
      var m_end = undefined;
      var result = {
         "status": function (in_status) {
            m_status = in_status;
            return result;
         },
         "getStatus": function () {
            return m_status;
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
         },
         "getEnd": function () {
            return m_end;
         }
      };
      return result;
   }

   function TestEndpointNotFound() {
      App.Unittest.LogInfo(" TestEndpointNotFound");
      var request = MakeRequest("GET", "");
      var response = MakeResponse();

      App.Server.Network_RequestListener(request, response);

      CheckValue(404, response.getStatus(), "status not found");
      CheckValue(true, response.getEnd(), "request ended");
   }

   function TestEndpointApi() {
      App.Unittest.LogInfo(" TestEndpointApi");
      var request = MakeRequest("GET", "/v0/api");
      var response = MakeResponse();

      App.Server.Network_RequestListener(request, response);

      CheckValue(200, response.getStatus(), "status ok");
      CheckValue(true, response.getEnd(), "request ended");
      App.Server.LogInfo(response.getContent());

   }

   function Main() {
      App.Unittest.LogInfo("Server");
      TestEndpointNotFound();
      TestEndpointApi();
   }

   App.RegisterOnLoad(Main);
})();