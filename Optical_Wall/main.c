/*
 * RoofSensor.c
 *
 * Created: 5/30/16 6:22:38 AM
 * Author : ArghyaDas
 */ 

#define F_CPU 20000000UL			// 20MHz clock is when the Vcc is between 4.5 - 5.5V. ( Rise time and fall times are 0.2Us)
#define SensorGap 4	// Distance between sensors is 3 inches . Recommended sensor gap is 3 inches because the distance between the starting edge of consecutive tape marks is 4 inches. So for the code to work properly the sensors should be placed more than 2 inches and less than 4 inches apart hence 3 inches is ideal.
#include <avr/io.h>
#include <avr/interrupt.h> // Enables interrupt functions
#include <util/delay.h>
#include <stdlib.h>
#include "uart.h"
int flag1 = 0;// this flag comes high when First sensor is triggered.
int flag2 = 0;// this flag come high when Second sensor is triggered.
long overflow = 0;

//float position[47] = {0.0,1200.0,2400.0,3600.0,4800.0,6000.0,7200.0,8400.0,9600.0,9604.0,9608.0,9612.0,9616.0,9620.0,9624.0,9628.0,9632.0,9636.0,9640.0,9644.0,9648.0,9652.0,9656.0,9660.0,9664.0,9668.0,9672.0,9676.0,10800.0,12000.0,13200.0,14400.0,15600.0,15604.0,15608.0,15612.0,15616.0,15620.0,15624.0,15628.0,15632.0,15636.0,16800.0,18000.0,19200.0,20400.0,21600.0}; // Position vector with distance from start in inches.

//Interrupt Service Routine for when the timer/counter reaches the value stored in OCR1A i.e 0x03FF or 1024 .
/*
ISR(TIMER1_COMPA_vect) 
{
	overflow = overflow + 1024;
}
*/

ISR(TIMER1_OVF_vect)
{
	overflow = overflow + 65536;		// When OVF, add # clock cycles to overflow
}

//Interrupt Service Routine for when Sensor 1 is Triggered.
ISR(PCINT1_vect)
{
	if((PINC & 0x01) == 0)
	{
		if(flag1 == 0)
		{
			TCNT1 = 0x0000;
			overflow = 0;
			flag1 = 1;// flag 1 is set high because first sensor was triggered.
			//PORTB |= (flag1 << PORTB2);
			if((TIFR1 & 0x02) != 0)
			{
				TIFR1 |= 0x02;				// If counter reaches OCR1A inside PCINT1, clear flag so ISR not serviced after exiting PCINT1 ISR
			}
		}
	}
}

// Interrupt Service Routine for when Sensor 2 is triggered.
ISR(PCINT0_vect)
{
	if((PINB & 0x01) == 0)
	{
		if(flag1 == 1)
		{
			flag2 = 1; // flag 2 is set high because Second Sensor was triggered after First sensor was triggered by the starting edge of the same tape mark.
			//PORTB |= (flag2 << PORTB1);
		}	
	}
}
// Connect First sensor to PORT C
void pcint1_init()
{	
	DDRC |= (0<<DDC0);			// Set PC0 to input
	PCICR |= (1<<PCIE1);		// Enable PC Interrupt 1
	PCMSK1 = (1<<PCINT8);		// Allow only PC0 to trigger PCINT1
}
// Connect Second sensor to PORT B
void pcint0_init()
{
	DDRB |= (0<<DDB0);			// Set PB0 to input
	PCICR |= (1<<PCIE0);		// Enable PC Interrupt 0
	PCMSK0 = (1<<PCINT0);		// Allow only PB0 to trigger PCINT0
}

void tc1_init()
{
	TCCR1B |= (1<<CS10);		// DIV1 clock source, no prescaling
	TIMSK1 |= (1<<TOIE1);		// Timer overflow interrupt is enabled because the timer will overflow when it passes 65536 cycles.
}

/*
void tc1_init()
{
	TCCR1A = 0x00;				// Disable output compare pins, WGM1 1-0 = 00
	TCCR1B = 0x09;				// WGM1 3-2 = 01, DIV1
	OCR1A = 0x03FF;				// Set TOP to 2^10 or '1k'
	TIMSK1 |= (1<<OCIE1A);		// Timer overflow interrupt is enabled because the timer will overflow when it passes 65536 cycles.
}
*/

int main (void)
{
	float time_elapsed = 0;
	float speed = 0;
	float pod_position = 0;
	int i =0;
	pcint0_init();
	pcint1_init();
	tc1_init();
	uart_init(28800, 0, 8, NONE, 1, TRUE);
	char display[10];
	DDRB |= 0x06;
	sei();
	while(1)
	{
		
		
		float cycles_elapsed = 0;
		while(flag1 == 0);
		
		//PORTB |= (flag1 << PORTB2);
		while(flag2 == 0);
		PORTB |= (1 << PORTB2);
		//PORTB |= (flag2 << PORTB1);
		
		cycles_elapsed = overflow + TCNT1;
		//time_elapsed = cycles_elapsed/(20000000);// 20000 Hz or 20 MHz is the clock frequency.
		//speed = (SensorGap/time_elapsed)*0.0568182; // converting inches per second to miles per hour by multiplying with a factor of 0.0568182 because 1 inch/sec = 0.0568182 miles/hour
		
		time_elapsed = cycles_elapsed/(20000);// 20000 Hz or 20 MHz is the clock frequency.
		speed = (SensorGap/time_elapsed)*56.8182; // converting inches per second to miles per hour by multiplying with a factor of 0.0568182 because 1 inch/sec = 0.0568182 miles/hour
		
		dtostrf(speed, 1, 0, display);							// Converts float to character string with no decimal points
		PORTB = 0x00;
		uart_string(display);
		uart_char(',');
		
		flag1 = 0;
		flag2 = 0;
		//PORTB = 0x00;
		
		
		
		/*
		if (flag2 == 1) // since Sensor 2 was triggered after Sensor 1 on the same tape mark we do the speed and distance calculations here.
		{
			cycles_elapsed = overflow + TCNT1;
			time_elapsed = cycles_elapsed/(20000000);// 20000 Hz or 20 MHz is the clock frequency.
			speed = (SensorGap/time_elapsed)*0.0568182; // converting inches per second to miles per hour by multiplying with a factor of 0.0568182 because 1 inch/sec = 0.0568182 miles/hour
			
			
			if (i<48) // to check so that array index doesn't go out of bounds
			{
				pod_position=position[i];// gives distance from start position in inches by reading position from the position vector defined at the beginning.
				//pod_position=position[i]*0.0833333;// gives distance from start position in feet.
				//pod_position=position[i]*0.0254;// gives distance from start position in metres.
				i++; // increment the index to traverse though the position vector each time the second sensor is triggered after the first sensor by the same tape mark.
			}
			
			
			
			dtostrf(speed, 1, 0, display);							// Converts float to character string with no decimal points
			uart_string(display);
			uart_char(',');
			
			//flag1 = 0;
			//flag2 = 0;
			//PORTB = 0x00;
		}
		*/
	}
}