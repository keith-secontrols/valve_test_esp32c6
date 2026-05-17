#include "lcd.h"
#include "font8x8.h"
#include "font12x16.h"
#include "board.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "lcd";
static esp_lcd_panel_handle_t s_panel;
static esp_lcd_panel_io_handle_t s_io;
static uint16_t s_line[LCD_H_RES];
static uint16_t s_char_buf[8 * 8];
static uint16_t s_char_xl_buf[12 * 16];

void lcd_init(void)
{
    gpio_config_t bl_cfg = {
        .pin_bit_mask = BIT64(LCD_BL_GPIO),
        .mode = GPIO_MODE_OUTPUT,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&bl_cfg));
    gpio_set_level(LCD_BL_GPIO, 0);

    spi_bus_config_t buscfg = {
        .mosi_io_num     = LCD_MOSI_GPIO,
        .miso_io_num     = -1,
        .sclk_io_num     = LCD_SCLK_GPIO,
        .quadwp_io_num   = -1,
        .quadhd_io_num   = -1,
        .max_transfer_sz = LCD_H_RES * LCD_V_RES * 2,
    };
    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    esp_lcd_panel_io_spi_config_t io_cfg = {
        .dc_gpio_num      = LCD_DC_GPIO,
        .cs_gpio_num      = LCD_CS_GPIO,
        .pclk_hz          = LCD_CLK_HZ,
        .lcd_cmd_bits     = 8,
        .lcd_param_bits   = 8,
        .spi_mode         = 0,
        .trans_queue_depth = 1,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI2_HOST, &io_cfg, &s_io));

    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num  = LCD_RST_GPIO,
        .rgb_ele_order   = LCD_RGB_ELEMENT_ORDER_BGR,
        .bits_per_pixel  = 16,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(s_io, &panel_cfg, &s_panel));

    ESP_ERROR_CHECK(esp_lcd_panel_reset(s_panel));
    ESP_ERROR_CHECK(esp_lcd_panel_init(s_panel));
    /* NORON exits partial/scroll mode — ESP-IDF generic driver omits this */
    esp_lcd_panel_io_tx_param(s_io, 0x13, NULL, 0);
    /* ST7789 colour inversion is required for most modules */
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(s_panel, true));
    /* Offset to centre the 172-pixel-wide panel in the 240-wide controller */
    ESP_ERROR_CHECK(esp_lcd_panel_set_gap(s_panel, LCD_X_OFFSET, LCD_Y_OFFSET));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(s_panel, true));

    ESP_LOGI(TAG, "ready %dx%d", LCD_H_RES, LCD_V_RES);
}

void lcd_backlight(int on)
{
    gpio_set_level(LCD_BL_GPIO, on ? 1 : 0);
}

void lcd_fill(uint16_t colour)
{
    for (int i = 0; i < LCD_H_RES; i++) s_line[i] = colour;
    for (int y = 0; y < LCD_V_RES; y++)
        esp_lcd_panel_draw_bitmap(s_panel, 0, y, LCD_H_RES, y + 1, s_line);
    /* tx_param is synchronous and waits for the last queued draw_bitmap to
       complete before returning — ensures fill is fully written before caller
       turns on the backlight */
    esp_lcd_panel_io_tx_param(s_io, 0x00, NULL, 0);
}

void lcd_draw_rect(int x1, int y1, int x2, int y2, uint16_t colour)
{
    if (x2 <= x1 || y2 <= y1) return;
    int w = x2 - x1;
    for (int i = 0; i < w; i++) s_line[i] = colour;
    for (int y = y1; y < y2; y++)
        esp_lcd_panel_draw_bitmap(s_panel, x1, y, x2, y + 1, s_line);
}

void lcd_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg)
{
    uint8_t idx = (uint8_t)c;
    if (idx >= 128) idx = '?';
    esp_lcd_panel_io_tx_param(s_io, 0x00, NULL, 0);
    for (int row = 0; row < 8; row++)
        for (int col = 0; col < 8; col++)
            s_char_buf[row * 8 + col] = (font8x8[idx][row] >> col) & 1 ? fg : bg;
    esp_lcd_panel_draw_bitmap(s_panel, x, y, x + 8, y + 8, s_char_buf);
}

void lcd_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg)
{
    while (*str) {
        lcd_draw_char(x, y, *str++, fg, bg);
        x += 8;
    }
}


void lcd_draw_char_xl(int x, int y, char c, uint16_t fg, uint16_t bg)
{
    uint8_t idx = (uint8_t)c;
    esp_lcd_panel_io_tx_param(s_io, 0x00, NULL, 0);
    for (int row = 0; row < 16; row++) {
        uint16_t bits = ((uint16_t)font12x16[idx][row * 2] << 8) | font12x16[idx][row * 2 + 1];
        for (int col = 0; col < 12; col++)
            s_char_xl_buf[row * 12 + col] = (bits >> (15 - col)) & 1 ? fg : bg;
    }
    esp_lcd_panel_draw_bitmap(s_panel, x, y, x + 12, y + 16, s_char_xl_buf);
}

void lcd_draw_text_xl(int x, int y, const char *str, uint16_t fg, uint16_t bg)
{
    while (*str) {
        lcd_draw_char_xl(x, y, *str++, fg, bg);
        x += 12;
    }
}
