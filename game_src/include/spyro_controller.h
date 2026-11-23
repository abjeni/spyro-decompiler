#pragma once

void init_controller(void);
void controller_wait_for_data(void);
void stop_registering_gamepads(void);

#define BUTTONS_MASK_TRIANGLE 0x10
#define BUTTONS_MASK_SQUARE 0x20
#define BUTTONS_MASK_CROSS 0x40
#define BUTTONS_MASK_CIRCLE 0x80

#define BUTTONS_MASK_SELECT 0x100
#define BUTTONS_MASK_START 0x800

#define BUTTONS_MASK_L1 0x1
#define BUTTONS_MASK_R1 0x2
#define BUTTONS_MASK_L2 0x4
#define BUTTONS_MASK_R2 0x8

#define BUTTONS_MASK_UP 0x1000
#define BUTTONS_MASK_RIGHT 0x2000
#define BUTTONS_MASK_DOWN 0x4000
#define BUTTONS_MASK_LEFT 0x8000