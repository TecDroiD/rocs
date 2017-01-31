#ifndef _TTY_H
#define _TTY_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "customconfig.h"


int tty_init(p_clientconfig clientconfig);
int tty_hasdata();
int tty_read (char *data, int len);
int tty_write (char *data, int len);
int tty_close();

#endif
