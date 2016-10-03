global gpio_motor = {}

global sendstr="{\"mask\":240,\"values\":%d}"

--[[
 -- set motors
--]]
function gpio_motor.set(motora, motorb) 
 -- set motor values
	val = 0
	-- test motor a
	if (motora > 0)
	then
		val += 16
	else if (motora < 0) 
		val += 32
	end

	-- test motor b
	if (motorb > 0)
	then
		val += 64
	else if (motorb < 0) 
		val += 128
	end
    
	sendstr = string.format(sendstr, val)
        send_message("gpio.set", sendstr)
end

--[[
 -- stop motors
--]]
function gpio_motor.stop() {
	sendstr = string.format(sendstr, 0)
        send_message("gpio.set", sendstr)
}	
return gpio_motor
