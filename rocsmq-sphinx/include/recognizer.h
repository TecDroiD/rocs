#ifndef RECOGNIZER_H
#define RECOGNIZER_H

#include "pocketsphinx.h"
#include "err.h"
#include "ad.h"

#include <log.h>
#include <rocsmq.h>
#include <configparser.h>

#include "client_config.h"



char const * recognize();

int sphinx_init(p_clientconfig config);
void sphinx_shutdown();
#endif
