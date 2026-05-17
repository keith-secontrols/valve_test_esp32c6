#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lcd.h"
#include "board.h"

static const char *TAG = "main";

void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare ESP32-C6-LCD-1.47 starting");

    lcd_init();

    lcd_fill(BLACK);
    lcd_backlight(1);
    lcd_draw_rect(10, 10, 162, 60, BLUE);
    lcd_draw_text(18, 22, "Hello, World!", WHITE, BLUE);
    lcd_draw_rect(10, 70, 162, 120, RED);
    lcd_draw_text(18, 82, "0123456789", WHITE, RED);

    ESP_LOGI(TAG, "ready");

    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
