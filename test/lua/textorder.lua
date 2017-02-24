-- input.textorder
-- reacts on textual orders which may come from a console as well as from spoken words
-- version: 	0.1.0
-- date:		2016/07/11

JSON = require "json"
cron = require "../lib/cronhelper"

function speaktext(text)
   log_message("debug", "order is " .. text) 
   --cron.start("speech.speak", "{\"text\":\"got order " .. retrieve("lastorder") .."\",}",50,10,2)
   send_message("speech.speak", "{\"text\":\"got order " .. text .."\",}")
   persist("lastorder",text);
end

function run(order,json)
	log_message("debug", "textorder triggered.")
	-- parse json tree
	local luatree = JSON.decode(json)
	local text = luatree["text"]
	-- do something with the order
	speaktext(text)
	
	log_message("debug", "textorder handled.")
end
