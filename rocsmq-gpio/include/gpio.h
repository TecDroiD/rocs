/*
 * GPIO.h
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 */

#ifndef GPIO_H_
#define GPIO_H_

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <log.h>

#include <rocsmq.h>

#include "customconfig.h"

#define JSON_KEY_READ	"get"
#define JSON_KEY_WRITE	"set"


#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1

#define GPIO_SYS "/sys/class/gpio/gpio%d/"
#define GPIO_EXPORT "/sys/class/gpio/export"
#define GPIO_UNEXPORT "/sys/class/gpio/unexport"
#define GPIO_DIRECTION "direction"
#define GPIO_VALUE "value"

int gpio_init(p_clientconfig config);
int gpio_deinit();

int gpio_read(int pin);
int gpio_write(int pin, int value);
#endif /* GPIO_H_ */
