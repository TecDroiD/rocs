#include "recognizer.h"

#define MODELDIR 
static ad_rec_t *ad;
static ps_decoder_t *ps;


/**
 * initialize sphinx
 */ 
int sphinx_init(p_clientconfig config) {
    char const *cfg;
    cmd_ln_t *cmdln = NULL;

	// ps_default_search_args(cmdln);
	
	log_message(DEBUG, "initializing sphinx params");
	cmdln = cmd_ln_init(NULL, ps_args(), TRUE, NULL);
	 
	// disable log
	cmd_ln_set_str_r(cmdln, "-logfn", "/dev/null");
	 
	if (strlen(config->device) != 0) {
		cmd_ln_set_str_r(cmdln, "-abcdev", config->device);
	}
	if (strlen(config->keywordlist) != 0) {
		log_message(DEBUG, "setting keyword list %s", config->keywordlist);
		cmd_ln_set_str_r(cmdln, "-kws", config->keywordlist);		
	}
	if (config->samplingrate != 0) {
		log_message(DEBUG, "setting sampling rate %d", config->samplingrate);		
		cmd_ln_set_int_r(cmdln, "-samprate", config->samplingrate);
	}
	if (strlen(config->hmmfile) != 0) {
		log_message(DEBUG, "setting hmm dir %s", config->hmmfile);
		cmd_ln_set_str_r(cmdln, "-hmm", config->hmmfile);		
	}
	if (strlen(config->lmfile) != 0) {
		log_message(DEBUG, "setting lm dir %s", config->lmfile);
		cmd_ln_set_str_r(cmdln, "-lm", config->lmfile);		
	}
	if (strlen(config->dictfile) != 0) {
		log_message(DEBUG, "setting doct dir %s", config->dictfile);
		cmd_ln_set_str_r(cmdln, "-dict", config->dictfile);		
	}
	    
	log_message(DEBUG, "initializing decoder");
	ps = ps_init(cmdln);
	if (! ps) {
		log_message(ERROR, "Could not initialize pocketsphinx");
		return 1;
	}
	
	log_message(DEBUG, "opening device");
    if ((ad = ad_open()) == NULL) {
        log_message(ERROR,"ad_open_dev failed\n");
        return 2;
	}

	cmd_ln_free_r(cmdln);
	return 0;
}

void sphinx_shutdown() {
    ps_free(ps);
    ad_close(ad);	
}
/*
 * Main utterance processing loop:
 *     for (;;) {
 * 	   wait for start of next utterance;
 * 	   decode utterance until silence of at least 1 sec observed;
 * 	   print utterance result;
 *     }
 */
char const * recognize() {
    int16 adbuf[2048];
    int32 k;
    uint8 utt_started, in_speech;
 
    char const *hyp;
    char word[256];


	/* Indicate listening for next utterance */
	log_message(DEBUG,"starting recognition....");
	if (!ad) {
		log_message(ERROR, "no ad");
		return 0;
	}
	
	if (ad_start_rec(ad) < 0)
		log_message(ERROR, "ad_start_rec failed\n");
 	log_message(DEBUG,"waiting");

	if (ps_start_utt(ps) < 0)
		log_message(ERROR, "ps_start_utt() failed\n");

	log_message(DEBUG,"Listening...");

	/* Decode utterance until end (marked by a "long" silence, >1sec) */
	for (;;) {
       if ((k = ad_read(ad, adbuf, 2048)) < 0)
            log_message(ERROR, "Failed to read audio\n");
        ps_process_raw(ps, adbuf, k, FALSE, FALSE);
        in_speech = ps_get_in_speech(ps);
        if (in_speech && !utt_started) {
            utt_started = TRUE;
            log_message(INFO, "Listening...\n");
        }
        if (!in_speech && utt_started) {
            /* speech -> silence transition, time to start new utterance  */
            ps_end_utt(ps);
            hyp = ps_get_hyp(ps, NULL );
            if (hyp != NULL) {
                printf("%s\n", hyp);
                return hyp;
            }

            if (ps_start_utt(ps) < 0)
                log_message(ERROR, "Failed to start utterance\n");
            utt_started = FALSE;
            log_message(INFO, "Ready....\n");
        }

		rocsmq_delayms(1);
	}

	/*
	 * Utterance ended; flush any accumulated, unprocessed A/D data and stop
	 * listening until current utterance completely decoded
	 */
	ps_end_utt(ps); 
	ad_stop_rec(ad);

	log_message(DEBUG,"Stopped listening, please wait...\n");
    
    return hyp;
}
