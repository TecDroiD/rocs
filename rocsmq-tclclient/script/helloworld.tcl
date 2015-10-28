package require json
package require json::write

proc dict2json {dictionary} {
    dict for {key value} $dictionary {
        if {[string match {\[*\]} $value]} {
            lappend Result "\"$key\":$value"
        } elseif {![catch {dict size $value}]} {
            lappend Result "\"$key\":\"[dict2json $value]\""
        } else {
            lappend Result "\"$key\":\"$value\""
        }
    }
    return "\{[join $Result ",\n"]\}"
}

set jsondata [json::json2dict $json]	
puts $jsondata
set newjson [dict2json ${jsondata}]

log_message "debug" "this is a debug message"
send_message 1 $newjson
