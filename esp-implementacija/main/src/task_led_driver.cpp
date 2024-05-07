#include "task_led_driver.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_check.h"
#include "led_strip.h"

#include "pinout.hpp"
#include "sample_player.hpp"
#include "utils.hpp"

static const char *TAG = "led_driver";

void task_led_driver(void *args){
    // LED Strip object handle
    led_strip_handle_t led_strip;

    // LED strip general initialization, according to your led board design
    led_strip_config_t strip_config = {
        .strip_gpio_num = LED_DATA,   // The GPIO that connected to the LED strip's data line
        .max_leds = 1,        // The number of LEDs in the strip,
        .led_pixel_format = LED_PIXEL_FORMAT_GRB, // Pixel format of your LED strip
        .led_model = LED_MODEL_WS2812,            // LED strip model
        .flags = {
            .invert_out = false
        }                // whether to invert the output signal
    };

    // LED strip backend configuration: RMT
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,        // different clock source can lead to different power consumption
        .resolution_hz = (10 * 1000 * 1000),    // RMT counter clock frequency
        .flags = {
            .with_dma = false
        }               // DMA feature is available on ESP target like ESP32-S3
    };
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip));
    ESP_LOGI(TAG, "Created LED strip object with RMT backend");


    ESP_LOGI(TAG, "Start blinking LED strip");
    while (1) {
        /* Set the LED pixel using RGB from 0 (0%) to 255 (100%) for each color */
        uint8_t v = CLAMP((int32_t)255 - (int32_t)sample_player::pos/4, 0, 255);
        if(sample_player::sample_type == 0){ // kick
            ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, v, v/2, 0));
        }
        else{ // snare
            ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, 0, 0, v/2, v/2));
        }
        /* Refresh the strip to send data */
        ESP_ERROR_CHECK(led_strip_refresh(led_strip));
    }
}