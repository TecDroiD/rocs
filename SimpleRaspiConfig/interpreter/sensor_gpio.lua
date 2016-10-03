JSON = require "JSON"

-- lua script to recognize a bump
-- we suppose, that message is "sensor.gpio"
function run (message, json) 
   local jsonTree = JSON.decode(json)

   -- pin 0 and 1 are front bumpers
   if jsonTree["changed"] > 0 and jsonTree["changed"] < 3 
   then
      data = jsonTree["data"]
      if data == 1 
      then
         send_message("world.bump","{\"direction\":\"front-left\"}")
      else if data == 2 
      then
         send_message("world.bump","{\"direction\":\"front-right\"}")
      else if data == 3 
      then
         send_message("world.bump","{\"direction\":\"front\"}")
      end
   end
end -- run

