#include "lcd_i2c.h"
#include <util/delay.h>

// I2C functions implementation
void i2c_init(void) {
    TWSR = 0x00;
    TWBR = 0x0C;
    TWCR = (1 << TWEN);
}

void i2c_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
    while (TWCR & (1 << TWSTO));
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

// LCD functions implementation
void lcd_init(uint8_t dispAttr) {
    _delay_ms(50);  // Wait for more than 40 ms after VCC rises to 2.7V
    lcd_command(0x33);
    lcd_command(0x32);
    lcd_command(0x28);
    lcd_command(0x0C);
    lcd_command(0x06);
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_clrscr(void) {
    lcd_command(0x01);
    _delay_ms(2);
}

void lcd_gotoxy(uint8_t x, uint8_t y) {
    uint8_t addr = (y == 0) ? 0x80 + x : 0xC0 + x;
    lcd_command(addr);
}

void lcd_puts(const char *s) {
    while (*s) {
        lcd_data(*s++);
    }
}

void lcd_putc(char c) {
    lcd_data(c);
}

void lcd_command(uint8_t cmd) {
    uint8_t high_nibble = (cmd & 0xF0) | LCD_BACKLIGHT;
    uint8_t low_nibble = ((cmd << 4) & 0xF0) | LCD_BACKLIGHT;
    lcd_toggle_enable(high_nibble);
    lcd_toggle_enable(low_nibble);
}

void lcd_data(uint8_t data) {
    uint8_t high_nibble = (data & 0xF0) | LCD_BACKLIGHT | 0x01;
    uint8_t low_nibble = ((data << 4) & 0xF0) | LCD_BACKLIGHT | 0x01;
    lcd_toggle_enable(high_nibble);
    lcd_toggle_enable(low_nibble);
}

void lcd_toggle_enable(uint8_t data) {
    i2c_start();
    i2c_write(I2C_ADDR << 1);
    i2c_write(data | 0x04);
    _delay_us(1);
    i2c_write(data & ~0x04);
    _delay_us(50);
    i2c_stop();
}

void lcd_backlight(uint8_t state) {
    i2c_start();
    i2c_write(I2C_ADDR << 1);
    i2c_write(state);
    i2c_stop();
}
