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
         mockDatabaseLatencyMillisecondPlusMinus: 50,

         databaseGuestTimeoutSeconds: 60 * 60 * 24,
         databaseSessionTimeoutSeconds: 60 * 60 * 24,
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
         "OnButtonClick" //in_verb, in_object
         "OnElementInput" //elementName
         "GetValueBool"
         "GetValueInt"
         "GetValueString"
         // named structs or generic? GetValueDataButton, GetValueArrayDataButton or just GetValueObject
         "GetValueObject"
         "GetValueArray"
         "GetValueLocaleKey" //in_localeKey, in_module, in_dataKeyArray
         "MakeGeneratorBool" //in_key, in_callback
         "MakeGeneratorInt"
         "MakeGeneratorString"
         // named structs or generic?
         "MakeGeneratorObject"
         "MakeGeneratorArray"
         "MakeGeneratorLocaleKey"//in_localeKey, in_module, in_dataKeyArray, callback
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

         "Client_MakeGeneratorBool": Unimplemented,
         "Client_MakeGeneratorLocaleKey": Unimplemented,
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
         "Root_GetServerVersion": Unimplemented,

         //register a middleware for use
         "Root_Use": Unimplemented, //(path, method, callback(request,response,next))  
         "Root_MakeMiddlewareStack": Unimplemented,
         //allow easiy way to share state between mutiple src files without "include", 
         "Root_SetKeyValue": Unimplemented,
         "Root_GetKeyValue": Unimplemented,

         "Root_MakeUUID": Unimplemented,
         "Root_MakeETag": Unimplemented,
         "Root_GetApiETag": Unimplemented,

         "Root_MakeResponseOk": Unimplemented, //request, response, json
         "Root_MakeResponseCreated": Unimplemented, //request, response, json
         "Root_MakeErrorResponseNotModified": Unimplemented, //request, response, error log message
         "Root_MakeErrorResponseBadRequest": Unimplemented, //request, response, error log message
         "Root_MakeErrorResponseUnauthorized": Unimplemented, //request, response, error log message
         "Root_MakeErrorResponseForbidden": Unimplemented, //request, response, error log message
         "Root_MakeErrorResponseNotFound": Unimplemented, //request, response, error log message
         "Root_MakeErrorResponsePreconditionFailed": Unimplemented, //request, response, error log message
         "Root_MakeErrorResponseInternalServerError": Unimplemented, //request, response, error log message

         "Root_AddLinkData": Unimplemented, //path, method, name
         "Root_AppendLinkDataToResponse": Unimplemented, //in_responseJson, in_path, in_method

         // request { "url", "headers", "method", "body"}
         // response { "status", "json", "end"}
         "Network_RequestListener": Unimplemented,//request, response

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

         "Server_ZRANGE": Unimplemented,//key min max
         "Server_ZADD": Unimplemented,//key [NX|XX] [GT|LT] [CH] [INCR] score member [score member ...]
         "Server_ZREM": Unimplemented,//key member [member ...]

         //PUBLISH
         //SUBSCRIBE
         //UNSUBSCRIBE

         //...

         //allow mock to preload data into database
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

