#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s_std.h"
#include "driver/gpio.h"
#include "esp_check.h"
#include "esp_system.h"
#include "sdkconfig.h"
#include <math.h>
#include "esp_timer.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "led_strip.h"
#include "esp_dsp.h"

#include "utils.hpp"

#include "task_sound_io.hpp"
#include "beat_detector.hpp"
#include "task_serial_io.hpp"
#include "task_led_driver.hpp"
#include "fft.hpp"
#include "nn_classifier.hpp"

#include "tensorflow/lite/micro/micro_log.h"

static const char *TAG = "main";


extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting");

    MicroPrintf("Hello world!\n ");

    nn_classifier::init();    
    beat_detector::init();
    fft::init();

    xTaskCreatePinnedToCore(task_sound_io, "sound_io", 8192, NULL, 5, NULL, 0);
    // xTaskCreatePinnedToCore(task_serial_io, "input_task", 8192, NULL, 1, NULL, 0);
    // xTaskCreatePinnedToCore(task_led_driver, "led_driver", 8192, NULL, 1, NULL, 0);
}
