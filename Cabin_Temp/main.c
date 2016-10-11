// Code to run reed switch, weight sensor, and temperature sensor
// To be used on board located under main cabin dummy seat and near cpu

#define F_CPU 1000000UL

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

#include "uart.h"
#include "adc.h"
#include "dht22.h"

int main(void)
{
	// initialize uart
	uart_init(4800, 0, 8, NONE, 1, FALSE);
	uart_string("ready\n\r");
	
	// temperature sensor setup
	char temp[6]; //string buffer
	float t = 0; //temperature value
	//float h = 0; //percent humidity
/*
	// flexiforce pressure sensor setup
	adc_init(2, FALSE); //initialize adc on channel 2
	DDRC &= ~(1 << PINC2); //set input pin
	DDRC |= (1 << PINC3); //set output indicator pin
	char seat;
*/
	// reed switch setup
	DDRC &= ~(1 << PINC5); //set input pin
	char buckle;
	
    while (1){
		// read temperature
		if (dht_read_temp(&t) != -1)
			dtostrf(t,4,1,temp); //convert temp to string
/*
		// read seat pressure
		if (adc_getValue() > 0){
			seat = TRUE; //set seat flag
			PORTC |= (1 << PINC3); //turn on indicator led
		}
		else {
			seat = FALSE; //clear seat flag
			PORTC &= ~(1 << PINC3); //turn off led
		}
*/
		// watch for reed activation
		if (bit_is_clear(PINC,5))
			buckle = FALSE; //clear belt flag
		else buckle = TRUE; //set belt flag
		
		//concat and send values
		uart_string("T = ");
		uart_string(temp);
		uart_string(" C\n\r");
		
		DDRD |= (1 << PIND5);
		if (t < 24)
			PORTD |= (1 << PIND5);
		else PORTD &= ~(1 << PIND5);
		
		/*char str[11];
		strcat(str, temp);
		strcat(str, " S");
		strcat(str, seat);
		strcat(str, " B");
		strcat(str, buckle);
		strcat(str, ",");
		uart_string(str);*/
		
		/*uart_char(0x2C);
		uart_char(buckle);
		uart_char(0x2C);
		uart_char(buckle);
		uart_char(0x7C);*/
		
		_delay_ms(1500);
    }
}