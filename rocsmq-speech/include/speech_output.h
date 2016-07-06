#ifndef SPEECH_OUTPUT_H
#define SPEECH_OUTPUT_H

#include <log.h>

#include <espeak/speak_lib.h>

#include "client_config.h"

int SynthCallback(short *wav, int numsamples, espeak_EVENT *events);

int init_espeak (p_clientconfig config);
void speak(char *text);
void shutup();
void exit_espeak();

#endif
