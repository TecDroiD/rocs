#!/bin/bash

LOG_DIR=log
CONFIG=test.config

rocsmq-server $CONFIG >$LOG_DIR/server.log &
sleep 1
rocsmq-luaclient $CONFIG >$LOG_DIR/lua.log &
rocsmq-speech $CONFIG >$LOG_DIR/speech.log &
rocsmq-sphinx $CONFIG >$LOG_DIR/sphinx.log &
sleep 1
rocsmq-cron $CONFIG >$LOG_DIR/cron.log &
