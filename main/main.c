#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lcd.h"
#include "drv8871.h"
#include "board.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare ESP32-C6-LCD-1.47 starting");

    drv8871_init();
    drv8871_set(3);
    vTaskDelay(pdMS_TO_TICKS(6000));
    drv8871_set(-3);
    vTaskDelay(pdMS_TO_TICKS(6000));
    drv8871_set(0);

    lcd_init();

    lcd_fill(BLACK);
    lcd_backlight(1);
    lcd_draw_rect(10, 10, 162, 36, BLUE);
    lcd_draw_text(18, 18, "Small 8x8", WHITE, BLUE);
    lcd_draw_rect(10, 50, 162, 98, GREEN);
    lcd_draw_text_xl(14, 57, "Large 12x16", BLACK, GREEN);

    ESP_LOGI(TAG, "ready");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
