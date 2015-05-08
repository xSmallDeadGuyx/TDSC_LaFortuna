/*
 * https://github.com/fatcookies
 * 
 * La Fortuna (at90usb1286) @ 8MHz
 * Plays an 8bit/8000 sample PCM audio on OC1A and OC3A
 * 
 * Adapted from:
 * http://avrpcm.blogspot.co.uk/2010/11/playing-8-bit-pcm-using-any-avr.html 
 */
 
#ifndef _AUDIO_H
#define _AUDIO_H

#ifndef _FATFS
#include "ff.h"
#endif

#ifndef _STDINT_H 
#include <stdint.h>
#endif

#define SAMPLE_RATE 8000
#define BUFFER_SIZE 4048
#define SAMPLE 32000/SAMPLE_RATE

uint8_t audio_isplaying();
void audio_close();
FRESULT audio_load(FIL* audiofile);

#endif /* _AUDIO_H */
