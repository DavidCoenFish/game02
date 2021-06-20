//in memory database emulating redis
(function () {
   var mData = {};
   var mDataTimeout = {};
   //key value [EX seconds|PX milliseconds|EXAT timestamp|PXAT milliseconds-timestamp|KEEPTTL] [NX|XX] [GET]
   App.Database.Server_SET = function (key, value) {
      var result = undefined; 
      var setValue = true;
      var keepTimeOut = false;
      var setTimeOut = false;
      var variableTimeoutMilliseconds = 0;
      var keyExisit = key in mData;
      mData[key] = value;
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
               result = mData[key];
               break;
         }
      }
      if (true === setValue) {
         mData[key] = value;
      }
      if (((true === setTimeOut) || (keepTimeOut === false)) && (key in mDataTimeout)) {
         clearTimeout(mDataTimeout[key]);
         delete mDataTimeout[key];
      }
      if (true === setTimeOut) {
         mDataTimeout[key] = setTimeout(function () {
            delete mData[key];
            delete mDataTimeout[key];
         }, variableTimeoutMilliseconds);
      }
      return result;
   };
   App.Database.Server_GET = function (key) {
      return mData[key];
   };
   App.Database.Server_MGET = function () {
      var result = [];
      for (let index = 0; index < arguments.length; index++) {
         result.push(mData[arguments[index]]);
      }
      return result;
   };
   App.Database.Server_MSET = function () {
      for (let index = 0; index < arguments.length; index += 2) {
         mData[arguments[index]] = arguments[index + 1]
      }
      return;
   };
   App.Database.Server_EXISTS = function () {
      var result = 0;
      for (let index = 0; index < arguments.length; index++) {
         if (arguments[index] in mData) {
            result += 1;
         }
      }
      return result;
   };
   App.Database.Server_DEL = function () {
      var result = 0;
      for (let index = 0; index < arguments.length; index++) {
         var key = arguments[index];
         if (key in mData) {
            delete mData[key];
            result += 1;
         }
      }
      return result;
   };
   App.Database.Server_INCR = function (key) {
      if (false === key in mData) {
         mData[key] = 1;
      } else {
         mData[key] += 1;
      }
   }
   App.Database.Server_INCRBY = function (key, delta) {
      if (false === key in mData) {
         mData[key] = delta;
      } else {
         mData[key] += delta;
      }
   }
   App.Database.Server_DECR = function (key) {
      if (false === key in mData) {
         mData[key] = -1;
      } else {
         mData[key] -= 1;
      }
   }
   App.Database.Server_DECRBY = function (key, delta) {
      if (false === key in mData) {
         mData[key] = -delta;
      } else {
         mData[key] -= delta;
      }
   }
   App.Database.Server_HGET = function (key, field) {
      if (key in mData) {
         return mData[key][field];
      }
      return null;
   }
   App.Database.Server_HGETAll = function (key) {
      var result = [];
      if (key in mData) {
         var hash = mData[key];
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
      if (key in mData) {
         var hash = mData[key];
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
      if (key in mData) {
         var hash = mData[key];
         if (field in hash) {
            result = 1;
         }
      }
      return result;
   }
   App.Database.Server_SADD = function (key) {
      var result = 0;
      if (false === key in mData) {
         mData[key] = new Set();
      }
      var set = mData[key];
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
      if (key in mData) {
         var set = mData[key];
         result = Array.from(set);
      }
      return result;
   }

   App.Database.SetData = function (data) {
      mData = data;
   }
})();