#ifndef DHT11_H_INCLUDED
#define DHT11_H_INCLUDED

#include <avr/io.h>

// Define the analog pin for temperature and humidity
#define TEMP_HUMIDITY_PIN PC0

void dht11_init(void);
uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity);

#endif // DHT11_H_INCLUDED
