JSON = require "json"

-- lua script to recognize a bump
-- we suppose, that message is "sensor.gpio"
function run (message, json) 
	local jsonTree = JSON.decode(json)

	local pins = jsonTree["pins"]
	data = 0
	
	log_message("debug", "checking buttons")

	-- iterate through pins
        for i,pin in ipairs(pins) do
	    if pin["name"] == "button_left" and pin["value"] == 1 then
		log_message("debug", "left button hit")
               data = data + 1
            elseif pin["name"] == "button_right" and pin["value"] == 1 then 
		log_message("debug", "right button hit")
               data = data + 2
            end
        end

	-- check where bumped
	if data == 1 then
	 send_message("world.bump","{\"direction\":\"front-left\"}")
	elseif data == 2 then
	 send_message("world.bump","{\"direction\":\"front-right\"}")
	elseif data == 3 then
	 send_message("world.bump","{\"direction\":\"front\"}")
	end
end -- run

