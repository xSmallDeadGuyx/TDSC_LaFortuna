/*
 * https://github.com/fatcookies
 * 
 * La Fortuna (at90usb1286) @ 8MHz
 * Plays an 8bit/8000 sample PCM audio on OC1A and OC3A
 * 
 * Adapted from:
 * http://avrpcm.blogspot.co.uk/2010/11/playing-8-bit-pcm-using-any-avr.html 
 */

#include "audio.h"
#include <stdint.h>
#include <avr/interrupt.h>


/* Current sample playing from the buffer */
volatile uint16_t sample;

/* How often to take samples (32000/sample_count) = sample rate*/
volatile int sample_count;

/* Unsigned 8 bit PCM sample buffer */
volatile unsigned char pcm_samples[BUFFER_SIZE];

/* Playing audio flag */
volatile uint8_t playing = 0;

/* initialise the PWM */
void pwm_init(void) {
	 
   /* use OC1A (RCH) and OC3A (LCH) pin as output */
    DDRB |= _BV(PB5);
    DDRC |= _BV(PC6);

    /* 
    * clear OC1A/OC3A on compare match 
    * set OC1A/OC3A at BOTTOM, non-inverting mode
    * Fast PWM, 8bit
    */
    TCCR1A |= _BV(COM1A1) | _BV(WGM10);
    TCCR3A |= _BV(COM3A1) | _BV(WGM30);
    
    /* 
    * Fast PWM, 8bit
    * Prescaler: clk/1 = 8MHz
    * PWM frequency = 8MHz / (255 + 1) = 31.25kHz
    */
    TCCR1B |= _BV(WGM12) | _BV(CS10);
    TCCR3B |= _BV(WGM32) | _BV(CS30);
    
    /* set initial duty cycle to zero */
    OCR1A = 0;
    OCR3A = 0;
    
    /* Setup Timer0 (RCH) */
    TCCR1A |= (1<<CS00);
    TCNT1 = 0;
    TIMSK1 |= (1<<TOIE1);
    
    /* Setup Timer3 (LCH) */
    TCCR3A |= (1<<CS00);
    TCNT3 = 0;
    TIMSK3 |= (1<<TOIE3);
    
    /* Scale sample rate from PWM frequency (32k/SAMPLE)= new rate */
    sample_count = SAMPLE;
	sei();
}

/* Replicate same behavior on LCH as RCH */
ISR(TIMER3_OVF_vect)
{
	OCR3A = OCR1A;
	
} 

/* Load a sample into PWM register*/
ISR(TIMER1_OVF_vect)
{		
         sample_count--;
         if (sample_count == 0) {
             sample_count = SAMPLE;           
             OCR1A = pcm_samples[sample++];
             
             if(sample > BUFFER_SIZE) {
				sample=0;
			 }
         }
}

/* Return playing flag */
uint8_t audio_isplaying() {
	return playing;
}

/* Disable Timer0 and Timer3 and set playing flag to FALSE */
void audio_close() {
	if(playing) {
		TCCR1B &= ~_BV(CS10);
		TCCR3B &= ~_BV(CS30);
		TCCR1A &= ~_BV(CS00);
		TIMSK1 &= (0<<TOIE1);
    
		TCCR3A &= (0<<CS00);
		TIMSK3 &= (0<<TOIE3);
    
		playing = 0;
		cli();
		sei();
	}
}
 
/* Load and start playing a File*/
FRESULT audio_load(FIL* File) {
	
	uint16_t read;  /* Number of bytes buffered (used to check eof)*/
	uint8_t status; /* FRESULT of the buffer read */
	
	f_lseek(File,44);
	f_read(File, &pcm_samples, BUFFER_SIZE ,&read);
	
	pwm_init();
	playing = 1;
	
	while(playing) {
		
		/* If at end of buffer, read more of file */
		if(sample >= BUFFER_SIZE) {
			status = f_read(File, &pcm_samples, BUFFER_SIZE ,&read);
			if(status) {
				audio_close();
				return status;
			}
			
			/* If eof, stop playback */
			if(read < BUFFER_SIZE) {
				audio_close();
			}
		}
	}
	return FR_OK;
}

