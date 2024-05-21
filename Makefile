CC=gcc
CFLAGS=-Wall -ansi -g -Werror
OUT_NAME := app
BUILD_BIN := bin
FILES := utils/misc.c comp/rle.c utils/rand.c enc/aes.c utils/screen.c utils/input.c ds/db.c ds/schema.c interface/interface.c main.c

DEBUG_FLAGS := -DDEBUG

.PHONY: all clean

all: $(BUILD_BIN)/$(OUT_NAME)
	@printf "Done."

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

clean:
	@rm -rf $(BUILD_BIN)

$(BUILD_BIN)/$(OUT_NAME):
	@mkdir -p $(BUILD_BIN)
	@$(CC) $(FILES) -o $(BUILD_BIN)/$(OUT_NAME) $(CFLAGS)
