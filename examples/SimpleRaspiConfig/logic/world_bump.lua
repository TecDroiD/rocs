JSON = require "json"

-- lua script to react on a bump
-- we suppose, that message is "input.bump"
function run (message, json) 
   local jsonTree = JSON.decode(json)
   
   if jsonTree["direction"] == "front"
   then
         send_message("move.surround","{\"direction\":\"left\"}")
   elseif jsonTree["direction"] == "front-left"
   then
         send_message("move.surround","{\"direction\":\"right\"}")
   elseif jsonTree["direction"] == "front-right"
   then
         send_message("move.surround","{\"direction\":\"left\"}")
   end 

end -- run

