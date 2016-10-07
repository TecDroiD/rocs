--[[ 
  a global motor object to require
--]]
local gpio_motor = {}

local sendstr = "{\"pins\" : [{\"name\" : \"motor_a_left\", \"value\" : %d},{\"name\" : \"motor_a_right\", \"value\" : %d}{\"name\" : \"motor_b_left\", \"value\" : %d}{\"name\" : \"motor_b_right\", \"value\" : %d}]}"

--[[
 -- set motors
--]]
function gpio_motor.set(motora, motorb) 
	 -- set motor values
	vala = {}
		vala[0] = 0
		vala[1] = 0
	valb = {}
		valb[0] = 0
		valb[1] = 0

	-- test motor a
	if (motora > 0)
	then
		vala[0] = 1
		vala[1] = 0
	else if (motora < 0) 
	then	
		vala[0] = 0
		vala[1] = 1
	end

	-- test motor b
	if (motorb > 0)
	then
		valb[0] = 1
		valb[1] = 0
	else if (motorb < 0) 
	then
		valb[0] = 0
		valb[1] = 1
	end

	-- send data    
	sendstr = string.format(sendstr, vala[0], vala[1], valb[0], valb[1])
        send_message("gpio.set", sendstr)
end

--[[
 -- stop motors
--]]
function gpio_motor.stop() 
	sendstr = string.format(sendstr, 0, 0, 0, 0)
        send_message("gpio.set", sendstr)
end
	
return gpio_motor
