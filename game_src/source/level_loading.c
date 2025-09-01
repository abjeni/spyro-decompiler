#include "main.h"
#include "psx_mem.h"
#include "level_loading.h"
#include "decompilation.h"
#include "function_chooser.h"

uint32_t lib_num = LIB_TITLE_SCREEN;

// size: 0x00001288
uint32_t update_level_functions(void)
{
  v1 = lw(LEVEL_ID);
  switch (v1)
  {
  case 0:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x80082068);
    sw(0x800758CC, 0x80080548);
    sw(0x80075734, 0x8007D8E0);
    sw(0x800758E4, 0x80081568);
    sw(0x800756BC, 0x80080A0C);
    sw(0x800757A0, 0x8007ADB8);
    sw(0x800758A8, 0x8007AEF4);
    sw(0x8007574C, 0x8007AF38);
    sw(0x800758D8, 0x8007CED8);
    break;
  case 1:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007BFF0);
    sw(0x800758CC, 0x8007B070);
    sw(0x80075734, 0x8007AC8C);
    sw(0x800758E4, 0x8007B8AC);
    sw(0x800756BC, 0x8007B144);
    break;
  case 2:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007F490);
    sw(0x800758CC, 0x8007D970);
    sw(0x80075734, 0x8007AD08);
    sw(0x800758E4, 0x8007E990);
    sw(0x800756BC, 0x8007DE34);
    break;
  case 3:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007C7B0);
    sw(0x800758CC, 0x8007B0AC);
    sw(0x80075734, 0x8007ACC8);
    sw(0x800758E4, 0x8007BC50);
    sw(0x800756BC, 0x8007B180);
    break;
  case 4:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007C204);
    sw(0x800758CC, 0x8007B08C);
    sw(0x80075734, 0x8007ACA8);
    sw(0x800758E4, 0x8007B9F0);
    sw(0x800756BC, 0x8007B160);
    break;
  case 7:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007F2F4);
    sw(0x800758CC, 0x8007D970);
    sw(0x80075734, 0x8007AD08);
    sw(0x800758E4, 0x8007E8E4);
    sw(0x800756BC, 0x8007DE34);
    break;
  case 9:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007C204);
    sw(0x800758CC, 0x8007B08C);
    sw(0x80075734, 0x8007ACA8);
    sw(0x800758E4, 0x8007B9F0);
    sw(0x800756BC, 0x8007B160);
    break;
  case 10:
    lib_num = LIB_ARTISANS_HOME;
    sw(0x800785D8, 0x80088620);
    sw(0x800758CC, 0x800857CC);
    sw(0x80075734, 0x8007D9C8);
    sw(0x800758E4, 0x800873E0);
    sw(0x800756BC, 0x80086134);
    sw(0x800757A0, 0x8007AEA0);
    sw(0x800758A8, 0x8007AFDC);
    sw(0x8007574C, 0x8007B020);
    sw(0x800758D8, 0x8007CFC0);
    break;
  case 11:
    lib_num = LIB_STONE_HILL;
    sw(0x800785D8, 0x8008A3B8);
    sw(0x800758CC, 0x8008772C);
    sw(0x80075734, 0x8007DA78);
    sw(0x800758E4, 0x800892C4);
    sw(0x800756BC, 0x80088098);
    sw(0x800757A0, 0x8007AF50);
    sw(0x800758A8, 0x8007B08C);
    sw(0x8007574C, 0x8007B0D0);
    sw(0x800758D8, 0x8007D070);
    break;
  case 12:
    lib_num = LIB_DARK_HOLLOW;
    sw(0x800785D8, 0x80085594);
    sw(0x800758CC, 0x80082960);
    sw(0x80075734, 0x8007AE40);
    sw(0x800758E4, 0x800844A0);
    sw(0x800756BC, 0x80083274);
    break;
  case 13:
    lib_num = LIB_TOWN_SQUARE;
    sw(0x800785D8, 0x80089ECC);
    sw(0x800758CC, 0x800872A4);
    sw(0x80075734, 0x8007DA54);
    sw(0x800758E4, 0x80088F68);
    sw(0x800756BC, 0x80087E20);
    sw(0x800757A0, 0x8007AF2C);
    sw(0x800758A8, 0x8007B068);
    sw(0x8007574C, 0x8007B0AC);
    sw(0x800758D8, 0x8007D04C);
    break;
  case 14:
    lib_num = LIB_TOASTY;
    sw(0x800785D8, 0x80084A10);
    sw(0x800758CC, 0x80081DA8);
    sw(0x80075734, 0x8007AF94);
    sw(0x800758E4, 0x8008391C);
    sw(0x800756BC, 0x800826F0);
    break;
  case 15:
    lib_num = LIB_SUNNY_FLIGHT;
    sw(0x800785D8, 0x80084ED0);
    sw(0x800758CC, 0x8008249C);
    sw(0x80075734, 0x8007CFB4);
    sw(0x800758E4, 0x80084128);
    sw(0x800756BC, 0x80083330);
    sw(0x80075694, 0x8007AE08);
    sw(0x800757A8, 0x8007AEDC);
    sw(0x800757C0, 0x8007B1FC);
    sw(0x8007567C, 0x8007B68C);
    sw(0x800758C4, 0x8007B4B0);
    break;
  case 20:
    lib_num = LIB_PEACEKEEPERS_HOME;
    sw(0x800785D8, 0x8008CFA4);
    sw(0x800758CC, 0x8008A258);
    sw(0x80075734, 0x8007E3A0);
    sw(0x800758E4, 0x8008BFF0);
    sw(0x800756BC, 0x8008AE28);
    sw(0x800757A0, 0x8007B878);
    sw(0x800758A8, 0x8007B9B4);
    sw(0x8007574C, 0x8007B9F8);
    sw(0x800758D8, 0x8007D998);
    break;
  case 21:
    lib_num = LIB_DRY_CANYON;
    sw(0x800785D8, 0x8008D600);
    sw(0x800758CC, 0x8008A4D0);
    sw(0x80075734, 0x8007E240);
    sw(0x800758E4, 0x8008C540);
    sw(0x800756BC, 0x8008B1C0);
    sw(0x800757A0, 0x8007B718);
    sw(0x800758A8, 0x8007B854);
    sw(0x8007574C, 0x8007B898);
    sw(0x800758D8, 0x8007D838);
    break;
  case 22:
    lib_num = LIB_CLIFF_TOWN;
    sw(0x800785D8, 0x8008BAF8);
    sw(0x800758CC, 0x80088B88);
    sw(0x80075734, 0x8007E240);
    sw(0x800758E4, 0x8008A9A8);
    sw(0x800756BC, 0x80089714);
    sw(0x800757A0, 0x8007B718);
    sw(0x800758A8, 0x8007B854);
    sw(0x8007574C, 0x8007B898);
    sw(0x800758D8, 0x8007D838);
    break;
  case 23:
    lib_num = LIB_ICE_CAVERN;
    sw(0x800785D8, 0x80086260);
    sw(0x800758CC, 0x80083608);
    sw(0x80075734, 0x8007B4C8);
    sw(0x800758E4, 0x80085184);
    sw(0x800756BC, 0x80083F2C);
    break;
  case 24:
    lib_num = LIB_DR_SHEMP;
    sw(0x800785D8, 0x80087210);
    sw(0x800758CC, 0x800845F0);
    sw(0x80075734, 0x8007AEB8);
    sw(0x800758E4, 0x8008611C);
    sw(0x800756BC, 0x80084EF0);
    break;
  case 25:
    lib_num = LIB_NIGHT_FLIGHT;
    sw(0x800785D8, 0x80083BF0);
    sw(0x800758CC, 0x800819BC);
    sw(0x80075734, 0x8007CFB4);
    sw(0x800758E4, 0x80082F58);
    sw(0x800756BC, 0x80082300);
    sw(0x80075694, 0x8007AE08);
    sw(0x800757A8, 0x8007AEDC);
    sw(0x800757C0, 0x8007B1FC);
    sw(0x8007567C, 0x8007B68C);
    sw(0x800758C4, 0x8007B4B0);
    break;
  case 30:
    lib_num = LIB_MAGICCRAFTERS_HOME;
    sw(0x800785D8, 0x8008E608);
    sw(0x800758CC, 0x8008B2C0);
    sw(0x80075734, 0x8007E398);
    sw(0x800758E4, 0x8008D2D0);
    sw(0x800756BC, 0x8008BE98);
    sw(0x800757A0, 0x8007B870);
    sw(0x800758A8, 0x8007B9AC);
    sw(0x8007574C, 0x8007B9F0);
    sw(0x800758D8, 0x8007D990);
    break;
  case 31:
    lib_num = LIB_ALPINE_RIDGE;
    sw(0x800785D8, 0x8008DEC0);
    sw(0x800758CC, 0x8008A36C);
    sw(0x80075734, 0x8007BB00);
    sw(0x800758E4, 0x8008C9D8);
    sw(0x800756BC, 0x8008AF54);
    break;
  case 32:
    lib_num = LIB_HIGH_CAVES;
    sw(0x800785D8, 0x8008C73C);
    sw(0x800758CC, 0x8008883C);
    sw(0x80075734, 0x8007B64C);
    sw(0x800758E4, 0x8008B0B0);
    sw(0x800756BC, 0x80089454);
    break;
  case 33:
    lib_num = LIB_WIZARD_PEAK;
    sw(0x800785D8, 0x8008A8A0);
    sw(0x800758CC, 0x80086DD8);
    sw(0x80075734, 0x8007B7A8);
    sw(0x800758E4, 0x80089450);
    sw(0x800756BC, 0x80087B40);
    break;
  case 34:
    lib_num = LIB_BLOWHARD;
    sw(0x800785D8, 0x8008749C);
    sw(0x800758CC, 0x80083AB4);
    sw(0x80075734, 0x8007AF28);
    sw(0x800758E4, 0x80085F40);
    sw(0x800756BC, 0x80084830);
    break;
  case 35:
    lib_num = LIB_CRYSTAL_FLIGHT;
    sw(0x800785D8, 0x80084390);
    sw(0x800758CC, 0x80081F0C);
    sw(0x80075734, 0x8007CFB4);
    sw(0x800758E4, 0x800836F8);
    sw(0x800756BC, 0x80082AA0);
    sw(0x80075694, 0x8007AE08);
    sw(0x800757A8, 0x8007AEDC);
    sw(0x800757C0, 0x8007B1FC);
    sw(0x8007567C, 0x8007B68C);
    sw(0x800758C4, 0x8007B4B0);
    break;
  case 40:
    lib_num = LIB_BEASTMAKERS_HOME;
    sw(0x800785D8, 0x8008AB70);
    sw(0x800758CC, 0x80087EF0);
    sw(0x80075734, 0x8007E18C);
    sw(0x800758E4, 0x80089AB8);
    sw(0x800756BC, 0x800888F8);
    sw(0x800757A0, 0x8007B664);
    sw(0x800758A8, 0x8007B7A0);
    sw(0x8007574C, 0x8007B7E4);
    sw(0x800758D8, 0x8007D784);
    break;
  case 41:
    lib_num = LIB_TERRACE_VILLAGE;
    sw(0x800785D8, 0x80087944);
    sw(0x800758CC, 0x8008465C);
    sw(0x80075734, 0x8007B5DC);
    sw(0x800758E4, 0x800866D8);
    sw(0x800756BC, 0x800853AC);
    break;
  case 42:
    lib_num = LIB_MISTY_BOG;
    sw(0x800785D8, 0x80087130);
    sw(0x800758CC, 0x80084718);
    sw(0x80075734, 0x8007AFBC);
    sw(0x800758E4, 0x800861CC);
    sw(0x800756BC, 0x80085084);
    break;
  case 43:
    lib_num = LIB_TREE_TOPS;
    sw(0x800785D8, 0x80089848);
    sw(0x800758CC, 0x80086B38);
    sw(0x80075734, 0x8007B698);
    sw(0x800758E4, 0x8008869C);
    sw(0x800756BC, 0x80087400);
    break;
  case 44:
    lib_num = LIB_METALHEAD;
    sw(0x800785D8, 0x8008A69C);
    sw(0x800758CC, 0x800874FC);
    sw(0x80075734, 0x8007B770);
    sw(0x800758E4, 0x800894B0);
    sw(0x800756BC, 0x80088178);
    break;
  case 45:
    lib_num = LIB_WILD_FLIGHT;
    sw(0x800785D8, 0x80084844);
    sw(0x800758CC, 0x8008223C);
    sw(0x80075734, 0x8007CFB4);
    sw(0x800758E4, 0x80083BAC);
    sw(0x800756BC, 0x80082F54);
    sw(0x80075694, 0x8007AE08);
    sw(0x800757A8, 0x8007AEDC);
    sw(0x800757C0, 0x8007B1FC);
    sw(0x8007567C, 0x8007B68C);
    sw(0x800758C4, 0x8007B4B0);
    break;
  case 50:
    lib_num = LIB_DREAMWEAVERS_HOME;
    sw(0x800785D8, 0x8008BB38);
    sw(0x800758CC, 0x80088E24);
    sw(0x80075734, 0x8007E3C0);
    sw(0x800758E4, 0x8008AA24);
    sw(0x800756BC, 0x800897FC);
    sw(0x800757A0, 0x8007B898);
    sw(0x800758A8, 0x8007B9D4);
    sw(0x8007574C, 0x8007BA18);
    sw(0x800758D8, 0x8007D9B8);
    break;
  case 51:
    lib_num = LIB_DARK_PASSAGE;
    sw(0x800785D8, 0x800880D4);
    sw(0x800758CC, 0x80084B94);
    sw(0x80075734, 0x8007B4F8);
    sw(0x800758E4, 0x80086D38);
    sw(0x800756BC, 0x800857FC);
    break;
  case 52:
    lib_num = LIB_LOFTY_CASTLE;
    sw(0x800785D8, 0x8008771C);
    sw(0x800758CC, 0x80084620);
    sw(0x80075734, 0x8007B4DC);
    sw(0x800758E4, 0x80086438);
    sw(0x800756BC, 0x800850A0);
    break;
  case 53:
    lib_num = LIB_HAUNTED_TOWERS;
    sw(0x800785D8, 0x80089820);
    sw(0x800758CC, 0x8008590C);
    sw(0x80075734, 0x8007B510);
    sw(0x800758E4, 0x800881D8);
    sw(0x800756BC, 0x80086754);
    break;
  case 54:
    lib_num = LIB_JACQUES;
    sw(0x800785D8, 0x80086348);
    sw(0x800758CC, 0x800836A8);
    sw(0x80075734, 0x8007AF50);
    sw(0x800758E4, 0x80085254);
    sw(0x800756BC, 0x80084028);
    break;
  case 55:
    lib_num = LIB_ICY_FLIGHT;
    sw(0x800785D8, 0x80084934);
    sw(0x800758CC, 0x80082028);
    sw(0x80075734, 0x8007CFB4);
    sw(0x800758E4, 0x80083B8C);
    sw(0x800756BC, 0x80082D94);
    sw(0x80075694, 0x8007AE08);
    sw(0x800757A8, 0x8007AEDC);
    sw(0x800757C0, 0x8007B1FC);
    sw(0x8007567C, 0x8007B68C);
    sw(0x800758C4, 0x8007B4B0);
    break;
  case 60:
    lib_num = LIB_GNASTYS_WORLD;
    sw(0x800785D8, 0x80085CE0);
    sw(0x800758CC, 0x80083568);
    sw(0x80075734, 0x8007D938);
    sw(0x800758E4, 0x80084EA0);
    sw(0x800756BC, 0x80083ED8);
    sw(0x800757A0, 0x8007AE10);
    sw(0x800758A8, 0x8007AF4C);
    sw(0x8007574C, 0x8007AF90);
    sw(0x800758D8, 0x8007CF30);
    break;
  case 61:
    lib_num = LIB_GNORC_COVE;
    sw(0x800785D8, 0x80088668);
    sw(0x800758CC, 0x80085664);
    sw(0x80075734, 0x8007B528);
    sw(0x800758E4, 0x8008747C);
    sw(0x800756BC, 0x80086144);
    break;
  case 62:
    lib_num = LIB_TWILIGHT_HARBOR;
    sw(0x800785D8, 0x80086004);
    sw(0x800758CC, 0x80083108);
    sw(0x80075734, 0x8007AE5C);
    sw(0x800758E4, 0x80084EAC);
    sw(0x800756BC, 0x80083B4C);
    break;
  case 63:
    lib_num = LIB_GNASTY_GNORC;
    sw(0x800785D8, 0x800854B4);
    sw(0x800758CC, 0x80082F24);
    sw(0x80075734, 0x8007AD64);
    sw(0x800758E4, 0x80084634);
    sw(0x800756BC, 0x800836F0);
    break;
  case 64:
    lib_num = LIB_GNASTYS_LOOT;
    sw(0x800785D8, 0x80086264);
    sw(0x800758CC, 0x80083690);
    sw(0x80075734, 0x8007AD4C);
    sw(0x800758E4, 0x80085230);
    sw(0x800756BC, 0x800840FC);
    break;
  case 99:
    lib_num = (uint32_t)-1;BREAKPOINT;
    sw(0x800785D8, 0x8007C654);
    sw(0x800758CC, 0x8007B0C0);
    sw(0x80075734, 0x8007ACDC);
    sw(0x800758E4, 0x8007BC44);
    sw(0x800756BC, 0x8007B194);
    break;
  }
  v0 = lw(0x800785D8);
  sw(0x800785DC, v0);
  return v0;
}

