#include <stdint.h>
#include "main.h"
#include "decompilation.h"

/////////////////
// spyro_psy.h //
/////////////////

int32_t ResetGraph(int32_t mode)
{
  a0 = mode;
  function_8005F2A4();
  return v0;
}

void SetDispMask(int32_t mask)
{
  a0 = mask;
  function_8005F6C8();
}

void spyro_srand(int32_t seed)
{
    a0 = seed;
    function_8006275C();
}

int spyro_rand(void)
{
    function_8006272C();
    return v0;
}

/////////////////
// spyro_psy.h //
/////////////////

////////////////////
// spyro_system.h //
////////////////////

uint32_t dma_callback(uint32_t dma_num, uint32_t callback)
{
  a0 = dma_num;
  a1 = callback;
  function_8005E804();
  return v0;
}

uint32_t init_hook_entry_int2(void)
{
  function_8005DDC8();
  return v0;
}

////////////////////
// spyro_system.h //
////////////////////

////////////////////////
// spyro_controller.h //
////////////////////////

void init_controller(void)
{
    function_800123C8();
}

void controller_wait_for_data(void)
{
    function_8006A0A4();
}

void stop_registering_gamepads(void)
{
    function_80069060();
}

////////////////////////
// spyro_controller.h //
////////////////////////