CC=gcc
CFLAGS=-Wall -Werror -pedantic -ansi -g
OUT_NAME := app
BUILD_BIN := bin
FILES := main.c ds/bp.c

.PHONY: all clean

all: $(BUILD_BIN)/$(OUT_NAME)
	@printf "Done."

clean:
	@rm -rf $(BUILD_BIN)

$(BUILD_BIN)/$(OUT_NAME):
	@mkdir -p $(BUILD_BIN)
	@$(CC) $(FILES) -o $(BUILD_BIN)/$(OUT_NAME) $(CFLAGS)
