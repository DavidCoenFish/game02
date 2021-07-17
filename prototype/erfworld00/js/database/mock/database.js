//in memory database emulating redis
(function () {
   var m_data = {};
   var m_dataTimeout = {};
   //key value [EX seconds|PX milliseconds|EXAT timestamp|PXAT milliseconds-timestamp|KEEPTTL] [NX|XX] [GET]
   App.Database.Server_SET = function (key, value) {
      var result = undefined;
      var setValue = true;
      var keepTimeOut = false;
      var setTimeOut = false;
      var variableTimeoutMilliseconds = 0;
      var keyExisit = key in m_data;
      m_data[key] = value;
      for (let index = 2; index < arguments.length; index++) {
         var argument = arguments[index];
         switch (argument) {
            default:
               break;
            case "EX":
               setTimeOut = true;
               index += 1;
               variableTimeoutMilliseconds = parseInt(arguments[index]) * 1000;
               break;
            case "PX":
               setTimeOut = true;
               index += 1;
               variableTimeoutMilliseconds = parseInt(arguments[index]);
               break;
            case "EXAT":
               setTimeOut = true;
               index += 1;
               variableTimeoutMilliseconds = parseInt(arguments[index]) - Math.floor(Date.now() / 1000);
               break;
            case "PXAT":
               setTimeOut = true;
               index += 1;
               variableTimeoutMilliseconds = parseInt(arguments[index]) - Math.floor(Date.now());
               break;
            case "KEEPTTL":
               keepTimeOut = true;
               break;
            case "NX"://onlySetKeyNotExisit
               setValue = !keyExisit;
               break;
            case "XX"://onlySetKeyAlreadyExisit
               setValue = keyExisit;
               break;
            case "GET":
               result = m_data[key];
               break;
         }
      }
      if (true === setValue) {
         m_data[key] = value;
      }
      if (((true === setTimeOut) || (keepTimeOut === false)) && (key in m_dataTimeout)) {
         clearTimeout(m_dataTimeout[key]);
         delete m_dataTimeout[key];
      }
      if (true === setTimeOut) {
         m_dataTimeout[key] = setTimeout(function () {
            delete m_data[key];
            delete m_dataTimeout[key];
         }, variableTimeoutMilliseconds);
      }
      return result;
   };
   App.Database.Server_GET = function (key) {
      return m_data[key];
   };
   App.Database.Server_MGET = function () {
      var result = [];
      for (let index = 0; index < arguments.length; index++) {
         result.push(m_data[arguments[index]]);
      }
      return result;
   };
   App.Database.Server_MSET = function () {
      for (let index = 0; index < arguments.length; index += 2) {
         m_data[arguments[index]] = arguments[index + 1]
      }
      return;
   };
   App.Database.Server_EXISTS = function () {
      var result = 0;
      for (let index = 0; index < arguments.length; index++) {
         if (arguments[index] in m_data) {
            result += 1;
         }
      }
      return result;
   };
   App.Database.Server_DEL = function () {
      var result = 0;
      for (let index = 0; index < arguments.length; index++) {
         var key = arguments[index];
         if (key in m_data) {
            delete m_data[key];
            result += 1;
         }
      }
      return result;
   };
   App.Database.Server_INCR = function (key) {
      if (false === key in m_data) {
         m_data[key] = 1;
      } else {
         m_data[key] += 1;
      }
   }
   App.Database.Server_INCRBY = function (key, delta) {
      if (false === key in m_data) {
         m_data[key] = delta;
      } else {
         m_data[key] += delta;
      }
   }
   App.Database.Server_DECR = function (key) {
      if (false === key in m_data) {
         m_data[key] = -1;
      } else {
         m_data[key] -= 1;
      }
   }
   App.Database.Server_DECRBY = function (key, delta) {
      if (false === key in m_data) {
         m_data[key] = -delta;
      } else {
         m_data[key] -= delta;
      }
   }
   App.Database.Server_HGET = function (key, field) {
      if (key in m_data) {
         return m_data[key][field];
      }
      return null;
   }
   App.Database.Server_HGETAll = function (key) {
      var result = [];
      if (key in m_data) {
         var hash = m_data[key];
         const keys = Object.keys(hash);
         keys.forEach((hashKey) => {
            result.push(hashKey);
            result.push(hash[hashKey]);
         });
      }
      return result;
   }
   App.Database.Server_HDEL = function (key, field) {
      var result = 0;
      if (key in m_data) {
         var hash = m_data[key];
         for (let index = 1; index < arguments.length; index++) {
            var hashKey = arguments[index];
            if (hashKey in hash) {
               delete hash[hashKey];
               result += 1;
            }
         }
      }
      return result;
   }
   App.Database.Server_HEXISTS = function (key, field) {
      var result = 0;
      if (key in m_data) {
         var hash = m_data[key];
         if (field in hash) {
            result = 1;
         }
      }
      return result;
   }
   App.Database.Server_SADD = function (key) {
      var result = 0;
      if (false === key in m_data) {
         m_data[key] = new Set();
      }
      var set = m_data[key];
      for (let index = 1; index < arguments.length; index++) {
         var member = arguments[index];
         if (false === member in set) {
            set.add(member)
            result += 1;
         }
      }

      return result;
   }

   App.Database.Server_SMEMBERS = function (key) {
      var result = [];
      if (key in m_data) {
         var set = m_data[key];
         result = Array.from(set);
      }
      return result;
   }

   App.Database.Root_SetData = function (data) {
      m_data = Object.assign(m_data, data);
   }
})();