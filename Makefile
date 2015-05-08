SHELL=C:/Windows/System32/cmd.exe

# Universal Makefile   Version: 31.01.2015
#
# Requires GNU 'make' and 'find.exe'.

# Target Architecture
BOARD := LaFortuna
MCU   := at90usb1286
F_CPU := 8000000UL

# Tool Options
CFLAGS    := -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)
CFLAGS    += -Wl,-u,vfprintf -lprintf_flt -lm  # floating point support
CFLAGS    += -fno-strict-aliasing  # FATfs does not adhere to strict aliasing
CFLAGS    += -Wno-main             # main() will never return
CFLAGS    += -Wall -Wextra -pedantic
CFLAGS    += -Wstrict-overflow=5 -fstrict-overflow -Winline
# CHKFLAGS  := -fsyntax-only
CHKFLAGS  :=
BUILD_DIR := _build

# Ignoring hidden directories; sorting to drop duplicates:
CFILES := $(shell find . ! -path "*/\.*" -type f -name "*.c")
CPATHS := $(sort $(dir $(CFILES)))
vpath %.c $(CPATHS)
HFILES := $(shell find . ! -path "*/\.*" -type f -name "*.h")
HPATHS := $(sort $(dir $(HFILES)))
vpath %.h $(HPATHS)
CFLAGS += $(addprefix -I ,$(HPATHS))
DEPENDENCIES := $(patsubst %.c,$(BUILD_DIR)/%.d,$(notdir $(CFILES)))
OBJFILES     := $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(CFILES)))

.PHONY: upld clean check-syntax ?

upld: $(BUILD_DIR)/main.hex
	$(info )
	$(info =========== ${BOARD} =============)
	dfu-programmer $(MCU) erase
	dfu-programmer $(MCU) flash $(BUILD_DIR)/main.hex


$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	@avr-gcc $(CFLAGS) -MMD -MP -c $< -o $@

$(BUILD_DIR)/%.elf: $(OBJFILES)
	@avr-gcc -mmcu=$(MCU) -o $@  $^

$(BUILD_DIR)/%.hex %.hex: $(BUILD_DIR)/%.elf
	@avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex  $<  "$@"

-include $(sort $(DEPENDENCIES))

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

# Emacs flymake support
check-syntax:
	@avr-gcc $(CFLAGS) $(CHKFLAGS) -o /dev/null -S $(CHK_SOURCES)

clean:
	@$(RM) -rf $(BUILD_DIR)
	@$(RM) -rf ./-p

?%:
	@echo '$*=$($*)'

?:
	$(info -------------------------------------------------)
	$(info Usage:)
	$(info Source files can be grouped into subdirectories.)
  	$(info To build an executable and attempt to upload it,)
  	$(info use just "make".)
  	$(info )
  	$(info make mymain.hex --> to build a hex-file for mymain.c)
  	$(info make ?CFILES    --> show source files to be used)
  	$(info make ?CPATHS    --> show source locations)
  	$(info make ?HFILES    --> show header files found)
  	$(info make ?HPATHS    --> show header locations)
  	$(info make ?CFLAGS    --> show compiler options)
  	$(info -------------------------------------------------)
  	@:

# Copyright 2014-2015 Klaus-Peter Zauner
# At your option this work is licensed under a Creative Commons
# Attribution-NonCommercial 3.0 Unported License [1], or under a
# Creative Commons Attribution-ShareAlike 3.0 Unported License [2].
# [1]: See: http://creativecommons.org/licenses/by-nc/3.0/
# [2]: See: http://creativecommons.org/licenses/by-sa/3.0/
#===================================================================