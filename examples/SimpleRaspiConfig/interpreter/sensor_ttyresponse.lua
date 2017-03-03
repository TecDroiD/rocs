JSON = require "json"

-- lua script to recognize a bump
-- we suppose, that message is "sensor.gpio"
function run (message, json) 
	local jsonTree = JSON.decode(json)
	recv = jsonTree["input"]
	send_message("tty.write","{\"value\": \""..recv.."\",}")
end -- run

