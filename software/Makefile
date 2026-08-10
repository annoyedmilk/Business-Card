# Business Card firmware
#
# Requires a RISC-V GCC on PATH (riscv-none-elf-gcc or any of the other
# prefixes ch32fun.mk probes for) and the ch32fun submodule:
#
#     git submodule update --init
#
#     make            build bin/main.bin
#     make flash      build and write to the target with minichlink
#     make clean      remove bin/
#
# Build products all land in bin/; src/ stays source-only.

BINDIR     := bin
TARGET     := $(BINDIR)/main
TARGET_MCU := CH32V003

ENTRY_C := src/main.c

ADDITIONAL_C_FILES := \
	src/tetris.c \
	src/pacman.c \
	src/testmode.c \
	src/ui.c \
	src/input.c \
	src/sprite.c \
	src/timing.c \
	src/spritebank.c \
	src/i2c_tx.c \
	src/oled_min.c

CH32FUN := ch32fun/ch32fun

# ch32fun.mk chooses between cmd.exe and POSIX helpers (rm/del, which/where,
# /dev/null vs nul) from $(OS) alone. Under an MSYS2 or Git-for-Windows make
# that is wrong: $(OS) is Windows_NT but recipes run in /bin/sh, so `make
# clean` deletes nothing and the `2>nul` probes leave a stray file named `nul`
# in the tree. Go by the shell make will actually use.
ifneq (,$(filter sh sh.exe bash bash.exe dash dash.exe,$(notdir $(SHELL))))
  OS :=
  COPYFILE = cp -f $1 $2
else
  COPYFILE = copy /Y "$(subst /,\,$1)" "$(subst /,\,$2)" >nul
endif

# Not the generated ch32fun script: ours excludes the last 64-byte flash page,
# which holds the high-score record (see ld/ch32v003.ld).
LINKER_SCRIPT := ld/ch32v003.ld

# Size flags, worth 164 bytes together. -msave-restore moves register
# save/restore sequences into shared libcalls; -fno-inline-small-functions
# stops GCC duplicating small helpers called from many places.
# -Oz gains nothing over -Os on this code.
EXTRA_CFLAGS += -msave-restore -fno-inline-small-functions

# The entry translation unit is compiled from a staged copy in bin/ (see the
# $(TARGET).$(TARGET_EXT) rule below), so src/ has to be on the include path
# for it to find the project headers.
EXTRA_CFLAGS += -Isrc

all : $(TARGET).bin

include $(CH32FUN)/ch32fun.mk

# ch32fun.mk takes the entry translation unit to be $(TARGET).c, which ties the
# output directory to the source directory. Staging a copy of the real entry
# file at that name is what lets products live in bin/ while src/ stays
# source-only. It has to be a genuine file rather than a do-nothing rule: make
# compares its timestamp against the .elf, and a target it cannot stat counts
# as always-newer, which would relink on every invocation.
$(TARGET).$(TARGET_EXT) : $(ENTRY_C) | $(BINDIR)
	$(call COPYFILE,$<,$@)

# ch32fun.mk owns the recipe for $(TARGET).elf; this adds the output directory
# as an order-only prerequisite, so a missing bin/ is created but its mtime
# never forces a relink.
$(TARGET).elf : | $(BINDIR)

# ch32fun.mk lists only .c files as prerequisites, so editing a header changed
# nothing and `make` reported "Nothing to be done" while leaving a stale image
# in place. Everything is built in one gcc invocation, so naming the headers
# directly is enough - there are no .d files to generate.
$(TARGET).elf : $(wildcard src/*.h) funconfig.h

# Bare `mkdir` so this works under both cmd.exe and sh; the leading `-` covers
# the case where the directory already exists.
$(BINDIR) :
	-mkdir $(BINDIR)

flash : cv_flash

clean : cv_clean
	-$(RMDIR) $(BINDIR)

# Applies the project standard in ../.clang-format. Editors backed by clangd,
# Zed included, pick that file up on their own, so this is only for formatting
# the tree in one go or checking it in CI.
format :
	clang-format --style=file -i src/*.c src/*.h

format-check :
	clang-format --style=file --dry-run --Werror src/*.c src/*.h

.PHONY : all flash clean format format-check
