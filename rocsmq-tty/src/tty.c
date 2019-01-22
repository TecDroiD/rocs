#include "tty.h"

int file;
char recvdata[255];
int len;

/* 
 * initialize nonblocking canonical read from tty
 */

int tty_init(p_clientconfig clientconfig) {
	file = 0;
	len = 0;
	int baudrate = 0;
	
	struct termios tio;
	tio.c_cc[VTIME]=5;

	// open tty
	file=open(clientconfig->device, O_RDWR | O_NONBLOCK);   
	if (file < 0) {
		log_message(ERROR,"Could not open tty %s",clientconfig->device);
		return -1;
	}
	// set baudrate
	switch (clientconfig->baudrate) {
		case 19200:
			baudrate = B19200;
			break;
		case 57600:	
			baudrate = B57600;
			break;
		case 115200:
			baudrate = B115200;
			break;
		case 9600:
		default:
			baudrate = B9600;
			break;
	}
	   
	cfsetospeed(&tio,baudrate);         // output baud rate
	cfsetispeed(&tio,baudrate);			// input baud rate


	// set terminal flags
	memset(&tio,0,sizeof(tio));
	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_lflag= ICANON;
	tio.c_cc[VMIN]=1;
	tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
	

	tcsetattr(file,TCSANOW,&tio);
	
	memset(recvdata,0,255);
	return 0;
}

int tty_hasdata() {	
	len = read(file,recvdata, 255);
	return 1 < len;
}


int tty_read ( char *data, int max) {
	if (max < len) {
		len = max;
	}
	strncpy (data, recvdata, len );
	memset(recvdata,0,255);
	
	return len;
}

int tty_write ( char *data, int max) {
	write(file, data,max);
	
	return 0;
}


int tty_close() {
	close(file);
	return 0;
}
