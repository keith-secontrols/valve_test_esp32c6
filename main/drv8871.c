#include "drv8871.h"
#include "board.h"
#include "driver/ledc.h"
#include "esp_log.h"
#include <stdlib.h>

#define PWM_FREQ_HZ    100
#define PWM_RES        LEDC_TIMER_10_BIT
#define PWM_MAX        ((1u << 10) - 1)    /* 1023 */
#define TIMER          LEDC_TIMER_0
#define CH_IN1         LEDC_CHANNEL_0
#define CH_IN2         LEDC_CHANNEL_1
#define SPEED_MODE     LEDC_LOW_SPEED_MODE

static const char *TAG = "drv8871";

void drv8871_init(void)
{
    ledc_timer_config_t timer = {
        .speed_mode      = SPEED_MODE,
        .duty_resolution = PWM_RES,
        .timer_num       = TIMER,
        .freq_hz         = PWM_FREQ_HZ,
        .clk_cfg         = LEDC_AUTO_CLK,
    };
    ESP_ERROR_CHECK(ledc_timer_config(&timer));

    ledc_channel_config_t in1 = {
        .gpio_num   = MOTOR_IN1_GPIO,
        .speed_mode = SPEED_MODE,
        .channel    = CH_IN1,
        .timer_sel  = TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&in1));

    ledc_channel_config_t in2 = {
        .gpio_num   = MOTOR_IN2_GPIO,
        .speed_mode = SPEED_MODE,
        .channel    = CH_IN2,
        .timer_sel  = TIMER,
        .duty       = 0,
        .hpoint     = 0,
    };
    ESP_ERROR_CHECK(ledc_channel_config(&in2));

    ESP_LOGI(TAG, "ready %d Hz 10-bit", PWM_FREQ_HZ);
}

void drv8871_set(int percent)
{
    if (percent >  100) percent =  100;
    if (percent < -100) percent = -100;

    uint32_t duty = (uint32_t)(abs(percent) * PWM_MAX / 100);

    if (percent > 0) {
        ledc_set_duty(SPEED_MODE, CH_IN1, duty);
        ledc_set_duty(SPEED_MODE, CH_IN2, 0);
    } else if (percent < 0) {
        ledc_set_duty(SPEED_MODE, CH_IN1, 0);
        ledc_set_duty(SPEED_MODE, CH_IN2, duty);
    } else {
        ledc_set_duty(SPEED_MODE, CH_IN1, 0);
        ledc_set_duty(SPEED_MODE, CH_IN2, 0);
    }
    ledc_update_duty(SPEED_MODE, CH_IN1);
    ledc_update_duty(SPEED_MODE, CH_IN2);
}

void drv8871_brake(void)
{
    ledc_set_duty(SPEED_MODE, CH_IN1, PWM_MAX);
    ledc_set_duty(SPEED_MODE, CH_IN2, PWM_MAX);
    ledc_update_duty(SPEED_MODE, CH_IN1);
    ledc_update_duty(SPEED_MODE, CH_IN2);
}
