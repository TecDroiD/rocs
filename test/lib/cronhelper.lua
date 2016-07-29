cron={}

--[[
 the orders that can be sent 
--]]
cronorders = {
	["stop"] = { ["id"] = "cron.stop", ["tail"] = "{\"message\" : \"%s\",}"},
	["start"] = { ["id"] = "cron.add", ["tail"] = "{\"message\" : \"%s\",\"timestamp\" : %d,\"period\" : %d,\"count\" : %d,\"data\" : \"%s\",}"},
}

--[[
 sends a start message to cron
--]]
function cron.start(id, data, timestamp, period, count)
	print "here i am"
	-- get message
	mesg = cronorders["start"]
	message = mesg["id"]
	tail = string.format(message["tail"],message,timestamp,period,count,data)
	log_message("debug","message: "..message.." with data "..data);
	send_message(id,tail)
end

--[[
 sends a stop message to cron
--]]
function cron.stop(id)
	-- get message
	message = cronorders["stop"]
	id = message["id"]
	tail = string.format(message["tail"],id)
end


--[[ return the cron object 
--]]
return cron
