/*
 * i2cbus.c
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 * 
 * Servo value between 400 and 1400 is good
 * so there are theoretically 1000 steps.
 * 
 */

#include "gpio.h"

static p_clientconfig config;

static int GPIOExport(int pin)
{
	char buffer[3];
	ssize_t bytes_written;
	int fd;
 
	fd = open(GPIO_EXPORT, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open export for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, 3, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}
 
static int GPIOUnexport(int pin)
{
	char buffer[3];
	ssize_t bytes_written;
	int fd;
 
	fd = open(GPIO_UNEXPORT, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open unexport for writing!\n");
		return(-1);
	}
 
	bytes_written = snprintf(buffer, 3, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return(0);
}
 
static int GPIODirection(int pin, int dir)
{
	static const char s_directions_str[]  = "in\0out";
 
	char path[40];
	int fd;
 
	snprintf(path, 40, GPIO_SYS GPIO_DIRECTION, pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio direction for writing!\n");
		return(-1);
	}
 
	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		return(-1);
	}
 
	close(fd);
	return(0);
}
 
int gpio_read(int pin)
{
	char path[30];
	char value_str[3];
	int fd;
 
	snprintf(path, 30, GPIO_SYS GPIO_VALUE, pin);
	fd = open(path, O_RDONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for reading!\n");
		return(-1);
	}
 
	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return(-1);
	}
 
	close(fd);
 
	return(atoi(value_str));
}
 
int gpio_write(int pin, int value)
{
	static const char s_values_str[] = "01";
 
	char path[30];
	int fd;
 
	snprintf(path, 30, GPIO_SYS GPIO_VALUE, pin);
	fd = open(path, O_WRONLY);
	if (-1 == fd) {
		fprintf(stderr, "Failed to open gpio value for writing!\n");
		return(-1);
	}
 
	if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
		fprintf(stderr, "Failed to write value!\n");
		return(-1);
	}
 
	close(fd);
	return(0);
}

/**
 * init pin
 */ 
int init_pin(int pin, int inout) {
	/*
	 * enable io port
	 */ 

	log_message(DEBUG, "Trying to enable Pin %d.",pin);
	if (-1 == GPIOExport(pin)) {
		log_message(ERROR, "could not enable GPIO-%d",pin);
		return(1);
	}

	/*
	 * Set GPIO directions
	 */
	log_message(DEBUG, "Setting direction %d for Pin %d.",inout, pin);
	if (-1 == GPIODirection(pin, inout)) {
		log_message(ERROR, "could set direction pin on %x", pin);
		return(2);
	}	
}	

/**
 * init gpios
 */ 
int gpio_init(p_clientconfig conf) {
	p_pin pin;
	int i;

	// set local config
	config = conf;
	
	// initialize pin
	for (i = 0; i < config->num_pins; i++) {
		pin = &(config->pins[i]);
		init_pin(pin->number, pin->direction);
	}
	
	return 0;
}


/**
 * deinit gpios 
 */ 
int gpio_deinit() {
	p_pin pin;
	int i;
	

	// initialize pin
	for (i = 0; i < config->num_pins; i++) {
		pin = &(config->pins[i]);
		if(-1 == GPIOUnexport(pin->number))  {
			log_message(ERROR, "could not disable GPIO %s", pin->mapname);
		}
	}

	
	return 0;
}