// size: 0x00001288
void function_8005A470(void)
{
  BREAKPOINT;
  v0 = update_level_functions();
}

void function_loaded_80075734(void)
{
  switch (lw(0x80075734))
  {
  case 0x8007D9C8:
    function_8007D9C8();
    break;
  case 0x8007DA78:
    function_8007DA78();
    break;
  case 0x8007AE40:
    function_8007AE40();
    break;
  case 0x8007DA54:
    function_8007DA54();
    break;
  case 0x8007AF94:
    function_8007AF94();
    break;
  case 0x8007CFB4:
    function_8007CFB4();
    break;
  case 0x8007E3A0:
    function_8007E3A0();
    break;
  case 0x8007E240:
    function_8007E240();
    break;
  case 0x8007B4C8:
    function_8007B4C8();
    break;
  case 0x8007AEB8:
    function_8007AEB8();
    break;
  case 0x8007E398:
    function_8007E398();
    break;
  case 0x8007BB00:
    function_8007BB00();
    break;
  case 0x8007B64C:
    function_8007B64C();
    break;
  case 0x8007B7A8:
    function_8007B7A8();
    break;
  case 0x8007AF28:
    function_8007AF28();
    break;
  case 0x8007E18C:
    function_8007E18C();
    break;
  case 0x8007B5DC:
    function_8007B5DC();
    break;
  case 0x8007AFBC:
    function_8007AFBC();
    break;
  case 0x8007B698:
    function_8007B698();
    break;
  case 0x8007B770:
    function_8007B770();
    break;
  case 0x8007E3C0:
    function_8007E3C0();
    break;
  case 0x8007B4F8:
    function_8007B4F8();
    break;
  case 0x8007B4DC:
    function_8007B4DC();
    break;
  case 0x8007B510:
    function_8007B510();
    break;
  case 0x8007AF50:
    function_8007AF50();
    break;
  case 0x8007D938:
    function_8007D938();
    break;
  case 0x8007B528:
    function_8007B528();
    break;
  case 0x8007AE5C:
    function_8007AE5C();
    break;
  case 0x8007AD64:
    function_8007AD64();
    break;
  case 0x8007AD4C:
    function_8007AD4C();
    break;
  default:
    BREAKPOINT;
  }
}

