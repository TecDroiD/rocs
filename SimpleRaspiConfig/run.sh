#!/bin/bash

LOG_DIR=/var/log/rocs

rocsmq-server src.config >$LOG_DIR/server.log &
sleep 1
rocsmq-luaclient -n interpreter src.config >$LOG_DIR/interpreter.log &
rocsmq-luaclient -n logic src.config >$LOG_DIR/logic.log &
rocsmq-luaclient -n planner src.config >$LOG_DIR/planner.log &
rocsmq-gpio src.config >$LOG_DIR/gpio.log &
sleep 1
rocsmq-cron src.config >$LOG_DIR/cron.log &
