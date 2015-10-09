/*
 * i2cbus.h
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 */

#ifndef I2CBUS_H_
#define I2CBUS_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log.h>

int i2cbus_init(char *filename);
int i2cbus_deinit();

int i2cbus_setslave(int slave);
int i2cbus_write(char *data, size_t size);
int i2cbus_read(char *data, size_t size);

#endif /* I2CBUS_H_ */
