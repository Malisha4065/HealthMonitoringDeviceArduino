#ifndef LCD_I2C_H_INCLUDED
#define LCD_I2C_H_INCLUDED

#include <avr/io.h>

// I2C address of the PCF8574
#define I2C_ADDR 0x27

// Control bit for the backlight
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

// I2C functions
void i2c_init(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_write(uint8_t data);

// LCD functions
void lcd_init(uint8_t dispAttr);
void lcd_clrscr(void);
void lcd_gotoxy(uint8_t x, uint8_t y);
void lcd_puts(const char *s);
void lcd_putc(char c);
void lcd_command(uint8_t cmd);
void lcd_data(uint8_t data);
void lcd_toggle_enable(uint8_t data);
void lcd_backlight(uint8_t state);

#endif // LCD_I2C_H
