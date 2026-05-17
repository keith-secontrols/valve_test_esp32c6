#pragma once
#include <stdint.h>

/* RGB565 colour helpers */
#define RGB565(r, g, b) ((((r) & 0xF8) << 8) | (((g) & 0xFC) << 3) | ((b) >> 3))
#define BLACK   0x0000
#define WHITE   0xFFFF
#define RED     0xF800
#define GREEN   0x07E0
#define BLUE    0x001F
#define YELLOW  0xFFE0
#define CYAN    0x07FF
#define MAGENTA 0xF81F

void lcd_init(void);
void lcd_backlight(int on);
void lcd_fill(uint16_t colour);
void lcd_draw_rect(int x1, int y1, int x2, int y2, uint16_t colour);
void lcd_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg);
void lcd_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg);
void lcd_draw_char_xl(int x, int y, char c, uint16_t fg, uint16_t bg);  /* 12x16, native */
void lcd_draw_text_xl(int x, int y, const char *str, uint16_t fg, uint16_t bg);
