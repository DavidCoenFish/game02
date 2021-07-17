/*
Root namespace, contract for modules to talk to each other
though the network,server,database modules could be mocked so the interfaces are allowed to be different once implemented for server and database? 
 */
var App = (function () {
   function Unimplemented() {
      alert("unimplemented method")
   }
   function RegisterOnLoad(in_callback) {
      if (window.attachEvent) {
         window.attachEvent('onload', in_callback);
      } else {
         if (window.onload) {
            var value = window.onload;
            window.onload = function (evt) {
               value(evt);
               in_callback(evt);
            };
         } else {
            window.onload = in_callback;
         }
      }
   }
   function RegisterBeforeUnload(in_callback) { //beforeunload
      if (window.attachEvent) {
         window.attachEvent('beforeunload', in_callback);
      } else {
         if (window.beforeunload) {
            var value = window.beforeunload;
            window.beforeunload = function (evt) {
               value(evt);
               in_callback(evt);
            };
         } else {
            window.beforeunload = in_callback;
         }
      }
   }
   var m_logArray = [];
   var m_logCallback = undefined;
   function LogMessage(in_module, in_filter, in_message) {
      if (undefined === m_logCallback) {
         m_logArray.push([in_module, in_filter, in_message]);
      } else {
         m_logCallback(in_module, in_filter, in_message);
      }
      switch (in_filter) {
         case "Error":
            console.log(in_module + ":" + in_message);
            break;
         case "Warning":
            console.warn(in_module + ":" + in_message);
            break;
         default:
         case "Info":
            console.info(in_module + ":" + in_message);
            break;
      }
   }
   function SetLogCallback(in_callback) {
      m_logCallback = in_callback;
      if (undefined !== m_logCallback) {
         for (let i = 0; i < m_logArray.length; i++) {
            var data = m_logArray[i];
            m_logCallback(data[0], data[1], data[2]);
         }
         m_logArray = [];
      }
   }

   /*
    document the public interface of each module, plus a hint at what module will be calling the public interface
    */
   return {
      "Unimplemented": Unimplemented,
      "RegisterOnLoad": RegisterOnLoad,
      "RegisterBeforeUnload": RegisterBeforeUnload,
      "LogMessage": LogMessage, //module, filter, message
      //AddLogCallback ?
      "SetLogCallback": SetLogCallback, //fn(module, filter, message)

      "Globals": {
         mockNetworkLatencyMillisecond: 100,
         mockNetworkLatencyMillisecondPlusMinus: 50,
         //mockNetworkErrorRate?
         mockDatabaseLatencyMillisecond: 100,
         mockDatabaseLatencyMillisecondPlusMinus: 50
      },
      "View": {
         "Root_AddTemplate": Unimplemented, //templateName, factory
         "Client_AddTemplate": Unimplemented,//templateName, factory

         // if you want a loading screen, use ClientSetScreen("loading", "loading0", null)?
         //"Client_AddRenderLayer": Unimplemented, //name
         "Client_AddScreen": Unimplemented,//template name, name, IDataSource, takeFocusWhenReady, transitionHint, renderLayerHint
         "Client_RemoveScreen": Unimplemented, //name

         "Client_AddRenderLayer": Unimplemented, //name
         //addTransition?

         "LogError": function (in_message) {
            App.LogMessage("View", "Error", in_message);
         },
         "LogWarning": function (in_message) {
            App.LogMessage("View", "Warning", in_message);
         },
         "LogInfo": function (in_message) {
            App.LogMessage("View", "Info", in_message);
         },
      },
      /*
      IDataSource : {
         "Dtor"
         "SetFocus" //true/false
         "OnButtonClick" //elementName, group data, verb, subject
         "OnElementInput" //elementName
         "GetValueBool"
         "GetValueInt"
         "GetValueString"
         // named structs or generic? GetValueDataButton, GetValueArrayDataButton or just GetValueObject
         "GetValueObject"
         "GetValueArray"
         "MakeGeneratorBool"
         "MakeGeneratorInt"
         "MakeGeneratorString"
         // named structs or generic?
         "MakeGeneratorObject"
         "MakeGeneratorArray"
      }
       */

      "Client": {
         // use RegisterOnLoad instead, allows unittest to add itself. alternative Root_ApplicationInPoint and Root_AddTask?
         //"Root_ApplicationInPoint": Unimplemented,

         "Root_AddStateFactory": Unimplemented, //stateName, factory
         "Client_AddStateFactory": Unimplemented, //stateName, factory
         "Client_SetState": Unimplemented, //name

         "Root_SetData": Unimplemented,//data
         "Client_SetDataValue": Unimplemented,//key, value
         "Client_GetDataValue": Unimplemented,//key
         "Client_AddDataChangeCallback": Unimplemented,//key, callback
         "Client_RemoveDataChangeCallback": Unimplemented,

         "Client_AsyncGetLocalisedText": Unimplemented,//callback, module, key, array of data keys to be converted to text to fill in format blanks of text (__0__, __1__, __2__, ...)
         "Client_SetAuthoriseToken": Unimplemented,//authoriseToken

         "Client_MakeDefaultDataSource": Unimplemented,

         "LogError": function (in_message) {
            App.LogMessage("Client", "Error", in_message);
         },
         "LogWarning": function (in_message) {
            App.LogMessage("Client", "Warning", in_message);
         },
         "LogInfo": function (in_message) {
            App.LogMessage("Client", "Info", in_message);
         },
      },
      "Network": {
         //special function to get Authorize token or just use ClientPOST
         "Client_AsyncGetAuthoriseTokenAndBootstrapData": Unimplemented,//ClientAuthorise(pass callback, error callback, username, password), or assume all logged in for now? return {"AuthoriseToken", "userId", "localeKey"}
         "Client_POST": Unimplemented,//[create] callback pass, callback error, Authorize token, endpoint, data
         "Client_GET": Unimplemented,//[read] callback pass, callback error, Authorize token, endpoint
         "Client_PUT": Unimplemented,//[update] callback pass, callback error, Authorize token, endpoint, data
         "Client_DELETE": Unimplemented,//[delete] callback pass, callback error, Authorize token, endpoint

         "LogError": function (in_message) {
            App.LogMessage("Network", "Error", in_message);
         },
         "LogWarning": function (in_message) {
            App.LogMessage("Network", "Warning", in_message);
         },
         "LogInfo": function (in_message) {
            App.LogMessage("Network", "Info", in_message);
         },
      },
      "Server": {
         "Network_RequestListener": Unimplemented,//request, response
         // request { "url", "headers", "method", "body"} ("onError"?)
         // response { "setHeader", "writeHead", "write", "end"} ("onError"?)

         "LogError": function (in_message) {
            App.LogMessage("Server", "Error", in_message);
         },
         "LogWarning": function (in_message) {
            App.LogMessage("Server", "Warning", in_message);
         },
         "LogInfo": function (in_message) {
            App.LogMessage("Server", "Info", in_message);
         },
      },
      "Database": {
         //https://redis.io/commands
         "Server_SET": Unimplemented,//key value [EX seconds|PX milliseconds|EXAT timestamp|PXAT milliseconds-timestamp|KEEPTTL] [NX|XX] [GET]
         "Server_GET": Unimplemented,//key
         "Server_MSET": Unimplemented,//key value [key value...]
         "Server_MGET": Unimplemented,//key [key ...]
         "Server_EXISTS": Unimplemented,//key [key ...]
         "Server_DEL": Unimplemented,//key [key ...]
         "Server_INCR": Unimplemented,//key
         "Server_INCRBY": Unimplemented,//key delta 
         "Server_DECR": Unimplemented,//key
         "Server_DECRBY": Unimplemented,//key delta

         //LPUSH, RPUSH, LPOP, RPOP, LRANGE, LTRIM

         "Server_HGET": Unimplemented,//key field
         "Server_HGETALL": Unimplemented,//key
         "Server_HDEL": Unimplemented,//key field [field ...]
         "Server_HEXISTS": Unimplemented,//key field
         //HINCRBY

         "Server_SADD": Unimplemented,//key member [member ...]
         "Server_SMEMBERS": Unimplemented,//key

         //PUBLISH
         //SUBSCRIBE
         //UNSUBSCRIBE

         //...

         //allow preloading data into database
         "Root_SetData": Unimplemented,//data

         "LogError": function (in_message) {
            App.LogMessage("Database", "Error", in_message);
         },
         "LogWarning": function (in_message) {
            App.LogMessage("Database", "Warning", in_message);
         },
         "LogInfo": function (in_message) {
            App.LogMessage("Database", "Info", in_message);
         },
      },

      "Unittest": {
         "LogError": function (in_message) {
            App.LogMessage("Unittest", "Error", in_message);
         },
         "LogWarning": function (in_message) {
            App.LogMessage("Unittest", "Warning", in_message);
         },
         "LogInfo": function (in_message) {
            App.LogMessage("Unittest", "Info", in_message);
         },
      }
   };
})();

