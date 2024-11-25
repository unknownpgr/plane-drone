# Project Information
PROJECT = kit-jt20-drone
BUILD_DIR = build
TARGET = $(BUILD_DIR)/$(PROJECT).elf
MCU = atmega128

# Toolchain
CC = avr-gcc
CPP = avr-g++

# Flags
COMMON = -mmcu=$(MCU)
CFLAGS = $(COMMON) -Wall -std=c99 -Os
LDFLAGS = $(COMMON) -Wl,-u,vfprintf,-Map=$(BUILD_DIR)/$(PROJECT).map,--trace

# Libraries and Directories
INCLUDES = -I./src

# Source / object Files
SOURCES = $(wildcard src/*.c)
SOURCES += $(wildcard src/core/*.c)
SOURCES += $(wildcard src/atmega128/*.c)
OBJECTS = $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SOURCES))

# Rules
all: $(TARGET) $(BUILD_DIR)/$(PROJECT).hex $(BUILD_DIR)/$(PROJECT).eep $(BUILD_DIR)/$(PROJECT).lss

# ==============================================================================

# Link Rule
$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBDIRS)
	@echo -e "\n\n\033[1;32mBuild Successful!\033[0m\n"

# Compile Rule
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# ==============================================================================
	
# Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature
HEX_EEPROM_FLAGS = -j .eeprom --change-section-lma .eeprom=0 --no-change-warnings

$(BUILD_DIR)/$(PROJECT).hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS) $< $@

$(BUILD_DIR)/$(PROJECT).eep: $(TARGET)
	avr-objcopy -O ihex $(HEX_EEPROM_FLAGS) $< $@

$(BUILD_DIR)/$(PROJECT).lss: $(TARGET)
	avr-objdump -h -S $< > $@

# ==============================================================================

# Clean Rule
clean:
	rm -rf $(BUILD_DIR)

# ==============================================================================

# Programmer Settings
PROGRAMMER = stk500
PORT = /dev/cu.usbserial-0001

# Upload Rule
upload:
	avrdude -p $(MCU) -c $(PROGRAMMER) -P $(PORT) -U flash:w:$(BUILD_DIR)/$(PROJECT).hex
