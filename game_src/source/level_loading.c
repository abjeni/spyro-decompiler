#include "main.h"
#include "psx_mem.h"
#include "level_loading.h"

uint32_t lib_num = LIB_TITLE_SCREEN;

// size: 0x00001288
void function_8005A470(void)
{
  v1 = lw(0x8007596C);
  if (v1 < 100) {
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
  }
  v0 = lw(0x800785D8);
  sw(0x800785DC, v0);
  return;
}