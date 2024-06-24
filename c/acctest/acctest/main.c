#define F_CPU 16000000UL
#define BAUD 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define xPin PC1
#define yPin PC2
#define zPin PC3

#define DHT11_PIN 3

int xMin = 272;
int xMax = 408;
int yMin = 269;
int yMax = 408;
int zMin = 287;
int zMax = 430;

#define SHOCK_THRESHOLD 2.0

volatile char receivedChar;
volatile int rxFlag = 0;

void init_adc() {
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); // prescaler = 64
}

uint16_t read_adc(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

void uart_init(void) {
	uint16_t ubrr = F_CPU/16/BAUD - 1;
	UBRR0H = (uint8_t)(ubrr >> 8);
	UBRR0L = (uint8_t)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void uart_transmit_string(const char* str) {
	while (*str) {
		uart_transmit(*str++);
	}
}

void uart_transmit_uint8(uint8_t value) {
	char buffer[4];
	snprintf(buffer, sizeof(buffer), "%u", value);
	
	for (int i = 0; buffer[i] != '\0'; i++) {
		uart_transmit(buffer[i]);
	}
}

ISR(USART_RX_vect) {
	rxFlag = 1;
	receivedChar = UDR0;
}

void uart_transmit_float(float value) {
	char buffer[10];
	dtostrf(value, 4, 2, buffer);
	
	for (int i = 0; i < strlen(buffer); i++) {
		uart_transmit(buffer[i]);
	}
}

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void Request() {
	DDRD |= (1 << DHT11_PIN);
	PORTD &= ~(1 << DHT11_PIN);
	_delay_ms(20);
	PORTD |= (1 << DHT11_PIN);
}

void Response() {
	DDRD &= ~(1 << DHT11_PIN);
	while (PIND & (1 << DHT11_PIN));
	while ((PIND & (1 << DHT11_PIN)) == 0);
	while (PIND & (1 << DHT11_PIN));
}
uint8_t c = 0, I_RH, D_RH, I_Temp, D_Temp, CheckSum;
uint8_t Receive_data() {
	for (int q = 0; q < 8; q++) {
		while ((PIND & (1 << DHT11_PIN)) == 0);
		_delay_us(30);
		if (PIND & (1 << DHT11_PIN))
		c = (c << 1) | (0x01);
		else
		c = (c << 1);
		while (PIND & (1 << DHT11_PIN));
	}
	return c;
}


// =================lcd============================
#define LCD PORTD
#define EN 3
#define RS 4

void lcdcmd(unsigned char cmd) {
	PORTB &= ~(1<<RS);
	//PORTB &= ~(1<<RW);
	LCD = cmd & 0xF0;
	PORTB |= (1<<EN);
	_delay_ms(1);
	PORTB &= ~(1<<EN);
	
	LCD = cmd<<4;
	PORTB |= (1<<EN);
	_delay_ms(1);
	PORTB &= ~(1<<EN);
}

void lcddata(unsigned char data) {
	PORTB |= (1<<RS);
	LCD = data & 0xF0;
	PORTB |= (1<<EN);
	_delay_ms(1);
	PORTB &= ~(1<<EN);
	
	LCD = data<<4;
	PORTB |= (1<<EN);
	_delay_ms(1);
	PORTB &= ~(1<<EN);
}

void lcd_display_control(uint8_t display, uint8_t cursor, uint8_t blink) {
	uint8_t command = 0x08;
	if (display) command |= 0x04;
	if (cursor) command |= 0x02;
	if (blink) command |= 0x01;
	lcdcmd(command);
}

void lcd_init() {
	DDRD |= 0xF0;
	DDRB = 0xFF;
	PORTB &= ~(1<<EN);
	
	lcdcmd(0x33);
	lcdcmd(0x32);
	lcdcmd(0x28);
	lcd_display_control(1, 0, 0);
	lcdcmd(0x01);
	_delay_ms(2);
}


void lcd_print(char *str) {
	unsigned char i = 0;
	
	while(str[i] != 0) {
		lcddata(str[i]);
		i++;
	}
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
	uint8_t address;
	
	if (row == 0) {
		address = 0x80 + col;
		} else if (row == 1) {
		address = 0xC0 + col;
		} else {
		return;
	}
	
	lcdcmd(address);
}

void lcd_clear() {
	lcdcmd(0x01);
	_delay_ms(2);
}
// ============================================================
volatile bool shock = false;
void ISR_shock_detection() {
	int x = read_adc(1);
	int y = read_adc(2);
	int z = read_adc(3);
	
	long xMilliG = map(x, xMin, xMax, -1000, 1000);
	long yMilliG = map(y, yMin, yMax, -1000, 1000);
	long zMilliG = map(z, zMin, zMax, -1000, 1000);
	
	float x_g_value = xMilliG / 1000.0;
	float y_g_value = yMilliG / 1000.0;
	float z_g_value = zMilliG / 1000.0;
	
	if (fabs(x_g_value) >= SHOCK_THRESHOLD ||
	fabs(y_g_value) >= SHOCK_THRESHOLD ||
	fabs(z_g_value) >= SHOCK_THRESHOLD) {
		shock = true;
	}
}
void setup_timer_interrupt(void (*isr_func)(void), uint16_t interval_ms) {
	// Set Timer1 to CTC mode
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);
	
	// Set prescaler to 64
	TCCR1B |= (1 << CS11) | (1 << CS10);
	
	// Calculate and set compare match value
	OCR1A = (F_CPU / 64 / 1000 * interval_ms) - 1;
	
	// Enable Timer1 compare match A interrupt
	TIMSK1 |= (1 << OCIE1A);
	
	// Enable global interrupts
	sei();
}
ISR(TIMER1_COMPA_vect) {
	ISR_shock_detection();
}

int main(void) {
	init_adc();
	uart_init();
	setup_timer_interrupt(ISR_shock_detection, 20);
	//sei();
	int displayState = 0;
	
	
	while (1) {
		if (shock) {
			uart_transmit_string("E:FALL DETECTED!\n");
			shock = false; 
		} else {
		int x = read_adc(1);
		int y = read_adc(2);
		int z = read_adc(3);
		long xMilliG = map(x, xMin, xMax, -1000, 1000);
		long yMilliG = map(y, yMin, yMax, -1000, 1000);
		long zMilliG = map(z, zMin, zMax, -1000, 1000);

		float x_g_value = xMilliG / 1000.0;
		float y_g_value = yMilliG / 1000.0;
		float z_g_value = zMilliG / 1000.0;

		
		
			
			uart_transmit_string("A:");
			uart_transmit_float(x_g_value);
			uart_transmit_string("G,");
			
			uart_transmit_float(y_g_value);
			uart_transmit_string("G,");
			
			uart_transmit_float(z_g_value);
			uart_transmit_string("G");
			uart_transmit('\n');
			_delay_ms(5000);
		}
		
	}
}
