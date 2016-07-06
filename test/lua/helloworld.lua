-- Testfunktion, sendet eine Nachricht an irgendwen..


function run(json) 
    print ("helloworld triggered")
	log_message("debug", json)
	send_message("speech.speak", "{'text':'hello world',}")
	log_message("info", "sent message to speech.")
end
