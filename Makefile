CC=gcc
CFLAGS=-Wall -ansi -g
OUT_NAME := app
BUILD_BIN := bin
FILES := utils/screen.c utils/input.c ds/bp.c interface/interface.c main.c

.PHONY: all clean

all: $(BUILD_BIN)/$(OUT_NAME)
	@printf "Done."

clean:
	@rm -rf $(BUILD_BIN)

$(BUILD_BIN)/$(OUT_NAME):
	@mkdir -p $(BUILD_BIN)
	@$(CC) $(FILES) -o $(BUILD_BIN)/$(OUT_NAME) $(CFLAGS)
