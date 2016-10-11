#ifndef ADC_H
#define ADC_H

#include <avr/io.h>
#include <stdlib.h>

#define FALSE 0
#define TRUE 1

void adc_init(uint8_t channel, char interrupt){
	ADCSRA |= (1 << ADEN); //ADC enable
	ADCSRA |= (1 << ADPS2); //prescalar set to 16
	ADMUX |= (1 << REFS0); //use AVCC as reference voltage
	
	ADMUX |= channel & 0b00001111; //ADC channel (pin) select
	
	if (interrupt) ADCSRA |= (1 << ADIE); //interrupt enable (ADC_vect)
	
	ADCSRA |= (1 << ADSC); //start conversion
}

uint16_t adc_getValue(void){
	uint8_t low = ADCL;
	uint16_t adcValue = ADCH << 8 | low;
	
	ADCSRA |= (1 << ADSC); //start next conversion
	
	return adcValue;
}

#endif