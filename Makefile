CC=gcc
CFLAGS=-I.
BUILD_DIR=build/
BIN_DIR=bin/

DEPS=CTui.h suggestion.h util.h
SRCS=my_shell.c CTui.c suggestion.c util.c
OBJS=$(SRCS:%.c=$(BUILD_DIR)%.o)

my_shell: $(OBJS) | $(BIN_DIR)
	$(CC) $(OBJS) -o $(BIN_DIR)my_shell

$(BUILD_DIR)%.o: %.c $(DEPS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ $(CFLAGS)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
