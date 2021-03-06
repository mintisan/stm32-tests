#
# arch: arm cortex-m0
# chip: stm32f030r8t6
# platform: stm32f030r8-nucleo
# description: nucleo stm32f030r8t6 board
#

## toolchain

CROSS_COMPILE = arm-none-eabi

CC = $(CROSS_COMPILE)-gcc
LD = $(CROSS_COMPILE)-ld
OBJCOPY = $(CROSS_COMPILE)-objcopy
OBJSIZE = $(CROSS_COMPILE)-size

## platform identifiers

CHIP = stm32f0

## platform dependencies

NRF24_CFG_FLAGS = -DSPI_SINGLE_BYTE

LIBCM3_TARGET	= stm32/f0
LIBCM3_FPFLAGS	= -msoft-float

deps: libopencm3 libnrf24 nanopb

## platform-specific compile flags

PFLAGS = \
	-mthumb			\
	-mcpu=cortex-m0		\
	-msoft-float		\
	-ffunction-sections	\

## projects

TARGETS = "			\
	leds-mini		\
	leds-systick		\
	leds-timer		\
	leds-pwm		\
	uart-simple		\
	uart-printf		\
	i2c-lm75a-temp		\
	spi-nrf24-node		\
	hse-led			\
	rtc-alarm		\
	lpm-rtc-wkup		\
	tim-input-capture	\
	adc-test		\
	w1-test"

ifeq ($(MAKECMDGOALS), leds-mini)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds-mini/build.mk
endif

ifeq ($(MAKECMDGOALS), leds-systick)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds-systick/build.mk
endif

ifeq ($(MAKECMDGOALS), leds-timer)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds-timer/build.mk
endif

ifeq ($(MAKECMDGOALS), leds-pwm)
include $(PRJ_DIR)/boards/$(PLAT)/apps/leds-pwm/build.mk
endif

ifeq ($(MAKECMDGOALS), uart-simple)
include $(PRJ_DIR)/boards/$(PLAT)/apps/uart-simple/build.mk
endif

ifeq ($(MAKECMDGOALS), uart-printf)
include $(PRJ_DIR)/boards/$(PLAT)/apps/uart-printf/build.mk
endif

ifeq ($(MAKECMDGOALS), i2c-lm75a-temp)
include $(PRJ_DIR)/boards/$(PLAT)/apps/i2c-lm75a-temp/build.mk
endif

ifeq ($(MAKECMDGOALS), spi-nrf24-node)
include $(PRJ_DIR)/boards/$(PLAT)/apps/spi-nrf24-node/build.mk
endif

ifeq ($(MAKECMDGOALS), hse-led)
include $(PRJ_DIR)/boards/$(PLAT)/apps/hse-led/build.mk
endif

ifeq ($(MAKECMDGOALS), rtc-alarm)
include $(PRJ_DIR)/boards/$(PLAT)/apps/rtc-alarm/build.mk
endif

ifeq ($(MAKECMDGOALS), lpm-rtc-wkup)
include $(PRJ_DIR)/boards/$(PLAT)/apps/lpm-rtc-wkup/build.mk
endif

ifeq ($(MAKECMDGOALS), adc-test)
include $(PRJ_DIR)/boards/$(PLAT)/apps/adc-test/build.mk
endif

ifeq ($(MAKECMDGOALS), tim-input-capture)
include $(PRJ_DIR)/boards/$(PLAT)/apps/tim-input-capture/build.mk
endif

ifeq ($(MAKECMDGOALS), w1-test)
include $(PRJ_DIR)/boards/$(PLAT)/apps/w1-test/build.mk
endif

## platform-specific flash rules

upload:
	openocd -f $(PRJ_DIR)/boards/$(PLAT)/scripts/openocd-jlink-swd.cfg -c 'program ()'

debug:
	openocd -f $(PRJ_DIR)/boards/$(PLAT)/scripts/openocd-jlink-swd.cfg -c 'attach ()'
