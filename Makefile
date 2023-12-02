#
# User defined vars
#

PYTHON ?= python2.7

#
# Do not touch these
#

BUILD_ROOT := build
SRC_ROOT := src
TARGETS_ROOT := targets

ifneq ($(MAKECMDGOALS),clean)
ifeq ($(TARGET),)
$(error No TARGET specified)
endif
include $(TARGETS_ROOT)/$(TARGET)/target.mk
$(info %%%%% building $(PROJ_NAME) for target $(TARGET))
endif

CC := xcrun -sdk iphoneos clang -arch armv7s
CFLAGS += --target=armv7s-apple-ios
CFLAGS += -mcpu=swift
CFLAGS += -mthumb
CFLAGS += -Os
CFLAGS += -Wall
CFLAGS += -Wno-multichar
CFLAGS += -fno-builtin
CFLAGS += -I$(SRC_ROOT)/include
CFLAGS += -Wno-unused-variable
CFLAGS += -fno-stack-protector
CFLAGS += -msoft-float
CFLAGS += -MMD

LD := $(CC)
LDFLAGS += -Wl,-preload
LDFLAGS += -Wl,-segalign,0x40
LDFLAGS += -Wl,-merge_zero_fill_sections
LDFLAGS += -Wl,-image_base,$(RELOCATE_ADDR)
LDFLAGS += -nostdlib
LDFLAGS += -static
LDFLAGS += -Wl,-order_file,misc/sym_order.txt

OBJCOPY := vmacho

POSTPROCESS := tools/binary-post-process.py


PROJ_NAME := StarInjector
FILENAME := $(PROJ_NAME)
BUILD_PATH := $(BUILD_ROOT)/$(TARGET)
MACHO_PATH := $(BUILD_PATH)/$(FILENAME).o
BIN_PATH := $(BUILD_PATH)/$(FILENAME).bin


SOURCES = \
	$(SRC_ROOT)/main.c \
	$(SRC_ROOT)/cmd.c \
	$(SRC_ROOT)/bootpage.c \
	$(SRC_ROOT)/utils.c \
	$(SRC_ROOT)/shsh.c \
	$(SRC_ROOT)/image3.c \
	$(SRC_ROOT)/der.c \
	$(SRC_ROOT)/overlap.c

OBJECTS = $(addprefix $(BUILD_PATH)/, $(SOURCES:.c=.o))

ENTRY = $(SRC_ROOT)/entry.S
ENTRY_OBJECT = $(addprefix $(BUILD_PATH)/, $(ENTRY:.S=.o))


DIR_HELPER = mkdir -p $(@D)

all: $(BIN_PATH)
	@echo "%%%%% done building"

$(BIN_PATH): $(MACHO_PATH)
	@echo "\textracting binary"
	@$(DIR_HELPER)
	@$(OBJCOPY) -f $< $@

	@echo "\tpost-processing binary"
	@$(PYTHON) $(POSTPROCESS) $(BIN_PATH)

$(MACHO_PATH): $(ENTRY_OBJECT) $(OBJECTS)
	@echo "\tlinking"
	@$(DIR_HELPER)
	@$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_PATH)/%.o: %.S
	@echo "\tbuilding ASM: $<"
	@$(DIR_HELPER)
	@$(CC) $(CFLAGS) -DPROJ_NAME=\"$(PROJ_NAME)\" \
					 -DTARGET=\"$(TARGET)\" \
					 -c $< -o $@ 

$(BUILD_PATH)/%.o: %.c
	@echo "\tbuilding C: $<"
	@$(DIR_HELPER)
	@$(CC) $(CFLAGS) -c $< -o $@ 

.PHONY: clean

clean:
	@echo "%%%%% cleaning"
	$(shell rm -rf $(BUILD_ROOT))
	@echo "%%%%% done cleaning"

-include $(OBJECTS:.o=.d) $(ENTRY_OBJECT:.o=.d)
