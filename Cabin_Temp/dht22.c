/* RHT03/DHT22 Temperature and Humidity Sensor Library */

#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>

#include "dht22.h"

// get data from sensor
int8_t dht_read(float *temperature, float *humidity) {

	uint8_t bits[5];
	uint8_t i,j = 0;

	memset(bits, 0, sizeof(bits));

	//reset port
	DHT_DDR |= (1<<DHT_INPUTPIN); //output
	DHT_PORT |= (1<<DHT_INPUTPIN); //high
	_delay_ms(100);

	//send request
	DHT_PORT &= ~(1<<DHT_INPUTPIN); //low
	_delay_us(500);
	DHT_PORT |= (1<<DHT_INPUTPIN); //high
	DHT_DDR &= ~(1<<DHT_INPUTPIN); //input
	_delay_us(40);

	//check start condition 1
	if((DHT_PIN & (1<<DHT_INPUTPIN))) {
		return -1;
	}
	_delay_us(80);
	//check start condition 2
	if(!(DHT_PIN & (1<<DHT_INPUTPIN))) {
		return -1;
	}
	_delay_us(80);

	//read the data
	uint16_t timeoutcounter = 0;
	for (j=0; j<5; j++) { //read 5 bytes
		uint8_t result=0;
		for(i=0; i<8; i++) { //read every bit
			timeoutcounter = 0;
			while(!(DHT_PIN & (1<<DHT_INPUTPIN))) { //wait for an high input (non-blocking)
				timeoutcounter++;
				if(timeoutcounter > DHT_TIMEOUT) {
					return -1; //timeout
				}
			}
			_delay_us(30);
			if(DHT_PIN & (1<<DHT_INPUTPIN)) //if input is high after 30us, get result
				result |= (1<<(7-i));
			timeoutcounter = 0;
			while(DHT_PIN & (1<<DHT_INPUTPIN)) { //wait until input get low (non-blocking)
				timeoutcounter++;
				if(timeoutcounter > DHT_TIMEOUT) {
					return -1; //timeout
				}
			}
		}
		bits[j] = result;
	}

	//reset port
	DHT_DDR |= (1<<DHT_INPUTPIN); //output
	DHT_PORT |= (1<<DHT_INPUTPIN); //low
	_delay_ms(100);

	//check checksum
	if ((uint8_t)(bits[0] + bits[1] + bits[2] + bits[3]) == bits[4]) {
		//return temperature and humidity
		uint16_t rawhumidity = bits[0]<<8 | bits[1];
		uint16_t rawtemperature = bits[2]<<8 | bits[3];
		if(rawtemperature & 0x8000) {
			*temperature = (float)((rawtemperature & 0x7FFF) / 10.0) * -1.0;
		} else {
			*temperature = (float)(rawtemperature)/10.0;
		}
		*humidity = (float)(rawhumidity)/10.0;
		return 0;
	}

	return -1;
}

// read temperature
int8_t dht_read_temp(float *temperature) {
	float humidity = 0;
	return dht_read(temperature, &humidity);
}

// read humidity
int8_t dht_read_humidity(float *humidity) {
	float temperature = 0;
	return dht_read(&temperature, humidity);
}

// read both temperature and humidity
int8_t dht_read_data(float *temperature, float *humidity) {
	return dht_read(temperature, humidity);
}