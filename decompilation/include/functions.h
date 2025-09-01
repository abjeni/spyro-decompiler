#pragma once

#include <stdint.h>

#define FUNCTION_CALLED 1
#define FUNCTION_INCOMPLETE 2
#define FUNCTION_HAS_TEMP 4

typedef struct {
    uint32_t *addrs;
    uint32_t alloc;
    uint32_t size;
} addr_list;

typedef struct {
  uint32_t address;
  uint32_t size;
  uint32_t end;
  addr_list jumps;
  addr_list calls;
  uint32_t function_info;
  uint32_t has_temp;
} function;

typedef struct {
  function *funcs;
  uint32_t funcs_alloc;
  uint32_t size;
} function_list;

addr_list addr_list_alloc();
void addr_list_free(addr_list list);
void addr_list_insert(addr_list *list, uint32_t addr);
void addr_list_sort(addr_list *list);
int addr_list_contains(addr_list list, uint32_t addr);

function function_alloc();
void function_free(function func);
void function_add_jump(function *func, uint32_t jump);
void function_add_call(function *func, uint32_t call);
void function_sort(function *func);

function_list function_list_alloc();
void function_list_free(function_list fl);
int function_contains_address(function func, uint32_t addr);
int function_list_contains_address(function_list *func_list, uint32_t addr);
void function_list_insert(function_list *func_list, function func);
void function_list_sort(function_list *func_list);