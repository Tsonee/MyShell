CC=gcc
CFLAGS=-I.

BUILD_DIR=build/
BIN_DIR=bin/
TEST_DIR=bin/test/

DEPS=CTui.h suggestion.h util.h CLParser.h command.h

MAIN_SRC=my_shell.c
CORE_SRCS=CTui.c suggestion.c util.c CLParser.c command.c

MAIN_OBJ=$(MAIN_SRC:%.c=$(BUILD_DIR)%.o)
CORE_OBJS=$(CORE_SRCS:%.c=$(BUILD_DIR)%.o)

TEST_SRC=test.c
TEST_OBJ=$(TEST_SRC:%.c=$(TEST_DIR)%.o)

# Default target
all: my_shell

# Build main shell program
my_shell: $(MAIN_OBJ) $(CORE_OBJS) | $(BIN_DIR)
	$(CC) $^ -o $(BIN_DIR)my_shell

# Compile core objects
$(BUILD_DIR)%.o: %.c $(DEPS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test object
$(TEST_DIR)%.o: %.c $(DEPS) | $(TEST_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test executable (WITHOUT my_shell.o)
test: $(CORE_OBJS) $(TEST_OBJ) | $(TEST_DIR)
	$(CC) $^ -o $(TEST_DIR)test

# Directory creation
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TEST_DIR):
	mkdir -p $(TEST_DIR)

# Clean build files
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)
