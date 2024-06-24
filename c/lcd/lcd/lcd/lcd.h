#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

// Pin definitions
#define LCD_DATA_PORT PORTD
#define LCD_DATA_DDR  DDRD
#define LCD_RS        PB4
#define LCD_EN        PB3
#define LCD_D4        PD5
#define LCD_D5        PD4
#define LCD_D6        PD3
#define LCD_D7        PD2

// Function prototypes
void LCD_Init(void);
void LCD_Command(uint8_t cmd);
void LCD_Char(char data);
void LCD_String(const char *str);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_CreateChar(uint8_t location, uint8_t *pattern);
void LCD_Integer(int num);

// Helper function to pulse Enable pin
static void LCD_Pulse_EN(void)
{
    PORTB |= (1 << LCD_EN);
    _delay_us(1);
    PORTB &= ~(1 << LCD_EN);
    _delay_us(100);
}

// Helper function to write 4-bits to LCD
static void LCD_Write4Bits(uint8_t data)
{
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | (data & 0xF0);
    LCD_Pulse_EN();
    _delay_us(40);
    LCD_DATA_PORT = (LCD_DATA_PORT & 0x0F) | ((data << 4) & 0xF0);
    LCD_Pulse_EN();
    _delay_us(40);
}

void LCD_Command(uint8_t cmd)
{
    PORTB &= ~(1 << LCD_RS);    // RS = 0 for command
    LCD_Write4Bits(cmd);
    _delay_us(40);
}

void LCD_Char(char data)
{
    PORTB |= (1 << LCD_RS);     // RS = 1 for data
    LCD_Write4Bits(data);
    _delay_us(40);
}

void LCD_Init(void)
{
    // Set data pins as output
    LCD_DATA_DDR |= (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);
    // Set RS and EN pins as output
    DDRB |= (1 << LCD_RS) | (1 << LCD_EN);
    
    _delay_ms(50);  // Wait for LCD to power up
    
    // Initialize in 4-bit mode
    LCD_Write4Bits(0x30);
    _delay_ms(5);
    LCD_Write4Bits(0x30);
    _delay_us(100);
    LCD_Write4Bits(0x30);
    _delay_us(100);
    LCD_Write4Bits(0x20);  // Set 4-bit mode
    _delay_us(100);
    
    LCD_Command(0x28);  // Function set: 4-bit mode, 2 lines, 5x8 font
    LCD_Command(0x0C);  // Display on, cursor off, blink off
    LCD_Command(0x06);  // Entry mode set: Increment cursor, no display shift
    LCD_Command(0x01);  // Clear display
    _delay_ms(2);
}

void LCD_String(const char *str)
{
    while (*str)
    {
        LCD_Char(*str++);
    }
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
    LCD_Command(0x80 | (col + row_offsets[row]));
}

void LCD_Clear(void)
{
    LCD_Command(0x01);
    _delay_ms(2);
}

void LCD_Home(void)
{
    LCD_Command(0x02);
    _delay_ms(2);
}

void LCD_CreateChar(uint8_t location, uint8_t *pattern)
{
    location &= 0x7;  // Only locations 0-7 are allowed
    LCD_Command(0x40 | (location << 3));
    for (uint8_t i = 0; i < 8; i++)
    {
        LCD_Char(pattern[i]);
    }
}

void LCD_Integer(int num)
{
    char buffer[16];
    itoa(num, buffer, 10);
    LCD_String(buffer);
}
