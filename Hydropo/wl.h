#ifndef WL_H_INCLUDED
#define WL_H_INCLUDED
#include <avr/io.h>

// Define the analog pin for water level sensor
#define WATERLEVEL_SENSOR_PIN PC2

void waterlevel_sensor_init(void);
uint8_t waterlevel_sensor_read(uint8_t *water_level);

#endif // WATERLEVEL_SENSOR_H_INCLUDED
