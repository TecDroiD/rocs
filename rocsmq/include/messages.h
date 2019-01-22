/*
 * messages.h
 * 
 * 
 * deprecated: TODO: remove
 *
 *  Created on: 29.09.2015
 *      Author: tecdroid
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

#define MESSAGE_ID_ACTOR			"actor" /* data for the actor clients */
#define MESSAGE_ID_SENSOR			"sensor" /* data from sensors */
#define MESSAGE_ID_LOGIC			"logic" /* information for logic units */
#define MESSAGE_ID_INFRASTRUCTURE	"infra" /* information for infrastructure */
#define MESSAGE_ID_SYSTEM			"system" /* control information for system */
#define MESSAGE_ID_QUEUE			"queue" /* all messages */

#define MESSAGE_CLIENT_CAN			"can"
#define MESSAGE_FILTER_CAN			"can*"
#define MESSAGE_CLIENT_I2C			"i2c"
#define MESSAGE_FILTER_I2C			"i2c*"
#define MESSAGE_CLIENT_RS232		"rs232"
#define MESSAGE_FILTER_RS232		"rs232*"
#define MESSAGE_CLIENT_GPIO			"gpio"
#define MESSAGE_FILTER_GPIO			"gpio*"
#define MESSAGE_CLIENT_CRON			"cron" 
#define MESSAGE_FILTER_CRON			"cron*"
#define MESSAGE_CLIENT_SYSTEM		"system" /* control information for all system clients */
#define MESSAGE_FILTER_SYSTEM		"system*"


#define MESSAGE_ID_MOTORPOS			"motor.pos"
#define MESSAGE_ID_AQUIRE			"aqu"
#define MESSAGE_ID_SHUTDOWN			"shutdown"

#define MESSAGE_ID_HIGHPRIO			"!!"
#define MESSAGE_ID_DELQUEUE			"delqueue"

/**
 * json order set for system
 */ 
#define ORDER_SHUTDOWN				"shutdown" /* system shutdown message */


#define STR1(z) #z
#define DOTSTR	STR1('.')
#define CREATE_CLIENTORDER(client,order) client DOTSTR order
#endif /* MESSAGES_H_ */
