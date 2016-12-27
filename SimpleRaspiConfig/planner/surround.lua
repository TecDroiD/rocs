
CRON = require "cronhelper"
JSON = require "json"
MOTOR = require "gpio_motor"

-- lua planner to surround without responding hardware
function run(message, json) 
   local jsonTree = JSON.decode(json)

   action = retrieve("action")
   direction = retrieve("direction")
 
   if message == "move.surround" 
   then
      log_message("debug","start surrounding")

      -- start movement process
      persist("action", message)
      persist("direction", jsonTree["direction"])

      -- move backward for 2 s
      MOTOR.set(-1,-1)
      -- set cron to stop motors
      CRON.start("move.surround.turn",20,1,1, "{}")

   elseif message == "move.surround.turn"
   then
      log_message("debug","turning"..direction)
      -- now turn
      if direction == "left"
      then
         -- turn left: move left motor backward and right motor forward
         MOTOR.set(-1,1)
      else
         -- turn right: move left motor forward and right motor backward
         MOTOR.set(1,-1)
      end
      -- wait 2 seconds
      CRON.start("move.surround.stop",20,1,1, "{}")
  elseif message == "move.surround.stop"
  then
	 log_message("debug","stop")
         -- stop motors
         MOTOR.set(0,0)
         -- set idle
         persist("action","idle")
         persist("direction", "")
   end
end -- function