void function_loaded_800758CC(void)
{
  switch (lw(0x800758CC))
  {
  case 0x800857CC:
    function_800857CC();
    break;
  case 0x8008772C:
    function_8008772C();
    break;
  case 0x80082960:
    function_80082960();
    break;
  case 0x800872A4:
    function_800872A4();
    break;
  case 0x80081DA8:
    function_80081DA8();
    break;
  case 0x8008249C:
    function_8008249C();
    break;
  case 0x8008A258:
    function_8008A258();
    break;
  case 0x8008A4D0:
    function_8008A4D0();
    break;
  case 0x80088B88:
    function_80088B88();
    break;
  case 0x80083608:
    function_80083608();
    break;
  case 0x800845F0:
    function_800845F0();
    break;
  case 0x800819BC:
    function_800819BC();
    break;
  case 0x8008B2C0:
    function_8008B2C0();
    break;
  case 0x8008A36C:
    function_8008A36C();
    break;
  case 0x8008883C:
    function_8008883C();
    break;
  case 0x80086DD8:
    function_80086DD8();
    break;
  case 0x80083AB4:
    function_80083AB4();
    break;
  case 0x80081F0C:
    function_80081F0C();
    break;
  case 0x80087EF0:
    function_80087EF0();
    break;
  case 0x8008465C:
    function_8008465C();
    break;
  case 0x80084718:
    function_80084718();
    break;
  case 0x80086B38:
    function_80086B38();
    break;
  case 0x800874FC:
    function_800874FC();
    break;
  case 0x8008223C:
    function_8008223C();
    break;
  case 0x80088E24:
    function_80088E24();
    break;
  case 0x80084B94:
    function_80084B94();
    break;
  case 0x80084620:
    function_80084620();
    break;
  case 0x8008590C:
    function_8008590C();
    break;
  case 0x800836A8:
    function_800836A8();
    break;
  case 0x80082028:
    function_80082028();
    break;
  case 0x80083568:
    function_80083568();
    break;
  case 0x80085664:
    function_80085664();
    break;
  case 0x80083108:
    function_80083108();
    break;
  case 0x80082F24:
    function_80082F24();
    break;
  case 0x80083690:
    function_80083690();
    break;
  default:
    BREAKPOINT;
  }
}

