-- input.textorder
-- reacts on textual orders which may come from a console as well as from spoken words
-- version: 	0.1.0
-- date:		2016/07/11

JSON = require "json"

function speaktext(text)
   log_message("debug", "order is " .. text) 
   send_message("speech.speak", "{\"text\":\"got order " .. text .."\",}")
end

function run(json)
	log_message("debug", "textorder triggered.")
	-- parse json tree
	local luatree = JSON.decode(json)
	local text = luatree["text"]
	-- do something with the order
	speaktext(text)
	
	log_message("debug", "textorder handled.")
end
