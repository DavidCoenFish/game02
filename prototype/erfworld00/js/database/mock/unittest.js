(function () {
   function CheckValue(lhs, rhs, message) {
      if (lhs !== rhs) {
         App.Unittest.LogError(message + " " + String(lhs) + " != " + String(rhs));
         return false;
      }
   }

   function SanityCheck0() {
      App.Unittest.LogInfo("SanityCheck0");
      CheckValue(0, App.Database.Server_EXISTS("unittest"), "SanityCheck0_0");
      App.Database.Server_SET("unittest", 4);
      CheckValue(1, App.Database.Server_EXISTS("unittest"), "SanityCheck0_1");
      App.Database.Server_DEL("unittest");
      CheckValue(0, App.Database.Server_EXISTS("unittest"), "SanityCheck0_2");
   }

   function Main() {
      App.Unittest.LogInfo("Database");
      SanityCheck0();
   }

   App.RegisterOnLoad(Main);
})();