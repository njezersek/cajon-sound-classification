#include "beat_detector.hpp"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_system.h"

#include "sample_player.hpp"
#include "fft.hpp"
#include "simple_classifier.hpp"
#include "nn_classifier.hpp"

static const char *TAG = "beat_detector";

using namespace beat_detector;

/* The signal has to surpass the running average by this factor register a beat (/10) */
uint32_t beat_detector::dynamic_threshold = 45;

/* The signal has to be greater than this value for any beats to be registered (*10000) */
uint32_t beat_detector::static_threshold = 500;

/* The number of samples to wait before registering another beat */
uint32_t beat_detector::cooldown_period = 44100 / 20;

/* The number of samples to keep in the circular buffer (read only) */
uint32_t beat_detector::circular_buffer_size = 2000;

uint32_t beat_detector::queue_end = 0;
int32_t* beat_detector::buffer = NULL;
int32_t* beat_detector::debug_buffer = NULL;
SemaphoreHandle_t beat_detector::buffer_mutex = xSemaphoreCreateMutex();

static int64_t running_sum = 0;
static int32_t running_avg = 0;

uint32_t beat_detector::cooldown_counter = 0;

bool beat_detector::beat_detected = false;

static int32_t update_circular_buffer(int32_t sample){
	// xSemaphoreTake(beat_detector::buffer_mutex, portMAX_DELAY);
	int32_t oldest_sample = beat_detector::buffer[beat_detector::queue_end];
    buffer[beat_detector::queue_end] = sample;
    beat_detector::queue_end++;
    if(beat_detector::queue_end == beat_detector::circular_buffer_size){
        beat_detector::queue_end = 0;
    }
	// xSemaphoreGive(beat_detector::buffer_mutex);
	return oldest_sample;
}

static void update_running_avg(int32_t old_sample, int32_t new_sample){
	running_sum -= abs(old_sample); // remove oldest sample from running sum
	running_sum += abs(new_sample); // add newest sample to running sum
	running_avg = running_sum / beat_detector::circular_buffer_size;
}

static void update_cooldown_counter(){
	beat_detector::cooldown_counter++;
	if(beat_detector::cooldown_counter > beat_detector::cooldown_period){
		beat_detector::cooldown_counter = beat_detector::cooldown_period;
	}
}

void beat_detector::resize_circular_buffer(uint32_t new_size){
	// xSemaphoreTake(beat_detector::buffer_mutex, portMAX_DELAY);
	if(new_size > MAX_BUFFER_SIZE){
        new_size = MAX_BUFFER_SIZE;
    }
    if(new_size < 1){
        new_size = 1;
    }
    beat_detector::circular_buffer_size = new_size;
    queue_end = 0;
    memset(buffer, 0, sizeof(int32_t) * MAX_BUFFER_SIZE);
    running_sum = 0;
	// xSemaphoreGive(beat_detector::buffer_mutex);
}

void beat_detector::init(){
	buffer = (int32_t*)calloc(sizeof(int32_t), MAX_BUFFER_SIZE);
	debug_buffer = (int32_t*)calloc(sizeof(int32_t), MAX_BUFFER_SIZE);
    resize_circular_buffer(beat_detector::circular_buffer_size);

	ESP_LOGI(TAG, "Beat detector initialized");
}

void beat_detector::update(int32_t sample){
	int32_t old_sample = update_circular_buffer(sample);
	update_running_avg(old_sample, sample);

	if(	abs(sample) > running_avg * (dynamic_threshold/10.0) && 
		abs(sample) > static_threshold*10000 && 
		cooldown_counter >= cooldown_period
	){ // beat detected
		cooldown_counter = 0;

		// sample_player::play(1);

		beat_detected = true;

		// xTaskNotify(task_beat_classifier_handle, 0, eNoAction);
	}

	if(cooldown_counter == FFT_WINDOW_SIZE){
		// copy a part of audio data to the FFT input buffer
		xSemaphoreTake(buffer_mutex, portMAX_DELAY);
		for(int i = 0; i < FFT_WINDOW_SIZE; i++){
			int index = (queue_end + i + circular_buffer_size - FFT_WINDOW_SIZE) % circular_buffer_size;
			fft::x[i] = buffer[index] / 2e8;
			// printf("%ld ", beat_detector::buffer[index]);
		}
		xSemaphoreGive(buffer_mutex);
		// fft::transform();
		int sample_type = nn_classifier::transform(fft::x, FFT_WINDOW_SIZE);
		// float sample_type = simple_classifier::transform(fft::x, FFT_WINDOW_SIZE/2);
		sample_player::play(sample_type);


		// copy the rolling buffer to the debug buffer
		for(int i = 0; i < circular_buffer_size; i++){
			int index = (queue_end + i + circular_buffer_size) % circular_buffer_size;
			debug_buffer[i] = buffer[index];
		}
	}

	update_cooldown_counter();
}
