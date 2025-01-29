#include <stdint.h>

uint32_t spyro_wait_frames(uint32_t wait_frames);
void interrupt_entry_hook(void);
uint32_t update_I_MASK(uint32_t new_mask);
uint32_t spyro_set_dma_interrupt_callback_wrapper(uint32_t int_num, uint32_t func);