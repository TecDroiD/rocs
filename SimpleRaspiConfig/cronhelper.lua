--[[ a cron object to use after require --]]
local cron={}

--[[
 the orders that can be sent 
--]]
local cronorders = {
	["stop"] = { ["id"] = "cron.stop", ["tail"] = "{\"message\" : \"%s\",}"},
	["start"] = { ["id"] = "cron.add", ["tail"] = "{\"message\" : \"%s\",\"timestamp\" : %d,\"period\" : %d,\"count\" : %d,\"data\" : \"%s\",}"},
}

--[[
 sends a start message to cron
--]]
function cron.start(id, timestamp, period, count, tail)
	-- get message
	message = cronorders["start"]
	mid = message["id"]
	mtail = string.format(message["tail"],id,timestamp,period,count,oldtail)
	send_message(mid, mtail)
end

--[[
 sends a stop message to cron
--]]
function cron.stop(order, id)
	-- get message
	message = cronorders["stop"]
	mid = message["id"]
	mtail = string.format(message["tail"],id)
	send_message(mid, mtail)
end


--[[ return the cron object --]]
return cron
