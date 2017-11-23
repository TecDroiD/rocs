#include "canport.h"
int soc;

int can_open(const char *port , int16_t baud) {
    struct ifreq ifr;
    struct sockaddr_can addr;

    /* open socket */
    soc = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if(soc < 0)
    {
        return (-1);
    }

    addr.can_family = AF_CAN;
    strcpy(ifr.ifr_name, port);

    if (ioctl(soc, SIOCGIFINDEX, &ifr) < 0)
    {

        return (-1);
    }

    addr.can_ifindex = ifr.ifr_ifindex;

    fcntl(soc, F_SETFL, O_NONBLOCK);

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {

        return (-1);
    }

    return 0;
}

int can_send(p_frame frame) {
    int retval;
   retval = write(soc, frame, sizeof(struct can_frame));
    if (retval != sizeof(struct can_frame))
    {
        return (-1);
    }
    else
    {
        return (0);
    }
}

/* this is just an example, run in a thread */
int can_read( p_frame frame) {
    int recvbytes = 0;

	struct timeval timeout = {1, 0};
	fd_set readSet;
	FD_ZERO(&readSet);
	FD_SET(soc, &readSet);

	if (select((soc + 1), &readSet, NULL, NULL, &timeout) >= 0)
	{
		if (FD_ISSET(soc, &readSet))
		{
			recvbytes = read(soc, frame, sizeof(struct can_frame));
			if(recvbytes) {
				return 1;
			}
		}
	}
	return 0;
}

int can_close() {
    close(soc);
    return 0;
}

