# valve_test_esp32c6

ESP-IDF firmware for the [Waveshare ESP32-C6-LCD-1.47](https://www.waveshare.com/wiki/ESP32-C6-LCD-1.47) development board.

## Hardware

### ESP32-C6 board

| Item | Detail |
|------|--------|
| MCU | ESP32-C6, 160 MHz RISC-V, 4 MB flash |
| Display | ST7789 1.47" TFT, 172×320, RGB565 |
| SPI pins | SCLK=GPIO7, MOSI=GPIO6, CS=GPIO14, DC=GPIO15, RST=GPIO21, BL=GPIO22 |
| Console UART | TX=GPIO16, RX=GPIO17 (requires external USB-UART adapter) |
| USB-C port | ESP32-C6 built-in USB-Serial-JTAG → `/dev/ttyACM0` for flashing |

### AS5600 magnetic angle sensor

| AS5600 pin | Connect to | Notes |
|------------|------------|-------|
| VCC | ESP32 3V3 | 3.0–3.6 V only |
| GND | GND | Common ground |
| SDA | GPIO4 | I2C, internal pull-up enabled in firmware |
| SCL | GPIO5 | I2C 400 kHz |
| DIR | GND | GND = CW increases angle; VCC = CCW increases angle |
| OUT | — | Analog/PWM output, not used (I2C used instead) |
| PGO | — | OTP burn pin, leave disconnected |

### DRV8871 H-bridge motor driver

| DRV8871 pin | Connect to | Notes |
|-------------|------------|-------|
| VM | Motor supply + | 6.5–45 V motor supply |
| GND | GND | Common ground with ESP32 |
| IN1 | GPIO2 | PWM, 20 kHz |
| IN2 | GPIO3 | PWM, 20 kHz |
| OUT1 | Motor terminal A | |
| OUT2 | Motor terminal B | |

## Prerequisites

- [ESP-IDF v5.5](https://docs.espressif.com/projects/esp-idf/en/v5.5/esp32c6/)

## Build and flash

```bash
source ~/esp/esp-idf/export.sh
idf.py set-target esp32c6
idf.py -p /dev/ttyACM0 flash
```

Monitor output appears on the UART pins (GPIO16/17), not the USB-C port.

## API

### LCD (`main/lcd.h`)

```c
void lcd_init(void);
void lcd_backlight(int on);                                   // call after lcd_fill() to avoid GRAM flash
void lcd_fill(uint16_t colour);
void lcd_draw_rect(int x1, int y1, int x2, int y2, uint16_t colour);
void lcd_draw_char(int x, int y, char c, uint16_t fg, uint16_t bg);
void lcd_draw_text(int x, int y, const char *str, uint16_t fg, uint16_t bg);   // 8×8 font
void lcd_draw_char_xl(int x, int y, char c, uint16_t fg, uint16_t bg);
void lcd_draw_text_xl(int x, int y, const char *str, uint16_t fg, uint16_t bg); // 12×16 font
```

Colour helpers: `RGB565(r,g,b)`, `BLACK`, `WHITE`, `RED`, `GREEN`, `BLUE`, `YELLOW`, `CYAN`, `MAGENTA`.

| Function | Size | Font |
|----------|------|------|
| `lcd_draw_text` | 8×8 px | Public-domain IBM VGA (`font8x8.h`) |
| `lcd_draw_text_xl` | 12×16 px | CP437/IBM VGA raster ([idispatch/raster-fonts](https://github.com/idispatch/raster-fonts)) |

### AS5600 (`main/as5600.h`)

```c
void     as5600_init(void);
uint16_t as5600_read_raw(void);      // 0–4095 per revolution
float    as5600_read_degrees(void);  // 0.0–360.0
```

### DRV8871 (`main/drv8871.h`)

```c
void drv8871_init(void);
void drv8871_set(int percent);  // -100 (full reverse) … +100 (full forward); 0 = coast
void drv8871_brake(void);       // active brake (both inputs high)
```

## Notes

- Use a **data-capable** USB-C cable — charge-only cables are common and will not enumerate the USB-Serial-JTAG interface.
- `lcd_backlight(1)` must be called **after** `lcd_fill()` to avoid showing uninitialised GRAM on boot.
- GND must be common between the ESP32, AS5600, and DRV8871.
- The DRV8871 VM pin is the motor supply input only — it does not power the ESP32.
