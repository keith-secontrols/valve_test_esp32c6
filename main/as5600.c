#include "as5600.h"
#include "board.h"
#include "driver/i2c_master.h"
#include "esp_log.h"

#define AS5600_ADDR  0x36
#define REG_ANGLE_H  0x0E  /* filtered 12-bit angle, high nibble */
#define REG_ANGLE_L  0x0F  /* filtered 12-bit angle, low byte    */

static const char *TAG = "as5600";
static i2c_master_bus_handle_t s_bus;
static i2c_master_dev_handle_t s_dev;

void as5600_init(void)
{
    i2c_master_bus_config_t bus_cfg = {
        .i2c_port            = I2C_NUM_0,
        .sda_io_num          = I2C_SDA_GPIO,
        .scl_io_num          = I2C_SCL_GPIO,
        .clk_source          = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt   = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_cfg, &s_bus));

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = AS5600_ADDR,
        .scl_speed_hz    = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(s_bus, &dev_cfg, &s_dev));
    ESP_LOGI(TAG, "ready");
}

uint16_t as5600_read_raw(void)
{
    uint8_t reg = REG_ANGLE_H;
    uint8_t buf[2];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(s_dev, &reg, 1, buf, 2, 10));
    return ((uint16_t)(buf[0] & 0x0F) << 8) | buf[1];
}

float as5600_read_degrees(void)
{
    return as5600_read_raw() * (360.0f / 4096.0f);
}
