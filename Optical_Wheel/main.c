#define F_CPU 20000000UL			// 20MHz clock is when the Vcc is between 4.5 - 5.5V. ( Rise time and fall times are 0.2Us)
#define circumference 18*3.14159	// Diameter of wheel is 18 inches
#include <avr/io.h>
#include <avr/interrupt.h> // Enables interrupt functions
#include <util/delay.h>
#include <stdlib.h>
#include "LCD.h"
#include "uart.h"

// Many variables are initialized as global as to increase speed -> reducing error


////////// Change TC top value to 2^10. This will reduce error if the TCNT1 value resets inside the PCINT1 ISR //////////


int flag1 = 0;
int flag2 = 0;
long overflow;
//char display[10];
//int cycles_elapsed;
//float time_elapsed;

uint16_t count_temp;
long overflow_temp;

void pcint1_init(void);
void spi_string(char *display);
void spi_character(char character);
void tc1_init(void);
//void usart_spi(unsigned int baud);


ISR(PCINT1_vect)
{
	/*
	//count_temp = TCNT1;
	//overflow_temp = overflow;
	
	if((PINC & 0x01) == 0)
	{		
		if(flag1 == 0)
		{
			TCNT1 = 0x0000;					// Set count to zero
			overflow = 0;					// Set overflow to zero
			if((TIFR1 & 0x01) != 0)
			{
				TIFR1 |= 0x01;				// If OVF triggered inside PCINT1, clear flag so OVF ISR not serviced after exiting PCINT1 ISR
			}
			flag1 = 1;
			PORTC |= (1<<PORTC4);			// Light first LED
		}
		else if(flag1 == 1)
		{
			PORTC |= (1<<PORTC5);								// Light second LEC		
			cycles_elapsed = overflow + TCNT1;
			//cycles_elapsed = overflow_temp + count_temp;		// Determine amount of cycles from first flag
			//time_elapsed = cycles_elapsed/F_CPU;				// Determine time elapsed in seconds
			time_elapsed = cycles_elapsed/(20000000);
			dtostrf(time_elapsed, 1, 4, display);				// Create string from float time_elapsed that has at least 1 element and 4 decimal figures, store in display
			lcd_command(0x01);									// Clear display
			lcd_string(display);								// Display previously made string on LCD
			lcd_char('s');										// Add 's' for seconds at the end
			flag1 = 0;											// Clear flag
			PORTC = 0x00;										// Turn off LEDs
		}
	}
	*/
	if((PINC & 0x01) == 0)
	{
		if(flag1 == 0)
		{
			flag1 = 1;
			TCNT1 = 0x0000;
			overflow = 0;
			if((TIFR1 & 0x02) != 0)
			{
				TIFR1 |= 0x02;				// If counter reaches OCR1A inside PCINT1, clear flag so ISR not serviced after exiting PCINT1 ISR
			}
		}
		else
		{
			flag2 = 1;
		}
	}
}

/*
ISR(TIMER1_OVF_vect)
{
	overflow = overflow + 65536;		// When OVF, add # clock cycles to overflow
}
*/

ISR(TIMER1_COMPA_vect)
{
	overflow = overflow + 1024;
}


void main()
{
	//lcd_init();				// Initialize LCD (from "LCD.h")
	pcint1_init();			// Initialize Pin Change INT1
	tc1_init();				// Initialize timer/counter 1
	//usart_spi(100);		// Initialize MSPIM,     INSERT BAUD RATE
	uart_init(57600, 0, 8, NONE, 1, FALSE);
	char display[10];
	int cycles_elapsed;
	//float time_elapsed;
	float speed;
	DDRB |= 0x06;
	
	uart_string("Ready");
	
	while(1)
	{	
		float cycles_elapsed = 0;
		uart_string("test");
		/*if(flag1 == 1)
		{
			PORTB |= (1<<PORTB1);
			while(flag2 != 1);
			PORTB |= (1<<PORTB2);
			cycles_elapsed = overflow + TCNT1;
			
			//////////
			speed = (1136364*circumference)/(cycles_elapsed);		// Calculates mph
			dtostrf(speed, 1, 0, display);							// Converts float to character string with no decimal points
			PORTB = 0x00;
			uart_string(display);
			uart_char(',');
			PORTB |= (1<<PORTB2);
			//spi_string(display);
			//////////
			
			//time_elapsed = cycles_elapsed/(20000);
			//dtostrf(time_elapsed, 1, 4, display);
			//lcd_command(0x01);
			//lcd_string(display);
			//lcd_string("mph");
			flag1 = 0;
			flag2 = 0;
			PORTB = 0x00;
		}*/
	}
}


void pcint1_init()
{
	DDRC |= (0<<DDC0);			// Set PC0 to input
	PCICR |= (1<<PCIE1);		// Enable PC Interrupt 1
	PCMSK1 = (1<<PCINT8);		// Allow only PC0 to trigger PCINT1
	sei();						// Enable interrupts
}

///// Use if want overflow to trigger on 2^16 cycles /////

/*
void tc1_init()
{
	TCCR1B |= (1<<CS10);		// DIV1 clock source, no prescaling
	TIMSK1 |= (1<<TOIE1);		// Timer overflow interrupt is enabled because the timer will overflow when it passes 65536 cycles.
}
*/

//////////////////////////////////////////////////////////
/*
void spi_character(char character)
{
	while(UDRE0 == 0);
	UDR0 = character;
}

void spi_string(char *display)
{
	int i = 0;
	while(display[i] != 0)
	{
		spi_character(display[i]);
		i++;
	}	
}
*/

void tc1_init()
{
	TCCR1A = 0x00;				// Disable output compare pins, WGM1 1-0 = 00
	TCCR1B = 0x09;				// WGM1 3-2 = 01, DIV1
	OCR1A = 0x03FF;				// Set TOP to 2^10 or '1k'
	TIMSK1 |= (1<<OCIE1A);		// Timer overflow interrupt is enabled because the timer will overflow when it passes 65536 cycles.
}

/*
void usart_spi( unsigned int baud )
{
	UBRR0 = 0;
	DDRD |= (1<<DDD4);			// Setting the XCKn port pin as output, enables master mode.
	DDRD |= (1<<DDD1);
	UCSR0C = (1<<UMSEL01)|(1<<UMSEL00)|(0<<UCPHA0)|(0<<UCPOL0);	// Set MSPI mode of operation and SPI data mode 0.
	UCSR0B = (1<<TXEN0);		// Enable transmitter.
	UBRR0 = baud;				// Set baud rate AFTER Tx enabled
}
*/