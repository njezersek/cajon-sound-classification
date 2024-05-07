#pragma once
#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#define MAX_BUFFER_SIZE 10000

namespace beat_detector{
	// onset beat detector settings
	extern uint32_t dynamic_threshold;
	extern uint32_t static_threshold;
	extern uint32_t cooldown_period;
	extern uint32_t cooldown_counter;
	extern uint32_t circular_buffer_size;

	extern bool beat_detected;

	extern uint32_t queue_end;
	extern int32_t* buffer;
	extern int32_t* debug_buffer;
	extern SemaphoreHandle_t buffer_mutex;

	void resize_circular_buffer(uint32_t new_size);

	void init();
	void update(int32_t sample);
}