#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <math.h>

#define EVEN 0
#define ODD 1
#define NONE 2

#define FALSE 0
#define TRUE 1

void uart_init(int baud, char async, char size, char parity, char stopbits, char interruptEn){
	UCSR0B = (1 << RXEN0) | (1 << TXEN0); //enable TX and RX
	
	uint16_t UBBRValue = F_CPU/16/baud-1; //baud rate
	
	UBRR0H = (unsigned char) (UBBRValue >> 8); //put the upper byte of the baud (bits 8 to 11)
	UBRR0L = (unsigned char) UBBRValue; //put the lower byte of the baud
	
	if (async == 1) UCSR0A = (1 << U2X0); //setting the U2X bit to 1 for double speed asynchronous
	
	//set data length
	switch(size){
		case 6: UCSR0C |= (1 << UCSZ00); break; //6-bit data length
		case 7: UCSR0C |= (2 << UCSZ00); break; //7-bit data length
		case 8: UCSR0C |= (3 << UCSZ00); break; //8-bit data length
		case 9: UCSR0C |= (7 << UCSZ00); break; //9-bit data length
	}
	
	//set parity
	if (parity == EVEN) UCSR0C |= (1 << UPM01);
	if (parity == ODD) UCSR0C |= (3 << UPM00);

	//set stop bits
	if (stopbits == 2) UCSR0C = (1 << USBS0);
	// else 1 bit
	
	if (interruptEn) UCSR0B |= (1 << RXCIE0); //enable interrupts
}

void uart_char(unsigned char data){
	while (! (UCSR0A & (1 << UDRE0)) ); //wait until the transmitter is ready
	UDR0 = data; //send data
}

void uart_string(char *str){
	while(*str > 0)
		uart_char(*str++);
}

unsigned char uart_recv(void){
	while (! (UCSR0A & (1 << RXC0)) ); //wait until buffer full
	return UDR0;
}

#endif