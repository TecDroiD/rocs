CRON = require "cronhelper"
MOTOR = require "gpio_motor"

-- standard run will run all 10 seconds
function run(message,json) 
   action = retrieve("action")
 
   if action == "" 
   then
	action = "idle"
	persist("action", "idle")
   end

   -- if nothing else has to be done
   if action == "idle" 
   then
     -- move both motors forward
     MOTOR.set(1,1)
   end      
   -- ask again in 10 seconds
   CRON.start("idle", 100, 1, "{}")
end -- run

