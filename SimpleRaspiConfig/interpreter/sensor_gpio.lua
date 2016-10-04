JSON = require "JSON"

-- lua script to recognize a bump
-- we suppose, that message is "sensor.gpio"
function run (message, json) 
	local jsonTree = JSON.decode(json)

	local pins = jsonTree[pins]
	data = 0

	-- iterate through pins
        for i,pin in ipairs(pins) do
	    if pin[name] == "button_left" then
		log_message(DEBUG, "left button hit")
               data = data + 1
            else if pin[name] == "button_right" then 
		log_message(DEBUG, "right button hit")
               data = data + 2
            end
        end

	-- check where bumped
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
end -- run

