# Business Card firmware
#
# Requires a RISC-V GCC on PATH (riscv-none-elf-gcc or any of the other
# prefixes ch32fun.mk probes for) and the ch32fun submodule:
#
#     git submodule update --init
#
#     make            build src/main.bin
#     make flash      build and write to the target with minichlink
#     make clean      remove build products

TARGET     := src/main
TARGET_MCU := CH32V003

# main.c includes driver.h, oled_min.h and spritebank.h from src/.
ADDITIONAL_C_FILES := src/i2c_tx.c src/oled_min.c

CH32FUN := ch32fun/ch32fun

# Not the generated ch32fun script: ours excludes the last 64-byte flash page,
# which holds the high-score record (see ld/ch32v003.ld).
LINKER_SCRIPT := ld/ch32v003.ld

# Size flags, worth 164 bytes together. -msave-restore moves register
# save/restore sequences into shared libcalls; -fno-inline-small-functions
# stops GCC duplicating small helpers called from many places.
# -Oz gains nothing over -Os on this code.
EXTRA_CFLAGS += -msave-restore -fno-inline-small-functions

all : $(TARGET).bin

include $(CH32FUN)/ch32fun.mk

flash : cv_flash
clean : cv_clean

.PHONY : all flash clean
