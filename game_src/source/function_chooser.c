#include "debug.h"
#include "level_loading.h"

#include "alpine_ridge.h"
#include "artisans_home.h"
#include "beastmakers_home.h"
#include "blowhard.h"
#include "cliff_town.h"
#include "credits.h"
#include "crystal_flight.h"
#include "dark_hollow.h"
#include "dark_passage.h"
#include "dr_shemp.h"
#include "dreamweavers_home.h"
#include "dry_canyon.h"
#include "gnasty_gnorc.h"
#include "gnastys_loot.h"
#include "gnastys_world.h"
#include "gnorc_cove.h"
#include "haunted_towers.h"
#include "high_caves.h"
#include "ice_cavern.h"
#include "icy_flight.h"
#include "jacques.h"
#include "lofty_castle.h"
#include "magiccrafters_home.h"
#include "metalhead.h"
#include "misty_bog.h"
#include "night_flight.h"
#include "peacekeepers_home.h"
#include "stone_hill.h"
#include "sunny_flight.h"
#include "terrace_village.h"
#include "title_screen.h"
#include "toasty.h"
#include "town_square.h"
#include "tree_tops.h"
#include "twilight_harbor.h"
#include "wild_flight.h"
#include "wizard_peak.h"

// 0x800785D8 is the end of the exe file
// which functions are currently in memory from address 0x8007AA38 onwards?

void function_8007AA50(void)
{
  function_8007AA50_credits();
}

void function_8007BFD0(void)
{
  function_8007BFD0_credits();
}

void function_8007ABAC(void)
{
  function_8007ABAC_title_screen();
}

void function_8007CEE4(void)
{
  function_8007CEE4_title_screen();
}

void function_800857CC(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_800857CC_artisans_home();
}

void function_8007D9C8(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_8007D9C8_artisans_home();
}

void function_800873E0(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_800873E0_artisans_home();
}

void function_80086134(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_80086134_artisans_home();
}

void function_8007AEA0(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_8007AEA0_artisans_home();
}

void function_8007AFDC(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_8007AFDC_artisans_home();
}

void function_8007B020(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_8007B020_artisans_home();
}

void function_8007CFC0(void)
{
  if (lib_num != LIB_ARTISANS_HOME) BREAKPOINT;
  function_8007CFC0_artisans_home();
}

