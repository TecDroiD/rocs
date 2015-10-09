/*
 * canbus.c
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 */

#include "canbus.h"

int canfp;

/**
 * initialize can device file
 */
int can_init(char *candev, int baud) {

	Config_par_t cfg;
	volatile Command_par_t cmd;

	// open file
	canfp = open(candev, O_RDWR);
	// if file could not be opened, exit
	if (canfp) {
		return -1;
	}

	// stop can
	cmd.cmd		= CMD_STOP;
	ioctl(canfp, CAN_IOCTL_COMMAND, &cmd);

	// configure baudrate
	cfg.target 	= CONF_TIMING;
	cfg.val1 	= baud;
	ioctl(canfp, CAN_IOCTL_CONFIG, &cfg);

	// restart can
	cmd.cmd		= CMD_START;
	ioctl(canfp, CAN_IOCTL_COMMAND, &cmd);

	return 0;
}

void can_uninit() {
	close(canfp);
}

int can_send(canmsg_t *msg) {
	int ret;
	// write message to can bus
	ret = write(canfp, msg, 1);
	if (ret == -1) {
		log_message(ERROR, "can message transmit failed, message id:%ld", msg->id);
	} else if (ret == 0) {
		log_message(ERROR,"can message transmit timed out. message id:%ld", msg->id);
	}
	return ret;
}

int can_recv(canmsg_t *msg) {
	int ret;

	// read a message
	ret = read(canfp, (void*)msg, 1);

	return ret;
}


