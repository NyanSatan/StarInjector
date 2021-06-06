#
# User defined vars
#

TOOLCHAIN ?= /opt/arm-arm64-llvm-11-toolchain/bin
PYTHON ?= python2.7

#
# Do not touch these
#

BUILD_ROOT = build
SRC_ROOT = src
TARGETS_ROOT = targets

CC = $(TOOLCHAIN)/clang
CFLAGS = -Wall
CFLAGS += -mcpu=swift
CFLAGS += -march=armv7-a
CFLAGS += -ffixed-r9
CFLAGS += -Wno-multichar
CFLAGS += -Os
CFLAGS += -mthumb
CFLAGS += -fno-builtin
CFLAGS += -I$(SRC_ROOT)/include
CFLAGS += -Wno-unused-variable
CFLAGS += -MMD

LD = $(TOOLCHAIN)/ld.lld
LDFLAGS = -nostdlib
LDFLAGS += -Tmisc/script.ld

OBJCOPY = $(TOOLCHAIN)/llvm-objcopy

POSTPROCESS = tools/binary-post-process.py


PROJ_NAME = StarInjector
FILENAME = $(PROJ_NAME)
BUILD_PATH = $(BUILD_ROOT)/$(TARGET)
ELF_PATH = $(BUILD_PATH)/$(FILENAME).elf
BIN_PATH = $(BUILD_PATH)/$(FILENAME).bin


ifneq ($(MAKECMDGOALS),clean)
ifeq ($(TARGET),)
$(error No TARGET specified)
endif
include $(TARGETS_ROOT)/$(TARGET)/target.mk
$(info %%%%% building $(PROJ_NAME) for target $(TARGET))
endif


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

$(BIN_PATH): $(ELF_PATH)
	@echo "\textracting binary"
	@$(DIR_HELPER)
	@$(OBJCOPY) -O binary $< $@

	@echo "\tpost-processing binary"
	@$(PYTHON) $(POSTPROCESS) $(BIN_PATH)

$(ELF_PATH): $(ENTRY_OBJECT) $(OBJECTS)
	@echo "\tlinking"
	@$(DIR_HELPER)
	@$(LD) -o $@ $(LDFLAGS) $^

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
	$(shell rm -rf $(BUILD_ROOT)/*)
	@echo "%%%%% done cleaning"

-include $(OBJECTS:.o=.d) $(ENTRY_OBJECT:.o=.d)
