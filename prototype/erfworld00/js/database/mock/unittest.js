(function () {
   function CheckValue(lhs, rhs, message) {
      if (lhs !== rhs) {
         App.Unittest.LogError(message + " " + String(lhs) + " != " + String(rhs));
         return false;
      }
   }

   function SanityCheck0() {
      App.Unittest.LogInfo(" SanityCheck0");
      var result;
      CheckValue(0, App.Database.Server_EXISTS("unittest"), "SanityCheck0_0");
      CheckValue("OK", App.Database.Server_SET("unittest", 4), "SanityCheck0_1");
      CheckValue(1, App.Database.Server_EXISTS("unittest"), "SanityCheck0_2");
      CheckValue(1, App.Database.Server_DEL("unittest"), "SanityCheck0_3");
      CheckValue(0, App.Database.Server_EXISTS("unittest"), "SanityCheck0_4");
      CheckValue(0, App.Database.Server_DEL("unittest"), "SanityCheck0_5");
   }

   function SanitySortedSet0() {
      App.Unittest.LogInfo(" SanitySortedSet0");
      CheckValue(0, App.Database.Server_EXISTS("unittest"), "SanitySortedSet0_0");
      CheckValue(1, App.Database.Server_ZADD("unittest", 1.4, "one"), "SanitySortedSet0_1");
      CheckValue(0, App.Database.Server_ZADD("unittest", 1.4, "one"), "SanitySortedSet0_2");
      CheckValue(1, App.Database.Server_DEL("unittest"), "SanitySortedSet0_3");
   }

   function Main() {
      App.Unittest.LogInfo("Database");
      SanityCheck0();
      SanitySortedSet0();
   }

   App.RegisterOnLoad(Main);
})();