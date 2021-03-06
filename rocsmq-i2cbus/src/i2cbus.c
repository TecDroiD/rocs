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

#include "i2cbus.h"

int i2cfile;

int i2cbus_init(char *filename) {
	log_message(INFO, "Opening i2c device %s", filename);
	if ((i2cfile = open(filename,O_RDWR)) < 0) {
		log_message(ERROR, "Could not open device. Errno: %d", errno);
	}
	return i2cfile;
}

int i2cbus_deinit() {
	log_message(INFO, "Closing i2c device.");
	return close(i2cfile);
}

int i2cbus_setslave(int slave) {
	log_message(DEBUG, "Setting I2C slave %d", slave);

	if (ioctl(i2cfile,I2C_SLAVE,slave) < 0) {
		log_message(ERROR,"Failed to acquire bus access and/or talk to slave. Errno: %d", errno);
		return -1;
	}
	return 0;
}

int i2cbus_write(char reg, char *data, size_t size) {
	int ret;
	int i;
	log_message(DEBUG, "Writing %d bytes of data to I2C slave.", size);
	for (i = 0; i < size; i++) {
		ret = i2c_smbus_write_byte_data(i2cfile,reg, data[i]);
		if ( ret == -1 ) {
			log_message(ERROR,"Failed to write to the i2c bus. Errno: %d", errno);
			return -1;
		}
	}
    return ret;
}

int i2cbus_read(char reg, char *data, size_t size) {
	int ret;
	int i;
	log_message(DEBUG, "Reading %d bytes of data from I2C slave.", size);
	for (i = 0; i < size; i++) {
		ret = i2c_smbus_read_byte_data(i2cfile, reg);
		if (-1 == ret) {
			log_message(ERROR,"Failed to read from the i2c bus. Errno: %d", errno);
			return -1;
		}
		data[i] = (char)ret;
	}
	return ret;
}
