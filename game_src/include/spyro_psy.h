#include <stdint.h>

uint32_t ResetGraph(uint32_t mode);
void SetDispMask(uint32_t mask);
uint32_t DrawSync(uint32_t mode);
uint32_t SetGraphDebug(uint32_t level);
uint32_t ClearImage(uint32_t box_ptr, uint32_t r, uint32_t g, uint32_t b);
uint32_t LoadImage(uint32_t box_ptr, uint32_t img_ptr);
uint32_t StoreImage(uint32_t box_ptr, uint32_t img_ptr);
uint32_t MoveImage(uint32_t box_ptr, uint32_t x, uint32_t y);
void DrawOTag(uint32_t of);
uint32_t PutDrawEnv(uint32_t env_ptr);
uint32_t PutDispEnv(uint32_t env_ptr);