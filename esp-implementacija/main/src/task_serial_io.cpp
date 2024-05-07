#include "task_serial_io.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_check.h"
#include "esp_system.h"
#include <string.h>

#include "fft.hpp"
#include "nn_classifier.hpp"
#include "simple_classifier.hpp"
#include "beat_detector.hpp"
#include "utils.hpp"

static const char *TAG = "serial_io";

void task_serial_io(void *args){
	ESP_LOGI(TAG, "Task start");

    char input[64];

    while (1) {
        if (scanf("%1s", input)  == 1){
            bool print_info = true;
            if(strcmp(input, "g") == 0){
                // copy sound data to fft::x
                xSemaphoreTake(beat_detector::buffer_mutex, portMAX_DELAY);
                for(int i = 0; i < FFT_WINDOW_SIZE; i++){
                    int index = (beat_detector::queue_end + i + beat_detector::circular_buffer_size - FFT_WINDOW_SIZE) % beat_detector::circular_buffer_size;
                    fft::x[i] = beat_detector::buffer[index] / 2e6;
                }
                xSemaphoreGive(beat_detector::buffer_mutex);
                fft::transform();
                fft::print_last_result();
                print_info = false;
            }
            if(strcmp(input, "n") == 0){
                // print the debug buffer to serial
                for(int i = 0; i < beat_detector::circular_buffer_size; i++){
                    printf("%ld ", beat_detector::debug_buffer[i]);
                }
                printf("\n");

                // // print the fft input buffer to serial
                // for(int i = 0; i < FFT_WINDOW_SIZE; i++){
                //     fft::x[i] = beat_detector::debug_buffer[beat_detector::circular_buffer_size - FFT_WINDOW_SIZE + i] / 2e6;
                //     printf("%f ", fft::x[i]);
                // }
                // printf("\n");

                // print the nn input buffer to serial
                for(int i = 0; i < NN_INPUT_SIZE; i++){
                    printf("%f ", nn_classifier::input_buffer_debug[i]);
                }
                printf("\n");

                fft::transform();
                float prediction = simple_classifier::transform(fft::x, FFT_WINDOW_SIZE/2);
                // int prediction = nn_classifier::transform(fft::x, FFT_WINDOW_SIZE/2);

                // print the fft output buffer to serial
                for(int i = 0; i < FFT_WINDOW_SIZE/2; i++){
                    printf("%f ", fft::x[i]);
                }
                printf("\n");

                printf("Prediction: %f\n", prediction);

                print_info = false;
            }
            if(strcmp(input, "h") == 0){
                fft::print_last_result();
            }
            if(strcmp(input, "q") == 0){
                beat_detector::dynamic_threshold += 1;
            }
            else if(strcmp(input, "a") == 0){
                beat_detector::dynamic_threshold -= 1;
            }
            else if(strcmp(input, "w") == 0){
                beat_detector::static_threshold += 1;
            }
            else if(strcmp(input, "s") == 0){
                beat_detector::static_threshold -= 1;
            }
            else if(strcmp(input, "e") == 0){
                beat_detector::resize_circular_buffer(beat_detector::circular_buffer_size + 100);
            }
            else if(strcmp(input, "d") == 0){
                beat_detector::resize_circular_buffer(beat_detector::circular_buffer_size - 100);
            }
            else if(strcmp(input, "r") == 0){
                simple_classifier::threshold += 0.001;
            }
            else if(strcmp(input, "f") == 0){
                simple_classifier::threshold -= 0.001;
            }
            beat_detector::dynamic_threshold = CLAMP(beat_detector::dynamic_threshold, 0, 100);
            beat_detector::static_threshold = CLAMP(beat_detector::static_threshold, 0, 500);
            simple_classifier::threshold = CLAMP(simple_classifier::threshold, 0, 1);
            if(print_info){
                printf("DT: %.1f \t ST: %ld \t QS: %ld \tT: %.4f \n", beat_detector::dynamic_threshold/10.0, beat_detector::static_threshold, beat_detector::circular_buffer_size, simple_classifier::threshold);
            }
        }

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}