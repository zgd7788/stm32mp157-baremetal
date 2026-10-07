# Cortex-A7 DDR image for U-Boot loadb/go at 0xc0008000.
CROSS_COMPILE ?= arm-none-eabi-
CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
SIZE    := $(CROSS_COMPILE)size

TARGET  := buzzer
BUILD   := build
CFLAGS  := -mcpu=cortex-a7 -marm -mfloat-abi=soft \
           -ffreestanding -fno-builtin -fno-pic -fno-pie \
           -fdata-sections -ffunction-sections \
           -Wall -Wextra -Werror -Wconversion -Wsign-conversion \
           -Wshadow -Wstrict-prototypes -Wmissing-prototypes \
           -MMD -MP -std=c11 -Iinc -Iinclude
LDFLAGS := -mcpu=cortex-a7 -marm -mfloat-abi=soft -nostdlib -no-pie \
           -Wl,--gc-sections -Wl,--build-id=none \
           -Wl,-Map=$(BUILD)/$(TARGET).map -Wl,-e,_start -Tmap.lds
LDLIBS  := -lgcc

SOURCES := main.c src/gpio.c src/tim.c startup.S
OBJECTS := $(SOURCES:%.c=$(BUILD)/%.o)
OBJECTS := $(OBJECTS:%.S=$(BUILD)/%.o)
DEPENDS := $(OBJECTS:.o=.d)

-include $(DEPENDS)

.DEFAULT_GOAL := all

.PHONY: all clean size

all: $(BUILD)/$(TARGET).bin

$(BUILD)/$(TARGET).elf: $(OBJECTS) map.lds
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

$(BUILD)/$(TARGET).bin: $(BUILD)/$(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(BUILD)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/%.o: %.S
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

size: $(BUILD)/$(TARGET).elf
	$(SIZE) $<

clean:
	rm -rf $(BUILD)
