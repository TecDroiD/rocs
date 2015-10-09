/*
 * canbus.h
 *
 *  Created on: 06.10.2015
 *      Author: tecdroid
 */

#ifndef CANBUS_H_
#define CANBUS_H_

#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <log.h>

#include "can4linux.h"


int can_init(char *candev, int baud);
void can_uninit(void);

int can_recv(canmsg_t * msg);
int can_send(canmsg_t * msg);

#include "can4linux.h"

#endif /* CANBUS_H_ */
