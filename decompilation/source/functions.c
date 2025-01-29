#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "functions.h"

addr_list addr_list_alloc()
{
  addr_list list = {};

  list.alloc = 16;
  list.addrs = malloc(sizeof(*list.addrs)*list.alloc);

  return list;
}

void addr_list_free(addr_list list)
{
    free(list.addrs);
}

void addr_list_insert(addr_list *list, uint32_t addr)
{
  if (list->size >= list->alloc) {
    list->alloc *= 2;
    list->addrs = realloc(list->addrs, sizeof(*list->addrs)*list->alloc);
  }

  list->addrs[list->size] = addr;
  list->size++;
}

int addr_list_contains(addr_list list, uint32_t addr)
{
  for (int i = 0; i < list.size; i++)
    if (list.addrs[i] == addr)
      return 1;

  return 0;
}

void addr_list_sort(addr_list *list)
{
  if (list->size == 0) return;

  for (int i = 0; i < (int)list->size-1; i++)
  for (int j = 0; j < (int)list->size-1-i; j++)
  {
    uint32_t addr1 = list->addrs[j+0];
    uint32_t addr2 = list->addrs[j+1];

    if (addr1 > addr2)
    {
      list->addrs[j+0] = addr2;
      list->addrs[j+1] = addr1;
    }
  }

  int j = 0;
  for (int i = 1; i < list->size; i++)
  {
    if (list->addrs[j] != list->addrs[i])
    {
      j++;
      list->addrs[j] = list->addrs[i];
    }
  }
  list->size = j+1;
}


function function_alloc()
{
  function func = {};

  func.jumps = addr_list_alloc();
  func.calls = addr_list_alloc();

  return func;
}

void function_free(function func)
{
  addr_list_free(func.jumps);
  addr_list_free(func.calls);
}

void function_add_jump(function *func, uint32_t jump)
{
  assert(jump >= 0x80000000 && jump < 0x80200000);
  addr_list_insert(&func->jumps, jump);
}

void function_add_call(function *func, uint32_t call)
{
  assert(call >= 0x80000000 && call < 0x80200000);
  addr_list_insert(&func->calls, call);
}

void function_sort(function *func)
{
  addr_list_sort(&func->jumps);
  addr_list_sort(&func->calls);
}

function_list function_list_alloc()
{
  uint32_t funcs = 128;
  return (function_list){
    .funcs_alloc = funcs,
    .funcs = malloc(sizeof(function)*funcs)
  };
}

void function_list_free(function_list fl)
{
  for (int i = 0; i < fl.size; i++)
    function_free(fl.funcs[i]);

  free(fl.funcs);
}

int function_contains_address(function func, uint32_t addr)
{
  return addr >= func.address && addr <= func.end;
}

int function_extends(function func1, function func2)
{
  return func1.end == func2.end;
}

int function_list_contains_address(function_list *func_list, uint32_t addr)
{
  for (int i = 0; i < func_list->size; i++)
    if (function_contains_address(func_list->funcs[i], addr))
      return i+1;

  return 0;
}

int function_list_function_extends(function_list *func_list, function func) {
  for (int i = 0; i < func_list->size; i++)
    if (function_extends(func_list->funcs[i], func))
      return i+1;

  return 0;
}

void function_list_insert(function_list *func_list, function func)
{
  int func_extends = function_list_function_extends(func_list, func);
  if (func_extends)
  {
    //printf("function extends [%8X - %8X] with %d [%8X - %8X]\n", func.address, func.end, func_extends-1, func_list->funcs[func_extends-1].address, func_list->funcs[func_extends-1].end);
    //function_free(func_list->funcs[func_extends-1]);
    func_list->funcs[func_extends-1] = func;
    return;
  }

  if (func_list->size >= func_list->funcs_alloc)
  {
    func_list->funcs_alloc *= 2;
    func_list->funcs = realloc(func_list->funcs, sizeof(function)*func_list->funcs_alloc);
  }

  //printf("adding function %d [%8X - %8X]\n", func_list->size, func.address, func.end);

  func_list->funcs[func_list->size] = func;
  func_list->size++;
}

void function_list_sort(function_list *func_list)
{
  for (int i = 0; i < func_list->size; i++)
  {
    function func = func_list->funcs[i];
    function_sort(&func);
  }

  for (int i = 0; i < (int)func_list->size-1; i++)
  for (int j = 0; j < (int)func_list->size-1-i; j++)
  {
    function func1 = func_list->funcs[j+0];
    function func2 = func_list->funcs[j+1];

    if (func1.address > func2.address)
    {
      func_list->funcs[j+0] = func2;
      func_list->funcs[j+1] = func1;
    }
  }
}