void function_8008249C(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8008249C_sunny_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084128(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_80084128_sunny_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083330(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_80083330_sunny_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007CFB4(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8007CFB4_sunny_flight();
      return;
    case LIB_NIGHT_FLIGHT:
      function_8007CFB4_night_flight();
      return;
    case LIB_CRYSTAL_FLIGHT:
      function_8007CFB4_crystal_flight();
      return;
    case LIB_WILD_FLIGHT:
      function_8007CFB4_wild_flight();
      return;
    case LIB_ICY_FLIGHT:
      function_8007CFB4_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AE08(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8007AE08_sunny_flight();
      return;
    case LIB_NIGHT_FLIGHT:
      function_8007AE08_night_flight();
      return;
    case LIB_CRYSTAL_FLIGHT:
      function_8007AE08_crystal_flight();
      return;
    case LIB_WILD_FLIGHT:
      function_8007AE08_wild_flight();
      return;
    case LIB_ICY_FLIGHT:
      function_8007AE08_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AEDC(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8007AEDC_sunny_flight();
      return;
    case LIB_NIGHT_FLIGHT:
      function_8007AEDC_night_flight();
      return;
    case LIB_CRYSTAL_FLIGHT:
      function_8007AEDC_crystal_flight();
      return;
    case LIB_WILD_FLIGHT:
      function_8007AEDC_wild_flight();
      return;
    case LIB_ICY_FLIGHT:
      function_8007AEDC_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B1FC(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8007B1FC_sunny_flight();
      return;
    case LIB_NIGHT_FLIGHT:
      function_8007B1FC_night_flight();
      return;
    case LIB_CRYSTAL_FLIGHT:
      function_8007B1FC_crystal_flight();
      return;
    case LIB_WILD_FLIGHT:
      function_8007B1FC_wild_flight();
      return;
    case LIB_ICY_FLIGHT:
      function_8007B1FC_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B68C(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8007B68C_sunny_flight();
      return;
    case LIB_NIGHT_FLIGHT:
      function_8007B68C_night_flight();
      return;
    case LIB_CRYSTAL_FLIGHT:
      function_8007B68C_crystal_flight();
      return;
    case LIB_WILD_FLIGHT:
      function_8007B68C_wild_flight();
      return;
    case LIB_ICY_FLIGHT:
      function_8007B68C_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B4B0(void)
{
  switch(lib_num) {
    case LIB_SUNNY_FLIGHT:
      function_8007B4B0_sunny_flight();
      return;
    case LIB_NIGHT_FLIGHT:
      function_8007B4B0_night_flight();
      return;
    case LIB_CRYSTAL_FLIGHT:
      function_8007B4B0_crystal_flight();
      return;
    case LIB_WILD_FLIGHT:
      function_8007B4B0_wild_flight();
      return;
    case LIB_ICY_FLIGHT:
      function_8007B4B0_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008772C(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_8008772C_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007DA78(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_8007DA78_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800892C4(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_800892C4_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80088098(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_80088098_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AF50(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_8007AF50_stone_hill();
      return;
    case LIB_JACQUES:
      function_8007AF50_jacques();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B08C(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_8007B08C_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B0D0(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_8007B0D0_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D070(void)
{
  switch(lib_num) {
    case LIB_STONE_HILL:
      function_8007D070_stone_hill();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80082960(void)
{
  switch(lib_num) {
    case LIB_DARK_HOLLOW:
      function_80082960_dark_hollow();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AE40(void)
{
  switch(lib_num) {
    case LIB_DARK_HOLLOW:
      function_8007AE40_dark_hollow();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800844A0(void)
{
  switch(lib_num) {
    case LIB_DARK_HOLLOW:
      function_800844A0_dark_hollow();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083274(void)
{
  switch(lib_num) {
    case LIB_DARK_HOLLOW:
      function_80083274_dark_hollow();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800872A4(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_800872A4_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007DA54(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_8007DA54_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80088F68(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_80088F68_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80087E20(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_80087E20_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AF2C(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_8007AF2C_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B068(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_8007B068_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B0AC(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_8007B0AC_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D04C(void)
{
  switch(lib_num) {
    case LIB_TOWN_SQUARE:
      function_8007D04C_town_square();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80081DA8(void)
{
  switch(lib_num) {
    case LIB_TOASTY:
      function_80081DA8_toasty();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AF94(void)
{
  switch(lib_num) {
    case LIB_TOASTY:
      function_8007AF94_toasty();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008391C(void)
{
  switch(lib_num) {
    case LIB_TOASTY:
      function_8008391C_toasty();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800826F0(void)
{
  switch(lib_num) {
    case LIB_TOASTY:
      function_800826F0_toasty();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008A258(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8008A258_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007E3A0(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8007E3A0_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008BFF0(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8008BFF0_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008AE28(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8008AE28_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B878(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8007B878_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B9B4(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8007B9B4_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B9F8(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8007B9F8_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D998(void)
{
  switch(lib_num) {
    case LIB_PEACEKEEPERS_HOME:
      function_8007D998_peacekeepers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008A4D0(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8008A4D0_dry_canyon();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007E240(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8007E240_dry_canyon();
      return;
    case LIB_CLIFF_TOWN:
      function_8007E240_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008C540(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8008C540_dry_canyon();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008B1C0(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8008B1C0_dry_canyon();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B718(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8007B718_dry_canyon();
      return;
    case LIB_CLIFF_TOWN:
      function_8007B718_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B854(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8007B854_dry_canyon();
      return;
    case LIB_CLIFF_TOWN:
      function_8007B854_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B898(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8007B898_dry_canyon();
      return;
    case LIB_CLIFF_TOWN:
      function_8007B898_cliff_town();
      return;
    case LIB_DREAMWEAVERS_HOME:
      function_8007B898_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D838(void)
{
  switch(lib_num) {
    case LIB_DRY_CANYON:
      function_8007D838_dry_canyon();
      return;
    case LIB_CLIFF_TOWN:
      function_8007D838_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80088B88(void)
{
  switch(lib_num) {
    case LIB_CLIFF_TOWN:
      function_80088B88_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008A9A8(void)
{
  switch(lib_num) {
    case LIB_CLIFF_TOWN:
      function_8008A9A8_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80089714(void)
{
  switch(lib_num) {
    case LIB_CLIFF_TOWN:
      function_80089714_cliff_town();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083608(void)
{
  switch(lib_num) {
    case LIB_ICE_CAVERN:
      function_80083608_ice_cavern();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007B4C8(void)
{
  switch(lib_num) {
    case LIB_ICE_CAVERN:
      function_8007B4C8_ice_cavern();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80085184(void)
{
  switch(lib_num) {
    case LIB_ICE_CAVERN:
      function_80085184_ice_cavern();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80083F2C(void)
{
  switch(lib_num) {
    case LIB_ICE_CAVERN:
      function_80083F2C_ice_cavern();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800845F0(void)
{
  switch(lib_num) {
    case LIB_DR_SHEMP:
      function_800845F0_dr_shemp();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007AEB8(void)
{
  switch(lib_num) {
    case LIB_DR_SHEMP:
      function_8007AEB8_dr_shemp();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8008611C(void)
{
  switch(lib_num) {
    case LIB_DR_SHEMP:
      function_8008611C_dr_shemp();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80084EF0(void)
{
  switch(lib_num) {
    case LIB_DR_SHEMP:
      function_80084EF0_dr_shemp();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800819BC(void)
{
  switch(lib_num) {
    case LIB_NIGHT_FLIGHT:
      function_800819BC_night_flight();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80082F58(void)
{
  switch(lib_num) {
    case LIB_NIGHT_FLIGHT:
      function_80082F58_night_flight();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80082300(void)
{
  switch(lib_num) {
    case LIB_NIGHT_FLIGHT:
      function_80082300_night_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008B2C0(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8008B2C0_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007E398(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8007E398_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8008D2D0(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8008D2D0_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8008BE98(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8008BE98_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007B870(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8007B870_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007B9AC(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8007B9AC_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007B9F0(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8007B9F0_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007D990(void)
{
  switch(lib_num) {
    case LIB_MAGICCRAFTERS_HOME:
      function_8007D990_magiccrafters_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008A36C(void)
{
  switch(lib_num) {
    case LIB_ALPINE_RIDGE:
      function_8008A36C_alpine_ridge();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007BB00(void)
{
  switch(lib_num) {
    case LIB_ALPINE_RIDGE:
      function_8007BB00_alpine_ridge();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8008C9D8(void)
{
  switch(lib_num) {
    case LIB_ALPINE_RIDGE:
      function_8008C9D8_alpine_ridge();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8008AF54(void)
{
  switch(lib_num) {
    case LIB_ALPINE_RIDGE:
      function_8008AF54_alpine_ridge();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008883C(void)
{
  switch(lib_num) {
    case LIB_HIGH_CAVES:
      function_8008883C_high_caves();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8007B64C(void)
{
  switch(lib_num) {
    case LIB_HIGH_CAVES:
      function_8007B64C_high_caves();
      return;
    default:
      BREAKPOINT;
  }
}
void function_8008B0B0(void)
{
  switch(lib_num) {
    case LIB_HIGH_CAVES:
      function_8008B0B0_high_caves();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80089454(void)
{
  switch(lib_num) {
    case LIB_HIGH_CAVES:
      function_80089454_high_caves();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80086DD8(void)
{
  switch(lib_num) {
    case LIB_WIZARD_PEAK:
      function_80086DD8_wizard_peak();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B7A8(void)
{
  switch(lib_num) {
    case LIB_WIZARD_PEAK:
      function_8007B7A8_wizard_peak();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80089450(void)
{
  switch(lib_num) {
    case LIB_WIZARD_PEAK:
      function_80089450_wizard_peak();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80087B40(void)
{
  switch(lib_num) {
    case LIB_WIZARD_PEAK:
      function_80087B40_wizard_peak();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083AB4(void)
{
  switch(lib_num) {
    case LIB_BLOWHARD:
      function_80083AB4_blowhard();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AF28(void)
{
  switch(lib_num) {
    case LIB_BLOWHARD:
      function_8007AF28_blowhard();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80085F40(void)
{
  switch(lib_num) {
    case LIB_BLOWHARD:
      function_80085F40_blowhard();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084830(void)
{
  switch(lib_num) {
    case LIB_BLOWHARD:
      function_80084830_blowhard();
      return;
    default:
      BREAKPOINT;
  }
}


void function_80081F0C(void)
{
  switch(lib_num) {
    case LIB_CRYSTAL_FLIGHT:
      function_80081F0C_crystal_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800836F8(void)
{
  switch(lib_num) {
    case LIB_CRYSTAL_FLIGHT:
      function_800836F8_crystal_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80082AA0(void)
{
  switch(lib_num) {
    case LIB_CRYSTAL_FLIGHT:
      function_80082AA0_crystal_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80087EF0(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_80087EF0_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007E18C(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_8007E18C_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80089AB8(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_80089AB8_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800888F8(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_800888F8_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B664(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_8007B664_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B7A0(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_8007B7A0_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B7E4(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_8007B7E4_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D784(void)
{
  switch(lib_num) {
    case LIB_BEASTMAKERS_HOME:
      function_8007D784_beastmakers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008465C(void)
{
  switch(lib_num) {
    case LIB_TERRACE_VILLAGE:
      function_8008465C_terrace_village();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B5DC(void)
{
  switch(lib_num) {
    case LIB_TERRACE_VILLAGE:
      function_8007B5DC_terrace_village();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800866D8(void)
{
  switch(lib_num) {
    case LIB_TERRACE_VILLAGE:
      function_800866D8_terrace_village();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800853AC(void)
{
  switch(lib_num) {
    case LIB_TERRACE_VILLAGE:
      function_800853AC_terrace_village();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084718(void)
{
  switch(lib_num) {
    case LIB_MISTY_BOG:
      function_80084718_misty_bog();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AFBC(void)
{
  switch(lib_num) {
    case LIB_MISTY_BOG:
      function_8007AFBC_misty_bog();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800861CC(void)
{
  switch(lib_num) {
    case LIB_MISTY_BOG:
      function_800861CC_misty_bog();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80085084(void)
{
  switch(lib_num) {
    case LIB_MISTY_BOG:
      function_80085084_misty_bog();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80086B38(void)
{
  switch(lib_num) {
    case LIB_TREE_TOPS:
      function_80086B38_tree_tops();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B698(void)
{
  switch(lib_num) {
    case LIB_TREE_TOPS:
      function_8007B698_tree_tops();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008869C(void)
{
  switch(lib_num) {
    case LIB_TREE_TOPS:
      function_8008869C_tree_tops();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80087400(void)
{
  switch(lib_num) {
    case LIB_TREE_TOPS:
      function_80087400_tree_tops();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800874FC(void)
{
  switch(lib_num) {
    case LIB_METALHEAD:
      function_800874FC_metalhead();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B770(void)
{
  switch(lib_num) {
    case LIB_METALHEAD:
      function_8007B770_metalhead();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800894B0(void)
{
  switch(lib_num) {
    case LIB_METALHEAD:
      function_800894B0_metalhead();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80088178(void)
{
  switch(lib_num) {
    case LIB_METALHEAD:
      function_80088178_metalhead();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008223C(void)
{
  switch(lib_num) {
    case LIB_WILD_FLIGHT:
      function_8008223C_wild_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083BAC(void)
{
  switch(lib_num) {
    case LIB_WILD_FLIGHT:
      function_80083BAC_wild_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80082F54(void)
{
  switch(lib_num) {
    case LIB_WILD_FLIGHT:
      function_80082F54_wild_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80088E24(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_80088E24_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007E3C0(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_8007E3C0_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008AA24(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_8008AA24_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800897FC(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_800897FC_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B9D4(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_8007B9D4_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007BA18(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_8007BA18_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D9B8(void)
{
  switch(lib_num) {
    case LIB_DREAMWEAVERS_HOME:
      function_8007D9B8_dreamweavers_home();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084B94(void)
{
  switch(lib_num) {
    case LIB_DARK_PASSAGE:
      function_80084B94_dark_passage();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B4F8(void)
{
  switch(lib_num) {
    case LIB_DARK_PASSAGE:
      function_8007B4F8_dark_passage();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80086D38(void)
{
  switch(lib_num) {
    case LIB_DARK_PASSAGE:
      function_80086D38_dark_passage();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800857FC(void)
{
  switch(lib_num) {
    case LIB_DARK_PASSAGE:
      function_800857FC_dark_passage();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084620(void)
{
  switch(lib_num) {
    case LIB_LOFTY_CASTLE:
      function_80084620_lofty_castle();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B4DC(void)
{
  switch(lib_num) {
    case LIB_LOFTY_CASTLE:
      function_8007B4DC_lofty_castle();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80086438(void)
{
  switch(lib_num) {
    case LIB_LOFTY_CASTLE:
      function_80086438_lofty_castle();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800850A0(void)
{
  switch(lib_num) {
    case LIB_LOFTY_CASTLE:
      function_800850A0_lofty_castle();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008590C(void)
{
  switch(lib_num) {
    case LIB_HAUNTED_TOWERS:
      function_8008590C_haunted_towers();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B510(void)
{
  switch(lib_num) {
    case LIB_HAUNTED_TOWERS:
      function_8007B510_haunted_towers();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800881D8(void)
{
  switch(lib_num) {
    case LIB_HAUNTED_TOWERS:
      function_800881D8_haunted_towers();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80086754(void)
{
  switch(lib_num) {
    case LIB_HAUNTED_TOWERS:
      function_80086754_haunted_towers();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800836A8(void)
{
  switch(lib_num) {
    case LIB_JACQUES:
      function_800836A8_jacques();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80085254(void)
{
  switch(lib_num) {
    case LIB_JACQUES:
      function_80085254_jacques();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084028(void)
{
  switch(lib_num) {
    case LIB_JACQUES:
      function_80084028_jacques();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80082028(void)
{
  switch(lib_num) {
    case LIB_ICY_FLIGHT:
      function_80082028_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80083B8C(void)
{
  switch(lib_num) {
    case LIB_ICY_FLIGHT:
      function_80083B8C_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}
void function_80082D94(void)
{
  switch(lib_num) {
    case LIB_ICY_FLIGHT:
      function_80082D94_icy_flight();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083568(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_80083568_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007D938(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_8007D938_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084EA0(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_80084EA0_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083ED8(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_80083ED8_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AE10(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_8007AE10_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AF4C(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_8007AF4C_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AF90(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_8007AF90_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007CF30(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_WORLD:
      function_8007CF30_gnastys_world();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80085664(void)
{
  switch(lib_num) {
    case LIB_GNORC_COVE:
      function_80085664_gnorc_cove();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007B528(void)
{
  switch(lib_num) {
    case LIB_GNORC_COVE:
      function_8007B528_gnorc_cove();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8008747C(void)
{
  switch(lib_num) {
    case LIB_GNORC_COVE:
      function_8008747C_gnorc_cove();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80086144(void)
{
  switch(lib_num) {
    case LIB_GNORC_COVE:
      function_80086144_gnorc_cove();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083108(void)
{
  switch(lib_num) {
    case LIB_TWILIGHT_HARBOR:
      function_80083108_twilight_harbor();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AE5C(void)
{
  switch(lib_num) {
    case LIB_TWILIGHT_HARBOR:
      function_8007AE5C_twilight_harbor();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084EAC(void)
{
  switch(lib_num) {
    case LIB_TWILIGHT_HARBOR:
      function_80084EAC_twilight_harbor();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083B4C(void)
{
  switch(lib_num) {
    case LIB_TWILIGHT_HARBOR:
      function_80083B4C_twilight_harbor();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80082F24(void)
{
  switch(lib_num) {
    case LIB_GNASTY_GNORC:
      function_80082F24_gnasty_gnorc();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AD64(void)
{
  switch(lib_num) {
    case LIB_GNASTY_GNORC:
      function_8007AD64_gnasty_gnorc();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80084634(void)
{
  switch(lib_num) {
    case LIB_GNASTY_GNORC:
      function_80084634_gnasty_gnorc();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800836F0(void)
{
  switch(lib_num) {
    case LIB_GNASTY_GNORC:
      function_800836F0_gnasty_gnorc();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80083690(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_LOOT:
      function_80083690_gnastys_loot();
      return;
    default:
      BREAKPOINT;
  }
}

void function_8007AD4C(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_LOOT:
      function_8007AD4C_gnastys_loot();
      return;
    default:
      BREAKPOINT;
  }
}

void function_80085230(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_LOOT:
      function_80085230_gnastys_loot();
      return;
    default:
      BREAKPOINT;
  }
}

void function_800840FC(void)
{
  switch(lib_num) {
    case LIB_GNASTYS_LOOT:
      function_800840FC_gnastys_loot();
      return;
    default:
      BREAKPOINT;
  }
}
