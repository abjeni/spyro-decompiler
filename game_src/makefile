PROGRAM := program

CC1 := gcc
CC2 := tcc

BUILD_DIR := build
INCLUDE_DIR := include
SOURCE_DIR := source
DECOMPILE_DIR := decompiled
MEMCARD_DIR := ../mem_card

SRCS = $(wildcard $(SOURCE_DIR)/*.c)
OBJS = $(patsubst $(SOURCE_DIR)%,$(BUILD_DIR)%,$(SRCS:.c=.o))

DESRCS = $(wildcard $(DECOMPILE_DIR)/*.c)
DEOBJS = $(patsubst $(DECOMPILE_DIR)%,$(BUILD_DIR)%,$(DESRCS:.c=.o))

CFLAGS := -Wall -march=native -Ofast -pipe -ggdb
#CFLAGS := -Wall -march=native -Ofast -pipe
all: $(BUILD_DIR) $(PROGRAM) $(MEMCARD_DIR)

$(MEMCARD_DIR):
	mkdir --parents $(MEMCARD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC1) $(CFLAGS) -I$(INCLUDE_DIR) -I$(DECOMPILE_DIR) -c $< -o $@ -I.

$(BUILD_DIR)/%.o: $(DECOMPILE_DIR)/%.c
	$(CC2) $(CFLAGS) -I$(INCLUDE_DIR) -I$(DECOMPILE_DIR) -c $< -o $@ -I.

$(BUILD_DIR):
	mkdir --parents $(BUILD_DIR)

$(PROGRAM): $(OBJS) $(DEOBJS)
	$(CC1) $(CFLAGS) -lSDL3 -L../disassembly/program $^ -o $@

.PHONY: clean run test

clean:
	rm -f $(PROGRAM)
	rm -r $(BUILD_DIR)

run: all
	./$(PROGRAM)

test: all
	./$(PROGRAM) test
