#pragma once

#define WASM_PUBLIC __attribute__((visibility("default")))
#define WASM_LOCAL __attribute__((visibility("hidden")))