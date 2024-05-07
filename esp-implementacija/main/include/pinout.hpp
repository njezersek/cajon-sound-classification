#include "driver/gpio.h"

/* i2s peripheral pin configuration */

#define I2S_MCLK        GPIO_NUM_7      // I2S master clock io number
#define I2S_WS          GPIO_NUM_6      // I2S word select io number
#define I2S_BCLK        GPIO_NUM_5      // I2S bit clock io number
#define I2S_DOUT        GPIO_NUM_18     // I2S data out io number
#define I2S_DIN         GPIO_NUM_8     	// I2S data in io number

#define DAC_XSMT 	  	GPIO_NUM_16     // DAC mute pin
#define DAC_FLT 	  	GPIO_NUM_17     // DAC filter pin

#define ADC_CONF_SCL   GPIO_NUM_38		// ADC i2c configuration SCL pin
#define ADC_CONF_SDA   GPIO_NUM_37		// ADC i2c configuration SDA pin


/* WS2812 RGB LED */
#define LED_DATA        GPIO_NUM_48		// LED data pin (built-in on ESP32-S3-DevKitC-1)