#include <stdint.h>

uint32_t spyro_set_drawing_area_top_left_command(int16_t x, int16_t y);
uint32_t spyro_set_drawing_area_bottom_right_command(int16_t x, int16_t y);
uint32_t spyro_set_drawing_offset_command(uint32_t x, uint32_t y);
uint32_t spyro_draw_mode_setting_command(uint32_t allow_drawing_to_display, uint32_t dithering_enabled, uint32_t tex_page);
uint32_t spyro_set_texture_window_setting_command(uint32_t texture_window);
void spyro_clear_screen(uint32_t cmd_ptr, uint32_t screen_info);