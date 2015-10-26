/*
 * messages.h
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_


#define MESSAGE_ID_ACTOR			0x1000 /* data for the actor clients */
#define MESSAGE_ID_SENSOR			0x2000 /* data from sensors */
#define MESSAGE_ID_INFRASTRUCTURE	0x4000 /* information for infrastructure */
#define MESSAGE_ID_SYSTEM			0x8000 /* control information for system */
#define MESSAGE_ID_QUEUE			0xf000 /* all messages */

#define MESSAGE_ID_HIGHPRIO			0x0001
#define MESSAGE_ID_DELQUEUE			0x0002

#define MESSAGE_MASK_MAIN			0xf000
#define MESSAGE_MASK_SUB			0x0fff

#define MESSAGE_ID_MOTORPOS			0x0010
#define MESSAGE_ID_AQUIRE			0x0020
#define MESSAGE_ID_SHUTDOWN			0x0001

/**
 * json order set for system
 */ 
#define ORDER_SHUTDOWN				"shutdown" /* system shutdown message */
#endif /* MESSAGES_H_ */
