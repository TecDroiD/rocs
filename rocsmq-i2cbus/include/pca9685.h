/*
 * pca9685.h
 *
 *  Created on: 07.10.2015
 *      Author: tecdroid
 */

#ifndef PCA9685_H_
#define PCA9685_H_

#include <SDL/SDL.h>
#include "i2cbus.h"

#define PCA_CLOCK 25000000
#define PRESCALE(hz) (int)(PCA_CLOCK/(4096 * hz)-1)

#define PCA_OFFTIME(ppm)(int)(((float)4096/1000.0) * ppm)

#define ADDR_MODE1		0x00
#define ADDR_MODE2		0x01
#define ADDR_SUBADDR	0x02
#define ADDR_ALLCALL	0x05
#define ADDR_PORTS		0x06
#define ADDR_PRE_SCALE	0xfe
#define ADDR_TESTMODE	0xff

#define MODE1_AUTOINCREMENT 0x20


#define PORT_SIZE		0x04
#define ADDR_PORT(no) ADDR_PORTS+PORT_SIZE*no

/**
 * pca device data
 */
typedef struct s_pca_device {
	Uint16 	addr;
	Uint8 	mode[2];
	Uint16 	frequency;
} t_pca_device, *p_pca_device;

/**
 * pca_port_data
 */
typedef struct s_pca_port {
	Uint8	port_addr;
	Uint16 on_time;
	Uint16 off_time;
} t_pca_port, *p_pca_port;

/**
 * initialize pca
 */
void pca_init(p_pca_device device);
void pca_port(p_pca_device device, Uint8 portno, int ppm);
#endif /* PCA9685_H_ */
