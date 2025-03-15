#include <stdint.h>

void wait_until_frame(int32_t frame, uint32_t timeout_time);
int VSync(int mode);
uint32_t set_I_MASK(uint32_t i_mask);