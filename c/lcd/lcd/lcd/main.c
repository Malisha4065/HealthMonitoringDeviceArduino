/*
 * lcd.c
 *
 * Created: 6/24/2024 2:46:56 AM
 * Author : Malisha A.P.D.
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

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

void lcd_init() {
	DDRD = 0xFF;
	DDRB = 0xFF;
	PORTB &= ~(1<<EN);
	
	lcdcmd(0x33);
	lcdcmd(0x32);
	lcdcmd(0x28);
	lcdcmd(0x0E);
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
	
	// Calculate the DDRAM address based on the row and column
	if (row == 0) {
		address = 0x80 + col;  // First row starts at 0x80
		} else if (row == 1) {
		address = 0xC0 + col;  // Second row starts at 0xC0
		} else {
		// Handle invalid row (assuming a 2-row display)
		return;
	}
	
	// Send the Set DDRAM Address command
	lcdcmd(address);
}

void lcd_clear() {
	lcdcmd(0x01);  // Send clear display command
	_delay_ms(2);  // Wait for the LCD to process the command
}

int main(void)
{
	lcd_init();
	
	lcd_set_cursor(0, 0);  // Set cursor to first row, first column
	lcd_print("Hello AVR");
	lcd_set_cursor(1, 5);  // Set cursor to second row, sixth column
	lcd_print("World!");
	_delay_ms(5000);
    /* Replace with your application code */
    while (1) 
    {
		lcd_clear();
		lcd_print("yes");
		_delay_ms(1000);
		lcd_clear();
		lcd_print("no");
		_delay_ms(1000);
    }
}

