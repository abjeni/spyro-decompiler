SDL_PROGRAM := program
WASM_PROGRAM := ../wasmweb/main.wasm

CC := tcc
wasm : CC := clang

BUILD_DIR := build

INCLUDE_DIR := include
SOURCE_DIR := source
DECOMPILE_DIR := decompiled
MEMCARD_DIR := ../mem_card

CFLAGS := -Wall -pipe -O3 -g -I$(INCLUDE_DIR) -I$(DECOMPILE_DIR)
#CFLAGS := -Wall -march=native -Ofast -pipe


sdl : CFLAGS += -I$(SOURCE_DIR)/platform/sdl/ -march=native
wasm : CFLAGS += -I$(SOURCE_DIR)/platform/wasm/ -I$(SOURCE_DIR)/platform/wasm/std/
wasm : CFLAGS += -fno-builtin --target=wasm32 --no-standard-libraries
#wasm : CFLAGS += -flto -fno-builtin --target=wasm32 --no-standard-libraries

WASM_FLAGS := --export-dynamic --no-entry --import-memory --allow-undefined --lto-O3
#WASM_FLAGS := --strip-all --export-dynamic --no-entry --import-memory --allow-undefined --lto-O3

SRCS = $(wildcard $(SOURCE_DIR)/*.c)
OBJS = $(patsubst $(SOURCE_DIR)%,$(BUILD_DIR)%,$(SRCS:.c=.o))

SDL_SRCS += $(wildcard $(SOURCE_DIR)/platform/sdl/*.c)
SDL_OBJS = $(patsubst $(SOURCE_DIR)/platform/sdl%,$(BUILD_DIR)%,$(SDL_SRCS:.c=.o))

WASM_SRCS += $(wildcard $(SOURCE_DIR)/platform/wasm/*.c)
WASM_OBJS = $(patsubst $(SOURCE_DIR)/platform/wasm%,$(BUILD_DIR)%,$(WASM_SRCS:.c=.o))

DESRCS = $(wildcard $(DECOMPILE_DIR)/*.c)
DEOBJS = $(patsubst $(DECOMPILE_DIR)%,$(BUILD_DIR)%,$(DESRCS:.c=.o))

.PHONY: clean run test sdl all wasm

sdl: all $(SDL_PROGRAM)

wasm: all $(WASM_PROGRAM)

all: $(BUILD_DIR) $(MEMCARD_DIR)

clean:
	rm -f $(SDL_PROGRAM)
	rm -f $(WASM_PROGRAM)
	rm -rf $(BUILD_DIR)

run: sdl
	./$(SDL_PROGRAM)

test: sdl
	./$(SDL_PROGRAM) test

$(MEMCARD_DIR):
	mkdir --parents $(MEMCARD_DIR)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/platform/sdl/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/platform/wasm/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

$(BUILD_DIR)/%.o: $(DECOMPILE_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I.

$(BUILD_DIR):
	mkdir --parents $(BUILD_DIR)

$(SDL_PROGRAM): $(OBJS) $(SDL_OBJS) $(DEOBJS)
	$(CC) $(CFLAGS) -lSDL3 -L../disassembly/program $^ -o $@

$(WASM_PROGRAM): $(OBJS) $(WASM_OBJS) $(DEOBJS)
	wasm-ld $(WASM_FLAGS) $^ -o $@