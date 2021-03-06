/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>

#include <RF24.h>

#include "rf24hub.h"

/*
 *	Connectivity between nRF24L01 on Wireless Shield v1.0 and
 *  stm32f401re-nucleo:
 *		MOSI <-> PA7
 *		MISO <-> PA6
 *		SCK  <-> PA5
 *		CSN  <-> PA9
 *		CE   <-> PA8
 *		IRQ  <-> PA10
 *		VCC  <-> 3V
 *		GND  <-> GND
 */

/* */

static void radio_spi_setup(void)
{
	/* enable spi clock */
	rcc_periph_clock_enable(RCC_SPI1);

	/* spi initialization */
	spi_set_master_mode(SPI1);
	spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_8);
	spi_set_clock_polarity_0(SPI1);
	spi_set_clock_phase_0(SPI1);
	spi_set_full_duplex_mode(SPI1);
	spi_set_unidirectional_mode(SPI1);
	spi_enable_software_slave_management(SPI1);
	spi_send_msb_first(SPI1);
	spi_set_dff_8bit(SPI1);
	spi_set_nss_high(SPI1);
	SPI_I2SCFGR(SPI1) &= ~SPI_I2SCFGR_I2SMOD;
	spi_enable(SPI1);
}

/* */

static void radio_pins_setup(void)
{
	/* enable pins for nRF24L01 */
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);

	/* disable RFM69HW on Wireless Shield v1.0 which is on the same spi */
	gpio_mode_setup(GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO7);
	gpio_set(GPIOC, GPIO7);

	/* enable SPI1: set AF5 mux */
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO5 | GPIO6 | GPIO7);
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,  GPIO5 | GPIO6 | GPIO7);
	gpio_set_af(GPIOA, GPIO_AF5, GPIO5 | GPIO6 | GPIO7);

	/* setup nRF24L01 gpio pins on Wireless Shield v1.0: CE, CSN, IRQ */
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO8);
	gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO9);
	gpio_mode_setup(GPIOA, GPIO_MODE_INPUT, GPIO_PUPD_NONE, GPIO10);

	/* start with spi communication disabled */
	gpio_set(GPIOA, GPIO9);
	gpio_clear(GPIOA, GPIO8);
}

/* */

void f_csn(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO9) : gpio_clear(GPIOA, GPIO9);
}

void f_ce(int level)
{
	(level > 0) ? gpio_set(GPIOA, GPIO8) : gpio_clear(GPIOA, GPIO8);
}

void f_spi_set_speed(int khz)
{
	/* not implemented */
}

uint8_t f_spi_xfer(uint8_t dat)
{
	return ((uint8_t) spi_xfer(SPI1, (uint16_t) dat));
}

struct rf24 nrf = {
	.csn = f_csn,
	.ce = f_ce,
	.spi_set_speed = f_spi_set_speed,
	.spi_xfer = f_spi_xfer,
};

/* */

struct rf24* radio_init(void)
{
	radio_pins_setup();
	radio_spi_setup();

	/* Default configuration from libnrf24:
	 *  - SETUP_RETR:ARD = 100b		~ auto retransmit delay 1500us
	 *  - SETUP_RETR:ARC = 1111b	~ up to 15 retransmits
	 *  - RF_SETUP:RF_PWR = 11b		~ RF output power 0dBm
	 *  - RF_SETUP:RF_DR_* = 0		~ data rate 1Mbps
	 *  - CONFIG:EN_CRC = 1			~ CRC enabled
	 *  - CONFIG:CRCO = 1			~ 16bit CRC mode
	 *  - DYNPD = 0					~ dynamic payload lengths zeroed
	 *  - RF_CH = 76				~ RF channel 76
	 *
	 *  - payload size 32 bytes
	 *  - no ack payload
	 */

	rf24_init(&nrf);

	return &nrf;
}
