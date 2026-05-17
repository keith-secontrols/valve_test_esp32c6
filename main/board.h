#pragma once

/* ── LCD (ST7789, SPI) ───────────────────────────────────── */
#define LCD_SCLK_GPIO   7
#define LCD_MOSI_GPIO   6
#define LCD_CS_GPIO     14
#define LCD_DC_GPIO     15
#define LCD_RST_GPIO    21
#define LCD_BL_GPIO     22
#define LCD_H_RES       172
#define LCD_V_RES       320
#define LCD_CLK_HZ      (40 * 1000 * 1000)

/* ST7789 controller native width is 240; panel is 172, centred */
#define LCD_X_OFFSET    ((240 - LCD_H_RES) / 2)
#define LCD_Y_OFFSET    0

/* ── Console UART (GPIO 16/17, see sdkconfig.defaults) ───── */
#define UART_TX_GPIO    16
#define UART_RX_GPIO    17

/* ── DRV8871 H-bridge (PWM) ──────────────────────────────── */
#define MOTOR_IN1_GPIO  2
#define MOTOR_IN2_GPIO  3

/* ── I2C bus (AS5600 angle sensor) ───────────────────────── */
#define I2C_SDA_GPIO    4
#define I2C_SCL_GPIO    5
