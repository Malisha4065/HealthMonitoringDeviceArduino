#include "wl.h"
#include <util/delay.h>

void waterlevel_sensor_init(void) {
    // Set WATERLEVEL_SENSOR_PIN (PC2) as input for ADC
    ADMUX = (0 << REFS1) | (0 << REFS0); // Use Vcc as reference voltage
    ADMUX |= WATERLEVEL_SENSOR_PIN; // Select PC2 as input channel
    ADCSRA |= (1 << ADEN); // Enable ADC
}

uint8_t waterlevel_sensor_read(uint8_t *water_level) {
    uint16_t adc_value;

    // Start ADC conversion
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete

    // Read ADC value (10-bit resolution)
    adc_value = ADC;

    // Convert ADC value to water level (example conversion, adjust as per your sensor)
    // Example conversion assuming linear mapping from ADC value to water level
    *water_level = adc_value / 4; // Example scaling for water level

    return 0;
}
