#pragma once
#include <stdint.h>

/////////////////
// spyro_psy.h //
/////////////////

int32_t ResetGraph(int32_t mode);
void SetDispMask(int32_t mask);

/////////////////
// spyro_psy.h //
/////////////////

////////////////////
// spyro_system.h //
////////////////////

uint32_t dma_callback(uint32_t dma_num, uint32_t callback);
uint32_t init_hook_entry_int2(void);
void spyro_srand(int32_t seed);
int spyro_rand(void);

////////////////////
// spyro_system.h //
////////////////////

////////////////////////
// spyro_controller.h //
////////////////////////

void init_controller(void);
void controller_wait_for_data(void);
void stop_registering_gamepads(void);

////////////////////////
// spyro_controller.h //
////////////////////////
