MCU          = attiny2313
ARCH         = AVR8
F_CPU        = 4000000
OPTIMIZATION = s
TARGET       = main
SRC          = $(TARGET).c
CC_FLAGS     =
LD_FLAGS     =
AVRDUDE_PROGRAMMER = usbasp

# Default target
all:

# Include DMBS build script makefiles
DMBS_PATH   ?= ../dmbs/DMBS
include $(DMBS_PATH)/core.mk
include $(DMBS_PATH)/gcc.mk
include $(DMBS_PATH)/cppcheck.mk
include $(DMBS_PATH)/doxygen.mk
include $(DMBS_PATH)/dfu.mk
include $(DMBS_PATH)/hid.mk
include $(DMBS_PATH)/avrdude.mk
include $(DMBS_PATH)/atprogram.mk
