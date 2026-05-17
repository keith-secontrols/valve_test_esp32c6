# valve_test_esp32c6

ESP-IDF firmware for the [Waveshare ESP32-C6-LCD-1.47](https://www.waveshare.com/wiki/ESP32-C6-LCD-1.47) development board.

## Hardware

| Item | Detail |
|------|--------|
| MCU | ESP32-C6, 160 MHz RISC-V, 4 MB flash |
| Display | ST7789 1.47" TFT, 172×320, RGB565 |
| SPI pins | SCLK=GPIO7, MOSI=GPIO6, CS=GPIO14, DC=GPIO15, RST=GPIO21, BL=GPIO22 |
| Console UART | TX=GPIO16, RX=GPIO17 (requires external USB-UART adapter) |
| USB-C port | ESP32-C6 built-in USB-Serial-JTAG → `/dev/ttyACM0` for flashing |

## Prerequisites

- [ESP-IDF v5.5](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32c6/)

## Build and flash

```bash
source ~/esp/esp-idf/export.sh
idf.py set-target esp32c6
idf.py -p /dev/ttyACM0 flash
```

Monitor output appears on the UART pins (GPIO16/17), not the USB-C port.

## LCD driver API

Declared in `main/lcd.h`:

```c
void lcd_init(void);                                          // initialise SPI + ST7789, backlight stays OFF
void lcd_backlight(int on);                                   // turn backlight on (1) or off (0)
void lcd_fill(uint16_t colour);                               // fill entire screen
void lcd_draw_rect(int x1, int y1, int x2, int y2,
                   uint16_t colour);                          // filled rectangle
void lcd_draw_char(int x, int y, char c,
                   uint16_t fg, uint16_t bg);                 // single 8×8 character
void lcd_draw_text(int x, int y, const char *str,
                   uint16_t fg, uint16_t bg);                 // string, 8 px per character
```

Colour helpers (RGB565):

```c
RGB565(r, g, b)   // build a colour from 8-bit components
BLACK, WHITE, RED, GREEN, BLUE, YELLOW, CYAN, MAGENTA
```

Font is the public-domain IBM 8×8 VGA bitmap font (full printable ASCII).

## Notes

- Use a **data-capable** USB-C cable — charge-only cables are common and will not enumerate the USB-Serial-JTAG interface.
- `lcd_backlight(1)` should be called **after** `lcd_fill()` to avoid a brief flash of uninitialised GRAM content on boot.
- `lcd_fill` sends a synchronous NOP command after the fill loop to ensure the last DMA transaction has completed before returning.
