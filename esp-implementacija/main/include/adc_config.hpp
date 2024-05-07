#pragma once

#include <cstdint>

#define ADC_CONF_ADDR 0x4E

namespace adc_config
{
	/* Initialize I2C connection to configure the PCM3120-Q1 ADC*/
	void init();

	/* Write a byte to a register */
	void write(uint8_t reg_addr, uint8_t data);

	/* Read a byte from a register */
	uint8_t read(uint8_t reg_addr);

	/* Select the page of registers */
	void set_page(uint8_t page);

	/* Print all registers on the current page in hex format */
	void print_page_hex();

	void setup();

	enum Registers : uint8_t {
		PAGE_CFG = 0x0,
		SW_RESET = 0x1,
		SLEEP_CFG = 0x2,
		SHDN_CFG = 0x5,
		ASI_CFG0 = 0x7,
		ASI_CFG1 = 0x8,
		ASI_CFG2 = 0x9,
		ASI_MIX_CFG = 0xA,
		ASI_CH1 = 0xB,
		ASI_CH2 = 0xC,
		ASI_CH3 = 0xD,
		ASI_CH4 = 0xE,
		MST_CFG0 = 0x13,
		MST_CFG1 = 0x14,
		ASI_STS = 0x15,
		CLK_SRC = 0x16,
		PDMCLK_CFG = 0x1F,
		PDMIN_CFG = 0x20,
		GPIO_CFG0 = 0x21,
		GPO_CFG0 = 0x22,
		GPO_VAL = 0x29,
		GPIO_MON = 0x2A,
		GPI_CFG0 = 0x2B,
		GPI_MON = 0x2F,
		INT_CFG = 0x32,
		INT_MASK0 = 0x33,
		INT_LTCH0 = 0x36,
		CM_TOL_CFG = 0x3A,
		BIAS_CFG = 0x3B,
		CH1_CFG0 = 0x3C,
		CH1_CFG1 = 0x3D,
		CH1_CFG2 = 0x3E,
		CH1_CFG3 = 0x3F,
		CH1_CFG4 = 0x40,
		CH2_CFG0 = 0x41,
		CH2_CFG1 = 0x42,
		CH2_CFG2 = 0x43,
		CH2_CFG3 = 0x44,
		CH2_CFG4 = 0x45,
		CH3_CFG2 = 0x48,
		CH3_CFG3 = 0x49,
		CH3_CFG4 = 0x4A,
		CH4_CFG2 = 0x4D,
		CH4_CFG3 = 0x4E,
		CH4_CFG4 = 0x4F,
		DSP_CFG0 = 0x6B,
		DSP_CFG1 = 0x6C,
		AGC_CFG0 = 0x70,
		GAIN_CFG = 0x71,
		IN_CH_EN = 0x73,
		ASI_OUT_CH_EN = 0x74,
		PWR_CFG = 0x75,
		DEV_STS0 = 0x76,
		DEV_STS1 = 0x77,
		I2C_CKSUM = 0x7E,
	};
}