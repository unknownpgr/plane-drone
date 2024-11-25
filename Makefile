# Project Information
PROJECT = kit-jt20-drone
TARGET = $(BUILD_DIR)/$(PROJECT).elf
MCU = atmega128
BUILD_DIR = build

# Toolchain
CC = avr-gcc
CPP = avr-g++

# Flags
COMMON = -mmcu=$(MCU)
CFLAGS = $(COMMON) -Wall -std=gnu99 -Os -Wl,-u,vfprintf -lprintf_flt -lm
LDFLAGS = $(COMMON) -Wl,-u,vfprintf,-Map=$(BUILD_DIR)/$(PROJECT).map

# Libraries and Directories
LIBS = -lm -lprintf_flt
LIBDIRS =
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
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJECTS) $(LIBDIRS) $(LIBS)
	@echo -e "\033[0;32mCompilation Successful\033[0m"

# Compile Rule
$(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $< $(INCLUDES) $(LIBDIRS) $(LIBS)

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
