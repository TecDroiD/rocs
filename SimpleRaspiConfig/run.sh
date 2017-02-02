#!/bin/bash
rocsmq-server src.config &
sleep 2
rocsmq-luaclient -n interpreter src.config &
rocsmq-luaclient -n logic src.config &
rocsmq-luaclient -n planner src.config &
rocsmq-tty src.config &
sleep 1
#rocsmq-gpio src.debug.config &
sleep 1
# rocsmq-cron src.config
