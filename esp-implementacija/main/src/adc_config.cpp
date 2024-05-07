#include "adc_config.hpp"

#include "driver/i2c.h"
#include "driver/gpio.h"

#include "pinout.hpp"


void adc_config::init(){
    // Initialize PCM3120-Q1 ADC
    // setup i2c
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = ADC_CONF_SDA,
        .scl_io_num = ADC_CONF_SCL,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master = {
            .clk_speed = 100000
        }
    };

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);
}

void adc_config::write(uint8_t reg_addr, uint8_t data){
    uint8_t write_buffer[2] = {reg_addr, data};
    i2c_master_write_to_device(I2C_NUM_0, ADC_CONF_ADDR, write_buffer, 2, 100);
}

uint8_t adc_config::read(uint8_t reg_addr){
    uint8_t write_buffer[1] = {reg_addr};
    uint8_t read_buffer[1] = {0};
    i2c_master_write_read_device(I2C_NUM_0, ADC_CONF_ADDR, write_buffer, 1, read_buffer, 1, 100);
    return read_buffer[0];
}

void adc_config::set_page(uint8_t page){
    write(Registers::PAGE_CFG, page);
}

void adc_config::print_page_hex(){
    uint8_t write_buffer[1] = {0x00};
    uint8_t read_buffer[256];
    i2c_master_write_read_device(I2C_NUM_0, ADC_CONF_ADDR, write_buffer, 1, read_buffer, 256, 100);

    printf("ADC Configuration: \n      ");
    for(int i = 0; i < 16; i++){
        printf("0x%02x  ", i);
    }
    for (int i = 0; i < 256; i++){
        if(i % 16 == 0){
            printf("\n");
            printf("0x%02x: ", i);
        }
        printf("0x%02x  ", read_buffer[i]);
    }
    printf("\n");
}

void adc_config::setup(){
	// configure power and sleep settings
	{
		uint8_t val = 0x00;
		val |= 1 << 7; // Analog power supply selection: 0 = external 1.8V on AVDD, 1 = internal 1.8V
		val |= 0 << 3; // Duration of quick-charge for the VREF ext. cap.: 0 = 3.5ms, 1 = 10ms, 2 = 50ms, 3 = 100ms 
		val |= 0 << 2; // i2c brodcast mode: 0 = disabled, 1 = enabled
		val |= 1 << 0; // sleep mode: 0 = enabled, 1 = disabled
		write(Registers::SLEEP_CFG, val);
	}

	// configure i2s format
	{
		uint8_t val = 0x30;
		val |= 1 << 6; // 0 = TDM, 1 = I2S, 2 = left-justified
		val |= 2 << 4; // 0 = 16bit, 1 = 20bit, 2 = 24bit, 3 = 32bit
		write(Registers::ASI_CFG0, val);
	}

	// // configure clock source
	// {
	// 	uint8_t val = 0x10;
	// 	val |= 1 << 7; // 0 = BCLK is used as root clock, 1 = MCLK is used as root clock
	// 	write(Registers::CLK_SRC, val);
	// }

	// configure GPIO1
	{
		uint8_t val = 0x00;
		val |= 10 << 4; // use as MCLK input
		write(Registers::GPIO_CFG0, val);
	}

	// configure CH1 gain
	{
		uint8_t val = 0x00;
		val |= 80 << 1; // 0 = 0dB, 1 = 0.5dB, ..., 84 = 42dB
		write(Registers::CH1_CFG1, val);
	}
	// configure CH1 digital volume
	{
		uint8_t val = 201; // 0 = muted, 1 = -100dB, ..., 201 = 0dB, ... 255 = 27dB
		write(Registers::CH1_CFG2, val);
	}

	// configure CH2 gain
	{
		uint8_t val = 0x00;
		val |= 0 << 1; // 0 = 0dB, 1 = 0.5dB, ..., 84 = 42dB
		write(Registers::CH2_CFG1, val);
	}
	// configure CH2 digital volume
	{
		uint8_t val = 201; // 0 = muted, 1 = -100dB, ..., 201 = 0dB, ... 255 = 27dB
		write(Registers::CH2_CFG2, val);
	}


	// configure DSP
	{
		uint8_t val = 0x01;
		write(Registers::DSP_CFG0, val);
	}

	// configure input chanel enable
	{
		uint8_t val = 0x00;
		val |= 1 << 7; // CH1
		val |= 1 << 6; // CH2
		val |= 0 << 5; // CH3
		val |= 0 << 4; // CH4
		write(Registers::IN_CH_EN, val);
	}

	// configure output channel enable
	{
		uint8_t val = 0x00;
		val |= 1 << 7; // CH1
		val |= 1 << 6; // CH2
		val |= 0 << 5; // CH3
		val |= 0 << 4; // CH4
		write(Registers::ASI_OUT_CH_EN, val);
	}

	// power on the adc
	{
		uint8_t val = 0x00;
		val |= 0 << 7; // mic. bias
		val |= 1 << 6; // all enabled ADC and PDM channels
		val |= 1 << 5; // PLL
		write(Registers::PWR_CFG, val);
	}





}
