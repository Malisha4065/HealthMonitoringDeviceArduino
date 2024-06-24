#include "dht11.h"
#include <util/delay.h>

void dht11_init(void) {
    // Set TEMP_HUMIDITY_PIN (PC0) as input for ADC
    ADMUX = (0 << REFS1) | (0 << REFS0); // Use Vcc as reference voltage
    ADMUX |= TEMP_HUMIDITY_PIN; // Select ADC0 (PC0) as input channel
    ADCSRA |= (1 << ADEN); // Enable ADC
}

uint8_t dht11_read(uint8_t *temperature, uint8_t *humidity) {
    uint16_t adc_value;

    // Start ADC conversion
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete

    // Read ADC value (10-bit resolution)
    adc_value = ADC;

    // Convert ADC value to temperature and humidity (example conversion, adjust as per your sensor)
    // Example conversion assuming linear mapping from ADC value to temperature and humidity
    *temperature = adc_value / 40; // Example scaling for temperature
    *humidity = adc_value / 16; // Example scaling for humidity

    return 0;
}
