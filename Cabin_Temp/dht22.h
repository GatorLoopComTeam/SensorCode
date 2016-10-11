/* RHT03/DHT22 Temperature and Humidity Sensor Library Header */

#ifndef DHT_H_
#define DHT_H_

#include <stdio.h>
#include <avr/io.h>

//setup port
#define DHT_DDR DDRC
#define DHT_PORT PORTC
#define DHT_PIN PINC
#define DHT_INPUTPIN PC0

//timeout retries
#define DHT_TIMEOUT 200

//functions
extern int8_t dht_read_temp(float *temperature);
extern int8_t dht_read_humidity(float *humidity);
extern int8_t dht_read_data(float *temperature, float *humidity);

#endif
