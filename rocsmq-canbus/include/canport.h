#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

typedef struct can_frame t_frame, *p_frame;

int can_open(const char *port, int16_t baud);
int can_send(p_frame frame);
int can_read(p_frame frame);
int can_close();
