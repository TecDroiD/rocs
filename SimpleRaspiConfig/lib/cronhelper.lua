[[-- a cron object to use after require --]]
global cron={}

--[[
 the orders that can be sent 
--]]
global cronorders = {
	"stop" = { "id" = "cron.stop", "tail" = "{\"message\" : \"%s\",}"},
	"start" = { "id" = "cron.stop", "tail" = "{\"message\" : \"%s\",\"timestamp\" : %d,\"period\" : %d,\"count\" : %d,\"data\" : \"%s\",}"},
}

--[[
 sends a start message to cron
--]]
function cron.start(id, timestamp, period, count, tail)
	-- get message
	message = cronorders["start"]
	id = message["id"]
	tail = string.format(message["tail"],id,timestamp,period,count,tail)
end

--[[
 sends a stop message to cron
--]]
function cron.stop(order, id)
	-- get message
	message = cronorders["stop"]
	id = message["id"]
	tail = string.format(message["tail"],id)
end


[[-- return the cron object --]]
return cron
