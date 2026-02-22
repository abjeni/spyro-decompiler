SDL_PROGRAM := program
WASM_PROGRAM := main.wasm

CC := tcc
wasm : CC := clang

BUILD_DIR := build
INCLUDE_DIR := include
SOURCE_DIR := source
DECOMPILE_DIR := decompiled
MEMCARD_DIR := ../mem_card

CFLAGS := -Wall -O2 -ggdb -pipe -I$(INCLUDE_DIR) -I$(DECOMPILE_DIR)
#CFLAGS := -Wall -march=native -Ofast -pipe

sdl : CFLAGS += -I$(SOURCE_DIR)/platform/sdl/ -march=native
wasm : CFLAGS += -I$(SOURCE_DIR)/platform/wasm/ -I$(SOURCE_DIR)/platform/wasm/std/
wasm : CFLAGS += -fno-builtin --target=wasm32 --no-standard-libraries -Wl,--strip-all,--export-dynamic,--no-entry,--import-memory

SRCS = $(wildcard $(SOURCE_DIR)/*.c)
OBJS = $(patsubst $(SOURCE_DIR)%,$(BUILD_DIR)%,$(SRCS:.c=.o))

DESRCS = $(wildcard $(DECOMPILE_DIR)/*.c)
DEOBJS = $(patsubst $(DECOMPILE_DIR)%,$(BUILD_DIR)%,$(DESRCS:.c=.o))

.PHONY: clean run test sdl all wasm

sdl: $(BUILD_DIR) $(MEMCARD_DIR) all $(SDL_PROGRAM)

wasm: $(BUILD_DIR) $(MEMCARD_DIR) all $(WASM_PROGRAM)
	echo $(BUILD_DIR)

all: $(BUILD_DIR) $(MEMCARD_DIR)

clean:
	rm -f $(SDL_PROGRAM)
	rm -f $(WASM_PROGRAM)
	rm -r $(BUILD_DIR)

run: sdl
	./$(SDL_PROGRAM)

test: sdl
	./$(SDL_PROGRAM) test

$(MEMCARD_DIR):
	mkdir --parents $(MEMCARD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

$(BUILD_DIR)/%.o: $(DECOMPILE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

$(BUILD_DIR):
	mkdir --parents $(BUILD_DIR)

$(SDL_PROGRAM): $(OBJS) $(DEOBJS) $(SOURCE_DIR)/platform/sdl/main.c $(SOURCE_DIR)/platform/sdl/memory_card.c
	$(CC) $(CFLAGS) -lSDL3 -L../disassembly/program $^ -o $@

$(WASM_PROGRAM): $(OBJS) $(DEOBJS)
	$(CC) $(CFLAGS) -L../disassembly/program $^ -o $@