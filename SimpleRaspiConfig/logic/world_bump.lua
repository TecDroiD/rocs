JSON = require "JSON"

-- lua script to react on a bump
-- we suppose, that message is "input.bump"
function run (message, json) 
   local jsonTree = JSON.decode(json)
   
   if jsonTree["direction"] == "front"
   then
         send_message("move.surround","{\"direction\":\"left\"}")
   else if jsonTree["direction"] == "front-left"
   then
         send_message("move.surround","{\"direction\":\"right\"}")
   else if jsonTree["direction"] == "front-right"
   then
         send_message("move.surround","{\"direction\":\"left\"}")
   else

end -- run

