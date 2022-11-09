#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include <stdlib.h>

#include "led.h"

// Students notice: This program uses global variables, which would be bad design for any larger systems than this.
// Not that the design were particularly elegant to begin with

void setup(void);
void tick(void);
void matrix(void);
void animate(void);
void setanimation(void);
uint16_t animate_parsevalue(uint8_t digits);
uint8_t animate_hex2dec(uint8_t character);
void powerdown(void);


int main(void)
{
	uint8_t pushcount;
	setup();
	for (uint8_t i = 0; i < 20; i++)
		tick();
	while (led_button)
		;
	for (uint8_t i = 0; i < 20; i++)
		tick();

	led_init();

	uint8_t brightness = 0;
	uint8_t going_up = 1;
	// this loop increases/decreases led brightness
	// every led should light up
	while (1)
	{
		for (uint16_t i = 0; i < ROWS * ROWS; i++)
			l[i] = brightness; // set every led to same brightness
		
		if (led_button)
		{
			for (uint16_t i = 0; i < ROWS * ROWS; i++)
				l[i] = 0;
			l2led(); // immediately blank display
			tick();	 // debounce push
			pushcount = 0;
			while (led_button)
			{
				tick();
				if (!(++pushcount))
					pushcount--;

			}
			if (pushcount > 100)
				powerdown();
			tick(); // debounce release
		}

		l2led(); // after you call this, leds will set according to l-matrix
		for (uint8_t i = 0; i < 10; i++) // wait for a while
			tick();

		if(brightness == 15){
			going_up = -1;
		}
		if(brightness == 0){
			going_up = 1;
		}
		brightness += going_up;
	}
}

void setup(void)
{
	led_init();
	sei();
}

void tick(void)
{
	uint8_t tmp;
	tmp = led_tick;
	while (tmp == led_tick)
		;
}

void powerdown(void)
{
	cli();
	TIMSK = 0x00; // disable timer 1 compare A interrupt
	DDRA = 0x00;
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x00;
	DDRE = 0x00;
	PORTA = 0x00;
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x04;
	PORTE = 0x00;
	MCUCR &= ~0x03; // select low level for INT0
	GIFR = 0x00;	// clear pending interrupts
	GICR = 0x40;	// enable INT0
	sei();
	MCUCR |= 0x20; // enable sleep
	asm volatile("sleep\n");
}