#!/bin/bash
rocsmq-server src.config &
rocsmq-luaclient -n interpreter src.config &
rocsmq-luaclient -n logic src.config &
rocsmq-luaclient -n planner src.config &
rocsmq-gpio src.config &
rocsmq-cron src.config
