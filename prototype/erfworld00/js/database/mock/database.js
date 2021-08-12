//in memory database emulating redis
/*
database... need to search user to login, rather than uuid to identify,
user name -> base64 or something that could use as key?
 //users.sort //sorted list, <score, uuid> if you need to paganate?
 users.etag
 users.map.[name]
 users.data.[uuid]

 //sessions.sort //sorted list, <score, uuid> if you need to paganate
 sessions.etag
 sessions.data.[uuid]

 //games.sort //sorted list, <score, uuid> if you need to paganate
 games.etag
 games.data.[uuid]

 */

(function () {
   var m_data = {};
   var m_dataTimeout = {};
   //key value [EX seconds|PX milliseconds|EXAT timestamp|PXAT milliseconds-timestamp|KEEPTTL] [NX|XX] [GET]
   App.Database.Server_SET = function (key, value) {
      var result = "OK";
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

   //https://www.freecodecamp.org/news/implementing-a-linked-list-in-javascript/
   class ListNode {
      constructor(in_score, in_data) {
         this.m_score = in_data
         this.m_data = in_data
         this.m_next = null
      }
   }
   class LinkedList {
      constructor(in_head = null) {
         this.m_head = in_head
      }
      Insert(in_score, in_data) {
         var node = new ListNode(in_score, in_data);

         var trace = this.m_head;
         if (trace && trace.m_score < in_score) {
            while (trace.m_next && (trace.m_score < in_score)) {
               trace = trace.next
            }
            node.m_next = trace.m_next;
            trace.m_next = node;
         } else {
            node.m_next = this.m_head;
            this.m_head = node;
         }
      }
      Exists(in_data) {
         var trace = this.m_head;
         while (trace) {
            if (in_data === trace.m_data) {
               return true;
            }
            trace = trace.m_next
         }
         return false;
      }
      GetNode(in_data) {
         var trace = this.m_head;
         while (trace) {
            if (in_data === trace.m_data) {
               return trace;
            }
            trace = trace.m_next
         }
         return null;
      }
      Remove(in_data) {
         var trace = this.m_head;
         var previous = undefined;
         while (trace) {
            if (in_data === trace.m_data) {
               if (previous) {
                  previous.m_next = trace.m_next;
               } else {
                  this.m_head = trace.m_next;
               }
               trace.m_next = null;
               return 1;
            }
            previous = trace;
            trace = trace.next
         }
         return 0;
      }
   }

   App.Database.Server_ZRANGE = function (in_key, min, max) {
      var linkedList = undefined;
      if (in_key in m_data) {
         linkedList = m_data[in_key];
      }
      var result = [];
      if (undefined === linkedList) {
         return result;
      }
      var trace = linkedList.m_head;
      var index = 0;
      //for (var index = 0; index < max; ++index) {
      while (trace !== null) {
         if ((max !== -1) && (max < index)) {
            break;
         }
         if (min <= index) {
            result.push(trace.m_data);
         }
         trace = trace.m_next;
         index += 1;
      }
      return result;
   }

   App.Database.Server_ZADD = function (in_key) { // [NX|XX] [GT|LT] [CH] [INCR] score member [score member ...]
      ////key value [EX seconds|PX milliseconds|EXAT timestamp|PXAT milliseconds-timestamp|KEEPTTL] [NX|XX] [GET]
      //App.Database.Server_SET = function (key, value) {
      var linkedList = undefined;
      if (in_key in m_data) {
         linkedList = m_data[in_key];
      } else {
         linkedList = new LinkedList();
         m_data[in_key] = linkedList;
      }
      var elementAddedCount = 0;
      var elementChangeCount = 0;

      var onlyAddNewElements = false;
      var onlyUpdateExistingElements = false;
      var onlyUpdateExistingElementsIfScoreLower = false; //can still add new
      var onlyUpdateExistingElementsIfScoreGreater = false; //can still add new
      var returnChangeElementCount = false;
      for (let index = 1; index < arguments.length; index++) {
         var argument = arguments[index];
         var scoreMemberPair = false;
         switch (argument) {
            default:
               scoreMemberPair = true;
               break;
            case "NX":
               onlyAddNewElements = true;
               break;
            case "XX":
               onlyUpdateExistingElements = true;
               break;
            case "GT":
               onlyUpdateExistingElementsIfScoreGreater = true;
               break;
            case "LT":
               onlyUpdateExistingElementsIfScoreLower = true;
               break;
            case "CH":
               returnChangeElementCount = true;
               break;
            case "INCR":
               onlyUpdateExistingElements = true;
               break;
         }
         if (true === scoreMemberPair) {
            var score = undefined;
            switch (argument) {
               default:
                  score = parseFloat(argument);
                  break;
               case "+inf":
                  score = Number.MAX_VALUE;
                  break;
               case "-inf":
                  score = -Number.MAX_VALUE;
                  break;
            }
            index += 1;
            var member = arguments[index];

            if (onlyAddNewElements ||
               onlyUpdateExistingElements ||
               onlyUpdateExistingElementsIfScoreLower ||
               onlyUpdateExistingElementsIfScoreGreater) {
               var node = linkedList.GetNode(member);
               var exists = (null !== node);
               if ((onlyAddNewElements === true) && (exists === true)) {
                  continue;
               }
               if ((onlyUpdateExistingElements === true) && (exists === false)) {
                  continue;
               }
               if (node && onlyUpdateExistingElementsIfScoreLower && (node.m_score <= score)) {
                  continue;
               }
               if (node && onlyUpdateExistingElementsIfScoreGreater && (score <= node.m_score)) {
                  continue;
               }
            }
            if (0 === linkedList.Remove(member)) {
               elementAddedCount += 1;
               elementChangeCount += 1;
            } else {
               elementChangeCount += 1;
            }
            linkedList.Insert(score, member);
         }
      }

      if (true === returnChangeElementCount) {
         return elementChangeCount;
      }
      return elementAddedCount;
   }

   App.Database.Server_ZREM = function (in_key, in_member) { // [member ...]
      var linkedList = undefined;
      if (in_key in m_data) {
         linkedList = m_data[in_key];
      }

      if (linkedList) {
         return 0;
      }
      var result = 0;
      for (let index = 1; index < arguments.length; index++) {
         var argument = arguments[index];
         result += linkedList.Remove(argument)
      }
      return result;
   }

   App.Database.Root_SetData = function (data) {
      m_data = Object.assign(m_data, data);
   }
})();