#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "lcd_i2c.h"

#define DHT11_PIN 6

// Control pins
#define FAN_PIN PD2
#define PUMP1_PIN PD3
#define PUMP2_PIN PD4
#define HUMIDIFIER_PIN PD5

// Predefined conditions
#define TEMP_THRESHOLD 25
#define HUMIDITY_THRESHOLD 50
#define WATER_LEVEL_THRESHOLD 500
#define TDS_THRESHOLD 200
volatile char receivedChar;
uint8_t c = 0, I_RH, D_RH, I_Temp, D_Temp, CheckSum;
volatile int rxFlag = 0;
void Request() {
    DDRD |= (1 << DHT11_PIN);
    PORTD &= ~(1 << DHT11_PIN); /* set to low pin */
    _delay_ms(20);              /* wait for 20ms */
    PORTD |= (1 << DHT11_PIN);  /* set to high pin */
}

void Response() {
    DDRD &= ~(1 << DHT11_PIN);
    while (PIND & (1 << DHT11_PIN));
    while ((PIND & (1 << DHT11_PIN)) == 0);
    while (PIND & (1 << DHT11_PIN));
}

uint8_t Receive_data() {
    for (int q = 0; q < 8; q++) {
        while ((PIND & (1 << DHT11_PIN)) == 0); /* check received bit 0 or 1 */
        _delay_us(30);
        if (PIND & (1 << DHT11_PIN)) /* if high pulse is greater than 30ms */
            c = (c << 1) | (0x01);   /* then its logic HIGH */
        else
            c = (c << 1);            /* otherwise its logic LOW */
        while (PIND & (1 << DHT11_PIN));
    }
    return c;
}

void init_adc() {
    ADMUX = (1 << REFS0); // Reference voltage AVcc
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // Enable ADC, prescaler = 64
}

uint16_t read_adc(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F); // Select ADC channel
    ADCSRA |= (1 << ADSC); // Start conversion
    while (ADCSRA & (1 << ADSC)); // Wait for conversion to complete
    return ADC;
}

void display_data(uint16_t wl_value, uint16_t tds_value, uint8_t temperature, uint8_t humidity) {
    char wl_str[6], tds_str[6], temp_str[4], hum_str[4],teststr[2];

    // Convert values to strings
    itoa(wl_value, wl_str, 10);
    itoa(tds_value, tds_str, 10);
    itoa(temperature, temp_str, 10);
    itoa(humidity, hum_str, 10);
    itoa(receivedChar,teststr,10);
    // Display readings on LCD
    lcd_clrscr();
    lcd_gotoxy(0, 0);
    lcd_puts("WL:");
    lcd_puts(wl_str);
    lcd_puts(" T:");
    lcd_puts(temp_str);
    lcd_gotoxy(0, 1);
    lcd_puts("C H:");
    lcd_puts(hum_str);
    lcd_puts("% TDS:");
    lcd_puts(teststr);
}

void control_devices(uint8_t temperature, uint8_t humidity, uint16_t wl_value, uint16_t tds_value) {
    // Control fan
    if (temperature > TEMP_THRESHOLD) {
        PORTD |= (1 << FAN_PIN); // Turn on fan
    } else {
        PORTD &= ~(1 << FAN_PIN); // Turn off fan
    }

    // Control pump1
    if (wl_value < WATER_LEVEL_THRESHOLD) {
        PORTD |= (1 << PUMP1_PIN); // Turn on pump1
    } else {
        PORTD &= ~(1 << PUMP1_PIN); // Turn off pump1
    }

    // Control pump2
    if (tds_value > TDS_THRESHOLD) {
        PORTD |= (1 << PUMP2_PIN); // Turn on pump2
    } else {
        PORTD &= ~(1 << PUMP2_PIN); // Turn off pump2
    }

    // Control humidifier
    if (humidity < HUMIDITY_THRESHOLD) {
        PORTD |= (1 << HUMIDIFIER_PIN); // Turn on humidifier
    } else {
        PORTD &= ~(1 << HUMIDIFIER_PIN); // Turn off humidifier
    }
}

#define F_CPU 16000000UL  // Define CPU frequency for delay calculation
#define BAUD 9600         // Define baud rate



void uart_init(void) {
    uint16_t ubrr = F_CPU/16/BAUD - 1;
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)ubrr;
    UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);  // Enable receiver, transmitter and RX interrupt
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  // Set frame format: 8 data bits, 1 stop bit
}

void uart_transmit(unsigned char data) {
    while (!(UCSR0A & (1 << UDRE0)));  // Wait for empty transmit buffer
    UDR0 = data;                       // Put data into buffer, sends the data
}

ISR(USART_RX_vect) {
    //receivedChar = UDR0;
      rxFlag = 1;
    receivedChar = UDR0;


}




int main(void) {
    DDRB |= (1 << DDB5);
    uint16_t wl_value, tds_value;
    uint8_t temperature, humidity;

    // Initialize I2C, LCD, USART and ADC
    i2c_init();
    lcd_init(LCD_BACKLIGHT);
    lcd_clrscr();
    lcd_gotoxy(4, 0);
    lcd_puts("Reading...");
    _delay_ms(2000);

    init_adc();
    uart_init();
    sei(); // Enable global interrupts

    // Set control pins as output
    DDRD |= (1 << FAN_PIN) | (1 << PUMP1_PIN) | (1 << PUMP2_PIN) | (1 << HUMIDIFIER_PIN);

    while (1) {
        // Read water level sensor connected to PC2 (ADC2)
        wl_value = read_adc(2);

        // Read TDS sensor connected to PC1 (ADC1)
        tds_value = read_adc(1);

        // Read temperature and humidity from DHT sensor
        Request(); /* send start pulse */
        Response(); /* receive response */
        I_RH = Receive_data(); /* store first eight bit in I_RH */
        D_RH = Receive_data(); /* store next eight bit in D_RH */
        I_Temp = Receive_data(); /* store next eight bit in I_Temp */
        D_Temp = Receive_data(); /* store next eight bit in D_Temp */
        CheckSum = Receive_data(); /* store next eight bit in CheckSum */

        if ((I_RH + D_RH + I_Temp + D_Temp) != CheckSum) {
            lcd_clrscr();
            lcd_gotoxy(0, 0);
            lcd_puts("Error");
        } else {
            temperature = I_Temp;
            humidity = I_RH;
            display_data(wl_value, tds_value, temperature, humidity);
            control_devices(temperature, humidity, wl_value, tds_value);
        }

         //PORTB |= (0 << PB5);
        if (rxFlag) {
            rxFlag = 0;
             if (receivedChar != '\0') {
        if (receivedChar == '1') {
                   PORTD |= (1 << FAN_PIN);
            PORTB |= (1 << PB5);  // Set PB5 (pin 13) high
        } else if (receivedChar == '0') {
                   PORTD &= ~(1 << FAN_PIN);
            PORTB &= ~(1 << PB5);  // Set PB5 (pin 13) low
        }

        // Echo received character back (optional)
        uart_transmit(receivedChar);

        // Clear receivedChar
        receivedChar = '\0';
    }

            uart_transmit('y');    // Transmit received byte
            uart_transmit(0x0A);      // Line feed
            uart_transmit(0x0D);      // Carriage return
            _delay_ms(500);
        }

        _delay_ms(2000); // Delay before next reading
    }
}
