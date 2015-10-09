/*
 * pca9685.c
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 */

#include "pca9685.h"

void pca_init(p_pca_device device){
	char buf[4];
	// set device id
	i2cbus_setslave(device->addr);

	// set auto increment mode
	buf[0] = ADDR_MODE1;
	buf[1] = MODE1_AUTOINCREMENT;
	i2cbus_write(buf,2);

	// set prescale for 50 Hz
	buf[0] = ADDR_PRE_SCALE;
	buf[1] = PRESCALE(50);
	i2cbus_write(buf,2);

}

void pca_port(p_pca_device device, Uint8 portno, int ppm) {
	t_pca_port port;

	// set device id
	i2cbus_setslave(device->addr);

	// set port data
	port.port_addr = ADDR_PORT(portno);
	port.on_time = 0;
	port.off_time = PCA_OFFTIME(ppm);
	i2cbus_write(&port, sizeof(t_pca_port));

}

