log_message "debug" "foobar tcl script got json text: $json"
after 1000 set end 1
vwait end
send_message 0 $json
