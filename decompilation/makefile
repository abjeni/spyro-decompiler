PROGRAM := program

CC := gcc
LD := ld

BUILD_DIR := build
INCLUDE_DIR := include
SOURCE_DIR := source
DECOMPILATION_OUT := ../game_src/decompiled

SRCS = $(wildcard $(SOURCE_DIR)/*.c)
OBJS = $(patsubst $(SOURCE_DIR)%,$(BUILD_DIR)%,$(SRCS:.c=.o))

CFLAGS := -Wall -march=native -pipe -ggdb
all: $(BUILD_DIR) $(PROGRAM) $(DECOMPILATION_OUT)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@ -I.

$(BUILD_DIR):
	mkdir $(BUILD_DIR)

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

$(DECOMPILATION_OUT):
	mkdir --parents $(DECOMPILATION_OUT)

.PHONY: clean run

clean:
	rm -f $(PROGRAM)
	rm -rf $(BUILD_DIR)
	rm -rf $(DECOMPILATION_OUT)

run: all
	./$(PROGRAM)