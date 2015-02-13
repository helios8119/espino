# which modules (subdirectories) of the project to include in compiling
SRC_DIRS    = . SSD1306 DS3231

SDK_ROOT    = ../SDK
ESPPORT     = /dev/tty.usbserial-AEXMIKFF

ESPINO_PATH = ../espino

XBIN_PATH   = $(SDK_ROOT)/xtensa-lx106-elf/bin/xtensa-lx106-elf-
XLIB_PATH   = -L$(SDK_ROOT)/xtensa-lx106-elf/lib/gcc/xtensa-lx106-elf/4.8.2 -L$(SDK_ROOT)/xtensa-lx106-elf/xtensa-lx106-elf/lib
XLIB_INCS   = -I$(SDK_ROOT)/xtensa-lx106-elf/xtensa-lx106-elf/include/c++/4.8.2 -I$(SDK_ROOT)/xtensa-lx106-elf/xtensa-lx106-elf/include/c++/4.8.2/xtensa-lx106-elf -I$(SDK_ROOT)/xtensa-lx106-elf/xtensa-lx106-elf/include
XLIB_LD     = -Teagle.app.v6.ld

SDK_BASE    = $(SDK_ROOT)/ESP8266_SDK
SDK_LIBS    = -L$(SDK_BASE)/lib -L$(SDK_BASE)/ld
SDK_INCS    = include include/json

ESPTOOL_PY  = $(SDK_ROOT)/bin/esptool.py
ESPTOOL     = $(SDK_ROOT)/bin/esptool

BUILD_BASE  = .build
FW_BASE     = firmware
APP_AR      = $(BUILD_BASE)/app.a
TARGET      = $(BUILD_BASE)/app.out

FW_FILE_1   = $(addprefix $(FW_BASE)/,0x00000.bin)
FW_FILE_2   = $(addprefix $(FW_BASE)/,0x40000.bin)
FW_1_ARGS   = -bo $@ -bs .text -bs .data -bs .rodata -bc -ec
FW_2_ARGS   = -es .irom0.text $@ -ec

ESPINO_SRC  = $(wildcard $(ESPINO_PATH)/*.c) $(wildcard $(ESPINO_PATH)/*.cpp)
ESPINO_OBJ  = $(patsubst $(ESPINO_PATH)/%, $(BUILD_BASE)/espino/%, $(filter-out %.cpp, $(filter-out %.c, $(ESPINO_SRC:.c=.o) $(ESPINO_SRC:.cpp=.o))))

SRC         = $(foreach sdir,$(SRC_DIRS),$(wildcard $(sdir)/*.c) $(wildcard $(sdir)/*.cpp))
OBJ         = $(ESPINO_OBJ) $(patsubst %.c,$(BUILD_BASE)/%.o,$(SRC)) $(patsubst %.cpp,$(BUILD_BASE)/%.o,$(SRC))
INCLUDES    = $(addprefix -I,$(SRC_DIRS)) $(addprefix -I,$(ESPINO_PATH)) $(addprefix -I$(SDK_BASE)/,$(SDK_INCS)) $(XLIB_INCS)

CC          = $(XBIN_PATH)gcc
CPP         = $(XBIN_PATH)g++
AR          = $(XBIN_PATH)ar
LD          = $(XBIN_PATH)g++
OBJDUMP     = $(SDK_ROOT)/bin/objdump.sh $(XBIN_PATH)objdump

LIBS        = -lc -lg -lm -lgcc -ljson -llwip -lmain -lnet80211 -lphy -lwpa -lpp
#LIBS        = -lstdc++ -lc -lg -lm -lgcc -ljson -llwip -lmain -lnet80211 -lphy -lwpa -lpp
CFLAGS      = -Os -g -O2 -Wpointer-arith -Wundef -Werror -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals  -D__ets__ -DICACHE_FLASH
LDFLAGS     = -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static
CPFLAGS     = -g -O3 -Wall -Wl,-EL -fno-rtti -nostdlib -mlongcalls -mtext-section-literals -D__ets__ -DICACHE_FLASH -fmessage-length=0 -fverbose-asm -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -fno-unwind-tables -fno-threadsafe-statics -fno-stack-protector -ftree-ter -Wundef -Wno-unused-local-typedefs -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-unused-but-set-variable -Werror=return-type -Wpointer-arith -Wno-reorder

define compile-objects
$(BUILD_BASE)/$1/%.o: $1/%.c
	@echo "CC $$<"
	@ mkdir -p $(BUILD_BASE)/$1
	@$(CC) $(INCLUDES) $(CFLAGS)  -c $$< -o $$@

$(BUILD_BASE)/$1/%.o: $1/%.cpp
	@echo "CPP $$<"
	@ mkdir -p $(BUILD_BASE)/$1
	@$(CPP) $(INCLUDES) $(CPFLAGS) -c $$< -o $$@
endef

.PHONY: all flash clean

all: $(BUILD_BASE) $(FW_BASE) $(TARGET) $(FW_FILE_1) $(FW_FILE_2) finfo

$(BUILD_BASE)/espino/%.o: $(ESPINO_PATH)/%.c
	@echo "CC $<"
	@ mkdir -p $(BUILD_BASE)/espino
	@$(CC) $(INCLUDES) $(CFLAGS)  -c $< -o $@

$(BUILD_BASE)/espino/%.o: $(ESPINO_PATH)/%.cpp
	@echo "CPP $<"
	@ mkdir -p $(BUILD_BASE)/espino
	@$(CPP) $(INCLUDES) $(CPFLAGS) -c $< -o $@

$(BUILD_BASE):
	@ mkdir -p $@

$(FW_BASE):
	@ mkdir -p $@

$(APP_AR): $(OBJ)
	@echo "AR $@"
	@$(AR) cru $@ $^

$(TARGET): $(APP_AR)
	@echo "LD $@"
	@$(LD) $(SDK_LIBS) $(XLIB_PATH) $(XLIB_LD) $(LDFLAGS) -Wl,--start-group $(LIBS) $(APP_AR) -Wl,--end-group -o $@

$(FW_FILE_1): $(TARGET)
	@echo "FW $@"
	@$(ESPTOOL) -eo $(TARGET) $(FW_1_ARGS)

$(FW_FILE_2): $(TARGET)
	@echo "FW $@"
	@$(ESPTOOL) -eo $(TARGET) $(FW_2_ARGS)

finfo: $(FW_FILE_1)
	@$(ESPTOOL_PY) image_info $(FW_FILE_1)
#	@$(OBJDUMP) $(TARGET)

flash: all
	@echo "Flashing: $(FW_FILE_1)"
	@$(ESPTOOL_PY) --port $(ESPPORT) write_flash 0x00000 $(FW_FILE_1)
	@sleep 3
	@echo "Flashing: $(FW_FILE_2)"
	@$(ESPTOOL_PY) --port $(ESPPORT) write_flash 0x40000 $(FW_FILE_2)
	@echo "Flashed Successfully"

clean:
	@ rm -rf $(APP_AR) $(TARGET_OUT) $(BUILD_BASE) $(FW_FILE_1) $(FW_FILE_2) $(FW_BASE)

$(foreach bdir,$(SRC_DIRS),$(eval $(call compile-objects,$(bdir))))
