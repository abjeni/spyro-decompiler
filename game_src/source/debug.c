#include <main.h>
#include <stdio.h>
#include <engine.h>

#include "debug.h"

void print_trace(void)
{
  /*void *array[10];
  char **strings;
  int size, i;

  size = backtrace (array, 10);
  strings = backtrace_symbols (array, size);
  if (strings != NULL)
  {

    printf ("Obtained %d stack frames.\n", size);
    for (i = 0; i < size; i++)
      printf ("%s\n", strings[i]);
  }

  free (strings);*/
}

void print_all_regs()
{
  printf("regs:\n"
  "pc: %.8x  s0: %.8X\n"
  "at: %.8X  s1: %.8X\n"
  "v0: %.8X  s2: %.8X\n"
  "v1: %.8X  s3: %.8X\n"
  "a0: %.8X  s4: %.8X\n"
  "a1: %.8X  s5: %.8X\n"
  "a2: %.8X  s6: %.8X\n"
  "a3: %.8X  s7: %.8X\n"
  "t0: %.8X  t8: %.8X\n"
  "t1: %.8X  t9: %.8X\n"
  "t2: %.8X  k0: %.8X\n"
  "t3: %.8X  k1: %.8X\n"
  "t4: %.8X  gp: %.8X\n"
  "t5: %.8X  sp: %.8X\n"
  "t6: %.8X  fp: %.8X\n"
  "t7: %.8X  ra: %.8X\n"
  "lo: %.8X  hi: %.8X\n",
  pc, s0, at, s1, v0, s2, v1, s3, a0, s4, a1, s5, a2, s6, a3, s7, t0,
  t8, t1, t9, t2, k0, t3, k1, t4, gp, t5, sp, t6, fp, t7, ra, lo, hi
  );
}

#include "decompilation.h"
#include "extra_gte.h"
#include "psx_mem.h"
#include "spyro_constants.h"

const char *gamestate_labels[] = {
  "GAMESTATE_INGAME",
  "GAMESTATE_LOADING",
  "GAMESTATE_PAUSE",
  "GAMESTATE_INVENTORY",
  "GAMESTATE_DEAD",
  "GAMESTATE_GAMEOVER",
  "GAMESTATE_UNKNOWN",
  "GAMESTATE_FLIGHT_LEVEL_MENU",
  "GAMESTATE_DRAGON_FREED",
  "GAMESTATE_LOADING_TRANSITION",
  "GAMESTATE_PREPARE_LOADING",
  "GAMESTATE_FAIRY",
  "GAMESTATE_BALLOONIST",
  "GAMESTATE_TITLE_SCREEN",
  "GAMESTATE_CUTSCENE",
  "GAMESTATE_CREDITS",
};

void update_debug_text(void)
{
  static char buf[256];

  uint32_t level_id = lw(CONTINUOUS_LEVEL_ID);
  uint32_t world_id = lw(WORLD_ID);

  // TODO: remove the update prefix from debug
  debug_begin();
  {
    sprintf(buf, "player_movestate: %d", lw(player_movestate));
    debug_text(buf);
    sprintf(buf, "gamestate: %s", gamestate_labels[lw(gamestate)]);
    debug_text(buf);
    sprintf(buf, "lives: %d", lw(lives));
    debug_text(buf);
    vec3_32 pos = load_vec3_32(player_position);
    sprintf(buf, "player position: {%d, %d, %d}", pos.x, pos.y, pos.z);
    debug_text(buf);
    vec3_32 d_buttons = load_vec3_32(buttons);
    sprintf(buf, "buttons: 0x%.8X, 0x%.8X, 0x%.8X}", d_buttons.x, d_buttons.y, d_buttons.z);
    debug_text(buf);
    sprintf(buf, "world: %s", homeworld_names[lw(WORLD_ID)]);
    debug_text(buf);
    sprintf(buf, "level: %s", level_names[level_id]);
    debug_text(buf);
    sprintf(buf, "level data: dragons %d gems %d eggs %d 0x%.2X 0x%.2X",
      lw(level_dragons + level_id*4),
      lw(level_gems + level_id*4),
      lw(level_eggs + level_id*4),
      lb(0x80078E78 + level_id),
      lb(0x8007A6A8 + level_id)
    );
    debug_text(buf);
    sprintf(buf, "world data: 0x%.2X 0x%.2X", lw(0x80078618 + world_id*4), lb(0x800758D5 + world_id));
    debug_text(buf);

    uint32_t addrs[] = {
        0x800758D0,
        0x80078BBC,
        0x8007587C,
        0x800756C8,
        0x80075830,
        0x800758E8,
        0x8007580C,
        0x80075838,
        0x8007583C,
    };

    for (size_t i = 0; i < sizeof(addrs)/sizeof(addrs[0]); i++)
    {
      sprintf(buf, "0x%.8X -> 0x%.8X", addrs[i], lw(addrs[i]));
      debug_text(buf);
    }

  }
  debug_end();
}