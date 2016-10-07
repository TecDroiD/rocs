-- sensor.*
-- reacts on sensor data
-- version: 	0.1.0
-- date:		2016/08/18

JSON = require "json"
cron = require "../lib/cronhelper"

--[[
 pre-formatted json strings
--]]
movementjson =  "{\"direction\":\"%s\",\"distance\":\"%s\"}"

--[[
 decision table for reactions
--]]
reaction = {
 ["world.obstacle"] =  do_evade(jsontree) , 
 [""] = ignore(jsontree),
}

--[[
	function entry
--]]
function run(order, json)
	log_message("debug", order.." triggered.")
	-- parse json tree
	local jsontree = JSON.decode(json)
	-- do something with the order
	reaction[order](jsontree)
	-- end
	log_message("debug", order.."textorder handled.")
end

function ignore(jsontree)
	log_message(INFO, "Yet n
	o reaction implemented."
end
--[[
	evading to avoid collision
	
--]]
function do_evade(jsontree) 
	-- flee from object to avoid it
	direction = jsontree["direction"]
	distance = jsontree ["distance"]
	send_message("move.flee", string.format(movementjson, direction, distance)
end
