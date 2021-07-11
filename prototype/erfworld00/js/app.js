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
   /*
    document the public interface of each module, plus a hint at what module will be calling the public interface
    */
   return {
      "RegisterOnLoad": RegisterOnLoad,
      "RegisterBeforeUnload": RegisterBeforeUnload,
      "Unimplemented": Unimplemented,

      "Globals": {
         mockNetworkLatencyMillisecond: 100,
         mockNetworkLatencyMillisecondPlusMinus: 50,
         mockDatabaseLatencyMillisecond: 100,
         mockDatabaseLatencyMillisecondPlusMinus: 50,
         //mockNetworkErrorRate?
      },
      "View": {
         // if you want a loading screen, use ClientSetScreen("loading", "loading0", null)?
         //template name, screen/dialog name, data
         "Client_SetScreen": Unimplemented,
         //template name, screen/dialog name, data
         "Client_PushDialog": Unimplemented,
         "Client_PopDialog": Unimplemented,
         "Client_ClearAllDialog": Unimplemented,
         "Client_GetDefaultTemplateName": Unimplemented,
         "Client_OnGetUserData": Unimplemented,
         //message
         "ConsoleError": Unimplemented,
         //message
         "ConsoleDebug": Unimplemented,
         "SetDefaultTemplateName": Unimplemented,
         //templateName, factory
         "AddTemplate": Unimplemented
      },
      "Client": {
         //// have one registed error callback (that people can forget) or oblige every async function to have an error callback   
         ////function callback(error message)
         //"ViewRegisterErrorCallback": Unimplemented,
         ////callback pass, callback error, locale key, modual name, key, 
         //"ViewAsyncGetLocalisedKey": Unimplemented,

         //screen name, element name, group data, verb, subject, x, y
         "View_OnClick": Unimplemented,
         //screen name, element name, normalised value, mouse finished dragging?
         "View_OnDrag": Unimplemented,
         //callback enabled, callback error, screen/dialog name, id
         "View_RegisterAsyncEnabled": Unimplemented,
         //"View_RemoveAsyncEnabled": Unimplemented,
         // locale, progress bar, slider?
         //callback text, callback error, screen/dialog name, id
         "View_RegisterAsyncText": Unimplemented,
         //"View_RemoveAsyncText": Unimplemented,
         //name, factory
         "RegisterState": Unimplemented,
         //name
         "SetState": Unimplemented,
         //authoriseToken
         "SetAuthoriseToken": Unimplemented,
         //callback, module, key, data
         "AsyncGetLocalisedText": Unimplemented,
      },
      "Network": {
         //special function to get Authorize token or just use ClientPOST
         //ClientAuthorise(pass callback, error callback, username, password), or assume all logged in for now? return {"AuthoriseToken", "userId", "localeKey"}
         "Client_GetUserDataAuthoriseToken": Unimplemented,
         //[create] callback pass, callback error, Authorize token, endpoint, data
         "Client_POST": Unimplemented,
         //[read] callback pass, callback error, Authorize token, endpoint
         "Client_GET": Unimplemented,
         //[update] callback pass, callback error, Authorize token, endpoint, data
         "Client_PUT": Unimplemented,
         //[delete] callback pass, callback error, Authorize token, endpoint
         "Client_DELETE": Unimplemented
      },
      "Server": {
         //request, response
         // request { "url", "headers", "method", "body"} ("onError"?)
         // response { "setHeader", "writeHead", "write", "end"} ("onError"?)
         "Network_RequestListener": Unimplemented
      },
      "Database": {
         //https://redis.io/commands
         //key value [EX seconds|PX milliseconds|EXAT timestamp|PXAT milliseconds-timestamp|KEEPTTL] [NX|XX] [GET]
         "Server_SET": Unimplemented,
         //key
         "Server_GET": Unimplemented,
         //key value [key value...]
         "Server_MSET": Unimplemented,
         //key [key ...]
         "Server_MGET": Unimplemented,
         //key [key ...]
         "Server_EXISTS": Unimplemented,
         //key [key ...]
         "Server_DEL": Unimplemented,
         //key
         "Server_INCR": Unimplemented,
         //key delta 
         "Server_INCRBY": Unimplemented,
         //key
         "Server_DECR": Unimplemented,
         //key delta
         "Server_DECRBY": Unimplemented,

         //LPUSH, RPUSH, LPOP, RPOP, LRANGE, LTRIM

         //key field
         "Server_HGET": Unimplemented,
         //key
         "Server_HGETALL": Unimplemented,
         //key field [field ...]
         "Server_HDEL": Unimplemented,
         //key field
         "Server_HEXISTS": Unimplemented,
         //HINCRBY

         //key member [member ...]
         "Server_SADD": Unimplemented,
         //key
         "Server_SMEMBERS": Unimplemented,

         //PUBLISH
         //SUBSCRIBE
         //UNSUBSCRIBE

         //...

         //allow preloading data into database
         "SetData": Unimplemented
      },
   };
})();

