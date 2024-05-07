#include "task_sound_io.hpp"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_check.h"
#include "esp_system.h"
#include "driver/i2s_std.h"
#include "driver/i2c.h"
#include "driver/gpio.h"

#include "pinout.hpp"
#include "beat_detector.hpp"
#include "sample_player.hpp"
#include "adc_config.hpp"

#define AUDIO_BUFFER_SIZE 32 

static const char *TAG = "sound_io";

static i2s_chan_handle_t                tx_chan;        // I2S tx channel handler
static i2s_chan_handle_t                rx_chan;        // I2S rx channel handler

static const char err_reason[][30] = {
	"input param is invalid",
    "operation timeout"
};


static int t = 0;
static int32_t test_frequency_generator(){
    float f = 440 * 2;
    int period_samples = 44100 / f;
    int max_amplitude = INT32_MAX;
    t++;
    t %= period_samples;
    return (int32_t)(max_amplitude * sin(2 * M_PI * t / period_samples));
}

/*
	Init I2S for sound input and output.
*/
static void i2s_init_std_duplex(){
    i2s_chan_config_t chan_cfg = {
        .id = I2S_NUM_AUTO,
        .role = I2S_ROLE_MASTER,
        .dma_desc_num = 4, // The number of DMA descriptors (buffers)
        .dma_frame_num = 48*2, // A frame is a set of samples for all channels. (frame_size_bits = bits_per_sample * channels)
        .auto_clear = false
    };

    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &tx_chan, &rx_chan));


    i2s_std_config_t std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(44100),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        // .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        // .slot_cfg = I2S_STD_PCM_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_MCLK,    // some codecs may require mclk signal, this example doesn't need it
            .bclk = I2S_BCLK,
            .ws   = I2S_WS,
            .dout = I2S_DOUT,
            .din  = I2S_DIN, // In duplex mode, bind output and input to a same gpio can loopback internally
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };

    /* Initialize the channels */
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(rx_chan, &std_cfg));

	ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));
    ESP_ERROR_CHECK(i2s_channel_enable(rx_chan));
}

static void init_dac(){
    // Initialize PCM5102A DAC
    gpio_set_direction(DAC_XSMT, GPIO_MODE_OUTPUT);
    gpio_set_direction(DAC_FLT, GPIO_MODE_OUTPUT);
    gpio_set_level(DAC_XSMT, 1); // LOW soft mute, HIGH soft un-mute
    gpio_set_level(DAC_FLT, 1); // LOW normal latency filter, HIGH low latency filter
}

/*
	FreeRTOS task that handles sound input and output.
*/
void task_sound_io(void *args){
    ESP_LOGI(TAG, "Task start");

    adc_config::init();
    adc_config::set_page(0);
    adc_config::setup();
    adc_config::print_page_hex();

    init_dac();

	i2s_init_std_duplex();

    int32_t *audio_data = (int32_t*)calloc(1, AUDIO_BUFFER_SIZE * sizeof(int32_t));
    if (!audio_data) {
        ESP_LOGE(TAG, "No memory for audio IO data buffer");
        abort();
    }
    esp_err_t ret = ESP_OK;
    size_t bytes_read = 0;
    size_t bytes_write = 0;

    while (1) {
        /* Read sample data from mic */
        ret = i2s_channel_read(rx_chan, audio_data, AUDIO_BUFFER_SIZE * sizeof(int32_t), &bytes_read, 1000);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "i2s read failed, %s", err_reason[ret == ESP_ERR_TIMEOUT]);
            abort();
        }

        for(int i = 0; i < AUDIO_BUFFER_SIZE; i += 2){
            int32_t left_sample = audio_data[i];
			int32_t right_sample = audio_data[i+1];

			// beat_detector::update(left_sample);
			// int32_t sample = sample_player::update();

            int32_t sample2 = test_frequency_generator();

            // printf("%ld ", left_sample);

            int32_t sample = left_sample / 2 + right_sample / 2;

			audio_data[i] = sample2 / 4;
			audio_data[i+1] = sample2 / 4;
        }
        // printf("\n");

        /* Write sample data to earphone */
        ret = i2s_channel_write(tx_chan, audio_data, AUDIO_BUFFER_SIZE * sizeof(int32_t), &bytes_write, 1000);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "i2s write failed, %s", err_reason[ret == ESP_ERR_TIMEOUT]);
            abort();
        }
        if (bytes_read != bytes_write) {
            ESP_LOGW(TAG, "%d bytes read but only %d bytes are written", bytes_read, bytes_write);
        }
    }
    vTaskDelete(NULL);
}