void function_loaded_800756BC(void)
{
  switch (lw(0x800756BC))
  {
  case 0x80086134:
    function_80086134();
    break;
  case 0x80088098:
    function_80088098();
    break;
  case 0x80083274:
    function_80083274();
    break;
  case 0x80087E20:
    function_80087E20();
    break;
  case 0x800826F0:
    function_800826F0();
    break;
  case 0x80083330:
    function_80083330();
    break;
  case 0x8008AE28:
    function_8008AE28();
    break;
  case 0x8008B1C0:
    function_8008B1C0();
    break;
  case 0x80089714:
    function_80089714();
    break;
  case 0x80083F2C:
    function_80083F2C();
    break;
  case 0x80084EF0:
    function_80084EF0();
    break;
  case 0x80082300:
    function_80082300();
    break;
  case 0x8008BE98:
    function_8008BE98();
    break;
  case 0x8008AF54:
    function_8008AF54();
    break;
  case 0x80089454:
    function_80089454();
    break;
  case 0x80087B40:
    function_80087B40();
    break;
  case 0x80084830:
    function_80084830();
    break;
  case 0x80082AA0:
    function_80082AA0();
    break;
  case 0x800888F8:
    function_800888F8();
    break;
  case 0x800853AC:
    function_800853AC();
    break;
  case 0x80085084:
    function_80085084();
    break;
  case 0x80087400:
    function_80087400();
    break;
  case 0x80088178:
    function_80088178();
    break;
  case 0x80082F54:
    function_80082F54();
    break;
  case 0x800897FC:
    function_800897FC();
    break;
  case 0x800857FC:
    function_800857FC();
    break;
  case 0x800850A0:
    function_800850A0();
    break;
  case 0x80086754:
    function_80086754();
    break;
  case 0x80084028:
    function_80084028();
    break;
  case 0x80082D94:
    function_80082D94();
    break;
  case 0x80083ED8:
    function_80083ED8();
    break;
  case 0x80086144:
    function_80086144();
    break;
  case 0x80083B4C:
    function_80083B4C();
    break;
  case 0x800836F0:
    function_800836F0();
    break;
  case 0x800840FC:
    function_800840FC();
    break;
  default:
    BREAKPOINT;
  }
}