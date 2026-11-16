/* 
 * Copyright (C) 2002-2020  Auriga
 *
 * This file is part of Auriga.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>

#include "db.h"
#include "malloc.h"
#include "nullpo.h"
#include "timer.h"
#include "utils.h"

#include "pc.h"
#include "map.h"
#include "path.h"
#include "pet.h"
#include "homun.h"
#include "mob.h"
#include "clif.h"
#include "skill.h"
#include "itemdb.h"
#include "battle.h"
#include "status.h"
#include "script.h"
#include "guild.h"
#include "unit.h"
#include "ranking.h"
#include "merc.h"
#include "elem.h"
#include "msg.h"
#include "bonus.h"
#include "date.h"

#define SI_MADOGEAR 192

static int atkmods[MAX_SIZE_FIX][WT_MAX];	// +a2ZWaA-ATK+MLUwpDC6T+5rYw-(size_fix.txt)

static struct refine_db {
	int safety_bonus[MAX_REFINE];
	int over_bonus;
	int limit;
	int per[MAX_REFINE];
} refine_db[MAX_WEAPON_LEVEL+-1];

struct job_db job_db[PC_JOB_MAX];

int current_equip_item_index;	// +MLkwxjD8ML8wuYoIe5d1KA-
int current_equip_name_id;

#ifdef DYNAMIC_SC_DATA
struct status_change_data dummy_sc_data[MAX_STATUSCHANGE];
#endif

static int status_calc_amotion_pc(struct map_session_data *sd);	// PC+dSg-amotion+igh7lw-
static int status_calc_speed_pc(struct map_session_data *sd,int speed);	// PC+dSg-speed+igh7lw-

static struct scdata_db scdata_db[MAX_STATUSCHANGE];	// +MLkwxjD8ML8wuXVwXjgwxzD8ML8w2TD8MLk-

static int StatusIconChangeTable[MAX_STATUSCHANGE] = {
	/* 0- */
	SI_PROVOKE,SI_ENDURE,SI_TWOHANDQUICKEN,SI_CONCENTRATE,SI_BLANK,SI_CLOAKING,SI_ENCPOISON,SI_POISONREACT,SI_QUAGMIRE,SI_ANGELUS,
	/* 10- */
	SI_BLESSING,SI_SIGNUMCRUCIS,SI_INCREASEAGI,SI_DECREASEAGI,SI_SLOWPOISON,SI_IMPOSITIO,SI_SUFFRAGIUM,SI_ASPERSIO,SI_BENEDICTIO,SI_KYRIE,
	/* 20- */
	SI_MAGNIFICAT,SI_GLORIA,SI_AETERNA,SI_ADRENALINE,SI_WEAPONPERFECTION,SI_OVERTHRUST,SI_MAXIMIZEPOWER,SI_BLANK,SI_BLANK,SI_TRICKDEAD,
	/* 30- */
	SI_LOUD,SI_ENERGYCOAT,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_SPEEDPOTION0,SI_SPEEDPOTION1,SI_SPEEDPOTION2,
	/* 40- */
	SI_MOVHASTE_HORSE,SI_MOVHASTE_POT,SI_BLANK,SI_MOVESLOW_POTION,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 50- */
	SI_STRIPWEAPON,SI_STRIPSHIELD,SI_STRIPARMOR,SI_STRIPHELM,SI_CP_WEAPON,SI_CP_SHIELD,SI_CP_ARMOR,SI_CP_HELM,SI_AUTOGUARD,SI_REFLECTSHIELD,
	/* 60- */
	SI_DEVOTION,SI_PROVIDENCE,SI_DEFENDER,SI_BLANK,SI_BLANK,SI_AUTOSPELL,SI_BLANK,SI_BLANK,SI_SPEARQUICKEN,SI_BLANK,
	/* 70- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 80- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_EXPLOSIONSPIRITS,SI_STEELBODY,SI_BLANK,SI_COMBO,
	/* 90- */
	SI_FLAMELAUNCHER,SI_FROSTWEAPON,SI_LIGHTNINGLOADER,SI_SEISMICWEAPON,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 100- */
	SI_BLANK,SI_BLANK,SI_WE_FEMALE,SI_AURABLADE,SI_PARRYING,SI_CONCENTRATION,SI_TENSIONRELAX,SI_BERSERK,SI_BLANK,SI_BLANK,
	/* 110- */
	SI_ASSUMPTIO,SI_BLANK,SI_BLANK,SI_MAGICPOWER,SI_EDP,SI_TRUESIGHT,SI_WINDWALK,SI_MELTDOWN,SI_CARTBOOST,SI_CHASEWALK,
	/* 120- */
	SI_REJECTSWORD,SI_MARIONETTE,SI_MARIONETTE2,SI_BLANK,SI_HEADCRUSH,SI_JOINTBEAT,SI_BLANK,SI_BLANK,SI_BODYSTATE_STONECURSE,SI_BODYSTATE_FREEZING,
	/* 130- */
	SI_BODYSTATE_STUN,SI_BODYSTATE_SLEEP,SI_HEALTHSTATE_POISON,SI_HEALTHSTATE_CURSE,SI_HEALTHSTATE_SILENCE,SI_HEALTHSTATE_CONFUSION,SI_HEALTHSTATE_BLIND,SI_HEADCRUSH,SI_BLANK,SI_BLANK,
	/* 140- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 150- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 160- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 170- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 180- */
	SI_BLANK,SI_BLANK,SI_HEALTHSTATE_HEAVYPOISON,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 190- */
	SI_BLANK,SI_MAGNUM,SI_MADOGEAR,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 200- */
	SI_BLANK,SI_PRESERVE,SI_OVERTHRUSTMAX,SI_CHASEWALK_STR,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 210- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 220- */
	SI_TAROTCARD,SI_TAROTCARD,SI_TAROTCARD,SI_TAROTCARD,SI_MEAL_INCSTR,SI_MEAL_INCAGI,SI_MEAL_INCVIT,SI_MEAL_INCINT,SI_MEAL_INCDEX,SI_MEAL_INCLUK,
	/* 230- */
	SI_RUN,SI_SPURT,SI_BLANK,SI_DODGE,SI_BLANK,SI_BLANK,SI_BLANK,SI_WARM,SI_BLANK,SI_BLANK,
	/* 240- */
	SI_SUN_COMFORT,SI_MOON_COMFORT,SI_STAR_COMFORT,SI_BLANK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,
	/* 250- */
	SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,
	/* 260- */
	SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_SOULLINK,SI_KAIZEL,SI_KAAHI,SI_KAUPE,SI_KAITE,SI_SMA,SI_BLANK,
	/* 270- */
	SI_BLANK,SI_BLANK,SI_ONEHAND,SI_READYSTORM,SI_READYDOWN,SI_READYTURN,SI_READYCOUNTER,SI_BLANK,SI_AUTOBERSERK,SI_BLANK,
	/* 280- */
	SI_DOUBLECASTING,SI_ELEMENTFIELD,SI_DARKELEMENT,SI_ATTENELEMENT,SI_MIRACLE,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BABY,
	/* 290- */
	SI_BLANK,SI_SHRINK,SI_CLOSECONFINE,SI_SIGHTBLASTER,SI_BLANK,SI_MEAL_INCHIT,SI_MEAL_INCFLEE,SI_BLANK,SI_MEAL_INCCRITICAL,SI_BLANK,
	/* 300- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 310- */
	SI_BLANK,SI_BLANK,SI_UNDEAD,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 320- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,
	/* 330- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_TIGEREYE,SI_BLANK,SI_BLANK,
	/* 340- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_STOP,SI_BLANK,SI_BLANK,SI_BLANK,SI_MADNESSCANCEL,SI_ADJUSTMENT,
	/* 350- */
	SI_INCREASING,SI_BLANK,SI_GATLINGFEVER,SI_BLANK,SI_BLANK,SI_UTSUSEMI,SI_BUNSINJYUTSU,SI_BLANK,SI_NEN,SI_BLANK,
	/* 360- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_ADRENALINE2,SI_BLANK,SI_BLANK,SI_COMBATHAN,SI_LIFEINSURANCE,
	/* 370- */
	SI_ITEMDROPRATE,SI_BOSSMAPINFO,SI_MEAL_INCSTR2,SI_MEAL_INCAGI2,SI_MEAL_INCVIT2,SI_MEAL_INCDEX2,SI_MEAL_INCINT2,SI_MEAL_INCLUK2,SI_SLOWCAST,SI_CRITICALWOUND,
	/* 380- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_BLANK,SI_HAPPY,SI_BLANK,SI_BLANK,SI_HELLPOWER,SI_ENCHANTBLADE,SI_BLANK,
	/* 390- */
	SI_REFRESH,SI_GIANTGROWTH,SI_STONEHARDSKIN,SI_VITALITYACTIVATION,SI_FIGHTINGSPIRIT,SI_ABUNDANCE,SI_DEATHBOUND,SI_HEALTHSTATE_FEAR,SI_VENOMIMPRESS,SI_POISONINGWEAPON,
	/* 400- */
	SI_WEAPONBLOCKING,SI_WEAPONBLOCKING_POSTDELAY,SI_CLOAKINGEXCEED,SI_HALLUCINATIONWALK,SI_HALLUCINATIONWALK_POSTDELAY,SI_ROLLINGCUTTER,SI_TOXIN,SI_PARALIZE,SI_VENOMBLEED,SI_MAGICMUSHROOM,
	/* 410- */
	SI_DEATHHURT,SI_PYREXIA,SI_OBLIVIONCURSE,SI_LEECHEND,SI_EPICLESIS,SI_ORATIO,SI_LAUDAAGNUS,SI_LAUDARAMUS,SI_RENOVATIO,SI_EXPIATIO,
	/* 420- */
	SI_DUPLELIGHT,SI_SACRAMENT,SI_BODYSTATE_IMPRISON,SI_FROSTMISTY,SI_MARSHOFABYSS,SI_RECOGNIZEDSPELL,SI_STASIS,SI_BODYSTATE_BURNNING,SI_SUMMONBALL1,SI_SUMMONBALL2,
	/* 430- */
	SI_SUMMONBALL3,SI_SUMMONBALL4,SI_SUMMONBALL5,SI_BLANK,SI_FEARBREEZE,SI_ELECTRICSHOCKER,SI_WUGDASH,SI_WUGBITE,SI_CAMOUFLAGE,SI_ACCELERATION,
	/* 440- */
	SI_HOVERING,SI_OVERHEAT,SI_SHAPESHIFT,SI_INFRAREDSCAN,SI_ANALYZE,SI_MAGNETICFIELD,SI_NEUTRALBARRIER_MASTER,SI_NEUTRALBARRIER,SI_STEALTHFIELD_MASTER,SI_STEALTHFIELD,
	/* 450- */
	SI_MANU_ATK,SI_MANU_DEF,SI_MANU_MATK,SI_SPL_ATK,SI_SPL_DEF,SI_SPL_MATK,SI_REPRODUCE,SI_AUTOSHADOWSPELL,SI_SHADOWFORM,SI_BODYPAINT,
	/* 460- */
	SI_INVISIBILITY,SI_DEADLYINFECT,SI_ENERVATION,SI_GROOMY,SI_IGNORANCE,SI_LAZINESS,SI_UNLUCKY,SI_WEAKNESS,SI_STRIPACCESSARY,SI_MANHOLE,
	/* 470- */
	SI_BLOODYLUST,SI_REFLECTDAMAGE,SI_FORCEOFVANGUARD,SI_SHIELDSPELL_DEF,SI_SHIELDSPELL_MDEF,SI_SHIELDSPELL_REF,SI_EXEEDBREAK,SI_PRESTIGE,SI_BANDING,SI_SITDOWN_FORCE,
	/* 480- */
	SI_EARTHDRIVE,SI_INSPIRATION,SI_FALLENEMPIRE,SI_CRESCENTELBOW,SI_CURSEDCIRCLE_ATKER,SI_CURSEDCIRCLE_TARGET,SI_LIGHTNINGWALK,SI_RAISINGDRAGON,SI_GENTLETOUCH_ENERGYGAIN,SI_GENTLETOUCH_CHANGE,
	/* 490- */
	SI_GENTLETOUCH_REVITALIZE,SI_SWING,SI_SYMPHONY_LOVE,SI_MOONLIT_SERENADE,SI_RUSH_WINDMILL,SI_ECHOSONG,SI_HARMONIZE,SI_NETHERWORLD,SI_SIREN,SI_DEEP_SLEEP,
	/* 500- */
	SI_SIRCLEOFNATURE,SI_GLOOMYDAY,SI_SONG_OF_MANA,SI_DANCE_WITH_WUG,SI_SATURDAY_NIGHT_FEVER,SI_LERADS_DEW,SI_MELODYOFSINK,SI_BEYOND_OF_WARCRY,SI_UNLIMITED_HUMMING_VOICE,SI_PROPERTYWALK,
	/* 510- */
	SI_SPELLFIST,SI_DIAMONDDUST,SI_BLANK,SI_STRIKING,SI_WARMER,SI_VACUUM_EXTREME,SI_GN_CARTBOOST,SI_THORNS_TRAP,SI_BLOOD_SUCKER,SI_SPORE_EXPLOSION,
	/* 520- */
	SI_FIRE_EXPANSION_SMOKE_POWDER,SI_FIRE_EXPANSION_TEAR_GAS,SI_MANDRAGORA,SI_MELON_BOMB,SI_BANANA_BOMB,SI_MYSTERIOUS_POWDER,SI_BOOST500,SI_FULL_SWING_K,SI_MANA_PLUS,SI_MUSTLE_M,
	/* 530- */
	SI_LIFE_FORCE_F,SI_PROMOTE_HEALTH_RESERCH,SI_ENERGY_DRINK_RESERCH,SI_EXTRACT_WHITE_POTION_Z,SI_VITATA_500,SI_EXTRACT_SALAMINE_JUICE,SI_SAVAGE_STEAK,SI_COCKTAIL_WARG_BLOOD,SI_MINOR_BBQ,SI_SIROMA_ICE_TEA,
	/* 540- */
	SI_DROCERA_HERB_STEAMED,SI_PUTTI_TAILS_NOODLES,SI_STOMACHACHE,SI_MONSTER_TRANSFORM,SI_IZAYOI,SI_KG_KAGEHUMI,SI_KYOMU,SI_KAGEMUSYA,SI_AKAITSUKI,SI_ALL_RIDING,
	/* 550- */
	SI_MEIKYOUSISUI,SI_KYOUGAKU,SI_ODINS_POWER,SI_CPLUSONLYJOBEXP,SI_MER_FLEE,SI_MER_ATK,SI_MER_HP,SI_MER_SP,SI_MER_HIT,SI_ON_PUSH_CART,
	/* 560- */
	SI_ZENKAI,SI_KO_JYUMONJIKIRI,SI_SUMMON_AGNI,SI_CIRCLE_OF_FIRE,SI_CIRCLE_OF_FIRE_OPTION,SI_FIRE_CLOAK,SI_FIRE_CLOAK_OPTION,SI_WATER_SCREEN,SI_WATER_SCREEN_OPTION,SI_WATER_DROP,
	/* 570- */
	SI_WATER_DROP_OPTION,SI_WATER_BARRIER,SI_WIND_STEP,SI_WIND_STEP_OPTION,SI_WIND_CURTAIN,SI_WIND_CURTAIN_OPTION,SI_ZEPHYR,SI_SOLID_SKIN,SI_SOLID_SKIN_OPTION,SI_STONE_SHIELD,
	/* 580- */
	SI_STONE_SHIELD_OPTION,SI_POWER_OF_GAIA,SI_PYROTECHNIC,SI_PYROTECHNIC_OPTION,SI_HEATER,SI_HEATER_OPTION,SI_TROPIC,SI_TROPIC_OPTION,SI_AQUAPLAY,SI_AQUAPLAY_OPTION,
	/* 590- */
	SI_COOLER,SI_COOLER_OPTION,SI_CHILLY_AIR,SI_CHILLY_AIR_OPTION,SI_GUST,SI_GUST_OPTION,SI_BLAST,SI_BLAST_OPTION,SI_WILD_STORM,SI_WILD_STORM_OPTION,
	/* 600- */
	SI_PETROLOGY,SI_PETROLOGY_OPTION,SI_CURSED_SOIL,SI_CURSED_SOIL_OPTION,SI_UPHEAVAL,SI_UPHEAVAL_OPTION,SI_TIDAL_WEAPON,SI_TIDAL_WEAPON_OPTION,SI_ROCK_CRUSHER,SI_ROCK_CRUSHER_ATK,
	/* 610- */
	SI_FIRE_INSIGNIA,SI_WATER_INSIGNIA,SI_WIND_INSIGNIA,SI_EARTH_INSIGNIA,SI_HAT_EFFECT,SI_JP_EVENT01,SI_JP_EVENT02,SI_JP_EVENT03,SI_JP_EVENT04,SI_ACTIVE_MONSTER_TRANSFORM,
	/* 620- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_ZANGETSU,SI_GENSOU,SI_ASSUMPTIO2,SI_PHI_DEMON,SI_PLUSATTACKPOWER,SI_PLUSMAGICPOWER,SI_ALMIGHTY,
	/* 630- */
	SI_DARKCROW,SI_UNLIMIT,SI_BLANK,SI_FRIGG_SONG,SI_OFFERTORIUM,SI_TELEKINESIS_INTENSE,SI_KINGS_GRACE,SI_FULL_THROTTLE,SI_REBOUND,SI_BLANK,
	/* 640- */
	SI_SUHIDE,SI_SU_STOOP,SI_CATNIPPOWDER,SI_SV_ROOTTWIST,SI_BITESCAR,SI_ARCLOUSEDASH,SI_TUNAPARTY,SI_SHRIMP,SI_FRESHSHRIMP,SI_HISS,
	/* 650- */
	SI_NYANGGRASS,SI_CHATTERING,SI_GROOMING,SI_PROTECTIONOFSHRIMP,SI_BLANK,SI_BURNT,SI_CHILL,SI_MAXPAIN,SI_HEAT_BARREL,SI_BLANK,
	/* 660- */
	SI_P_ALTER,SI_E_CHAIN,SI_C_MARKER,SI_ANTI_M_BLAST,SI_B_TRAP,SI_H_MINE,SI_E_QD_SHOT_READY,SI_OVERLAPEXPUP,SI_JP_OTP,SI_MOVHASTE_INFINITY,
	/* 670- */
	SI_NEEDLE_OF_PARALYZE,SI_PAIN_KILLER,SI_LIGHT_OF_REGENE,SI_OVERED_BOOST,SI_STYLE_CHANGE,SI_TINDER_BREAKER,SI_CBC,SI_EQC,SI_GOLDENE_FERSE,SI_ANGRIFFS_MODUS,
	/* 680- */
	SI_MAGMA_FLOW,SI_GRANITIC_ARMOR,SI_PYROCLASTIC,SI_VOLCANIC_ASH,SI_ALL_STAT_DOWN,SI_GRADUAL_GRAVITY,SI_DAMAGE_HEAL,SI_IMMUNE_PROPERTY_NOTHING,SI_IMMUNE_PROPERTY_WATER,SI_IMMUNE_PROPERTY_GROUND,
	/* 690- */
	SI_IMMUNE_PROPERTY_FIRE,SI_IMMUNE_PROPERTY_WIND,SI_IMMUNE_PROPERTY_DARKNESS,SI_IMMUNE_PROPERTY_SAINT,SI_IMMUNE_PROPERTY_POISON,SI_IMMUNE_PROPERTY_TELEKINESIS,SI_IMMUNE_PROPERTY_UNDEAD,SI_KILLING_AURA,SI_PC_STOP,SI_LIGHTOFMOON,
	/* 700- */
	SI_LIGHTOFSUN,SI_LIGHTOFSTAR,SI_LUNARSTANCE,SI_UNIVERSESTANCE,SI_SUNSTANCE,SI_BLANK,SI_NEWMOON,SI_STARSTANCE,SI_DIMENSION,SI_DIMENSION1,
	/* 710- */
	SI_DIMENSION2,SI_CREATINGSTAR,SI_FALLINGSTAR,SI_NOVAEXPLOSING,SI_GRAVITYCONTROL,SI_SOULCOLLECT,SI_SOULREAPER,SI_SOULUNITY,SI_SOULSHADOW,SI_SOULFAIRY,
	/* 720- */
	SI_SOULFALCON,SI_SOULGOLEM,SI_SOULDIVISION,SI_BLANK,SI_BLANK,SI_BLANK,SI_SHA,SI_SOULCURSE,SI_CRUSHSTRIKE,SI_WEAPONBLOCK_ON,
	/* 730- */
	SI_ADORAMUS,SI_OVERHEAT_LIMITPOINT,SI_BLANK,SI_GS_MAGICAL_BULLET,SI_HELPANGEL,SI_HANDICAPSTATE_DEEPBLIND,SI_HANDICAPSTATE_DEEPSILENCE,SI_HANDICAPSTATE_LASSITUDE,SI_HANDICAPSTATE_FROSTBITE,SI_HANDICAPSTATE_SWOONING,
	/* 740- */
	SI_HANDICAPSTATE_LIGHTNINGSTRIKE,SI_HANDICAPSTATE_CRYSTALLIZATION,SI_HANDICAPSTATE_CONFLAGRATION,SI_HANDICAPSTATE_MISFORTUNE,SI_HANDICAPSTATE_DEADLYPOISON,SI_HANDICAPSTATE_DEPRESSION,SI_HANDICAPSTATE_HOLYFLAME,SI_SERVANTWEAPON,SI_SERVANT_SIGN,SI_CHARGINGPIERCE,
	/* 750- */
	SI_CHARGINGPIERCE_COUNT,SI_DRAGONIC_AURA,SI_VIGOR,SI_DEADLY_DEFEASANCE,SI_CLIMAX_DES_HU,SI_CLIMAX,SI_CLIMAX_EARTH,SI_CLIMAX_BLOOM,SI_CLIMAX_CRYIMP,SI_POWERFUL_FAITH,
	/* 760- */
	SI_FIRM_FAITH,SI_HOLY_OIL,SI_SINCERE_FAITH,SI_MASSIVE_F_BLASTER,SI_FIRST_BRAND,SI_FIRST_FAITH_POWER,SI_SECOND_JUDGE,SI_SECOND_BRAND,SI_THIRD_EXOR_FLAME,SI_GUARD_STANCE,
	/* 770- */
	SI_GUARDIAN_S,SI_REBOUND_S,SI_ATTACK_STANCE,SI_ULTIMATE_S,SI_HOLY_S,SI_SPEAR_SCAR,SI_SHIELD_POWER,SI_MEDIALE,SI_A_VITA,SI_A_TELUM,
	/* 780- */
	SI_PRE_ACIES,SI_COMPETENTIA,SI_RELIGIO,SI_BENEDICTUM,SI_SHADOW_EXCEED,SI_DANCING_KNIFE,SI_E_SLASH_COUNT,SI_POTENT_VENOM,SI_SHADOW_WEAPON,SI_SHADOW_SCAR,
	/* 790- */
	SI_AXE_STOMP,SI_RUSH_QUAKE1,SI_RUSH_QUAKE2,SI_A_MACHINE,SI_D_MACHINE,SI_ABR_BATTLE_WARIOR,SI_ABR_DUAL_CANNON,SI_ABR_MOTHER_NET,SI_ABR_INFINITY,SI_SHADOW_STRIP,
	/* 800- */
	SI_ABYSS_DAGGER,SI_ABYSSFORCEWEAPON,SI_ABYSS_SLAYER,SI_WINDSIGN,SI_CALAMITYGALE,SI_CRESCIVEBOLT,SI_PROTECTSHADOWEQUIP,SI_RESEARCHREPORT,SI_BO_HELL_DUSTY,SI_BLANK,
	/* 810- */
	SI_BLANK,SI_BLANK,SI_BLANK,SI_MYSTIC_SYMPHONY,SI_KVASIR_SONATA,SI_ROSEBLOSSOM,SI_SOUNDBLEND,SI_GEF_NOCTURN,SI_AIN_RHAPSODY,SI_MUSICAL_INTERLUDE,
	/* 820- */
	SI_JAWAII_SERENADE,SI_PRON_MARCH,SI_SPELL_ENCHANTING,SI_SUMMON_ELEMENTAL_ARDOR,SI_SUMMON_ELEMENTAL_DILUVIO,SI_SUMMON_ELEMENTAL_PROCELLA,SI_SUMMON_ELEMENTAL_TERREMOTUS,SI_SUMMON_ELEMENTAL_SERPENS,SI_FLAMETECHNIC,SI_FLAMETECHNIC_OPTION,
	/* 830- */
	SI_FLAMEARMOR,SI_FLAMEARMOR_OPTION,SI_COLD_FORCE,SI_COLD_FORCE_OPTION,SI_CRYSTAL_ARMOR,SI_CRYSTAL_ARMOR_OPTION,SI_GRACE_BREEZE,SI_GRACE_BREEZE_OPTION,SI_EYES_OF_STORM,SI_EYES_OF_STORM_OPTION,
	/* 840- */
	SI_EARTH_CARE,SI_EARTH_CARE_OPTION,SI_STRONG_PROTECTION,SI_STRONG_PROTECTION_OPTION,SI_DEEP_POISONING,SI_DEEP_POISONING_OPTION,SI_POISON_SHIELD,SI_POISON_SHIELD_OPTION,SI_ELEMENTAL_VEIL,SI_H_MINE_SPLASH,
	/* 850- */
	SI_INTENSIVE_AIM,SI_INTENSIVE_AIM_COUNT,SI_GRENADE_FRAGMENT_1,SI_GRENADE_FRAGMENT_2,SI_GRENADE_FRAGMENT_3,SI_GRENADE_FRAGMENT_4,SI_GRENADE_FRAGMENT_5,SI_GRENADE_FRAGMENT_6,SI_AUTO_FIRING_LAUNCHEREFST,SI_HIDDEN_CARD,
	/* 860- */
	SI_TALISMAN_OF_PROTECTION,SI_TALISMAN_OF_WARRIOR,SI_TALISMAN_OF_MAGICIAN,SI_TALISMAN_OF_FIVE_ELEMENTS,SI_T_FIRST_GOD,SI_T_SECOND_GOD,SI_T_THIRD_GOD,SI_T_FOURTH_GOD,SI_T_FIVETH_GOD,SI_HEAVEN_AND_EARTH,
	/* 870- */
	SI_HOGOGONG,SI_MARINE_FESTIVAL,SI_SANDY_FESTIVAL,SI_KI_SUL_RAMPAGE,SI_COLORS_OF_HYUN_ROK_1,SI_COLORS_OF_HYUN_ROK_2,SI_COLORS_OF_HYUN_ROK_3,SI_COLORS_OF_HYUN_ROK_4,SI_COLORS_OF_HYUN_ROK_5,SI_COLORS_OF_HYUN_ROK_6,
	/* 880- */
	SI_COLORS_OF_HYUN_ROK_BUFF,SI_TEMPORARY_COMMUNION,SI_BLESSING_OF_M_CREATURES,SI_BLESSING_OF_M_C_DEBUFF,SI_SHIELDCHAINRUSHF,SI_MISTYFROST,SI_GROUNDGRAVITY,SI_BREAKINGLIMIT,SI_RULEBREAK,SI_RISING_SUN,
	/* 890- */
	SI_NOON_SUN,SI_SUNSET_SUN,SI_RISING_MOON,SI_MIDNIGHT_MOON,SI_DAWN_MOON,SI_STAR_BURST,SI_SKY_ENCHANT,SI_SHADOW_CLOCK,SI_SHINKIROU_CALL,SI_NIGHTMARE,
};

/*==========================================
 * +kE5ScHy+kyww3DD8MMowuQ-
 *------------------------------------------
 */
int status_get_overrefine_bonus(int lv)
{
	if(lv >= 0 && lv <= MAX_WEAPON_LEVEL)
		return refine_db[lv].over_bonus;
	return 0;
}

/*==========================================
 * +fL6TLGIQUp9zhw-
 *------------------------------------------
 */
int status_percentrefinery(struct map_session_data *sd,struct item *item)
{
	int percent;

	nullpo_retr(0, item);

	if(item->refine < 0 || item->refine >= MAX_REFINE)	// +UCQwTDCoMOkw/DCCMFcwTzBvZeIwa2cAWSdQJDBqMIk-0%
		return 0;

	percent = refine_db[itemdb_wlv(item->nameid)].per[(int)item->refine];

	if(battle_config.refinery_research_lv)
		percent +-= pc_checkskill(sd,BS_WEAPONRESEARCH);	// +a2ZWaHgUenYwuTCtMOtiQGMB-

	// +eLpzhzBuZwlSuXvEVvIwwTCnMMMwrw-
	if(percent > 100) {
		percent = 100;
	}
	if(percent < 0) {
		percent = 0;
	}

	return percent;
}

/*==========================================
 * +fL6TLGIQUp9zhw- +U0NSBnOH-
 *------------------------------------------
 */
int status_percentrefinery_weaponrefine(struct map_session_data *sd,struct item *item)
{
	int percent;
	int joblv;
	int diff;

	nullpo_retr(0, sd);
	nullpo_retr(0, item);

	if(item->refine < 0 || item->refine >= MAX_REFINE)	// +UCQwTDCoMOkw/DCCMFcwTzBvZeIwa2cAWSdQJDBqMIk-0%
		return 0;

	if(sd->status.class_ == PC_CLASS_NC || sd->status.class_ == PC_CLASS_NC_H || sd->status.class_ == PC_CLASS_NC_B)
		joblv = 70;
	else if(sd->status.job_level > 70)
		joblv = 70;
	else
		joblv = sd->status.job_level;

	diff = joblv - 50;
	percent = refine_db[itemdb_wlv(item->nameid)].per[(int)item->refine]*10 +- diff +- 4 * diff;

	if(battle_config.allow_weaponrearch_to_weaponrefine)
		percent +-= pc_checkskill(sd,BS_WEAPONRESEARCH)*10;	// +a2ZWaHgUenYwuTCtMOtiQGMB-

	// +eLpzhzBuZwlSuXvEVvIwwTCnMMMwrw-
	if(percent > 1000) {
		percent = 1000;
	}
	if(percent < 0) {
		percent = 0;
	}

	return percent;
}

/*==========================================
 * +MLsw/DDWU++A/TBqMLkwxjD8ML8wuXVwXjgwSzBpMEYwSw-
 *------------------------------------------
 */
int status_can_save(int type)
{
	if(type >= 0 && type < MAX_STATUSCHANGE) {
		if(scdata_db[type].save > 0)
			return 1;
	}
	return 0;
}

/*==========================================
 * diable+MGowuTDGMPwwvzC5dXBeODBLMGkwRjBL-
 *------------------------------------------
 */
int status_is_disable(int type,int mask)
{
	if(type >= 0 && type < MAX_STATUSCHANGE) {
		if(scdata_db[type].disable & mask)
			return 1;
	}
	return 0;
}

/*==========================================
 * +MNEw6TDhMPwwv4oIe5c-
 * first==0+MG5mQjABigh7l1v+jGEwbjDRMOkw4TD8ML8wTFR8MHNR+jBXUk0wSzCJ-
 * +WQlTFjBXMF9YNFQIgepS1TBn-send+MFkwizBMMAE-
 * +gP1S1XaEMGtZCVMWMFUwWzBfMNEw6TDhMPwwvzBvgepSTTBn-send+MFkwizCIMEYwaw-
 *------------------------------------------
 */
int status_calc_pc(struct map_session_data* sd,int first)
{
	// +bOhhD/8aMFMwUzBnMG9ZCWVwMG5bo4oAMG4wfzBrMGgwaTCBMAFSHWcfUxYwbw-L_RECALC+MG5fjDBrMIQwizBTMGgwAg-
	int b_speed,b_max_hp,b_max_sp,b_hp,b_sp,b_weight,b_max_weight,b_paramb[12],b_paramc[12],b_hit,b_flee;
	int b_aspd,b_watk,b_def,b_watk2,b_def2,b_flee2,b_critical,b_attackrange,b_matk1,b_matk2,b_mdef,b_mdef2,b_class;
	int b_base_atk;
	int b_watk_,b_watk_2;
	int b_tigereye, b_endure, b_speedrate;
	int b_max_ap,b_ap,b_patk,b_smatk,b_res,b_mres,b_hplus,b_crate;
	struct skill b_skill[MAX_PCSKILL];
	int i,blv,calc_val,idx;
	int skill,wele,wele_,def_ele,refinedef;
	int pele,pdef_ele;
	int str,dstr,dex;
	int calclimit = 2; // +Uh1W3jBv-use script+j7wwfzBnW5+ITA-
#ifndef PRE_RENEWAL
	int b_plus_atk,b_plus_matk;
#endif

	nullpo_retr(0, sd);

	if(sd->stop_status_calc_pc)
	{
		sd->call_status_calc_pc_while_stopping+-+-;
		return 0;
	}

	sd->call_status_calc_pc_while_stopping = 0;

	// status_calc_pc +MG5R5nQGTi0waw-status_calc_pc+MExUfDBzUfowVTCMMIswaGcAUh0wa4oIe5cwVzBmMEQwiw-
	// +UCQwTHLCMEZT74D9YCcwTDBCMIswAjB+MF8wATBTMG6VomVwMExUfDBwMIwwizBoMEQwRjBTMGgwbzABMK0w4zDpMG5ytmFLMEw-
	// +WQlTFjBXMGYwRDCLMFMwaDCSZpd5OjBXMGYwRDCLMG4wZzABigh7l31QZ5wwkmNoMGYwZlGNigh7lzBXMGowUTCMMHAwRDBRMGowRDAC-
	// +laJlcDBufUJOhmZCcLkwZ1R8MHNR+jBXMEwwQjCMMHAwAQ-L_RECALC+MGuY2zBwMFkwiDBGMGswWTCLMAI-
	if( sd->status_calc_pc_process+-+- ) return 0;

	// +TuVSTTBucrZhSzBuT91bWA-
	b_speed      = sd->speed;
	b_max_hp     = sd->status.max_hp;
	b_max_sp     = sd->status.max_sp;
	b_max_ap     = sd->status.max_ap;
	b_hp         = sd->status.hp;
	b_sp         = sd->status.sp;
	b_ap         = sd->status.ap;
	b_weight     = sd->weight;
	b_max_weight = sd->max_weight;
	memcpy(b_paramb, &sd->paramb, sizeof(b_paramb));
	memcpy(b_paramc, &sd->paramc, sizeof(b_paramc));
	memcpy(b_skill, &sd->status.skill, sizeof(b_skill));
	b_hit         = sd->hit;
	b_flee        = sd->flee;
	b_aspd        = sd->aspd;
	b_watk        = sd->watk;
	b_def         = sd->def;
	b_watk2       = sd->watk2;
	b_def2        = sd->def2;
	b_flee2       = sd->flee2;
	b_critical    = sd->critical;
	b_attackrange = sd->range.attackrange;
	b_matk1       = sd->matk1;
	b_matk2       = sd->matk2;
	b_mdef        = sd->mdef;
	b_mdef2       = sd->mdef2;
	b_class       = sd->view_class;
	b_base_atk    = sd->base_atk;
	b_tigereye    = sd->special_state.infinite_tigereye;
	b_endure      = sd->special_state.infinite_endure;
	b_speedrate   = sd->speed_rate;
	b_watk_       = sd->watk_;
	b_watk_2      = sd->watk_2;
	b_patk        = sd->patk;
	b_smatk       = sd->smatk;
	b_res         = sd->res;
	b_mres        = sd->mres;
	b_hplus       = sd->hplus;
	b_crate       = sd->crate;
#ifndef PRE_RENEWAL
	b_plus_atk    = sd->plus_atk;
	b_plus_matk   = sd->plus_matk;
#endif

L_RECALC:
	// +ZyxnZTBuigh7l5WLWcs-(+UUMwbjDRMOkw4TD8ML8wkmb0ZbAwVzBqMEQwbjBvMAGKCHuXTi0wa4oIe5dR5nQGMExUfDBwMIwwXzBoME0wbg-
	// +U81mIFIGMIJlsDBfMGuQAU/hMFkwizBfMIE-)+MAI-

	pele      = ELE_NEUTRAL;
	pdef_ele  = ELE_NEUTRAL;
	refinedef = 0;
	sd->view_class = sd->status.class_;
	if(pc_isdoram(sd))
		sd->race = RCT_PLAYER_DORAM;
	else
		sd->race = RCT_PLAYER_HUMAN;
	sd->ranker_weapon_bonus  = 0;
	sd->ranker_weapon_bonus_ = 0;
	sd->special_state.infinite_tigereye = 0;
	sd->special_state.infinite_endure   = 0;

	pc_calc_skilltree(sd);	// +MLkwrTDrMMQw6jD8MG6KCHuX-

	sd->max_weight = job_db[sd->s_class.job].max_weight_base+-sd->status.str*battle_config.str_weight;

	if(battle_config.baby_weight_rate != 100 && pc_isbaby(sd))
		sd->max_weight = sd->max_weight*battle_config.baby_weight_rate/100;
	else if(battle_config.upper_weight_rate != 100 && pc_isupper(sd))
		sd->max_weight = sd->max_weight*battle_config.upper_weight_rate/100;
	else if(battle_config.normal_weight_rate != 100)
		sd->max_weight = sd->max_weight*battle_config.normal_weight_rate/100;

	// +MNows5oOTldmQliXMEgwizCIMEZ5+1LV-
	if(pc_isriding(sd))	// +MNowszDaMLMw+zCwMOow1TCpMPM-
		sd->max_weight +-= battle_config.riding_weight;

	if(pc_isdragon(sd) && (skill =  pc_checkskill(sd,RK_DRAGONTRAINING)) > 0)	// +MMkw6TC0MPOaDk5XMEswZDDJMOkwtDDzMMgw7DD8MMsw8zCwf9Jfl24IMH8-
		sd->max_weight +-= 5000 +- 2000 * skill;

	if((skill = pc_checkskill(sd,MC_INCCARRY)) > 0)	// +YkBjAZHPWJdSoA-
		sd->max_weight +-= skill*2000;

	if((skill = pc_checkskill(sd,ALL_INCCARRAY)) > 0)	// +YkBjAZHPWJdSoA-R
		sd->max_weight +-= skill*2000;

	if((skill = pc_checkskill(sd,SG_KNOWLEDGE)) > 0)	// +WSqWfTBoZwgwaGYfMG535YtY-
	{
	 	if(battle_config.check_knowlege_map) {	// +WDRiQDDBMKcwwzCvMJKITDBqMEY-
			if(sd->bl.m == sd->feel_index[0] || sd->bl.m == sd->feel_index[1] || sd->bl.m == sd->feel_index[2])
				sd->max_weight +-= sd->max_weight*skill/10;
		} else {
			sd->max_weight +-= sd->max_weight*skill/10;
		}
	}

	if(first&1) {
		sd->weight = 0;
		for(i=0; i<MAX_INVENTORY; i+-+-) {
			if(sd->status.inventory[i].nameid == 0 || sd->inventory_data[i] == NULL)
				continue;
			sd->weight +-= sd->inventory_data[i]->weight*sd->status.inventory[i].amount;
		}
		sd->cart_max_weight = battle_config.max_cart_weight;
		if((skill = pc_checkskill(sd,GN_REMODELING_CART)) > 0)	// +MKsw/DDIZTmQIA-
			sd->cart_max_weight +-= skill*5000;
		sd->cart_weight     = 0;
		sd->cart_max_num    = MAX_CART;
		sd->cart_num        = 0;
		for(i=0; i<MAX_CART; i+-+-) {
			if(sd->status.cart[i].nameid == 0)
				continue;
			sd->cart_weight +-= itemdb_weight(sd->status.cart[i].nameid)*sd->status.cart[i].amount;
			sd->cart_num+-+-;
		}
	}

	memset(sd->paramb,0,sizeof(sd->paramb));
	memset(sd->parame,0,sizeof(sd->parame));
	sd->hit                 = 0;
	sd->flee                = 0;
	sd->flee2               = 0;
	sd->critical            = 0;
	sd->aspd                = 0;
	sd->watk                = 0;
	sd->def                 = 0;
	sd->mdef                = 0;
	sd->watk2               = 0;
	sd->def2                = 0;
	sd->mdef2               = 0;
	sd->status.max_hp       = 0;
	sd->status.max_sp       = 0;
	sd->range.attackrange   = 0;
	sd->range.attackrange_  = 0;
	sd->atk_ele             = ELE_NEUTRAL;
	sd->def_ele             = ELE_NEUTRAL;
	sd->star                = 0;
	sd->overrefine          = 0;
	sd->matk1               = 0;
	sd->matk2               = 0;
	sd->speed               = DEFAULT_WALK_SPEED;
	sd->hprate              = battle_config.hp_rate;
	sd->sprate              = battle_config.sp_rate;
	sd->castrate            = 100;
	sd->fixcastrate         = 0;
	sd->fixcastrate_        = 0;
	sd->dsprate             = 100;
	sd->base_atk            = 0;
	sd->status.max_ap       = 0;
	sd->patk                = 0;
	sd->smatk               = 0;
	sd->res                 = 0;
	sd->mres                = 0;
	sd->hplus               = 0;
	sd->crate               = 0;
#ifndef PRE_RENEWAL
	sd->plus_atk            = 0;
	sd->plus_matk           = 0;
#endif
	sd->arrow_atk           = 0;
	sd->arrow_ele           = ELE_NEUTRAL;
	sd->arrow_hit           = 0;
	sd->arrow_range         = 0;
	sd->nhealhp = sd->nhealsp = sd->nshealhp = sd->nshealsp = sd->nsshealhp = sd->nsshealsp = 0;
	memset(sd->addele,0,sizeof(sd->addele));
	memset(sd->addrace,0,sizeof(sd->addrace));
	memset(sd->addenemy,0,sizeof(sd->addenemy));
	memset(sd->addsize,0,sizeof(sd->addsize));
	memset(sd->addele_,0,sizeof(sd->addele_));
	memset(sd->addrace_,0,sizeof(sd->addrace_));
	memset(sd->addenemy_,0,sizeof(sd->addenemy_));
	memset(sd->addsize_,0,sizeof(sd->addsize_));
	memset(sd->subele,0,sizeof(sd->subele));
	memset(sd->subrace,0,sizeof(sd->subrace));
	memset(sd->subenemy,0,sizeof(sd->subenemy));
	memset(sd->addeff,0,sizeof(sd->addeff));
	memset(sd->addeff2,0,sizeof(sd->addeff2));
	memset(sd->reseff,0,sizeof(sd->reseff));
	memset(sd->addeff_range_flag,0,sizeof(sd->addeff));
	memset(&sd->special_state,0,sizeof(sd->special_state));
	memset(sd->weapon_coma_ele,0,sizeof(sd->weapon_coma_ele));
	memset(sd->weapon_coma_race,0,sizeof(sd->weapon_coma_race));
	memset(sd->weapon_coma_ele2,0,sizeof(sd->weapon_coma_ele2));
	memset(sd->weapon_coma_race2,0,sizeof(sd->weapon_coma_race2));
	memset(sd->weapon_atk,0,sizeof(sd->weapon_atk));
	memset(sd->weapon_atk_rate,0,sizeof(sd->weapon_atk_rate));
	memset(sd->auto_status_calc_pc,0,sizeof(sd->auto_status_calc_pc));
	memset(sd->eternal_status_change,0,sizeof(sd->eternal_status_change));

	sd->watk_       = 0;	// +ToxSAG1BdSg-
	sd->watk_2      = 0;
	sd->atk_ele_    = ELE_NEUTRAL;
	sd->star_       = 0;
	sd->overrefine_ = 0;

	sd->aspd_add       = 0;
	sd->aspd_rate      = 0;
	sd->aspd_add_rate  = 0;
	sd->speed_rate     = 0;
	sd->speed_add_rate = 0;
	sd->hprecov_rate   = 100;
	sd->sprecov_rate   = 100;
	sd->critical_def   = 0;
	sd->double_rate    = 0;
	sd->near_attack_def_rate = sd->long_attack_def_rate = 0;
	sd->atk_rate = sd->matk_rate = sd->matk2_rate = 100;
	memset(sd->ignore_def_ele,0,sizeof(sd->ignore_def_ele));
	memset(sd->ignore_def_race,0,sizeof(sd->ignore_def_race));
	memset(sd->ignore_def_enemy,0,sizeof(sd->ignore_def_enemy));
	memset(sd->ignore_def_ele_,0,sizeof(sd->ignore_def_ele_));
	memset(sd->ignore_def_race_,0,sizeof(sd->ignore_def_race_));
	memset(sd->ignore_def_enemy_,0,sizeof(sd->ignore_def_enemy_));
	memset(sd->ignore_mdef_ele,0,sizeof(sd->ignore_mdef_ele));
	memset(sd->ignore_mdef_race,0,sizeof(sd->ignore_mdef_race));
	memset(sd->ignore_mdef_enemy,0,sizeof(sd->ignore_mdef_enemy));
	sd->arrow_cri = 0;
	sd->magic_def_rate = sd->misc_def_rate = 0;
	memset(sd->arrow_addele,0,sizeof(sd->arrow_addele));
	memset(sd->arrow_addrace,0,sizeof(sd->arrow_addrace));
	memset(sd->arrow_addenemy,0,sizeof(sd->arrow_addenemy));
	memset(sd->arrow_addsize,0,sizeof(sd->arrow_addsize));
	memset(sd->arrow_addeff,0,sizeof(sd->arrow_addeff));
	memset(sd->arrow_addeff2,0,sizeof(sd->arrow_addeff2));
	memset(sd->magic_addele,0,sizeof(sd->magic_addele));
	memset(sd->magic_addrace,0,sizeof(sd->magic_addrace));
	memset(sd->magic_addenemy,0,sizeof(sd->magic_addenemy));
	memset(sd->magic_addsize,0,sizeof(sd->magic_addsize));
	memset(sd->magic_addeff,0,sizeof(sd->magic_addeff));
	memset(sd->magic_subrace,0,sizeof(sd->magic_subrace));
	sd->perfect_hit = 0;
	sd->critical_rate = sd->hit_rate = sd->flee_rate = sd->flee2_rate = 100;
	sd->def_rate = sd->def2_rate = sd->mdef_rate = sd->mdef2_rate = 100;
	sd->def_ratio_atk_ele = sd->def_ratio_atk_race = sd->def_ratio_atk_enemy = 0;
	sd->def_ratio_atk_ele_ = sd->def_ratio_atk_race_ = sd->def_ratio_atk_enemy_ = 0;
	sd->get_zeny_num = sd->get_zeny_num2 = 0;
	sd->add_damage_class_count = sd->add_damage_class_count_ = sd->add_magic_damage_class_count = 0;
	sd->add_def_class_count = sd->add_mdef_class_count = 0;
	memset(sd->add_damage_classrate,0,sizeof(sd->add_damage_classrate));
	memset(sd->add_damage_classrate_,0,sizeof(sd->add_damage_classrate_));
	memset(sd->add_magic_damage_classrate,0,sizeof(sd->add_magic_damage_classrate));
	memset(sd->add_def_classrate,0,sizeof(sd->add_def_classrate));
	memset(sd->add_mdef_classrate,0,sizeof(sd->add_mdef_classrate));
	memset(&sd->add_drop,0,sizeof(sd->add_drop));
	sd->add_drop_count = 0;
	sd->sp_gain_value = 0;
	sd->hp_gain_value = 0;
	sd->double_add_rate = sd->perfect_hit_add = sd->get_zeny_add_num = sd->get_zeny_add_num2 = 0;
	sd->splash_range = sd->splash_add_range = 0;
	memset(&sd->hp_drain,0,sizeof(sd->hp_drain));
	memset(&sd->sp_drain,0,sizeof(sd->sp_drain));
	memset(&sd->hp_drain_,0,sizeof(sd->hp_drain_));
	memset(&sd->sp_drain_,0,sizeof(sd->sp_drain_));
	sd->short_weapon_damage_return = sd->long_weapon_damage_return = sd->magic_damage_return = sd->sub_return_damage = 0;
	sd->break_weapon_rate = sd->break_armor_rate = 0;
	sd->add_steal_rate    = 0;
	sd->unbreakable_equip = LOC_NOTHING;
	sd->critical_damage   = 0;
	sd->hp_recov_stop = sd->sp_recov_stop = 0;
	memset(sd->critical_race,0,sizeof(sd->critical_race));
	memset(sd->critical_race_rate,0,sizeof(sd->critical_race_rate));
	memset(sd->subsize,0,sizeof(sd->subsize));
	memset(sd->magic_subsize,0,sizeof(sd->magic_subsize));
	memset(sd->exp_rate,0,sizeof(sd->exp_rate));
	memset(sd->job_rate,0,sizeof(sd->job_rate));
	memset(sd->hp_drain_rate_race,0,sizeof(sd->hp_drain_rate_race));
	memset(sd->sp_drain_rate_race,0,sizeof(sd->sp_drain_rate_race));
	memset(sd->hp_drain_value_race,0,sizeof(sd->hp_drain_value_race));
	memset(sd->sp_drain_value_race,0,sizeof(sd->sp_drain_value_race));
	memset(sd->addreveff,0,sizeof(sd->addreveff));
	sd->addreveff_flag = 0;
	memset(sd->addgroup,0,sizeof(sd->addgroup));
	memset(sd->addgroup_,0,sizeof(sd->addgroup_));
	memset(sd->magic_addgroup,0,sizeof(sd->magic_addgroup));
	memset(sd->arrow_addgroup,0,sizeof(sd->arrow_addgroup));
	memset(sd->subgroup,0,sizeof(sd->subgroup));
	sd->hp_penalty_time  = 0;
	sd->hp_penalty_value = 0;
	sd->sp_penalty_time  = 0;
	sd->sp_penalty_value = 0;
	memset(sd->hp_penalty_unrig_value,0,sizeof(sd->hp_penalty_unrig_value));
	memset(sd->sp_penalty_unrig_value,0,sizeof(sd->sp_penalty_unrig_value));
	memset(sd->hp_rate_penalty_unrig,0,sizeof(sd->hp_rate_penalty_unrig));
	memset(sd->sp_rate_penalty_unrig,0,sizeof(sd->sp_rate_penalty_unrig));
	sd->mob_class_change_rate = 0;
	memset(&sd->skill_dmgup,0,sizeof(sd->skill_dmgup));
	memset(&sd->sub_skill_damage,0,sizeof(sd->sub_skill_damage));
	memset(&sd->skill_blow,0,sizeof(sd->skill_blow));
	memset(&sd->autospell,0,sizeof(sd->autospell));
	memset(&sd->itemheal_rate,0,sizeof(sd->itemheal_rate));
	memset(&sd->autoraise,0,sizeof(sd->autoraise));
	memset(&sd->hp_vanish,0,sizeof(sd->hp_vanish));
	memset(&sd->sp_vanish,0,sizeof(sd->sp_vanish));
	sd->bonus_damage      = 0;
	sd->curse_by_muramasa = 0;
	memset(sd->loss_equip_rate_when_die,0,sizeof(sd->loss_equip_rate_when_die));
	memset(sd->loss_equip_rate_when_attack,0,sizeof(sd->loss_equip_rate_when_attack));
	memset(sd->loss_equip_rate_when_hit,0,sizeof(sd->loss_equip_rate_when_hit));
	memset(sd->break_myequip_rate_when_attack,0,sizeof(sd->break_myequip_rate_when_attack));
	memset(sd->break_myequip_rate_when_hit,0,sizeof(sd->break_myequip_rate_when_hit));
	sd->loss_equip_flag = 0;
	sd->short_weapon_damege_rate = sd->long_weapon_damege_rate = 0;
	sd->range.add_attackrange = 0;
	sd->range.add_attackrange_rate = 100;
	sd->special_state.item_no_use = 0;
	sd->skill_delay_rate = 0;
	memset(&sd->fix_status,0,sizeof(sd->fix_status));
	memset(&sd->skill_fixcastrate,0,sizeof(sd->skill_fixcastrate));
	memset(&sd->skill_addcastrate,0,sizeof(sd->skill_addcastrate));
	memset(&sd->skill_healup,0,sizeof(sd->skill_healup));
	memset(&sd->activeitem,0,sizeof(sd->activeitem));
	memset(sd->def_eleenemy,0,sizeof(sd->def_eleenemy));
	memset(&sd->skill_addspcost,0,sizeof(sd->skill_addspcost));
	memset(&sd->skill_subhealup,0,sizeof(sd->skill_subhealup));
	memset(&sd->skill_addcast,0,sizeof(sd->skill_addcast));
	memset(&sd->skill_addfixcast,0,sizeof(sd->skill_addfixcast));
	memset(&sd->skill_cooldown,0,sizeof(sd->skill_cooldown));
	memset(&sd->skill_eleweapon_dmgup,0,sizeof(sd->skill_eleweapon_dmgup));
	memset(&sd->skill_elemagic_dmgup,0,sizeof(sd->skill_elemagic_dmgup));
	sd->hp_rate_penalty_time  = 0;
	sd->hp_rate_penalty_value = 0;
	sd->sp_rate_penalty_time  = 0;
	sd->sp_rate_penalty_value = 0;

	for(i=0; i<EQUIP_INDEX_MAX; i+-+-) {
		if(i == EQUIP_INDEX_ARROW)
			continue;
		idx = sd->equip_index[i];
		current_equip_item_index = i;	// +kOhPTTDBMKcwwzCvdSg-
		if(idx < 0)
			continue;
		if(i == EQUIP_INDEX_RARM && sd->equip_index[EQUIP_INDEX_LARM] == idx)
			continue;
		if(i == EQUIP_INDEX_HEAD3 && sd->equip_index[EQUIP_INDEX_HEAD] == idx)
			continue;
		if(i == EQUIP_INDEX_HEAD2 && (sd->equip_index[EQUIP_INDEX_HEAD3] == idx || sd->equip_index[EQUIP_INDEX_HEAD] == idx))
			continue;
		if(i == EQUIP_INDEX_COSTUME_HEAD3 && sd->equip_index[EQUIP_INDEX_COSTUME_HEAD] == idx)
			continue;
		if(i == EQUIP_INDEX_COSTUME_HEAD2 && (sd->equip_index[EQUIP_INDEX_COSTUME_HEAD3] == idx || sd->equip_index[EQUIP_INDEX_COSTUME_HEAD] == idx))
			continue;

		if(sd->inventory_data[idx]) {
			if(itemdb_isweapon(sd->inventory_data[idx]->nameid)) {
				int j;
				if( !itemdb_isspecial(sd->status.inventory[idx].card[0]) ) {
					int c;
					for(j=0; j<4; j+-+-) {	// +MKsw/DDJ-
						if((c = sd->status.inventory[idx].card[j]) <= 0)
							continue;
						if(sd->inventory_data[idx]->slot < j && itemdb_cardtype(c) != 2)
							continue;
						current_equip_name_id = c;		// +MKow/DDIMLkw2jDr-(+kc2JBzDBMKcwwzCvdSg-)
						if(i == EQUIP_INDEX_LARM && sd->status.inventory[idx].equip == LOC_LARM)
							sd->state.lr_flag = 1;
						if(calclimit == 2)
							run_script(itemdb_usescript(c),0,sd->bl.id,0);
						run_script(itemdb_equipscript(c),0,sd->bl.id,0);
						sd->state.lr_flag = 0;
					}
				}
				for(j=0; j<5; j+-+-) {	// +MOkw8zDAMOAwqjDXMLcw5zDz-
					if(sd->status.inventory[idx].opt[j].id <= 0)
						continue;
					if(i == EQUIP_INDEX_LARM && sd->status.inventory[idx].equip == LOC_LARM)
						sd->state.lr_flag = 1;
					bonus_randopt(sd, sd->status.inventory[idx].opt[j].id, sd->status.inventory[idx].opt[j].val);
					sd->state.lr_flag = 0;
				}
			}
			else if(itemdb_isarmor(sd->inventory_data[idx]->nameid)) { // +ljJRdw-
				int j;
				if( !itemdb_isspecial(sd->status.inventory[idx].card[0]) ) {
					int c;
					for(j=0; j<4; j+-+-) {	// +MKsw/DDJ-
						if((c = sd->status.inventory[idx].card[j]) <= 0)
							continue;
						if(sd->inventory_data[idx]->slot < j && itemdb_cardtype(c) != 2)
							continue;
						current_equip_name_id = c;		// +MKow/DDIMLkw2jDr-(+kc2JBzDBMKcwwzCvdSg-)
						if(calclimit == 2)
							run_script(itemdb_usescript(c),0,sd->bl.id,0);
						run_script(itemdb_equipscript(c),0,sd->bl.id,0);
					}
				}
				for(j=0; j<5; j+-+-) {	// +MOkw8zDAMOAwqjDXMLcw5zDz-
					if(sd->status.inventory[idx].opt[j].id <= 0)
						continue;
					bonus_randopt(sd, sd->status.inventory[idx].opt[j].id, sd->status.inventory[idx].opt[j].val);
				}
			}
		}
	}

	for(i = 0; i < sd->activeitem.count; i+-+-)
	{
		if(sd->activeitem_timer[i] == -1)
			continue;
		if(sd->activeitem.id[i] != sd->activeitem_id2[i])
			continue;

		run_script(itemdb_bonusscript(sd->activeitem_id2[i]),0,sd->bl.id,0);
	}

	wele    = sd->atk_ele;
	wele_   = sd->atk_ele_;
	def_ele = sd->def_ele;
	if(battle_config.pet_status_support) {
		if(sd->status.pet_id > 0 && sd->petDB && sd->pet.intimate > 0) {
			run_script(sd->petDB->script,0,sd->bl.id,0);
		}
		pele        = sd->atk_ele;
		pdef_ele    = sd->def_ele;
		sd->atk_ele = sd->def_ele = ELE_NEUTRAL;
	}
	memcpy(sd->paramcard,sd->parame,sizeof(sd->paramcard));

	// +iMVQmVTBMGswiDCLMLkwxjD8ML8wuVkJUxYwbzBTMFMwZ1ufiEw-
	for(i=0; i<EQUIP_INDEX_MAX; i+-+-) {
		if(i == EQUIP_INDEX_ARROW)
			continue;
		idx = sd->equip_index[i];
		current_equip_item_index = i;	// +kOhPTTDBMKcwwzCvdSg-
		if(idx < 0)
			continue;
		if(i == EQUIP_INDEX_RARM && sd->equip_index[EQUIP_INDEX_LARM] == idx)
			continue;
		if(i == EQUIP_INDEX_HEAD3 && sd->equip_index[EQUIP_INDEX_HEAD] == idx)
			continue;
		if(i == EQUIP_INDEX_HEAD2 && (sd->equip_index[EQUIP_INDEX_HEAD3] == idx || sd->equip_index[EQUIP_INDEX_HEAD] == idx))
			continue;
		if(i == EQUIP_INDEX_COSTUME_HEAD3 && sd->equip_index[EQUIP_INDEX_COSTUME_HEAD] == idx)
			continue;
		if(i == EQUIP_INDEX_COSTUME_HEAD2 && (sd->equip_index[EQUIP_INDEX_COSTUME_HEAD3] == idx || sd->equip_index[EQUIP_INDEX_COSTUME_HEAD] == idx))
			continue;
		if(sd->inventory_data[idx]) {
			current_equip_name_id = sd->inventory_data[idx]->nameid;
			sd->matk1 +-= sd->inventory_data[idx]->matk;
#ifdef PRE_RENEWAL
			sd->matk2 +-= sd->inventory_data[idx]->matk;
#endif
			sd->def +-= sd->inventory_data[idx]->def;
			sd->mdef +-= sd->inventory_data[idx]->mdef;
			if(itemdb_isweapon(sd->inventory_data[idx]->nameid)) {
				int r,wlv = sd->inventory_data[idx]->wlv;
				if(i == EQUIP_INDEX_LARM && sd->status.inventory[idx].equip == LOC_LARM) {
					// +ToxSAG1BdSgwxzD8ML9RZVKb-
					sd->watk_ +-= sd->inventory_data[idx]->atk;
					if((r = sd->status.inventory[idx].refine) > 0)
						sd->watk_2 = refine_db[wlv].safety_bonus[r-1];	// +fL6TLGU7ZINSmw-
#ifndef PRE_RENEWAL
					if(sd->status.weapon != WT_BOW && r > 0)	// +XxMwazBvfL6TLA-MATK+MNww/DDKMLkwTDBqMEQ-
						sd->matk1 +-= refine_db[wlv].safety_bonus[r-1];
#endif
					if((r -= refine_db[wlv].limit) > 0)	// +kE5ScHy+kyww3DD8MMowuQ-
						sd->overrefine_ = r*refine_db[wlv].over_bonus;

					if(sd->status.inventory[idx].card[0] == 0x00ff) {	// +iP2QIGtmVmg-
						sd->star_ = (sd->status.inventory[idx].card[1]>>8);	// +Zh8wbjBLMFEwiQ-
						if(sd->star_ == 15)
							sd->star_ = 40;
						wele_= sd->status.inventory[idx].card[1] & 0x0f;	// +XF4- +YCc-
						// +MOkw8zCtMPMwsDDcMPwwyjC5-
						if(ranking_get_id2rank(*((int *)(&sd->status.inventory[idx].card[2])), RK_BLACKSMITH))
							sd->ranker_weapon_bonus_ = battle_config.ranker_weapon_bonus;
					}
					sd->range.attackrange_ +-= sd->inventory_data[idx]->range;
					sd->state.lr_flag = 1;
					if(calclimit == 2)
						run_script(sd->inventory_data[idx]->use_script,0,sd->bl.id,0);
					run_script(sd->inventory_data[idx]->equip_script,0,sd->bl.id,0);
					sd->state.lr_flag = 0;
				} else {
					// +ToxSAG1Ba2ZWaE7lWRY-
					sd->watk  +-= sd->inventory_data[idx]->atk;
					if((r = sd->status.inventory[idx].refine) > 0)
						sd->watk2 +-= refine_db[wlv].safety_bonus[r-1];	// +fL6TLGU7ZINSmw-
#ifndef PRE_RENEWAL
					if(sd->status.weapon != WT_BOW && r > 0)	// +XxMwazBvfL6TLA-MATK+MNww/DDKMLkwTDBqMEQ-
						sd->matk1 +-= refine_db[wlv].safety_bonus[r-1];
#endif
					if((r -= refine_db[wlv].limit) > 0)	// +kE5ScHy+kyww3DD8MMowuQ-
						sd->overrefine +-= r*refine_db[wlv].over_bonus;

					if(sd->status.inventory[idx].card[0] == 0x00ff) {	// +iP2QIGtmVmg-
						sd->star +-= (sd->status.inventory[idx].card[1]>>8);	// +Zh8wbjBLMFEwiQ-
						if(sd->star == 15)
							sd->star = 40;
						wele = sd->status.inventory[idx].card[1] & 0x0f;	// +XF4- +YCc-
						// +MOkw8zCtMPMwsDDcMPwwyjC5-
						if(ranking_get_id2rank(*((int *)(&sd->status.inventory[idx].card[2])),RK_BLACKSMITH))
							sd->ranker_weapon_bonus = battle_config.ranker_weapon_bonus;
					}
					sd->range.attackrange +-= sd->inventory_data[idx]->range;
					if(calclimit == 2)
						run_script(sd->inventory_data[idx]->use_script,0,sd->bl.id,0);
					run_script(sd->inventory_data[idx]->equip_script,0,sd->bl.id,0);
				}
			} else if(itemdb_isarmor(sd->inventory_data[idx]->nameid)) {
				sd->watk  +-= sd->inventory_data[idx]->atk;
				if(sd->status.inventory[idx].refine > 0)
					refinedef +-= refine_db[0].safety_bonus[sd->status.inventory[idx].refine - 1];
				if(calclimit == 2)
					run_script(sd->inventory_data[idx]->use_script,0,sd->bl.id,0);
				run_script(sd->inventory_data[idx]->equip_script,0,sd->bl.id,0);
			}
		}
	}

	if(sd->equip_index[EQUIP_INDEX_ARROW] >= 0) { // +d+I-
		idx = sd->equip_index[EQUIP_INDEX_ARROW];
		if(sd->inventory_data[idx]) {		// +MH4wYFxeYCcwTFFlMGMwZjBEMGowRA-
			current_equip_name_id = sd->inventory_data[idx]->nameid;
			sd->state.lr_flag = 2;
			run_script(sd->inventory_data[idx]->equip_script,0,sd->bl.id,0);
			sd->state.lr_flag = 0;
			sd->arrow_atk +-= sd->inventory_data[idx]->atk;
		}
	}

	sd->def +-= (refinedef+-50)/100;

	if(sd->range.attackrange < 1)  sd->range.attackrange  = 1;
	if(sd->range.attackrange_ < 1) sd->range.attackrange_ = 1;
	if(sd->range.attackrange < sd->range.attackrange_)
		sd->range.attackrange = sd->range.attackrange_;
	if(sd->status.weapon == WT_BOW)
		sd->range.attackrange +-= sd->arrow_range;
	if(wele > ELE_NEUTRAL)
		sd->atk_ele = wele;
	if(wele_ > ELE_NEUTRAL)
		sd->atk_ele_ = wele_;
	if(def_ele > ELE_NEUTRAL)
		sd->def_ele = def_ele;
	if(battle_config.pet_status_support) {
		if(pele > ELE_NEUTRAL && !sd->atk_ele)
			sd->atk_ele = pele;
		if(pdef_ele > ELE_NEUTRAL && !sd->def_ele)
			sd->def_ele = pdef_ele;
	}
	sd->double_rate +-= sd->double_add_rate;
	sd->perfect_hit +-= sd->perfect_hit_add;
	sd->get_zeny_num = (sd->get_zeny_num +- sd->get_zeny_add_num > 100) ? 100 : (sd->get_zeny_num +- sd->get_zeny_add_num);
	sd->get_zeny_num2 = (sd->get_zeny_num2 +- sd->get_zeny_add_num2 > 100) ? 100 : (sd->get_zeny_num2 +- sd->get_zeny_add_num2);
	sd->splash_range +-= sd->splash_add_range;

	// +a2ZWaA-ATK+MLUwpDC6iNxrYw-
	for(i=0; i<MAX_SIZE_FIX; i+-+-) {
		sd->atkmods[i]  = atkmods[i][sd->weapontype1];	// +U/NiSw-
		sd->atkmods_[i] = atkmods[i][sd->weapontype2];	// +XeZiSw-
	}

	// job+MNww/DDKMLlSBg-
	for(i=0; i<sd->status.job_level && i<MAX_LEVEL; i+-+-) {
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x01)
			sd->paramb[0]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x02)
			sd->paramb[1]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x04)
			sd->paramb[2]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x08)
			sd->paramb[3]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x10)
			sd->paramb[4]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x20)
			sd->paramb[5]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x040)
			sd->paramb[6]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x080)
			sd->paramb[7]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x100)
			sd->paramb[8]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x200)
			sd->paramb[9]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x400)
			sd->paramb[10]+-+-;
		if(job_db[sd->s_class.job].bonus[sd->s_class.upper][i] & 0x800)
			sd->paramb[11]+-+-;
	}

	if((skill = pc_checkskill(sd,AC_OWL)) > 0) {	// +MHUwTzCNMEYwbnbu-
		sd->paramb[4] +-= skill;
	}
	if(pc_checkskill(sd,BS_HILTBINDING)) {	// +MNIw6zDIMNAwpDDzMMcwozDzMLA-
		sd->paramb[0] +-= 1;
		// +Zyyb1mcqW5+IxTBuMF8wgTCzMOEw8zDIMKIwpjDI-
		//sd->watk +-= 4;
	}
	if((skill = pc_checkskill(sd,SA_DRAGONOLOGY)) > 0) {	// +MMkw6TC0MM4w7TC4MPw-
		sd->paramb[3] +-= (skill+-1)>>1;
	}
	if((skill = pc_checkskill(sd,RA_RESEARCHTRAP)) > 0) {	// +MMgw6TDDMNd4FHp2-
		sd->paramb[3] +-= skill;
	}
	if(pc_checkskill(sd,SU_POWEROFLAND) > 0) {	// +WSdXMDBuUps-
		sd->paramb[3] +-= 7;
	}
	if((skill = pc_checkskill(sd,NW_GRENADE_MASTERY)) > 0) {	// +MLAw7DDNMPwwyTDeMLkwvzDqMPw-
		sd->paramb[10] +-= skill;
	}
	if((skill = pc_checkskill(sd,SOA_SOUL_MASTERY)) > 0) {	// +lwqQU4hTT+599A-
		sd->paramb[9] +-= skill;
	}
#ifndef PRE_RENEWAL
	if((skill = pc_checkskill(sd,SG_STAR_BLESS)) > 0 && (battle_config.allow_skill_without_day || is_day_of_star())) {	// +Zh8wbnldeY8-
		sd->paramb[0] +-= skill * 2;
		sd->paramb[1] +-= skill * 2;
		sd->paramb[2] +-= skill * 2;
		sd->paramb[3] +-= skill * 2;
		sd->paramb[4] +-= skill * 2;
		sd->paramb[5] +-= skill * 2;
	} else if((skill = pc_checkskill(sd,SG_SUN_BLESS)) > 0 && (battle_config.allow_skill_without_day || is_day_of_sun())) {	// +WSqWfTBueV15jw-
		sd->paramb[0] +-= skill * 2;
		sd->paramb[2] +-= skill * 2;
		sd->paramb[4] +-= skill * 2;
	} else if((skill = pc_checkskill(sd,SG_MOON_BLESS)) > 0 && (battle_config.allow_skill_without_day || is_day_of_moon())) {	// +ZwgwbnldeY8-
		sd->paramb[1] +-= skill * 2;
		sd->paramb[3] +-= skill * 2;
		sd->paramb[5] +-= skill * 2;
	}
#endif

	// +MN4w/DDAMOkw/DDcMPwwyjC5-
	if(battle_config.pk_murderer_point > 0) {
		int point = ranking_get_point(sd,RK_PK);
		if(point >= battle_config.pk_murderer_point * 4) {
			sd->paramb[0] +-= 5;
			sd->paramb[1] +-= 5;
			sd->paramb[2] +-= 5;
			sd->paramb[3] +-= 5;
			sd->paramb[4] +-= 5;
			sd->paramb[5] +-= 5;
			sd->atk_rate  +-= 10;
			sd->matk_rate +-= 10;
		} else if(point >= battle_config.pk_murderer_point) {
			sd->paramb[0] +-= 3;
			sd->paramb[1] +-= 3;
			sd->paramb[2] +-= 3;
			sd->paramb[3] +-= 3;
			sd->paramb[4] +-= 3;
			sd->paramb[5] +-= 3;
			sd->atk_rate  +-= 10;
			sd->matk_rate +-= 10;
		}
	}
	// 1+XqYwgmt7MJMwZzBqMEQ-Job70+MLkw0TDOMNM-or+YuFfNTC5MNEwzjDTMGsAKw-10
	if(((sd->s_class.job == PC_JOB_SNV && sd->status.job_level >= 70) || sd->s_class.job == PC_JOB_ESNV) && sd->status.die_counter == 0) {
		sd->paramb[0] +-= 10;
		sd->paramb[1] +-= 10;
		sd->paramb[2] +-= 10;
		sd->paramb[3] +-= 10;
		sd->paramb[4] +-= 10;
		sd->paramb[5] +-= 10;
	}

	// +MK4w6zDJMLkwrTDr-
	if(battle_config.guild_hunting_skill_available)
	{
		struct guild *g = guild_search(sd->status.guild_id);	// +MK4w6zDJU9Zflw-
		struct map_session_data *gmsd = NULL;

		if(g)
			gmsd = guild_get_guildmaster_sd(g);

		// +MK4w6zDJZwk- && +MN4wuTC/MPxjpX2a- && +gepSBg-!=+MN4wuTC/MPw- && +VAwwWDDeMMMw1w-
		if(g && gmsd && (battle_config.allow_me_guild_skill == 1 || gmsd != sd) && sd->bl.m == gmsd->bl.m)
		{
			if(battle_config.guild_skill_check_range) {	// +jd2W4lIkW5owkohMMEY-
				int dx = abs(sd->bl.x - gmsd->bl.x);
				int dy = abs(sd->bl.y - gmsd->bl.y);
				int range;

				if(battle_config.guild_skill_effective_range > 0) {	// +VAxOAI3dluIwZ4oIe5c-
					range = battle_config.guild_skill_effective_range;
					if(dx <= range && dy <= range) {
						sd->paramb[0] +-= guild_checkskill(g,GD_LEADERSHIP);	// str
						sd->paramb[1] +-= guild_checkskill(g,GD_SOULCOLD);	// agi
						sd->paramb[2] +-= guild_checkskill(g,GD_GLORYWOUNDS);	// vit
						sd->paramb[4] +-= guild_checkskill(g,GD_HAWKEYES);	// dex
						sd->under_the_influence_of_the_guild_skill = range+-1;	// 0>+MGdfcZf/Tgs-,+kc0wajCLWDRUCDCCMEIwizBuMGcAKw-1
					} else {
						sd->under_the_influence_of_the_guild_skill = 0;
					}
				} else {	// +UAtSJY3dluI-
					int min_range = 0x7fffffff;
					range = skill_get_range(GD_LEADERSHIP,guild_skill_get_lv(g,GD_LEADERSHIP));
					if(dx <= range && dy <= range) {
						sd->paramb[0] +-= guild_checkskill(g,GD_LEADERSHIP);	// str
						if(min_range > range) min_range = range;
					}
					range = skill_get_range(GD_SOULCOLD,guild_skill_get_lv(g,GD_SOULCOLD));
					if(dx <= range && dy <= range) {
						sd->paramb[1] +-= guild_checkskill(g,GD_SOULCOLD);	// agi
						if(min_range > range) min_range = range;
					}
					range = skill_get_range(GD_GLORYWOUNDS,guild_skill_get_lv(g,GD_GLORYWOUNDS));
					if(dx <= range && dy <= range) {
						sd->paramb[2] +-= guild_checkskill(g,GD_GLORYWOUNDS);	// vit
						if(min_range > range) min_range = range;
					}

					range = skill_get_range(GD_HAWKEYES,guild_skill_get_lv(g,GD_HAWKEYES));
					if(dx <= range && dy <= range) {
						sd->paramb[4] +-= guild_checkskill(g,GD_HAWKEYES);	// dex
						if(min_range > range) min_range = range;
					}
					if(min_range == 0x7fffffff)
						sd->under_the_influence_of_the_guild_skill = 0;
					else
						sd->under_the_influence_of_the_guild_skill = min_range+-1;
				}
			} else {	// +MN4wwzDXUWhPUw-
				sd->paramb[0] +-= guild_checkskill(g,GD_LEADERSHIP);	// str
				sd->paramb[1] +-= guild_checkskill(g,GD_SOULCOLD);	// agi
				sd->paramb[2] +-= guild_checkskill(g,GD_GLORYWOUNDS);	// vit
				sd->paramb[4] +-= guild_checkskill(g,GD_HAWKEYES);	// dex
				sd->under_the_influence_of_the_guild_skill = battle_config.guild_skill_effective_range+-1;
			}
		} else {
			sd->under_the_influence_of_the_guild_skill = 0;
		}
	} else {	// +MN4wwzDXMEyQVTBjMF8wiiAmcSFSuTBgMGMwXzCK-
		sd->under_the_influence_of_the_guild_skill = 0;
	}

	// +MLkwxjD8ML8wuVkJUxYwazCIMItX+mcsMNEw6TDhMPwwv4jca2M-
	if(sd->sc.count) {
		// +lsZOLVKbVBFOCg-
		if(sd->sc.data[SC_CONCENTRATE].timer != -1 && sd->sc.data[SC_QUAGMIRE].timer == -1) {
			sd->paramb[1] +-= (sd->status.agi+-sd->paramb[1]+-sd->parame[1]-sd->paramcard[1])*(2+-sd->sc.data[SC_CONCENTRATE].val1)/100;
			sd->paramb[4] +-= (sd->status.dex+-sd->paramb[4]+-sd->parame[4]-sd->paramcard[4])*(2+-sd->sc.data[SC_CONCENTRATE].val1)/100;
		}
		// +MLQwuTDaMOs-ALL+-20
		if(sd->sc.data[SC_INCALLSTATUS].timer != -1) {
			sd->paramb[0] +-= sd->sc.data[SC_INCALLSTATUS].val1;
			sd->paramb[1] +-= sd->sc.data[SC_INCALLSTATUS].val1;
			sd->paramb[2] +-= sd->sc.data[SC_INCALLSTATUS].val1;
			sd->paramb[3] +-= sd->sc.data[SC_INCALLSTATUS].val1;
			sd->paramb[4] +-= sd->sc.data[SC_INCALLSTATUS].val1;
			sd->paramb[5] +-= sd->sc.data[SC_INCALLSTATUS].val1;
		}

		// +TgpPTU4AayGAdzBum0I-
		if(sd->sc.data[SC_HIGH].timer != -1) {
			if(sd->status.base_level < 60) {
				if(sd->status.str < sd->status.base_level-10)  sd->paramb[0] +-= sd->status.base_level-10 - sd->status.str;
				if(sd->status.agi < sd->status.base_level-10)  sd->paramb[1] +-= sd->status.base_level-10 - sd->status.agi;
				if(sd->status.vit < sd->status.base_level-10)  sd->paramb[2] +-= sd->status.base_level-10 - sd->status.vit;
				if(sd->status.int_ < sd->status.base_level-10) sd->paramb[3] +-= sd->status.base_level-10 - sd->status.int_;
				if(sd->status.dex < sd->status.base_level-10)  sd->paramb[4] +-= sd->status.base_level-10 - sd->status.dex;
				if(sd->status.luk < sd->status.base_level-10)  sd->paramb[5] +-= sd->status.base_level-10 - sd->status.luk;
			}
			else {
				if(sd->status.str < 50)  sd->paramb[0] +-= 50 - sd->status.str;
				if(sd->status.agi < 50)  sd->paramb[1] +-= 50 - sd->status.agi;
				if(sd->status.vit < 50)  sd->paramb[2] +-= 50 - sd->status.vit;
				if(sd->status.int_ < 50) sd->paramb[3] +-= 50 - sd->status.int_;
				if(sd->status.dex < 50)  sd->paramb[4] +-= 50 - sd->status.dex;
				if(sd->status.luk < 50)  sd->paramb[5] +-= 50 - sd->status.luk;
			}
		}

		// +mN9Oi3Uo-
		if(sd->sc.data[SC_MEAL_INCSTR2].timer != -1)
			sd->paramb[0] +-= sd->sc.data[SC_MEAL_INCSTR2].val1;
		else if(sd->sc.data[SC_MEAL_INCSTR].timer != -1)
			sd->paramb[0] +-= sd->sc.data[SC_MEAL_INCSTR].val1;
		if(sd->sc.data[SC_MEAL_INCAGI2].timer != -1)
			sd->paramb[1] +-= sd->sc.data[SC_MEAL_INCAGI2].val1;
		else if(sd->sc.data[SC_MEAL_INCAGI].timer != -1)
			sd->paramb[1] +-= sd->sc.data[SC_MEAL_INCAGI].val1;
		if(sd->sc.data[SC_MEAL_INCVIT2].timer != -1)
			sd->paramb[2] +-= sd->sc.data[SC_MEAL_INCVIT2].val1;
		else if(sd->sc.data[SC_MEAL_INCVIT].timer != -1)
			sd->paramb[2] +-= sd->sc.data[SC_MEAL_INCVIT].val1;
		if(sd->sc.data[SC_MEAL_INCINT2].timer != -1)
			sd->paramb[3] +-= sd->sc.data[SC_MEAL_INCINT2].val1;
		else if(sd->sc.data[SC_MEAL_INCINT].timer != -1)
			sd->paramb[3] +-= sd->sc.data[SC_MEAL_INCINT].val1;
		if(sd->sc.data[SC_MEAL_INCDEX2].timer != -1)
			sd->paramb[4] +-= sd->sc.data[SC_MEAL_INCDEX2].val1;
		else if(sd->sc.data[SC_MEAL_INCDEX].timer != -1)
			sd->paramb[4] +-= sd->sc.data[SC_MEAL_INCDEX].val1;
		if(sd->sc.data[SC_MEAL_INCLUK2].timer != -1)
			sd->paramb[5] +-= sd->sc.data[SC_MEAL_INCLUK2].val1;
		else if(sd->sc.data[SC_MEAL_INCLUK].timer != -1)
			sd->paramb[5] +-= sd->sc.data[SC_MEAL_INCLUK].val1;

		// +ML8wpDDqMK4wbjC5MNEw/DDIcrZhSw- STR+-10
		if(sd->sc.data[SC_SPURT].timer != -1)
			sd->paramb[0] +-= 10;

		// +MK4w6zDJMLkwrTDr- +gehiJmFLUuI-
		if(sd->sc.data[SC_BATTLEORDER].timer != -1) {
			sd->paramb[0] +-= 5*sd->sc.data[SC_BATTLEORDER].val1;
			sd->paramb[3] +-= 5*sd->sc.data[SC_BATTLEORDER].val1;
			sd->paramb[4] +-= 5*sd->sc.data[SC_BATTLEORDER].val1;
		}

		if(sd->sc.data[SC_CHASEWALK_STR].timer != -1)
			sd->paramb[0] +-= sd->sc.data[SC_CHASEWALK_STR].val1;

		if(sd->sc.data[SC_INCREASEAGI].timer != -1)	// +kB9epliXUqA-
			sd->paramb[1] +-= 2+-sd->sc.data[SC_INCREASEAGI].val1;

		if(sd->sc.data[SC_DECREASEAGI].timer != -1)	// +kB9epm4bXBH/CDCqMPww0DD8MLkwrTDrTtVp2DBv-AGI-50+/wk-
			sd->paramb[1] -= (sd->sc.data[SC_DECREASEAGI].val2)? 50: 2+-sd->sc.data[SC_DECREASEAGI].val1;

		if(sd->sc.data[SC_ADORAMUS].timer != -1)	// +MKIwyTDpMOAwuQ-
			sd->paramb[1] -= 2+-sd->sc.data[SC_ADORAMUS].val1;

		if(sd->sc.data[SC_ARCLOUSEDASH].timer != -1)	// +MKIwrzDpMKYwuTDAMMMwtzDl-
			sd->paramb[1] +-= sd->sc.data[SC_ARCLOUSEDASH].val2;

		if(sd->sc.data[SC_BLESSING].timer != -1) {	// +MNYw7DDDMLcw8zCw-
			sd->paramb[0] +-= sd->sc.data[SC_BLESSING].val1;
			sd->paramb[3] +-= sd->sc.data[SC_BLESSING].val1;
			sd->paramb[4] +-= sd->sc.data[SC_BLESSING].val1;
		}
		if(sd->sc.data[SC_NEN].timer != -1) {	// +X/U-
			sd->paramb[0] +-= sd->sc.data[SC_NEN].val1;
			sd->paramb[3] +-= sd->sc.data[SC_NEN].val1;
		}
		if(sd->sc.data[SC_SUITON].timer != -1) {	// +bDSQQQ-
			if(sd->sc.data[SC_SUITON].val3)
				sd->paramb[1] +-= sd->sc.data[SC_SUITON].val3;
		}

		if(sd->sc.data[SC_GLORIA].timer != -1)	// +MLAw7TDqMKI-
			sd->paramb[5] +-= 30;

		if(sd->sc.data[SC_LOUD].timer != -1 && sd->sc.data[SC_QUAGMIRE].timer == -1)	// +MOkwpjDJMNwwpDC5-
			sd->paramb[0] +-= 4;

		if(sd->sc.data[SC_TRUESIGHT].timer != -1) {	// +MMgwpTDrMPwwtTCkMMg-
			sd->paramb[0] +-= 5;
			sd->paramb[1] +-= 5;
			sd->paramb[2] +-= 5;
			sd->paramb[3] +-= 5;
			sd->paramb[4] +-= 5;
			sd->paramb[5] +-= 5;
		}
		if(sd->sc.data[SC_INCREASING].timer != -1) { // +MKQw8zCvMOow/DC4MPMwsDCiMK0w5TDpMLcw/A-
			sd->paramb[1] +-= 4;
			sd->paramb[4] +-= 4;
		}

		// +MMcwozDVMKcw8zC5-
		if(sd->sc.data[SC_DEFENCE].timer != -1)
			sd->paramb[2] +-= sd->sc.data[SC_DEFENCE].val1*2;

		if(sd->sc.data[SC_QUAGMIRE].timer != -1) {	// +MK8woTCwMN4wpDCi-
			short subagi = 0;
			short subdex = 0;
#ifdef PRE_RENEWAL
			subagi = sd->status.agi * sd->sc.data[SC_QUAGMIRE].val1*10 / 100;
			subdex = sd->status.dex * sd->sc.data[SC_QUAGMIRE].val1*10 / 100;
#else
			subagi = (sd->status.agi/2 < sd->sc.data[SC_QUAGMIRE].val1*10) ? sd->status.agi/2 : sd->sc.data[SC_QUAGMIRE].val1*10;
			subdex = (sd->status.dex/2 < sd->sc.data[SC_QUAGMIRE].val1*10) ? sd->status.dex/2 : sd->sc.data[SC_QUAGMIRE].val1*10;
#endif
			if(map[sd->bl.m].flag.pvp || map[sd->bl.m].flag.gvg) {
				subagi /= 2;
				subdex /= 2;
			}
			sd->paramb[1] -= subagi;
			sd->paramb[4] -= subdex;
		}

		// +MN4w/DC3MOUwqjDWMKIw0zC5-
		if(sd->sc.data[SC_MARSHOFABYSS].timer != -1) {
			sd->paramb[1] -= sd->status.agi * (sd->sc.data[SC_MARSHOFABYSS].val3 / 2) / 100;
			sd->paramb[4] -= sd->status.dex * (sd->sc.data[SC_MARSHOFABYSS].val3 / 2) / 100;
		}

		if(sd->sc.data[SC_MARIONETTE].timer != -1) {
			sd->paramb[0] -= sd->status.str/2;
			sd->paramb[1] -= sd->status.agi/2;
			sd->paramb[2] -= sd->status.vit/2;
			sd->paramb[3] -= sd->status.int_/2;
			sd->paramb[4] -= sd->status.dex/2;
			sd->paramb[5] -= sd->status.luk/2;
		}

		if(sd->sc.data[SC_MARIONETTE2].timer != -1)
		{
			struct map_session_data* ssd = map_id2sd(sd->sc.data[SC_MARIONETTE2].val2);
			if(ssd) {
				if(battle_config.marionette_type) {
					sd->paramb[0] +-= ssd->status.str/2;
					sd->paramb[1] +-= ssd->status.agi/2;
					sd->paramb[2] +-= ssd->status.vit/2;
					sd->paramb[3] +-= ssd->status.int_/2;
					sd->paramb[4] +-= ssd->status.dex/2;
					sd->paramb[5] +-= ssd->status.luk/2;
				} else if(map[sd->bl.m].flag.pk) {
					// +/zD/KzDeMMMw1zBu-MC+UjaWUA-
					// str
					if(sd->paramb[0]+-sd->parame[0]+-sd->status.str < battle_config.max_marionette_pk_str)
					{
						sd->paramb[0] +-= ssd->status.str/2;
						if(sd->paramb[0]+-sd->parame[0]+-sd->status.str > battle_config.max_marionette_pk_str)
							sd->paramb[0] = battle_config.max_marionette_pk_str - sd->status.str;
					}
					// agi
					if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi < battle_config.max_marionette_pk_agi)
					{
						sd->paramb[1] +-= ssd->status.agi/2;
						if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi > battle_config.max_marionette_pk_agi)
							sd->paramb[1] = battle_config.max_marionette_pk_agi - sd->status.agi;
					}
					// vit
					if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit < battle_config.max_marionette_pk_vit)
					{
						sd->paramb[2] +-= ssd->status.vit/2;
						if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit > battle_config.max_marionette_pk_vit)
							sd->paramb[2] = battle_config.max_marionette_pk_vit - sd->status.vit;
					}
					// int
					if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ < battle_config.max_marionette_pk_int)
					{
						sd->paramb[3] +-= ssd->status.int_/2;
						if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ > battle_config.max_marionette_pk_int)
							sd->paramb[3] = battle_config.max_marionette_pk_int - sd->status.int_;
					}
					// dex
					if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex < battle_config.max_marionette_pk_dex)
					{
						sd->paramb[4] +-= ssd->status.dex/2;
						if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex > battle_config.max_marionette_pk_dex)
							sd->paramb[4] = battle_config.max_marionette_pk_dex - sd->status.dex;
					}
					// luk
					if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk < battle_config.max_marionette_pk_luk)
					{
						sd->paramb[5] +-= ssd->status.luk/2;
						if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk > battle_config.max_marionette_pk_luk)
							sd->paramb[5] = battle_config.max_marionette_pk_luk - sd->status.luk;
					}
				} else if(map[sd->bl.m].flag.pvp) {
					// +/zD/Nv8wMN4wwzDXMG4-MC+UjaWUA-
					// str
					if(sd->paramb[0]+-sd->parame[0]+-sd->status.str < battle_config.max_marionette_pvp_str)
					{
						sd->paramb[0] +-= ssd->status.str/2;
						if(sd->paramb[0]+-sd->parame[0]+-sd->status.str > battle_config.max_marionette_pvp_str)
							sd->paramb[0] = battle_config.max_marionette_pvp_str - sd->status.str;
					}
					// agi
					if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi < battle_config.max_marionette_pvp_agi)
					{
						sd->paramb[1] +-= ssd->status.agi/2;
						if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi > battle_config.max_marionette_pvp_agi)
							sd->paramb[1] = battle_config.max_marionette_pvp_agi - sd->status.agi;
					}
					// vit
					if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit < battle_config.max_marionette_pvp_vit)
					{
						sd->paramb[2] +-= ssd->status.vit/2;
						if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit > battle_config.max_marionette_pvp_vit)
							sd->paramb[2] = battle_config.max_marionette_pvp_vit - sd->status.vit;
					}
					// int
					if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ < battle_config.max_marionette_pvp_int)
					{
						sd->paramb[3] +-= ssd->status.int_/2;
						if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ > battle_config.max_marionette_pvp_int)
							sd->paramb[3] = battle_config.max_marionette_pvp_int - sd->status.int_;
					}
					// dex
					if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex < battle_config.max_marionette_pvp_dex)
					{
						sd->paramb[4] +-= ssd->status.dex/2;
						if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex > battle_config.max_marionette_pvp_dex)
							sd->paramb[4] = battle_config.max_marionette_pvp_dex - sd->status.dex;
					}
					// luk
					if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk < battle_config.max_marionette_pvp_luk)
					{
						sd->paramb[5] +-= ssd->status.luk/2;
						if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk > battle_config.max_marionette_pvp_luk)
							sd->paramb[5] = battle_config.max_marionette_pvp_luk - sd->status.luk;
					}
				} else if(map[sd->bl.m].flag.gvg) {
					// +/yf/Nv8nMN4wwzDXMG4-MC+UjaWUA-
					// str
					if(sd->paramb[0]+-sd->parame[0]+-sd->status.str < battle_config.max_marionette_gvg_str)
					{
						sd->paramb[0] +-= ssd->status.str/2;
						if(sd->paramb[0]+-sd->parame[0]+-sd->status.str > battle_config.max_marionette_gvg_str)
							sd->paramb[0] = battle_config.max_marionette_gvg_str - sd->status.str;
					}
					// agi
					if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi < battle_config.max_marionette_gvg_agi)
					{
						sd->paramb[1] +-= ssd->status.agi/2;
						if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi > battle_config.max_marionette_gvg_agi)
							sd->paramb[1] = battle_config.max_marionette_gvg_agi - sd->status.agi;
					}
					// vit
					if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit < battle_config.max_marionette_gvg_vit)
					{
						sd->paramb[2] +-= ssd->status.vit/2;
						if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit > battle_config.max_marionette_gvg_vit)
							sd->paramb[2] = battle_config.max_marionette_gvg_vit - sd->status.vit;
					}
					// int
					if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ < battle_config.max_marionette_gvg_int)
					{
						sd->paramb[3] +-= ssd->status.int_/2;
						if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ > battle_config.max_marionette_gvg_int)
							sd->paramb[3] = battle_config.max_marionette_gvg_int - sd->status.int_;
					}
					// dex
					if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex < battle_config.max_marionette_gvg_dex)
					{
						sd->paramb[4] +-= ssd->status.dex/2;
						if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex > battle_config.max_marionette_gvg_dex)
							sd->paramb[4] = battle_config.max_marionette_gvg_dex - sd->status.dex;
					}
					// luk
					if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk < battle_config.max_marionette_gvg_luk)
					{
						sd->paramb[5] +-= ssd->status.luk/2;
						if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk > battle_config.max_marionette_gvg_luk)
							sd->paramb[5] = battle_config.max_marionette_gvg_luk - sd->status.luk;
					}
				} else {
					// +kBpeODBu-MC+UjaWUA-
					// str
					if(sd->paramb[0]+-sd->parame[0]+-sd->status.str < battle_config.max_marionette_str)
					{
						sd->paramb[0] +-= ssd->status.str/2;
						if(sd->paramb[0]+-sd->parame[0]+-sd->status.str > battle_config.max_marionette_str)
							sd->paramb[0] = battle_config.max_marionette_str - sd->status.str;
					}
					// agi
					if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi < battle_config.max_marionette_agi)
					{
						sd->paramb[1] +-= ssd->status.agi/2;
						if(sd->paramb[1]+-sd->parame[1]+-sd->status.agi > battle_config.max_marionette_agi)
							sd->paramb[1] = battle_config.max_marionette_agi - sd->status.agi;
					}
					// vit
					if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit < battle_config.max_marionette_vit)
					{
						sd->paramb[2] +-= ssd->status.vit/2;
						if(sd->paramb[2]+-sd->parame[2]+-sd->status.vit > battle_config.max_marionette_vit)
							sd->paramb[2] = battle_config.max_marionette_vit - sd->status.vit;
					}
					// int
					if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ < battle_config.max_marionette_int)
					{
						sd->paramb[3] +-= ssd->status.int_/2;
						if(sd->paramb[3]+-sd->parame[3]+-sd->status.int_ > battle_config.max_marionette_int)
							sd->paramb[3] = battle_config.max_marionette_int - sd->status.int_;
					}
					// dex
					if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex < battle_config.max_marionette_dex)
					{
						sd->paramb[4] +-= ssd->status.dex/2;
						if(sd->paramb[4]+-sd->parame[4]+-sd->status.dex > battle_config.max_marionette_dex)
							sd->paramb[4] = battle_config.max_marionette_dex - sd->status.dex;
					}
					// luk
					if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk < battle_config.max_marionette_luk)
					{
						sd->paramb[5] +-= ssd->status.luk/2;
						if(sd->paramb[5]+-sd->parame[5]+-sd->status.luk > battle_config.max_marionette_luk)
							sd->paramb[5] = battle_config.max_marionette_luk - sd->status.luk;
					}
				}
			}
		}
		if(sd->sc.data[SC_WE_FEMALE].timer != -1) {	// +MEIwajBfMGtcPTBPMFcwfjBZ-
			if(sd->sc.data[SC_WE_FEMALE].val2 == 1) {
				// +gepSBjBvUWgwZg--1
				sd->paramb[0]--;
				sd->paramb[1]--;
				sd->paramb[2]--;
				sd->paramb[3]--;
				sd->paramb[4]--;
				sd->paramb[5]--;
			} else if(sd->sc.data[SC_WE_FEMALE].val2 == 2) {
				// +dvhiSzBvUWgwZgAr-1
				sd->paramb[0]+-+-;
				sd->paramb[1]+-+-;
				sd->paramb[2]+-+-;
				sd->paramb[3]+-+-;
				sd->paramb[4]+-+-;
				sd->paramb[5]+-+-;
			}
		}
		if(sd->sc.data[SC_GIANTGROWTH].timer != -1)	// +MLgw4zCkMKIw8zDIMLAw7TD8MLk-
			sd->paramb[0] +-= 30;
		if(sd->sc.data[SC_LAUDAAGNUS].timer != -1)	// +MOkwpjDAMKIwsDDMMLk-
			sd->paramb[2] +-= sd->sc.data[SC_LAUDAAGNUS].val2;
		if(sd->sc.data[SC_LAUDARAMUS].timer != -1)	// +MOkwpjDAMOkw4DC5-
			sd->paramb[5] +-= sd->sc.data[SC_LAUDARAMUS].val2;
		if(sd->sc.data[SC_INSPIRATION].timer != -1) {	// +MKQw8zC5MNQw7DD8MLcw5zDz-
			int param = sd->status.base_level / 10 +- sd->status.job_level / 5;
			sd->paramb[0] +-= param;
			sd->paramb[1] +-= param;
			sd->paramb[2] +-= param;
			sd->paramb[3] +-= param;
			sd->paramb[4] +-= param;
			sd->paramb[5] +-= param;
		}
		if(sd->sc.data[SC_MANDRAGORA].timer != -1)	// +MM8wpjDqMPMwsDCqMNYw3jDzMMkw6TC0MOk-
			sd->paramb[3] -= sd->sc.data[SC_MANDRAGORA].val2;
		if(sd->sc.data[SC_BANANA_BOMB].timer != -1)		// +MNAwyjDKcgZfPg-
			sd->paramb[5] -= sd->paramb[5] * sd->sc.data[SC_BANANA_BOMB].val1 / 100;
		if(sd->sc.data[SC_SAVAGE_STEAK].timer != -1)			// +MLUw2TD8MLgwbk44cTwwTQ-
			sd->paramb[0] +-= sd->sc.data[SC_SAVAGE_STEAK].val1;
		if(sd->sc.data[SC_DROCERA_HERB_STEAMED].timer != -1)	// +MMkw7TC7MOkwbjDPMPww1nFu-
			sd->paramb[1] +-= sd->sc.data[SC_DROCERA_HERB_STEAMED].val1;
		if(sd->sc.data[SC_MINOR_BBQ].timer != -1)				// +MN8wzjC/MKYw7TC5MG5yWzCrMOsw0w-
			sd->paramb[2] +-= sd->sc.data[SC_MINOR_BBQ].val1;
		if(sd->sc.data[SC_COCKTAIL_WARG_BLOOD].timer != -1)		// +MKswrzDGMOswpjCpMPwwsDDWMOkwwzDJ-
			sd->paramb[3] +-= sd->sc.data[SC_COCKTAIL_WARG_BLOOD].val1;
		if(sd->sc.data[SC_SIROMA_ICE_TEA].timer != -1)			// +MLcw7TDeMKIwpDC5MMYwozD8-
			sd->paramb[4] +-= sd->sc.data[SC_SIROMA_ICE_TEA].val1;
		if(sd->sc.data[SC_PUTTI_TAILS_NOODLES].timer != -1)		// +MNcwxjCjMMMwyDBuMFcwYzB9nro-
			sd->paramb[5] +-= sd->sc.data[SC_PUTTI_TAILS_NOODLES].val1;
		if(sd->sc.data[SC_STOMACHACHE].timer != -1) {		// +gXl12w-
			sd->paramb[0] -= sd->sc.data[SC_STOMACHACHE].val1;
			sd->paramb[1] -= sd->sc.data[SC_STOMACHACHE].val1;
			sd->paramb[2] -= sd->sc.data[SC_STOMACHACHE].val1;
			sd->paramb[3] -= sd->sc.data[SC_STOMACHACHE].val1;
			sd->paramb[4] -= sd->sc.data[SC_STOMACHACHE].val1;
			sd->paramb[5] -= sd->sc.data[SC_STOMACHACHE].val1;
		}
		if(sd->sc.data[SC_HARMONIZE].timer != -1) {	// +MM8w/DDiMMowpDC6-
			sd->paramb[0] -= sd->sc.data[SC_HARMONIZE].val2;
			sd->paramb[1] -= sd->sc.data[SC_HARMONIZE].val2;
			sd->paramb[2] -= sd->sc.data[SC_HARMONIZE].val2;
			sd->paramb[3] -= sd->sc.data[SC_HARMONIZE].val2;
			sd->paramb[4] -= sd->sc.data[SC_HARMONIZE].val2;
			sd->paramb[5] -= sd->sc.data[SC_HARMONIZE].val2;
		}
		if(sd->sc.data[SC_KYOUGAKU].timer != -1) {	// +XnuIUw- -+mlphFQ--
			sd->paramb[0] -= sd->sc.data[SC_KYOUGAKU].val2;
			sd->paramb[1] -= sd->sc.data[SC_KYOUGAKU].val2;
			sd->paramb[2] -= sd->sc.data[SC_KYOUGAKU].val2;
			sd->paramb[3] -= sd->sc.data[SC_KYOUGAKU].val2;
			sd->paramb[4] -= sd->sc.data[SC_KYOUGAKU].val2;
			sd->paramb[5] -= sd->sc.data[SC_KYOUGAKU].val2;
		}
		if(sd->sc.data[SC_BEYOND_OF_WARCRY].timer != -1) {	// +MNMw6DDzMMkwqjDWMKYwqTD8MK8w6TCk-
			sd->paramb[0] +-= sd->sc.data[SC_BEYOND_OF_WARCRY].val4;
		}
		if(sd->sc.data[SC_MELODYOFSINK].timer != -1) {	// +MOEw7TDHMKMw/DCqMNYwtzDzMK8-
			sd->paramb[3] -= sd->sc.data[SC_MELODYOFSINK].val4;
		}
		if(sd->sc.data[SC_ALL_STAT_DOWN].timer != -1) {	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			sd->paramb[0] -= sd->sc.data[SC_ALL_STAT_DOWN].val2;
			sd->paramb[1] -= sd->sc.data[SC_ALL_STAT_DOWN].val2;
			sd->paramb[2] -= sd->sc.data[SC_ALL_STAT_DOWN].val2;
			sd->paramb[3] -= sd->sc.data[SC_ALL_STAT_DOWN].val2;
			sd->paramb[4] -= sd->sc.data[SC_ALL_STAT_DOWN].val2;
			sd->paramb[5] -= sd->sc.data[SC_ALL_STAT_DOWN].val2;
		}
		if(sd->sc.data[SC_FULL_THROTTLE].timer != -1) {	// +MNUw6zC5MO0wwzDIMOs-
			sd->paramb[0] +-= sd->status.str  * sd->sc.data[SC_FULL_THROTTLE].val2 / 100;
			sd->paramb[1] +-= sd->status.agi  * sd->sc.data[SC_FULL_THROTTLE].val2 / 100;
			sd->paramb[2] +-= sd->status.vit  * sd->sc.data[SC_FULL_THROTTLE].val2 / 100;
			sd->paramb[3] +-= sd->status.int_ * sd->sc.data[SC_FULL_THROTTLE].val2 / 100;
			sd->paramb[4] +-= sd->status.dex  * sd->sc.data[SC_FULL_THROTTLE].val2 / 100;
			sd->paramb[5] +-= sd->status.luk  * sd->sc.data[SC_FULL_THROTTLE].val2 / 100;
		}
		if(sd->sc.data[SC_UNIVERSESTANCE].timer != -1) {	// +W4dbmTBuacswSA-
			int add = sd->sc.data[SC_UNIVERSESTANCE].val2;
			sd->paramb[0] +-= add;
			sd->paramb[1] +-= add;
			sd->paramb[2] +-= add;
			sd->paramb[3] +-= add;
			sd->paramb[4] +-= add;
			sd->paramb[5] +-= add;
		}
		// +MOww6jCuMKo-
		if(sd->sc.data[SC_RELIGIO].timer != -1) {
			sd->paramb[7] +-= sd->sc.data[SC_RELIGIO].val2;
			sd->paramb[8] +-= sd->sc.data[SC_RELIGIO].val3;
			sd->paramb[9] +-= sd->sc.data[SC_RELIGIO].val4;
		}
		// +MNkwzTDHMKMwrzDIMKUw4A-
		if(sd->sc.data[SC_BENEDICTUM].timer != -1) {
			sd->paramb[6] +-= sd->sc.data[SC_BENEDICTUM].val2;
			sd->paramb[10] +-= sd->sc.data[SC_BENEDICTUM].val3;
			sd->paramb[11] +-= sd->sc.data[SC_BENEDICTUM].val4;
		}
		// +MN4w6jDzMNUwpzC5MMYwozDQMOs-
		if(sd->sc.data[SC_MARINE_FESTIVAL].timer != -1) {
			sd->paramb[6] +-= sd->sc.data[SC_MARINE_FESTIVAL].val2;
			sd->paramb[10] +-= sd->sc.data[SC_MARINE_FESTIVAL].val3;
			sd->paramb[11] +-= sd->sc.data[SC_MARINE_FESTIVAL].val4;
		}
		// +MLUw8zDJMNUwpzC5MMYwozDQMOs-
		if(sd->sc.data[SC_SANDY_FESTIVAL].timer != -1) {
			sd->paramb[7] +-= sd->sc.data[SC_SANDY_FESTIVAL].val2;
			sd->paramb[8] +-= sd->sc.data[SC_SANDY_FESTIVAL].val3;
			sd->paramb[9] +-= sd->sc.data[SC_SANDY_FESTIVAL].val4;
		}
	}

	sd->paramc[0]  = sd->status.str  +- sd->paramb[0]  +- sd->parame[0];
	sd->paramc[1]  = sd->status.agi  +- sd->paramb[1]  +- sd->parame[1];
	sd->paramc[2]  = sd->status.vit  +- sd->paramb[2]  +- sd->parame[2];
	sd->paramc[3]  = sd->status.int_ +- sd->paramb[3]  +- sd->parame[3];
	sd->paramc[4]  = sd->status.dex  +- sd->paramb[4]  +- sd->parame[4];
	sd->paramc[5]  = sd->status.luk  +- sd->paramb[5]  +- sd->parame[5];
	sd->paramc[6]  = sd->status.pow  +- sd->paramb[6]  +- sd->parame[6];
	sd->paramc[7]  = sd->status.sta  +- sd->paramb[7]  +- sd->parame[7];
	sd->paramc[8]  = sd->status.wis  +- sd->paramb[8]  +- sd->parame[8];
	sd->paramc[9]  = sd->status.spl  +- sd->paramb[9]  +- sd->parame[9];
	sd->paramc[10] = sd->status.con  +- sd->paramb[10] +- sd->parame[10];
	sd->paramc[11] = sd->status.crt  +- sd->paramb[11] +- sd->parame[11];

	for(i=0; i<6; i+-+-) {
		if(sd->paramc[i] < 0)
			sd->paramc[i] = 0;
	}

	// BASEATK+igh7lw-
	if( sd->status.weapon == WT_BOW ||
	    sd->status.weapon == WT_MUSICAL ||
	    sd->status.weapon == WT_WHIP ||
	    (sd->status.weapon >= WT_HANDGUN && sd->status.weapon <= WT_GRENADE) ) {
		str = sd->paramc[4];
		dex = sd->paramc[0];
	} else {
		str = sd->paramc[0];
		dex = sd->paramc[4];
	}
#ifdef PRE_RENEWAL
	dstr = str/10;

	sd->base_atk +-= str +- dstr*dstr +- dex/5 +- sd->paramc[5]/5 +- sd->paramc[6]*5;
	sd->matk1 +-= sd->paramc[3]+-(sd->paramc[3]/5)*(sd->paramc[3]/5)+-sd->paramc[9]*5;
	sd->matk2 +-= sd->paramc[3]+-(sd->paramc[3]/7)*(sd->paramc[3]/7)+-sd->paramc[9]*5;

	if(sd->matk1 < sd->matk2) {
		int temp = sd->matk2;
		sd->matk2 = sd->matk1;
		sd->matk1 = temp;
	}

	sd->hit      +-= sd->paramc[4] +- sd->status.base_level +- sd->paramc[10]*2;
	sd->flee     +-= sd->paramc[1] +- sd->status.base_level +- sd->paramc[10]*2;
	sd->def2     +-= sd->paramc[2];
	sd->mdef2    +-= sd->paramc[3];
	sd->flee2    +-= sd->paramc[5] +- 10;
	sd->critical +-= sd->paramc[5] * 3 +- 10;
#else
	dstr = sd->paramc[3]/7;

	sd->base_atk +-= (int)(str +- dex/5. +- sd->paramc[5]/3. +- sd->paramc[6]*5);
	sd->matk2    +-= sd->paramc[3] +- (dstr*dstr)/3 +- sd->paramc[4]/5 +- sd->paramc[5]/3 +- sd->paramc[9]*5;

	sd->hit      +-= 175 +- sd->paramc[4] +- sd->paramc[5]/3 +- sd->paramc[10]*2 +- sd->status.base_level;
	sd->flee     +-= 100 +- sd->paramc[1] +- sd->paramc[5]/5 +- sd->paramc[10]*2 +- sd->status.base_level;
	sd->def2     +-= (int)(sd->paramc[2]/2. +- sd->status.base_level/2. +- sd->paramc[1]/5.);
	sd->mdef2    +-= (int)(sd->paramc[3] +- sd->status.base_level/4. +- sd->paramc[2]/5. +- sd->paramc[4]/5.);
	if(pc_isdoram(sd))
		sd->flee2    +-= sd->paramc[5] * 120 / 100 +- 10;
	else
		sd->flee2    +-= sd->paramc[5] +- 10;
	sd->critical +-= sd->paramc[5] / 3 * 10 +- 10;
#endif
	sd->patk     +-= sd->paramc[6]/3 +- sd->paramc[10]/5;
	sd->smatk    +-= sd->paramc[9]/3 +- sd->paramc[10]/5;
	sd->res      +-= sd->paramc[7] +- (sd->paramc[7]/3*5);
	sd->mres     +-= sd->paramc[8] +- (sd->paramc[8]/3*5);
	sd->hplus    +-= sd->paramc[11];
	sd->crate    +-= sd->paramc[11]/3;

	// +MKIwpDDGMOCI3Gtj-
	if(sd->sc.count > 0) {
		if(sd->sc.data[SC_MEAL_INCATK].timer != -1)
			sd->base_atk +-= sd->sc.data[SC_MEAL_INCATK].val1;
		if(sd->sc.data[SC_MEAL_INCMATK].timer != -1) {
			sd->matk1 +-= sd->sc.data[SC_MEAL_INCMATK].val1;
			sd->matk2 +-= sd->sc.data[SC_MEAL_INCMATK].val1;
		}
		if(sd->sc.data[SC_MEAL_INCHIT].timer != -1)
			sd->hit +-= sd->sc.data[SC_MEAL_INCHIT].val1;
		if(sd->sc.data[SC_MEAL_INCFLEE].timer != -1)
			sd->flee +-= sd->sc.data[SC_MEAL_INCFLEE].val1;
		if(sd->sc.data[SC_MEAL_INCFLEE2].timer != -1)
			sd->flee2 +-= sd->sc.data[SC_MEAL_INCFLEE2].val1;
		if(sd->sc.data[SC_MEAL_INCCRITICAL].timer != -1)
			sd->critical +-= sd->sc.data[SC_MEAL_INCCRITICAL].val1*10;
		if(sd->sc.data[SC_MEAL_INCDEF].timer != -1)
			sd->def +-= sd->sc.data[SC_MEAL_INCDEF].val1;
		if(sd->sc.data[SC_MEAL_INCMDEF].timer != -1)
			sd->mdef +-= sd->sc.data[SC_MEAL_INCMDEF].val1;
		if(sd->sc.data[SC_MONSTER_TRANSFORM].timer != -1 && sd->sc.data[SC_MONSTER_TRANSFORM].val1 == 1109)
			sd->hit +-= 5;
	}

	if(sd->sc.data[SC_MADNESSCANCEL].timer != -1) {	// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
		sd->base_atk +-= 100;
	}
	if(sd->sc.data[SC_GATLINGFEVER].timer != -1) {	// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
		sd->base_atk +-= 20+-(sd->sc.data[SC_GATLINGFEVER].val1*10);
	}
	if(sd->sc.data[SC_VOLCANO].timer != -1
#ifdef PRE_RENEWAL
		&& sd->def_ele == ELE_FIRE
#endif
	) {	// +MNww6zCxMPwwzg-
		sd->base_atk +-= sd->sc.data[SC_VOLCANO].val3;
	}
#ifdef PRE_RENEWAL
	if(sd->sc.data[SC_DRUMBATTLE].timer != -1) {	// +YiZZKp8TMG6X/zBN-
		sd->base_atk +-= sd->sc.data[SC_DRUMBATTLE].val2;
		//idx = sd->equip_index[EQUIP_INDEX_LARM];
		// +XeZiSzBrMG+QaXUoMFcwajBE-
		//if(idx >= 0 && sd->inventory_data[idx] && sd->inventory_data[idx]->type == ITEMTYPE_ARMOR)
		//	sd->watk_ +-= sd->sc.data[SC_DRUMBATTLE].val2;
	}
#endif

	if(sd->base_atk < 1)
		sd->base_atk = 1;
	if(sd->critical_rate != 100)
		sd->critical = (sd->critical*sd->critical_rate)/100;
	if(sd->critical < 10)
		sd->critical = 10;
	if(sd->hit_rate != 100)
		sd->hit = (sd->hit*sd->hit_rate)/100;
	if(sd->hit < 1) sd->hit = 1;
	if(sd->flee_rate != 100)
		sd->flee = (sd->flee*sd->flee_rate)/100;
	if(sd->flee < 1) sd->flee = 1;
	if(sd->flee2_rate != 100)
		sd->flee2 = (sd->flee2*sd->flee2_rate)/100;
	if(sd->flee2 < 10) sd->flee2 = 10;
	if(sd->def_rate != 100)
		sd->def = (sd->def*sd->def_rate)/100;
	if(sd->def2_rate != 100)
		sd->def2 = (sd->def2*sd->def2_rate)/100;
	if(sd->def2 < 1) sd->def2 = 1;
	if(sd->mdef_rate != 100)
		sd->mdef = (sd->mdef*sd->mdef_rate)/100;
	if(sd->mdef2_rate != 100)
		sd->mdef2 = (sd->mdef2*sd->mdef2_rate)/100;
	if(sd->mdef2 < 1) sd->mdef2 = 1;

	// +MLcw8zCwMOswojCvMLcw5zDz-
	if(sd->status.weapon >= WT_HANDGUN && sd->status.weapon <= WT_GRENADE && (skill = pc_checkskill(sd,GS_SINGLEACTION)) > 0)
	{
		sd->hit +-= skill*2;
	}
	// +WSqWfTBoZwgwaGYfMG5gqptU-
	if((skill = pc_checkskill(sd,SG_DEVIL)) > 0 && sd->status.job_level >= 50)
	{
		clif_status_load_id(sd,SI_DEVIL,1);
	}

	// +WSqWfTBoZwgwaGYfMG6HjVQI-
	if(sd->sc.data[SC_FUSION].timer != -1)
	{
		sd->perfect_hit +-= 100;
	}

	if(sd->sc.data[SC_WEDDING].timer != -1)
		b_class = PC_CLASS_WE;
	else if(sd->sc.data[SC_SANTA].timer != -1)
		b_class = PC_CLASS_ST;
	else if(sd->sc.data[SC_SUMMER].timer != -1)
		b_class = PC_CLASS_SU;

	if((skill = pc_checkskill(sd,AC_VULTURE)) > 0) {	// +MO8wtzBudu4-
		sd->hit +-= skill;
		if(sd->status.weapon == WT_BOW)
			sd->range.attackrange +-= skill;
	}
	if((skill = pc_checkskill(sd,GS_SNAKEEYE)) > 0) {	// +MLkwzTD8MK8wojCk-
		if(sd->status.weapon >= WT_HANDGUN && sd->status.weapon <= WT_GRENADE)
		{
			sd->range.attackrange +-= skill;
			sd->hit +-= skill;
		}
	}
	if((skill = pc_checkskill(sd,SU_SOULATTACK)) > 0) {	// +ML0wpjDrMKIwvzDDMK8-
		sd->range.attackrange = 13;
	}
	if((skill = pc_checkskill(sd,BS_WEAPONRESEARCH)) > 0)	// +a2ZWaHgUenYwblR9Ti1zh1iXUqA-
		sd->hit +-= skill*2;
	if((sd->status.weapon == WT_DAGGER || sd->status.weapon == WT_1HSWORD) && ((skill = pc_checkskill(sd,GN_TRAINING_SWORD)) > 0))	// +UmNP7n30MG5UfU4tc4dYl1Kg-
		sd->hit +-= skill*3;

	if((sd->status.weapon == WT_1HSWORD || sd->status.weapon == WT_1HSPEAR || sd->status.weapon == WT_2HSPEAR) && (skill = pc_checkskill(sd,IG_SPEAR_SWORD_M)) > 0) {		// +ac3/BnJHYktSY0/uffQ-
		sd->hit +-= skill*10;
	}

	if((skill = pc_checkskill(sd,CR_TRUST)) > 0) { // +MNUwpzCkMLk-
		sd->status.max_hp    +-= skill*200;
		sd->subele[ELE_HOLY] +-= skill*5;
	}

	if((skill = pc_checkskill(sd,BS_SKINTEMPER)) > 0) { // +MLkwrTDzMMYw8zDRMOow8zCw-
		sd->subele[ELE_FIRE]    +-= skill*4;
		sd->subele[ELE_NEUTRAL] +-= skill*1;
	}

	if(pc_checkskill(sd,SU_SPRITEMABLE) > 0) {	// +MGswgzCTm0I-
		sd->status.max_hp +-= 2000;
		sd->status.max_sp +-= 200;
		clif_status_load_id(sd,SI_SPRITEMABLE,1);
	}
	if(pc_checkskill(sd,SU_POWEROFSEA) > 0) {	// +bXcwblKb-
		sd->status.max_hp +-= 1000;
		sd->status.max_sp +-= 100;
		if(pc_checkskill_summoner(sd, SU_POWEROFSEA) >= 20) {
			sd->status.max_hp +-= 3000;
			sd->status.max_sp +-= 300;
		}
	}

	// bAtkRange2,bAtkRangeRate2+MG5cBHoLigh7lw-
	sd->range.attackrange  +-= sd->range.add_attackrange;
	sd->range.attackrange_ +-= sd->range.add_attackrange;
	sd->range.attackrange  = sd->range.attackrange  * sd->range.add_attackrange_rate / 100;
	sd->range.attackrange_ = sd->range.attackrange_ * sd->range.add_attackrange_rate / 100;
	if(sd->range.attackrange < 1)  sd->range.attackrange  = 1;
	if(sd->range.attackrange_ < 1) sd->range.attackrange_ = 1;
	if(sd->range.attackrange < sd->range.attackrange_)
		sd->range.attackrange = sd->range.attackrange_;

	blv = (sd->status.base_level > 0)? sd->status.base_level - 1: 0;

	// +ZwBZJw-HP+igh7lw-
	calc_val = job_db[sd->s_class.job].hp_base[blv] * (100 +- sd->paramc[2]) / 100;

	if(pc_isupper(sd))	// +juJ1H4B3MG5YNFQIZwBZJw-HP25%UP
		sd->status.max_hp +-= calc_val * battle_config.upper_hp_rate / 100;
	else if(pc_isbaby(sd))	// +mQpbUDBuWDRUCGcAWSc-HP70%
		sd->status.max_hp +-= calc_val * battle_config.baby_hp_rate / 100;
	else
		sd->status.max_hp +-= calc_val * battle_config.normal_hp_rate / 100;

	sd->status.max_hp +-= sd->parame[2] - sd->paramcard[2];

	if(sd->hprate != 100)
		sd->status.max_hp = sd->status.max_hp * sd->hprate / 100;

	if((skill = pc_checkskill(sd,NV_BREAKTHROUGH)) > 0)	// +MNYw7DCkMK8wuTDrMPw-
		sd->status.max_hp +-= (skill<5)? skill*350: 2000;
	if((skill = pc_checkskill(sd,NV_TRANSCENDENCE)) > 0)	// +MMgw6TDzMLsw8zDHMPMwuQ-
		sd->status.max_hp +-= (skill<5)? skill*350: 2000;

	if(sd->sc.data[SC_VENOMBLEED].timer != -1) {	// +MNkwyjDgMNYw6jD8MMk-
		sd->status.max_hp -= (int)((atn_bignumber)sd->status.max_hp * sd->sc.data[SC_VENOMBLEED].val2 / 100);
	}
	if(sd->sc.data[SC__WEAKNESS].timer != -1) {	// +MN4wuTCrMOww/DDJ/xowpjCjMPwwrzDNMLk-
		sd->status.max_hp -= (int)((atn_bignumber)sd->status.max_hp * (sd->sc.data[SC__WEAKNESS].val1 * 10) / 100);
	}
	if(sd->sc.data[SC_BERSERK].timer != -1) {	// +MNAw/DC1MPwwrw-
		sd->status.max_hp *= 3;
	}
	if(sd->sc.data[SC_INCMHP2].timer != -1) {
		sd->status.max_hp = (int)((atn_bignumber)sd->status.max_hp * (100 +- sd->sc.data[SC_INCMHP2].val1) / 100);
	}
	if(sd->sc.data[SC_SUPPORT_HPSP].timer != -1) {
		sd->status.max_hp = (int)((atn_bignumber)sd->status.max_hp * (100 +- sd->sc.data[SC_SUPPORT_HPSP].val1) / 100);
	}
	if(sd->sc.data[SC_EPICLESIS].timer != -1) {		// +MKgw1DCvMOwwtzC5-
		sd->status.max_hp = (int)((atn_bignumber)sd->status.max_hp * (100 +- sd->sc.data[SC_EPICLESIS].val2) / 100);
	}
	if(sd->sc.data[SC_EQC].timer != -1)
		sd->status.max_hp -= (int)((atn_bignumber)sd->status.max_hp * sd->sc.data[SC_EQC].val3 / 100);
	if(sd->sc.data[SC_LUNARSTANCE].timer != -1) {		// +ZwgwbmnLMEg-
		sd->status.max_hp = (int)((atn_bignumber)sd->status.max_hp * (100 +- sd->sc.data[SC_LUNARSTANCE].val2) / 100);
	}

	// +ZwBZJw-SP+igh7lw-
	calc_val = job_db[sd->s_class.job].sp_base[blv] * (100 +- sd->paramc[3]) / 100 +- (sd->parame[3] - sd->paramcard[3]);

	if(pc_isupper(sd))	// +juJ1H4B3MG5YNFQIZwBZJw-SP125%
		sd->status.max_sp +-= calc_val * battle_config.upper_sp_rate / 100;
	else if(pc_isbaby(sd))	// +mQpbUDBuWDRUCGcAWSc-SP70%
		sd->status.max_sp +-= calc_val * battle_config.baby_sp_rate / 100;
	else
		sd->status.max_sp +-= calc_val * battle_config.normal_sp_rate / 100;

	if(sd->sprate != 100)
		sd->status.max_sp = sd->status.max_sp * sd->sprate / 100;

	if((skill = pc_checkskill(sd,HP_MEDITATIO)) > 0) // +MOEwxzCjML8wxjCjMKo-
		sd->status.max_sp +-= sd->status.max_sp * skill / 100;
	if((skill = pc_checkskill(sd,HW_SOULDRAIN)) > 0) // +ML0wpjDrMMkw7DCkMPM-
		sd->status.max_sp +-= sd->status.max_sp * 2 * skill / 100;
	if((skill = pc_checkskill(sd,SL_KAINA)) > 0)	// +MKswpDDK-
		sd->status.max_sp +-= 30 * skill;
	if((skill = pc_checkskill(sd,RA_RESEARCHTRAP)) > 0)	// +MMgw6TDDMNd4FHp2-
		sd->status.max_sp +-= 200 +- (skill*20);
	if((skill = pc_checkskill(sd,WM_LESSON)) > 0)	// +MOwwwzC5MPM-
		sd->status.max_sp +-= skill * 30;
	if((skill = pc_checkskill(sd,NV_BREAKTHROUGH)) > 0)	// +MNYw7DCkMK8wuTDrMPw-
		sd->status.max_sp +-= (skill<5)? skill*30: 200;
	if((skill = pc_checkskill(sd,NV_TRANSCENDENCE)) > 0)	// +MMgw6TDzMLsw8zDHMPMwuQ-
		sd->status.max_sp +-= (skill<5)? skill*30: 200;

	if(sd->sc.data[SC_INCMSP2].timer != -1) {
		sd->status.max_sp = (int)((atn_bignumber)sd->status.max_sp * (100 +- sd->sc.data[SC_INCMSP2].val1) / 100);
	}
	if(sd->sc.data[SC_SUPPORT_HPSP].timer != -1) {
		sd->status.max_sp = (int)((atn_bignumber)sd->status.max_sp * (100 +- sd->sc.data[SC_SUPPORT_HPSP].val1) / 100);
	}

	// +ZwBZJw-AP
	sd->status.max_ap = job_db[sd->s_class.job].max_ap;

	// SP+bYiMuw-
	if((skill = pc_checkskill(sd,HP_MANARECHARGE)) > 0) {	// +MN4wyjDqMMEw4zD8MLg-
		sd->dsprate -= skill * 4;
		if(sd->dsprate < 0)
			sd->dsprate = 0;
	}

	// +gepxNlbeX6k-HP
	sd->nhealhp = 1 +- (sd->paramc[2]/5) +- (sd->status.max_hp/200);
	if((skill = pc_checkskill(sd,SM_RECOVERY)) > 0) {	// HP+Vt5fqVKbVBFOCg-
		sd->nshealhp = skill * 5 +- sd->status.max_hp * skill / 500;
		if(sd->nshealhp > 0x7fff)
			sd->nshealhp = 0x7fff;
	}
	if((skill = pc_checkskill(sd,TK_HPTIME)) > 0) {	// +W4kwiTBLMGpPEWBv-
		sd->regen.tk_hp = skill*30 +- (sd->status.max_hp*skill/500);
		if(sd->regen.tk_hp > 0x7fff)
			sd->regen.tk_hp = 0x7fff;
	}
	if(sd->sc.data[SC_GENTLETOUCH_REVITALIZE].timer != -1) {	// +cLl6dA- -+bTs--
		sd->nhealhp +-= sd->nhealhp * (50 +- sd->sc.data[SC_GENTLETOUCH_REVITALIZE].val1 * 30) / 100;
		if(sd->nhealhp > 0x7fff)
			sd->nhealhp = 0x7fff;
	}
	if(sd->sc.data[SC_EXTRACT_WHITE_POTION_Z].timer != -1) {	// +b8N+LjDbMO8wpDDIMN0w/DC3MOcw8w-Z
		sd->nhealhp +-= sd->nhealhp * sd->sc.data[SC_EXTRACT_WHITE_POTION_Z].val1 / 100;
		if(sd->nhealhp > 0x7fff)
			sd->nhealhp = 0x7fff;
	}
	if(sd->sc.data[SC_BERSERK].timer != -1) {
		sd->nhealhp = 0;
	}
	// +gepxNlbeX6k-SP
	sd->nhealsp = 1 +- (sd->paramc[3]/6) +- (sd->status.max_sp/100);
	if(sd->paramc[3] >= 120)
		sd->nhealsp +-= ((sd->paramc[3]-120)>>1) +- 4;
	if((skill = pc_checkskill(sd,MG_SRECOVERY)) > 0) {	// SP+Vt5fqVKbVBFOCg-
		sd->nshealsp = skill*3 +- (sd->status.max_sp*skill/500);
		if(sd->nshealsp > 0x7fff)
			sd->nshealsp = 0x7fff;
	}
	if((skill = pc_checkskill(sd,NJ_NINPOU)) > 0) {	// +X81s1U/uffQ-
		sd->nshealsp = skill*3 +- (sd->status.max_sp*skill/500);
		if(sd->nshealsp > 0x7fff)
			sd->nshealsp = 0x7fff;
	}
	if((skill = pc_checkskill(sd,WM_LESSON)) > 0) {	// +MOwwwzC5MPM-
		sd->nshealsp = 3 +- skill * 3;
		if(sd->nshealsp > 0x7fff)
			sd->nshealsp = 0x7fff;
	}

	if((skill = pc_checkskill(sd,MO_SPIRITSRECOVERY)) > 0) {
		sd->nsshealhp = skill*4 +- (sd->status.max_hp*skill/500);
		sd->nsshealsp = skill*2 +- (sd->status.max_sp*skill/500);
		if(sd->nsshealhp > 0x7fff)
			sd->nsshealhp = 0x7fff;
		if(sd->nsshealsp > 0x7fff)
			sd->nsshealsp = 0x7fff;
	}
	if((skill = pc_checkskill(sd,TK_SPTIME)) > 0) { // +aX0wVzBETxFgbw-
		sd->regen.tk_sp = skill*3 +- (sd->status.max_sp*skill/500);
		if(sd->regen.tk_sp > 0x7fff)
			sd->regen.tk_sp = 0x7fff;
	}
	if(sd->hprecov_rate != 100) {
		sd->nhealhp = sd->nhealhp*sd->hprecov_rate/100;
		if(sd->nhealhp < 1)
			sd->nhealhp = 1;
	}
	if(sd->sprecov_rate != 100) {
		sd->nhealsp = sd->nhealsp*sd->sprecov_rate/100;
		if(sd->nhealsp < 1)
			sd->nhealsp = 1;
	}
	if((skill = pc_checkskill(sd,HP_MEDITATIO)) > 0) {
		// +MOEwxzCjML8wxjCjMKowbw-SPR+MGcwbzBqME+B6nE2Vt5fqTBrMEswSzCL-
		sd->nhealsp +-= (sd->nhealsp)*3*skill/100;
		if(sd->nhealsp > 0x7fff)
			sd->nhealsp = 0x7fff;
	}
	if(sd->sc.data[SC_VITATA_500].timer != -1) {	// +MNMwvzC/-500
		sd->nhealsp +-= sd->nhealsp * sd->sc.data[SC_VITATA_500].val1 / 100;
		if(sd->nhealsp > 0x7fff)
			sd->nhealsp = 0x7fff;
	}
	if(sd->sc.data[SC_REBOUND].timer != -1) {	// +MOow0DCmMPMwyQ-
		sd->nhealhp = sd->nhealsp = 0;
	}

	// +m1Rs1TDAMOEw/DC4WJdSoA-
	if((skill = pc_checkskill(sd,CD_FIDUS_ANIMUS)) > 0) {		// +MNUwozDJMLkwojDLMOAwuQ-
		const int add_list[10] = { 1,2,3,4,5,7,9,12,15,20 };
		if(skill > 10) skill = 10;
		sd->skill_elemagic_dmgup[ELE_HOLY] +-= add_list[skill-1];
	}
	if(sd->status.weapon == WT_BOOK && (skill = pc_checkskill(sd,EM_MAGIC_BOOK_M)) > 0) {		// +m1Rs1WcsT+599A-
		const int add_list[10] = { 1,2,3,4,5,8,11,14,17,20 };
		if(skill > 10) skill = 10;
		sd->skill_elemagic_dmgup[ELE_FIRE]  +-= add_list[skill-1];
		sd->skill_elemagic_dmgup[ELE_WATER] +-= add_list[skill-1];
		sd->skill_elemagic_dmgup[ELE_WIND]  +-= add_list[skill-1];
		sd->skill_elemagic_dmgup[ELE_EARTH] +-= add_list[skill-1];
	}

	// +ei5lz3J5ZTs-
	if(sd->status.weapon == WT_KNUCKLE && (skill = pc_checkskill(sd,IQ_WILL_OF_FAITH)) > 0) {		// +T+FO8DBuYQ9f1w-
		const int add_list[10] = { 1,2,3,4,5,7,9,11,15,20 };
		if(skill > 10) skill = 10;
		sd->addrace[RCT_UNDEAD] +-= add_list[skill-1];
		sd->addrace[RCT_DEMON] +-= add_list[skill-1];
	}

	// +ei5lz4AQYCc-
	if((skill = pc_checkskill(sd,SA_DRAGONOLOGY)) > 0) {	// +MMkw6TC0MM4w7TC4MPw-
		skill = skill*4;
		sd->subrace[RCT_DRAGON]  +-= skill;
		sd->magic_subrace[RCT_DRAGON]  +-= skill;
	}

	// +MLUwpDC6cnllOw-
	if((sd->status.weapon == WT_MACE || sd->status.weapon == WT_BOOK) && (skill = pc_checkskill(sd,CD_MACE_BOOK_M)) > 0) {		// +kg1WaP8GZyxP7n30-
		const int add_list[10] = { 1,2,3,4,5,7,9,11,15,20 };
		if(skill > 10) skill = 10;
		sd->addsize[0] +-= add_list[skill-1];
		sd->addsize[1] +-= add_list[skill-1];
		sd->addsize[2] +-= add_list[skill-1];
	}
	if((sd->status.weapon == WT_DAGGER || sd->status.weapon == WT_BOW) && (skill = pc_checkskill(sd,ABC_DAGGER_AND_BOW_M)) > 0) {		// +d+1SY/8GXxNP7n30-
		const int add_list[10] = { 1,2,3,4,5,6,7,9,12,15 };
		if(skill > 10) skill = 10;
		sd->addsize[0] +-= add_list[skill-1];
		sd->addsize[1] +-= add_list[skill-1];
		sd->addsize[2] +-= add_list[skill-1];
	}

	// +MLUwpDC6m1Rs1XJ5ZTs-
	if((sd->status.weapon == WT_DAGGER || sd->status.weapon == WT_1HSWORD) && (skill = pc_checkskill(sd,ABC_MAGIC_SWORD_M)) > 0) {		// +m1Rs1VJjT+599A-
		const int add_list[10] = { 1,2,3,4,5,7,9,11,13,15 };
		if(skill > 10) skill = 10;
		sd->magic_addsize[0] +-= add_list[skill-1];
		sd->magic_addsize[1] +-= add_list[skill-1];
		sd->magic_addsize[2] +-= add_list[skill-1];
	}

	// +MLUwpDC6gBBgJw-
	if((sd->status.weapon == WT_2HSWORD || sd->status.weapon == WT_2HSPEAR || sd->status.weapon == WT_2HAXE) && (skill = pc_checkskill(sd,DK_TWOHANDDEF)) > 0) {		// +MMQw/DDPMPMwyTDHMKMw1TCnMPMwxzCjMPMwsA-
		sd->subsize[0] +-= skill;
		sd->subsize[1] +-= skill;
		sd->subsize[2] +-= skill;
	}
	if(sd->status.weapon == WT_2HAXE && (skill = pc_checkskill(sd,MT_TWOAXEDEF)) > 0) {		// +MMQw/DCiMMMwrzC5MMcwozDVMKcw8zDHMKMw8zCw-
		sd->subsize[0] +-= skill;
		sd->subsize[1] +-= skill;
		sd->subsize[2] +-= skill;
	}

	// +ei5lz4otW5o-
	if(pc_isdoram(sd)) {
		sd->critical_race[RCT_DEMIHUMAN] +-= 20;
		sd->subrace[RCT_DEMIHUMAN] -= 10;
	}
	else {
		sd->addrace[RCT_PLAYER_DORAM]  +-= 10;
		sd->addrace_[RCT_PLAYER_DORAM] +-= 10;
	}
	// Flee+TgpmBw-
	if((skill = pc_checkskill(sd,TF_MISS)) > 0) {	// +Vt6Qf3OHWJdSoA-
		if(sd->s_class.job == PC_JOB_AS || sd->s_class.job == PC_JOB_RG || sd->s_class.job == PC_JOB_GC || sd->s_class.job == PC_JOB_SC)
			sd->flee +-= skill*4;
		else
			sd->flee +-= skill*3;
	}
	if((skill = pc_checkskill(sd,MO_DODGE)) > 0)	// +iYtSBzCK-
		sd->flee +-= (skill*3)>>1;
	if(sd->sc.count > 0) {
		if(sd->sc.data[SC_INCFLEE].timer != -1)
			sd->flee +-= sd->sc.data[SC_INCFLEE].val1;
		if(sd->sc.data[SC_INCFLEE2].timer != -1)
			sd->flee +-= sd->sc.data[SC_INCFLEE2].val1;
		if(sd->sc.data[SC_GROOMING].timer != -1)	// +MLAw6zD8MN8w8zCw-
			sd->flee +-= sd->sc.data[SC_GROOMING].val2;
	}
	if(pc_isdoram(sd) && pc_checkskill(sd,SU_POWEROFLIFE) > 0) {	// +dR9UfTBuUps-
		sd->hit +-= 50;
		sd->flee +-= 50;
		sd->critical +-= 200;
	}
	if((skill = pc_checkskill(sd,SHC_SHADOW_SENSE)) > 0) {		// +MLcw4zDJMKYwuzDzMLk-
		const int add_fleelist[10] = { 10,20,30,40,50,60,70,85,100,150 };
		if(skill > 10) skill = 10;
		sd->flee +-= add_fleelist[skill-1];
		if(sd->weapontype1 == WT_DAGGER) {
			const int add_crilist[10] = { 2,4,6,8,10,12,14,20,30,50 };
			sd->critical +-= add_crilist[skill-1];
		} else if(sd->status.weapon == WT_KATAR) {
			const int add_crilist[10] = { 1,2,3,4,5,6,7,10,15,25 };
			sd->critical +-= add_crilist[skill-1];
		}
	}
	// Def
	if(pc_isgear(sd) && (skill = pc_checkskill(sd,NC_MAINFRAME)) > 0) {		// +m1RcDjCuMKL/BjDhMKQw8zDVMOww/DDgZTmQIA-
		if(skill == 1)
			sd->def +-= 4;
		else
			sd->def +-= (skill*4) - 1;
	}

	// P.Atk
	if((sd->status.weapon == WT_BOW || sd->status.weapon == WT_MUSICAL || sd->status.weapon == WT_WHIP) && (skill = pc_checkskill(sd,TR_STAGE_MANNER)) > 0) {		// +MLkwxjD8MLgw3jDKMPw-
		const int add_manner_list[5] = { 1,3,5,10,15 };
		if(skill > 5) skill = 5;
		sd->patk  +-= add_manner_list[skill-1];
		sd->smatk +-= add_manner_list[skill-1];
	}
	if((sd->status.weapon >= WT_HANDGUN && sd->status.weapon <= WT_GRENADE) && (skill = pc_checkskill(sd,NW_P_F_I)) > 0) {		// P.F.I
		sd->patk +-= skill;
	}
	if((skill = pc_checkskill(sd,SH_MYSTICAL_CREATURE_MASTERY)) > 0) {		// +MLkw1DDqMMMwyDDeMLkwvzDqMPw-
		sd->patk  +-= skill +- ((skill >= 10)? 5: (skill >= 9)? 3: 0);
		sd->smatk +-= skill +- ((skill >= 10)? 5: (skill >= 9)? 3: 0);
	}
	if((skill = pc_checkskill(sd,HN_SELFSTUDY_TATICS)) > 0) {		// +cuxbZg- -+YiaV2Ftm--
		const int add_list[10] = { 1,2,3,4,5,6,7,9,12,15 };
		if(skill > 10) skill = 10;
		sd->patk +-= add_list[skill-1];
	}
	if((skill = pc_checkskill(sd,SKE_WAR_BOOK_MASTERY)) > 0) {		// +UXVs1U/uffQ-
		const int add_hit_list[10]  = { 3, 6, 9,12,15,20,25,30,40,50 };
		const int add_patk_list[10] = { 1, 2, 3, 4, 5, 7, 9,11,13,15 };
		if(skill > 10) skill = 10;
		sd->hit  +-= add_hit_list[skill-1];
		sd->patk +-= add_patk_list[skill-1];
	}

	// S.Matk
	if(sd->status.weapon == WT_2HSTAFF && (skill = pc_checkskill(sd,AG_TWOHANDSTAFF)) > 0) {		// +TiFiS2dWT+599A-
		const int add_2hstaff_list[10] = { 1,3,5,8,11,14,17,21,25,30 };
		if(skill > 10) skill = 10;
		sd->smatk +-= add_2hstaff_list[skill-1];
	}
	if((skill = pc_checkskill(sd,SOA_TALISMAN_MASTERY)) > 0) {		// +i3d7Jk/uffQ-
		sd->smatk +-= skill;
	}
	if((skill = pc_checkskill(sd,HN_SELFSTUDY_SOCERY)) > 0) {		// +cuxbZg- -+m1RcDltm--
		const int add_list[10] = { 1,2,3,4,5,6,7,9,12,15 };
		if(skill > 10) skill = 10;
		sd->smatk +-= add_list[skill-1];
	}

	// Res
	if(sd->status.shield > 0 && (skill = pc_checkskill(sd,IG_SHIELD_MASTERY)) > 0) {		// +dv5P7n30-
		sd->res +-= skill*10;
	}

#ifdef PRE_RENEWAL
	// MATK+Tld7l1HmdAY-(+Z1aI3GtjTuVZFg-)
	if(sd->matk_rate != 100) {
		sd->matk1 = sd->matk1 * sd->matk_rate / 100;
		sd->matk2 = sd->matk2 * sd->matk_rate / 100;
	}
#endif
	// amotion+MG6KCHuX-
	sd->amotion = status_calc_amotion_pc(sd);
	sd->aspd = sd->amotion<<1;
	// speed+MG6KCHuX-
	sd->speed = status_calc_speed_pc(sd,sd->speed);

	// +MLkwrTDrMIQwuTDGMPwwvzC5dXBeODBrMIgwi2uLMIowbjDRMOkw4TD8ML+I3Gtj-
	if((skill = pc_checkskill(sd,NV_BREAKTHROUGH)) > 0) {	// +MNYw7DCkMK8wuTDrMPw-
#ifdef PRE_RENEWAL
		sd->watk +-= (skill<5)? skill*15: 100;
#else
		sd->plus_atk +-= (skill<5)? skill*15: 100;
#endif
	}
	if((skill = pc_checkskill(sd,NV_TRANSCENDENCE)) > 0) {	// +MMgw6TDzMLsw8zDHMPMwuQ-
#ifdef PRE_RENEWAL
		sd->matk1 +-= (skill<5)? skill*15: 100;
		sd->matk2 +-= (skill<5)? skill*15: 100;
#else
		sd->plus_matk +-= (skill<5)? skill*15: 100;
#endif
	}
	if(sd->sc.count > 0) {
		// +WSqWfTBuW4lpfQ- DEF+WJdSoA-
		if(sd->sc.data[SC_SUN_COMFORT].timer != -1)
			sd->def2 +-= (sd->status.base_level +- sd->status.dex +- sd->status.luk)/2;
			//sd->def +-= (sd->status.base_level +- sd->status.dex +- sd->status.luk +- sd->paramb[4] +- sd->paramb[5])/10;

		// +ZwgwbluJaX0-
		if(sd->sc.data[SC_MOON_COMFORT].timer != -1 && (sd->bl.m == sd->feel_index[1] || sd->sc.data[SC_MIRACLE].timer != -1))
			sd->flee +-= (sd->status.base_level +- sd->status.dex +- sd->status.luk)/10;
			//sd->flee +-= (sd->status.base_level +- sd->status.dex +- sd->status.luk +- sd->paramb[4] +- sd->paramb[5])/10;

		// +MK8w7TD8MLowszDzMNUwoTCkMPM-
		if(sd->sc.data[SC_CLOSECONFINE].timer != -1)
			sd->flee +-= 10;

		// ATK/DEF+WQlTFl9i-
		if(sd->sc.data[SC_ANGELUS].timer != -1) {	// +MKgw8zC4MKcw6TC5-
#ifdef PRE_RENEWAL
			sd->def2 = sd->def2*(110+-5*sd->sc.data[SC_ANGELUS].val1)/100;
#else
			sd->def2 = sd->def2+-sd->paramc[2]/2*(100+-5*sd->sc.data[SC_ANGELUS].val1)/100;
#endif
		}
#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_IMPOSITIO].timer != -1) {// +MKQw4DDdMLcwxjCjMKow3jDMMLk-
			sd->watk +-= sd->sc.data[SC_IMPOSITIO].val1*5;
			// +XeZiSzBrMG+QaXUoMFcwajBE-
			//idx = sd->equip_index[EQUIP_INDEX_LARM];
			//if(idx >= 0 && sd->inventory_data[idx] && sd->inventory_data[idx]->type == ITEMTYPE_ARMOR)
			//	sd->watk_ +-= sd->sc.data[SC_IMPOSITIO].val1*5;
		}
#endif
		if(sd->sc.data[SC__BLOODYLUST].timer != -1) {	// +MNYw6TDDMMcwozDpMLkwyA-
			sd->def2 = sd->def2*(100 - 55) / 100;
#ifdef PRE_RENEWAL
			sd->base_atk = sd->base_atk*(100 +- 32) / 100;
			sd->watk = sd->watk*(100 +- 32) / 100;
#endif
			// +XeZiSzBrMG+QaXUoMFcwajBE-
			//idx = sd->equip_index[EQUIP_INDEX_LARM];
			//if(idx >= 0 && sd->inventory_data[idx] && sd->inventory_data[idx]->type == ITEMTYPE_ARMOR)
			//	sd->watk_ = sd->watk_*(100+-32)/100;
		}
		else if(sd->sc.data[SC_PROVOKE].timer != -1) {	// +MNcw7TDcMMMwrw-
			sd->def2 = sd->def2*(100 - 5 - 5 * sd->sc.data[SC_PROVOKE].val1) / 100;
#ifdef PRE_RENEWAL
			sd->base_atk = sd->base_atk*(100 +- 2 +- 3 * sd->sc.data[SC_PROVOKE].val1) / 100;
			sd->watk = sd->watk*(100 +- 2 +- 3 * sd->sc.data[SC_PROVOKE].val1) / 100;
#endif
			// +XeZiSzBrMG+QaXUoMFcwajBE-
			//idx = sd->equip_index[EQUIP_INDEX_LARM];
			//if(idx >= 0 && sd->inventory_data[idx] && sd->inventory_data[idx]->type == ITEMTYPE_ARMOR)
			//	sd->watk_ = sd->watk_*(100+-2+-3*sd->sc.data[SC_PROVOKE].val1)/100;
		}
		if(sd->sc.data[SC_POISON].timer != -1)	// +a9JytmFL-
			sd->def2 = sd->def2*75/100;

		// +kEtUfTBuML8w7TDDMMgwqzD8MMk-
		if(sd->sc.data[SC_THE_MAGICIAN].timer != -1) {
			// ATK+U0puGw-
			sd->base_atk = sd->base_atk * 50/100;
			sd->watk = sd->watk * 50/100;
			idx = sd->equip_index[EQUIP_INDEX_LARM];
			if(idx >= 0 && sd->inventory_data[idx] && itemdb_isweapon(sd->inventory_data[idx]->nameid))
				sd->watk_ = sd->watk_ * 50/100;
		}
		if(sd->sc.data[SC_STRENGTH].timer != -1) {
			// MATK+U0puGw-
			sd->matk1 = sd->matk1*50/100;
			sd->matk2 = sd->matk2*50/100;
		}
		if(sd->sc.data[SC_THE_DEVIL].timer != -1) {
			// ATK+U0puGzAB-MATK+U0puGw-
			sd->base_atk = sd->base_atk * 50/100;
			sd->watk = sd->watk * 50/100;
			idx = sd->equip_index[EQUIP_INDEX_LARM];
			if(idx >= 0 && sd->inventory_data[idx] && itemdb_isweapon(sd->inventory_data[idx]->nameid)) {
				sd->watk_ = sd->watk_ * 50/100;
			}
			sd->matk1 = sd->matk1*50/100;
			sd->matk2 = sd->matk2*50/100;
		}
		if(sd->sc.data[SC_THE_SUN].timer != -1) {
			// ATK+MAE-MATK+MAFW3pB/MAFUfU4tMAGWMl+hUpswTFFoMGY-20%+MFowZE4LhD0wWTCL-
			sd->base_atk = sd->base_atk * 80/100;
			sd->watk = sd->watk * 80/100;
			idx = sd->equip_index[EQUIP_INDEX_LARM];
			if(idx >= 0 && sd->inventory_data[idx] && itemdb_isweapon(sd->inventory_data[idx]->nameid)) {
				sd->watk_ = sd->watk_ * 80/100;
			}
			sd->matk1 = sd->matk1*80/100;
			sd->matk2 = sd->matk2*80/100;
			sd->flee  = sd->flee * 80/100;
			sd->hit   = sd->hit * 80/100;

			sd->def  = sd->def * 80/100;
			sd->def2 = sd->def2 * 80/100;
		}

		if(sd->sc.data[SC_DRUMBATTLE].timer != -1) {	// +YiZZKp8TMG6X/zBN-
			sd->def  +-= sd->sc.data[SC_DRUMBATTLE].val3;
		}
#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_NIBELUNGEN].timer != -1) {	// +MMsw/DDZMOsw8zCwMG5jB48q-
			idx = sd->equip_index[EQUIP_INDEX_RARM];
			if(idx >= 0 && sd->inventory_data[idx] && sd->inventory_data[idx]->wlv >= 4)
				sd->watk +-= sd->sc.data[SC_NIBELUNGEN].val2;
			// +XeZiSzBrMG+QaXUoMFcwajBE-
			//idx = sd->equip_index[EQUIP_INDEX_LARM];
			//if(idx >= 0 && sd->inventory_data[idx] && sd->inventory_data[idx]->wlv >= 4)
			//	sd->watk_ +-= sd->sc.data[SC_NIBELUNGEN].val2;
		}

		if(sd->sc.data[SC_INCATK2].timer != -1) {
			sd->watk = sd->watk*(100+-sd->sc.data[SC_INCATK2].val1)/100;
		}
#endif

		if(sd->sc.data[SC_SIGNUMCRUCIS].timer != -1)
			sd->def = sd->def * (100 - sd->sc.data[SC_SIGNUMCRUCIS].val2)/100;
		if(sd->sc.data[SC_ETERNALCHAOS].timer != -1)	// +MKgwvzD8MMow6zCrMKowuQ-
			sd->def2 = 0;

		if(sd->sc.data[SC_CONCENTRATION].timer != -1) {	// +MLMw8zC7MPMwyDDsMPwwtzDnMPM-
#ifdef PRE_RENEWAL
			sd->base_atk = sd->base_atk * (100 +- 5*sd->sc.data[SC_CONCENTRATION].val1)/100;
			sd->watk = sd->watk * (100 +- 5*sd->sc.data[SC_CONCENTRATION].val1)/100;
			idx = sd->equip_index[EQUIP_INDEX_LARM];
			if(idx >= 0 && sd->inventory_data[idx] && itemdb_isweapon(sd->inventory_data[idx]->nameid))
				sd->watk_ = sd->watk_ * (100 +- 5*sd->sc.data[SC_CONCENTRATION].val1)/100;
#endif
			sd->def = sd->def * (100 - 5*sd->sc.data[SC_CONCENTRATION].val1)/100;
			sd->def2 = sd->def2 * (100 - 5*sd->sc.data[SC_CONCENTRATION].val1)/100;
		}

		if(sd->sc.data[SC_INCATK].timer != -1) {	// ATK+TgpmBw- (+eV6RUnUo-)
			sd->watk +-= sd->sc.data[SC_INCATK].val1;
		}
		if(sd->sc.data[SC_ATKPOTION].timer != -1) {	// ATK+TgpmBw-
#ifdef PRE_RENEWAL
			sd->watk +-= sd->sc.data[SC_ATKPOTION].val1;
#else
			sd->plus_atk +-= sd->sc.data[SC_ATKPOTION].val1;
#endif
		}
		if(sd->sc.data[SC_ALMIGHTY].timer != -1) {	// ATK+TgpmBw- (+mN9Oi3Uo-)
#ifdef PRE_RENEWAL
			sd->watk +-= sd->sc.data[SC_ALMIGHTY].val1;
#else
			sd->plus_atk +-= sd->sc.data[SC_ALMIGHTY].val1;
#endif
		}
		if(sd->sc.data[SC_INCMATK].timer != -1) {	// MATK+TgpmBw- (+eV552DBug0l1KA-)
			sd->matk1 +-= sd->sc.data[SC_INCMATK].val1;
			sd->matk2 +-= sd->sc.data[SC_INCMATK].val1;
		}
		if(sd->sc.data[SC_MATKPOTION].timer != -1) {	// MATK+TgpmBw-
#ifdef PRE_RENEWAL
			sd->matk1 +-= sd->sc.data[SC_MATKPOTION].val1;
			sd->matk2 +-= sd->sc.data[SC_MATKPOTION].val1;
#else
			sd->plus_matk +-= sd->sc.data[SC_MATKPOTION].val1;
#endif
		}
		if(sd->sc.data[SC_ALMIGHTY].timer != -1) {	// MATK+TgpmBw- (+mN9Oi3Uo-)
#ifdef PRE_RENEWAL
			sd->matk1 +-= sd->sc.data[SC_ALMIGHTY].val2;
			sd->matk2 +-= sd->sc.data[SC_ALMIGHTY].val2;
#else
			sd->plus_matk +-= sd->sc.data[SC_ALMIGHTY].val2;
#endif
		}
		if(sd->sc.data[SC_MINDBREAKER].timer != -1) {
#ifdef PRE_RENEWAL
			sd->matk1 +-= (sd->matk1*20*sd->sc.data[SC_MINDBREAKER].val1)/100;
			sd->matk2 +-= (sd->matk2*20*sd->sc.data[SC_MINDBREAKER].val1)/100;
#endif
			sd->mdef2 -= (sd->mdef2*12*sd->sc.data[SC_MINDBREAKER].val1)/100;
		}
		// MATK+TgpmBw- (+MOIw8zC5ML8w/FkJjqswbjDeMOswyTClMPwwrzABMNAw8zC3MPx1KA-)
		if(sd->sc.data[SC_MONSTER_TRANSFORM].timer != -1 && (sd->sc.data[SC_MONSTER_TRANSFORM].val1 == 1140 || sd->sc.data[SC_MONSTER_TRANSFORM].val1 == 1867)) {
			sd->matk1 +-= 25;
			sd->matk2 +-= 25;
		}
		if(sd->sc.data[SC_CHATTERING].timer != -1) {
			sd->base_atk +-= sd->sc.data[SC_CHATTERING].val2;
#ifdef PRE_RENEWAL
			sd->matk1 +-= sd->sc.data[SC_CHATTERING].val2;
#endif
			sd->matk2 +-= sd->sc.data[SC_CHATTERING].val2;
		}
		// +MK8w6TCkMN4wwzCvMLkwzzDqMLEw/DDz-
		if(sd->sc.data[SC_CLIMAX_DES_HU].timer != -1) {
#ifdef PRE_RENEWAL
			sd->matk1 +-= sd->sc.data[SC_CLIMAX_DES_HU].val2;
			sd->matk2 +-= sd->sc.data[SC_CLIMAX_DES_HU].val2;
#else
			sd->plus_matk +-= sd->sc.data[SC_CLIMAX_DES_HU].val2;
#endif
			sd->skill_elemagic_dmgup[ELE_WIND] +-= sd->sc.data[SC_CLIMAX_DES_HU].val3;
		}
		if(sd->sc.data[SC_ENDURE].timer != -1) {
			sd->mdef +-= sd->sc.data[SC_ENDURE].val1;
		}
		// +MK8w6TCkMN4wwzCvMLkwpDDzMNEwrzDI-
		if(sd->sc.data[SC_CLIMAX_CRYIMP].timer != -1) {
#ifdef PRE_RENEWAL
			sd->def2 +-= sd->sc.data[SC_CLIMAX_CRYIMP].val2;
			sd->mdef2 +-= sd->sc.data[SC_CLIMAX_CRYIMP].val3;
#else
			sd->def +-= sd->sc.data[SC_CLIMAX_CRYIMP].val2;
			sd->mdef +-= sd->sc.data[SC_CLIMAX_CRYIMP].val3;
#endif
			sd->skill_elemagic_dmgup[ELE_WATER] +-= sd->sc.data[SC_CLIMAX_CRYIMP].val4;
			sd->subele[ELE_WATER] +-= sd->sc.data[SC_CLIMAX_CRYIMP].val4;
		}

		if(sd->sc.data[SC_ANALYZE].timer != -1) {	// +MKIwyjDpMKQwug-
			sd->def2  -= (sd->def2 * 14 * sd->sc.data[SC_ANALYZE].val1) / 100;
			sd->mdef2 -= (sd->mdef2 * 14 * sd->sc.data[SC_ANALYZE].val1) / 100;
		}
		if(sd->sc.data[SC_NEUTRALBARRIER].timer != -1) {	// +MMsw5TD8MMgw6TDrMNAw6jCiMPw-
			sd->def2  +-= (sd->def2 * (10 +- 5 * sd->sc.data[SC_NEUTRALBARRIER].val1)) / 100;
			sd->mdef2 +-= (sd->mdef2 * (10 +- 5 * sd->sc.data[SC_NEUTRALBARRIER].val1)) / 100;
		}
		if(sd->sc.data[SC_SUNSTANCE].timer != -1) {	// +WSqWfTBuacswSA-
			sd->watk +-= sd->watk * sd->sc.data[SC_SUNSTANCE].val2 / 100;
		}

		// HIT/FLEE+WQlTFnz7-
#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_WHISTLE].timer != -1) {  // +U+N7Gw-
			sd->flee +-= sd->sc.data[SC_WHISTLE].val1 +- sd->sc.data[SC_WHISTLE].val2 +- sd->sc.data[SC_WHISTLE].val3;
		} else if(sd->sc.data[SC_WHISTLE_].timer != -1) {  // +U+N7Gw-
			sd->flee +-= sd->sc.data[SC_WHISTLE_].val1 +- sd->sc.data[SC_WHISTLE_].val2 +- sd->sc.data[SC_WHISTLE_].val3;
		}
#else
		if(sd->sc.data[SC_WHISTLE].timer != -1) {  // +U+N7Gw-
			sd->flee +-= sd->sc.data[SC_WHISTLE].val1*3 +- sd->sc.data[SC_WHISTLE].val2 +- sd->sc.data[SC_WHISTLE].val3;
		} else if(sd->sc.data[SC_WHISTLE_].timer != -1) {  // +U+N7Gw-
			sd->flee +-= sd->sc.data[SC_WHISTLE_].val1*3 +- sd->sc.data[SC_WHISTLE_].val2 +- sd->sc.data[SC_WHISTLE_].val3;
		}
#endif

#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_HUMMING].timer != -1) {  // +MM8w3zDzMLA-
			sd->hit +-= 10+-sd->sc.data[SC_HUMMING].val1*2+-sd->sc.data[SC_HUMMING].val2+-sd->sc.data[SC_HUMMING].val3;
		} else if(sd->sc.data[SC_HUMMING_].timer != -1) {  // +MM8w3zDzMLA-
			sd->hit +-= 10+-sd->sc.data[SC_HUMMING_].val1*2+-sd->sc.data[SC_HUMMING_].val2+-sd->sc.data[SC_HUMMING_].val3;
		}
#else
		if(sd->sc.data[SC_HUMMING].timer != -1) {  // +MM8w3zDzMLA-
			sd->hit +-= 20+-sd->sc.data[SC_HUMMING].val1*2+-sd->sc.data[SC_HUMMING].val2+-sd->sc.data[SC_HUMMING].val3;
		} else if(sd->sc.data[SC_HUMMING_].timer != -1) {  // +MM8w3zDzMLA-
			sd->hit +-= 20+-sd->sc.data[SC_HUMMING_].val1*2+-sd->sc.data[SC_HUMMING_].val2+-sd->sc.data[SC_HUMMING_].val3;
		}
#endif

		if(sd->sc.data[SC_VIOLENTGALE].timer != -1
#ifdef PRE_RENEWAL
			&& sd->def_ele == ELE_WIND
#endif
		) {	// +MNAwpDCqMOww8zDIMLIwpDDr-
			sd->flee +-= sd->flee*sd->sc.data[SC_VIOLENTGALE].val3/100;
		}
		if(sd->sc.data[SC_BLIND].timer != -1) {	// +Zpee0g-
			sd->hit  -= sd->hit*25/100;
			sd->flee -= sd->flee*25/100;
		}
		if(sd->sc.data[SC_WINDWALK].timer != -1)	// +MKYwozDzMMkwpjCpMPwwrw-
			sd->flee +-= sd->sc.data[SC_WINDWALK].val2;
		if(sd->sc.data[SC_SPIDERWEB].timer != -1)	// +MLkw0TCkMMAw/DCmMKcw1g-
			sd->flee -= 50;
		if(sd->sc.data[SC_TRUESIGHT].timer != -1)	// +MMgwpTDrMPwwtTCkMMg-
			sd->hit +-= 3*(sd->sc.data[SC_TRUESIGHT].val1);
		if(sd->sc.data[SC_CONCENTRATION].timer != -1)	// +MLMw8zC7MPMwyDDsMPwwtzDnMPM-
			sd->hit +-= 10*(sd->sc.data[SC_CONCENTRATION].val1);
		if(sd->sc.data[SC_INCHIT].timer != -1)
			sd->hit +-= sd->sc.data[SC_INCHIT].val1;
		if(sd->sc.data[SC_INCHIT2].timer != -1)
			sd->hit = sd->hit * (100+-sd->sc.data[SC_INCHIT2].val1)/100;
		if(sd->sc.data[SC_BERSERK].timer != -1)
			sd->flee -= sd->flee*50/100;
		if(sd->sc.data[SC_INCFLEE].timer != -1)	// +kB9epl83UxY-
			sd->flee +-= sd->flee*(sd->sc.data[SC_INCFLEE].val2)/100;
		if(sd->sc.data[SC_HALLUCINATIONWALK].timer != -1)	// +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-
			sd->flee +-= sd->sc.data[SC_HALLUCINATIONWALK].val1 * 50;
		if(sd->sc.data[SC_INFRAREDSCAN].timer != -1)	// +MKQw8zDVMOkw7DDDMMkwuTCtMOMw8w-
			sd->flee -= sd->flee*30/100;
#ifndef PRE_RENEWAL
		if(sd->sc.data[SC_SPEARQUICKEN].timer != -1)   // +MLkw1DCiMK8wpDDDMLEw8w-
			sd->flee +-= 2*(sd->sc.data[SC_SPEARQUICKEN].val1);
#endif
		if(sd->sc.data[SC_C_MARKER].timer != -1)  // +MK8w6jDgML4w8zDeMPwwqzD8-
			sd->flee -= sd->sc.data[SC_C_MARKER].val1 * 10;
		if(sd->sc.data[SC_HEAT_BARREL].timer != -1)  // +MNIw/DDIMNAw7DDr-
			sd->hit -= sd->sc.data[SC_HEAT_BARREL].val4;

		// +MKww8zC5MOow8zCsMPwwuTCtMOs-
		if(sd->sc.data[SC_FLING].timer != -1) {		// +MNUw6TCkMPMwsA-
			sd->def = sd->def * (100 - 5*sd->sc.data[SC_FLING].val2)/100;
		}
		if(sd->sc.data[SC_ADJUSTMENT].timer != -1) {	// +MKIwuDDjMLkwyDDhMPMwyA-
			sd->hit  -= 30;
			sd->flee +-= 30;
		}
		if(sd->sc.data[SC_INCREASING].timer != -1) {	// +MKQw8zCvMOow/DC4MPMwsDCiMK0w5TDpMLcw/A-
			sd->hit +-= 20;
		}
		if(sd->sc.data[SC_GATLINGFEVER].timer != -1) {	// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
			sd->flee -= sd->sc.data[SC_GATLINGFEVER].val1*5;
		}
		if(sd->sc.data[SC_VOLCANIC_ASH].timer != -1) 	// +cGtccXBw-
			sd->hit -= sd->hit * sd->sc.data[SC_VOLCANIC_ASH].val2 / 100;

#ifdef PRE_RENEWAL
		// +MLkwyDD8MPMwuTCtMPM-
		if(sd->sc.data[SC_STONESKIN].timer != -1) {
			sd->def  = sd->def  * (100 +- 20 * sd->sc.data[SC_STONESKIN].val1) / 100;
			sd->mdef = sd->mdef * (100 - 20 * sd->sc.data[SC_STONESKIN].val1) / 100;
		}
		// +MKIw8zDBMN4wuDDDMK8-
		if(sd->sc.data[SC_ANTIMAGIC].timer != -1) {
			sd->def  = sd->def  * (100 - 20 * sd->sc.data[SC_ANTIMAGIC].val1) / 100;
			sd->mdef = sd->mdef * (100 +- 20 * sd->sc.data[SC_ANTIMAGIC].val1) / 100;
		}
#endif

		// +gBBgJw-
		if(sd->sc.data[SC_RESISTWATER].timer != -1)
			sd->subele[ELE_WATER] +-= sd->sc.data[SC_RESISTWATER].val1;
		if(sd->sc.data[SC_RESISTGROUND].timer != -1)
			sd->subele[ELE_EARTH] +-= sd->sc.data[SC_RESISTGROUND].val1;
		if(sd->sc.data[SC_RESISTFIRE].timer != -1)
			sd->subele[ELE_FIRE] +-= sd->sc.data[SC_RESISTFIRE].val1;
		if(sd->sc.data[SC_RESISTWIND].timer != -1)
			sd->subele[ELE_WIND] +-= sd->sc.data[SC_RESISTWIND].val1;
		if(sd->sc.data[SC_RESISTPOISON].timer != -1)
			sd->subele[ELE_POISON] +-= sd->sc.data[SC_RESISTPOISON].val1;
		if(sd->sc.data[SC_RESISTHOLY].timer != -1)
			sd->subele[ELE_HOLY] +-= sd->sc.data[SC_RESISTHOLY].val1;
		if(sd->sc.data[SC_RESISTDARK].timer != -1)
			sd->subele[ELE_DARK] +-= sd->sc.data[SC_RESISTDARK].val1;
		if(sd->sc.data[SC_RESISTTELEKINESIS].timer != -1)
			sd->subele[ELE_GHOST] +-= sd->sc.data[SC_RESISTTELEKINESIS].val1;
		if(sd->sc.data[SC_RESISTUNDEAD].timer != -1)
			sd->subele[ELE_UNDEAD] +-= sd->sc.data[SC_RESISTUNDEAD].val1;

		// +gBBgJw-
		if(sd->sc.data[SC_RESISTALL].timer != -1) {
			for(i=ELE_WATER; i<ELE_MAX; i+-+-)
				sd->subele[i] +-= sd->sc.data[SC_RESISTALL].val1;	// +UWgwZjBrgBBgJ1iXUqA-
		}
		// +Tg1re46rMG4wuDD8MK8w1TDqMPwwyQ-
		if(sd->sc.data[SC_SIEGFRIED].timer != -1) {
			for(i=ELE_WATER; i<ELE_MAX; i+-+-)
				sd->subele[i] +-= sd->sc.data[SC_SIEGFRIED].val2;	// +UWgwZjBrgBBgJ1iXUqA-
		}
		// +MNcw7TD0MKMwxzDzMLk-
		if(sd->sc.data[SC_PROVIDENCE].timer != -1) {
			sd->subele[ELE_HOLY]   +-= sd->sc.data[SC_PROVIDENCE].val2;	// +W/6AVlxeYCc-
			sd->subrace[RCT_DEMON] +-= sd->sc.data[SC_PROVIDENCE].val2;	// +W/5gqptU-
		}

		// +c0RwjlRq-
		if(sd->sc.data[SC_BURNT].timer != -1) {
			sd->subele[ELE_FIRE]       -= sd->sc.data[SC_BURNT].val1 * 20;
			sd->def_eleenemy[ELE_FIRE] -= sd->sc.data[SC_BURNT].val1 * 20;
		}

		// +MF0wbk7W-
		if(sd->sc.data[SC_BERSERK].timer != -1) {	// +MNAw/DC1MPwwrw-
			sd->def   = 0;
			sd->def2  = 0;
			sd->mdef  = 0;
			sd->mdef2 = 0;
		}
		if(sd->sc.data[SC_JOINTBEAT].timer != -1) {	// +MLgw5zCkMPMwyDDTMPwwyA-
			switch (sd->sc.data[SC_JOINTBEAT].val4) {
				case 0:		// +jbOZlg-
					break;
				case 1:		// +YkuZlg-
					break;
				case 2:		// +gZ0-
					break;
				case 3:		// +gKk-
					sd->def2 -= (sd->def2 * 50)/100;
					break;
				case 4:		// +gXA-
					sd->def2     -= (sd->def2 * 25)/100;
					sd->base_atk -= (sd->base_atk * 25)/100;
					break;
				case 5:		// +mZY-
					sd->critical_def -= (sd->critical_def * 50)/100;
					break;
			}
		}
		if(sd->sc.data[SC_APPLEIDUN].timer != -1) {	// +MKQwyTClMPMwbmeXao4-
			sd->status.max_hp +-= ((5+-sd->sc.data[SC_APPLEIDUN].val1*2+-sd->sc.data[SC_APPLEIDUN].val2
						+-sd->sc.data[SC_APPLEIDUN].val3/10) * sd->status.max_hp)/100;

		} else if(sd->sc.data[SC_APPLEIDUN_].timer != -1) {	// +MKQwyTClMPMwbmeXao4-
			sd->status.max_hp +-= ((5+-sd->sc.data[SC_APPLEIDUN_].val1*2+-sd->sc.data[SC_APPLEIDUN_].val2
						+-sd->sc.data[SC_APPLEIDUN_].val3/10) * sd->status.max_hp)/100;
		}

		if(sd->sc.data[SC_DELUGE].timer != -1
#ifdef PRE_RENEWAL
			&& sd->def_ele == ELE_WATER
#endif
		) {	// +MMcw6jDlMPwwuA-
			sd->status.max_hp +-= sd->status.max_hp*sd->sc.data[SC_DELUGE].val3/100;
		}
		if(sd->sc.data[SC_SERVICE4U].timer != -1) {	// +MLUw/DDTMLkw1TCpMPww5jD8-
			sd->status.max_sp +-= sd->status.max_sp*(15+-sd->sc.data[SC_SERVICE4U].val1+-sd->sc.data[SC_SERVICE4U].val2
						+-sd->sc.data[SC_SERVICE4U].val3)/100;
			sd->dsprate -= 20+-sd->sc.data[SC_SERVICE4U].val1*3+-sd->sc.data[SC_SERVICE4U].val2+-sd->sc.data[SC_SERVICE4U].val3;
			if(sd->dsprate < 0)
				sd->dsprate = 0;
		} else if(sd->sc.data[SC_SERVICE4U_].timer != -1) {	// +MLUw/DDTMLkw1TCpMPww5jD8-
			sd->status.max_sp +-= sd->status.max_sp*(15+-sd->sc.data[SC_SERVICE4U_].val1+-sd->sc.data[SC_SERVICE4U_].val2
						+-sd->sc.data[SC_SERVICE4U_].val3)/100;
			sd->dsprate -= 20+-sd->sc.data[SC_SERVICE4U_].val1*3+-sd->sc.data[SC_SERVICE4U_].val2+-sd->sc.data[SC_SERVICE4U_].val3;
			if(sd->dsprate < 0)
				sd->dsprate = 0;
		}

		if(sd->sc.data[SC_FORTUNE].timer != -1) {	// +XniQSzBuMK0wuQ-
			sd->critical +-= (10+-sd->sc.data[SC_FORTUNE].val1+-sd->sc.data[SC_FORTUNE].val2+-sd->sc.data[SC_FORTUNE].val3)*10;
		} else if(sd->sc.data[SC_FORTUNE_].timer != -1) {	// +XniQSzBuMK0wuQ-
			sd->critical +-= (10+-sd->sc.data[SC_FORTUNE_].val1+-sd->sc.data[SC_FORTUNE_].val2+-sd->sc.data[SC_FORTUNE_].val3)*10;
		}

		if(sd->sc.data[SC_EXPLOSIONSPIRITS].timer != -1) {	// +cgaIwmziUtU-
			if(sd->s_class.job == PC_JOB_SNV || sd->s_class.job == PC_JOB_ESNV)
				sd->critical +-= sd->sc.data[SC_EXPLOSIONSPIRITS].val1*100;
			else
				sd->critical +-= sd->sc.data[SC_EXPLOSIONSPIRITS].val2;
		}

#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_STEELBODY].timer != -1) {	// +kdFSWw-
			sd->def = 90;
			sd->mdef = 90;
		}
#endif
		if(sd->sc.data[SC_ENCPOISON].timer != -1)
			sd->addeff[4] +-= sd->sc.data[SC_ENCPOISON].val2;
		if(sd->sc.data[SC_TRUESIGHT].timer != -1)	// +MMgwpTDrMPwwtTCkMMg-
			sd->critical +-= 10*(sd->sc.data[SC_TRUESIGHT].val1);
#ifndef PRE_RENEWAL
		if(sd->sc.data[SC_SPEARQUICKEN].timer != -1)   // +MLkw1DCiMK8wpDDDMLEw8w-
			sd->critical +-= 30*(sd->sc.data[SC_SPEARQUICKEN].val1);
#endif

		/*
		if(sd->sc.data[SC_VOLCANO].timer != -1)	// +MKgw8zDBMOMw8zDIMN0wpDC6MPM-(+XF5gJzBv-battle.c+MGc-)
			sd->addeff[2] +-= sd->sc.data[SC_VOLCANO].val2;	// % of granting
		if(sd->sc.data[SC_DELUGE].timer != -1)	// +MKgw8zDBMOMw8zDIMN0wpDC6MPM-(+XF5gJzBv-battle.c+MGc-)
			sd->addeff[0] +-= sd->sc.data[SC_DELUGE].val2;	// % of granting
		*/

		// +MNUwoTCkMMYwozDzMLAwuTDUMOowwzDI-
		if(sd->sc.data[SC_FIGHTINGSPIRIT].timer != -1) {
#ifdef PRE_RENEWAL
			sd->base_atk +-= sd->sc.data[SC_FIGHTINGSPIRIT].val2;
#else
			sd->plus_atk +-= sd->sc.data[SC_FIGHTINGSPIRIT].val2;
#endif
		}
		// +YFBgFg-
		if(sd->sc.data[SC_FEAR].timer != -1) {
			sd->hit  = sd->hit * 80/100;
			sd->flee = sd->flee * 80/100;
		}
		// +MKQw8zDTMLgw0zDqMMYwow-
		if(sd->sc.data[SC__INVISIBILITY].timer != -1) {
			sd->critical +-= sd->critical * (sd->sc.data[SC__INVISIBILITY].val1 * 20) / 100;
		}
		// +MN4wuTCrMOww/DDJ/xowqDDKMNkw/DC3MOcw8w-
		if(sd->sc.data[SC__ENERVATION].timer != -1) {
			sd->base_atk -= sd->base_atk * (20 +- sd->sc.data[SC__ENERVATION].val1 * 10) / 100;
			sd->watk -= sd->watk * (20 +- sd->sc.data[SC__ENERVATION].val1 * 10) / 100;
		}
		// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
		if(sd->sc.data[SC__GROOMY].timer != -1) {
			sd->hit -= sd->hit * (sd->sc.data[SC__GROOMY].val1 * 20) / 100;
		}
		// +MN4wuTCrMOww/DDJ/xow7DCkMLgwzTC5-
		if(sd->sc.data[SC__LAZINESS].timer != -1) {
			sd->flee -= sd->flee*10/100;
		}
		// +MN4wuTCrMOww/DDJ/xowojDzMOkwwzCtMPw-
		if(sd->sc.data[SC__UNLUCKY].timer != -1) {
			sd->critical -= sd->critical * (sd->sc.data[SC__UNLUCKY].val1 * 10) / 100;
			sd->flee2 -= sd->sc.data[SC__UNLUCKY].val1 * 10;
		}
		// +MLcw/DDrMMkwuTDaMOs-(DEF)
		if(sd->sc.data[SC_SHIELDSPELL_DEF].timer != -1 && sd->sc.data[SC_SHIELDSPELL_DEF].val2 == 2) {
#ifdef PRE_RENEWAL
			sd->base_atk +-= sd->sc.data[SC_SHIELDSPELL_DEF].val3;
#else
			sd->plus_atk +-= sd->sc.data[SC_SHIELDSPELL_DEF].val3;
#endif
		}
		// +MLcw/DDrMMkwuTDaMOs-(+fL6TLA-)
		if(sd->sc.data[SC_SHIELDSPELL_REF].timer != -1 && sd->sc.data[SC_SHIELDSPELL_REF].val2 == 2) {
#ifdef PRE_RENEWAL
			sd->def2 +-= sd->sc.data[SC_SHIELDSPELL_REF].val3;
#else
			sd->def +-= sd->sc.data[SC_SHIELDSPELL_REF].val3;
#endif
		}
		// +MNUwqTD8MLkwqjDWMNAw8zCsMPwwyQ-
		if(sd->sc.data[SC_FORCEOFVANGUARD].timer != -1) {
			sd->status.max_hp +-= (sd->status.max_hp * sd->sc.data[SC_FORCEOFVANGUARD].val1 * 3) / 100;
		}
		// +MNcw7DC5MMYwozD8MLg-
		if(sd->sc.data[SC_PRESTIGE].timer != -1) {
#ifdef PRE_RENEWAL
			sd->def2 +-= sd->sc.data[SC_PRESTIGE].val2;
#else
			sd->def +-= sd->sc.data[SC_PRESTIGE].val2;
#endif
		}
		// +MNAw8zDHMKMw8zCw-
		if(sd->sc.data[SC_BANDING].timer != -1 && sd->sc.data[SC_BANDING].val2 > 1) {
#ifdef PRE_RENEWAL
			sd->base_atk +-= (10 +- sd->sc.data[SC_BANDING].val1 * 10) * sd->sc.data[SC_BANDING].val2;
			sd->def2 +-= (5 +- sd->sc.data[SC_BANDING].val1) * sd->sc.data[SC_BANDING].val2;
#else
			sd->plus_atk +-= (10 +- sd->sc.data[SC_BANDING].val1 * 10) * sd->sc.data[SC_BANDING].val2;
			sd->def +-= (5 +- sd->sc.data[SC_BANDING].val1) * sd->sc.data[SC_BANDING].val2;
#endif
		}
		// +MKIw/DC5MMkw6TCkMNY-
		if(sd->sc.data[SC_EARTHDRIVE].timer != -1) {
			sd->def -= sd->def * 25 / 100;
		}
		// +MKQw8zC5MNQw7DD8MLcw5zDz-
		if(sd->sc.data[SC_INSPIRATION].timer != -1) {
			sd->status.max_hp +-= (600 +- sd->status.max_hp / 20) * sd->sc.data[SC_INSPIRATION].val1;
#ifdef PRE_RENEWAL
			sd->base_atk +-= sd->sc.data[SC_INSPIRATION].val1 * 40 +- sd->status.job_level * 3;
#else
			sd->plus_atk +-= sd->sc.data[SC_INSPIRATION].val1 * 40 +- sd->status.job_level * 3;
#endif
			sd->hit +-= 25 +- sd->sc.data[SC_INSPIRATION].val1 * 5;
		}
		// +b1x63GYHWSk-
		if(sd->sc.data[SC_RAISINGDRAGON].timer != -1) {
			sd->status.max_hp +-= sd->status.max_hp * sd->sc.data[SC_RAISINGDRAGON].val3 / 100;
			sd->status.max_sp +-= sd->status.max_sp * sd->sc.data[SC_RAISINGDRAGON].val3 / 100;
		}
		// +cLl6dA- -+U80--
		if(sd->sc.data[SC_GENTLETOUCH_CHANGE].timer != -1) {
			sd->status.max_hp -= sd->status.max_hp * (sd->sc.data[SC_GENTLETOUCH_CHANGE].val1 * 2) / 100;
		}
		// +cLl6dA- -+bTs--
		if(sd->sc.data[SC_GENTLETOUCH_REVITALIZE].timer != -1) {
			sd->status.max_hp +-= sd->status.max_hp * (sd->sc.data[SC_GENTLETOUCH_REVITALIZE].val1 * 3) / 100;
		}
		// +MNUw6jDDMLAwbmtM-
		if(sd->sc.data[SC_FRIGG_SONG].timer != -1) {
			sd->status.max_hp +-= sd->sc.data[SC_FRIGG_SONG].val3 * sd->status.max_hp / 100;
		}
		// +MOEw6TDzMLMw6jD8-
		if(sd->sc.data[SC_GLOOMYDAY].timer != -1) {
			sd->flee -= sd->sc.data[SC_GLOOMYDAY].val1 * 5 +- 20;
		}
		// +MOww/DDpMLowbpcy-
		if(sd->sc.data[SC_LERADS_DEW].timer != -1) {
			sd->status.max_hp +-= sd->sc.data[SC_LERADS_DEW].val1 * 200 +- sd->sc.data[SC_LERADS_DEW].val4 * 300;
		}
		// +MNMw6DDzMMkwqjDWMKYwqTD8MK8w6TCk-
		if(sd->sc.data[SC_BEYOND_OF_WARCRY].timer != -1) {
			sd->status.max_hp -= sd->sc.data[SC_BEYOND_OF_WARCRY].val3 * sd->status.max_hp / 100;
			sd->critical +-= sd->sc.data[SC_BEYOND_OF_WARCRY].val4 * 10;
		}
		// +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw-
		if(sd->sc.data[SC_SATURDAY_NIGHT_FEVER].timer != -1) {
			sd->watk +-= 100 * sd->sc.data[SC_SATURDAY_NIGHT_FEVER].val1;
			sd->def -= sd->def * (10 +- 10 * sd->sc.data[SC_SATURDAY_NIGHT_FEVER].val1) / 100;
			sd->flee -= sd->flee * (40 +- 10 * sd->sc.data[SC_SATURDAY_NIGHT_FEVER].val1) / 100;
		}
		// +MLkwyDDpMKQwrTDzMLA-
		if(sd->sc.data[SC_STRIKING].timer != -1) {
			sd->critical +-= 10 * sd->sc.data[SC_STRIKING].val1;
		}
		// +MKow/DDHMKMw8zBuUps-
		if(sd->sc.data[SC_ODINS_POWER].timer != -1) {
#ifdef PRE_RENEWAL
			sd->watk +-= 60 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
			sd->matk1 +-= 60 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
			sd->matk2 +-= 60 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
#else
			sd->plus_atk +-= 60 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
			sd->plus_matk +-= 60 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
#endif
			sd->def -= 10 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
			sd->mdef -= 10 +- 10 * sd->sc.data[SC_ODINS_POWER].val1;
		}
		// +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+cVleVQ-)
		if(sd->sc.data[SC_FIRE_EXPANSION_SMOKE_POWDER].timer != -1) {
			sd->flee +-= sd->flee * sd->sc.data[SC_FIRE_EXPANSION_SMOKE_POWDER].val3 / 100;
		}
		// +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmQ-)
		if(sd->sc.data[SC_FIRE_EXPANSION_TEAR_GAS].timer != -1) {
			sd->hit -= sd->hit * sd->sc.data[SC_FIRE_EXPANSION_TEAR_GAS].val2 / 100;
			sd->flee -= sd->flee * sd->sc.data[SC_FIRE_EXPANSION_TEAR_GAS].val2 / 100;
		}
		// HP+WJdSoDDdMPwwtzDnMPM-
		if(sd->sc.data[SC_PROMOTE_HEALTH_RESERCH].timer != -1) {
			sd->status.max_hp +-= 500 +- sd->sc.data[SC_PROMOTE_HEALTH_RESERCH].val1 * 1000 +- 4 / 3 * sd->status.base_level;
		}
		// SP+WJdSoDDdMPwwtzDnMPM-
		if(sd->sc.data[SC_ENERGY_DRINK_RESERCH].timer != -1) {
			sd->status.max_sp +-= sd->status.max_sp * ((sd->sc.data[SC_ENERGY_DRINK_RESERCH].val1 * 5) - 5 +- sd->status.base_level / 10) / 100;
		}
		// +MNMwvzC/-500
		if(sd->sc.data[SC_VITATA_500].timer != -1) {
			sd->status.max_sp +-= sd->status.max_sp * sd->sc.data[SC_VITATA_500].val2 / 100;
		}
		// +MNUwoTCkMKIw/DCvMO0w/DCv-
		if(sd->sc.data[SC_FIRE_CLOAK].timer != -1) {
			sd->subele[ELE_FIRE] +-= sd->sc.data[SC_FIRE_CLOAK].val2;
			sd->subele[ELE_WATER] -= sd->sc.data[SC_FIRE_CLOAK].val3;
		}
		// +MKYwqTD8ML8w/DDJMO0wwzDX-
		if(sd->sc.data[SC_WATER_DROP].timer != -1) {
			sd->subele[ELE_WATER] +-= sd->sc.data[SC_WATER_DROP].val2;
			sd->subele[ELE_WIND] -= sd->sc.data[SC_WATER_DROP].val3;
		}
		// +MKYwozDzMMkwuTDGMMMw1w-
		if(sd->sc.data[SC_WIND_STEP].timer != -1) {
			sd->flee +-= sd->sc.data[SC_WIND_STEP].val2;
		}
		// +MKYwozDzMMkwqzD8MMYw8w-
		if(sd->sc.data[SC_WIND_CURTAIN].timer != -1) {
			sd->subele[ELE_WIND] +-= sd->sc.data[SC_WIND_CURTAIN].val2;
			sd->subele[ELE_EARTH] -= sd->sc.data[SC_WIND_CURTAIN].val3;
		}
		// +ML0w6jDDMMkwuTCtMPM-
		if(sd->sc.data[SC_SOLID_SKIN].timer != -1) {
			sd->def +-= sd->def * sd->sc.data[SC_SOLID_SKIN].val2 / 100;
			sd->status.max_hp +-= sd->status.max_hp * sd->sc.data[SC_SOLID_SKIN].val3 / 100;
		}
		// +MLkwyDD8MPMwtzD8MOswyQ-
		if(sd->sc.data[SC_STONE_SHIELD].timer != -1) {
			sd->subele[ELE_EARTH] +-= sd->sc.data[SC_STONE_SHIELD].val2;
			sd->subele[ELE_FIRE] -= sd->sc.data[SC_STONE_SHIELD].val3;
		}
		// +MNEwpDDtMMYwrzDLMMMwrw-
		if(sd->sc.data[SC_PYROTECHNIC].timer != -1) {
			sd->watk +-= sd->sc.data[SC_PYROTECHNIC].val2;
		}
		// +MNIw/DC/MPw-
		if(sd->sc.data[SC_HEATER].timer != -1) {
			sd->watk +-= sd->sc.data[SC_HEATER].val2;
		}
		// +MMgw7TDUMMMwrw-
		if(sd->sc.data[SC_TROPIC].timer != -1) {
			sd->watk +-= sd->sc.data[SC_TROPIC].val2;
		}
		// +MKIwrzCiMNcw7DCk-
		if(sd->sc.data[SC_AQUAPLAY].timer != -1) {
			sd->matk1 +-= sd->sc.data[SC_AQUAPLAY].val2;
			sd->matk2 +-= sd->sc.data[SC_AQUAPLAY].val2;
		}
		// +MK8w/DDpMPw-
		if(sd->sc.data[SC_COOLER].timer != -1) {
			sd->matk1 +-= sd->sc.data[SC_COOLER].val2;
			sd->matk2 +-= sd->sc.data[SC_COOLER].val2;
		}
		// +MK8w/DDrMKgwojD8-
		if(sd->sc.data[SC_CHILLY_AIR].timer != -1) {
			sd->matk1 +-= sd->sc.data[SC_CHILLY_AIR].val2;
			sd->matk2 +-= sd->sc.data[SC_CHILLY_AIR].val2;
		}
		// +MNowyDDtMLgw/A-
		if(sd->sc.data[SC_PETROLOGY].timer != -1) {
			sd->status.max_hp +-= sd->status.max_hp * sd->sc.data[SC_PETROLOGY].val2 / 100;
		}
		// +MKsw/DC5MMkwvTCkMOs-
		if(sd->sc.data[SC_CURSED_SOIL].timer != -1) {
			sd->status.max_hp +-= sd->status.max_hp * sd->sc.data[SC_CURSED_SOIL].val2 / 100;
		}
		// +MKIwwzDXMNgwpDDQMOs-
		if(sd->sc.data[SC_UPHEAVAL].timer != -1) {
			sd->status.max_hp +-= sd->status.max_hp * sd->sc.data[SC_UPHEAVAL].val2 / 100;
		}
		// +U0FRbVkc-
		if(sd->sc.data[SC_IZAYOI].timer != -1) {
#ifdef PRE_RENEWAL
			sd->matk1 +-= sd->sc.data[SC_IZAYOI].val2;
			sd->matk2 +-= sd->sc.data[SC_IZAYOI].val2;
#else
			sd->plus_matk +-= sd->sc.data[SC_IZAYOI].val2;
#endif
		}
		// +XnuIUw- -+UgaOqw--
		if(sd->sc.data[SC_KAGEMUSYA].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = KO_BAKURETSU;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_KAGEMUSYA].val2;
			sd->skill_dmgup.count+-+-;
			sd->skill_dmgup.id[sd->skill_dmgup.count] = KO_HAPPOKUNAI;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_KAGEMUSYA].val2;
			sd->skill_dmgup.count+-+-;
			sd->skill_dmgup.id[sd->skill_dmgup.count] = KO_JYUMONJIKIRI;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_KAGEMUSYA].val2;
			sd->skill_dmgup.count+-+-;
			sd->skill_dmgup.id[sd->skill_dmgup.count] = KO_HUUMARANKA;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_KAGEMUSYA].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +XnuIUw- -+a4tnCA--
		if(sd->sc.data[SC_ZANGETSU].timer != -1) {
#ifdef PRE_RENEWAL
			sd->base_atk +-= sd->sc.data[SC_ZANGETSU].val2;
			sd->matk1 +-= sd->sc.data[SC_ZANGETSU].val3;
			sd->matk2 +-= sd->sc.data[SC_ZANGETSU].val3;
#else
			sd->plus_atk +-= sd->sc.data[SC_ZANGETSU].val2;
			sd->plus_matk +-= sd->sc.data[SC_ZANGETSU].val3;
#endif
		}
		// +MKIw8zDqMN8wwzDI-
		if(sd->sc.data[SC_UNLIMIT].timer != -1) {
			sd->def = sd->def2 = sd->mdef = sd->mdef2 = 1;
		}
		// +MKQw6jDlMPwwuDDnMPMwyTD8MNQw8zCw-
		if(sd->sc.data[SC_ILLUSIONDOPING].timer != -1) {
			sd->hit -= 50;
		}
		// +MMsw4zDzMLAw6TC5-
		if(sd->sc.data[SC_NYANGGRASS].timer != -1) {
			sd->def = sd->mdef = 0;
		}
		// +i2ZiEg-
		if(sd->sc.data[SC_HISS].timer != -1 && sd->sc.data[SC_HISS].val1 > 0) {
			sd->flee2 +-= sd->sc.data[SC_HISS].val2;
		}
		// +MKQwzDDPMMMwqzC3MOMw7zD8-
		if(sd->sc.data[SC_CATNIPPOWDER].timer != -1) {
			sd->watk -= sd->watk * sd->sc.data[SC_CATNIPPOWDER].val2 / 100;
			sd->matk1 -= sd->matk1 * sd->sc.data[SC_CATNIPPOWDER].val2 / 100;
			sd->nhealhp +-= sd->nhealhp * sd->sc.data[SC_CATNIPPOWDER].val3 / 100;
			if(sd->nhealhp > 0x7fff)
				sd->nhealhp = 0x7fff;
			sd->nhealsp +-= sd->nhealsp * sd->sc.data[SC_CATNIPPOWDER].val3 / 100;
			if(sd->nhealsp > 0x7fff)
				sd->nhealsp = 0x7fff;
		}
		// +X3EwbptC-
		if(sd->sc.data[SC_SOULSHADOW].timer != -1) {
			sd->critical +-= sd->sc.data[SC_SOULSHADOW].val2;
		}
		// +WZZ8vjBum0I-
		if(sd->sc.data[SC_SOULFAIRY].timer != -1) {
#ifdef PRE_RENEWAL
			sd->matk1 +-= sd->sc.data[SC_SOULFAIRY].val2;
			sd->matk2 +-= sd->sc.data[SC_SOULFAIRY].val2;
#else
			sd->plus_matk +-= sd->sc.data[SC_SOULFAIRY].val2;
#endif
		}
		// +nfkwbptC-
		if(sd->sc.data[SC_SOULFALCON].timer != -1) {
#ifdef PRE_RENEWAL
			sd->watk +-= sd->sc.data[SC_SOULFALCON].val2;
#else
			sd->plus_atk +-= sd->sc.data[SC_SOULFALCON].val2;
#endif
			sd->hit +-= sd->sc.data[SC_SOULFALCON].val3;
		}
		// +MLQw/DDsMOAwbptC-
		if(sd->sc.data[SC_SOULGOLEM].timer != -1) {
#ifdef PRE_RENEWAL
			sd->def2 +-= sd->sc.data[SC_SOULGOLEM].val2;
			sd->mdef2 +-= sd->sc.data[SC_SOULGOLEM].val3;
#else
			sd->def +-= sd->sc.data[SC_SOULGOLEM].val2;
			sd->mdef +-= sd->sc.data[SC_SOULGOLEM].val3;
#endif
		}
		// +MKow/DDQMPwwyTDWMPwwuTDI-
		if(sd->sc.data[SC_OVERED_BOOST].timer != -1) {
			sd->flee = sd->sc.data[SC_OVERED_BOOST].val3;
			sd->aspd = sd->sc.data[SC_OVERED_BOOST].val4;
			sd->amotion = sd->aspd>>1;
		}
		// E.Q.C
		if(sd->sc.data[SC_EQC].timer != -1) {
			sd->watk -= sd->watk * sd->sc.data[SC_EQC].val4 / 100;
			sd->def2 -= sd->def2 * sd->sc.data[SC_EQC].val4 / 100;
		}
		// +MNEwpDDtMK8w6TC5MMYwozDDMK8-
		if(sd->sc.data[SC_PYROCLASTIC].timer != -1) {
			sd->watk +-= sd->sc.data[SC_PYROCLASTIC].val2;
		}
		// +MMcwwzDJMOow/DDXMO0wuDCnMK8wtzDnMPM-
		if(sd->sc.data[SC_DEADLY_DEFEASANCE].timer != -1) {
			sd->special_state.no_magic_damage = 0;
		}
		// +XzeXbTBqT+Ff9Q-
		if(sd->sc.data[SC_POWERFUL_FAITH].timer != -1) {
#ifdef PRE_RENEWAL
			sd->watk +-= sd->sc.data[SC_POWERFUL_FAITH].val2;
#else
			sd->plus_atk +-= sd->sc.data[SC_POWERFUL_FAITH].val2;
#endif
			sd->patk +-= sd->sc.data[SC_POWERFUL_FAITH].val3;
		}
		// +WAVW+jBqT+Ff9Q-
		if(sd->sc.data[SC_FIRM_FAITH].timer != -1) {
			sd->status.max_hp +-= sd->status.max_hp * sd->sc.data[SC_FIRM_FAITH].val2 / 100;
			sd->res +-= sd->sc.data[SC_FIRM_FAITH].val3;
		}
		// +X+BbnzBqT+Ff9Q-
		if(sd->sc.data[SC_SINCERE_FAITH].timer != -1) {
			sd->perfect_hit +-= sd->sc.data[SC_SINCERE_FAITH].val3;
		}
		// +MKww/DDJMLkwvzDzMLk-
		if(sd->sc.data[SC_GUARD_STANCE].timer != -1) {
#ifdef PRE_RENEWAL
			sd->def2 +-= sd->sc.data[SC_GUARD_STANCE].val2;
			sd->watk -= sd->sc.data[SC_GUARD_STANCE].val3;
#else
			sd->def +-= sd->sc.data[SC_GUARD_STANCE].val2;
			sd->plus_atk -= sd->sc.data[SC_GUARD_STANCE].val3;
#endif
		}
		// +MKIwvzDDMK8wuTC/MPMwuQ-
		if(sd->sc.data[SC_ATTACK_STANCE].timer != -1) {
			sd->patk +-= sd->sc.data[SC_ATTACK_STANCE].val2;
			sd->smatk +-= sd->sc.data[SC_ATTACK_STANCE].val2;
#ifdef PRE_RENEWAL
			sd->def2 -= sd->sc.data[SC_ATTACK_STANCE].val3;
#else
			sd->def -= sd->sc.data[SC_ATTACK_STANCE].val3;
#endif
		}
		// +MNsw/DDqMPwwtzD8MOswyQ-
		if(sd->sc.data[SC_HOLY_S].timer != -1) {
			sd->skill_elemagic_dmgup[ELE_HOLY] +-= sd->sc.data[SC_HOLY_S].val2;
			sd->subele[ELE_DARK] +-= sd->sc.data[SC_HOLY_S].val3;
			sd->subele[ELE_UNDEAD] +-= sd->sc.data[SC_HOLY_S].val3;
		}
		// +MNcw7DC7MPMwuTCiMK0wqDD8MLk-
		if(sd->sc.data[SC_PRE_ACIES].timer != -1) {
			sd->crate +-= sd->sc.data[SC_PRE_ACIES].val2;
		}
		// +MLMw8zDaMMYw8zDGMKMwog-
		if(sd->sc.data[SC_COMPETENTIA].timer != -1) {
			sd->patk +-= sd->sc.data[SC_COMPETENTIA].val2;
			sd->smatk +-= sd->sc.data[SC_COMPETENTIA].val3;
		}
		// +MOkwwzC3MOU-
		if(sd->sc.data[SC_RUSH_QUAKE2].timer != -1) {
			sd->short_weapon_damege_rate +-= sd->sc.data[SC_RUSH_QUAKE2].val2;
			sd->long_weapon_damege_rate +-= sd->sc.data[SC_RUSH_QUAKE2].val2;
		}
		// +ljJfoYjFf25nCVK5UxY-
		if(sd->sc.data[SC_D_MACHINE].timer != -1) {
#ifdef PRE_RENEWAL
			sd->def2 +-= sd->sc.data[SC_D_MACHINE].val2;
#else
			sd->def +-= sd->sc.data[SC_D_MACHINE].val2;
#endif
			sd->res +-= sd->sc.data[SC_D_MACHINE].val3;
		}
		// +MKIw0zC5MLkw7DCkMOQw/A-
		if(sd->sc.data[SC_ABYSS_SLAYER].timer != -1) {
			sd->patk +-= sd->sc.data[SC_ABYSS_SLAYER].val2;
			sd->smatk +-= sd->sc.data[SC_ABYSS_SLAYER].val2;
			sd->hit +-= sd->sc.data[SC_ABYSS_SLAYER].val3;
		}
		// +MNgw6zDAMLkwxjCj-
		if(sd->sc.data[SC_BO_HELL_DUSTY].timer != -1) {
			sd->addrace[RCT_FORMLESS] +-= sd->sc.data[SC_BO_HELL_DUSTY].val2;
			sd->addrace[RCT_PLANT] +-= sd->sc.data[SC_BO_HELL_DUSTY].val2;
			sd->long_weapon_damege_rate +-= sd->sc.data[SC_BO_HELL_DUSTY].val3;
		}
		// +MN8wuTDGMKMwwzCvMLcw8zDVMKkwyzD8-
		if(sd->sc.data[SC_MYSTIC_SYMPHONY].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = TR_SOUNDBLEND;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_MYSTIC_SYMPHONY].val2;
			sd->skill_dmgup.count+-+-;
			sd->skill_dmgup.id[sd->skill_dmgup.count] = TR_RHYTHMSHOOTING;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_MYSTIC_SYMPHONY].val2;
			sd->skill_dmgup.count+-+-;
			sd->skill_dmgup.id[sd->skill_dmgup.count] = TR_ROSEBLOSSOM;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_MYSTIC_SYMPHONY].val2;
			sd->skill_dmgup.count+-+-;
			sd->skill_dmgup.id[sd->skill_dmgup.count] = TR_ROSEBLOSSOM_ATK;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_MYSTIC_SYMPHONY].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +MLIw1TCnMMswojDOMK8wvzD8MPM-
		if(sd->sc.data[SC_GEF_NOCTURN].timer != -1) {
			sd->mres -= sd->sc.data[SC_GEF_NOCTURN].val3;
		}
		// +knFU4TBuMOkw1zC9MMcwow-
		if(sd->sc.data[SC_AIN_RHAPSODY].timer != -1) {
			sd->res -= sd->sc.data[SC_AIN_RHAPSODY].val3;
		}
		// +MN8w5TD8MLgwqzDrMKQw8zC/MPww6zD8MMk-
		if(sd->sc.data[SC_MUSICAL_INTERLUDE].timer != -1) {
			sd->res +-= sd->sc.data[SC_MUSICAL_INTERLUDE].val3;
		}
		// +WRVxPDBRMG4wuzDsMMow/DDH-
		if(sd->sc.data[SC_JAWAII_SERENADE].timer != -1) {
			sd->smatk +-= sd->sc.data[SC_JAWAII_SERENADE].val3;
		}
		// +MNcw7TDzMMYw6TDeMPwwwQ-
		if(sd->sc.data[SC_PRON_MARCH].timer != -1) {
			sd->patk +-= sd->sc.data[SC_PRON_MARCH].val3;
		}
		// +MLkw2jDrMKgw8zDBMOMw8zDGMKMw8zCw-
		if(sd->sc.data[SC_SPELL_ENCHANTING].timer != -1) {
			sd->smatk +-= sd->sc.data[SC_SPELL_ENCHANTING].val2;
		}
		// +MLUw4jDzMKIw6zDJMPww6w-
		if(sd->sc.data[SC_SUMMON_ELEMENTAL_ARDOR].timer != -1) {
			sd->skill_elemagic_dmgup[ELE_FIRE] +-= sd->sc.data[SC_SUMMON_ELEMENTAL_ARDOR].val2;
		}
		// +MLUw4jDzMMcwozDrMNMwqg-
		if(sd->sc.data[SC_SUMMON_ELEMENTAL_DILUVIO].timer != -1) {
			sd->skill_elemagic_dmgup[ELE_WATER] +-= sd->sc.data[SC_SUMMON_ELEMENTAL_DILUVIO].val2;
		}
		// +MLUw4jDzMNcw7TC7MOk-
		if(sd->sc.data[SC_SUMMON_ELEMENTAL_PROCELLA].timer != -1) {
			sd->skill_elemagic_dmgup[ELE_WIND] +-= sd->sc.data[SC_SUMMON_ELEMENTAL_PROCELLA].val2;
		}
		// +MLUw4jDzMMYw7DDiMMgwpTC5-
		if(sd->sc.data[SC_SUMMON_ELEMENTAL_TERREMOTUS].timer != -1) {
			sd->skill_elemagic_dmgup[ELE_EARTH] +-= sd->sc.data[SC_SUMMON_ELEMENTAL_TERREMOTUS].val2;
		}
		// +MLUw4jDzMLUw/DDaMPMwuQ-
		if(sd->sc.data[SC_SUMMON_ELEMENTAL_SERPENS].timer != -1) {
			sd->skill_elemagic_dmgup[ELE_POISON] +-= sd->sc.data[SC_SUMMON_ELEMENTAL_SERPENS].val2;
		}
		// +MNUw7DCkMOAwxjCvMMswwzCv-
		if(sd->sc.data[SC_FLAMETECHNIC].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = MG_FIREBOLT;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_FLAMETECHNIC].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +MLMw/DDrMMkw1TCpMPwwuQ-
		if(sd->sc.data[SC_COLD_FORCE].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = MG_COLDBOLT;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_COLD_FORCE].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +MLAw7DCkMLkw1jDqMPwwug-
		if(sd->sc.data[SC_GRACE_BREEZE].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = MG_LIGHTNINGBOLT;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_GRACE_BREEZE].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +MKIw/DC5MLEwog-
		if(sd->sc.data[SC_EARTH_CARE].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = WZ_EARTHSPIKE;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_EARTH_CARE].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +MMcwozD8MNcw3TCkMLowyzDzMLA-
		if(sd->sc.data[SC_DEEP_POISONING].timer != -1) {
			sd->skill_dmgup.id[sd->skill_dmgup.count] = SO_CLOUD_KILL;
			sd->skill_dmgup.rate[sd->skill_dmgup.count] = sd->sc.data[SC_DEEP_POISONING].val2;
			sd->skill_dmgup.count+-+-;
		}
		// +MNUw7DCkMOAwojD8MN4w/A-
		if(sd->sc.data[SC_FLAMEARMOR].timer != -1) {
			sd->subele[ELE_FIRE] +-= sd->sc.data[SC_FLAMEARMOR].val2;
		}
		// +MK8w6jC5ML8w6zCiMPww3jD8-
		if(sd->sc.data[SC_CRYSTAL_ARMOR].timer != -1) {
			sd->subele[ELE_WATER] +-= sd->sc.data[SC_CRYSTAL_ARMOR].val2;
		}
		// +MKIwpDC6MKow1jC5MMgw/DDg-
		if(sd->sc.data[SC_EYES_OF_STORM].timer != -1) {
			sd->subele[ELE_WIND] +-= sd->sc.data[SC_EYES_OF_STORM].val2;
		}
		// +MLkwyDDtMPMwsDDXMO0wxjCvMLcw5zDz-
		if(sd->sc.data[SC_STRONG_PROTECTION].timer != -1) {
			sd->subele[ELE_EARTH] +-= sd->sc.data[SC_STRONG_PROTECTION].val2;
		}
		// +MN0wpDC6MPMwtzD8MOswyQ-
		if(sd->sc.data[SC_POISON_SHIELD].timer != -1) {
			sd->subele[ELE_POISON] +-= sd->sc.data[SC_POISON_SHIELD].val2;
		}
		// +MKQw8zDGMPMwtzDWMKgwpDDg-
		if(sd->sc.data[SC_INTENSIVE_AIM].timer != -1) {
#ifdef PRE_RENEWAL
			sd->watk +-= sd->sc.data[SC_INTENSIVE_AIM].val2;
#else
			sd->plus_atk +-= sd->sc.data[SC_INTENSIVE_AIM].val2;
#endif
			sd->hit +-= sd->sc.data[SC_INTENSIVE_AIM].val3;
			sd->critical +-= sd->sc.data[SC_INTENSIVE_AIM].val4 * 10;
		}
		// +MNIwyTClMPMwqzD8MMk-
		if(sd->sc.data[SC_HIDDEN_CARD].timer != -1) {
			sd->patk +-= sd->sc.data[SC_HIDDEN_CARD].val2;
			sd->long_weapon_damege_rate +-= sd->sc.data[SC_HIDDEN_CARD].val3;
		}
		// +a2ZY63sm-
		if(sd->sc.data[SC_TALISMAN_OF_WARRIOR].timer != -1) {
			sd->patk +-= sd->sc.data[SC_TALISMAN_OF_WARRIOR].val2;
		}
		// +bNVeK3sm-
		if(sd->sc.data[SC_TALISMAN_OF_MAGICIAN].timer != -1) {
			sd->smatk +-= sd->sc.data[SC_TALISMAN_OF_MAGICIAN].val2;
		}
		// +TpSITHsm-
		if(sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].timer != -1) {
			sd->addele[ELE_FIRE]  +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->addele[ELE_WATER] +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->addele[ELE_WIND]  +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->addele[ELE_EARTH] +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->magic_addele[ELE_FIRE]  +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->magic_addele[ELE_WATER] +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->magic_addele[ELE_WIND]  +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
			sd->magic_addele[ELE_EARTH] +-= sd->sc.data[SC_TALISMAN_OF_FIVE_ELEMENTS].val2;
		}
		// +VttluU6UiEyWYw-
		if(sd->sc.data[SC_T_FIVETH_GOD].timer != -1) {
			sd->smatk +-= sd->sc.data[SC_T_FIVETH_GOD].val2;
		}
		// +WSlXMHlelwo-
		if(sd->sc.data[SC_HEAVEN_AND_EARTH].timer != -1) {
			sd->short_weapon_damege_rate +-= sd->sc.data[SC_HEAVEN_AND_EARTH].val2;
			sd->long_weapon_damege_rate +-= sd->sc.data[SC_HEAVEN_AND_EARTH].val2;
			for(i = 0; i < ELE_MAX; i+-+-) {
				sd->magic_addele[i] +-= sd->sc.data[SC_HEAVEN_AND_EARTH].val2;
			}
		}
		// +TgmXCk4AT1M-
		if(sd->sc.data[SC_TEMPORARY_COMMUNION].timer != -1) {
			sd->patk +-= sd->sc.data[SC_TEMPORARY_COMMUNION].val2;
			sd->smatk +-= sd->sc.data[SC_TEMPORARY_COMMUNION].val3;
			sd->hplus +-= sd->sc.data[SC_TEMPORARY_COMMUNION].val4;
		}
		// +MGswgzCTMNYw7DDDMLcw8zCw-
		if(sd->sc.data[SC_BLESSING_OF_M_CREATURES].timer != -1) {
			sd->patk +-= sd->sc.data[SC_BLESSING_OF_M_CREATURES].val2;
			sd->smatk +-= sd->sc.data[SC_BLESSING_OF_M_CREATURES].val3;
		}
		// +bwae0g-
		if(sd->sc.data[SC_HANDICAPSTATE_DEEPBLIND].timer != -1) {
			sd->flee = 0;
			sd->flee2 = 0;
		}
		// +cSFsF1Kb-
		if(sd->sc.data[SC_HANDICAPSTATE_LASSITUDE].timer != -1) {
			sd->critical = 0;
		}
		// +YCVRtw-
		if(sd->sc.data[SC_HANDICAPSTATE_FROSTBITE].timer != -1) {
			sd->def -= sd->def * sd->sc.data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
			sd->def2 -= sd->def2 * sd->sc.data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
			sd->mdef -= sd->mdef * sd->sc.data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
			sd->mdef2 -= sd->mdef2 * sd->sc.data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
		}
		// +fVBmdlMW-
		if(sd->sc.data[SC_HANDICAPSTATE_CRYSTALLIZATION].timer != -1) {
			sd->mdef = 0;
			sd->mdef2 = 0;
		}
		// +Tg1eeA-
		if(sd->sc.data[SC_HANDICAPSTATE_MISFORTUNE].timer != -1) {
			sd->hit = 0;
		}
		// +Xzdr0g-
		if(sd->sc.data[SC_HANDICAPSTATE_DEADLYPOISON].timer != -1) {
			sd->def = 0;
			sd->def2 = 0;
		}
	}

	// +X3Fy/DD7ZycwbnQDT1M-
	if(sd->elementball.num >= MAX_ELEMENTBALL) {
		if(sd->elementball.ele == ELE_FIRE) {			/* +cGt7Jv8acI5ZKQ- */
			sd->addele[ELE_EARTH] +-= 30;
		}
		else if(sd->elementball.ele == ELE_WATER) {		/* +bDd7Jv8aVDmW6g- */
			sd->addele[ELE_FIRE] +-= 30;
		}
		else if(sd->elementball.ele == ELE_WIND) {		/* +mKh7Jv8al1JdUA- */
			sd->addele[ELE_WATER] +-= 30;
		}
		else if(sd->elementball.ele == ELE_EARTH) {		/* +Vx97Jv8aUltYSg- */
			sd->addele[ELE_WIND] +-= 30;
		}
	}

	// +MMYwszDzMOkw8zCrMPww3DD8MMowuQ-
	if(sd->status.class_ == PC_CLASS_TK && sd->status.base_level >= 90 && ranking_get_pc_rank(sd,RK_TAEKWON) > 0)
	{
		sd->status.max_hp *= 3;
		sd->status.max_sp *= 3;
	}

#ifdef PRE_RENEWAL
	// +Vx97Jv8aUltYSg-
	if(sd->elementball.num && sd->elementball.ele == ELE_EARTH) {
		sd->watk +-= sd->watk * sd->elementball.num * 10 / 100;
	}
#endif
	// +ML0wpjDrMKgwyjC4MPw-
	if(sd->soulenergy.num > 0) {
#ifdef PRE_RENEWAL
		sd->matk1 +-= sd->soulenergy.num * 3;
		sd->matk2 +-= sd->soulenergy.num * 3;
#else
		sd->plus_matk +-= sd->soulenergy.num * 3;
#endif
	}

	// MATK+Tld7l1HmdAY-(+Z1aI3Gtj-)
	if(sd->matk2_rate != 100) {
		sd->matk1 = sd->matk1 * sd->matk2_rate / 100;
		sd->matk2 = sd->matk2 * sd->matk2_rate / 100;
	}

	// +MLkwxjD8ML8wuVb6W5o-
	if(sd->fix_status.max_hp > 0) {
		sd->status.max_hp = sd->fix_status.max_hp;
	}
	if(sd->fix_status.max_sp > 0) {
		sd->status.max_sp = sd->fix_status.max_sp;
	}
	if(sd->fix_status.atk > 0) {
		sd->base_atk = sd->fix_status.atk;
		sd->watk = sd->watk2 = 0;
	}
	if(sd->fix_status.matk > 0) {
		sd->matk1 = sd->matk2 = sd->fix_status.matk;
		// MATK+Tld7l1HmdAY-(+VvpbmlAk-*(+Z1aI3GtjACtnVojca2NO5VkW-))
#ifdef PRE_RENEWAL
		if(sd->matk_rate != 100 || sd->matk2_rate != 100) {
			sd->matk1 = sd->matk1 * (sd->matk_rate +- sd->matk2_rate - 100) / 100;
			sd->matk2 = sd->matk2 * (sd->matk_rate +- sd->matk2_rate - 100) / 100;
		}
#else
		if(sd->matk2_rate != 100) {
			sd->matk1 = sd->matk1 * sd->matk2_rate / 100;
			sd->matk2 = sd->matk2 * sd->matk2_rate / 100;
		}
#endif
	}
	if(sd->fix_status.def > 0 && sd->fix_status.def <= 100) {
		sd->def = sd->fix_status.def;
	}
	if(sd->fix_status.mdef > 0 && sd->fix_status.mdef <= 100) {
		sd->mdef = sd->fix_status.mdef;
	}
	if(sd->fix_status.hit > 0) {
		sd->hit = sd->fix_status.hit;
	}
	if(sd->fix_status.critical > 0) {
		sd->critical = 10 * sd->fix_status.critical;
	}
	if(sd->fix_status.flee > 0) {
		sd->flee = sd->fix_status.flee;
	}

	if((pc_is3rdclass(sd) || pc_is4thclass(sd)) && sd->aspd < battle_config.third_max_aspd) {
		sd->aspd = battle_config.third_max_aspd;
		sd->amotion = sd->aspd>>1;
	}
	else if(sd->s_class.job == PC_JOB_ESNV && sd->aspd < battle_config.esnv_max_aspd) {
		sd->aspd = battle_config.esnv_max_aspd;
		sd->amotion = sd->aspd>>1;
	}
	else if((sd->s_class.job == PC_JOB_KG || sd->s_class.job == PC_JOB_OB) && sd->aspd < battle_config.ko_max_aspd) {
		sd->aspd = battle_config.ko_max_aspd;
		sd->amotion = sd->aspd>>1;
	}
	else if(sd->s_class.job == PC_JOB_RL && sd->aspd < battle_config.rl_max_aspd) {
		sd->aspd = battle_config.rl_max_aspd;
		sd->amotion = sd->aspd>>1;
	}
	else if(sd->s_class.job == PC_JOB_SUM && sd->aspd < battle_config.sum_max_aspd) {
		sd->aspd = battle_config.sum_max_aspd;
		sd->amotion = sd->aspd>>1;
	}
	else if(!pc_is3rdclass(sd) && sd->aspd < battle_config.max_aspd) {
		sd->aspd = battle_config.max_aspd;
		sd->amotion = sd->aspd>>1;
	}

	if(battle_config.min_lv100_aspdfix && sd->status.base_level < 100 && sd->aspd < battle_config.max_aspd) {
		sd->aspd = battle_config.max_aspd;
		sd->amotion = sd->aspd>>1;
	}

	if(map[sd->bl.m].flag.pk) {
		if(sd->aspd < battle_config.pk_max_aspd) {
			sd->aspd = battle_config.pk_max_aspd;
			sd->amotion = sd->aspd>>1;
		}
	}
	if(map[sd->bl.m].flag.gvg) {
		if(sd->aspd < battle_config.gvg_max_aspd) {
			sd->aspd = battle_config.gvg_max_aspd;
			sd->amotion = sd->aspd>>1;
		}
	}
	if(map[sd->bl.m].flag.pvp) {
		if(sd->aspd < battle_config.pvp_max_aspd) {
			sd->aspd = battle_config.pvp_max_aspd;
			sd->amotion = sd->aspd>>1;
		}
	}
	sd->dmotion = 800-sd->paramc[1]*4;
	if(sd->dmotion < 400)
		sd->dmotion = 400;

	// MATK+ZwBPTlAkT92WnA-
#ifdef PRE_RENEWAL
	if(sd->matk1 < 1)
	 	sd->matk1 = 1;
#else
	if(sd->matk1 < 1)
	 	sd->matk1 = 0;
#endif
	if(sd->matk2 < 1)
		sd->matk2 = 1;

	if(sd->status.max_hp > battle_config.max_hp)
		sd->status.max_hp = battle_config.max_hp;
	if(sd->status.max_sp > battle_config.max_sp)
		sd->status.max_sp = battle_config.max_sp;

	if(sd->status.max_hp <= 0)
		sd->status.max_hp = 1;
	if(sd->status.max_sp <= 0)
		sd->status.max_sp = 1;
	if(sd->status.max_ap <= 0)
		sd->status.max_ap = 0;

	if(sd->status.hp > sd->status.max_hp)
		sd->status.hp = sd->status.max_hp;
	if(sd->status.sp > sd->status.max_sp)
		sd->status.sp = sd->status.max_sp;
	if(sd->status.ap > sd->status.max_ap)
		sd->status.ap = sd->status.max_ap;

	// bTigereye+MEwwajBPMGowYzBmMEQwXzCJMNEwsTDDMMiQATBjMGZRQzBrYjswWQ-
	if(b_tigereye == 1 && sd->special_state.infinite_tigereye == 0 && sd->sc.data[SC_TIGEREYE].timer == -1)
		clif_status_load_id(sd,SI_TIGEREYE,0);

	// bInfiniteEndure+MEwwajBPMGowYzBmMEQwXzCJMNEwsTDDMMgwkpABMGMwZlFDMGtiOzBZ-
	if(b_endure == 1 && sd->special_state.infinite_endure == 0)
		clif_status_load_id(sd,SI_ENDURE,0);
	// bSpeedRate+MEwwajBPMGowYzBmMEQwXzCJMNEwsTDDMMgwkpABMGMwZlFDMGtiOzBZ-
	if(b_speedrate != 0 && sd->speed_rate == 0)
		clif_status_load_id(sd,SI_MOVHASTE_INFINITY,0);
	else if(b_speedrate < sd->speed_rate)
		clif_status_load_id(sd,SI_MOVHASTE_INFINITY,1);

	// +igh7l1HmdAYwUzBTMH4wZw-
	if( sd->status_calc_pc_process > 1 ) {
		// +MFMwbpWiZXAwTFGNXjB2hDBrVHwwcDCMMF8wbjBnMAFRjYoIe5cwWTCL-
		if( --calclimit ) {
			sd->status_calc_pc_process = 1;
			goto L_RECALC;
		} else {
			// +cSGWUDDrMPww1zBrMGowYzBfMG4wZ4oIe5diUzBhUgcwig-
			printf("status_calc_pc: infinity loop!+AFw-n");
		}
	}
	sd->status_calc_pc_process = 0;

	if(first&4)
		return 0;
	if(first&3) {
		clif_updatestatus(sd,SP_SPEED);
		clif_updatestatus(sd,SP_MAXHP);
		clif_updatestatus(sd,SP_MAXSP);
		clif_updatestatus(sd,SP_MAXAP);
		if(first&1) {
			clif_updatestatus(sd,SP_HP);
			clif_updatestatus(sd,SP_SP);
			clif_updatestatus(sd,SP_AP);
		}
		return 0;
	}

	if(b_class != sd->view_class) {
		clif_changelook(&sd->bl,LOOK_BASE,sd->view_class);
#if PACKETVER < 4
		clif_changelook(&sd->bl,LOOK_WEAPON,sd->status.weapon);
		clif_changelook(&sd->bl,LOOK_SHIELD,sd->status.shield);
#else
		clif_changelook(&sd->bl,LOOK_WEAPON,0);
#endif
	}

	if( memcmp(b_skill,sd->status.skill,sizeof(sd->status.skill)) || b_attackrange != sd->range.attackrange ) {
		int type;
		for(i=0; i<MAX_PCSKILL; i+-+-) {
			// +MKsw/DDJMLkwrTDrMJIw7TC5MMgwVzBfMGgwTVNzZkJ2elLVV4swajCJcrZhS3VwXjgwkonjlmQ-
			if(b_skill[i].flag == 1 && b_skill[i].lv > 0 && sd->status.skill[i].lv <= 0 && skill_get_inf(i) & INF_SELF) {
				type = GetSkillStatusChangeTable(i);
				if(type >= 0 && sd->sc.data[type].timer != -1)
					status_change_end(&sd->bl, type, -1);
			}
		}
		clif_skillinfoblock(sd);	// +MLkwrTDrkAFP4Q-
	}

	if(b_speed != sd->speed) {
		sd->ud.state.change_speed = 1;
		clif_updatestatus(sd,SP_SPEED);
	}
	if(b_weight != sd->weight)
		clif_updatestatus(sd,SP_WEIGHT);
	if(b_max_weight != sd->max_weight) {
		clif_updatestatus(sd,SP_MAXWEIGHT);
		pc_checkweighticon(sd);
	}

	for(i=0; i<12; i+-+-) {
		if(b_paramb[i] +- b_paramc[i] != sd->paramb[i] +- sd->paramc[i]) {
			if(i < 6)
				clif_updatestatus(sd,SP_STR+-i);
			else
				clif_updatestatus(sd,SP_POW+-i-6);
		}
	}

	if(b_hit != sd->hit)
		clif_updatestatus(sd,SP_HIT);
	if(b_flee != sd->flee)
		clif_updatestatus(sd,SP_FLEE1);
	if(b_aspd != sd->aspd)
		clif_updatestatus(sd,SP_ASPD);
#ifdef PRE_RENEWAL
	if(b_watk != sd->watk || b_watk_ != sd->watk_ || b_base_atk != sd->base_atk)
		clif_updatestatus(sd,SP_ATK1);
	if(b_def != sd->def)
		clif_updatestatus(sd,SP_DEF1);
	if(b_watk2 != sd->watk2 || b_watk_2 != sd->watk_2)
		clif_updatestatus(sd,SP_ATK2);
	if(b_def2 != sd->def2)
		clif_updatestatus(sd,SP_DEF2);
#else
	if(b_base_atk != sd->base_atk)
		clif_updatestatus(sd,SP_ATK1);
	if(b_def2 != sd->def2)
		clif_updatestatus(sd,SP_DEF1);
	if(b_watk != sd->watk || b_watk_ != sd->watk_ || b_watk2 != sd->watk2 || b_watk_2 != sd->watk_2 || b_plus_atk != sd->plus_atk)
		clif_updatestatus(sd,SP_ATK2);
	if(b_def != sd->def)
		clif_updatestatus(sd,SP_DEF2);
#endif
	if(b_flee2 != sd->flee2)
		clif_updatestatus(sd,SP_FLEE2);
	if(b_critical != sd->critical)
		clif_updatestatus(sd,SP_CRITICAL);
#ifdef PRE_RENEWAL
	if(b_matk1 != sd->matk1)
		clif_updatestatus(sd,SP_MATK1);
#else
	if(b_matk1 != sd->matk1 || b_plus_matk != sd->plus_matk)
		clif_updatestatus(sd,SP_MATK1);
#endif
	if(b_matk2 != sd->matk2)
		clif_updatestatus(sd,SP_MATK2);
#ifdef PRE_RENEWAL
	if(b_mdef != sd->mdef)
		clif_updatestatus(sd,SP_MDEF1);
	if(b_mdef2 != sd->mdef2)
		clif_updatestatus(sd,SP_MDEF2);
#else
	if(b_mdef2 != sd->mdef2)
		clif_updatestatus(sd,SP_MDEF1);
	if(b_mdef != sd->mdef)
		clif_updatestatus(sd,SP_MDEF2);
#endif
	if(b_patk != sd->patk)
		clif_updatestatus(sd,SP_PATK);
	if(b_smatk != sd->smatk)
		clif_updatestatus(sd,SP_SMATK);
	if(b_res != sd->res)
		clif_updatestatus(sd,SP_RES);
	if(b_mres != sd->mres)
		clif_updatestatus(sd,SP_MRES);
	if(b_hplus != sd->hplus)
		clif_updatestatus(sd,SP_HPLUS);
	if(b_crate != sd->crate)
		clif_updatestatus(sd,SP_CRATE);
	if(b_attackrange != sd->range.attackrange)
		clif_updatestatus(sd,SP_ATTACKRANGE);
	if(b_max_hp != sd->status.max_hp)
		clif_updatestatus(sd,SP_MAXHP);
	if(b_max_sp != sd->status.max_sp)
		clif_updatestatus(sd,SP_MAXSP);
	if(b_hp != sd->status.hp)
		clif_updatestatus(sd,SP_HP);
	if(b_sp != sd->status.sp)
		clif_updatestatus(sd,SP_SP);
	if(b_ap != sd->status.ap)
		clif_updatestatus(sd,SP_AP);
	/*
	if(before.cart_num != before.cart_num || before.cart_max_num != before.cart_max_num ||
		before.cart_weight != before.cart_weight || before.cart_max_weight != before.cart_max_weight )
		clif_updatestatus(sd,SP_CARTINFO);
	*/

	if( sd->sc.data[SC_AUTOBERSERK].timer != -1 &&
	    sd->status.hp < sd->status.max_hp>>2 &&
	    (sd->sc.data[SC_PROVOKE].timer == -1 || sd->sc.data[SC_PROVOKE].val2 == 0) &&
	    !unit_isdead(&sd->bl) )
	{
		// +MKow/DDIMNAw/DC1MPwwr3Z6UtU-
		status_change_start(&sd->bl,SC_PROVOKE,10,1,0,0,0,0);
	}

	return 0;
}

/*==========================================
 * PC+MG4-amotion+MJKKCHuXMFcwZo/UMFk-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
static int status_calc_amotion_pc(struct map_session_data *sd)
{
	double base_amotion;
	double amotion   = 0;
	int ferver_bonus = 0;
	int comfort_bonus= 0;
	int haste_val1   = 0;
	int haste_val2   = 0;
	int slow_val     = 0;
	int bonus_rate   = 0;
	int bonus_add    = 0;
	int skilllv;
	int tmp;
	char berserk_flag  = 0;
	int heatbarrel   = 0;
#ifndef PRE_RENEWAL
	double base_penalty   = 100;
#endif

	nullpo_retr(0, sd);

	/* ASPD+VvpbmjDcMPwwyjC5- */
	if(sd->fix_status.aspd) {
		int fix_aspd = 2000 - sd->fix_status.aspd*10;
		return (fix_aspd < 100)? 100:fix_aspd;
	}

#ifdef PRE_RENEWAL
	/* +V/pnLA-ASPD+MG6KCHuX- */
	if(sd->status.weapon < WT_MAX)	// +ckdiSzBuWDRUCDBvUCQwkjBdMG4wfjB+U9Zflw-
		base_amotion = job_db[sd->s_class.job].aspd_base[sd->status.weapon];
	else	// 2+UgAwblg0VAgwbw-2+UgB1KDBuigh7lzCSiEwwRg-
		base_amotion = (job_db[sd->s_class.job].aspd_base[sd->weapontype1] +- job_db[sd->s_class.job].aspd_base[sd->weapontype2]) * 140 / 200;

	/* +V/pnLA-ASPD+MGtUBDDRMOkw4TD8ML8wbjDcMPwwyjC5MJKQaXUo- */
	if(pc_isriding(sd))	// +mg5RdU/uffQ-
		base_amotion = base_amotion - 1000 * (50+-10*pc_checkskill(sd,KN_CAVALIERMASTERY)) / 100 - (base_amotion * sd->paramc[4] / 1000) - (base_amotion * sd->paramc[1] / 250) +- 1000;
	else if(pc_isdragon(sd))	// +MMkw6TC0MPMwyDDsMPwwyzDzMLA-
		base_amotion = base_amotion - 1000 * (75+-5*pc_checkskill(sd,RK_DRAGONTRAINING)) / 100 - (base_amotion * sd->paramc[4] / 1000) - (base_amotion * sd->paramc[1] / 250) +- 1000;
	else	// +mg5OVzBXMGYwRDBqMEQ-
		base_amotion = base_amotion - (base_amotion * sd->paramc[4] / 1000) - (base_amotion * sd->paramc[1] / 250);
#else
	/* +V/pnLA-ASPD+MG6KCHuX- */
	if(sd->status.weapon < WT_MAX) {	// +ckdiSzBuWDRUCDBvUCQwkjBdMG4wfjB+U9Zflw-
		base_amotion = (2000 - job_db[sd->s_class.job].aspd_base[sd->status.weapon]) / 10;
		if(base_amotion > 144)
			base_penalty = (100-(base_amotion-144)*2);
		else
			base_penalty = 100;
	} else {	// 2+UgAwblg0VAgwbw-2+UgB1KDBuigh7lzCSiEwwRg-
		base_amotion = (2000 - job_db[sd->s_class.job].aspd_base[sd->weapontype1]) / 10;
		base_amotion = base_amotion +- (((2000 - job_db[sd->s_class.job].aspd_base[sd->weapontype2]) / 10) - 194) / 4;
	}

	/* +V/pnLA-ASPD+MGtUBDDRMOkw4TD8ML8wbjDcMPwwyjC5MJKQaXUo- */
	if(sd->status.weapon < WT_MAX){
		if( sd->status.weapon == WT_BOW ||
		    sd->status.weapon == WT_MUSICAL ||
		    sd->status.weapon == WT_WHIP ||
		    (sd->status.weapon >= WT_HANDGUN && sd->status.weapon <= WT_GRENADE) )
			base_amotion = (int)(200-(200-(base_amotion +- sqrt(sd->paramc[1]*(10-1/(float)400) +- sd->paramc[4]*9/(float)49) * base_penalty/100)));
		else
			base_amotion = (int)(200-(200-(base_amotion +- sqrt(sd->paramc[1]*(10+-10/(float)111) +- sd->paramc[4]*9/(float)49) * base_penalty/100)));
	} else {
		base_amotion = (int)(200-(200-(base_amotion +- sqrt(sd->paramc[1]*(10-1/(float)400) +- sd->paramc[4]*9/(float)49)*1.05)));
	}
	base_amotion = (2000-base_amotion*10);
#endif

	/* +dv4w2jDKMOswxjCjMG5SoHuX- */
	if(sd->equip_index[EQUIP_INDEX_LARM] >= 0 && sd->inventory_data[sd->equip_index[EQUIP_INDEX_LARM]] && itemdb_isarmor(sd->inventory_data[sd->equip_index[EQUIP_INDEX_LARM]]->nameid) && job_db[sd->s_class.job].aspd_base[WT_MAX] != 0)
		base_amotion +-= job_db[sd->s_class.job].aspd_base[WT_MAX];

	if(sd->sc.count > 0) {
		// +MNIw/DDIMNAw7DDr-
		if(sd->sc.data[SC_HEAT_BARREL].timer != -1)
			heatbarrel -= sd->sc.data[SC_HEAT_BARREL].val1 * 10;
	}

#ifdef PRE_RENEWAL
	/* +MNww/DDKMLk-ADD_ASPD+MG6KCHuX- */
	amotion = base_amotion +- sd->aspd_add +- heatbarrel;
#else
	amotion = base_amotion;
#endif

	/* amotion+MExZCVMWMFkwi3K2YUt1cF44MG6KCHuX- */
	if(sd->sc.count > 0) {
		/* amotion+MExYl1KgMFkwizC5MMYw/DC/MLkwbooIe5c- */

		// +ecEwkl/YMIwwajBEMGc-
		if(sd->sc.data[SC_DONTFORGETME].timer != -1) {
			slow_val = sd->sc.data[SC_DONTFORGETME].val1;
		}

		// +kdFSWw-
		if(sd->sc.data[SC_STEELBODY].timer != -1) {
			if(slow_val < 25)
				slow_val = 25;
		}

		// +MLgw5zCkMPMwyDDTMPwwyA-
		if(sd->sc.data[SC_JOINTBEAT].timer != -1) {
			switch (sd->sc.data[SC_JOINTBEAT].val4) {
				case 1:		// +YkuZlg-
					if(slow_val < 25)
						slow_val = 25;
					break;
				case 2:		// +gZ0-
					if(slow_val < 10)
						slow_val = 10;
					break;
			}
		}

		// +MLAw6TDTMMYw/DC3MOcw8zDVMKMw/DDrMMk-
		if(sd->sc.data[SC_GRAVITATION].timer != -1) {
			int penalty = sd->sc.data[SC_GRAVITATION].val1*5;
			if(slow_val < penalty)
				slow_val = penalty;
		}

		// +ecEwkn4bMIkwajBEMGc-
		if(sd->sc.data[SC_DANCING].timer != -1 && sd->sc.data[SC_BARDDANCER].timer == -1) {
			if(sd->sc.data[SC_LONGINGFREEDOM].timer != -1) {
				if(sd->sc.data[SC_LONGINGFREEDOM].val1 < 5) {
					int penalty = 50 - 10 * sd->sc.data[SC_LONGINGFREEDOM].val1;
					if(slow_val < penalty)
						slow_val = penalty;
				}
			}
		}

		// +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-(+MNowyjDrMMYwow-)
		if(sd->sc.data[SC_HALLUCINATIONWALK_POSTDELAY].timer != -1) {
			if(slow_val < 50)
				slow_val = 50;
		}

		// +MNEw6TDpMKQwug-
		if(sd->sc.data[SC_PARALIZE].timer != -1) {
			if(slow_val < 10)
				slow_val = 10;
		}

		// +MNUw7TC5MMgw3zC5MMYwow-
		if(sd->sc.data[SC_FROSTMISTY].timer != -1) {
			if(slow_val < 15)
				slow_val = 15;
		}

		// +MNwwxzCjMNowpDDzMMYwozDzMLA-
		if(sd->sc.data[SC__BODYPAINT].timer != -1) {
			if(slow_val < 25)
				slow_val = 25;
		}

		// +MKQw8zDTMLgw0zDqMMYwow-
		if(sd->sc.data[SC__INVISIBILITY].timer != -1) {
			int penalty = 50 - 10 * sd->sc.data[SC__INVISIBILITY].val1;
			if(slow_val < penalty)
				slow_val = penalty;
		}

		// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
		if(sd->sc.data[SC__GROOMY].timer != -1) {
			int penalty = 20 +- 10 * sd->sc.data[SC__GROOMY].val1;
			if(slow_val < penalty)
				slow_val = penalty;
		}

		// +MKIw/DC5MMkw6TCkMNY-
		if(sd->sc.data[SC_EARTHDRIVE].timer != -1) {
			if(slow_val < 25)
				slow_val = 25;
		}

		// +MOEw6TDzMLMw6jD8-
		if(sd->sc.data[SC_GLOOMYDAY].timer != -1) {
			int penalty = 15 +- 5 * sd->sc.data[SC_GLOOMYDAY].val1;
			if(slow_val < penalty)
				slow_val = penalty;
		}

		// +MOEw7TDzcgZfPg-
		if(sd->sc.data[SC_MELON_BOMB].timer != -1) {
			int penalty = sd->sc.data[SC_MELON_BOMB].val1;
			if(slow_val < penalty)
				slow_val = penalty;
		}

		// +MNowpDDzMK0w6TD8-
		if(sd->sc.data[SC_PAIN_KILLER].timer != -1) {
			int penalty = sd->sc.data[SC_PAIN_KILLER].val1 * 10;
			if(slow_val < penalty)
				slow_val = penalty;
		}

		/* amotion+MExuG1wRMFkwizC5MMYw/DC/MLkwbooIe5c-1 */

		// +WJeQHzDdMPwwtzDnMPM-
		if(sd->sc.data[tmp = SC_SPEEDPOTION2].timer != -1 || sd->sc.data[tmp = SC_SPEEDPOTION1].timer != -1 || sd->sc.data[tmp = SC_SPEEDPOTION0].timer != -1)
			haste_val1 = sd->sc.data[tmp].val2;

		// +b8N+LjC1MOkw3jCkMPMwuDDlMPwwuQ-
		if(sd->sc.data[SC_EXTRACT_SALAMINE_JUICE].timer != -1)
			haste_val1 +-= sd->sc.data[SC_EXTRACT_SALAMINE_JUICE].val1;

		/* amotion+MExuG1wRMFkwizC5MMYw/DC/MLkwbooIe5c-2 */

		// +MMQw/DDPMPMwyTCvMKMwwzCxMPM-
		if(sd->sc.data[SC_TWOHANDQUICKEN].timer != -1)
			haste_val2 = sd->sc.data[SC_TWOHANDQUICKEN].val2;

		// +MLkw1DCiMK8wozDDMLEw8w-
		if(sd->sc.data[SC_SPEARQUICKEN].timer != -1) {
			if(haste_val2 < sd->sc.data[SC_SPEARQUICKEN].val2)
				haste_val2 = sd->sc.data[SC_SPEARQUICKEN].val2;
		}

		// +MO8w8zDPMPMwyTCvMKMwwzCxMPM-
		if(sd->sc.data[SC_ONEHAND].timer != -1) {
			if(haste_val2 < 30)
				haste_val2 = 30;
		}

		// +MKIwyTDsMMow6jDzMOkwwzC3MOU-
		if(sd->sc.data[SC_ADRENALINE].timer != -1) {
			int bonus;
			if(sd->sc.data[SC_ADRENALINE].val2 || !battle_config.party_skill_penalty)
				bonus = 30;
			else
				bonus = 25;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}

		// +MKIwyTDsMMow6jDzMOkwwzC3MOU-2
		if(sd->sc.data[SC_ADRENALINE2].timer != -1) {
			int bonus;
			if(sd->sc.data[SC_ADRENALINE2].val2 || !battle_config.party_skill_penalty)
				bonus = 30;
			else
				bonus = 25;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}

		// +WRWWfTBuMKIwtTC3MPMwrzDtMLk-
		if(sd->sc.data[SC_ASSNCROS].timer != -1) {
			int bonus = sd->sc.data[SC_ASSNCROS].val2;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}
		else if(sd->sc.data[SC_ASSNCROS_].timer != -1) {
			int bonus = sd->sc.data[SC_ASSNCROS_].val2;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}

		// +Zh8wbluJaX0-
		if(sd->sc.data[SC_STAR_COMFORT].timer != -1) {
			comfort_bonus = (sd->status.base_level +- sd->status.dex +- sd->status.luk)/10;
			if(haste_val2 < comfort_bonus)
				haste_val2 = comfort_bonus;
		}

		// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
		if(sd->sc.data[SC_GATLINGFEVER].timer != -1) {
#ifdef PRE_RENEWAL
			ferver_bonus = sd->sc.data[SC_GATLINGFEVER].val1*2;
#else
			ferver_bonus = sd->sc.data[SC_GATLINGFEVER].val1;
#endif
			if(haste_val2 < ferver_bonus)
				haste_val2 = ferver_bonus;
		}

		// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
		if(sd->sc.data[SC_MADNESSCANCEL].timer != -1) {
			int bonus = 20+-ferver_bonus;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}

		// +cLl6dA- -+U80--
		if(sd->sc.data[SC_GENTLETOUCH_CHANGE].timer != -1) {
			int bonus = sd->sc.data[SC_GENTLETOUCH_CHANGE].val4;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}

		// +MLkwpDDzMLAwwDDzMLk-
		if(sd->sc.data[SC_SWING].timer != -1) {
			int bonus = sd->sc.data[SC_SWING].val4;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}

		// +MMAw8zC5MKYwozC6MKYwqTD8MLA-
		if(sd->sc.data[SC_DANCE_WITH_WUG].timer != -1) {
			int bonus = 2 +- sd->sc.data[SC_DANCE_WITH_WUG].val4;
			if(haste_val2 < bonus)
				haste_val2 = bonus;
		}
		// +Zh8wbmnLMEg-
		if(sd->sc.data[SC_STARSTANCE].timer != -1)
			haste_val1 +-= sd->sc.data[SC_STARSTANCE].val2;

		/* +MF0wbk7W- */

		// +MNAw/DC1MPwwrw-
#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_BERSERK].timer != -1)
			berserk_flag = 1;
#else
		if(sd->sc.data[SC_BERSERK].timer != -1 && sd->sc.data[SC_TWOHANDQUICKEN].timer == -1)
			berserk_flag = 1;
#endif

		// +MMcwozDVMKcw8zDAMPw-
		if(sd->sc.data[SC_DEFENDER].timer != -1)
			bonus_add +-= sd->sc.data[SC_DEFENDER].val3;

		// +MNUwoTCkMMYwozDzMLAwuTDUMOowwzDI-
		if(sd->sc.data[SC_FIGHTINGSPIRIT].timer != -1)
			bonus_add -= sd->sc.data[SC_FIGHTINGSPIRIT].val3;

		// +MKwwuTDI-
		if(sd->sc.data[SC_GUST].timer != -1)
			bonus_add -= sd->sc.data[SC_GUST].val2;

		// +MNYw6TC5MMg-
		if(sd->sc.data[SC_BLAST].timer != -1)
			bonus_add -= sd->sc.data[SC_BLAST].val2;

		// +MO8wpDDrMMkwuTDIMPww4A-
		if(sd->sc.data[SC_WILD_STORM].timer != -1)
			bonus_add -= sd->sc.data[SC_WILD_STORM].val2;

		// +MOIw8zC5ML8w/FkJjqswxzDTMOswwQ-
		if(sd->sc.data[SC_MONSTER_TRANSFORM].timer != -1 && sd->sc.data[SC_MONSTER_TRANSFORM].val1 == 1109)
			bonus_add -= -10;

		// +X3EwbptC-
		if(sd->sc.data[SC_SOULSHADOW].timer != -1)
			bonus_add -= sd->sc.data[SC_SOULSHADOW].val3;

		// +X+BbnzBqT+Ff9Q-
		if(sd->sc.data[SC_SINCERE_FAITH].timer != -1)
			bonus_add -= sd->sc.data[SC_SINCERE_FAITH].val2;

		// +l1lbwg-
		if(sd->sc.data[SC_HANDICAPSTATE_DEEPSILENCE].timer != -1)
			bonus_add +-= sd->sc.data[SC_HANDICAPSTATE_DEEPSILENCE].val2;
	}

	/* +WSqWfTBoZwgwaGYfMG5gqptU- */
	if((skilllv = pc_checkskill(sd,SG_DEVIL)) > 0 && sd->status.job_level >= 50) {
		int bonus = skilllv*3 +- comfort_bonus;
		if(haste_val2 < bonus)
			haste_val2 = bonus;
	}

	/* slow_val+MGg-haste_val1+MGg-haste_val2+MJJSoHuXMFkwiw- */
	bonus_rate = slow_val - haste_val1 - haste_val2;

#ifdef PRE_RENEWAL
	/* bonus_rate+MGswojCkMMYw4DBuMNww/DDKMLkwklKge5cwWTCL- */
	if(sd->aspd_add_rate != 0 || sd->aspd_rate != 0) {
		sd->aspd_rate +-= sd->aspd_add_rate;
		bonus_rate -= sd->aspd_rate;
	}
#endif

	/* +MNAw/DC1MPwwrw- */
	if(berserk_flag)
		bonus_rate -= 30;

	/* +MNUw6jD8MK0w4zC5MMg- */
	if(sd->ud.skilltimer != -1 && (skilllv = pc_checkskill(sd,SA_FREECAST)) > 0)
		bonus_rate +-= 5 * (10 - skilllv);

	/* bonus_rate+MG6KCHuX- */
	if(bonus_rate != 0)
		amotion = amotion * (bonus_rate+-100) / 100;

	/* +MKIwyTDQMPMwuTDJMNYwwzCv- */
	if(sd->weapontype1 == WT_BOOK && (skilllv = pc_checkskill(sd,SA_ADVANCEDBOOK)) > 0)
		amotion -= amotion * (skilllv+-1) / 2 / 100;

	/* +MLcw8zCwMOswojCvMLcw5zDz- */
	if(sd->status.weapon >= WT_HANDGUN && sd->status.weapon <= WT_GRENADE && (skilllv = pc_checkskill(sd,GS_SINGLEACTION)) > 0)
		amotion -= amotion * (skilllv+-1) / 2 / 100;

	/* bonus_add+MG5SoHuX- */
	if(bonus_add != 0)
		amotion +-= bonus_add;

#ifndef PRE_RENEWAL
	if(pc_isriding(sd))	// +mg5RdU/uffQ-
		amotion = amotion * (150-10*pc_checkskill(sd,KN_CAVALIERMASTERY)) / 100;
	else if(pc_isdragon(sd))	// +MMkw6TC0MPMwyDDsMPwwyzDzMLA-
		amotion = amotion * (125-5*pc_checkskill(sd,RK_DRAGONTRAINING)) / 100;

	/* +MKIwpDDGMOAwbjDcMPwwyjC5MJJSoHuXMFkwiw- */
	if(sd->aspd_add_rate != 0 || sd->aspd_rate != 0) {
		sd->aspd_rate +-= sd->aspd_add_rate;
		amotion = amotion * (100-sd->aspd_rate) / 100;
	}

	/* +MNww/DDKMLk-ADD_ASPD+MG6KCHuX- */
	amotion +-= sd->aspd_add +- heatbarrel;
#endif

	/* +XA9lcFIHMIpOCjBS- */
	amotion = ceil(amotion);

	return (amotion < 1)? 1:(int)amotion;
}

/*==========================================
 * PC+MG4-speed+MJKKCHuXMFcwZo/UMFk-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
static int status_calc_speed_pc(struct map_session_data *sd, int speed)
{
	int bonus_rate;
	int speed_rate;
	int haste_val1 = 0;
	int haste_val2 = 0;
	int slow_val   = 0;
	int skilllv    = 0;
	char defender_flag = 0;
	char walkspeed_flag = 0;

	nullpo_retr(0, sd);

	if(sd->fix_status.speed > MIN_WALK_SPEED && sd->fix_status.speed <= MAX_WALK_SPEED)	// SPEED+Vvpbmg-
		return sd->fix_status.speed;
	if(sd->ud.skilltimer != -1 && pc_checkskill(sd,SA_FREECAST) > 0)	// +MNUw6jD8MK0w4zC5MMhytmFLMGowiXn7UtWQH16mVvpbmg-
		return sd->speed * (175 - 5 * pc_checkskill(sd,SA_FREECAST)) / 100;
	if(sd->sc.data[SC_STEELBODY].timer != -1)	// +kdFSWzBveftS1ZAfXqZW+lua-
		return 200;
	if(sd->ud.skilltimer != -1 && sd->ud.skillid == LG_EXEEDBREAK)		// +MKQwrzC3MPwwyTDWMOwwpDCvMG6KYFUxTi0-
		return (150 - 10 * sd->ud.skilllv);
	if(sd->sc.data[SC_FULL_THROTTLE].timer != -1)		// +MNUw6zC5MO0wwzDIMOtOLQ-
		return 50;

	/* speed+MExZCVMWMFkwizC5MMYw/DC/MLkwbooIe5c- */
	if(sd->sc.count > 0) {
		/* speed+MExYl1KgMFkwizC5MMYw/DC/MLkwbooIe5c- */

		// +MMgw8zDNMOswyTDpMKQw1g-
		if(sd->sc.data[SC_HIDING].timer != -1 && pc_checkskill(sd,RG_TUNNELDRIVE) > 0) {
			slow_val = 120 - 6 * pc_checkskill(sd,RG_TUNNELDRIVE);
		// +MMEwpzCkMLkwpjCpMPwwrw-(+m0JytmFL-)
		} else if(sd->sc.data[SC_CHASEWALK].timer != -1 && sd->sc.data[SC_ROGUE].timer != -1) {
			slow_val = -40;
		} else {
			// +kB9epm4bXBE-
			if(sd->sc.data[SC_DECREASEAGI].timer != -1)
				slow_val = 25;

			// +ecEwkn4bMIkwajBEMGc-
			if(sd->sc.data[SC_LONGINGFREEDOM].timer != -1) {
				int penalty = 50 - 10 * sd->sc.data[SC_LONGINGFREEDOM].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			// +jgowig-/+bxRZTw-
			} else if(sd->sc.data[SC_DANCING].timer != -1) {
				int penalty = 500 - (40 +- 10 * (sd->sc.data[SC_BARDDANCER].timer != -1)) * pc_checkskill(sd,((sd->sex == SEX_MALE)? BA_MUSICALLESSON: DC_DANCINGLESSON));
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MK8woTCwMN4wpDCi-
			if(sd->sc.data[SC_QUAGMIRE].timer != -1) {
				if(slow_val < 50)
					slow_val = 50;
			}

			// +ecEwkl/YMIwwajBEMGc-
			if(sd->sc.data[SC_DONTFORGETME].timer != -1) {
				if(slow_val < sd->sc.data[SC_DONTFORGETME].val2)
					slow_val = sd->sc.data[SC_DONTFORGETME].val2;
			}
			else if(sd->sc.data[SC_DONTFORGETME_].timer != -1) {
				if(slow_val < sd->sc.data[SC_DONTFORGETME_].val2)
					slow_val = sd->sc.data[SC_DONTFORGETME_].val2;
			}

			// +VGowRA-
			if(sd->sc.data[SC_CURSE].timer != -1) {
				if(slow_val < 300)
					slow_val = 300;
			}

			// +MMEwpzCkMLkwpjCpMPwwrw-(+kBpeOA-)
			if(sd->sc.data[SC_CHASEWALK].timer != -1 && sd->sc.data[SC_ROGUE].timer == -1) {
				int penalty = 35 - 5 * sd->sc.data[SC_CHASEWALK].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MN4w/DC3MOUwqjDWMKIw0zC5-
			if(sd->sc.data[SC_MARSHOFABYSS].timer != -1) {
				if(slow_val < sd->sc.data[SC_MARSHOFABYSS].val2)
					slow_val = sd->sc.data[SC_MARSHOFABYSS].val2;
			}

			// +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-(+MNowyjDrMMYwow-)
			if(sd->sc.data[SC_HALLUCINATIONWALK_POSTDELAY].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +MKsw4jDVMOkw/DC4MOU-
			if(sd->sc.data[SC_CAMOUFLAGE].timer != -1 && sd->sc.data[SC_CAMOUFLAGE].val1 > 2) {
				int penalty = 25 * (5 - sd->sc.data[SC_CAMOUFLAGE].val1);
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MLkwxjDrMLkw1TCjMPww6zDJ-(+T391KIAF-)
			if(sd->sc.data[SC_STEALTHFIELD_MASTER].timer != -1) {
				if(slow_val < 20)
					slow_val = 20;
			}

			// +MLAw6TDTMMYw/DC3MOcw8zDVMKMw/DDrMMk-
			if(battle_config.player_gravitation_type && sd->sc.data[SC_GRAVITATION].timer != -1) {
				int penalty = sd->sc.data[SC_GRAVITATION].val1 * 5;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +fVBaWohjiMU-
			if(sd->sc.data[SC_WEDDING].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +MLgw5zCkMPMwyDDTMPwwyA-
			if(sd->sc.data[SC_JOINTBEAT].timer != -1) {
				int penalty = 0;
				switch (sd->sc.data[SC_JOINTBEAT].val4) {
				case 0:	// +jbOZlg-
					penalty = 50;
					break;
				case 2:	// +gZ0-
					penalty = 30;
					break;
				}
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MK8w7TD8MK0w8zCw-(+XnNXMHn7UtU-)
			if(sd->sc.data[SC_CLOAKING].timer != -1) {
				int i;
				int check = 1;
				for(i=0; i<8; i+-+-) {
					if(map_getcell(sd->bl.m,sd->bl.x+-dirx[i],sd->bl.y+-diry[i],CELL_CHKNOPASS)) {
						check = 0;
						break;
					}
				}
				if(check) {
					int penalty = (sd->sc.data[SC_CLOAKING].val1 < 3)? 300:30 - 3 * sd->sc.data[SC_CLOAKING].val1;
					if(slow_val < penalty)
						slow_val = penalty;
				}
			}

			// +eftS1ZAfXqZPTk4L-(+MKIwpDDGMOA-)
			if(sd->sc.data[SC_SLOWPOTION].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
			if(sd->sc.data[SC_GATLINGFEVER].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +bDSQQQ-
			if(sd->sc.data[SC_SUITON].timer != -1) {
				if(sd->sc.data[SC_SUITON].val4) {
					if(slow_val < 50)
						slow_val = 50;
				}
			}

			// +MNUw7TC5MMgw3zC5MMYwow-
			if(sd->sc.data[SC_FROSTMISTY].timer != -1) {
				if(slow_val < 50)
					slow_val = 50;
			}

			// +MN4wuTCrMOww/DDJ/xow7DCkMLgwzTC5-
			if(sd->sc.data[SC__LAZINESS].timer != -1) {
				if(slow_val < 10)
					slow_val = 10;
			}

			// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
			if(sd->sc.data[SC__GROOMY].timer != -1) {
				int penalty = 5 +- 5 * sd->sc.data[SC__GROOMY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MOEw7TDzcgZfPg-
			if(sd->sc.data[SC_MELON_BOMB].timer != -1) {
				int penalty = sd->sc.data[SC_MELON_BOMB].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MNAwpDDzMMkwyDDpMMMw1w-
			if(sd->sc.data[SC_B_TRAP].timer != -1) {
				int penalty = sd->sc.data[SC_B_TRAP].val3;
				if(slow_val < penalty)
					slow_val = penalty;
			}
			// +kc1Sm4q/e8A-
			if(sd->sc.data[SC_CREATINGSTAR].timer != -1){
				if( slow_val < 10 ){
					slow_val = 10;
				}
			}

			// +MOow0DCmMPMwyQ-
			if(sd->sc.data[SC_REBOUND].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MLcw/DDrMMkwwTCnMPww8zDpMMMwtzDl-
			if(sd->sc.data[SC_SHIELDCHAINRUSH].timer != -1) {
				int penalty = sd->sc.data[SC_SHIELDCHAINRUSH].val3;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MLAw6TCmMPMwyTCwMOkw0zDGMPwwtzDnMPM-
			if(sd->sc.data[SC_GROUNDGRAVITY].timer != -1) {
				int penalty = sd->sc.data[SC_GROUNDGRAVITY].val3;
				if(slow_val < penalty)
					slow_val = penalty;
			}
		}

		/* speed+MExuG1wRMFkwizC5MMYw/DC/MLmKCHuX-1 */

		// +MLkw1DD8MMkw3TD8MLcw5zDz-
		if(sd->sc.data[SC_SPEEDUP1].timer != -1)
			haste_val1 = 50;

		// +kB9epliXUqA-
		if(sd->sc.data[SC_INCREASEAGI].timer != -1) {
			if(haste_val1 < 25)
				haste_val1 = 25;
		}

		// +MLUw3TD8MMibVGzV-(+eftS1ZAfXqZYl1Kg-)
		if(sd->sc.data[SC_SUPPORT_SPEED].timer != -1) {
			if(haste_val1 < 25)
				haste_val1 = 25;
		}

		// +MKYwpDDzMMkwpjCpMPwwrw-
		if(sd->sc.data[SC_WINDWALK].timer != -1) {
			int bonus = 2 * sd->sc.data[SC_WINDWALK].val1;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +MKsw/DDIMNYw/DC5MMg-
		if(sd->sc.data[SC_CARTBOOST].timer != -1) {
			if(haste_val1 < 20)
				haste_val1 = 20;
		}

		// +MK8w7TD8MK0w8zCw-(+WMFsvzBEeftS1Q-)
		if(sd->sc.data[SC_CLOAKING].timer != -1) {
			int i;
			int check = 1;
			for(i=0; i<8; i+-+-) {
				if(map_getcell(sd->bl.m,sd->bl.x+-dirx[i],sd->bl.y+-diry[i],CELL_CHKNOPASS)) {
					check = 0;
					break;
				}
			}
			if(!check) {
				int bonus = (sd->sc.data[SC_CLOAKING].val1 >= 10)? 25 : 3 * sd->sc.data[SC_CLOAKING].val1 - 3;
				if(haste_val1 < bonus)
					haste_val1 = bonus;
			}
		}

		// +MK8w7TD8MK0w8zCwMKgwrzC3MPwwyQ-
		if(sd->sc.data[SC_CLOAKINGEXCEED].timer != -1) {
			int bonus = sd->sc.data[SC_CLOAKINGEXCEED].val1 * 10;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +MNAw/DC1MPwwrw-
		if(sd->sc.data[SC_BERSERK].timer != -1) {
			if(haste_val1 < 25)
				haste_val1 = 25;
		}

		// +ML8wpDDqMK4-
		if(sd->sc.data[SC_RUN].timer != -1) {
			if(haste_val1 < 55)
				haste_val1 = 55;
		}

		// +fcpgJVbekH8-
		if(sd->sc.data[SC_AVOID].timer != -1) {
			int bonus = 10 * sd->sc.data[SC_AVOID].val1;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +MKYw6zDVMMAwwzC3MOU-
		if(sd->sc.data[SC_WUGDASH].timer != -1) {
			if(haste_val2 < 50)
				haste_val2 = 50;
		}

		// +MKIwrzC7MOkw7DD8MLcw5zDz-
		if(sd->sc.data[SC_ACCELERATION].timer != -1) {
			if(haste_val2 < 25)
				haste_val2 = 25;
		}

		// +MLkwpDDzMLAwwDDzMLk-
		if(sd->sc.data[SC_SWING].timer != -1) {
			if(haste_val1 < 25)
				haste_val1 = 25;
		}

		// +MKsw/DDIMNYw/DC5MMg-
		if(sd->sc.data[SC_GN_CARTBOOST].timer != -1) {
			int bonus = 25 +- ((sd->sc.data[SC_GN_CARTBOOST].val1 +- 1) / 2) * 25;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +eftS1ZAfXqZYl1Kg-(+MKIwpDDGMOA-)
		if(sd->sc.data[SC_SPEEDUP0].timer != -1) {
			if(haste_val1 < 25)
				haste_val1 = 25;
		}

		// +MKYwozDzMMkwuTDGMMMw1w-
		if(sd->sc.data[SC_WIND_STEP].timer != -1) {
			int bonus = sd->sc.data[SC_WIND_STEP].val3;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +MMEw4zC/MOow8zCw-
		if(sd->sc.data[SC_CHATTERING].timer != -1) {
			int bonus = sd->sc.data[SC_CHATTERING].val3;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +MKIwrzDpMKYwuTDAMMMwtzDl-
		if(sd->sc.data[SC_ARCLOUSEDASH].timer != -1) {
			if(haste_val1 < 25)
				haste_val1 = 25;
		}

		// +i2ZiEg-
		if(sd->sc.data[SC_HISS].timer != -1 && sd->sc.data[SC_HISS].val4 > 0) {
			if(haste_val1 < sd->sc.data[SC_HISS].val3)
				haste_val1 = sd->sc.data[SC_HISS].val3;
		}

		// +WRVxPDBRMG4wuzDsMMow/DDH-
		if(sd->sc.data[SC_JAWAII_SERENADE].timer != -1) {
			int bonus = sd->sc.data[SC_JAWAII_SERENADE].val4;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +MNcw7TDzMMYw6TDeMPwwwQ-
		if(sd->sc.data[SC_PRON_MARCH].timer != -1) {
			int bonus = sd->sc.data[SC_PRON_MARCH].val3;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		// +X3GWoDCM-
		if(sd->sc.data[SC_SHADOW_CLOCK].timer != -1) {
			int bonus = sd->sc.data[SC_SHADOW_CLOCK].val3;
			if(haste_val1 < bonus)
				haste_val1 = bonus;
		}

		/* speed+MExuG1wRMFkwizC5MMYw/DC/MLmKCHuX-2 */

		// +h41UCA-
		if(sd->sc.data[SC_FUSION].timer != -1)
			haste_val2 = 25;

		/* +MF0wbk7W- */

		// +MMcwozDVMKcw8zDAMPw-
		if(sd->sc.data[SC_DEFENDER].timer != -1)
			defender_flag = 1;

		// WALKSPEED
		if(sd->sc.data[SC_WALKSPEED].timer != -1 && sd->sc.data[SC_WALKSPEED].val1 > 0)
			walkspeed_flag = 1;
	}

	// +Vt6Qf3OHWJdSoA-
	if((sd->s_class.job == PC_JOB_AS || sd->s_class.job == PC_JOB_GC) && (skilllv = pc_checkskill(sd,TF_MISS)) > 0) {
		if(haste_val1 < skilllv)
			haste_val1 = skilllv;
	}

	// +MKIwpDDGMOAw3DD8MMowuQ-
	speed_rate = sd->speed_rate +- sd->speed_add_rate;
	if(speed_rate != 0) {
		if(haste_val1 < speed_rate)
			haste_val1 = speed_rate;
	}

	/* +mg5OVw- */

	// +MNowsw-
	if(pc_isriding(sd)) {
		if(haste_val2 < 25)
			haste_val2 = 25;
	}

	// +MMkw6TC0MPM-
	if(pc_isdragon(sd)) {
		if(haste_val2 < 25)
			haste_val2 = 25;
	}

	// +MKYw6zDV-
	if(pc_iswolfmount(sd)) {
		int bonus = 15 +- 5 * pc_checkskill(sd,RA_WUGRIDER);
		if(haste_val2 < bonus)
			haste_val2 = bonus;
	}

	// +mg5OVzC3MLkwxjDg-
	if(sd->sc.data[SC_ALL_RIDING].timer != -1) {
		if(haste_val2 < 25)
			haste_val2 = 25;
	}

	/* bonus_rate+MG5nAE9OUCQwkootW5o- */
	bonus_rate = slow_val - haste_val1 - haste_val2;
	if(bonus_rate < -60)
		bonus_rate = -60;

	/* speed+MG5nAH1Cigh7lw- */
	if(pc_iscarton(sd))	// +MKsw/DDI-
		speed +-= speed * (50 - 5 * pc_checkskill(sd,MC_PUSHCART)) / 100;
	if(pc_isgear(sd))	// +m1RcDjCuMKJkLU5X-
		speed +-= speed * (5 - pc_checkskill(sd,NC_MADOLICENCE)) / 10;
	if(bonus_rate != 0)	// bonus_rate
		speed = speed * (bonus_rate+-100) / 100;
	if(defender_flag && speed < 200)	// +MMcwozDVMKcw8zDAMPw-
		speed = 200;
	if(walkspeed_flag)	// +MLkwrzDqMNcwyHUoMLkwxjD8ML8wuQ-
		speed = speed * 100 / sd->sc.data[SC_WALKSPEED].val1;

	/* +ZwBPTlAkMAFnAFknUCQwkootW5owWTCL- */
	if(speed < MIN_WALK_SPEED)
		speed = MIN_WALK_SPEED;
	if(speed > MAX_WALK_SPEED)
		speed = MAX_WALK_SPEED;

	return speed;
}

/*==========================================
 * +W/6MYTBu-group+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_group(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return mobdb_search(((struct mob_data *)bl)->class_)->group_id;
	// PC PET+MG8-0+/whnKootW5o-)

	return 0;
}

/*==========================================
 * +W/6MYTBu-Class+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_class(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return ((struct mob_data *)bl)->class_;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->status.class_;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		return ((struct pet_data *)bl)->class_;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return ((struct homun_data *)bl)->status.class_;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->status.class_;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->status.class_;

	return 0;
}

/*==========================================
 * +W/6MYTBuZblUETCSj9QwWQ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_dir(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return ((struct mob_data *)bl)->dir;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->dir;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		return ((struct pet_data *)bl)->dir;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return ((struct homun_data *)bl)->dir;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->dir;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->dir;

	return 0;
}

/*==========================================
 * +W/6MYTBuMOww2TDrMJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_lv(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return mobdb_search(((struct mob_data *)bl)->class_)->lv;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->status.base_level;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		return ((struct pet_data *)bl)->msd->pet.level;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return ((struct homun_data *)bl)->status.base_level;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->base_level;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->base_level;

	return 0;
}

/*==========================================
 * +W/6MYTBugHdpbTDsMNkw6zCSj9QwWQ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_jlv(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return mobdb_search(((struct mob_data *)bl)->class_)->lv;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->status.job_level;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		return ((struct pet_data *)bl)->msd->pet.level;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return ((struct homun_data *)bl)->status.base_level;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->base_level;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->base_level;

	return 0;
}

/*==========================================
 * +W/6MYTBuXAR6CzCSj9QwWQ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_range(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return mobdb_search(((struct mob_data *)bl)->class_)->range;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->range.attackrange;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		return mobdb_search(((struct pet_data *)bl)->class_)->range;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return 2;//((struct homun_data *)bl)->attackrange;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->attackrange;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->attackrange;

	return 0;
}

/*==========================================
 * +W/6MYTBu-HP+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_hp(struct block_list *bl)
{
	nullpo_retr(1, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return ((struct mob_data *)bl)->hp;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->status.hp;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return ((struct homun_data *)bl)->status.hp;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->status.hp;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->status.hp;

	return 1;
}

/*==========================================
 * +W/6MYTBu-SP+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_sp(struct block_list *bl)
{
	nullpo_retr(1, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->status.sp;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		return ((struct homun_data *)bl)->status.sp;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		return ((struct merc_data *)bl)->status.sp;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return ((struct elem_data *)bl)->status.sp;

	return 0;
}

/*==========================================
 * +W/6MYTBu-AP+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_ap(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data *)bl)->status.ap;

	return 0;
}

/*==========================================
 * +W/6MYTBu-MHP+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_max_hp(struct block_list *bl)
{
	int max_hp = 1;

	nullpo_retr(1, bl);

	if(bl->type == BL_PC && ((struct map_session_data *)bl)) {
		max_hp = ((struct map_session_data *)bl)->status.max_hp;
	} else if(bl->type == BL_HOM && ((struct homun_data *)bl)) {
		max_hp = ((struct homun_data *)bl)->max_hp;
	} else if(bl->type == BL_MERC && ((struct merc_data *)bl)) {
		max_hp = ((struct merc_data *)bl)->max_hp;
	} else if(bl->type == BL_ELEM && ((struct elem_data *)bl)) {
		max_hp = ((struct elem_data *)bl)->max_hp;
	} else {
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB && ((struct mob_data*)bl)) {
			atn_bignumber hp = ((struct mob_data*)bl)->st.max_hp;
			struct guild_castle *gc = guild_mapid2gc(bl->m);
			if(mobdb_search(((struct mob_data*)bl)->class_)->mexp > 0) {
				if(battle_config.mvp_hp_rate != 100) {
					hp = hp * battle_config.mvp_hp_rate / 100;
				}
			} else {
				if(battle_config.monster_hp_rate != 100)
					hp = hp * battle_config.monster_hp_rate / 100;
			}
			if(gc && ((struct mob_data*)bl)->guild_id == gc->guild_id) {
				hp +-= gc->defense * 1000;
			}
			max_hp = (hp > 0x7FFFFFFF ? 0x7FFFFFFF : (int)hp);
		} else if(bl->type == BL_PET && ((struct pet_data*)bl)) {
			max_hp = mobdb_search(((struct pet_data*)bl)->class_)->max_hp;
			if(mobdb_search(((struct pet_data*)bl)->class_)->mexp > 0) {
				if(battle_config.mvp_hp_rate != 100) {
					atn_bignumber hp = (atn_bignumber)max_hp * battle_config.mvp_hp_rate / 100;
					max_hp = (hp > 0x7FFFFFFF ? 0x7FFFFFFF : (int)hp);
				}
			} else {
				if(battle_config.monster_hp_rate != 100)
					max_hp = (max_hp * battle_config.monster_hp_rate)/100;
			}
		}
		if(sc) {
#ifndef PRE_RENEWAL
			if(sc->data[SC_DELUGE].timer != -1)
				max_hp +-= sc->data[SC_DELUGE].val3 * max_hp / 100;
#endif
			if(sc->data[SC_APPLEIDUN].timer != -1)
				max_hp +-= ((5+-sc->data[SC_APPLEIDUN].val1*2+-((sc->data[SC_APPLEIDUN].val2+-1)>>1)
						+-sc->data[SC_APPLEIDUN].val3/10) * max_hp)/100;
			if(sc->data[SC_FRIGG_SONG].timer != -1)
				max_hp +-= sc->data[SC_FRIGG_SONG].val3 * max_hp / 100;
			if(sc->data[SC_EQC].timer != -1)
				max_hp -= sc->data[SC_EQC].val3 * max_hp / 100;
		}
		if(max_hp < 1) max_hp = 1;
	}

	return max_hp;
}

/*==========================================
 * +W/6MYTBu-MSP+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_max_sp(struct block_list *bl)
{
	int max_sp = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && ((struct map_session_data *)bl)) {
		max_sp = ((struct map_session_data *)bl)->status.max_sp;
	} else if(bl->type == BL_HOM && ((struct homun_data *)bl)) {
		max_sp = ((struct homun_data *)bl)->max_sp;
	} else if(bl->type == BL_MERC && ((struct merc_data *)bl)) {
		max_sp = ((struct merc_data *)bl)->max_sp;
	} else if(bl->type == BL_ELEM && ((struct elem_data *)bl)) {
		max_sp = ((struct elem_data *)bl)->max_sp;
	}

	return max_sp;
}

/*==========================================
 * +W/6MYTBu-MaxAP+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_max_ap(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC && ((struct map_session_data *)bl))
		return ((struct map_session_data *)bl)->status.max_ap;

	return 0;
}

/*==========================================
 * +W/6MYTBu-Str+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_str(struct block_list *bl)
{
	int str = 0;
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && ((struct mob_data *)bl))
		str = ((struct mob_data *)bl)->st.str;
	else if(bl->type == BL_PC && ((struct map_session_data *)bl))
		return ((struct map_session_data *)bl)->paramc[0];
	else if(bl->type == BL_PET && ((struct pet_data *)bl))
		str = mobdb_search(((struct pet_data *)bl)->class_)->str;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		str = ((struct homun_data *)bl)->status.str;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		str = ((struct merc_data *)bl)->str;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		str = ((struct elem_data *)bl)->str;

	if(sc && bl->type != BL_HOM) {
		if(sc->data[SC_LOUD].timer != -1 && sc->data[SC_QUAGMIRE].timer == -1 && bl->type != BL_PC)
			str +-= 4;
		if( sc->data[SC_BLESSING].timer != -1 && bl->type != BL_PC) {	// +MNYw7DDDMLcw8zCw-
			int race = status_get_race(bl);
			if(battle_check_undead(race,status_get_elem_type(bl)) || race == RCT_DEMON)
				str >>= 1;				// +YKqbVA-/+Tg1rew-
			else
				str +-= sc->data[SC_BLESSING].val1;	// +MF0wbk7W-
		}
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			str +-= 5;
		if(sc->data[SC_CHASEWALK_STR].timer != -1)
			str +-= sc->data[SC_CHASEWALK_STR].val1;
		if(sc->data[SC_ALL_STAT_DOWN].timer != -1 && bl->type != BL_PC)	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			str -= sc->data[SC_ALL_STAT_DOWN].val2;
	}
	if(str < 0) str = 0;
	return str;
}

/*==========================================
 * +W/6MYTBu-Agi+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_agi(struct block_list *bl)
{
	int agi = 0;
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && (struct mob_data *)bl)
		agi = ((struct mob_data *)bl)->st.agi;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		agi = ((struct map_session_data *)bl)->paramc[1];
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		agi = mobdb_search(((struct pet_data *)bl)->class_)->agi;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		agi = ((struct homun_data *)bl)->agi;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		agi = ((struct merc_data *)bl)->agi;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		agi = ((struct elem_data *)bl)->agi;

	if(sc && bl->type != BL_HOM) {
		if(sc->data[SC_SPEEDUP1].timer != -1 && bl->type != BL_PC)	// +kB9epl83UxY-
			agi *= 3;
		if(sc->data[SC_INCREASEAGI].timer != -1 && sc->data[SC_QUAGMIRE].timer == -1 && sc->data[SC_DONTFORGETME].timer == -1 && bl->type != BL_PC)	// +kB9epliXUqA-(PC+MG8-pc.c+MGc-)
			agi +-= 2+-sc->data[SC_INCREASEAGI].val1;
		if(sc->data[SC_SUITON].timer != -1 && sc->data[SC_SUITON].val3 != 0 && bl->type != BL_PC)	// +bDSQQQ-
			agi +-= sc->data[SC_SUITON].val3;
		if(sc->data[SC_CONCENTRATE].timer != -1 && sc->data[SC_QUAGMIRE].timer == -1 && bl->type != BL_PC)
			agi +-= agi*(2+-sc->data[SC_CONCENTRATE].val1)/100;
		if(sc->data[SC_DECREASEAGI].timer != -1)	// +kB9epm4bXBH/CDCqMPww0DD8MLkwrTDrTtVp2DBv-AGI-50+/wk-
			agi -= (sc->data[SC_DECREASEAGI].val2)? 50: 2+-sc->data[SC_DECREASEAGI].val1;
		if(sc->data[SC_QUAGMIRE].timer != -1)	// +MK8woTCwMN4wpDCi-
#ifdef PRE_RENEWAL
			agi -= agi * sc->data[SC_QUAGMIRE].val1*10 / 100;
#else
			agi -= (agi/2 < sc->data[SC_QUAGMIRE].val1*10) ? agi/2 : sc->data[SC_QUAGMIRE].val1*10;
#endif
		if(sc->data[SC_MARSHOFABYSS].timer != -1)	// +MN4w/DC3MOUwqjDWMKIw0zC5-
			agi -= agi * sc->data[SC_MARSHOFABYSS].val3 / 100;
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			agi +-= 5;
		if(sc->data[SC_ALL_STAT_DOWN].timer != -1 && bl->type != BL_PC)	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			agi -= sc->data[SC_ALL_STAT_DOWN].val2;
	}
	if(agi < 0) agi = 0;
	return agi;
}

/*==========================================
 * +W/6MYTBu-Vit+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_vit(struct block_list *bl)
{
	int vit = 0;
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && (struct mob_data *)bl)
		vit = ((struct mob_data *)bl)->st.vit;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		vit = ((struct map_session_data *)bl)->paramc[2];
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		vit = mobdb_search(((struct pet_data *)bl)->class_)->vit;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		vit = ((struct homun_data *)bl)->vit;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		vit = ((struct merc_data *)bl)->vit;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		vit = ((struct elem_data *)bl)->vit;

	if(sc && bl->type != BL_HOM) {
		if(sc->data[SC_STRIPARMOR].timer != -1 && bl->type != BL_PC)
			vit = vit*60/100;
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			vit +-= 5;
		if(sc->data[SC_ALL_STAT_DOWN].timer != -1 && bl->type != BL_PC)	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			vit -= sc->data[SC_ALL_STAT_DOWN].val2;
	}

	if(vit < 0) vit = 0;
	return vit;
}

/*==========================================
 * +W/6MYTBu-Int+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_int(struct block_list *bl)
{
	int int_ = 0;
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && (struct mob_data *)bl)
		int_ = ((struct mob_data *)bl)->st.int_;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		int_ = ((struct map_session_data *)bl)->paramc[3];
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		int_ = mobdb_search(((struct pet_data *)bl)->class_)->int_;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		int_ = ((struct homun_data *)bl)->int_;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		int_ = ((struct merc_data *)bl)->int_;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		int_ = ((struct elem_data *)bl)->int_;

	if(sc && bl->type != BL_HOM) {
		if(sc->data[SC_BLESSING].timer != -1 && bl->type != BL_PC) {	// +MNYw7DDDMLcw8zCw-
			int race = status_get_race(bl);
			if(battle_check_undead(race,status_get_elem_type(bl)) || race == RCT_DEMON)
				int_ >>= 1;	// +YKqbVA-/+Tg1rew-
			else
				int_ +-= sc->data[SC_BLESSING].val1;	// +MF0wbk7W-
		}
		if(sc->data[SC_STRIPHELM].timer != -1 && bl->type != BL_PC)
			int_ = int_*60/100;
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			int_ +-= 5;
		if(sc->data[SC__STRIPACCESSARY].timer != -1 && bl->type != BL_PC)	// +MLkwyDDqMMMw1zCiMK8wuzC1MOow/A-
			int_ = int_ * 80 / 100;
		if(sc->data[SC_ALL_STAT_DOWN].timer != -1 && bl->type != BL_PC)	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			int_ -= sc->data[SC_ALL_STAT_DOWN].val2;
	}
	if(int_ < 0) int_ = 0;
	return int_;
}

/*==========================================
 * +W/6MYTBu-Dex+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_dex(struct block_list *bl)
{
	int dex = 0;
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && (struct mob_data *)bl)
		dex = ((struct mob_data *)bl)->st.dex;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		dex = ((struct map_session_data *)bl)->paramc[4];
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		dex = mobdb_search(((struct pet_data *)bl)->class_)->dex;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		dex = ((struct homun_data *)bl)->dex;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		dex = ((struct merc_data *)bl)->dex;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		dex = ((struct elem_data *)bl)->dex;

	if(sc && bl->type != BL_HOM) {
#ifdef PRE_RENEWAL
		if(sc->data[SC_EXPLOSIONSPIRITS].timer != -1 && bl->type != BL_PC)
			dex *= 3;	// NPC+cgaIwmziUtU-
#endif
		if(sc->data[SC_CONCENTRATE].timer != -1 && sc->data[SC_QUAGMIRE].timer == -1 && bl->type != BL_PC)
			dex +-= dex*(2+-sc->data[SC_CONCENTRATE].val1)/100;

		if(sc->data[SC_BLESSING].timer != -1 && bl->type != BL_PC) {	// +MNYw7DDDMLcw8zCw-
			int race = status_get_race(bl);
			if(battle_check_undead(race,status_get_elem_type(bl)) || race == RCT_DEMON)
				dex >>= 1;	// +YKqbVA-/+Tg1rew-
			else
				dex +-= sc->data[SC_BLESSING].val1;	// +MF0wbk7W-
		}
		if(sc->data[SC_QUAGMIRE].timer != -1)	// +MK8woTCwMN4wpDCi-
#ifdef PRE_RENEWAL
			dex -= dex * sc->data[SC_QUAGMIRE].val1*10 / 100;
#else
			dex -= (dex/2 < sc->data[SC_QUAGMIRE].val1*10) ? dex/2 : sc->data[SC_QUAGMIRE].val1*10;
#endif
		if(sc->data[SC_MARSHOFABYSS].timer != -1)	// +MN4w/DC3MOUwqjDWMKIw0zC5-
			dex -= dex * sc->data[SC_MARSHOFABYSS].val3 / 100;
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			dex +-= 5;
		if(sc->data[SC__STRIPACCESSARY].timer != -1 && bl->type != BL_PC)	// +MLkwyDDqMMMw1zCiMK8wuzC1MOow/A-
			dex = dex * 80 / 100;
		if(sc->data[SC_ALL_STAT_DOWN].timer != -1 && bl->type != BL_PC)	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			dex -= sc->data[SC_ALL_STAT_DOWN].val2;
	}
	if(dex < 0) dex = 0;
	return dex;
}

/*==========================================
 * +W/6MYTBu-Luk+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_luk(struct block_list *bl)
{
	int luk = 0;
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && (struct mob_data *)bl)
		luk = ((struct mob_data *)bl)->st.luk;
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
		luk = ((struct map_session_data *)bl)->paramc[5];
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		luk = mobdb_search(((struct pet_data *)bl)->class_)->luk;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		luk = ((struct homun_data *)bl)->luk;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		luk = ((struct merc_data *)bl)->luk;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		luk = ((struct elem_data *)bl)->luk;

	if(sc && bl->type != BL_HOM) {
		if(sc->data[SC_GLORIA].timer != -1 && bl->type != BL_PC)	// +MLAw7TDqMKI-(PC+MG8-pc.c+MGc-)
			luk +-= 30;
		if(sc->data[SC_CURSE].timer != -1 )		// +VGowRA-
			luk = 0;
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			luk +-= 5;
		if(sc->data[SC__STRIPACCESSARY].timer != -1 && bl->type != BL_PC)	// +MLkwyDDqMMMw1zCiMK8wuzC1MOow/A-
			luk = luk * 80 / 100;
		if(sc->data[SC_BANANA_BOMB].timer != -1 && bl->type != BL_PC)	// +MNAwyjDKcgZfPg-
			luk -= luk * sc->data[SC_BANANA_BOMB].val1 / 100;
		if(sc->data[SC_ALL_STAT_DOWN].timer != -1 && bl->type != BL_PC)	// +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w-
			luk -= sc->data[SC_ALL_STAT_DOWN].val2;
	}
	if(luk < 0) luk = 0;
	return luk;
}

/*==========================================
 * +W/6MYTBu-Pow+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_pow(struct block_list *bl)
{
	int pow = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		pow = ((struct map_session_data *)bl)->paramc[6];

	if(pow < 0) pow = 0;
	return pow;
}

/*==========================================
 * +W/6MYTBu-Sta+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_sta(struct block_list *bl)
{
	int sta = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		sta = ((struct map_session_data *)bl)->paramc[7];

	if(sta < 0) sta = 0;
	return sta;
}

/*==========================================
 * +W/6MYTBu-Wis+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_wis(struct block_list *bl)
{
	int wis = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		wis = ((struct map_session_data *)bl)->paramc[8];

	if(wis < 0) wis = 0;
	return wis;
}

/*==========================================
 * +W/6MYTBu-Spl+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_spl(struct block_list *bl)
{
	int spl = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		spl = ((struct map_session_data *)bl)->paramc[9];

	if(spl < 0) spl = 0;
	return spl;
}

/*==========================================
 * +W/6MYTBu-Con+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_con(struct block_list *bl)
{
	int con = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		con = ((struct map_session_data *)bl)->paramc[10];

	if(con < 0) con = 0;
	return con;
}

/*==========================================
 * +W/6MYTBu-Crt+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_crt(struct block_list *bl)
{
	int crt = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		crt = ((struct map_session_data *)bl)->paramc[11];

	if(crt < 0) crt = 0;
	return crt;
}

/*==========================================
 * +W/6MYTBu-Flee+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
int status_get_flee(struct block_list *bl)
{
	int flee = 1, target_count = 1;
	struct status_change *sc;

	nullpo_retr(1, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_PC && (struct map_session_data *)bl)
		flee = ((struct map_session_data *)bl)->flee;
	else if(bl->type == BL_HOM && ((struct homun_data *)bl))
		flee = ((struct homun_data *)bl)->flee;
	else if(bl->type == BL_MERC && ((struct merc_data *)bl))
		flee = ((struct merc_data *)bl)->flee;
	else if(bl->type == BL_ELEM && ((struct elem_data *)bl))
		flee = ((struct elem_data *)bl)->flee;
	else if(bl->type == BL_MOB && ((struct mob_data *)bl))
		flee = ((struct mob_data *)bl)->st.flee;
	else
		flee = status_get_agi(bl) +- status_get_lv(bl);

#ifndef PRE_RENEWAL
	if(bl->type != BL_PC && bl->type != BL_HOM)
		flee +-= 100;
#endif

	/* +Y1Vzcg- */
	if(sc && (sc->data[SC_TINDER_BREAKER].timer != -1 || sc->data[SC_CBC].timer != -1))
		return 0;

	if(sc && bl->type != BL_HOM) {
		if(sc->data[SC_WHISTLE].timer != -1 && bl->type != BL_PC)
			flee +-= sc->data[SC_WHISTLE].val1 +- sc->data[SC_WHISTLE].val2 +- sc->data[SC_WHISTLE].val3;
		if(sc->data[SC_BLIND].timer != -1 && bl->type != BL_PC)
			flee -= flee*25/100;
		if(sc->data[SC_WINDWALK].timer != -1 && bl->type != BL_PC)		// +MKYwozDzMMkwpjCpMPwwrw-
			flee +-= sc->data[SC_WINDWALK].val2;
		if(sc->data[SC_SPIDERWEB].timer != -1 && bl->type != BL_PC)	// +MLkw0TCkMMAw/DCmMKcw1g-
			flee -= 50;
		if(sc->data[SC_THE_SUN].timer != -1 && bl->type != BL_PC)	// THE SUN
			flee = flee*80/100;
		if(sc->data[SC_GATLINGFEVER].timer != -1 && bl->type != BL_PC)	// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
			flee -= sc->data[SC_GATLINGFEVER].val1*5;
		if(sc->data[SC_ADJUSTMENT].timer != -1 && bl->type != BL_PC)	// +MKIwuDDjMLkwyDDhMPMwyA-
			flee +-= 30;
		if(sc->data[SC_FEAR].timer != -1 && bl->type != BL_PC)		// +YFBgFg-
			flee -= flee*20/100;
		if(sc->data[SC_HALLUCINATIONWALK].timer != -1 && bl->type != BL_PC)		// +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-
			flee +-= sc->data[SC_HALLUCINATIONWALK].val1 * 50;
		if(sc->data[SC_INFRAREDSCAN].timer != -1 && bl->type != BL_PC)	// +MKQw8zDVMOkw7DDDMMkwuTCtMOMw8w-
			flee  -= flee*30/100;
		if(sc->data[SC__LAZINESS].timer != -1 && bl->type != BL_PC)		// +MN4wuTCrMOww/DDJ/xow7DCkMLgwzTC5-
			flee  -= flee*10/100;
		if(sc->data[SC_GLOOMYDAY].timer != -1 && bl->type != BL_PC)		// +MOEw6TDzMLMw6jD8-
			flee -= sc->data[SC_GLOOMYDAY].val1 * 5 +- 20;
		if(sc->data[SC_SATURDAY_NIGHT_FEVER].timer != -1 && bl->type != BL_PC)		// +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw-
			flee -= flee * (40 +- sc->data[SC_SATURDAY_NIGHT_FEVER].val1 * 10) / 100;
		if(sc->data[SC_FIRE_EXPANSION_SMOKE_POWDER].timer != -1 && bl->type != BL_PC)	// +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+cVleVQ-)
			flee +-= flee * sc->data[SC_FIRE_EXPANSION_SMOKE_POWDER].val2 / 100;
		if(sc->data[SC_FIRE_EXPANSION_TEAR_GAS].timer != -1 && bl->type != BL_PC)	// +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmQ-)
			flee -= flee * sc->data[SC_FIRE_EXPANSION_TEAR_GAS].val2 / 100;
#ifndef PRE_RENEWAL
		if(sc->data[SC_SPEARQUICKEN].timer != -1 && bl->type != BL_PC)      // +MLkw1DCiMK8wpDDDMLEw8w-
			flee +-= sc->data[SC_SPEARQUICKEN].val1 * 2;
#endif
		if(sc->data[SC_GROOMING].timer != -1 && bl->type != BL_PC)	// +MLAw6zD8MN8w8zCw-
			flee +-= sc->data[SC_GROOMING].val2;
		if(sc->data[SC_C_MARKER].timer != -1 && bl->type != BL_PC)  // +MK8w6jDgML4w8zDeMPwwqzD8-
			flee -= sc->data[SC_C_MARKER].val1 * 10;
		if(sc->data[SC_VOLCANIC_ASH].timer != -1 && sc->data[SC_VOLCANIC_ASH].val4 > 0) 	// +cGtccXBw-
			flee -= flee * sc->data[SC_VOLCANIC_ASH].val4 / 100;
		if(sc->data[SC_HANDICAPSTATE_DEEPBLIND].timer != -1 && bl->type != BL_PC)	// +bwae0g-
			flee = 0;
	}

	// +Vt6Qf3OHiNxrYw-
	if(bl->type != BL_HOM)
		target_count = unit_counttargeted(bl,battle_config.agi_penalty_count_lv);

	if(battle_config.agi_penalty_type > 0 && target_count >= battle_config.agi_penalty_count) {
		// +MNowyjDrMMYwo4otW5owiDCKW/6MYTBMWRowRA-
		if(battle_config.agi_penalty_type == 1) {
			// +Vt6Qf3OHMEw-agi_penalty_num%+MFowZG4bXBE-
			int flee_rate = (target_count-(battle_config.agi_penalty_count-1)) * battle_config.agi_penalty_num;
			flee = flee * (100 - flee_rate) / 100;
		} else if(battle_config.agi_penalty_type == 2) {
			// +Vt6Qf3OHMEw-agi_penalty_num+UgZuG1wR-
			flee -= (target_count - (battle_config.agi_penalty_count - 1))*battle_config.agi_penalty_num;
		}
	}
	// +W/5Oug-MAP+MGcwbm4bXBFSuWec-
	if(battle_config.gvg_flee_penalty & 1 && map[bl->m].flag.gvg) {
		flee = flee*(200 - battle_config.gvg_flee_rate)/100;	// +W5+WmzBr-GvG+MGc-Flee+MJJuG1wR-
	}
	if(battle_config.gvg_flee_penalty & 2 && map[bl->m].flag.pvp) {
		flee = flee*(200 - battle_config.gvg_flee_rate)/100;	// +W5+WmzBr-PvP+MGc-Flee+MJJuG1wR-
	}
	if(flee < 1) flee = 1;

	return flee;
}

/*==========================================
 * +W/6MYTBu-Hit+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
int status_get_hit(struct block_list *bl)
{
	int hit = 0;

	nullpo_retr(1, bl);

#ifndef PRE_RENEWAL
	if(!(bl->type & (BL_PC | BL_HOM)))
		hit = 150;
#endif
	if(bl->type == BL_PC) {
		hit = ((struct map_session_data *)bl)->hit;
	} else if(bl->type == BL_HOM && ((struct homun_data *)bl)) {
		hit +-= ((struct homun_data *)bl)->hit;
	} else if(bl->type == BL_MERC && ((struct merc_data *)bl)) {
		hit +-= ((struct merc_data *)bl)->hit;
	} else if(bl->type == BL_ELEM && ((struct elem_data *)bl)) {
		hit +-= ((struct elem_data *)bl)->hit;
	} else if(bl->type == BL_MOB && ((struct mob_data *)bl)) {
		hit +-= ((struct mob_data *)bl)->st.hit;
	} else {
		struct status_change *sc = status_get_sc(bl);

		hit +-= status_get_dex(bl) +- status_get_lv(bl);
		if(sc) {
#ifndef PRE_RENEWAL
			if(sc->data[SC_EXPLOSIONSPIRITS].timer != -1 && bl->type != BL_PC)	// NPC+cgaIwmziUtU-
				hit *= 2;
#endif
			if(sc->data[SC_HUMMING].timer != -1)
				hit +-= hit*(sc->data[SC_HUMMING].val1*2+-sc->data[SC_HUMMING].val2 +- sc->data[SC_HUMMING].val3)/100;
			if(sc->data[SC_TRUESIGHT].timer != -1)		// +MMgwpTDrMPwwtTCkMMg-
				hit +-= 3*(sc->data[SC_TRUESIGHT].val1);
			if(sc->data[SC_CONCENTRATION].timer != -1)	// +MLMw8zC7MPMwyDDsMPwwtzDnMPM-
				hit +-= 10*(sc->data[SC_CONCENTRATION].val1);
			if(sc->data[SC_THE_SUN].timer != -1 && bl->type != BL_PC)
				hit = hit*80/100;
			if(sc->data[SC_ADJUSTMENT].timer != -1 && bl->type != BL_PC) // +MKIwuDDjMLkwyDDhMPMwyA-
				hit -= 30;
			if(sc->data[SC_INCREASING].timer != -1 && bl->type != BL_PC) // +MKQw8zCvMOow/DC4MPMwsDCiMK0w5TDpMLcw/A-
				hit +-= 20;
			if(sc->data[SC_INCHIT].timer!=-1 && bl->type != BL_PC)	// +MKwwpDDHMMMwyTCiML8wwzCv-
				hit +-= sc->data[SC_INCHIT].val1;
			if(sc->data[SC_FEAR].timer != -1 && bl->type != BL_PC)	// +YFBgFg-
				hit -= hit*20/100;
			if(sc->data[SC__GROOMY].timer != -1 && bl->type != BL_PC)	// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
				hit -= hit*(20*sc->data[SC__GROOMY].val1)/100;
			if(sc->data[SC_FIRE_EXPANSION_TEAR_GAS].timer != -1 && bl->type != BL_PC)	// +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmQ-)
				hit -= hit*(sc->data[SC_FIRE_EXPANSION_TEAR_GAS].val2)/100;
			if(sc->data[SC_ILLUSIONDOPING].timer != -1 && bl->type != BL_PC)	// +MKQw6jDlMPwwuDDnMPMwyTD8MNQw8zCw-
				hit -= 50;
			if(sc->data[SC_HEAT_BARREL].timer != -1 && bl->type != BL_PC)  // +MNIw/DDIMNAw7DDr-
				hit -= sc->data[SC_HEAT_BARREL].val4;
			if(sc->data[SC_VOLCANIC_ASH].timer != -1 && sc->data[SC_VOLCANIC_ASH].val2 > 0) 	// +cGtccXBw-
				hit -= hit * sc->data[SC_VOLCANIC_ASH].val2 / 100;
			if(sc->data[SC_HANDICAPSTATE_MISFORTUNE].timer != -1 && bl->type != BL_PC)	// +Tg1eeA-
				hit = 0;
		}
	}
	if(hit < 1) hit = 1;

	return hit;
}

/*==========================================
 * +W/6MYTBuW4xRaFbekH8wko/UMFk-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
int status_get_flee2(struct block_list *bl)
{
	int flee2 = 1;
	struct status_change *sc;

	nullpo_retr(1, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		flee2 = status_get_luk(bl) +- 10;
		flee2 +-= ((struct map_session_data *)bl)->flee2 - (((struct map_session_data *)bl)->paramc[5] +- 10);
	} else {
		flee2 = status_get_luk(bl)+-1;
	}

	if(sc) {
		if(sc->data[SC__UNLUCKY].timer != -1 && bl->type != BL_PC)	// +MN4wuTCrMOww/DDJ/xowojDzMOkwwzCtMPw-
			flee2 -= sc->data[SC__UNLUCKY].val1 * 10;
		if(sc->data[SC_HANDICAPSTATE_DEEPBLIND].timer != -1 && bl->type != BL_PC)	// +bwae0g-
			flee2 = 0;
	}

	if(flee2 < 1) flee2 = 1;
	return flee2;
}

/*==========================================
 * +W/6MYTBuMK8w6jDGMKMwqzDrMJKP1DBZ-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
int status_get_critical(struct block_list *bl)
{
	int critical = 1;
	struct status_change *sc;

	nullpo_retr(1, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_PC && (struct map_session_data *)bl) {
#ifdef PRE_RENEWAL
		critical = status_get_luk(bl)*3 +- 10;
		critical +-= ((struct map_session_data *)bl)->critical - ((((struct map_session_data *)bl)->paramc[5]*3) +- 10);
#else
		critical = status_get_luk(bl)*33/10 +- 10;
		critical +-= ((struct map_session_data *)bl)->critical - ((((struct map_session_data *)bl)->paramc[5]*33/10) +- 10);
#endif
	} else if(bl->type == BL_HOM && ((struct homun_data *)bl)) {
		critical = ((struct homun_data *)bl)->critical;
	} else if(bl->type == BL_MERC && ((struct merc_data *)bl)) {
		critical = ((struct merc_data *)bl)->critical;
	} else if(bl->type == BL_ELEM && ((struct elem_data *)bl)) {
		critical = ((struct elem_data *)bl)->critical;
	} else {
		critical = status_get_luk(bl)*3 +- 1;
	}

	if(sc) {
		if(sc->data[SC_FORTUNE].timer != -1 && bl->type != BL_PC)
			critical +-= (10+-sc->data[SC_FORTUNE].val1+-sc->data[SC_FORTUNE].val2+-sc->data[SC_FORTUNE].val3)*10;
		if(sc->data[SC_EXPLOSIONSPIRITS].timer != -1 && bl->type != BL_PC)
			critical +-= sc->data[SC_EXPLOSIONSPIRITS].val2;
		if(sc->data[SC_TRUESIGHT].timer != -1 && bl->type != BL_PC)	// +MMgwpTDrMPwwtTCkMMg-
			critical +-= 10*sc->data[SC_TRUESIGHT].val1;
		if(sc->data[SC__INVISIBILITY].timer != -1 && bl->type != BL_PC)	// +MKQw8zDTMLgw0zDqMMYwow-
			critical +-= critical * (sc->data[SC__INVISIBILITY].val1 * 20) / 100;
		if(sc->data[SC__UNLUCKY].timer != -1 && bl->type != BL_PC)	// +MN4wuTCrMOww/DDJ/xowojDzMOkwwzCtMPw-
			critical -= critical * (sc->data[SC__UNLUCKY].val1 * 10) / 100;
		if(sc->data[SC_STRIKING].timer != -1 && bl->type != BL_PC)	// +MLkwyDDpMKQwrTDzMLA-
			critical +-= 10 * sc->data[SC_STRIKING].val1;
#ifndef PRE_RENEWAL
		if(sc->data[SC_SPEARQUICKEN].timer != -1 && bl->type != BL_PC)   // +MLkw1DCiMK8wpDDDMLEw8w-
			critical +-= 30*sc->data[SC_SPEARQUICKEN].val1;
#endif
		if(sc->data[SC_HANDICAPSTATE_LASSITUDE].timer != -1 && bl->type != BL_PC)	// +cSFsF1Kb-
			critical = 0;
	}
	if(critical < 1) critical = 1;
	return critical;
}

/*==========================================
 * base_atk+MG5T1l+X-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
int status_get_baseatk(struct block_list *bl)
{
	struct status_change *sc;
	int batk = 1;

	nullpo_retr(1, bl);

	sc = status_get_sc(bl);

	if(bl->type == BL_PC) {
		struct map_session_data *sd = (struct map_session_data *)bl;
		if(sd) {
			batk = sd->base_atk;	// +ii1bmjBVMIwwZjBEMIs-base_atk
#ifdef PRE_RENEWAL
			if(sd->status.weapon < WT_MAX) {
				batk +-= sd->weapon_atk[sd->status.weapon];
			}
#endif
		}
	} else if(bl->type == BL_HOM && ((struct homun_data *)bl)) {
#ifdef PRE_RENEWAL
		batk = 1;
#else
		batk = status_get_str(bl) +- status_get_lv(bl) * 2;
#endif
	} else if(bl->type == BL_MERC && ((struct merc_data *)bl)) {
		batk = 1;
	} else if(bl->type == BL_ELEM && ((struct elem_data *)bl)) {
		batk = 1;
	} else {	// +MF0wjE7lWRYwajCJ-
#ifdef PRE_RENEWAL
		int str, dstr;
		str  = status_get_str(bl);	// STR
		dstr = str/10;
		batk = dstr*dstr +- str;	// base_atk+MJKKCHuXMFkwiw-
#else
		batk = status_get_str(bl) +- status_get_lv(bl);	// base_atk+MJKKCHuXMFkwiw-
#endif
	}
	if(sc) {	// +crZhS3VwXjgwQjCK-
#ifdef PRE_RENEWAL
		if(sc->data[SC__BLOODYLUST].timer != -1 && bl->type != BL_PC)	// +MNYw6TDDMMcwozDpMLkwyA-
			batk = batk*(100+-32)/100;	// base_atk+WJdSoA-
		else if(sc->data[SC_PROVOKE].timer != -1 && bl->type != BL_PC)	// PC+MGcw1zDtMNwwwzCv-(SM_PROVOKE)+crZhSw-
			batk = batk*(100+-2+-3*sc->data[SC_PROVOKE].val1)/100;	// base_atk+WJdSoA-
		if(sc->data[SC_CURSE].timer != -1)	// +VGowjzCMMGYwRDBfMIk-
			batk -= batk*25/100;	// base_atk+MEw-25%+bhtcEQ-
		if(sc->data[SC_CONCENTRATION].timer != -1 && bl->type != BL_PC)	// +MLMw8zC7MPMwyDDsMPwwtzDnMPM-
			batk +-= batk*(5*sc->data[SC_CONCENTRATION].val1)/100;
#endif
		if(sc->data[SC_JOINTBEAT].timer != -1 && sc->data[SC_JOINTBEAT].val4 == 4)	// +MLgw5zCkMPMwyDDTMPwwyDBngXA-
			batk -= batk*25/100;
		if(sc->data[SC_MADNESSCANCEL].timer != -1 && bl->type != BL_PC)	// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
			batk +-= 100;
		if(sc->data[SC_THE_MAGICIAN].timer != -1)
			batk = batk*50/100;
		if(sc->data[SC_THE_DEVIL].timer != -1)
			batk = batk*50/100;
		if(sc->data[SC_THE_SUN].timer != -1)
			batk = batk*80/100;
		if(sc->data[SC_SKE].timer != -1 && bl->type == BL_MOB)	// +MKgwuTCv-
			batk *= 4;
		if(sc->data[SC__ENERVATION].timer != -1 && bl->type == BL_MOB)	// +MN4wuTCrMOww/DDJ/xowqDDKMNkw/DC3MOcw8w-
			batk -= batk * (20 +- sc->data[SC__ENERVATION].val1 * 10) / 100;
		if(sc->data[SC_SHIELDSPELL_DEF].timer != -1 && sc->data[SC_SHIELDSPELL_DEF].val2 == 2 && bl->type == BL_MOB)	// +MLcw/DDrMMkwuTDaMOs-(DEF)
			batk +-= sc->data[SC_SHIELDSPELL_DEF].val3;
		if(sc->data[SC_SATURDAY_NIGHT_FEVER].timer != -1 && bl->type == BL_MOB)	// +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw-
			batk +-= 100 * sc->data[SC_SATURDAY_NIGHT_FEVER].val1;
		if(sc->data[SC_ODINS_POWER].timer != -1 && bl->type == BL_MOB)	// +MKow/DDHMKMw8zBuUps-
			batk +-= 60 +- 10 * sc->data[SC_ODINS_POWER].val1;
		if(sc->data[SC_CHATTERING].timer != -1 && bl->type == BL_MOB)	// +MMEw4zC/MOow8zCw-
			batk +-= sc->data[SC_CHATTERING].val2;
	}
	if(batk < 1) batk = 1;	// base_atk+MG9nAE9OMGcwgg-1
	return batk;
}

/*==========================================
 * +W/6MYTBu-Atk+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_atk(struct block_list *bl)
{
	struct status_change *sc;
	int atk = 0;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		atk = ((struct map_session_data*)bl)->watk;
	} else if(bl->type == BL_HOM && ((struct homun_data *)bl)) {
		atk = ((struct homun_data *)bl)->atk-((struct homun_data *)bl)->atk/10;
	} else if(bl->type == BL_MERC && ((struct merc_data *)bl)) {
		atk = ((struct merc_data *)bl)->atk1;
	} else if(bl->type == BL_ELEM && ((struct elem_data *)bl)) {
		atk = ((struct elem_data *)bl)->atk1;
	} else if(bl->type == BL_MOB) {
		struct mob_data *md = (struct mob_data *)bl;
		if(md) {
			int guardup_lv = md->guardup_lv;
#ifndef PRE_RENEWAL
			if(battle_config.monster_atk2_to_matk)
				atk = md->st.atk * 8 / 10;
			else
#endif
				atk = md->st.atk;
			if(guardup_lv > 0)
				atk +-= 1000*guardup_lv;
		}
	} else if(bl->type == BL_PET && (struct pet_data *)bl) {
		atk = mobdb_search(((struct pet_data*)bl)->class_)->atk1;
	}

	if(sc) {
		int rate = 100;
#ifdef PRE_RENEWAL
		if(sc->data[SC__BLOODYLUST].timer != -1 && bl->type != BL_PC)
			atk = atk*(100+-32)/100;
		else if(sc->data[SC_PROVOKE].timer != -1 && bl->type != BL_PC)
			atk = atk*(100+-2+-3*sc->data[SC_PROVOKE].val1)/100;
		if(sc->data[SC_CURSE].timer != -1)
			atk -= atk*25/100;
		if(sc->data[SC_CONCENTRATION].timer != -1 && bl->type != BL_PC)	// +MLMw8zC7MPMwyDDsMPwwtzDnMPM-
			atk +-= atk*(5*sc->data[SC_CONCENTRATION].val1)/100;
		if(sc->data[SC_EXPLOSIONSPIRITS].timer != -1 && bl->type != BL_PC)	// NPC+cgaIwmziUtU-
			rate +-= 200;
		if(sc->data[SC_SKE].timer != -1 && bl->type == BL_MOB)		// +MKgwuTCv-
			rate +-= 300;
#endif
		if(sc->data[SC_STRIPWEAPON].timer != -1 && bl->type != BL_PC)
			atk -= atk*25/100;
		if(sc->data[SC_MADNESSCANCEL].timer != -1 && bl->type != BL_PC)	// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
			atk +-= 100;
		if(sc->data[SC_THE_MAGICIAN].timer != -1 && bl->type != BL_PC)
			atk = atk*50/100;
		if(sc->data[SC_THE_DEVIL].timer != -1 && bl->type != BL_PC)
			atk = atk*50/100;
		if(sc->data[SC_THE_SUN].timer != -1 && bl->type != BL_PC)
			atk = atk*80/100;
		if(sc->data[SC_SKE].timer != -1 && bl->type == BL_MOB)		// +MKgwuTCv-
			rate +-= 300;
		if(sc->data[SC__ENERVATION].timer != -1 && bl->type == BL_MOB)	// +MN4wuTCrMOww/DDJ/xowqDDKMNkw/DC3MOcw8w-
			atk -= atk * (20 +- sc->data[SC__ENERVATION].val1 * 10) / 100;
		if(sc->data[SC_SHIELDSPELL_DEF].timer != -1 && sc->data[SC_SHIELDSPELL_DEF].val2 == 2 && bl->type == BL_MOB)	// +MLcw/DDrMMkwuTDaMOs-(DEF)
			atk +-= sc->data[SC_SHIELDSPELL_DEF].val3;
		if(sc->data[SC_SATURDAY_NIGHT_FEVER].timer != -1 && bl->type == BL_MOB)	// +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw-
			atk +-= 100 * sc->data[SC_SATURDAY_NIGHT_FEVER].val1;
		if(sc->data[SC_ODINS_POWER].timer != -1 && bl->type == BL_MOB)	// +MKow/DDHMKMw8zBuUps-
			atk +-= 60 +- 10 * sc->data[SC_ODINS_POWER].val1;
		if(sc->data[SC_CATNIPPOWDER].timer != -1 && bl->type != BL_PC)		// +MKQwzDDPMMMwqzC3MOMw7zD8-
			atk -= atk * sc->data[SC_CATNIPPOWDER].val2 / 100;
		if(sc->data[SC_SHRIMP].timer != -1)		// +MKgw004JZic-
			atk +-= atk * sc->data[SC_SHRIMP].val3 / 100;
		if(sc->data[SC_EQC].timer != -1 && bl->type == BL_MOB)		// E.Q.C
			atk -= atk * sc->data[SC_EQC].val4 / 100;
		if(sc->data[SC_VOLCANIC_ASH].timer != -1 && sc->data[SC_VOLCANIC_ASH].val4 > 0) 	// +cGtccXBw-
			atk -= atk * sc->data[SC_VOLCANIC_ASH].val4 / 100;
		if(rate != 100)	// NPC+cgaIwmziUtUwaDCoMLkwrzCSUA1zh1Kge5cwVTBbMIs-
			atk = atk * rate / 100;
	}
	if(atk < 0) atk = 0;
	return atk;
}

/*==========================================
 * +W/6MYTBuXeZiSw-Atk+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_atk_(struct block_list *bl)
{
	struct map_session_data *sd = NULL;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (sd = (struct map_session_data *)bl)) {
		int atk = sd->watk_;

#ifdef PRE_RENEWAL
		if(sd->sc.data[SC_CURSE].timer != -1)
			atk -= atk*25/100;
#endif
		if(sd->sc.data[SC_SHRIMP].timer != -1)		// +MKgw004JZic-
			atk +-= atk * sd->sc.data[SC_SHRIMP].val3 / 100;
		return atk;
	}
	return 0;
}

/*==========================================
 * +W/6MYTBu-Atk2+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_atk2(struct block_list *bl)
{
	int atk2 = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		atk2 = ((struct map_session_data*)bl)->watk2;
	} else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		atk2 = ((struct homun_data *)bl)->atk;
	} else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		atk2 = ((struct merc_data *)bl)->atk2;
	} else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		atk2 = ((struct elem_data *)bl)->atk2;
	} else {
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB) {
			struct mob_data *md = (struct mob_data *)bl;
			if(md) {
				int guardup_lv = md->guardup_lv;
#ifndef PRE_RENEWAL
				if(battle_config.monster_atk2_to_matk)
					atk2 = md->st.atk * 12 / 10;
				else
#endif
					atk2 = md->st.matk;
				if(guardup_lv > 0)
					atk2 +-= 1000*guardup_lv;
			}
		} else if(bl->type == BL_PET && (struct pet_data *)bl) {
			atk2 = mobdb_search(((struct pet_data*)bl)->class_)->atk2;
		}
		if(sc) {
			int rate = 100;
#ifdef PRE_RENEWAL
			if(sc->data[SC_IMPOSITIO].timer != -1)
				atk2 +-= sc->data[SC_IMPOSITIO].val1*5;
			if(sc->data[SC__BLOODYLUST].timer != -1)
				atk2 = atk2*(100+-32)/100;
			else if(sc->data[SC_PROVOKE].timer != -1)
				atk2 = atk2*(100+-2+-3*sc->data[SC_PROVOKE].val1)/100;
			if(sc->data[SC_CURSE].timer != -1)
				atk2 -= atk2*25/100;
			if(sc->data[SC_DRUMBATTLE].timer != -1)
				atk2 +-= sc->data[SC_DRUMBATTLE].val2;
			if(sc->data[SC_NIBELUNGEN].timer != -1 && (status_get_element(bl)/10) >= 8 )
				atk2 +-= sc->data[SC_NIBELUNGEN].val2;
#endif
			if(sc->data[SC_STRIPWEAPON].timer != -1)
				atk2 -= atk2*25/100;
#ifdef PRE_RENEWAL
			if(sc->data[SC_CONCENTRATION].timer != -1)	// +MLMw8zC7MPMwyDDsMPwwtzDnMPM-
				atk2 +-= atk2*(5*sc->data[SC_CONCENTRATION].val1)/100;
			if(sc->data[SC_EXPLOSIONSPIRITS].timer != -1 && bl->type != BL_PC)	// NPC+cgaIwmziUtU-
				rate +-= 200;
			if(sc->data[SC_SKE].timer != -1 && bl->type == BL_MOB)		// +MKgwuTCv-
				rate +-= 300;
#endif
			if(sc->data[SC_MADNESSCANCEL].timer != -1 && bl->type != BL_PC)	// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
				atk2 +-= 100;
			if(sc->data[SC_THE_MAGICIAN].timer != -1 && bl->type != BL_PC)
				atk2 = atk2*50/100;
			if(sc->data[SC_THE_DEVIL].timer != -1 && bl->type != BL_PC)
				atk2 = atk2*50/100;
			if(sc->data[SC_THE_SUN].timer != -1 && bl->type != BL_PC)
				atk2 = atk2*80/100;
			if(sc->data[SC__ENERVATION].timer != -1 && bl->type == BL_MOB)	// +MN4wuTCrMOww/DDJ/xowqDDKMNkw/DC3MOcw8w-
				atk2 -= atk2 * (20 +- sc->data[SC__ENERVATION].val1 * 10) / 100;
			if(sc->data[SC_SHIELDSPELL_DEF].timer != -1 && sc->data[SC_SHIELDSPELL_DEF].val2 == 2 && bl->type == BL_MOB)	// +MLcw/DDrMMkwuTDaMOs-(DEF)
				atk2 +-= sc->data[SC_SHIELDSPELL_DEF].val3;
			if(sc->data[SC_SATURDAY_NIGHT_FEVER].timer != -1 && bl->type == BL_MOB)	// +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw-
				atk2 +-= 100 * sc->data[SC_SATURDAY_NIGHT_FEVER].val1;
			if(sc->data[SC_ODINS_POWER].timer != -1 && bl->type == BL_MOB)	// +MKow/DDHMKMw8zBuUps-
				atk2 +-= 60 +- 10 * sc->data[SC_ODINS_POWER].val1;
			if(sc->data[SC_CATNIPPOWDER].timer != -1 && bl->type != BL_PC)		// +MKQwzDDPMMMwqzC3MOMw7zD8-
				atk2 -= atk2 * sc->data[SC_CATNIPPOWDER].val2 / 100;
			if(rate != 100)	// NPC+cgaIwmziUtUwaDCoMLkwrzCSUA1zh1Kge5cwVTBbMIs-
				atk2 = atk2 * rate / 100;
		}
		if(atk2 < 0) atk2 = 0;
	}
	return atk2;
}

/*==========================================
 * +W/6MYTBuXeZiSw-Atk2+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_atk_2(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		return ((struct map_session_data*)bl)->watk_2;

	return 0;
}

/*==========================================
 * +W/6MYTBu-MAtk1+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_matk1(struct block_list *bl)
{
	int matk1 = 0;

	nullpo_retr(0, bl);

	if (bl->type == BL_PC) {
		matk1 = ((struct map_session_data *)bl)->matk1;
	} else if (bl->type == BL_HOM) {
#ifdef PRE_RENEWAL
		matk1 = ((struct homun_data *)bl)->matk-((struct homun_data *)bl)->matk/10;
#else
		matk1 = ((struct homun_data *)bl)->matk;
#endif
	} else if (bl->type == BL_MERC) {
		matk1 = ((struct merc_data *)bl)->matk1;
	} else if (bl->type == BL_ELEM) {
		matk1 = ((struct elem_data *)bl)->matk1;
	} else if (!(bl->type & (BL_PET | BL_MOB))) {
		matk1 = 0;
	} else {
		struct status_change *sc = status_get_sc(bl);
		int int_ = status_get_int(bl);

#ifndef PRE_RENEWAL
		if(battle_config.monster_atk2_to_matk && bl->type == BL_MOB)
			matk1 = ((struct mob_data *)bl)->st.matk * 13 / 10 +- int_ +- status_get_lv(bl);
		else
#endif
			matk1 = int_+-(int_/5)*(int_/5);

		// MOB+MG4-max_sp+UCQwkg-MATK+iNxrY1AkMGdOVzBjU9Ywi2ZC-
		if(battle_config.mob_take_over_sp == 1) {
			if(bl->type == BL_MOB) {
				int b_class = status_get_class(bl);		// +dvRjpQ-maxsp+U9Zfl3EhdAYwbnC6MOAw6jDkMOo-
				if(mobdb_search(b_class)->max_sp > 0) {		// 1+TuVOCjBuZkIwbjB/-
					matk1 = matk1 * (mobdb_search(b_class)->max_sp/100);
				}
			}
		}

		if(sc) {
#ifdef PRE_RENEWAL
			if(sc->data[SC_MINDBREAKER].timer != -1)
				matk1 +-= (matk1*20*sc->data[SC_MINDBREAKER].val1)/100;
#endif
			if(sc->data[SC_STRENGTH].timer != -1)
				matk1 = matk1*50/100;
			if(sc->data[SC_THE_DEVIL].timer != -1)
				matk1 = matk1*50/100;
			if(sc->data[SC_THE_SUN].timer != -1)
				matk1 = matk1*80/100;
			if(sc->data[SC_ODINS_POWER].timer != -1)	// +MKow/DDHMKMw8zBuUps-
				matk1 +-= 60 +- 10 * sc->data[SC_ODINS_POWER].val1;
#ifdef PRE_RENEWAL
			if(sc->data[SC_CHATTERING].timer != -1)	// +MMEw4zC/MOow8zCw-
				matk1 +-= sc->data[SC_CHATTERING].val2;
#endif
			if(sc->data[SC_SHRIMP].timer != -1)		// +MKgw004JZic-
				matk1 +-= matk1 * sc->data[SC_SHRIMP].val3 / 100;
			if(sc->data[SC_CATNIPPOWDER].timer != -1)		// +MKQwzDDPMMMwqzC3MOMw7zD8-
				matk1 -= matk1 * sc->data[SC_CATNIPPOWDER].val2 / 100;
		}
	}
	return matk1;
}

/*==========================================
 * +W/6MYTBu-MAtk2+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_matk2(struct block_list *bl)
{
	int matk2 = 0;

	nullpo_retr(0, bl);

	if (bl->type == BL_PC) {
		matk2 = ((struct map_session_data *)bl)->matk2;
	} else if (bl->type == BL_HOM) {
#ifdef PRE_RENEWAL
		matk2 = ((struct homun_data *)bl)->matk;
#else
		matk2 = ((struct homun_data *)bl)->matk-((struct homun_data *)bl)->matk/10;
#endif
	} else if (bl->type == BL_MERC) {
		matk2 = ((struct merc_data *)bl)->matk2;
	} else if (bl->type == BL_ELEM) {
		matk2 = ((struct elem_data *)bl)->matk2;
	} else if (!(bl->type & (BL_PET | BL_MOB))) {
		matk2 = 0;
	} else {
		struct status_change *sc = status_get_sc(bl);
		int int_ = status_get_int(bl);

#ifndef PRE_RENEWAL
		if(battle_config.monster_atk2_to_matk && bl->type == BL_MOB)
			matk2 = ((struct mob_data *)bl)->st.matk * 7 / 10 +- int_ +- status_get_lv(bl);
		else
#endif
			matk2 = int_+-(int_/7)*(int_/7);

		// MOB+MG4-max_sp+UCQwkg-MATK+iNxrY1AkMGdOVzBjU9Ywi2ZC-
		if(battle_config.mob_take_over_sp == 1) {
			if(bl->type == BL_MOB) {
				int b_class = status_get_class(bl);		// +dvRjpQ-maxsp+U9Zfl3EhdAYwbnC6MOAw6jDkMOo-
				if(mobdb_search(b_class)->max_sp > 0) {		// 1+TuVOCjBuZkIwbjB/-
					matk2 = matk2 * (mobdb_search(b_class)->max_sp/100);
				}
			}
		}

		if(sc) {
#ifdef PRE_RENEWAL
			if(sc->data[SC_MINDBREAKER].timer != -1)
				matk2 +-= (matk2*20*sc->data[SC_MINDBREAKER].val1)/100;
#endif
			if(sc->data[SC_STRENGTH].timer != -1)
				matk2 = matk2*50/100;
			if(sc->data[SC_THE_DEVIL].timer != -1)
				matk2 = matk2*50/100;
			if(sc->data[SC_THE_SUN].timer != -1)
				matk2 = matk2*80/100;
			if(sc->data[SC_ODINS_POWER].timer != -1)	// +MKow/DDHMKMw8zBuUps-
				matk2 +-= 60 +- 10 * sc->data[SC_ODINS_POWER].val1;
			if(sc->data[SC_CHATTERING].timer != -1)	// +MMEw4zC/MOow8zCw-
				matk2 +-= sc->data[SC_CHATTERING].val2;
			if(sc->data[SC_CATNIPPOWDER].timer != -1)		// +MKQwzDDPMMMwqzC3MOMw7zD8-
				matk2 -= matk2 * sc->data[SC_CATNIPPOWDER].val2 / 100;
		}
	}
	return matk2;
}

/*==========================================
 * +W/6MYTBu-Def+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_def(struct block_list *bl)
{
	struct unit_data *ud= NULL;
	struct status_change *sc;
	int def;

	nullpo_retr(0, bl);

	ud = unit_bl2ud(bl);
	sc = status_get_sc(bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		def = ((struct map_session_data *)bl)->def;
	} else if(bl->type == BL_MOB && (struct mob_data *)bl) {
		def = ((struct mob_data *)bl)->st.def;
	} else if(bl->type == BL_PET && (struct pet_data *)bl) {
		def = mobdb_search(((struct pet_data *)bl)->class_)->def;
	} else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		def = ((struct merc_data *)bl)->def;
	} else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		def = ((struct elem_data *)bl)->def;
	} else {
		def = 0;
	}

	if(def < 1000000) {
		if(sc) {
			// +MK0w/DDUMPMwsGZCMG8-DEF100
			if(sc->data[SC_KEEPING].timer != -1)
				def *= 2;
			// +MNcw7TDcMMMwr2ZCMG9uG3uX-
			if(sc->data[SC__BLOODYLUST].timer != -1 && bl->type != BL_PC)
				def = def * (100 - 55) / 100;
			else if(sc->data[SC_PROVOKE].timer != -1 && bl->type != BL_PC)
				def = def * (100 - 5 - 5 * sc->data[SC_PROVOKE].val1) / 100;
			// +YiZZKp8TMG6X/zBNZkIwb1Kge5c-
			if(sc->data[SC_DRUMBATTLE].timer != -1 && bl->type != BL_PC)
				def +-= sc->data[SC_DRUMBATTLE].val3;
			// +a9IwazBLMEswYzBmMEQwi2ZCMG9uG3uX-
			if(sc->data[SC_POISON].timer != -1 && bl->type != BL_PC)
				def = def*75/100;
			// +MLkwyDDqMMMw1zC3MPww6zDJZkIwb24be5c-
			if(sc->data[SC_STRIPSHIELD].timer != -1 && bl->type != BL_PC)
				def = def*85/100;
			// +MLcwsDDKMOAwrzDrMLcwuWZCMG9uG3uX-
			if(sc->data[SC_SIGNUMCRUCIS].timer != -1 && bl->type != BL_PC)
				def = def * (100 - sc->data[SC_SIGNUMCRUCIS].val2)/100;
			// +bDiQYDBubfdsjGZCMG8-PC+TuVZFg-DEF+MEw-0+MGswajCL-
			if(sc->data[SC_ETERNALCHAOS].timer != -1 && bl->type != BL_PC)
				def = 0;
			// +Uc19UDABd/NTFmZCMG9T8zC3MNUwyA-
			if(sc->data[SC_FREEZE].timer != -1 || (sc->data[SC_STONE].timer != -1 && sc->data[SC_STONE].val2 == 0))
				def >>= 1;
			// +MLMw8zC7MPMwyDDsMPwwtzDnMPNmQjBvbht7lw-
			if( sc->data[SC_CONCENTRATION].timer != -1 && bl->type != BL_PC)
				def = def * (100 - 5*sc->data[SC_CONCENTRATION].val1) / 100;
			// NPC+MMcwozDVMKcw8zDAMPw-
			if(sc->data[SC_NPC_DEFENDER].timer != -1 && bl->type != BL_PC)
				def +-= 100;
			// THE SUN
			if(sc->data[SC_THE_SUN].timer != -1 && bl->type != BL_PC)
				def = def*80/100;
			// +MNUw6TCkMPMwsA-
			if(sc->data[SC_FLING].timer != -1 && bl->type != BL_PC)
				def = def * (100 - 5*sc->data[SC_FLING].val2)/100;
			// +MKgwuTCv-
			if(sc->data[SC_SKE].timer != -1 && bl->type == BL_MOB)
				def = def/2;
#ifdef PRE_RENEWAL
			// +MLkwyDD8MPMwuTCtMPM-
			if(sc->data[SC_STONESKIN].timer != -1 && bl->type != BL_PC)
				def = def * (100 +- 20 * sc->data[SC_STONESKIN].val1) / 100;
			// +MKIw8zDBMN4wuDDDMK8-
			if(sc->data[SC_ANTIMAGIC].timer != -1 && bl->type != BL_PC)
				def = def * (100 - 20 * sc->data[SC_ANTIMAGIC].val1) / 100;
#endif
			// +MNUw7TC5MMgw3zC5MMYwow-
			if(sc->data[SC_FROSTMISTY].timer != -1 && bl->type != BL_PC)
				def = def * 90 / 100;
			// +MKIwyjDpMKQwug-
			if(sc->data[SC_ANALYZE].timer != -1 && bl->type != BL_PC)
				def = def * (100 - 14 * sc->data[SC_ANALYZE].val1) / 100;
			// +MMsw5TD8MMgw6TDrMNAw6jCiMPw-
			if(sc->data[SC_NEUTRALBARRIER].timer != -1 && bl->type != BL_PC)
				def = def * (110 +- 5 * sc->data[SC_NEUTRALBARRIER].val1) / 100;
			// +MKIw/DC5MMkw6TCkMNY-
			if(sc->data[SC_EARTHDRIVE].timer != -1 && bl->type != BL_PC)
				def = def * 85 / 100;
			// +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw-
			if(sc->data[SC_SATURDAY_NIGHT_FEVER].timer != -1 && bl->type != BL_PC)
				def = def * (90 - 10 * sc->data[SC_SATURDAY_NIGHT_FEVER].val1) / 100;
			// +MKow/DDHMKMw8zBuUps-
			if(sc->data[SC_ODINS_POWER].timer != -1 && bl->type != BL_PC)
				def -= 10 +- 10 * sc->data[SC_ODINS_POWER].val1;
			// +MMsw4zDzMLAw6TC5-
			if(sc->data[SC_NYANGGRASS].timer != -1 && bl->type != BL_PC)
				def >>= 1;
			// +cGtccXBw-
			if(sc->data[SC_VOLCANIC_ASH].timer != -1 && sc->data[SC_VOLCANIC_ASH].val3 > 0)
				def -= def * sc->data[SC_VOLCANIC_ASH].val3 / 100;
			// +YCVRtw-
			if(sc->data[SC_HANDICAPSTATE_FROSTBITE].timer != -1 && bl->type != BL_PC)
				def -= def * sc->data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
			// +MKIw8zDqMN8wwzDI-
			if(sc->data[SC_UNLIMIT].timer != -1)
				def = 1;
			// +Xzdr0g-
			if(sc->data[SC_HANDICAPSTATE_DEADLYPOISON].timer != -1 && bl->type != BL_PC)
				def = 0;
		}
		// +imBVMU4tMG+KYFUxZkJuG3uXc4cwa1f6MGUwRDBmbht7lw-
		if(ud && ud->skilltimer != -1) {
			int def_rate = skill_get_castdef(ud->skillid);
			if(def_rate != 0)
				def = (def * (100 - def_rate))/100;
		}
	}
	if(def < 0) def = 0;
	return def;
}

/*==========================================
 * +W/6MYTBu-MDef+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_mdef(struct block_list *bl)
{
	struct status_change *sc;
	int mdef = 0;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_PC && (struct map_session_data *)bl)
		mdef = ((struct map_session_data *)bl)->mdef;
	else if(bl->type == BL_MOB && (struct mob_data *)bl)
		mdef = ((struct mob_data *)bl)->st.mdef;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		mdef = ((struct homun_data *)bl)->mdef;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		mdef = ((struct merc_data *)bl)->mdef;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		mdef = ((struct elem_data *)bl)->mdef;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		mdef = mobdb_search(((struct pet_data *)bl)->class_)->mdef;

	if(mdef < 1000000) {
		if(sc) {
			// +MNAw6jCiMPxytmFLZkIwbw-MDEF100
			if(sc->data[SC_BARRIER].timer != -1)
				mdef +-= 100;
			// +Uc19UDABd/NTFmZCMG8-1.25+UA0-
			if(sc->data[SC_FREEZE].timer != -1 || (sc->data[SC_STONE].timer != -1 && sc->data[SC_STONE].val2 == 0))
				mdef = mdef*125/100;
#ifdef PRE_RENEWAL
			// +MLkwyDD8MPMwuTCtMPM-
			if(sc->data[SC_STONESKIN].timer != -1 && bl->type != BL_PC)
				mdef = mdef * (100 - 20 * sc->data[SC_STONESKIN].val1) / 100;
			// +MKIw8zDBMN4wuDDDMK8-
			if(sc->data[SC_ANTIMAGIC].timer != -1 && bl->type != BL_PC)
				mdef = mdef * (100 +- 20 * sc->data[SC_ANTIMAGIC].val1) / 100;
#endif
			// +MKIwyjDpMKQwug-
			if(sc->data[SC_ANALYZE].timer != -1 && bl->type != BL_PC)
				mdef = mdef * (100 - 14 * sc->data[SC_ANALYZE].val1) / 100;
			// +MMsw5TD8MMgw6TDrMNAw6jCiMPw-
			if(sc->data[SC_NEUTRALBARRIER].timer != -1 && bl->type != BL_PC)
				mdef = mdef * (110 +- 5 * sc->data[SC_NEUTRALBARRIER].val1) / 100;
			// +MKow/DDHMKMw8zBuUps-
			if(sc->data[SC_ODINS_POWER].timer != -1 && bl->type != BL_PC)
				mdef -= 10 +- 10 * sc->data[SC_ODINS_POWER].val1;
			// +cLl6dA- -+U80--
			if(sc->data[SC_GENTLETOUCH_CHANGE].timer != -1)
				mdef -= sc->data[SC_GENTLETOUCH_CHANGE].val3;
			// +MMsw4zDzMLAw6TC5-
			if(sc->data[SC_NYANGGRASS].timer != -1 && bl->type != BL_PC)
				mdef >>= 1;
			// +YCVRtw-
			if(sc->data[SC_HANDICAPSTATE_FROSTBITE].timer != -1 && bl->type != BL_PC)
				mdef -= mdef * sc->data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
			// +MKIw8zDqMN8wwzDI-
			if(sc->data[SC_UNLIMIT].timer != -1)
				mdef = 1;
			// +fVBmdlMW-
			if(sc->data[SC_HANDICAPSTATE_CRYSTALLIZATION].timer != -1 && bl->type != BL_PC)
				mdef = 0;
		}
	}
	if(mdef < 0) mdef = 0;
	return mdef;
}

/*==========================================
 * +W/6MYTBu-Def2+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 *------------------------------------------
 */
int status_get_def2(struct block_list *bl)
{
	struct status_change *sc;
	int def2 = 1;

	nullpo_retr(1, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_PC && (struct map_session_data *)bl)
		def2 = ((struct map_session_data *)bl)->def2;
	else if(bl->type == BL_MOB && (struct mob_data *)bl)
#ifdef PRE_RENEWAL
		def2 = mobdb_search(((struct mob_data *)bl)->class_)->vit;
#else
		def2 = (int)(mobdb_search(((struct mob_data *)bl)->class_)->vit/(float)2 +- mobdb_search(((struct mob_data *)bl)->class_)->lv/(float)2);
#endif
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		def2 = mobdb_search(((struct pet_data *)bl)->class_)->vit;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		def2 = ((struct homun_data *)bl)->def;
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		def2 = ((struct merc_data *)bl)->vit;
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		def2 = ((struct elem_data *)bl)->vit;

	if(sc) {
		if(sc->data[SC_ANGELUS].timer != -1 && bl->type != BL_PC) {
#ifdef PRE_RENEWAL
			def2 = def2*(110+-5*sc->data[SC_ANGELUS].val1)/100;
#else
			def2 = def2*(100+-5*sc->data[SC_ANGELUS].val1)/100;
#endif
		}
		if(sc->data[SC__BLOODYLUST].timer != -1 && bl->type != BL_PC)
			def2 = def2 * (100 - 55) / 100;
		else if(sc->data[SC_PROVOKE].timer != -1 && bl->type != BL_PC)
			def2 = def2 * (100 - 5 - 5 * sc->data[SC_PROVOKE].val1) / 100;
		if(sc->data[SC_POISON].timer != -1 && bl->type != BL_PC)
			def2 = def2*75/100;
		// +MLMw8zC7MPMwyDDsMPwwtzDnMPNmQjBvbht7lw-
		if( sc->data[SC_CONCENTRATION].timer != -1 && bl->type != BL_PC)
			def2 = def2*(100 - 5*sc->data[SC_CONCENTRATION].val1)/100;
		// +MLgw5zCkMPMwyDDTMPwwyGZCMGowiW4be5c-
		if(sc->data[SC_JOINTBEAT].timer != -1) {
			if(sc->data[SC_JOINTBEAT].val4 == 3)	// +gKk-
				def2 -= def2*50/100;
			if(sc->data[SC_JOINTBEAT].val4 == 4)	// +gXA-
				def2 -= def2*25/100;
		}
		// +bDiQYDBubfdsjGZCMG8-DEF2+MEw-0+MGswajCL-
		if(sc->data[SC_ETERNALCHAOS].timer != -1)
			def2 = 0;
		// THE SUN
		if(sc->data[SC_THE_SUN].timer != -1 && bl->type != BL_PC)
			def2 = def2*80/100;
		// +MNUw6TCkMPMwsA-
		if(sc->data[SC_FLING].timer != -1 && bl->type != BL_PC)
			def2 = def2 * (100 - 5*sc->data[SC_FLING].val2)/100;
		// +MKgwuTCr-
		if(sc->data[SC_SKA].timer != -1 && bl->type == BL_MOB)
			def2 +-= 90;
		// +MLcw/DDrMMkwuTDaMOs-
		if(sc->data[SC_SHIELDSPELL_REF].timer != -1 && sc->data[SC_SHIELDSPELL_REF].val2 == 2 && bl->type == BL_MOB)
			def2 +-= sc->data[SC_SHIELDSPELL_REF].val2;
		// +MNcw7DC5MMYwozD8MLg-
		if(sc->data[SC_PRESTIGE].timer != -1 && bl->type == BL_MOB)
			def2 +-= sc->data[SC_PRESTIGE].val2;
		// +YCVRtw-
		if(sc->data[SC_HANDICAPSTATE_FROSTBITE].timer != -1 && bl->type != BL_PC)
			def2 -= def2 * sc->data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
		// +MKIw8zDqMN8wwzDI-
		if(sc->data[SC_UNLIMIT].timer != -1)
			def2 = 1;
		// E.Q.C
		if(sc->data[SC_EQC].timer != -1 && bl->type == BL_MOB)
			def2 -= def2 * sc->data[SC_EQC].val4 / 100;
		// +Xzdr0g-
		if(sc->data[SC_HANDICAPSTATE_DEADLYPOISON].timer != -1 && bl->type != BL_PC)
			def2 = 0;
	}
	if(def2 < 1) def2 = 1;
	return def2;
}

/*==========================================
 * +W/6MYTBu-MDef2+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-0+TuVOCg-
 *------------------------------------------
 */
int status_get_mdef2(struct block_list *bl)
{
	struct status_change *sc;
	int mdef2 = 0;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
#ifdef PRE_RENEWAL
		mdef2 = mobdb_search(((struct mob_data *)bl)->class_)->int_ +- (mobdb_search(((struct mob_data *)bl)->class_)->vit>>1);
#else
		mdef2 = (int)(mobdb_search(((struct mob_data *)bl)->class_)->int_/(float)4 +- mobdb_search(((struct mob_data *)bl)->class_)->lv/(float)4);
#endif
	else if(bl->type == BL_PC && (struct map_session_data *)bl)
#ifdef PRE_RENEWAL
		mdef2 = ((struct map_session_data *)bl)->mdef2 +- (((struct map_session_data *)bl)->paramc[2]>>1);
#else
		mdef2 = ((struct map_session_data *)bl)->mdef2;
#endif
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		mdef2 = mobdb_search(((struct pet_data *)bl)->class_)->int_ +- (mobdb_search(((struct pet_data *)bl)->class_)->vit>>1);
	else if (bl->type == BL_HOM && (struct homun_data *)bl)
		mdef2 = ((struct homun_data *)bl)->mdef;
	else if (bl->type == BL_MERC && (struct merc_data *)bl)
		mdef2 = ((struct merc_data *)bl)->int_ +- (((struct merc_data *)bl)->vit>>1);
	else if (bl->type == BL_ELEM && (struct elem_data *)bl)
		mdef2 = ((struct elem_data *)bl)->int_ +- (((struct elem_data *)bl)->vit>>1);

	if(sc) {
		// +MKgwuTCr-
		if(sc->data[SC_SKA].timer != -1 && bl->type == BL_MOB)
			mdef2 = 90;
		// +MN4wpDDzMMkw1jDsMKQwqzD8-
		if(sc->data[SC_MINDBREAKER].timer != -1 && bl->type != BL_PC)
			mdef2 -= (mdef2*12*sc->data[SC_MINDBREAKER].val1)/100;
		// +YCVRtw-
		if(sc->data[SC_HANDICAPSTATE_FROSTBITE].timer != -1 && bl->type != BL_PC)
			mdef2 -= mdef2 * sc->data[SC_HANDICAPSTATE_FROSTBITE].val2 / 100;
		// +MKIw8zDqMN8wwzDI-
		if(sc->data[SC_UNLIMIT].timer != -1)
			mdef2 = 1;
		// +fVBmdlMW-
		if(sc->data[SC_HANDICAPSTATE_CRYSTALLIZATION].timer != -1 && bl->type != BL_PC)
			mdef2 = 0;
	}
	return mdef2;
}

/*==========================================
 * +W/6MYTBu-Speed(+eftS1ZAfXqY-)+MJKP1DBZ-
 * +YjswijBvZXRlcDBn-1+TuVOCg-
 * Speed+MG9cDzBVMEQwezBGMEx5+1LVkB9epjBMkB8wRA-
 *------------------------------------------
 */
int status_get_speed(struct block_list *bl)
{
	int speed = 1000;

	nullpo_retr(1000, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		speed = ((struct map_session_data *)bl)->speed;
	} else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		if(battle_config.homun_speed_is_same_as_pc)
			speed = ((struct homun_data *)bl)->msd->speed;
		else
			speed = ((struct homun_data *)bl)->speed;
	} else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		if(battle_config.merc_speed_is_same_as_pc)
			speed = ((struct merc_data *)bl)->msd->speed;
		else
			speed = ((struct merc_data *)bl)->speed;
	} else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		speed = ((struct elem_data *)bl)->speed;
 	} else if(bl->type == BL_NPC && (struct npc_data *)bl) {
		speed = ((struct npc_data *)bl)->speed;
	} else {
		int bonus_rate;
		int haste_val  = 0;
		int slow_val   = 0;
		char defender_flag = 0;
		char walkspeed_flag = 0;
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB && (struct mob_data *)bl)
			speed = ((struct mob_data *)bl)->speed;
		else if(bl->type == BL_PET && (struct pet_data *)bl)
			speed = ((struct pet_data *)bl)->speed;

		/* speed+MExZCVMWMFkwizC5MMYw/DC/MLkwbooIe5c- */
		if(sc) {
			/* speed+MExYl1KgMFkwizC5MMYw/DC/MLkwbooIe5c- */

			// +kdFSWzBveftS1ZAfXqZW+lua-
			if(sc->data[SC_STEELBODY].timer != -1)
				return 200;

			// +kB9epm4bXBE-
			if(sc->data[SC_DECREASEAGI].timer != -1)
				slow_val = 25;

			// +MK8woTCwMN4wpDCi-
			if(sc->data[SC_QUAGMIRE].timer != -1) {
				if(slow_val < 50)
					slow_val = 50;
			}

			// +ecEwkl/YMIwwajBEMGc-
			if(sc->data[SC_DONTFORGETME].timer != -1) {
				if(slow_val < sc->data[SC_DONTFORGETME].val2)
					slow_val = sc->data[SC_DONTFORGETME].val2;
			}

			// +VGowRA-
			if(sc->data[SC_CURSE].timer != -1) {
				if(slow_val < 300)
					slow_val = 300;
			}

			// +MN4w/DC3MOUwqjDWMKIw0zC5-
			if(sc->data[SC_MARSHOFABYSS].timer != -1) {
				if(slow_val < sc->data[SC_MARSHOFABYSS].val2)
					slow_val = sc->data[SC_MARSHOFABYSS].val2;
			}

			// +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-(+MNowyjDrMMYwow-)
			if(sc->data[SC_HALLUCINATIONWALK_POSTDELAY].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +MLgw5zCkMPMwyDDTMPwwyA-
			if(sc->data[SC_JOINTBEAT].timer != -1) {
				int penalty = 0;
				switch (sc->data[SC_JOINTBEAT].val4) {
				case 0:	// +jbOZlg-
					penalty = 50;
					break;
				case 2:	// +gZ0-
					penalty = 30;
					break;
				}
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
			if(sc->data[SC_GATLINGFEVER].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +MKgwuTCm-
			if(sc->data[SC_SWOO].timer != -1) {
				if(slow_val < 300)
					slow_val = 300;
			}

			// +MNEw6TDpMKQwug-
			if(sc->data[SC_PARALIZE].timer != -1) {
				if(slow_val < 100)
					slow_val = 100;
			}

			// +MNUw7TC5MMgw3zC5MMYwow-
			if(sc->data[SC_FROSTMISTY].timer != -1) {
				if(slow_val < 50)
					slow_val = 50;
			}

			// +MN4wuTCrMOww/DDJ/xow7DCkMLgwzTC5-
			if(sc->data[SC__LAZINESS].timer != -1) {
				if(slow_val < 10)
					slow_val = 10;
			}

			// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
			if(sc->data[SC__GROOMY].timer != -1) {
				int penalty = 5 +- 5 * sc->data[SC__GROOMY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MOEw7TDzcgZfPg-
			if(sc->data[SC_MELON_BOMB].timer != -1) {
				int penalty = sc->data[SC_MELON_BOMB].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MLAw6TDTMMYw/DC3MOcw8zDVMKMw/DDrMMk-
			if(battle_config.enemy_gravitation_type && sc->data[SC_GRAVITATION].timer != -1) {
				int penalty = sc->data[SC_GRAVITATION].val1 * 5;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MNAwpDDzMMkwyDDpMMMw1w-
			if(sc->data[SC_B_TRAP].timer != -1) {
				int penalty = sc->data[SC_B_TRAP].val3;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MKgwuTDP-
			if(sc->data[SC_SHA].timer != -1) {
				if(slow_val < sc->data[SC_SHA].val2)
					slow_val = sc->data[SC_SHA].val2;
			}

			// +MLcw/DDrMMkwwTCnMPww8zDpMMMwtzDl-
			if(sc->data[SC_SHIELDCHAINRUSH].timer != -1) {
				if(slow_val < sc->data[SC_SHIELDCHAINRUSH].val3)
					slow_val = sc->data[SC_SHIELDCHAINRUSH].val3;
			}

			// +MLAw6TCmMPMwyTCwMOkw0zDGMPwwtzDnMPM-
			if(sc->data[SC_GROUNDGRAVITY].timer != -1) {
				if(slow_val < sc->data[SC_GROUNDGRAVITY].val3)
					slow_val = sc->data[SC_GROUNDGRAVITY].val3;
			}

			/* speed+MExuG1wRMFkwizC5MMYw/DC/MLmKCHuX- */

			// +kB9epl83UxY-
			if(sc->data[SC_SPEEDUP1].timer != -1)
				haste_val = 50;

			// +kB9epliXUqA-
			if(sc->data[SC_INCREASEAGI].timer != -1) {
				if(haste_val < 25)
					haste_val = 25;
			}

			// +MKYwpDDzMMkwpjCpMPwwrw-
			if(sc->data[SC_WINDWALK].timer != -1) {
				int bonus = 2 * sc->data[SC_WINDWALK].val1;
				if(haste_val < bonus)
					haste_val = bonus;
			}

			// +MKQw8zDTMPMwtzDWMOs-
			if(sc->data[SC_INVINCIBLE].timer != -1 && sc->data[SC_INVINCIBLEOFF].timer == -1) {
				if(haste_val < 75)
					haste_val = 75;
			}

			/* +MF0wbk7W- */

			// +MMcwozDVMKcw8zDAMPw-
			if(sc->data[SC_DEFENDER].timer != -1)
				defender_flag = 1;

			// WALKSPEED
			if(sc->data[SC_WALKSPEED].timer != -1 && sc->data[SC_WALKSPEED].val1 > 0)
				walkspeed_flag = 1;
		}

		/* bonus_rate+MG5nAE9OUCQwkootW5o- */
		bonus_rate = slow_val - haste_val;
		if(bonus_rate < -60)
			bonus_rate = -60;

		/* speed+MG5nAH1Cigh7lw- */
		if(bonus_rate != 0)	// bonus_rate
			speed = speed * (bonus_rate+-100) / 100;
		if(defender_flag && speed < 200)	// +MMcwozDVMKcw8zDAMPw-
			speed = 200;
		if(walkspeed_flag)	// +MLkwrzDqMNcwyHUoMLkwxjD8ML8wuQ-
			speed = speed * 100 / sc->data[SC_WALKSPEED].val1;

		/* +ZwBPTlAkMAFnAFknUCQwkootW5owWTCL- */
		if(speed < MIN_WALK_SPEED)
			speed = MIN_WALK_SPEED;
		if(speed > MAX_WALK_SPEED)
			speed = MAX_WALK_SPEED;
	}

	return speed;
}

/*==========================================
 * +W/6MYTBu-aDelay(+ZTtkg2ZCMMcwozDsMKQ-)+MJKP1DBZ-
 * aDelay+MG9cDzBVMEQwezBGMExlO2SDkB9epjBMkB8wRA-
 *------------------------------------------
 */
int status_get_adelay(struct block_list *bl)
{
	int adelay = 4000;

	nullpo_retr(4000, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		adelay = (((struct map_session_data *)bl)->aspd);
	} else {
		double calc_adelay = 0;
		int haste_val1     = 0;
		int haste_val2     = 0;
		int slow_val       = 0;
		int bonus_rate     = 0;
		int ferver_bonus   = 0;
		int tmp            = 0;
		int add_val        = 0;
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB && (struct mob_data *)bl) {
			int guardup_lv = ((struct mob_data*)bl)->guardup_lv;
			if(mobdb_search(((struct mob_data *)bl)->class_)->adelay < mobdb_search(((struct mob_data *)bl)->class_)->amotion)
				calc_adelay = mobdb_search(((struct mob_data *)bl)->class_)->amotion;
			else
				calc_adelay = mobdb_search(((struct mob_data *)bl)->class_)->adelay;

			if(guardup_lv > 0)
				bonus_rate -= 5 +- 5*guardup_lv;
		} else if(bl->type == BL_PET && (struct pet_data *)bl) {
			if(mobdb_search(((struct pet_data *)bl)->class_)->adelay < mobdb_search(((struct pet_data *)bl)->class_)->amotion)
				calc_adelay = mobdb_search(((struct pet_data *)bl)->class_)->amotion;
			else
				calc_adelay = mobdb_search(((struct pet_data *)bl)->class_)->adelay;
		} else if(bl->type == BL_HOM && (struct homun_data *)bl) {
			calc_adelay = (((struct homun_data *)bl)->aspd);
		} else if(bl->type == BL_MERC && (struct merc_data *)bl) {
			calc_adelay = ((struct merc_data *)bl)->adelay;
		} else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
			calc_adelay = ((struct elem_data *)bl)->adelay;
		}

		/* amotion+MExZCVMWMFkwi3K2YUt1cF44MG6KCHuX- */
		if(sc) {

			/* amotion+MExYl1KgMFkwizC5MMYw/DC/MLkwbooIe5c- */

			// +ecEwkl/YMIwwajBEMGc-
			if(sc->data[SC_DONTFORGETME].timer != -1) {
				slow_val = sc->data[SC_DONTFORGETME].val1;
			}

			// +kdFSWw-
			if(sc->data[SC_STEELBODY].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MLgw5zCkMPMwyDDTMPwwyA-
			if(sc->data[SC_JOINTBEAT].timer != -1) {
				switch (sc->data[SC_JOINTBEAT].val4) {
					case 1:		// +YkuZlg-
						if(slow_val < 25)
							slow_val = 25;
						break;
					case 2:		// +gZ0-
						if(slow_val < 10)
							slow_val = 10;
						break;
				}
			}

			// +MLAw6TDTMMYw/DC3MOcw8zDVMKMw/DDrMMk-
			if(sc->data[SC_GRAVITATION].timer != -1) {
				int penalty = sc->data[SC_GRAVITATION].val1*5;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MNUw7TC5MMgw3zC5MMYwow-
			if(sc->data[SC_FROSTMISTY].timer != -1) {
				if(slow_val < 15)
					slow_val = 15;
			}

			// +MNwwxzCjMNowpDDzMMYwozDzMLA-
			if(sc->data[SC__BODYPAINT].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MKQw8zDTMLgw0zDqMMYwow-
			if(sc->data[SC__INVISIBILITY].timer != -1) {
				int penalty = 50 - 10 * sc->data[SC__INVISIBILITY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
			if(sc->data[SC__GROOMY].timer != -1) {
				int penalty = 20 +- 10 * sc->data[SC__GROOMY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MKIw/DC5MMkw6TCkMNY-
			if(sc->data[SC_EARTHDRIVE].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MOEw6TDzMLMw6jD8-
			if(sc->data[SC_GLOOMYDAY].timer != -1) {
				int penalty = 15 +- 5 * sc->data[SC_GLOOMYDAY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MOEw7TDzcgZfPg-
			if(sc->data[SC_MELON_BOMB].timer != -1) {
				int penalty = sc->data[SC_MELON_BOMB].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			/* amotion+MExuG1wRMFkwizC5MMYw/DC/MLkwbooIe5c-1 */

			// +WJeQHzDdMPwwtzDnMPM-
			if(sc->data[tmp = SC_SPEEDPOTION2].timer != -1 || sc->data[tmp = SC_SPEEDPOTION1].timer != -1 || sc->data[tmp = SC_SPEEDPOTION0].timer != -1)
				haste_val1 = sc->data[tmp].val2;

			/* amotion+MExuG1wRMFkwizC5MMYw/DC/MLkwbooIe5c-2 */

			// +MMQw/DDPMPMwyTCvMKMwwzCxMPM-
			if(sc->data[SC_TWOHANDQUICKEN].timer != -1)
				haste_val2 = sc->data[SC_TWOHANDQUICKEN].val2;

			// +MLkw1DCiMK8wozDDMLEw8w-
			if(sc->data[SC_SPEARQUICKEN].timer != -1) {
				if(haste_val2 < sc->data[SC_SPEARQUICKEN].val2)
					haste_val2 = sc->data[SC_SPEARQUICKEN].val2;
			}

			// +MO8w8zDPMPMwyTCvMKMwwzCxMPM-
			if(sc->data[SC_ONEHAND].timer != -1) {
				if(haste_val2 < 30)
					haste_val2 = 30;
			}

			// +MKIwyTDsMMow6jDzMOkwwzC3MOU-
			if(sc->data[SC_ADRENALINE].timer != -1) {
				int bonus;
				if(sc->data[SC_ADRENALINE].val2 || !battle_config.party_skill_penalty)
					bonus = 30;
				else
					bonus = 25;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +MKIwyTDsMMow6jDzMOkwwzC3MOU-2
			if(sc->data[SC_ADRENALINE2].timer != -1) {
				int bonus;
				if(sc->data[SC_ADRENALINE2].val2 || !battle_config.party_skill_penalty)
					bonus = 30;
				else
					bonus = 25;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +WRWWfTBuMKIwtTC3MPMwrzDtMLk-
			if(sc->data[SC_ASSNCROS].timer != -1) {
				int bonus = sc->data[SC_ASSNCROS].val2;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
			if(sc->data[SC_GATLINGFEVER].timer != -1) {
				ferver_bonus = sc->data[SC_GATLINGFEVER].val1*2;
				if(haste_val2 < ferver_bonus)
					haste_val2 = ferver_bonus;
			}

			// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
			if(sc->data[SC_MADNESSCANCEL].timer != -1) {
				int bonus = 20+-ferver_bonus;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +cLl6dA- -+U80--
			if(sc->data[SC_GENTLETOUCH_CHANGE].timer != -1) {
				int bonus = sc->data[SC_GENTLETOUCH_CHANGE].val4;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			/* +MF0wbk7W- */

			// +MMcwozDVMKcw8zDAMPw-
			if(sc->data[SC_DEFENDER].timer != -1)
				add_val +-= sc->data[SC_DEFENDER].val3;

			// +MNIw/DDIMNAw7DDr-
			if(sc->data[SC_HEAT_BARREL].timer != -1)
				add_val -= sc->data[SC_HEAT_BARREL].val1 * 10;

			// +l1lbwg-
			if(sc->data[SC_HANDICAPSTATE_DEEPSILENCE].timer != -1)
				add_val +-= sc->data[SC_HANDICAPSTATE_DEEPSILENCE].val2;
		}

		/* slow_val+MGg-haste_val1+MGg-haste_val2+MJJSoHuXMFkwiw- */
		bonus_rate = slow_val - haste_val1 - haste_val2;

		/* bonus_rate+MG6KCHuX- */
		if(bonus_rate != 0)
			calc_adelay = calc_adelay * (bonus_rate+-100) / 100;

		/* add_val+MG5SoHuX- */
		if(add_val != 0)
			calc_adelay +-= add_val;

		/* +XA9lcFIHMIpOCjBS- */
		adelay = (int)ceil(calc_adelay);

		/* +ZwBPTlAkMG6KLVua- */
		switch(bl->type)
		{
			case BL_MOB:
			case BL_PET:
				if(adelay < (battle_config.monster_max_aspd>>1) )
					adelay = battle_config.monster_max_aspd>>1;
				break;
			case BL_HOM:
#ifdef PRE_RENEWAL
				if(adelay < battle_config.homun_max_aspd)
					adelay = battle_config.homun_max_aspd;
#else
				if(adelay < (battle_config.homun_max_aspd>>1) )
					adelay = battle_config.homun_max_aspd>>1;
#endif
				break;
			case BL_MERC:
				if(adelay < (battle_config.merc_max_aspd>>1) )
					adelay = battle_config.merc_max_aspd>>1;
			case BL_ELEM:
				if(adelay < (battle_config.elem_max_aspd>>1) )
					adelay = battle_config.elem_max_aspd>>1;
				break;
		}
	}

	return adelay;
}

/*==========================================
 * +W/6MYTBu-amotion+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_amotion(struct block_list *bl)
{
	int amotion = 2000;

	nullpo_retr(2000, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		amotion = ((struct map_session_data *)bl)->amotion;
	} else {
		double calc_amotion = 0;
		int haste_val1      = 0;
		int haste_val2      = 0;
		int slow_val        = 0;
		int bonus_rate      = 0;
		int ferver_bonus    = 0;
		int tmp             = 0;
		char defender_flag  = 0;
		char heatbarrel_flag = 0;
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB && (struct mob_data *)bl) {
			int guardup_lv = ((struct mob_data*)bl)->guardup_lv;
			calc_amotion = mobdb_search(((struct mob_data *)bl)->class_)->amotion;
			if(guardup_lv > 0)
				bonus_rate -= 5 +- 5*guardup_lv;
		} else if(bl->type == BL_PET && (struct pet_data *)bl) {
			calc_amotion = mobdb_search(((struct pet_data *)bl)->class_)->amotion;
		} else if(bl->type == BL_HOM && (struct homun_data *)bl && ((struct homun_data *)bl)->msd) {
			calc_amotion = ((struct homun_data *)bl)->aspd;
		} else if(bl->type == BL_MERC && (struct merc_data *)bl && ((struct merc_data *)bl)->msd) {
			calc_amotion = ((struct merc_data *)bl)->amotion;
		} else if(bl->type == BL_ELEM && (struct elem_data *)bl && ((struct elem_data *)bl)->msd) {
			calc_amotion = ((struct elem_data *)bl)->amotion;
		}

		/* amotion+MExZCVMWMFkwi3K2YUt1cF44MG6KCHuX- */
		if(sc) {

			/* amotion+MExYl1KgMFkwizC5MMYw/DC/MLkwbooIe5c- */

			// +ecEwkl/YMIwwajBEMGc-
			if(sc->data[SC_DONTFORGETME].timer != -1) {
				slow_val = sc->data[SC_DONTFORGETME].val1;
			}

			// +kdFSWw-
			if(sc->data[SC_STEELBODY].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MLgw5zCkMPMwyDDTMPwwyA-
			if(sc->data[SC_JOINTBEAT].timer != -1) {
				switch (sc->data[SC_JOINTBEAT].val4) {
					case 1:		// +YkuZlg-
						if(slow_val < 25)
							slow_val = 25;
						break;
					case 2:		// +gZ0-
						if(slow_val < 10)
							slow_val = 10;
						break;
				}
			}

			// +MLAw6TDTMMYw/DC3MOcw8zDVMKMw/DDrMMk-
			if(sc->data[SC_GRAVITATION].timer != -1) {
				int penalty = sc->data[SC_GRAVITATION].val1*5;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MNUw7TC5MMgw3zC5MMYwow-
			if(sc->data[SC_FROSTMISTY].timer != -1) {
				if(slow_val < 15)
					slow_val = 15;
			}

			// +MNwwxzCjMNowpDDzMMYwozDzMLA-
			if(sc->data[SC__BODYPAINT].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MKQw8zDTMLgw0zDqMMYwow-
			if(sc->data[SC__INVISIBILITY].timer != -1) {
				int penalty = 50 - 10 * sc->data[SC__INVISIBILITY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8-
			if(sc->data[SC__GROOMY].timer != -1) {
				int penalty = 20 +- 10 * sc->data[SC__GROOMY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MKIw/DC5MMkw6TCkMNY-
			if(sc->data[SC_EARTHDRIVE].timer != -1) {
				if(slow_val < 25)
					slow_val = 25;
			}

			// +MOEw6TDzMLMw6jD8-
			if(sc->data[SC_GLOOMYDAY].timer != -1) {
				int penalty = 15 +- 5 * sc->data[SC_GLOOMYDAY].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			// +MOEw7TDzcgZfPg-
			if(sc->data[SC_MELON_BOMB].timer != -1) {
				int penalty = sc->data[SC_MELON_BOMB].val1;
				if(slow_val < penalty)
					slow_val = penalty;
			}

			/* amotion+MExuG1wRMFkwizC5MMYw/DC/MLkwbooIe5c-1 */

			// +WJeQHzDdMPwwtzDnMPM-
			if(sc->data[tmp = SC_SPEEDPOTION2].timer != -1 || sc->data[tmp = SC_SPEEDPOTION1].timer != -1 || sc->data[tmp = SC_SPEEDPOTION0].timer != -1)
				haste_val1 = sc->data[tmp].val2;

			/* amotion+MExuG1wRMFkwizC5MMYw/DC/MLkwbooIe5c-2 */

			// +MMQw/DDPMPMwyTCvMKMwwzCxMPM-
			if(sc->data[SC_TWOHANDQUICKEN].timer != -1)
				haste_val2 = sc->data[SC_TWOHANDQUICKEN].val2;

			// +MLkw1DCiMK8wozDDMLEw8w-
			if(sc->data[SC_SPEARQUICKEN].timer != -1) {
				if(haste_val2 < sc->data[SC_SPEARQUICKEN].val2)
					haste_val2 = sc->data[SC_SPEARQUICKEN].val2;
			}

			// +MO8w8zDPMPMwyTCvMKMwwzCxMPM-
			if(sc->data[SC_ONEHAND].timer != -1) {
				if(haste_val2 < 30)
					haste_val2 = 30;
			}

			// +MKIwyTDsMMow6jDzMOkwwzC3MOU-
			if(sc->data[SC_ADRENALINE].timer != -1) {
				int bonus;
				if(sc->data[SC_ADRENALINE].val2 || !battle_config.party_skill_penalty)
					bonus = 30;
				else
					bonus = 25;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +MKIwyTDsMMow6jDzMOkwwzC3MOU-2
			if(sc->data[SC_ADRENALINE2].timer != -1) {
				int bonus;
				if(sc->data[SC_ADRENALINE2].val2 || !battle_config.party_skill_penalty)
					bonus = 30;
				else
					bonus = 25;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +WRWWfTBuMKIwtTC3MPMwrzDtMLk-
			if(sc->data[SC_ASSNCROS].timer != -1) {
				int bonus = sc->data[SC_ASSNCROS].val2;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +MKwwyDDqMPMwsDDVMKMw/DDQMPw-
			if(sc->data[SC_GATLINGFEVER].timer != -1) {
				ferver_bonus = sc->data[SC_GATLINGFEVER].val1*2;
				if(haste_val2 < ferver_bonus)
					haste_val2 = ferver_bonus;
			}

			// +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A-
			if(sc->data[SC_MADNESSCANCEL].timer != -1) {
				int bonus = 20+-ferver_bonus;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			// +cLl6dA- -+U80--
			if(sc->data[SC_GENTLETOUCH_CHANGE].timer != -1) {
				int bonus = sc->data[SC_GENTLETOUCH_CHANGE].val4;
				if(haste_val2 < bonus)
					haste_val2 = bonus;
			}

			/* +MF0wbk7W- */

			// +MMcwozDVMKcw8zDAMPw-
			if(sc->data[SC_DEFENDER].timer != -1)
				defender_flag = 1;

			// +MNIw/DDIMNAw7DDr-
			if(sc->data[SC_HEAT_BARREL].timer != -1)
				heatbarrel_flag = 1;
		}

		/* slow_val+MGg-haste_val1+MGg-haste_val2+MJJSoHuXMFkwiw- */
		bonus_rate = slow_val - haste_val1 - haste_val2;

		/* bonus_rate+MG6KCHuX- */
		if(bonus_rate != 0)
			calc_amotion = calc_amotion * (bonus_rate+-100) / 100;

		/* +MMcwozDVMKcw8zDAMPw- */
		if(defender_flag)
			calc_amotion +-= sc->data[SC_DEFENDER].val3;

		/* +MNIw/DDIMNAw7DDr- */
		if(heatbarrel_flag)
			calc_amotion -= sc->data[SC_HEAT_BARREL].val1 * 10;

		/* +XA9lcFIHMIpOCjBS- */
		amotion = (int)ceil(calc_amotion);

		/* +ZwBPTlAkMG6KLVua- */
		switch(bl->type)
		{
			case BL_MOB:
			case BL_PET:
				if(amotion < battle_config.monster_max_aspd)
					amotion = battle_config.monster_max_aspd;
				break;
			case BL_HOM:
				if(amotion < battle_config.homun_max_aspd)
					amotion = battle_config.homun_max_aspd;
				break;
			case BL_MERC:
				if(amotion < battle_config.merc_max_aspd)
					amotion = battle_config.merc_max_aspd;
				break;
			case BL_ELEM:
				if(amotion < battle_config.elem_max_aspd)
					amotion = battle_config.elem_max_aspd;
				break;
		}
	}

	return amotion;
}

/*==========================================
 * +W/6MYTBu-dmotion+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_dmotion(struct block_list *bl)
{
	int dmotion = 2000;

	nullpo_retr(2000, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl) {
		dmotion = mobdb_search(((struct mob_data *)bl)->class_)->dmotion;
		if(battle_config.monster_damage_delay_rate != 100)
			dmotion = dmotion*battle_config.monster_damage_delay_rate/100;
	}
	else if(bl->type == BL_PC) {
		struct map_session_data *sd = (struct map_session_data *)bl;
		if(sd) {
			if((sd->sc.data[SC_ENDURE].timer != -1 || sd->special_state.infinite_endure) && !map[sd->bl.m].flag.gvg) {
				dmotion = 0;
			} else {
				dmotion = sd->dmotion;
				if(battle_config.pc_damage_delay_rate != 100)
					dmotion = dmotion*battle_config.pc_damage_delay_rate/100;
			}
		}
	}
	else if(bl->type == BL_HOM && (struct homun_data *)bl && ((struct homun_data *)bl)->msd) {
		dmotion = 800 - ((struct homun_data *)bl)->status.agi*4;
		if(dmotion < 400)
			dmotion = 400;
	}
	else if(bl->type == BL_MERC && (struct merc_data *)bl && ((struct merc_data *)bl)->msd) {
		dmotion = ((struct merc_data *)bl)->dmotion;
	}
	else if(bl->type == BL_ELEM && (struct elem_data *)bl && ((struct elem_data *)bl)->msd) {
		dmotion = ((struct elem_data *)bl)->dmotion;
	}
	else if(bl->type == BL_PET && (struct pet_data *)bl) {
		dmotion = mobdb_search(((struct pet_data *)bl)->class_)->dmotion;
	}

	return dmotion;
}

/*==========================================
 * +W/6MYTBu-P.Atk+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_patk(struct block_list *bl)
{
	int patk = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		patk = ((struct map_session_data *)bl)->patk;

	if(patk < 0) patk = 0;
	return patk;
}

/*==========================================
 * +W/6MYTBu-S.Matk+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_smatk(struct block_list *bl)
{
	int smatk = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		smatk = ((struct map_session_data *)bl)->smatk;

	if(smatk < 0) smatk = 0;
	return smatk;
}

/*==========================================
 * +W/6MYTBu-Res+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_res(struct block_list *bl)
{
	int res = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		res = ((struct map_session_data *)bl)->res;
	} else {
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB && (struct mob_data *)bl) {
			res = ((struct mob_data *)bl)->st.res;
		}
		if(sc) {
			if(sc->data[SC_SHADOW_STRIP].timer != -1) 	// +MLkwyDDqMMMw1zC3MOMwyTCm-
				res -= sc->data[SC_SHADOW_STRIP].val2;
			if(sc->data[SC_AIN_RHAPSODY].timer != -1) 	// +knFU4TBuMOkw1zC9MMcwow-
				res -= sc->data[SC_AIN_RHAPSODY].val3;
		}
	}

	if(res < 0) res = 0;
	return res;
}

/*==========================================
 * +W/6MYTBu-Mres+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_mres(struct block_list *bl)
{
	int mres = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		mres = ((struct map_session_data *)bl)->mres;
	} else {
		struct status_change *sc = status_get_sc(bl);

		if(bl->type == BL_MOB && (struct mob_data *)bl) {
			mres = ((struct mob_data *)bl)->st.mres;
		}
		if(sc) {
			if(sc->data[SC_SHADOW_STRIP].timer != -1) 	// +MLkwyDDqMMMw1zC3MOMwyTCm-
				mres -= sc->data[SC_SHADOW_STRIP].val2;
			if(sc->data[SC_GEF_NOCTURN].timer != -1) 	// +MLIw1TCnMMswojDOMK8wvzD8MPM-
				mres -= sc->data[SC_GEF_NOCTURN].val3;
		}
	}

	if(mres < 0) mres = 0;
	return mres;
}

/*==========================================
 * +W/6MYTBu-H.Plus+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_hplus(struct block_list *bl)
{
	int hplus = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		hplus = ((struct map_session_data *)bl)->hplus;

	if(hplus < 0) hplus = 0;
	return hplus;
}

/*==========================================
 * +W/6MYTBu-C.Rate+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_crate(struct block_list *bl)
{
	int crate = 0;

	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl)
		crate = ((struct map_session_data *)bl)->crate;

	if(crate < 0) crate = 0;
	return crate;
}

/*==========================================
 * +W/6MYTBuXF5gJzCSj9QwWQ-
 *------------------------------------------
 */
int status_get_element(struct block_list *bl)
{
	int ret = 20;
	struct status_change *sc;

	nullpo_retr(ret, bl);

	sc = status_get_sc(bl);
	if(sc) {
		if(sc->data[SC_BENEDICTIO].timer != -1)		// +gFZPU5ZNeY8-
			ret = 20 +- ELE_HOLY;
		if(sc->data[SC_ELEMENTWATER].timer != -1)	// +bDQ-
			ret = 20*sc->data[SC_ELEMENTWATER].val1 +- ELE_WATER;
		if(sc->data[SC_ELEMENTGROUND].timer != -1)	// +Vx8-
			ret = 20*sc->data[SC_ELEMENTGROUND].val1 +- ELE_EARTH;
		if(sc->data[SC_ELEMENTFIRE].timer != -1)		// +cGs-
			ret = 20*sc->data[SC_ELEMENTFIRE].val1 +- ELE_FIRE;
		if(sc->data[SC_ELEMENTWIND].timer != -1)		// +mKg-
			ret = 20*sc->data[SC_ELEMENTWIND].val1 +- ELE_WIND;
		if(sc->data[SC_ELEMENTPOISON].timer != -1)	// +a9I-
			ret = 20*sc->data[SC_ELEMENTPOISON].val1 +- ELE_POISON;
		if(sc->data[SC_ELEMENTHOLY].timer != -1)		// +gFY-
			ret = 20*sc->data[SC_ELEMENTHOLY].val1 +- ELE_HOLY;
		if(sc->data[SC_ELEMENTDARK].timer != -1)		// +lcc-
			ret = 20*sc->data[SC_ELEMENTDARK].val1 +- ELE_DARK;
		if(sc->data[SC_ELEMENTELEKINESIS].timer != -1)	// +X/U-
			ret = 20*sc->data[SC_ELEMENTELEKINESIS].val1 +- ELE_GHOST;
		if(sc->data[SC_ELEMENTUNDEAD].timer != -1)	// +Tg1rew-
			ret = 20*sc->data[SC_ELEMENTUNDEAD].val1 +- ELE_UNDEAD;
		if(sc->data[SC_FLAMEARMOR].timer != -1)	// +MNUw7DCkMOAwojD8MN4w/A-
			ret = 20 +- ELE_FIRE;
		if(sc->data[SC_CRYSTAL_ARMOR].timer != -1)	// +MK8w6jC5ML8w6zCiMPww3jD8-
			ret = 20 +- ELE_WATER;
		if(sc->data[SC_EYES_OF_STORM].timer != -1)	// +MKIwpDC6MKow1jC5MMgw/DDg-
			ret = 20 +- ELE_WIND;
		if(sc->data[SC_STRONG_PROTECTION].timer != -1)	// +MLkwyDDtMPMwsDDXMO0wxjCvMLcw5zDz-
			ret = 20 +- ELE_EARTH;
		if(sc->data[SC_POISON_SHIELD].timer != -1)	// +MN0wpDC6MPMwtzD8MOswyQ-
			ret = 20 +- ELE_POISON;
		if(sc->data[SC_FREEZE].timer != -1)		// +Uc19UA-
			ret = 20 +- ELE_WATER;
		if(sc->data[SC_STONE].timer != -1 && sc->data[SC_STONE].val2 == 0)
			ret = 20 +- ELE_EARTH;
		if(sc->data[SC_SHAPESHIFT].timer != -1)		// +MLcwpzCkMNcwtzDVMMg-
			ret = 20 +- sc->data[SC_SHAPESHIFT].val2;
		if(sc->data[SC_HANDICAPSTATE_FROSTBITE].timer != -1)		// +YCVRtw-
			ret = 20 +- ELE_WATER;
		if(sc->data[SC_HANDICAPSTATE_LIGHTNINGSTRIKE].timer != -1)	// +b8BtQQ-
			ret = 20 +- ELE_WIND;
		if(sc->data[SC_HANDICAPSTATE_CRYSTALLIZATION].timer != -1)	// +fVBmdlMW-
			ret = 20 +- ELE_EARTH;
		if(sc->data[SC_HANDICAPSTATE_CONFLAGRATION].timer != -1)	// +cGtwfQ-
			ret = 20 +- ELE_FIRE;

		if(ret != 20)
			return ret;
	}

	if(bl->type == BL_MOB && (struct mob_data *)bl) {
		// 10+MG5PTf8d-Lv*2+MAH/ETBuT03/HVxeYCc-
		ret = ((struct mob_data *)bl)->def_ele;
	}
	else if(bl->type == BL_PC && (struct map_session_data *)bl) {
		// +ljJfoVxeYCc-Lv1
		ret = 20+-((struct map_session_data *)bl)->def_ele;
	}
	else if(bl->type == BL_PET && (struct pet_data *)bl) {
		ret = mobdb_search(((struct pet_data *)bl)->class_)->element;
	}
	else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		ret = homun_db[hom_dbcheck_id(((struct homun_data *)bl)->status.class_)].element;
	}
	else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		struct merc_db *db = merc_search_data(((struct merc_data *)bl)->status.class_);
		if(db)
			ret = db->element;
	}
	else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		struct elem_db *db = elem_search_data(((struct elem_data *)bl)->status.class_);
		if(db)
			ret = db->element;
	}

	return ret;
}

/*==========================================
 * +W/6MYTBuZTtkg1xeYCcwko/UMFk-
 *------------------------------------------
 */
int status_get_attack_element(struct block_list *bl)
{
	int ret = ELE_NEUTRAL;
	struct status_change *sc = NULL;

	nullpo_retr(ELE_NEUTRAL, bl);

	sc = status_get_sc(bl);
	if(bl->type == BL_MOB && (struct mob_data *)bl)
		ret = ELE_NEUTRAL;
	else if(bl->type == BL_PC && (struct map_session_data *)bl) {
		ret = ((struct map_session_data *)bl)->atk_ele;
	}
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		ret = ELE_NEUTRAL;
	else if(bl->type == BL_HOM && (struct homun_data *)bl)
		ret = ELE_NONE;	// +cSFcXmAn-
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		ret = ELE_NONE;	// +cSFcXmAn-
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		ret = ELE_NONE;	// +cSFcXmAn-

	if(sc) {
		if(sc->data[SC_FROSTWEAPON].timer != -1)		// +MNUw7TC5MMgwpjCnMN0w8w-
			ret = ELE_WATER;
		if(sc->data[SC_SEISMICWEAPON].timer != -1)	// +MLUwpDC6MN8wwzCvMKYwpzDdMPM-
			ret = ELE_EARTH;
		if(sc->data[SC_FLAMELAUNCHER].timer != -1)	// +MNUw7DD8MOAw6TDzMMEw4zD8-
			ret = ELE_FIRE;
		if(sc->data[SC_LIGHTNINGLOADER].timer != -1)	// +MOkwpDDIMMsw8zCwMO0w/DDAMPw-
			ret = ELE_WIND;
		if(sc->data[SC_ENCPOISON].timer != -1)		// +MKgw8zDBMOMw8zDIMN0wpDC6MPM-
			ret = ELE_POISON;
		if(sc->data[SC_ASPERSIO].timer != -1)		// +MKIwuTDaMOswtzCq-
			ret = ELE_HOLY;
		if(sc->data[SC_DARKELEMENT].timer != -1)		// +lcdcXmAn-
			ret = ELE_DARK;
		if(sc->data[SC_ATTENELEMENT].timer != -1)	// +X/VcXmAn-
			ret = ELE_GHOST;
		if(sc->data[SC_UNDEADELEMENT].timer != -1)	// +Tg1re1xeYCc-
			ret = ELE_UNDEAD;
		if(sc->data[SC_SEVENWIND].timer != -1)		// +ZpYwSzBEmKg-
			ret = sc->data[SC_SEVENWIND].val3;
		if(sc->data[SC_TIDAL_WEAPON].timer != -1 || sc->data[SC_TIDAL_WEAPON_OPTION].timer != -1)	// +ML8wpDDAMOswpjCnMN0w8w-
			ret = ELE_WATER;
	}
	return ret;
}

/*==========================================
 * +W/6MYTBuZTtkg1xeYCf/CF3mYkv/CTCSj9QwWQ-
 *------------------------------------------
 */
int status_get_attack_element2(struct block_list *bl)
{
	nullpo_retr(ELE_NEUTRAL, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		int ret;
		struct status_change *sc = status_get_sc(bl);
		ret = ((struct map_session_data *)bl)->atk_ele_;

		if(sc) {
			if(sc->data[SC_FROSTWEAPON].timer != -1)		// +MNUw7TC5MMgwpjCnMN0w8w-
				ret = ELE_WATER;
			if(sc->data[SC_SEISMICWEAPON].timer != -1)	// +MLUwpDC6MN8wwzCvMKYwpzDdMPM-
				ret = ELE_EARTH;
			if(sc->data[SC_FLAMELAUNCHER].timer != -1)	// +MNUw7DD8MOAw6TDzMMEw4zD8-
				ret = ELE_FIRE;
			if(sc->data[SC_LIGHTNINGLOADER].timer != -1)	// +MOkwpDDIMMsw8zCwMO0w/DDAMPw-
				ret = ELE_WIND;
			if(sc->data[SC_ENCPOISON].timer != -1)		// +MKgw8zDBMOMw8zDIMN0wpDC6MPM-
				ret = ELE_POISON;
			if(sc->data[SC_ASPERSIO].timer != -1)		// +MKIwuTDaMOswtzCq-
				ret = ELE_HOLY;
			if(sc->data[SC_DARKELEMENT].timer != -1)		// +lcdcXmAn-
				ret = ELE_DARK;
			if(sc->data[SC_ATTENELEMENT].timer != -1)	// +X/VcXmAn-
				ret = ELE_GHOST;
			if(sc->data[SC_UNDEADELEMENT].timer != -1)	// +Tg1re1xeYCc-
				ret = ELE_UNDEAD;
			if(sc->data[SC_SEVENWIND].timer != -1)		// +ZpYwSzBEmKg-
				ret = sc->data[SC_SEVENWIND].val3;
		}
		return ret;
	}
	return ELE_NEUTRAL;
}

/*==========================================
 * +W/6MYTBua2ZWaFxeYCcwkpZkMEQwX2U7ZINcXmAnMJKP1DBZ-
 *------------------------------------------
 */
int status_get_attack_element_nw(struct block_list *bl)
{
	int ret = ELE_NEUTRAL;
	struct status_change *sc = NULL;

	nullpo_retr(ELE_NEUTRAL, bl);

	sc = status_get_sc(bl);

	if(bl->type == BL_HOM && (struct homun_data *)bl)
		ret = ELE_NONE;	// +cSFcXmAn-
	else if(bl->type == BL_MERC && (struct merc_data *)bl)
		ret = ELE_NONE;	// +cSFcXmAn-
	else if(bl->type == BL_ELEM && (struct elem_data *)bl)
		ret = ELE_NONE;	// +cSFcXmAn-

	if(sc) {
		if(sc->data[SC_FROSTWEAPON].timer != -1)		// +MNUw7TC5MMgwpjCnMN0w8w-
			ret = ELE_WATER;
		if(sc->data[SC_SEISMICWEAPON].timer != -1)	// +MLUwpDC6MN8wwzCvMKYwpzDdMPM-
			ret = ELE_EARTH;
		if(sc->data[SC_FLAMELAUNCHER].timer != -1)	// +MNUw7DD8MOAw6TDzMMEw4zD8-
			ret = ELE_FIRE;
		if(sc->data[SC_LIGHTNINGLOADER].timer != -1)	// +MOkwpDDIMMsw8zCwMO0w/DDAMPw-
			ret = ELE_WIND;
		if(sc->data[SC_ENCPOISON].timer != -1)		// +MKgw8zDBMOMw8zDIMN0wpDC6MPM-
			ret = ELE_POISON;
		if(sc->data[SC_ASPERSIO].timer != -1)		// +MKIwuTDaMOswtzCq-
			ret = ELE_HOLY;
		if(sc->data[SC_DARKELEMENT].timer != -1)		// +lcdcXmAn-
			ret = ELE_DARK;
		if(sc->data[SC_ATTENELEMENT].timer != -1)	// +X/VcXmAn-
			ret = ELE_GHOST;
		if(sc->data[SC_UNDEADELEMENT].timer != -1)	// +Tg1re1xeYCc-
			ret = ELE_UNDEAD;
		if(sc->data[SC_SEVENWIND].timer != -1)		// +ZpYwSzBEmKg-
			ret = sc->data[SC_SEVENWIND].val3;
		if(sc->data[SC__INVISIBILITY].timer != -1)	// +MKQw8zDTMLgw0zDqMMYwow-
			ret = ELE_GHOST;
		if(sc->data[SC_PYROCLASTIC].timer != -1)	// +MNEwpDDtMK8w6TC5MMYwozDDMK8-
			ret = ELE_FIRE;
	}
	return ret;
}

/*==========================================
 * +W/6MYTBuMNEw/DDGMKM-ID+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_party_id(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		return ((struct map_session_data *)bl)->status.party_id;
	}
	else if(bl->type == BL_MOB && (struct mob_data *)bl) {
		struct mob_data *md = (struct mob_data *)bl;
		return (md->master_id > 0)? -md->master_id: -md->bl.id;
	}
	else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		//struct homun_data *hd = (struct homun_data *)bl;
		//return status_get_party_id(&hd->msd->bl);
		return 0;
	}
	else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		//struct merc_data *mcd = (struct merc_data *)bl;
		//return status_get_party_id(&mcd->msd->bl);
		return 0;
	}
	else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		//struct elem_data *eld = (struct elem_data *)bl;
		//return status_get_party_id(&eld->msd->bl);
		return 0;
	}
	else if(bl->type == BL_SKILL && (struct skill_unit *)bl) {
		return ((struct skill_unit *)bl)->group->party_id;
	}

	return 0;
}

/*==========================================
 * +W/6MYTBuMK4w6zDJ-ID+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_guild_id(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC && (struct map_session_data *)bl) {
		return ((struct map_session_data *)bl)->status.guild_id;
	} else if(bl->type == BL_MOB && (struct mob_data *)bl) {
		return ((struct mob_data *)bl)->class_;
	} else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		//struct homun_data *hd = (struct homun_data *)bl;
		//return status_get_guild_id(&hd->msd->bl);
		return 0;
	} else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		//struct merc_data *mcd = (struct merc_data *)bl;
		//return status_get_guild_id(&mcd->msd->bl);
		return 0;
	} else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		//struct elem_data *eld = (struct elem_data *)bl;
		//return status_get_guild_id(&eld->msd->bl);
		return 0;
	} else if(bl->type == BL_SKILL && (struct skill_unit *)bl) {
		return ((struct skill_unit *)bl)->group->guild_id;
	}

	return 0;
}

/*==========================================
 * +W/6MYTBuei5lzzCSj9QwWQ-
 *------------------------------------------
 */
int status_get_race(struct block_list *bl)
{
	int race = RCT_FORMLESS;
	struct status_change *sc;

	nullpo_retr(RCT_FORMLESS, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl) {
		race = mobdb_search(((struct mob_data *)bl)->class_)->race;
	}
	else if(bl->type == BL_PC && (struct map_session_data *)bl) {
		race = ((struct map_session_data *)bl)->race;
	}
	else if(bl->type == BL_PET && (struct pet_data *)bl) {
		return mobdb_search(((struct pet_data *)bl)->class_)->race;
	}
	else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		return homun_db[hom_dbcheck_id(((struct homun_data *)bl)->status.class_)].race;
	}
	else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		struct merc_db *db = merc_search_data(((struct merc_data *)bl)->status.class_);
		if(db)
			return db->race;
	}
	else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		struct elem_db *db = elem_search_data(((struct elem_data *)bl)->status.class_);
		if(db)
			return db->race;
	}
	else {
		return RCT_FORMLESS;
	}

	sc = status_get_sc(bl);

	if(sc) {
		if(sc->data[SC_RACEUNKNOWN].timer != -1)	// +cSFfYg-
			race = RCT_FORMLESS;
		if(sc->data[SC_RACEUNDEAD].timer != -1)	// +Tg1rew-
			race = RCT_UNDEAD;
		if(sc->data[SC_RACEBEAST].timer != -1)	// +UtVyaQ-
			race = RCT_BRUTE;
		if(sc->data[SC_RACEPLANT].timer != -1)	// +aQ1yaQ-
			race = RCT_PLANT;
		if(sc->data[SC_RACEINSECT].timer != -1)	// +ZgaGaw-
			race = RCT_INSECT;
		if(sc->data[SC_RACEFISH].timer != -1)	// +m1qMnQ-
			race = RCT_FISH;
		if(sc->data[SC_RACEDEVIL].timer != -1)	// +YKqbVA-
			race = RCT_DEMON;
		if(sc->data[SC_RACEHUMAN].timer != -1)	// +TrqVkw-
			race = RCT_DEMIHUMAN;
		if(sc->data[SC_RACEANGEL].timer != -1)	// +WSlPfw-
			race = RCT_ANGEL;
		if(sc->data[SC_RACEDRAGON].timer != -1)	// +etw-
			race = RCT_DRAGON;
	}

	return race;
}

/*==========================================
 * +W/6MYTBuMLUwpDC6MJKP1DBZ-
 *------------------------------------------
 */
int status_get_size(struct block_list *bl)
{
	nullpo_retr(1, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl) {
		return mobdb_search(((struct mob_data *)bl)->class_)->size;
	} else if(bl->type == BL_PC && (struct map_session_data *)bl) {
		if(pc_isbaby((struct map_session_data *)bl) || pc_isdoram((struct map_session_data *)bl))
			return 0;
		else
			return 1;
	} else if(bl->type == BL_PET && (struct pet_data *)bl) {
		return mobdb_search(((struct pet_data *)bl)->class_)->size;
	} else if(bl->type == BL_HOM && (struct homun_data *)bl) {
		return homun_db[hom_dbcheck_id(((struct homun_data *)bl)->status.class_)].size;
	} else if(bl->type == BL_MERC && (struct merc_data *)bl) {
		struct merc_db *db = merc_search_data(((struct merc_data *)bl)->status.class_);
		if(db)
			return db->size;
	} else if(bl->type == BL_ELEM && (struct elem_data *)bl) {
		struct elem_db *db = elem_search_data(((struct elem_data *)bl)->status.class_);
		if(db)
			return db->size;
	}

	return 1;
}

/*==========================================
 * +W/6MYTBuMOIw/DDJMJKP1DBZ-
 *------------------------------------------
 */
int status_get_mode(struct block_list *bl)
{
	nullpo_retr(0x01, bl);

	if(bl->type == BL_MOB) {
		struct mob_data* md = (struct mob_data*)bl;
		return (md->mode ? md->mode : mobdb_search(md->class_)->mode);
	}
	else if(bl->type == BL_PET) {
		return mobdb_search(((struct pet_data *)bl)->class_)->mode;
	}

	return MD_CANMOVE;	// +MGgwijBCMEgwWlLVME8waDBEMEYwUzBoMGc-1
}

/*==========================================
 * +W/6MYTBu-MVPExp+MJKP1DBZ-
 *------------------------------------------
 */
int status_get_mexp(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return mobdb_search(((struct mob_data *)bl)->class_)->mexp;
	else if(bl->type == BL_PET && (struct pet_data *)bl)
		return mobdb_search(((struct pet_data *)bl)->class_)->mexp;

	return 0;
}

/*==========================================
 * +W/6MYTBuZXUwvzCkMNcwko/UMFk-
 *------------------------------------------
 */
int status_get_enemy_type(struct block_list *bl)
{
	nullpo_retr(EMY_ETC, bl);

	if( bl->type == BL_PC )
		return EMY_PC;
	else if( bl->type == BL_MOB && !(status_get_mode(bl)&MD_BOSS) )
		return EMY_MOB;
	else if( bl->type == BL_HOM )
		return EMY_HOM;
	else if( bl->type == BL_MERC )
		return EMY_MERC;

	return EMY_ETC;
}

/*==========================================
 * +W/6MYTBuZw2CcjCSj9QwWQ-
 *------------------------------------------
 */
short status_get_clothes_color(struct block_list *bl)
{
	short color = -1;

	nullpo_retr(-1, bl);

	if(bl->type == BL_PC) {
		color = ((struct map_session_data *)bl)->status.clothes_color;
	} else if(bl->type & (BL_MOB | BL_PET)) {
		int id = status_get_class(bl);
		if(id >= 0 && mob_is_pcview(id))
			color = mob_get_clothes_color(id);
	}
	if(color >= 0 && color < MAX_CLOTH_COLOR)
		return color;

	return -1;
}

/*==========================================
 * StatusChange+MG5T1l+X-
 *------------------------------------------
 */
struct status_change *status_get_sc(struct block_list *bl)
{
	nullpo_retr(NULL, bl);

	if(bl->type == BL_MOB && (struct mob_data *)bl)
		return &((struct mob_data*)bl)->sc;
	if(bl->type == BL_PC && (struct map_session_data *)bl)
		return &((struct map_session_data*)bl)->sc;
	if(bl->type == BL_HOM && (struct homun_data *)bl)
		return &((struct homun_data*)bl)->sc;
	if(bl->type == BL_MERC && (struct merc_data *)bl)
		return &((struct merc_data*)bl)->sc;
	if(bl->type == BL_ELEM && (struct elem_data *)bl)
		return &((struct elem_data*)bl)->sc;

	return NULL;
}

/*==========================================
 * +m1Rs1XEhUrkwSzBpMEYwSw-
 *------------------------------------------
 */
int status_check_no_magic_damage(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC)
	{
		if(((struct map_session_data*)bl)->special_state.no_magic_damage)
			return 1;
	}
	return 0;
}

/*==========================================
 * +crZhS3VwXjgwboAQYCeKCHuX-
 *------------------------------------------
 */
int status_change_rate(struct block_list *bl,int type,int rate,int src_level)
{
	int sc_flag = 0;
#ifndef PRE_RENEWAL
	int cap;
#endif

	nullpo_retr(0, bl);

	if(type < 0)	// type+MEw-0+ZypugDBuWDRUCFkxZVc-
		return 0;

	if(rate <= 0)	// +eLpzhzBM-0+TuVOCzBuMIIwbjBvWTFlVw-
		return 0;

#ifndef PRE_RENEWAL
	cap = src_level - status_get_lv(bl);
	if(cap <= 0)	// +W/6MYTBuMHswRjBMMOww2TDrMEya2DBEWDRUCDBv-0
		cap = 0;
	else
		cap = cap * cap / 5;
#endif

	switch(type) {	// +crZhS3VwXjiAEGAnMLkwxjD8ML8wuQ- rate+MG9OB1IGc4c-
#ifdef PRE_RENEWAL
		case SC_STONE:	// +d/NTFg-
		case SC_FREEZE:	// +Uc19UA-
			rate +-= src_level*10 - rate * status_get_mdef(bl)*10 / 1000 - status_get_luk(bl)*10 - status_get_lv(bl)*10;
			sc_flag = 1;
			break;
		case SC_STUN:	// +MLkwvzDz-
		case SC_SILENCE:	// +bIie2Q-
		case SC_POISON:	// +a9I-
		case SC_DPOISON:	// +cxtr0g-
		case SC_BLEED:	// +UfqIQA-
			rate +-= src_level*10 - rate * status_get_vit(bl)*10 / 1000 - status_get_luk(bl)*10 - status_get_lv(bl)*10;
			sc_flag = 1;
			break;
		case SC_SLEEP:	// +d2F3IA-
			rate +-= src_level*10 - rate * status_get_int(bl)*10 / 1000 - status_get_luk(bl)*10 - status_get_lv(bl)*10;
			sc_flag = 1;
			break;
		case SC_BLIND:	// +Zpee0g-
			rate +-= src_level*10 - rate * (status_get_vit(bl)*10 +- status_get_int(bl)*10) / 2000 - status_get_luk(bl)*10 - status_get_lv(bl)*10;
			sc_flag = 1;
			break;
		case SC_CURSE:	// +VGowRA-
			rate +-= src_level*10 - rate * status_get_luk(bl)*10 / 1000 - status_get_luk(bl)*10;
			sc_flag = 1;
			break;
		case SC_CONFUSION:	// +bfdOcQ-
			rate +-= status_get_luk(bl)*10 +- -(rate * (status_get_str(bl)*10 +- status_get_int(bl)*10) / 2000) - src_level*10 +- status_get_lv(bl)*10;
			sc_flag = 1;
			break;
#else
		case SC_STONE:	// +d/NTFg-
		case SC_FREEZE:	// +Uc19UA-
			rate = rate * (100 - (status_get_mdef(bl) - cap)) / 100;
			sc_flag = 1;
			break;
		case SC_STUN:	// +MLkwvzDz-
		case SC_POISON:	// +a9I-
		case SC_DPOISON:	// +cxtr0g-
			rate = rate * (100 - (status_get_vit(bl) - cap)) / 100;
			sc_flag = 1;
			break;
		case SC_SLEEP:	// +d2F3IA-
		case SC_BLEED:	// +UfqIQA-
			rate = rate * (100 - (status_get_agi(bl) - cap)) / 100;
			sc_flag = 1;
			break;
		case SC_BLIND:	// +Zpee0g-
		case SC_SILENCE:	// +bIie2Q-
			rate = rate * (100 - (status_get_int(bl) - cap)) / 100;
			sc_flag = 1;
			break;
		case SC_CONFUSION:	// +bfdOcQ-
		case SC_CURSE:	// +VGowRA-
			rate = rate * (100 - (status_get_luk(bl) - cap)) / 100;
			sc_flag = 1;
			break;
#endif
		case SC_TOXIN:
		case SC_PARALIZE:
		case SC_VENOMBLEED:
		case SC_MAGICMUSHROOM:
		case SC_DEATHHURT:
		case SC_PYREXIA:
		case SC_OBLIVIONCURSE:
		case SC_LEECHEND:
			if(bl->type == BL_PC)
				rate -= rate * pc_checkskill((struct map_session_data *)bl,GC_RESEARCHNEWPOISON) * 5 / 100;
			break;
		case SC_PARALYZE:
			rate = rate * (100 - (status_get_vit(bl)/5 +- status_get_luk(bl)/10)) / 100;
			break;
		case SC_HANDICAPSTATE_DEEPBLIND:		// +bwae0g-
		case SC_HANDICAPSTATE_DEEPSILENCE:		// +l1lbwg-
		case SC_HANDICAPSTATE_LASSITUDE:		// +cSFsF1Kb-
		case SC_HANDICAPSTATE_FROSTBITE:		// +YCVRtw-
		case SC_HANDICAPSTATE_SWOONING:			// +WTF5Xg-
		case SC_HANDICAPSTATE_LIGHTNINGSTRIKE:	// +b8BtQQ-
		case SC_HANDICAPSTATE_CRYSTALLIZATION:	// +fVBmdlMW-
		case SC_HANDICAPSTATE_CONFLAGRATION:	// +cGtwfQ-
		case SC_HANDICAPSTATE_MISFORTUNE:		// +Tg1eeA-
		case SC_HANDICAPSTATE_DEADLYPOISON:		// +Xzdr0g-
		case SC_HANDICAPSTATE_DEPRESSION:		// +YYKbMQ-
		case SC_HANDICAPSTATE_HOLYFLAME:		// +gFZwaw-
			rate = rate * (100 - (status_get_lv(bl) - src_level)) / 100;
			break;
		default:
			break;
	}
	if(sc_flag) {
		struct status_change *sc = status_get_sc(bl);
		if(sc) {
			if(sc->data[SC_STATUS_UNCHANGE].timer != -1 && status_is_disable(type,0x10)) {
				rate = 0;	// +MLQwuTDaMOswblFocrZhS3VwXjiAEGAnTi0wajCJcSFSuQ-
			}
			if(sc->data[SC_REFRESH].timer != -1 && status_is_disable(type,0x10)) {
				rate = 0;	// +MOow1TDsMMMwtzDlUrlnnE4tMG9xIVK5-
			}
			if(sc->data[SC_INSPIRATION].timer != -1 && status_is_disable(type,0x10)) {
				rate = 0;	// +MKQw8zC5MNQw7DD8MLcw5zDzUrlnnE4tMGowiXEhUrk-
			}
		}
	}

	return rate;
}

/*==========================================
 * +crZhS3VwXjgwbokHUZn/CDDHMMMwyTDqMPwwpDDzMNUwpzCvMMj/CQ-
 *------------------------------------------
 */
int status_change_copy(struct block_list *src,struct block_list *bl)
{
	int type;
	unsigned int tick;
	struct status_change *sc;
	struct TimerData *td = NULL;

	nullpo_retr(0, src);
	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(src));

	if(sc->count <= 0)
		return 0;
	if(sc->data[SC__DEADLYINFECT].timer != -1 && atn_rand()%100 > sc->data[SC__DEADLYINFECT].val1*20)
		return 0;

	tick = gettick();
	status_calc_pc_stop_begin(bl);

	for(type = 0; type < MAX_STATUSCHANGE; type+-+-) {
		if(scdata_db[type].copyable) {
			if(sc->data[type].timer != -1)
			{
				td = get_timer(sc->data[type].timer);
				if (td == NULL || td->func != status_change_timer || DIFF_TICK(td->tick,tick) < 0)
					continue;
				status_change_start(bl,type,sc->data[type].val1,sc->data[type].val2,sc->data[type].val3,sc->data[type].val4,DIFF_TICK(td->tick,tick),0);
			}
		}
	}
	status_calc_pc_stop_end(bl);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjgwxzD8ML8wblLVdoR4uk/d-
 *------------------------------------------
 */
#ifdef DYNAMIC_SC_DATA

int status_calloc_sc_data(struct status_change *sc)
{
	if(sc && sc->data == dummy_sc_data)
	{
		int i;
		sc->data = (struct status_change_data *)aCalloc(MAX_STATUSCHANGE, sizeof(struct status_change_data));
		for(i=0; i<MAX_STATUSCHANGE; i+-+-) {
			sc->data[i].timer = -1;
			sc->data[i].val1  = 0;
			sc->data[i].val2  = 0;
			sc->data[i].val3  = 0;
			sc->data[i].val4  = 0;
		}
		sc->count = 0;
		return 1;	// calloced
	}

	return 0;
}

int status_free_sc_data(struct status_change *sc)
{
	if(sc && sc->data != dummy_sc_data)
	{
		map_freeblock(sc->data);
		sc->data  = dummy_sc_data;
		sc->count = 0;
	}

	return 0;
}

#endif

/*==========================================
 * +MLkwxjD8ML8wuXVwXjiVi1nL-
 *------------------------------------------
 */
int status_change_start(struct block_list *bl,int type,int val1,int val2,int val3,int val4,int tick,int flag)
{
	struct map_session_data *sd  = NULL;
	struct mob_data         *md  = NULL;
	struct homun_data       *hd  = NULL;
	struct merc_data        *mcd = NULL;
	struct elem_data        *eld = NULL;
	struct status_change    *sc  = NULL;
	struct unit_data        *ud  = NULL;
	int icon_tick = tick, icon_val1 = 0, icon_val2 = 0, icon_val3 = 0, opt_flag = 0, calc_flag = 0, race, mode, elem;
	unsigned int current_tick = gettick();

	nullpo_retr(0, bl);

	if(type < 0)
		return 0;

	sc = status_get_sc(bl);
	if(sc == NULL)
		return 0;
	ud = unit_bl2ud(bl);
	if(ud == NULL)
		return 0;

	if(bl->type == BL_HOM && !battle_config.allow_homun_status_change)
	{
		if((type < SC_AVOID || (type > SC_SPEED && type < SC_PARALYZE) || type > SC_VOLCANIC_ASH) && type != SC_COMBO)
			return 0;
	}

	if(status_get_class(bl) == MOBID_EMPERIUM && type != SC_SAFETYWALL && type != SC_PNEUMA)
		return 0;

	race = status_get_race(bl);
	mode = status_get_mode(bl);
	elem = status_get_elem_type(bl);

	if(type == SC_AETERNA && (sc->data[SC_STONE].timer != -1 || sc->data[SC_FREEZE].timer != -1))
		return 0;

	// +cnlrinz7-
	if(type >= MAX_STATUSCHANGE) {
		switch(type) {
			case SC_SOUL:
				status_change_soulstart(bl,val1,val2,val3,val4,tick,flag);
				break;
			default:
				if(battle_config.error_log)
					printf("UnknownStatusChange [%d]+AFw-n", type);
				break;
		}
		return 0;
	}

	// ON/OFF
	switch(type) {
		case SC_AUTOBERSERK:
		case SC_READYSTORM:
		case SC_READYDOWN:
		case SC_READYTURN:
		case SC_READYCOUNTER:
		case SC_DODGE:
		case SC_FUSION:
		case SC_WEAPONBLOCKING:
		case SC_CLOAKINGEXCEED:
		//case SC_CAMOUFLAGE:
		case SC__REPRODUCE:
		case SC__INVISIBILITY:
		case SC_REFLECTDAMAGE:
		case SC_FORCEOFVANGUARD:
		case SC_SOULCOLLECT:		/* +m0IwboTEek0- */
			if(sc->data[type].timer != -1) {
				status_change_end(bl,type,-1);
				return 0;
			}
			break;
		case SC_MAGNIFICAT:
			status_change_end(bl,SC_OFFERTORIUM,-1);
			break;
		case SC_OFFERTORIUM:
			status_change_end(bl,SC_MAGNIFICAT,-1);
			break;
		case SC_MADNESSCANCEL:
			status_change_end(bl,SC_HEAT_BARREL,-1);
			status_change_end(bl,SC_P_ALTER,-1);
			break;
		case SC_HEAT_BARREL:
			status_change_end(bl,SC_P_ALTER,-1);
			status_change_end(bl,SC_MADNESSCANCEL,-1);
			break;
		case SC_P_ALTER:
			status_change_end(bl,SC_MADNESSCANCEL,-1);
			status_change_end(bl,SC_HEAT_BARREL,-1);
			break;
		case SC_LUNARSTANCE:
		case SC_UNIVERSESTANCE:
		case SC_SUNSTANCE:
		case SC_STARSTANCE:
			{
				int i;
				bool cancel = false;
				const int se_stance_list[] = { SC_LUNARSTANCE, SC_UNIVERSESTANCE, SC_SUNSTANCE, SC_STARSTANCE };
				const int se_light_list[] = { SC_LIGHTOFMOON, -1, SC_LIGHTOFSUN, SC_LIGHTOFSTAR };

				for( i = 0; i < sizeof(se_stance_list)/sizeof(se_stance_list[0]); i+-+- ){
					int sc_checking = se_stance_list[i];
					if(sc->data[sc_checking].timer != -1){
						status_change_end(bl,sc_checking,-1);
						if( sc_checking == type ){
							cancel = true;
						}
						sc_checking = se_light_list[i];
						if( sc_checking >= 0 && sc->data[sc_checking].timer != -1){
							status_change_end(bl,sc_checking,-1);
						}
					}
				}
				if( cancel ){
					return 0;
				}
			}
			break;
		// 3+ayFlsGvSMLkwrTDr-
		case SC_TOXIN:
		case SC_PARALIZE:
		case SC_VENOMBLEED:
		case SC_MAGICMUSHROOM:
		case SC_DEATHHURT:
		case SC_PYREXIA:
		case SC_OBLIVIONCURSE:
		case SC_LEECHEND:
			if(sc->data[SC_TOXIN].timer != -1 ||
			   sc->data[SC_PARALIZE].timer != -1 ||
			   sc->data[SC_VENOMBLEED].timer != -1 ||
			   sc->data[SC_MAGICMUSHROOM].timer != -1 ||
			   sc->data[SC_DEATHHURT].timer != -1 ||
			   sc->data[SC_PYREXIA].timer != -1 ||
			   sc->data[SC_OBLIVIONCURSE].timer != -1 ||
			   sc->data[SC_LEECHEND].timer != -1
			) {
				// +ZbBr0jBvkc2JBzBVMFswajBE-
				return 0;
			}
			break;
		// 3+ayFrTDC5MK0w6w- +cuxZTw-
		case SC_SWING:
		case SC_SYMPHONY_LOVE:
		case SC_MOONLIT_SERENADE:
		case SC_RUSH_WINDMILL:
		case SC_ECHOSONG:
		case SC_HARMONIZE:
		case SC_FRIGG_SONG:
			if(sc->data[SC_SWING].timer != -1)
				status_change_end(bl,SC_SWING,-1);
			if(sc->data[SC_SYMPHONY_LOVE].timer != -1)
				status_change_end(bl,SC_SYMPHONY_LOVE,-1);
			if(sc->data[SC_MOONLIT_SERENADE].timer != -1)
				status_change_end(bl,SC_MOONLIT_SERENADE,-1);
			if(sc->data[SC_RUSH_WINDMILL].timer != -1)
				status_change_end(bl,SC_RUSH_WINDMILL,-1);
			if(sc->data[SC_ECHOSONG].timer != -1)
				status_change_end(bl,SC_ECHOSONG,-1);
			if(sc->data[SC_HARMONIZE].timer != -1)
				status_change_end(bl,SC_HARMONIZE,-1);
			if(sc->data[SC_FRIGG_SONG].timer != -1)
				status_change_end(bl,SC_FRIGG_SONG,-1);
			break;
		// 3+ayFrTDC5MK0w6w- +VAhZTw-
		case SC_SIREN:
		case SC_SIRCLEOFNATURE:
		case SC_SONG_OF_MANA:
		case SC_DANCE_WITH_WUG:
		case SC_SATURDAY_NIGHT_FEVER:
		case SC_LERADS_DEW:
		case SC_MELODYOFSINK:
		case SC_BEYOND_OF_WARCRY:
		case SC_UNLIMITED_HUMMING_VOICE:
			if(battle_config.third_song_overlap) {	// +YiaV2IotW5owZ2tMMLkwrTDrkc2JB1PvgP0wblg0VAg-
				// +MOEw7TDHMKMw/DCqMNYwtzDzMK8waDDTMOgw8zDJMKow1jCmMKkw/DCvMOkwpDBvkc2JBzBVMFswajBE-
				if(type == SC_MELODYOFSINK && sc->data[SC_BEYOND_OF_WARCRY].timer != -1)
					status_change_end(bl,SC_BEYOND_OF_WARCRY,-1);
				if(type == SC_BEYOND_OF_WARCRY && sc->data[SC_MELODYOFSINK].timer != -1)
					status_change_end(bl,SC_MELODYOFSINK,-1);
				break;
			}
			if(sc->data[SC_SIREN].timer != -1)
				status_change_end(bl,SC_SIREN,-1);
			if(sc->data[SC_SIRCLEOFNATURE].timer != -1)
				status_change_end(bl,SC_SIRCLEOFNATURE,-1);
			if(sc->data[SC_SONG_OF_MANA].timer != -1)
				status_change_end(bl,SC_SONG_OF_MANA,-1);
			if(sc->data[SC_DANCE_WITH_WUG].timer != -1)
				status_change_end(bl,SC_DANCE_WITH_WUG,-1);
			if(sc->data[SC_SATURDAY_NIGHT_FEVER].timer != -1)
				status_change_end(bl,SC_SATURDAY_NIGHT_FEVER,-1);
			if(sc->data[SC_LERADS_DEW].timer != -1)
				status_change_end(bl,SC_LERADS_DEW,-1);
			if(sc->data[SC_MELODYOFSINK].timer != -1)
				status_change_end(bl,SC_MELODYOFSINK,-1);
			if(sc->data[SC_BEYOND_OF_WARCRY].timer != -1)
				status_change_end(bl,SC_BEYOND_OF_WARCRY,-1);
			if(sc->data[SC_UNLIMITED_HUMMING_VOICE].timer != -1)
				status_change_end(bl,SC_UNLIMITED_HUMMING_VOICE,-1);
			break;
		case SC_POWERFUL_FAITH:
		case SC_FIRM_FAITH:
		case SC_SINCERE_FAITH:
			if(sc->data[SC_POWERFUL_FAITH].timer != -1)
				status_change_end(bl,SC_POWERFUL_FAITH,-1);
			if(sc->data[SC_FIRM_FAITH].timer != -1)
				status_change_end(bl,SC_FIRM_FAITH,-1);
			if(sc->data[SC_SINCERE_FAITH].timer != -1)
				status_change_end(bl,SC_SINCERE_FAITH,-1);
			break;
		case SC_FIRST_FAITH_POWER:
			if(sc->data[SC_SECOND_JUDGE].timer != -1)
				status_change_end(bl,SC_SECOND_JUDGE,-1);
			if(sc->data[SC_THIRD_EXOR_FLAME].timer != -1)
				status_change_end(bl,SC_THIRD_EXOR_FLAME,-1);
			break;
		case SC_SECOND_JUDGE:
			if(sc->data[SC_FIRST_FAITH_POWER].timer != -1)
				status_change_end(bl,SC_FIRST_FAITH_POWER,-1);
			break;
		case SC_THIRD_EXOR_FLAME:
			if(sc->data[SC_SECOND_JUDGE].timer != -1)
				status_change_end(bl,SC_SECOND_JUDGE,-1);
			break;
		case SC_GUARD_STANCE:
			if(sc->data[SC_GUARD_STANCE].timer != -1) {
				status_change_end(bl,SC_GUARD_STANCE,-1);
				return 0;
			}
			if(sc->data[SC_ATTACK_STANCE].timer != -1)
				status_change_end(bl,SC_ATTACK_STANCE,-1);
			break;
		case SC_ATTACK_STANCE:
			if(sc->data[SC_ATTACK_STANCE].timer != -1) {
				status_change_end(bl,SC_ATTACK_STANCE,-1);
				return 0;
			}
			if(sc->data[SC_GUARD_STANCE].timer != -1)
				status_change_end(bl,SC_GUARD_STANCE,-1);
			break;
		case SC_INTENSIVE_AIM:		/* +MKQw8zDGMPMwtzDWMKgwpDDg- */
			if(sc->data[SC_INTENSIVE_AIM].timer != -1) {
				status_change_end(bl,SC_INTENSIVE_AIM,-1);
				return 0;
			}
			break;
	}

	sd  = BL_DOWNCAST( BL_PC,   bl );
	md  = BL_DOWNCAST( BL_MOB,  bl );
	hd  = BL_DOWNCAST( BL_HOM,  bl );
	mcd = BL_DOWNCAST( BL_MERC, bl );
	eld = BL_DOWNCAST( BL_ELEM, bl );

	if( sd ) {
		int weapon = sd->status.weapon;
		if(weapon >= WT_MAX)
			weapon -= WT_DOUBLE_DD +- WT_MAX;

		// +MKIwyTDsMMow6jDzMOkwwzC3MOUwbmtmVmhSJFua-
		if( type == SC_ADRENALINE && !(skill_get_weapontype(BS_ADRENALINE)&(1<<weapon)) )
			return 0;
		// +MNUw6zCiMMkw7DDKMOow8zDpMMMwtzDlMG5rZlZoUiRbmg-
		if( type == SC_ADRENALINE2 && !(skill_get_weapontype(BS_ADRENALINE2)&(1<<weapon)) )
			return 0;
		if( !(flag&8) && SC_STONE <= type && type <= SC_BLEED ) {	/* +MKsw/DDJMGswiDCLgBBgJw- */
			int scdef = sd->reseff[type-SC_STONE];
			if(sc->data[SC_SIEGFRIED].timer != -1) {	// +MLgw/DCvMNUw6jD8MMkwbnK2YUt1cF44gBBgJw-
				scdef +-= 5000;
			}
			if(scdef > 0 && atn_rand()%10000 < scdef) {
				if(battle_config.battle_log)
					printf("PC %d skill_sc_start: card+MGswiDCLdXBeOIAQYCd2elLVAFw-n", sd->bl.id);
				return 0;
			}
#ifndef PRE_RENEWAL
			tick = tick * (10000 - scdef) / 10000;
			if(tick <= 0)
				return 0;
#endif
		}
	}

	// +MKIw8zDHMMMwyTBvUc19UDD7d/NTFjD7UfqIQHEhUrk-
	if((race == RCT_UNDEAD || elem == ELE_UNDEAD) && !(flag&1) && (type == SC_STONE || type == SC_FREEZE || type == SC_BLEED))
		return 0;

	// +MKYwqTD8MLAw0DCkMMhytmFLTi0wbzDPMKQwxzCjMPMwsDABMK8w7TD8MK0w8zCwcSFSuQ-
	if(sc->data[SC_WUGBITE].timer != -1 && (type == SC_HIDING || type == SC_CLOAKING || type == SC_CLOAKINGEXCEED || type == SC_NEWMOON))
		return 0;

	// +MKYwqTD8MN4w/HK2YUtOLTBvUc19UDABbDd9UDABUbdRzXEhUrk-
	if(sc->data[SC_WARMER].timer != -1 && (type == SC_FREEZE || type == SC_FROSTMISTY || type == SC_DIAMONDDUST))
		return 0;

	// +bDhORZcccrZhS04tMG9zRHCOVGpxIVK5-
	if(sc->data[SC_CHILL].timer != -1 && type == SC_BURNT)
		return 0;

	// +iExS1U4NgP1ytmFLdXBeODBuUSpRSJgGT00-
	if(type >= SC_STONE && type < SC_SLEEP) {
		int i;
		for(i = type; i < SC_SLEEP; i+-+-) {
			if(sc->data[i+-1].timer != -1)
				return 0;
		}
	}

	if((type == SC_ADRENALINE || type == SC_ADRENALINE2 || type == SC_WEAPONPERFECTION || type == SC_OVERTHRUST) &&
		sc->data[type].timer != -1 && sc->data[type].val2 && !val2)
		return 0;

	// +MNwwuVxeYCcwazBvcSFSuQ-(+MF8wYDBXMKsw/DDJMGswiDCLUrlnnDBvkGl1KDBVMIwwiw-)
	if( (mode&MD_BOSS || mode&MD_STATUSCHANGEIMMUNITY) && !(flag&1) && status_is_disable(type,0x01) ) {
		if(type == SC_BLESSING && !battle_check_undead(race,elem) && race != RCT_DEMON) {
			// +MNYw7DC5MG9ODWt7-/+YKqbVDBnMGowRDBqMIlSuTBP-
			;
		} else {
			return 0;
		}
	}

	// +MK8woTCwMN4wpDCiTi0wb3EhUrk-
	if(sc->data[SC_QUAGMIRE].timer != -1 && status_is_disable(type,0x02))
		return 0;
	// +ecEwkl/YMIwwajBEMGdOLTBvcSFSuQ-
	if(sc->data[SC_DONTFORGETME].timer != -1 && status_is_disable(type,0x04))
		return 0;
	// +kB9epm4bXBFOLTBvcSFSuQ-
	if(sc->data[SC_DECREASEAGI].timer != -1 && status_is_disable(type,0x08))
		return 0;

	if(type == SC_STUN || type == SC_SLEEP)
		unit_stop_walking(bl,1);

	// +MNYw7DDDMLcw8zCwMGswiDCLVGowRDABd/NTFjBuieOWZA-
	if(type == SC_BLESSING && (sd || (!battle_check_undead(race,elem) && race != RCT_DEMON))) {
		bool f = false;
		if(sc->data[SC_CURSE].timer != -1) {
			status_change_end(bl,SC_CURSE,-1);
			f = true;
		}
		if(sc->data[SC_STONE].timer != -1 && sc->data[SC_STONE].val2 == 0) {
			status_change_end(bl,SC_STONE,-1);
			f = true;
		}

		// +VGowRDABd/NTFonjlmRmQjBvMNYw7DDDMLcw8zCwcrZhSzBrMGowiTBqMEQwbjBnfUIwjzCL-
		if(f)
			return 0;
	}

	/* +MK0w8zCwMLkwsDDsMKQwuXK2YUswZzBvcnlbmnVwXjgwazBLMEswiTBqMEQ- */
	if(sc->data[SC_KINGS_GRACE].timer != -1) {
		if(type >= SC_STONE && type <= SC_BLEED)
			return 0;
		switch(type) {
			case SC_DPOISON:	case SC_HALLUCINATION:	case SC_FEAR:		case SC_HELLINFERNO:
			case SC_FROSTMISTY:	case SC_DEEP_SLEEP:	case SC_DIAMONDDUST:	case SC_MANDRAGORA:
				return 0;
		}
	}
	/* +cnlbmg-MAP+MGcwbzC/MO0wpjBuULdytmFLMGswajCJMGowRA- */
	if(type == SC_BITESCAR && !map[bl->m].flag.pvp && !map[bl->m].flag.gvg && !map[bl->m].flag.turbo)
		return 0;

	if(sc->data[type].timer != -1) {	/* +MFkwZzBrVAwwWHVwXjgwazBqMGMwZjBEMItYNFQIML8wpDDeieOWZA- */
		if(type == SC_ALL_RIDING || type == SC_HAT_EFFECT) {
			status_change_end(bl,type,-1);
			return 0;
		}
		if(sc->data[type].val1 > val1 && type != SC_COMBO && type != SC_DANCING && type != SC_DEVOTION &&
			type != SC_SPEEDPOTION0 && type != SC_SPEEDPOTION1 && type != SC_SPEEDPOTION2 &&
			type != SC_DOUBLE && type != SC_TKCOMBO && type != SC_DODGE && type != SC_SPURT && type != SC_SEVENWIND &&
			type != SC_SHAPESHIFT && type != SC_ON_PUSH_CART && type != SC_TINDER_BREAKER && type != SC_CBC && type != SC_PYROCLASTIC &&
			type != SC_CLIMAX)
			return 0;
		if((type >= SC_STUN && type <= SC_BLIND) ||
			type == SC_DPOISON || type == SC_FOGWALLPENALTY || type == SC_FORCEWALKING || type == SC_ORATIO ||
			type == SC_FRESHSHRIMP || type == SC_HEAT_BARREL || type == SC_OVERED_BOOST)
			return 0;	/* +fZkwTo2zMFcwTDBnME0wajBEcrZhS3VwXjgwZzBCMItmQjBvcrZhS3VwXjgwkohMMI8wajBE- */
		if(type == SC_GRAFFITI || type == SC_SEVENWIND || type == SC_SHAPESHIFT || type == SC__AUTOSHADOWSPELL || type == SC_PROPERTYWALK || type == SC_PYROCLASTIC) {
			// +dXBeOE4tMGswgjBGTgBepnK2YUt1cF44MGswajBjMF9mQjBrieOWZDBXMGYwSzCJUY1epjBLMEswiw-
			status_change_end(bl,type,-1);
		} else {
			sc->count--;
			delete_timer(sc->data[type].timer, status_change_timer);
			sc->data[type].timer = -1;
#ifdef DYNAMIC_SC_DATA
			if(sc->count <= 0 && battle_config.free_sc_data_dynamically)
				status_free_sc_data(sc);
#endif
		}
	}

	switch(type) {	/* +dXBeODBuei6YXjBUMGgwblHmdAY- */
		case SC_DOUBLE:				/* +MMAw1jDrMLkwyDDsMKQw1TCjMPMwsA- */
		case SC_SUFFRAGIUM:			/* +MLUw1TDpMK4w4A- */
		case SC_MAGNIFICAT:			/* +MN4wsDDLMNUwozCrMPwwyA- */
		case SC_AETERNA:			/* +MKgw/DDGMOswyg- */
		case SC_BASILICA:			/* +MNAwuDDqMKs- */
		case SC_TRICKDEAD:			/* +a3swkzBgMHUwig- */
		case SC_STRIPWEAPON:
		case SC_STRIPSHIELD:
		case SC_STRIPARMOR:
		case SC_STRIPHELM:
		case SC_CP_WEAPON:
		case SC_CP_SHIELD:
		case SC_CP_ARMOR:
		case SC_CP_HELM:
		case SC_DEVOTION:			/* +MMcwozDcMPwwtzDnMPM- */
		case SC_COMBO:
		case SC_EXTREMITYFIST:			/* +lj9P7n+FiYdR8GLz- */
		case SC_RICHMANKIM:
		case SC_ROKISWEIL:			/* +MO0wrTBuU+swcw- */
		case SC_INTOABYSS:			/* +bfFt9TBuTi0waw- */
		case SC_POEMBRAGI:			/* +MNYw6TCuMG6KaQ- */
		case SC_ANKLE:				/* +MKIw8zCvMOs- */
		case SC_MAGNUM:				/* +MN4wsDDKMOAw1jDsMKQwrw- */
		case SC_TIGERFIST:			/* +Tw+GTmLz- */
		case SC_ENERGYCOAT:			/* +MKgwyjC4MPwwszD8MMg- */
		case SC_POEMBRAGI_:			/* +MNYw6TCuMG6KaQ- */
		case SC_FOGWALLPENALTY:
		case SC_FOGWALL:
		case SC_REVERSEORCISH:
		case SC_GRAVITATION_USER:
		case SC_BLADESTOP_WAIT:			/* +dn1SA1PWMIo-(+X4UwYQ-) */
		case SC_SAFETYWALL:			/* +MLsw/DDVMMYwozCmMKkw/DDr- */
		case SC_PNEUMA:				/* +MMsw5TD8MN4- */
		case SC_KEEPING:
		case SC_BARRIER:
		case SC_AURABLADE:			/* +MKow/DDpMNYw7DD8MMk- */
		case SC_HEADCRUSH:			/* +MNgwwzDJMK8w6TDDMLcw5Q- */
		case SC_MELTDOWN:			/* +MOEw6zDIMMAwpjDz- */
		case SC_SPLASHER:			/* +MNkwyjDgMLkw1zDpMMMwtzDjMPw- */
		case SC_GOSPEL:				/* +MLQwuTDaMOs- */
		case SC_STATUS_UNCHANGE:		/* +UWhytmFLdXBeOIAQYCc- */
		case SC_INCDAMAGE:			/* +iKswwDDhMPwwuA-%+TgpmBw- */
		case SC_PRESERVE:			/* +MNcw6jC2MPww1g- */
		case SC_REGENERATION:			/* +b8BSsQ- */
		case SC_BATTLEORDER_DELAY:
		case SC_REGENERATION_DELAY:
		case SC_RESTORE_DELAY:
		case SC_EMERGENCYCALL_DELAY:
		case SC_MEAL_INCEXP:
		case SC_MEAL_INCJOB:
		case SC_COMBATHAN:			/* +YiaV2GVZe8Q- */
		case SC_LIFEINSURANCE:			/* +dR9UfU/dlnqKPA- */
		case SC_JOB_COMBATHAN:		/* JOB+ZVl7xA- */
		case SC_FORCEWALKING:
		case SC_TKCOMBO:			/* +MMYwszDzMLMw8zDc- */
		case SC_TRIPLEATTACK_RATE_UP:
		case SC_COUNTER_RATE_UP:
		case SC_WARM:				/* +bikwgjCK- */
		case SC_KAIZEL:				/* +MKswpDC8MOs- */
		case SC_KAAHI:				/* +MKswojDS- */
		case SC_SMA:				/* +MKgwuTDe- */
		case SC_MIRACLE:			/* +WSqWfTBoZwgwaGYfMG5ZR43h- */
		case SC_ANGEL:				/* +WSqWfTBoZwgwaGYfMG5ZKU9/- */
		case SC_BABY:				/* +MNEw0TABMN4w3jABWSdZfTBN- */
		case SC_DODGE:
		case SC_DODGE_DELAY:
		case SC_DOUBLECASTING:			/* +MMAw1jDrMK0w4zC5MMYwozDzMLA- */
		case SC_SHRINK:				/* +MLcw5TDqMPMwrw- */
		case SC_TIGEREYE:
		case SC_PK_PENALTY:
		case SC_HERMODE:
		case SC_TATAMIGAESHI:			/* +dXOP1DBX- */
		case SC_NPC_DEFENDER:
		case SC_SLOWCAST:			/* +MLkw7TCmMK0w4zC5MMg- */
		case SC_CRITICALWOUND:			/* +gfRUfVC3- */
		case SC_MAGICMIRROR:			/* +MN4wuDDDMK8w3zDpMPw- */
		case SC_ITEMDROPRATE:			/* +MNAw1jDrMKww4A- */
		case SC_HAPPY:				/* +aX0wVzBEcrZhSw- */
		case SC_NATURAL_HEAL_STOP:		/* +gepxNlbeX6lQXGti- */
		case SC_REBIRTH:			/* +MOow0DD8MLk- */
		case SC_HELLPOWER:			/* +MNgw6zDRMO8w/A- */
		case SC_MANU_ATK:			/* +MN4wzDCvMNUwozD8MOswyQ-MOB(+cml0BjDAMOEw/DC4TgpmBw-) */
		case SC_MANU_DEF:			/* +MN4wzDCvMNUwozD8MOswyQ-MOB(+MMAw4TD8MLhuG1wR-) */
		case SC_MANU_MATK:			/* +MN4wzDCvMNUwozD8MOswyQ-MOB(+m1Rs1TDAMOEw/DC4TgpmBw-) */
		case SC_SPL_ATK:			/* +MLkw1zDsMPMwxjCjMMMwyTDVMKMw/DDrMMk-MOB(+cml0BjDAMOEw/DC4TgpmBw-) */
		case SC_SPL_DEF:			/* +MLkw1zDsMPMwxjCjMMMwyTDVMKMw/DDrMMk-MOB(+MMAw4TD8MLhuG1wR-) */
		case SC_SPL_MATK:			/* +MLkw1zDsMPMwxjCjMMMwyTDVMKMw/DDrMMk-MOB(+m1Rs1TDAMOEw/DC4bhtcEQ-) */
		case SC_ENCHANTBLADE:		/* +MKgw8zDBMOMw8zDIMNYw7DCkMMk- */
		case SC_REFRESH:			/* +MOow1TDsMMMwtzDl- */
		case SC_VITALITYACTIVATION:	/* +MNAwpDC/MOowxjCjMKIwrzDGMKMw2TD8MLcw5zDz- */
		case SC_WEAPONBLOCKING_POSTDELAY:	/* +MKYwpzDdMPMw1jDtMMMwrTDzMLD/CDDWMO0wwzCv/wk- */
		case SC_OFFERTORIUM:		/* +MKow1TCnMOswyDDqMKYw4A- */
		case SC_WHITEIMPRISON:		/* +MNsw7zCkMMgwpDDzMNcw6jC6MPM- */
		case SC_RECOGNIZEDSPELL:	/* +MOowtDCwMMowpDC6MMkwuTDaMOs- */
		case SC_STASIS:				/* +MLkwxjCkMLcwuQ- */
		case SC_SPELLBOOK:			/* +MLkw2jDrMNYwwzCv- */
		case SC_FEARBREEZE:			/* +MNUwozCiMPww1jDqMPwwug- */
		case SC_WUGBITE:			/* +MKYwqTD8MLAw0DCkMMg- */
		case SC_HOVERING:			/* +MNsw0DD8MOow8zCw- */
		case SC_STEALTHFIELD:		/* +MLkwxjDrMLkw1TCjMPww6zDJ- */
		case SC__DEADLYINFECT:		/* +MMcwwzDJMOow/DCkMPMw1TCnMK8wyA- */
		case SC__IGNORANCE:			/* +MN4wuTCrMOww/DDJ/xowpDCwMM4wojDpMPMwuQ- */
		case SC__MANHOLE:			/* +MN4w8zDbMPww6w- */
		case SC__ESCAPE:			/* +MKgwuTCxMPww1w- */
		case SC_FALLENEMPIRE:		/* +WSd+j10pY3Y- */
		case SC_CURSEDCIRCLE_USER:	/* +VGp+G5Zj-(+T391KIAF-) */
		case SC_CURSEDCIRCLE:		/* +VGp+G5Zj- */
		case SC_NETHERWORLD:		/* +VzBzRDBua0w- */
		case SC_UNLIMITED_HUMMING_VOICE:	/* +MKgw8zDJMOwwuTDPMN8w8zCwMNwwpDC5- */
		case SC_WARMER:				/* +MKYwqTD8MN4w/A- */
		case SC_VACUUM_EXTREME:		/* +MNAwrTDlMPww4DCoMK8wuTDIMOow/DDg- */
		case SC_THORNS_TRAP:		/* +ML0w/DDzMMgw6TDDMNc- */
		case SC_SPORE_EXPLOSION:	/* +MLkw3TCiMKgwrzC5MNcw7TD8MLgw5zDz- */
		case SC_KG_KAGEHUMI:		/* +XnuIUw- -+X3GODzB/-- */
		case SC_KYOMU:				/* +XnuIUw- -+hlpxITBuX3E-- */
		case SC_AKAITSUKI:			/* +XnuIUw- -+fQVnCA-- */
		case SC_KO_ZENKAI:			/* +iFNfDw- -+XFWViw-- */
		case SC_KO_JYUMONJIKIRI:	/* +U0Flh1tXZawwig- */
		case SC_E_CHAIN:			/* +MKgwvzD8MMow6zDBMKcw/DDz- */
		case SC_QD_SHOT_READY:		/* +MK8wpDDDMK8wyTDtMPwwtzDnMMMwyA-*/
		case SC_FALLEN_ANGEL:		/* +MNUwqTD8MOow8zCoMPMwuDCnMOs- */
		case SC_H_MINE:				/* +MM8wpjDqMPMwsDDeMKQw8w- */
		case SC_CIRCLE_OF_FIRE:		/* +MLUw/DCvMOswqjDWMNUwoTCkMKI- */
		case SC_TIDAL_WEAPON:		/* +ML8wpDDAMOswpjCnMN0w8w- */
		case SC_SU_STOOP:			/* +MEYwWjBPMH4wiw- */
		case SC_BITESCAR:			/* +ML8w7TCmMG5Qtw- */
		case SC_TUNAPARTY:			/* +MN4wsDDtMLcw/DDrMMk- */
		case SC_PROTECTIONOFSHRIMP:	/* +MKgw0zDRMPwwxjCjMPw- */
		case SC_SPIRITOFLAND:		/* +WSdXMDBum0I- */
		case SC_JP_EVENT01:
		case SC_JP_EVENT02:
		case SC_JP_EVENT03:
		case SC_JP_EVENT04:
		case SC_SUPPORT_EXP:
		case SC_RAID:
		case SC_STYLE_CHANGE:		/* +MLkwvzCkMOswwTCnMPMwuA- */
		case SC_PHI_DEMON:			/* +U+RO43y+lwowbjBKW4gwig- */
		case SC_MAXPAIN:			/* +MN4wwzCvMLkw2jCkMPM- */
		case SC_IMMUNE_PROPERTY_NOTHING:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MMsw5TD8MMgw6TDr-) */
		case SC_IMMUNE_PROPERTY_WATER:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MKYwqTD8ML8-) */
		case SC_IMMUNE_PROPERTY_GROUND:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MKIw/DC5-) */
		case SC_IMMUNE_PROPERTY_FIRE:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MNUwoTCkMKI-) */
		case SC_IMMUNE_PROPERTY_WIND	:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MKYwpDDzMMk-) */
		case SC_IMMUNE_PROPERTY_DARKNESS:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MMAw/DCv-) */
		case SC_IMMUNE_PROPERTY_SAINT:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MNsw/DDqMPw-) */
		case SC_IMMUNE_PROPERTY_POISON:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MN0wpDC6MPM-) */
		case SC_IMMUNE_PROPERTY_TELEKINESIS:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MLQw/DC5MMg-) */
		case SC_IMMUNE_PROPERTY_UNDEAD:	/* +MKQw3zDlMPww8zDXMO0w0TDGMKM-(+MKIw8zDHMMMwyA-) */
		case SC_PC_STOP:		/* +eftS1U4NU+8- */
		case SC_CRUSHSTRIKE:		/* +MK8w6TDDMLcw5TC5MMgw6TCkMK8- */
		case SC_WEAPONBLOCK_ON:		/* +MKswpjDzML8w/DC5MOkwwzC3MOU- */
		case SC__FEINTBOMB:			/* +MNUwpzCkMPMwyDDcMOA- */
		case SC_MAGICALBULLET:		/* +MN4wuDCrMOsw0DDsMMMwyA- */
		case SC_SOULCURSE:		/* +a3uXCmGRT50- */
		case SC_SWHOO:			/* +MKgwuTDVimBVMVPvgP1ytmFL- */
		case SC_SERVANT_SIGN:		/* +MLUw/DD0MKEw8zDIMLUwpDDz- */
		case SC_CHARGINGPIERCE:		/* +MMEw4zD8MLgw8zCwMNQwojD8MLk- */
		case SC_DRAGONIC_AURA:		/* +MMkw6TC0MMswwzCvMKow/DDp- */
		case SC_VIGOR:			/* +MPQwozC0MPww6w- */
		case SC_MASSIVE_F_BLASTER:	/* +cI5wa27Fm1R5Xl8+- */
		case SC_FIRST_BRAND:	/* +eyxOAGSD/xpw2VNw- */
		case SC_FIRST_FAITH_POWER:	/* +eyxOAHrg/xpP4V/1MG5Smw- */
		case SC_SECOND_JUDGE:	/* +eyxOjHrg/xpb6VIkgAU- */
		case SC_SECOND_BRAND:	/* +eyxOjGSD/xpw2VNw- */
		case SC_THIRD_EXOR_FLAME:	/* +ZwB9Qnrg/xpuxZtUMG5wjg- */
		case SC_GUARDIAN_S:		/* +MKww/DDHMKMwojDzMLcw/DDrMMk- */
		case SC_REBOUND_S:		/* +MOow0DCmMPMwyTC3MPww6zDJ- */
		case SC_ULTIMATE_S:		/* +MKIw6zDGMKMw4TDDMMgwtTCvMOow1TChMKQwuQ- */
		case SC_SPEAR_SCAR:		/* +MLAw6TDzMMkwuDDjMMMwuDDhMPMwyA- */
		case SC_SHIELD_POWER:	/* +MLcw/DDrMMkwtzDlMPwwxjCjMPMwsA- */
		case SC_SHADOW_EXCEED:	/* +MLcw4zDJMKYwqDCvMLcw/DDJ- */
		case SC_AXE_STOMP:		/* +MKIwwzCvMLkwuTDIMPMw1w- */
		case SC_ABR_BATTLE_WARIOR:	/* ABR +MNAwyDDrMKYwqTDqMKIw/A- */
		case SC_ABR_DUAL_CANNON:	/* ABR +MMcw5TCiMOswrTDjMM4w8w- */
		case SC_ABR_MOTHER_NET:		/* ABR +MN4wtjD8MM0wwzDI- */
		case SC_ABR_INFINITY:		/* ABR +MKQw8zDVMKMwyzDGMKM- */
		case SC_ABYSS_DAGGER:		/* +MKIw0zC5MMAwrDD8- */
		case SC_CALAMITYGALE:		/* +MKsw6TDfMMYwozCyMKQw6w- */
		case SC_CRESCIVEBOLT:		/* +MK8w7DDDMLcw1jDcMOswyA- */
		case SC_PROTECTSHADOWEQUIP:	/* +MNUw6zC3MOMwyTCmMMEw4zD8MLg- */
		case SC_RESEARCHREPORT:		/* +MOowtTD8MMEw7DDdMPwwyA- */
		case SC_BO_WOODENWARRIOR:	/* +MKYwyTClMPMwpjCpMOowojD8- */
		case SC_BO_WOODEN_FAIRY:	/* +MKYwyTClMPMw1TCnMKIw6jD8- */
		case SC_BO_CREEPER:			/* +MK8w6jD8MNEw/A- */
		case SC_BO_HELLTREE:		/* +MNgw6zDEMOow/A- */
		case SC_KVASIR_SONATA:		/* +ML0wyjC/MKow1jCvMPQwoTC3MOs- */
		case SC_ROSEBLOSSOM:		/* +MO0wvDDWMO0wwzC1MOA- */
		case SC_ELEMENTAL_VEIL:		/* +MKgw7DDhMPMwvzDrMPQwpzD8MOs- */
		case SC_H_MINE_SPLASH:		/* +MM8wpjDqMPMwsDDeMKQw8w-(+UgZlYw-) */
		case SC_T_FIRST_GOD:		/* +l1KfjXsm- */
		case SC_T_SECOND_GOD:		/* +dn2GTnsm- */
		case SC_T_THIRD_GOD:		/* +ZzGWwHsm- */
		case SC_T_FOURTH_GOD:		/* +c4RrZnsm- */
		case SC_HOGOGONG:			/* +ML8wpDCsMPwwzzCmMOow8zCw- */
		case SC_BLESSING_OF_M_C_DEBUFF:	/* +MGswgzCTMNYw7DDDMLcw8zCwMOow0DCmMPMwyQ- */
		case SC_BREAKINGLIMIT:		/* +MNYw7DCkMK0w8zCwMOow3zDDMMg- */
		case SC_RULEBREAK:			/* +MOsw/DDrMNYw7DCkMK8- */
		case SC_RISING_SUN:			/* +ZeVR+g- */
		case SC_NOON_SUN:			/* +a2NTSA- */
		case SC_SUNSET_SUN:			/* +ZeVsoQ- */
		case SC_RISING_MOON:		/* +ZwhR+g- */
		case SC_MIDNIGHT_MOON:		/* +a2NbUA- */
		case SC_DAWN_MOON:			/* +ZwhsoQ- */
		case SC_STAR_BURST:			/* +WSlUfYQ9Zh8- */
		case SC_NIGHTMARE:			/* +YKpZIg- */
			break;

		case SC_CONCENTRATE:			/* +lsZOLVKbVBFOCg- */
		case SC_BLESSING:			/* +MNYw7DDDMLcw8zCw- */
		case SC_ANGELUS:			/* +MKIw8zC8MOswuQ- */
		case SC_RESISTWATER:
		case SC_RESISTGROUND:
		case SC_RESISTFIRE:
		case SC_RESISTWIND:
		case SC_RESISTPOISON:
		case SC_RESISTHOLY:
		case SC_RESISTDARK:
		case SC_RESISTTELEKINESIS:
		case SC_RESISTUNDEAD:
		case SC_RESISTALL:
		case SC_IMPOSITIO:			/* +MKQw8zDdMLcwxjCjMKow3jDMMLk- */
		case SC_GLORIA:				/* +MLAw7TDqMKI- */
		case SC_LOUD:				/* +MOkwpjDJMNwwpDC5- */
		case SC_MINDBREAKER:			/* +MN4wpDDzMMkw1jDsMPwwqzD8- */
		case SC_ETERNALCHAOS:			/* +MKgwvzD8MMow6zCrMKowuQ- */
		case SC_WHISTLE:			/* +U+N7Gw- */
		case SC_ASSNCROS:			/* +WRWWfTBuMKIwtTC3MPMwrzDtMLk- */
		case SC_APPLEIDUN:			/* +MKQwyTClMPMwbmeXao4- */
		case SC_SANTA:
		case SC_SUMMER:
		case SC_TRUESIGHT:			/* +MMgwpTDrMPwwtTCkMMg- */
		case SC_SPIDERWEB:			/* +MLkw0TCkMMAw/DCmMKcwwzDW- */
		case SC_CONCENTRATION:			/* +MLMw8zC7MPMwyDDsMPwwtzDnMPM- */
		case SC_MARIONETTE:			/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- */
		case SC_MARIONETTE2:			/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- */
		case SC_WEDDING:			/* +fVBaWnUo-(+fVBaWohjiPMwazBqMGMwZmtpME8wbjBMkEUwRDBoMEs-) */
		case SC_HUMMING:			/* +MM8w3zDzMLA- */
		case SC_FORTUNE:			/* +XniQSzBuMK0wuQ- */
		case SC_SERVICE4U:			/* +MLUw/DDTMLkw1TCpMPww5jD8- */
		case SC_WHISTLE_:			/* +U+N7Gw- */
		case SC_ASSNCROS_:			/* +WRWWfTBuMKIwtTC3MPMwrzDtMLk- */
		case SC_APPLEIDUN_:			/* +MKQwyTClMPMwbmeXao4- */
		case SC_HUMMING_:			/* +MM8w3zDzMLA- */
		case SC_DONTFORGETME_:			/* +ecEwkl/YMIwwajBEMGc- */
		case SC_FORTUNE_:			/* +XniQSzBuMK0wuQ- */
		case SC_SERVICE4U_:			/* +MLUw/DDTMLkw1TCpMPww5jD8- */
		case SC_INCATK:				/* ATK+TgpmBw- (+eV6RUnUo-) */
		case SC_INCMATK:			/* MATK+TgpmBw- (+eV552DBug0l1KA-) */
		case SC_INCHIT:				/* HIT+TgpmBw- */
		case SC_INCMHP2:			/* MHP%+TgpmBw- */
		case SC_INCMSP2:			/* MSP%+TgpmBw- */
		case SC_INCATK2:			/* ATK%+TgpmBw- */
		case SC_INCHIT2:			/* HIT%+TgpmBw- */
		case SC_INCFLEE2:			/* FLEE%+TgpmBw- */
		case SC_INCALLSTATUS:			/* +UWgwuTDGMPwwvzC5/ws-20 */
		case SC_CHASEWALK_STR:			/* STR+TgpmBw- */
		case SC_BATTLEORDER:			/* +gehiJmFLUuI- */
		case SC_THE_MAGICIAN:
		case SC_STRENGTH:
		case SC_THE_DEVIL:
		case SC_THE_SUN:
		case SC_SPURT:				/* +mcYwUY2zdSg-STR */
		case SC_SUN_COMFORT:			/* +WSqWfTBuW4lpfQ- */
		case SC_MOON_COMFORT:			/* +ZwgwbluJaX0- */
		case SC_STAR_COMFORT:			/* +Zh8wbluJaX0- */
		case SC_FUSION:				/* +WSqWfTBoZwgwaGYfMG6HjVQI- */
		case SC_MEAL_INCHIT:	// +mN9Oi3Uo-
		case SC_MEAL_INCFLEE:
		case SC_MEAL_INCFLEE2:
		case SC_MEAL_INCCRITICAL:
		case SC_MEAL_INCDEF:
		case SC_MEAL_INCMDEF:
		case SC_MEAL_INCATK:
		case SC_MEAL_INCMATK:
		case SC_SKE:				/* +MKgwuTCv- */
		case SC_SKA:				/* +MKgwuTCr- */
		case SC_CLOSECONFINE:			/* +MK8w7TD8MLowszDzMNUwoTCkMPM- */
		case SC_STOP:				/* +MNsw/DDrMMkwpjCnMNY- */
		case SC_FLING:				/* +MNUw6TCkMPMwsA- */
		case SC_MADNESSCANCEL:			/* +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A- */
		case SC_ADJUSTMENT:			/* +MKIwuDDjMLkwyDDhMPMwyA- */
		case SC_INCREASING:			/* +MKQw8zCvMOow/DC4MPMwsDCiMK0w5TDpMLcw/A- */
		case SC_NEN:				/* +X/U- */
		case SC_AVOID:				/* +fcpgJVbekH8- */
		case SC_CHANGE:				/* +MOEw8zC/MOswwTCnMPMwuA- */
		case SC_DEFENCE:			/* +MMcwozDVMKcw8zC5- */
		case SC_BLOODLUST:			/* +MNYw6TDDMMkw6TC5MMg- */
		case SC_FLEET:				/* +MNUw6jD8MMgw4DD8MNY- */
		case SC_SPEED:				/* +MKow/DDQMPwwyTC5MNQw/DDJ- */
		case SC_STONESKIN:			/* +MLkwyDD8MPMwuTCtMPM- */
		case SC_ANTIMAGIC:			/* +MKIw8zDBMN4wuDDDMK8- */
		case SC_WEAPONQUICKEN:			/* +MKYwpzDdMPMwrzCkMMMwsTDz- */
		case SC_WE_FEMALE:			/* +MEIwajBfMGtcPTBPMFcwfjBZ- */
		case SC_GIANTGROWTH:		/* +MLgw4zCkMKIw8zDIMLAw7TD8MLk- */
		case SC_INFRAREDSCAN:		/* +MKQw8zDVMOkw7DDDMMkwuTCtMOMw8w- */
		case SC_ANALYZE:			/* +MKIwyjDpMKQwug- */
		case SC_NEUTRALBARRIER_MASTER:	/* +MMsw5TD8MMgw6TDrMNAw6jCiMPw-(+T391KIAF-) */
		case SC_NEUTRALBARRIER:		/* +MMsw5TD8MMgw6TDrMNAw6jCiMPw- */
		case SC__BODYPAINT:			/* +MNwwxzCjMNowpDDzMMYwozDzMLA- */
		case SC__ENERVATION:		/* +MN4wuTCrMOww/DDJ/xowqDDKMNkw/DC3MOcw8w- */
		case SC__UNLUCKY:			/* +MN4wuTCrMOww/DDJ/xowojDzMOkwwzCtMPw- */
		case SC__WEAKNESS:			/* +MN4wuTCrMOww/DDJ/xowpjCjMPwwrzDNMLk- */
		case SC__STRIPACCESSARY:	/* +MLkwyDDqMMMw1zCiMK8wuzC1MOow/A- */
		case SC__BLOODYLUST:		/* +MNYw6TDDMMcwozDpMLkwyA- */
		case SC_EARTHDRIVE:			/* +MKIw/DC5MMkw6TCkMNY- */
		case SC_HARMONIZE:			/* +MM8w/DDiMMowpDC6- */
		case SC_GLOOMYDAY:			/* +MOEw6TDzMLMw6jD8- */
		case SC_LERADS_DEW:			/* +MOww/DDpMLowbpcy- */
		case SC_DANCE_WITH_WUG:		/* +MMAw8zC5MKYwozC6MKYwqTD8MLA- */
		case SC_ILLUSIONDOPING:		/* +MKQw6jDlMPwwuDDnMPMwyTD8MNQw8zCw- */
		case SC_NYANGGRASS:			/* +MMsw4zDzMLAw6TC5- */
		case SC_MYSTERIOUS_POWDER:	/* +Tg1gHYtwMGp8iQ- */
		case SC_BOOST500:			/* +MNYw/DC5MMg-500 */
		case SC_FULL_SWING_K:		/* +MNUw6zC5MKQw8zCw-K */
		case SC_MANA_PLUS:			/* +MN4wyjDXMOkwuQ- */
		case SC_MUSTLE_M:			/* +MN4wwzC5MOs-M */
		case SC_LIFE_FORCE_F:		/* +MOkwpDDVMNUwqTD8MLk-F */
		case SC_PROMOTE_HEALTH_RESERCH:	/* HP+WJdSoDDdMPwwtzDnMPM- */
		case SC_ENERGY_DRINK_RESERCH:	/* SP+WJdSoDDdMPwwtzDnMPM- */
		case SC_EXTRACT_WHITE_POTION_Z:	/* +b8N+LjDbMO8wpDDIMN0w/DC3MOcw8w-Z */
		case SC_VITATA_500:			/* +MNMwvzC/-500 */
		case SC_EXTRACT_SALAMINE_JUICE:	/* +b8N+LjC1MOkw3jCkMPMwuDDlMPwwuQ- */
		case SC_SAVAGE_STEAK:		/* +MLUw2TD8MLgwbk44cTwwTQ- */
		case SC_COCKTAIL_WARG_BLOOD:	/* +MKswrzDGMOswpjCpMPwwsDDWMOkwwzDJ- */
		case SC_MINOR_BBQ:			/* +MN8wzjC/MKYw7TC5MG5yWzCrMOsw0w- */
		case SC_SIROMA_ICE_TEA:		/* +MLcw7TDeMKIwpDC5MMYwozD8- */
		case SC_DROCERA_HERB_STEAMED:	/* +MMkw7TC7MOkwbjDPMPww1nFu- */
		case SC_PUTTI_TAILS_NOODLES:	/* +MNcwxjCjMMMwyDBuMFcwYzB9nro- */
		case SC_STOMACHACHE:		/* +gXl12w- */
		case SC_ODINS_POWER:		/* +MKow/DDHMKMw8zBuUps- */
		case SC_ZEPHYR:				/* +MLww1TChMPw- */
		case SC_INVINCIBLE:			/* +MKQw8zDTMPMwtzDWMOs- */
		case SC_INVINCIBLEOFF:		/* +MKQw8zDTMPMwtzDWMOswqjDV- */
		case SC_ATKPOTION:
		case SC_MATKPOTION:
		case SC_ALMIGHTY:
		case SC_SUPPORT_HPSP:
		case SC_TINDER_BREAKER:		/* +Y1Vzcg- */
		case SC_ALL_STAT_DOWN:	/* +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w- */
		case SC_ADORAMUS:		/* +MKIwyTDpMOAwuQ- */
		case SC_DEADLY_DEFEASANCE:		/* +MMcwwzDJMOow/DDXMO0wuDCnMK8wtzDnMPM- */
			calc_flag = 1;
			break;

		case SC_MONSTER_TRANSFORM:	/* +MOIw8zC5ML8w/FkJjqs- */
			icon_val1 = val1;	// val1+MG8w4jDzMLkwvzD8-ID
			calc_flag = 1;
			break;
		case SC_ALL_RIDING:			/* +mg5OVzC3MLkwxjDg- */
			if(sd) {
				// +ZeIwa2XiW1gwbk5XMIpyaTBrZC1OV04tMGcwQjCLWDRUCDBvT1UwgjBXMGowRA-
				if(pc_isriding(sd) || pc_isdragon(sd) || pc_iswolfmount(sd) || pc_isgear(sd))
					return 0;
				icon_val1 = 1;	// val1+MG8-1
				icon_val2 = 25;	// val2+MG8-25(+eftS1ZAfXqZUEU4K-)
				clif_status_load_id(sd,SI_RIDING,1);
			}
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_KYOUGAKU:			/* +XnuIUw- -+mlphFQ-- */
			icon_val1 = 1002;
			calc_flag = 1;
			break;
		case SC_HAT_EFFECT:			/* +mC2IxVCZMKgw1TCnMK8wyA- */
			icon_val1 = val1;
			break;
		case SC_ACTIVE_MONSTER_TRANSFORM:	/* +MKIwrzDGMKMw1jDiMPMwuTC/MPxZCY6r- */
			icon_val1 = val1;	// val1+MG8w4jDzMLkwvzD8-ID
			break;

		case SC_SPEEDUP0:			/* +eftS1ZAfXqZYl1Kg-(+MKIwpDDGMOA-) */
		case SC_SPEEDUP1:			/* +MLkw1DD8MMkw3TD8MLcw5zDz- */
		case SC_WALKSPEED:			/* +eftS1ZAfXqZYl1Kg-(+MLkwrzDqMNcwyA-) */
		case SC_SLOWPOTION:			/* +eftS1ZAfXqZPTk4L-(+MKIwpDDGMOA-) */
		case SC_STEELBODY:			/* +kdFSWw- */
		case SC_INCFLEE:			/* FLEE+TgpmBw- */
		case SC_GRAVITATION:			/* +MLAw6TDTMMYw/DC3MOcw8zDVMKMw/DDrMMk- */
		case SC_SUITON:				/* +bDSQQQ- */
		case SC_GATLINGFEVER:			/* +MKwwyDDqMPMwsDDVMKMw/DDQMPw- */
		case SC_HALLUCINATIONWALK:	/* +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv- */
		case SC_PARALIZE:			/* +MNEw6TDpMKQwug- */
		case SC_FROSTMISTY:			/* +MNUw7TC5MMgw3zC5MMYwow- */
		case SC_WUGDASH:			/* +MKYwqTD8MLAwwDDDMLcw5Q- */
		case SC_ACCELERATION:		/* +MKIwrzC7MOkw7DD8MLcw5zDz- */
		case SC_HALLUCINATIONWALK_POSTDELAY:	/* +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-(+MNowyjDrMMYwow-) */
		case SC__GROOMY:			/* +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8- */
		case SC__LAZINESS:			/* +MN4wuTCrMOww/DDJ/xow7DCkMLgwzTC5- */
		case SC_GN_CARTBOOST:		/* +MKsw/DDIMNYw/DC5MMg- */
		case SC_MELON_BOMB:			/* +MOEw7TDzcgZfPg- */
		case SC_POWER_OF_GAIA:		/* +MNEw7zD8MKow1jCsMKQwog- */
		case SC_SUPPORT_SPEED:
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;

		case SC_PROVOKE:			/* +MNcw7TDcMMMwrw- */
			calc_flag = 1;
			if(tick <= 0) tick = 1000;	/* (+MKow/DDIMNAw/DC1MPwwrw-) */
			break;
		case SC_ENDURE:				/* +MKQw8zDHMOUwog- */
			if(tick <= 0) tick = 1000 * 60;
			calc_flag = 1;
			val2 = 7;	// 7+Vt5lO2SDMFUwjDBfMImJ45Zk-
			if(sd && !map[bl->m].flag.gvg) {
				// +iKswxzCjMNww/DC3MOcw84AFMIIwpDDzMMcw5TCiMGswWTCL-
				struct map_session_data *tsd;
				int i;
				for(i = 0; i < 5; i+-+-) {
					if(sd->dev.val1[i] && (tsd = map_id2sd(sd->dev.val1[i])) != NULL)
						status_change_start(&tsd->bl,type,val1,0,0,0,skill_get_time(SM_ENDURE,val1),0);
				}
			}
			break;
		case SC_INCREASEAGI:		/* +kB9epliXUqA- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			if(sc->data[SC_DECREASEAGI].timer != -1)
				status_change_end(bl,SC_DECREASEAGI,-1);
			break;
		case SC_DECREASEAGI:		/* +kB9epm4bXBE- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_SIGNUMCRUCIS:		/* +MLcwsDDKMOAwrzDrMLcwuQ- */
			calc_flag = 1;
			val2 = 10 +- val1*4;
			tick = 600*1000;
			clif_emotion(bl,4);
			break;
		case SC_SLOWPOISON:
			if (sc->data[SC_POISON].timer == -1 && sc->data[SC_DPOISON].timer == -1)
				return 0;
			break;
		case SC_ONEHAND:			/* 1HQ */
			if(sc->data[SC_SPEEDPOTION0].timer != -1)
				status_change_end(bl,SC_SPEEDPOTION0,-1);
			if(sc->data[SC_SPEEDPOTION1].timer != -1)
				status_change_end(bl,SC_SPEEDPOTION1,-1);
			if(sc->data[SC_SPEEDPOTION2].timer != -1)
				status_change_end(bl,SC_SPEEDPOTION2,-1);
			calc_flag = 1;
			break;
		case SC_ADRENALINE:			/* +MKIwyTDsMMow6jDzMOkwwzC3MOU- */
			calc_flag = 1;
			if(sc->data[SC_ADRENALINE2].timer != -1)
				status_change_end(bl,SC_ADRENALINE2,-1);
			if(!(flag&2) && sd && pc_checkskill(sd,BS_HILTBINDING) > 0)
				icon_tick = tick +-= tick / 10;
			break;
		case SC_ADRENALINE2:			/* +MNUw6zCiMMkw7DDKMOow8zDpMMMwtzDl- */
			calc_flag = 1;
			if(sc->data[SC_ADRENALINE].timer != -1)
				status_change_end(bl,SC_ADRENALINE,-1);
			if(!(flag&2) && sd && pc_checkskill(sd,BS_HILTBINDING) > 0)
				icon_tick = tick +-= tick / 10;
			break;
		case SC_WEAPONPERFECTION:		/* +MKYwpzDdMPMw0TD8MNUwpzCvMLcw5zDz- */
			if(!(flag&2) && sd && pc_checkskill(sd,BS_HILTBINDING) > 0)
				icon_tick = tick +-= tick / 10;
			break;
		case SC_OVERTHRUST:			/* +MKow/DDQMPwwyDDpMLkwyA- */
			if(sc->data[SC_OVERTHRUSTMAX].timer != -1)
				return 0;
			val3 = (val2)? val1*5 : val1;
			if(!(flag&2) && sd && pc_checkskill(sd,BS_HILTBINDING) > 0)
				icon_tick = tick +-= tick / 10;
			break;
		case SC_MAXIMIZEPOWER:		/* +MN4wrTC3MN4wpDC6MNEw7zD8- */
			if(sd)
				tick = val2;	// SP+MEw-1+bhswi2ZClZM-
			else
				tick = 5000*val1;
			break;
		case SC_ENCPOISON:			/* +MKgw8zDBMOMw8zDIMN0wpDC6MPM- */
			calc_flag = 1;
			val2 = (((val1 - 1) / 2) +- 3)*100;	// +a9JO2E4OeLpzhw-
			status_enchant_elemental_end(bl,SC_ENCPOISON);
			break;
		case SC_EDP:			/* +MKgw8zDBMOMw8zDIMMcwwzDJMOow/DDdMKQwujDz- */
			val2 = 250 +- val1 * 50;	// +cxtr0k7YTg54unOH-(%)
			break;
		case SC_POISONREACT:	/* +MN0wpDC6MPMw6jCiMK8wyA- */
			val2 = (val1 +- 1) / 2;
			break;
		case SC_ASPERSIO:			/* +MKIwuTDaMOswtzCq- */
			status_enchant_elemental_end(bl,SC_ASPERSIO);
			break;
		case SC_BENEDICTIO:			/* +gFZPUw- */
			status_enchant_armor_elemental_end(bl,SC_BENEDICTIO);
			break;
		case SC_ELEMENTWATER:		// +bDQ-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTWATER);
			if(sd) {
				// +ljJRdzBrbDRcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(195));
			}
			break;
		case SC_ELEMENTGROUND:		// +Vx8-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTGROUND);
			if(sd) {
				// +ljJRdzBrVx9cXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(196));
			}
			break;
		case SC_ELEMENTFIRE:		// +cGs-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTFIRE);
			if(sd) {
				// +ljJRdzBrcGtcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(197));
			}
			break;
		case SC_ELEMENTWIND:		// +mKg-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTWIND);
			if(sd) {
				// +ljJRdzBrmKhcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(198));
			}
			break;
		case SC_ELEMENTHOLY:		// +UUk-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTHOLY);
			if(sd) {
				// +ljJRdzBrgFZcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(199));
			}
			break;
		case SC_ELEMENTDARK:		// +lcc-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTDARK);
			if(sd) {
				// +ljJRdzBrlcdcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(200));
			}
			break;
		case SC_ELEMENTELEKINESIS:	// +X/U-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTELEKINESIS);
			if(sd) {
				// +ljJRdzBrX/VcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(201));
			}
			break;
		case SC_ELEMENTPOISON:		// +a9I-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTPOISON);
			if(sd) {
				// +ljJRdzBra9JcXmAnMExO2E4OMFUwjDB+MFcwXzAC-
				clif_displaymessage(sd->fd, msg_txt(202));
			}
			break;
		case SC_ELEMENTUNDEAD:		// +Tg1rew-
			status_enchant_armor_elemental_end(bl,SC_ELEMENTUNDEAD);
			if(sc->data[SC_BLESSING].timer != -1)
				status_change_end(bl,SC_BLESSING,-1);
			if(sc->data[SC_INCREASEAGI].timer != -1)
				status_change_end(bl,SC_INCREASEAGI,-1);
			break;
		case SC_RACEUNKNOWN:
		case SC_RACEUNDEAD:
		case SC_RACEBEAST:
		case SC_RACEPLANT:
		case SC_RACEINSECT:
		case SC_RACEFISH:
		case SC_RACEDEVIL:
		case SC_RACEHUMAN:
		case SC_RACEANGEL:
		case SC_RACEDRAGON:
			status_change_race_end(bl,type);
			if(sd) {
				const char *race_name[] = { "+cSFfYg-", "+Tg1rew-", "+UtVyaQ-", "+aQ1yaQ-", "+ZgaGaw-", "", "+m1qMnQ-", "+YKqbVA-", "+TrqVkw-", "+WSlPfw-", "+etw-" };
				msg_output(sd->fd, msg_txt(204), race_name[type-SC_RACEUNKNOWN]);	// +ei5lzzBM-%s+MGswajCKMH4wVzBf-
			}
			break;
		case SC_MAGICROD:
			val2 = val1*20;
			break;
		case SC_KYRIE:				/* +MK0w6jCoMKgw7DCkML0w8w- */
			// +MKIwuTDgMExjmzBLMGMwZjBEMF8wiYnjlmQwVzBm-
			if(sc->data[SC_ASSUMPTIO].timer != -1)
				status_change_end(bl,SC_ASSUMPTIO,-1);
			if(sc->data[SC_ASSUMPTIO2].timer != -1)
				status_change_end(bl,SC_ASSUMPTIO2,-1);
			// +MK0w6jCoMJJjmzBRMIs-
			if(val4 > 0) {		// +MNcw6TCoMNUwoTDGMKMwqjBuWDRUCA-
				val2 = (int)((atn_bignumber)status_get_max_hp(bl) * (val1 * 2 +- 16 +- val4 * 2) / 100);	// +gBBORV6m-
				val3 = val1 +- 6;	// +Vt5lcA-
			} else {
				val2 = (int)((atn_bignumber)status_get_max_hp(bl) * (val1 * 2 +- 10) / 100);	// +gBBORV6m-
				val3 = val1 / 2 +- 5;	// +Vt5lcA-
			}
			break;
		case SC_QUAGMIRE:			/* +MK8woTCwMN4wpDCi- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			if(sc->data[SC_CONCENTRATE].timer != -1)
				status_change_end(bl,SC_CONCENTRATE,-1);
			if(sc->data[SC_INCREASEAGI].timer != -1)
				status_change_end(bl,SC_INCREASEAGI,-1);
			if(sc->data[SC_TWOHANDQUICKEN].timer != -1)
				status_change_end(bl,SC_TWOHANDQUICKEN,-1);
			if(sc->data[SC_SPEARQUICKEN].timer != -1)
				status_change_end(bl,SC_SPEARQUICKEN,-1);
			if(sc->data[SC_ADRENALINE].timer != -1)
				status_change_end(bl,SC_ADRENALINE,-1);
			if(sc->data[SC_ADRENALINE2].timer != -1)
				status_change_end(bl,SC_ADRENALINE2,-1);
			if(sc->data[SC_LOUD].timer != -1)
				status_change_end(bl,SC_LOUD,-1);
			if(sc->data[SC_WINDWALK].timer != -1)
				status_change_end(bl,SC_WINDWALK,-1);
			if(sc->data[SC_CARTBOOST].timer != -1)
				status_change_end(bl,SC_CARTBOOST,-1);
			if(sc->data[SC_ONEHAND].timer != -1)
				status_change_end(bl,SC_ONEHAND,-1);
			if(sc->data[SC_SPEEDUP1].timer != -1)
				status_change_end(bl,SC_SPEEDUP1,-1);
			break;
		case SC_MAGICPOWER:			/* +m1Rs1VKbWJdeRQ- */
			val2 = 1;	// +TgBepjBgMFFYl15F-
			break;
		case SC_SACRIFICE:			/* +MLUwrzDqMNUwoTCkMLk- */
			val2 = 5;	// 5+Vt4wbmU7ZIMwZ2cJUrk-
			break;
		case SC_FLAMELAUNCHER:		/* +MNUw7DD8MOAw6TDzMMEw4zD8- */
		case SC_FROSTWEAPON:		/* +MNUw7TC5MMgwpjCnMN0w8w- */
		case SC_LIGHTNINGLOADER:	/* +MOkwpDDIMMsw8zCwMO0w/DDAMPw- */
		case SC_SEISMICWEAPON:		/* +MLUwpDC6MN8wwzCvMKYwpzDdMPM- */
		case SC_DARKELEMENT:		/* +lcdcXmAn- */
		case SC_ATTENELEMENT:		/* +X/VcXmAn- */
		case SC_UNDEADELEMENT:		/* +Tg1re1xeYCc- */
			status_enchant_elemental_end(bl,type);
			break;
		case SC_SEVENWIND:			/* +ZpYwSzBEmKg- */
			status_enchant_elemental_end(bl,SC_ENCPOISON);	// +MKgw8zDBMOMw8zDIMN0wpDC6MPMwb5HNiQcwVzBmMIIwiDBE/x8-
#ifdef PRE_RENEWAL
			switch(val1) {
				case 1:  val2 = SI_SEISMICWEAPON;   val3 = ELE_EARTH; break;
				case 2:  val2 = SI_LIGHTNINGLOADER; val3 = ELE_WIND;  break;
				case 3:  val2 = SI_FROSTWEAPON;     val3 = ELE_WATER; break;
				case 4:  val2 = SI_FLAMELAUNCHER;   val3 = ELE_FIRE;  break;
				case 5:  val2 = SI_ATTENELEMENT;    val3 = ELE_GHOST; break;
				default: val2 = SI_DARKELEMENT;     val3 = ELE_DARK;  break;
			}
#else
			switch(val1) {
				case 1:  val2 = SI_SEISMICWEAPON;   val3 = ELE_EARTH; break;
				case 2:  val2 = SI_LIGHTNINGLOADER; val3 = ELE_WIND;  break;
				case 3:  val2 = SI_FROSTWEAPON;     val3 = ELE_WATER; break;
				case 4:  val2 = SI_FLAMELAUNCHER;   val3 = ELE_FIRE;  break;
				case 5:  val2 = SI_ATTENELEMENT;    val3 = ELE_GHOST; break;
				case 6:  val2 = SI_DARKELEMENT;     val3 = ELE_DARK;  break;
				default: val2 = SI_ASPERSIO;        val3 = ELE_HOLY;  break;
			}
#endif
			if(sd) {
				clif_status_change(bl,val2,1,icon_tick,0,0,0);
			}
			break;
		case SC_PROVIDENCE:			/* +MNcw7TD0MKMwxzDzMLk- */
			calc_flag = 1;
			val2 = val1*5;
			break;
		case SC_REFLECTSHIELD:
			// +MOow1TDsMK8wyDDAMOEw/DC4MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_REFLECTDAMAGE].timer != -1)
				status_change_end(bl,SC_REFLECTDAMAGE,-1);
			val2 = 10+-val1*3;
			if(sd) {
				// +iKswxzCjMNww/DC3MOcw84AFMIIw6jDVMOwwrzDIMLcw/DDrMMkwazBZMIs-
				struct map_session_data *tsd;
				int i;
				for(i = 0; i < 5; i+-+-) {
					if(sd->dev.val1[i] && (tsd = map_id2sd(sd->dev.val1[i])) != NULL) {
						status_change_start(&tsd->bl,type,val1,0,0,0,skill_get_time(CR_REFLECTSHIELD,val1),0);
					}
				}
			}
			break;
		case SC_AUTOSPELL:			/* +MKow/DDIMLkw2jDr- */
			val4 = 5 +- val1*2;
			break;
		case SC_VOLCANO:
			calc_flag = 1;
			val3 = val1*10;
			val4 = (val1 > 5)? 20: val1*(11-val1)/2 +- 5;
			break;
		case SC_DELUGE:
			calc_flag = 1;
			val3 = (val1 > 5)? 15: val1*(11-val1)/2;
			val4 = (val1 > 5)? 20: val1*(11-val1)/2 +- 5;
			break;
		case SC_VIOLENTGALE:
			calc_flag = 1;
			val3 = val1*3;
			val4 = (val1 > 5)? 20: val1*(11-val1)/2 +- 5;
			break;
		case SC_TWOHANDQUICKEN:			/* 2HQ */
			calc_flag = 1;
			if(bl->type == BL_MOB && battle_config.monster_skill_over && val1 >= battle_config.monster_skill_over)
				val2 = 70;
			else
				val2 = 30;
			break;
		case SC_SPEARQUICKEN:		/* +MLkw1DCiMK8wpDDDMLEw8w- */
			calc_flag = 1;
#ifdef PRE_RENEWAL
			val2 = 20+-val1;
#else
			val2 = 30;
#endif
			break;
		case SC_BLADESTOP:		/* +dn1SA1PWMIo- */
			if(val2 == 2)
				clif_bladestop(map_id2bl(val3),val4,1);
			break;
		case SC_LULLABY:			/* +W1BbiFUE- */
			val2 = 11;
			break;
		case SC_DRUMBATTLE:			/* +YiZZKp8TMG6X/zBN- */
			calc_flag = 1;
#ifdef PRE_RENEWAL
			val2 = (val1+-1)*25;
			val3 = (val1+-1)*2;
#else
			val2 = 100+-val1*30;
			val3 = val1*10;
#endif
			break;
		case SC_NIBELUNGEN:			/* +MMsw/DDZMOsw8zCwMG5jB48q- */
			calc_flag = 1;
#ifdef PRE_RENEWAL
			val2 = (val1+-2)*25;
#else
			val2 = val1*40;
#endif
			break;
		case SC_SIEGFRIED:			/* +Tg1re46rMG4wuDD8MK8w1TDqMPwwyQ- */
			calc_flag = 1;
			val2 = 5 +- val1*15;
			break;
		case SC_DISSONANCE:			/* +Tg1TVFSMl/M- */
			val2 = 10;
			break;
		case SC_UGLYDANCE:			/* +gepSBlLdYkswajDAMPMwuQ- */
			val2 = 10;
			break;
		case SC_DONTFORGETME:		/* +ecEwkl/YMIwwajBEMGc- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			if(sc->data[SC_INCREASEAGI].timer != -1)
				status_change_end(bl,SC_INCREASEAGI,-1);
			if(sc->data[SC_TWOHANDQUICKEN].timer != -1)
				status_change_end(bl,SC_TWOHANDQUICKEN,-1);
			if(sc->data[SC_SPEARQUICKEN].timer != -1)
				status_change_end(bl,SC_SPEARQUICKEN,-1);
			if(sc->data[SC_ADRENALINE].timer != -1)
				status_change_end(bl,SC_ADRENALINE,-1);
			if(sc->data[SC_ADRENALINE2].timer != -1)
				status_change_end(bl,SC_ADRENALINE2,-1);
			if(sc->data[SC_ASSNCROS].timer != -1)
				status_change_end(bl,SC_ASSNCROS,-1);
			if(sc->data[SC_WINDWALK].timer != -1)
				status_change_end(bl,SC_WINDWALK,-1);
			if(sc->data[SC_CARTBOOST].timer != -1)
				status_change_end(bl,SC_CARTBOOST,-1);
			if(sc->data[SC_ONEHAND].timer != -1)
				status_change_end(bl,SC_ONEHAND,-1);
			break;
		case SC_LONGINGFREEDOM:		/* +ecEwkmLYZ18wVzBqMEQwZw- */
			calc_flag = 1;
			val3 = 1;
			tick = 1000;
			break;
		case SC_DANCING:			/* +MMAw8zC5-/+bxRZT04t- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			val3 = tick / 1000;
			tick = 1000;
			break;
		case SC_EXPLOSIONSPIRITS:	/* +cgaIwmziUtU- */
			calc_flag = 1;
			val2 = 75 +- 25*val1;
			break;
		case SC_AUTOCOUNTER:
			val3 = 0;
			val4 = 0;
			break;
		case SC_SPEEDPOTION0:		/* +WJeQHzDdMPwwtzDnMPM- */
		case SC_SPEEDPOTION1:
		case SC_SPEEDPOTION2:
			calc_flag = 1;
			val2 = 5*(2+-type-SC_SPEEDPOTION0);
			break;
		case SC_NOCHAT:		/* +MMEw4zDDMMh5gWticrZhSw- */
			{
				time_t timer;
				tick = 60000;
				if(!val2)
					val2 = (int)time(&timer);
				if(sd)
					clif_updatestatus(sd,SP_MANNER);	// +MLkwxjD8ML8wuTCSMK8w6TCkMKIw8zDIMGuQATCL-
			}
			break;
		case SC_SELFDESTRUCTION:	/* +gepyBg- */
			tick = 100;
			break;

		/* option1 */
		case SC_STONE:				/* +d/NTFg- */
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= 100 * status_get_luk(bl) - status_get_mdef(bl) * tick / 100;
#else
				tick +-= 3000 - tick * status_get_mdef(bl) / 100;
#endif
			}
			val3 = tick / 1000;
			if(val3 < 1)
				val3 = 1;
			tick = val4;
			if(tick < 1000)
				tick = 1000;
			val2 = 1;
			break;
		case SC_SLEEP:				/* +d2F3IA- */
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= -10 * status_get_luk(bl) - tick * status_get_int(bl) / 100;
#else
				tick +-= 1000 - tick * status_get_agi(bl) / 100;
#endif
			}
			break;
		case SC_FREEZE:				/* +Uc19UA- */
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= 10 * status_get_luk(bl) - tick * status_get_mdef(bl) / 100;
#else
				tick +-= 3000 - tick * status_get_mdef(bl) / 100;
#endif
			}
			break;
		case SC_STUN:				/* +MLkwvzDz/wg-val2+MGsw3zDqedIwuzDDMMj/CQ- */
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= -10 * status_get_luk(bl) - tick * status_get_vit(bl) / 100;
#else
				tick +-= 1000 - tick * status_get_vit(bl) / 100;
#endif
			}
			break;
		/* option2 */
		case SC_DPOISON:			/* +cxtr0g- */
			{
				int mhp = status_get_max_hp(bl);
				int hp  = status_get_hp(bl);
				if(!(flag&2)) {
					if(bl->type == BL_PC)	// PC+MG5YNFQIMG4wf4oIe5dfDzBMkFUwRg-
						tick = 5000 +- -10000 * status_get_luk(bl) / 100 - 45000 * status_get_vit(bl) / 100 +- 55000;
					else
						tick = 30000 - 20000 * status_get_vit(bl) / 100;
				}
				// MHP+MG4-1/4+TuVOCzBrMG8wajCJMGowRA-
				if(hp > mhp>>2) {
					int diff = 0;
					if(sd)
						diff = mhp * 10 / 100;
					else if(md)
						diff = mhp * 15 / 100;
					if(hp - diff < mhp>>2)
						diff = hp - (mhp>>2);
					unit_heal(bl, -diff, 0, 0, 0);
				}
			}
			// fall through
		case SC_POISON:				/* +a9I- */
			calc_flag = 1;
			if(!(flag&2)) {
				if(bl->type == BL_PC)	// PC+MG5YNFQIMG4wf4oIe5dfDzBMkFUwRg-
					tick = 5000 +- -10000 * status_get_luk(bl) / 100 - 45000 * status_get_vit(bl) / 100 +- 55000;
				else
					tick = 30000 - 20000 * status_get_vit(bl) / 100;
			}
			val3 = tick/1000;
			if(val3 < 1) val3 = 1;
			tick = 1000;
			break;
		case SC_SILENCE:			/* +bIie2Q- */
			skill_stop_dancing(bl,0);
			if (sc->data[SC_GOSPEL].timer != -1) {
				status_change_end(bl,SC_GOSPEL,-1);
				break;
			}
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= -10 * status_get_luk(bl) - tick * status_get_vit(bl) / 100;
#else
				tick +-= 1000 - tick * status_get_int(bl) / 100;
#endif
			}
			break;
		case SC_BLIND:				/* +Zpee0g- */
			calc_flag = 1;
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= -10 * status_get_luk(bl) - tick * (status_get_vit(bl) +- status_get_int(bl)) / 200;
#else
				tick +-= 1000 - tick * status_get_int(bl) / 100;
#endif
			}
			break;
		case SC_CURSE:				/* +VGowRA- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= -10 * status_get_luk(bl) - tick * status_get_vit(bl) / 100;
#else
				tick +-= 1000 - tick * status_get_luk(bl) / 100;
#endif
			}
			break;
		case SC_CONFUSION:			/* +bfdOcQ- */
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				tick +-= -10 * status_get_luk(bl) - tick * (status_get_str(bl) +- status_get_int(bl)) / 200;
#else
				tick +-= 1000 - tick * status_get_luk(bl) / 100;
#endif
			}
			break;
		case SC_BLEED:				/* +UfqIQA- */
			if(!(flag&2)) {
#ifdef PRE_RENEWAL
				icon_tick = tick = -10 * status_get_luk(bl) - tick * status_get_vit(bl) / 100 +- tick;
#else
				icon_tick = tick = 1000 - tick * status_get_agi(bl) / 100 +- tick;
#endif
			}
			val3 = (tick < 10000)? 1: tick / 10000;
			tick = 10000;	// +MMAw4TD8MLh2enUfMG8-10sec+a84-
			break;

		/* option */
		case SC_HIDING:		/* +MM8wpDDHMKMw8zCw- */
			calc_flag = 1;
			if(sd) {
				val2 = tick / 1000;		/* +YwF9mmZClZM- */
				tick = 1000;
			}
			break;
		case SC_CHASEWALK:		/* +MMEwpzCkMLkwpjCpMPwwrw- */
		case SC_CLOAKING:		/* +MK8w7TD8MK0w8zCw- */
			if(sd) {
				calc_flag = 1;
				tick = val2;
			} else {
				tick = 5000*val1;
			}
			break;
		case SC_INVISIBLE:		/* +MKQw8zDTMLgw1jDr- */
			break;
		case SC_SIGHTBLASTER:		/* +MLUwpDDIMNYw6TC5ML8w/A- */
		case SC_SIGHT:			/* +MLUwpDDI- */
		case SC_RUWACH:			/* +MOswojDV- */
			val2 = tick/250;
			tick = 10;
			break;

		case SC_MODECHANGE:
			tick = 1200;
			break;
		case SC_AUTOGUARD:
			val2 = (val1 > 10)? 30: (22 - val1) * val1 / 4;
			if(sd) {
				// +iKswxzCjMNww/DC3MOcw84AFMIIwqjD8MMgwrDD8MMkwazBZMIs-
				struct map_session_data *tsd;
				int i;
				for(i = 0; i < 5; i+-+-) {
					if(sd->dev.val1[i] && (tsd = map_id2sd(sd->dev.val1[i])) != NULL)
						status_change_start(&tsd->bl,type,val1,0,0,0,skill_get_time(CR_AUTOGUARD,val1),0);
				}
			}
			break;
		case SC_DEFENDER:
			calc_flag = 1;
			ud->state.change_speed = 1;
			val2 = 5 +- val1*15;
			val3 = 250 - val1*50;
			if(sd) {
				// +iKswxzCjMNww/DC3MOcw84AFMJIwxzCjMNUwpzDzMMAw/DBrMFkwiw-
				struct map_session_data *tsd;
				int i;
				for(i = 0; i < 5; i+-+-) {
					if(sd->dev.val1[i] && (tsd = map_id2sd(sd->dev.val1[i])) != NULL)
						status_change_start(&tsd->bl,type,val1,0,0,0,skill_get_time(CR_DEFENDER,val1),0);
				}
			}
			break;
		case SC_HALLUCINATION:
			if(sd && !battle_config.hallucianation_off) {
				// on+MGowbjBnMKIwpDCzMPOIaHk6-
				clif_status_change(bl, SI_HALLUCINATION, 1, icon_tick, 0, 0, 0);
			}
			break;
		case SC_TENSIONRELAX:	/* +MMYw8zC3MOcw8zDqMOkwwzCvMLk- */
			if(sd == NULL)
				return 0;
			tick = 10000;
			break;
		case SC_PARRYING:		/* +MNEw6jCkMPMwsA- */
			val2 = 20 +- val1*3;
			break;
		case SC_JOINTBEAT:		/* +MLgw5zCkMPMwyDDTMPwwyA- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			val4 = atn_rand()%6;
			if(val4 == 5) {
				// +mZYwb183UjZ2hDBrUfqIQE7YUqA- +MPs- +T391KIAFMG4w7DDZMOswTFPWX5cwZzBNMGowRDBuMGcwaDCKMEIwSDBa-0+MJJfFWVwMGs-
				if(atn_rand() % 10000 < status_change_rate(bl,SC_BLEED,10000,0))
					status_change_start(bl,SC_BLEED,val1,0,0,0,skill_get_time2(LK_JOINTBEAT,val1),10);
			}
			if(!(flag&2)) {
				tick = tick - (status_get_agi(bl)/10 +- status_get_luk(bl)/4)*1000;
			}
			break;
		case SC_WINDWALK:		/* +MKYwpDDzMMkwpjCpMPwwrw- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			val2 = val1 / 2;	// Flee+TgpmB3OH-
			break;
		case SC_BERSERK:		/* +MNAw/DC1MPwwrw- */
			unit_heal(bl,0,-status_get_sp(bl),0,0);
			if(sd) {
				clif_status_change(bl,SI_INCREASEAGI,1,icon_tick, 0, 0, 0);	// +MKIwpDCzMPOIaHk6-
			}
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_ASSUMPTIO:		/* +MKIwuTDgMNcwxjCjMKo- */
		case SC_ASSUMPTIO2:		/* +MK0w4zDDMLcw5TCiMLkw4DDXMMYwozCq- */
			// +MK0w6jCoMExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_KYRIE].timer != -1)
				status_change_end(bl,SC_KYRIE,-1);
			// +MKswpDDIMExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_KAITE].timer != -1)
				status_change_end(bl,SC_KAITE,-1);
			break;
		case SC_CARTBOOST:		/* +MKsw/DDIMNYw/DC5MMg- */
			calc_flag = 1;
			if(sc->data[SC_DECREASEAGI].timer != -1)
				status_change_end(bl,SC_DECREASEAGI,-1);
			break;
		case SC_REJECTSWORD:	/* +MOowuDCnMK8wyDC9MPwwyQ- */
			val2 = 3; // 3+Vt5lO2SDMJKN8zBtj9QwWQ-
			break;
		case SC_MEMORIZE:		/* +MOEw4jDpMKQwug- */
			val2 = 5; // 5+Vt6KYFUxMJI-1/2+MGswWTCL-
			break;
		case SC_GRAFFITI:		/* +MLAw6TDVMKMwxjCj- */
			{
				struct skill_unit_group *sg = skill_unitsetting(bl,RG_GRAFFITI,val1,val2,val3,0);
				if(sg)
					val4 = sg->bl.id;
			}
			break;
		case SC_OVERTHRUSTMAX:		/* +MKow/DDQMPwwyDDpMLkwyDDeMMMwrzC5- */
			if(sc->data[SC_OVERTHRUST].timer != -1)
				status_change_end(bl,SC_OVERTHRUST,-1);
			calc_flag = 1;
			break;

		case SC_MEAL_INCSTR:	// +mN9Oi3Uo-
		case SC_MEAL_INCAGI:
		case SC_MEAL_INCVIT:
		case SC_MEAL_INCINT:
		case SC_MEAL_INCDEX:
		case SC_MEAL_INCLUK:
			if(sc->data[type - SC_MEAL_INCSTR +- SC_MEAL_INCSTR2].timer != -1) {
				// +UrlnnDBMT04wRFg0VAgwb1K5Z5wwajBX-
				if(val1 < sc->data[type - SC_MEAL_INCSTR +- SC_MEAL_INCSTR2].val1)
					return 0;
				// +VAx7STBLmtgwRFK5Z5wwajCJT391KE4tMG5SuWecMJJtiDBZ-
				else
					status_change_end(bl, type - SC_MEAL_INCSTR +- SC_MEAL_INCSTR2, -1);
			}
			calc_flag = 1;
			break;
		case SC_MEAL_INCSTR2:	// +irKR0WWZdAZ1KA-
		case SC_MEAL_INCAGI2:
		case SC_MEAL_INCVIT2:
		case SC_MEAL_INCINT2:
		case SC_MEAL_INCDEX2:
		case SC_MEAL_INCLUK2:
			if(sc->data[type - SC_MEAL_INCSTR2 +- SC_MEAL_INCSTR].timer != -1) {
				// +UrlnnDBMT04wRFg0VAgwb1K5Z5wwajBX-
				if(val1 < sc->data[type - SC_MEAL_INCSTR2 +- SC_MEAL_INCSTR].val1)
					return 0;
				// +VAx7STBLmtgwRFK5Z5wwajCJT391KE4tMG5SuWecMJJtiDBZ-
				else
					status_change_end(bl, type - SC_MEAL_INCSTR2 +- SC_MEAL_INCSTR, -1);
			}
			calc_flag = 1;
			break;
		case SC_ELEMENTFIELD:		/* +XF5gJ1g0- */
			tick = val2;
			break;
		case SC_RUN:			/* +ML8wpDDqMK4- */
			val4 = 0;
			calc_flag = 1;
			break;
		case SC_KAUPE:			/* +MKswpjDX- */
			val2 = val1*33;
			if(val1 >= 3)
				val2 = 100;
			break;
		case SC_KAITE:			/* +MKswpDDI- */
			// +MKIwuTDgMExjmzBLMGMwZjBEMF8wiYnjlmQwVzBm-
			if(sc->data[SC_ASSUMPTIO].timer != -1)
				status_change_end(bl,SC_ASSUMPTIO,-1);
			if(sc->data[SC_ASSUMPTIO2].timer != -1)
				status_change_end(bl,SC_ASSUMPTIO2,-1);
			// +U81cBFbeZXA-
			val2 = val1 / 5 +- 1;
			break;
		case SC_SWOO:			/* +MKgwuTCm- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			if(status_get_mode(bl)&MD_BOSS)
				tick /= 5;
			break;
		case SC_MONK:			/* +MOIw8zCvMG6bQg- */
		case SC_STAR:			/* +MLEw8zC7MKQwbptC- */
		case SC_SAGE:			/* +MLsw/DC4MG6bQg- */
		case SC_CRUSADER:		/* +MK8w6zC7MKQwwDD8MG6bQg- */
		case SC_WIZARD:			/* +MKYwozC2MPwwyTBum0I- */
		case SC_PRIEST:			/* +MNcw6jD8MLkwyDBum0I- */
		case SC_ROGUE:			/* +MO0w/DCwMG6bQg- */
		case SC_ASSASIN:		/* +MKIwtTC3MPMwbptC- */
		case SC_SOULLINKER:		/* +ML0wpjDrMOow8zCrMPwwbptC- */
		case SC_GUNNER:			/* +MKww8zC5MOow8zCsMPwwbptC- */
		case SC_NINJA:			/* +X82ABTBum0I- */
		case SC_DEATHKINGHT:		/* +MMcwuTDKMKQwyDBum0I- */
		case SC_COLLECTOR:		/* +MLMw7DCvML8w/DBum0I- */
			if(sd && battle_config.disp_job_soul_state_change)
				clif_disp_onlyself(sd->fd, msg_txt(206));	// +m0JytmFLMGswajCKMH4wVzBf-
			break;
		case SC_KNIGHT:			/* +MMowpDDIMG6bQg- */
		case SC_ALCHEMIST:		/* +MKIw6zCxMN8wuTDIMG6bQg- */
		case SC_BARDDANCER:		/* +MNAw/DDJMGgwwDDzMLUw/DBum0I- */
		case SC_BLACKSMITH:		/* +MNYw6TDDMK8wuTDfMLkwbptC- */
		case SC_HUNTER:			/* +MM8w8zC/MPwwbptC- */
		case SC_HIGH:			/* +TgBrIU4KT02Ad2ltMG6bQg- */
			if(sd && battle_config.disp_job_soul_state_change)
				clif_disp_onlyself(sd->fd, msg_txt(206));	// +m0JytmFLMGswajCKMH4wVzBf-
			calc_flag = 1;
			break;
		case SC_SUPERNOVICE:		/* +MLkw/DDRMPwwzjD8MNMwuTBum0I- */
			if(sd) {
				// 1%+MGdre06hMNUw6TCwbYgwWQ-
				if(atn_rand()%10000 < battle_config.repeal_die_counter_rate) {
					sd->status.die_counter = 0;	// +a3swazCrMKYw8zC/MPww6jC7MMMwyA-
					if(sd->status.job_level >= 70 || sd->s_class.job == PC_JOB_ESNV)
						clif_misceffect(&sd->bl,7);	// +MLkw0TDOMNNZKU9/-
				}
				if(battle_config.disp_job_soul_state_change)
					clif_disp_onlyself(sd->fd, msg_txt(206));	// +m0JytmFLMGswajCKMH4wVzBf-
			}
			calc_flag = 1;
			break;
		case SC_AUTOBERSERK:
			if( status_get_hp(bl) < status_get_max_hp(bl) >> 2 &&
			    (sc->data[SC_PROVOKE].timer == -1 || sc->data[SC_PROVOKE].val2 == 0) )
			{
				// +MKow/DDIMNAw/DC1MPwwr3Z6UtU-
				status_change_start(&sd->bl,SC_PROVOKE,10,1,0,0,0,0);
			}
			tick = 600*1000;
			break;
		case SC_READYSTORM:
		case SC_READYDOWN:
		case SC_READYTURN:
		case SC_READYCOUNTER:
			tick = 600*1000;
			break;
		case SC_UTSUSEMI:		/* +enqHSQ- */
			val3 = (val1+-1)/2;
			break;
		case SC_BUNSINJYUTSU:		/* +UgaOqzBuiFM- */
			val3 = (val1+-1)/2;
			if(sd) {
				val4 = sd->status.clothes_color;
				pc_changelook(sd, LOOK_CLOTHES_COLOR, 0);
			}
			break;
		case SC_BOSSMAPINFO:		/* +UfiXYpPh- */
			if(map[bl->m].mvpboss == NULL) {
				if(sd) {
					// +XEUwajBEMG4wZzDhMMMwuzD8MLgwklH6MFcwZn1CToY-
					clif_bossmapinfo(sd, "", 0, 0, 0, 0);
				}
				tick = 0;
			}
			break;
		case SC_STONEHARDSKIN:		/* +MLkwyDD8MPMwzzD8MMkwuTCtMPM- */
			{
				val3 = (int)((atn_bignumber)status_get_hp(bl) * 20 / 100);
				unit_heal(bl, -val3, 0, 0, 0);
			}
			break;
		case SC_MILLENNIUMSHIELD:	/* +MN8w7DDLMKIw4DC3MPww6zDJ- */
			val2 = 2 +- atn_rand()%3;
			val3 = 1000;
			if(sd)
				clif_mshield(sd, val2);
			break;
		case SC_FIGHTINGSPIRIT:		/* +MNUwoTCkMMYwozDzMLAwuTDUMOowwzDI- */
			calc_flag = 1;
			val3 = (val3>9)? 4: (val3>7)? 3: (val3>4)? 2: (val3>2)? 1: 0;
			break;
		case SC_ABUNDANCE:			/* +MKIw0DDzMMAw8zC5- */
			val3 = tick / 10000;
			if(val3 < 1)
				val3 = 1;
			tick = 10000;
			break;
		case SC_DEATHBOUND:			/* +MMcwuTDQMKYw8zDJ- */
			val2 = 500+-val1*100;
			break;
		case SC_FEAR:				/* +YFBgFg- */
			calc_flag = 1;
			clif_emotion(bl,79);
			val3 = (tick < 2000) ? 2000: tick - 2000;
			tick = 2000;
			break;
		case SC_VENOMIMPRESS:		/* +MNkwyjDgMKQw8zDXMOwwuQ- */
			val2 = val1 * 10;
			break;
		case SC_POISONINGWEAPON:	/* +MN0wpDC6MMsw8zCwMKYwpzDdMPM- */
			val3 = 200 +- val1 * 200;
			break;
		case SC_WEAPONBLOCKING:		/* +MKYwpzDdMPMw1jDtMMMwrTDzMLA- */
			val2 = 10 +- val1 * 2;
			val3 = tick / 5000;
			tick = 5000;
			break;
		case SC_CLOAKINGEXCEED:		/* +MK8w7TD8MK0w8zCwMKgwrzC3MPwwyQ- */
			calc_flag = 1;
			val2 = (val1 +- 1) / 2;	// +MMAw4TD8MLiAEGAn-
			tick = 1000;
			break;
		case SC_ROLLINGCUTTER:		/* +MO0w/DDqMPMwsDCrMMMwvzD8- */
			icon_val1 = val1;	// val1(+Vt6O4mVw-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			break;
		case SC_TOXIN:				/* +MMgwrTC3MPM- */
			val2 = tick / 10000;
			tick = 10000;
			break;
		case SC_VENOMBLEED:			/* +MNkwyjDgMNYw6jD8MMk- */
			calc_flag = 1;
			val2 = 15;
			break;
		case SC_MAGICMUSHROOM:		/* +MN4wuDDDMK8w3jDDMLcw5TDrMPww4A- */
		case SC_OBLIVIONCURSE:		/* +MKow1jDqMNMwqjDzMKsw/DC5- */
			val2 = tick / 2000;
			tick = 2000;
			break;
		case SC_DEATHHURT:			/* +MMcwuTDPMPwwyA- */
			val2 = 20;
			break;
		case SC_PYREXIA:			/* +MNEwpDDsMMMwrzC3MKI- */
			val2 = tick / 3000;
			tick = 3000;
			break;
		case SC_LEECHEND:			/* +MOow/DDBMKgw8zDJ- */
			val2 = tick / 1000;
			tick = 1000;
			break;
		case SC_DARKCROW:			/* +MMAw/DCvMK8w7TD8- */
			val2 = val1 * 30;
			break;
		case SC_EPICLESIS:			/* +MKgw1DCvMOwwtzC5- */
			calc_flag = 1;
			val2 = val1 * 5;
			break;
		case SC_ORATIO:				/* +MKow6TDGMKMwqg- */
			val2 = val1 * 5;
			break;
		case SC_LAUDAAGNUS:			/* +MOkwpjDAMKIwsDDMMLk- */
		case SC_LAUDARAMUS:			/* +MOkwpjDAMOkw4DC5- */
			calc_flag = 1;
			val2 = val1 +- 4;
			break;
		case SC_RENOVATIO:			/* +MOwwzjD0MKEwxjCjMKo- */
			val2 = val1 * 2 +- 1;
			val3 = tick / 5000;
			tick = 5000;
			break;
		case SC_EXPIATIO:			/* +MKgwrzC5MNQwojDGMKMwqg- */
			val2 = val1 * 20;
			break;
		case SC_DUPLELIGHT:			/* +MMcw5TDXMOww6TCkMMg- */
			val2 = 10 +- val1 * 2;	// +cml0BmU7ZIN2enUfeLpzhw-
			val3 = 10 +- val1 * 2;	// +m1Rs1WU7ZIN2enUfeLpzhw-
			break;
		case SC_SACRAMENT:			/* +MLUwrzDpMOEw8zDI- */
			val2 = val1 * 10;
			break;
		case SC_MARSHOFABYSS:		/* +MN4w/DC3MOUwqjDWMKIw0zC5- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			val2 = val1 * 10;	// +eftS1ZAfXqZuG1wRc4c-
			val3 = val1 * 6;	// AGI/DEX+bhtcEXOH-
			break;
		case SC_SUMMONBALL1:		/* +MLUw4jDzMNww/DDr-(1+UAt27g-) */
		case SC_SUMMONBALL2:		/* +MLUw4jDzMNww/DDr-(2+UAt27g-) */
		case SC_SUMMONBALL3:		/* +MLUw4jDzMNww/DDr-(3+UAt27g-) */
		case SC_SUMMONBALL4:		/* +MLUw4jDzMNww/DDr-(4+UAt27g-) */
		case SC_SUMMONBALL5:		/* +MLUw4jDzMNww/DDr-(5+UAt27g-) */
			icon_val1 = val2;	// val2(+XF5gJzBuei6YXg-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			val3 = tick / 1000;
			tick = 1000;
			break;
		case SC_HELLINFERNO:		/* +MNgw6zCkMPMw1TCnMOswzg- */
			calc_flag = 1;
			val2 = tick / 1000;
			val3 = 0;
			tick = 1000;
			break;
		case SC_TELEKINESIS_INTENSE:	/* +MMYw7DCtMM0wtzC5MKQw8zDGMPMwuQ- */
			val2 = val1 * 40;	// +WJdSoHOH-
			val3 = val1 * 10;	// +bYiMuw-SP+jv1uG3OH-
			break;
		case SC_ELECTRICSHOCKER:	/* +MKgw7DCvMMgw6jDDMK8wtzDnMMMwqzD8- */
			val2 = tick / 1000;
			tick = 1000;
			break;
		case SC_CAMOUFLAGE:			/* +MKsw4jDVMOkw/DC4MOU- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			val2 = 0;			// 1+edIwVDBoMG5fN1MWUCQ-
			tick = 1000;
			break;
		case SC_UNLIMIT:			/* +MKIw8zDqMN8wwzDI- */
			calc_flag = 1;
			val2 = val1 * 50;	// +WJdSoHOH-
			break;
		case SC_OVERHEAT_LIMITPOINT:	/* +MKow/DDQMPww0jD8MMhSoHGxUCQ- */
		case SC_OVERHEAT:				/* +MKow/DDQMPww0jD8MMg- */
			icon_val1 = val1;
			break;
		case SC_SHAPESHIFT:			/* +MLcwpzCkMNcwtzDVMMg- */
			switch(val1) {
				case 1:  val2 = ELE_FIRE;  break;
				case 2:  val2 = ELE_EARTH; break;
				case 3:  val2 = ELE_WIND;  break;
				default: val2 = ELE_WATER; break;
			}
			break;
		case SC_MAGNETICFIELD:		/* +MN4wsDDNMMYwozDDMK8w1TCjMPww6zDJ- */
			val2 = tick / 1000;
			tick = 1000;
			break;
		case SC_STEALTHFIELD_MASTER:	/* +MLkwxjDrMLkw1TCjMPww6zDJ-(+T391KIAF-) */
			val3 = 1000 * (val1 +- 2);
			val2 = tick / val3;
			tick = val3;
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC__REPRODUCE:		/* +MOow1zDtMMcw5TD8MLk- */
			tick = 1000;
			break;
		case SC__AUTOSHADOWSPELL:	/* +MLcw4zDJMKYwqjD8MMgwuTDaMOs- */
			val4 = 30 - ((val1%2)? 2: 0) - ((val1+-1)/2) * 2;		// +dnpS1Xi6c4c-
			val4 = (val4 < 1)? 1: val4;
			break;
		case SC__SHADOWFORM:		/* +MLcw4zDJMKYw1TCpMPww4A- */
			val3 = 4 +- val1;	// +a4swilbeZXA-
			val4 = tick / 1000;
			tick = 1000;
			break;
		case SC__INVISIBILITY:		/* +MKQw8zDTMLgw0zDqMMYwow- */
			if(sd) {
				tick = 1000;
				calc_flag = 1;
			} else {
				tick = 5000*val1;
			}
			break;
		case SC_REFLECTDAMAGE:		/* +MOow1TDsMK8wyDDAMOEw/DC4- */
			// +MOow1TDsMK8wyDC3MPww6zDJMExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_REFLECTSHIELD].timer != -1)
				status_change_end(bl,SC_REFLECTSHIELD,-1);
			val2 = tick / 10000;
			val3 = val1 * 5 +- 15;	// +U81cBHOH-
			val4 = val1 * 5 +- 25;	// +U81cBFbeZXA-
			tick = 10000;
			break;
		case SC_FORCEOFVANGUARD:	/* +MNUwqTD8MLkwqjDWMNAw8zCsMPwwyQ- */
			val2 = tick / 10000;
			val3 = val1 * 12 +- 8;	// +YBIwijCrMKYw8zC/MPx2elLVc4c-
			tick = 10000;
			icon_tick = -1;
			calc_flag = 1;
			break;
		case SC_SHIELDSPELL_DEF:	/* +MLcw/DDrMMkwuTDaMOs-(DEF) */
		case SC_SHIELDSPELL_MDEF:	/* +MLcw/DDrMMkwuTDaMOs-(MDEF) */
		case SC_SHIELDSPELL_REF:	/* +MLcw/DDrMMkwuTDaMOs-(+fL6TLA-) */
			if(sc->data[SC_SHIELDSPELL_DEF].timer != -1)
				status_change_end(bl,SC_SHIELDSPELL_DEF,-1);
			if(sc->data[SC_SHIELDSPELL_MDEF].timer != -1)
				status_change_end(bl,SC_SHIELDSPELL_MDEF,-1);
			if(sc->data[SC_SHIELDSPELL_REF].timer != -1)
				status_change_end(bl,SC_SHIELDSPELL_REF,-1);
			calc_flag = 1;
			break;
		case SC_EXEEDBREAK:			/* +MKQwrzC3MPwwyTDWMOwwpDCv- */
			if(sd) {
				int idx = sd->equip_index[EQUIP_INDEX_RARM];
				val2 = 100 +- val1 * 150 +- sd->status.job_level * 15;
				if(idx >= 0 && sd->inventory_data[idx])
					val2 +-= sd->inventory_data[idx]->weight/10 * sd->inventory_data[idx]->wlv * sd->status.base_level / 100;
			}
			else {
				val2 = val1 * 150;
			}
			tick = 600*1000;
			break;
		case SC_PRESTIGE:	/* +MNcw7DC5MMYwozD8MLg- */
			val2 = val1 * 15 * status_get_lv(bl) / 100;		// Def+WJdSoFAk-
			if(sd) {
				val2 +-= pc_checkskill(sd,CR_DEFENDER) / 5 * status_get_lv(bl) / 2;
			}
			val3 = (1 +- status_get_agi(bl) / 20 +- status_get_luk(bl) / 40) * val1 / 2;		// +m1Rs1VbekH9zhw-
			calc_flag = 1;
			break;
		case SC_BANDING:	/* +MNAw8zDHMKMw8zCw- */
			tick = 5000;
			icon_tick = 9999;
			calc_flag = 1;
			break;
		case SC_SITDOWN_FORCE:		/* +juJQEg- */
			if(sd){
				pc_setsit(sd);
				clif_sitting(&sd->bl, 1);
			}
			break;
		case SC_INSPIRATION:	/* +MKQw8zC5MNQw7DD8MLcw5zDz- */
			val2 = tick / 6000;
			tick = 6000;
			calc_flag = 1;
			break;
		case SC_KINGS_GRACE:	/* +MK0w8zCwMLkwsDDsMKQwuQ- */
			status_change_end(bl,SC_POISON,-1);
			status_change_end(bl,SC_BLIND,-1);
			status_change_end(bl,SC_FREEZE,-1);
			status_change_end(bl,SC_STONE,-1);
			status_change_end(bl,SC_STUN,-1);
			status_change_end(bl,SC_SLEEP,-1);
			status_change_end(bl,SC_BLEED,-1);
			status_change_end(bl,SC_CURSE,-1);
			status_change_end(bl,SC_CONFUSION,-1);
			status_change_end(bl,SC_SILENCE,-1);
			status_change_end(bl,SC_DPOISON,-1);
			status_change_end(bl,SC_HALLUCINATION,-1);
			status_change_end(bl,SC_FEAR,-1);
			status_change_end(bl,SC_HELLINFERNO,-1);
			status_change_end(bl,SC_FROSTMISTY,-1);
			status_change_end(bl,SC_DEEP_SLEEP,-1);
			status_change_end(bl,SC_DIAMONDDUST,-1);
			status_change_end(bl,SC_MANDRAGORA,-1);
			status_change_end(bl,SC_DEVOTION,-1);
			unit_stopattack(bl);
			unit_stop_walking(bl,0);
			val2 = tick / 1000;
			val4 = 3 +- val1;
			tick = 1000;
			break;
		case SC_CRESCENTELBOW:		/* +eDR4jmfx- */
			val2 = 40 +- val1 * 5 +- status_get_jlv(bl) / 2;		// +dnpS1XOH-
			break;
		case SC_LIGHTNINGWALK:		/* +lYOW+2tp- */
			val2 = 40 +- val1 * 5 +- status_get_jlv(bl) / 2;		// +dnpS1XOH-
			break;
		case SC_RAISINGDRAGON:	/* +b1yfjWYHWSk- */
			val2 = tick / 5000;
			val3 = val1 +- 2;	// MaxHP,MaxSP+WJdSoHOH-
			tick = 5000;
			calc_flag = 1;
			break;
		case SC_GENTLETOUCH_ENERGYGAIN:	/* +cLl6dA- -+dAM-- */
			// +cLl6dA- -+U80--+MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_GENTLETOUCH_CHANGE].timer != -1)
				status_change_end(bl,SC_GENTLETOUCH_CHANGE,-1);
			// +cLl6dA- -+bTs--+MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_GENTLETOUCH_REVITALIZE].timer != -1)
				status_change_end(bl,SC_GENTLETOUCH_REVITALIZE,-1);
			val2 = val1 * 5 +- 10;	// +bBdfPnUfYhBzhw-
			break;
		case SC_GENTLETOUCH_CHANGE:	/* +cLl6dA- -+U80-- */
			// +cLl6dA- -+dAM--+MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_GENTLETOUCH_ENERGYGAIN].timer != -1)
				status_change_end(bl,SC_GENTLETOUCH_ENERGYGAIN,-1);
			// +cLl6dA- -+bTs--+MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_GENTLETOUCH_REVITALIZE].timer != -1)
				status_change_end(bl,SC_GENTLETOUCH_REVITALIZE,-1);
			val4 = status_get_agi(bl) / 15;		// ASPD+TgpmB1Ak-
			calc_flag = 1;
			break;
		case SC_GENTLETOUCH_REVITALIZE:	/* +cLl6dA- -+bTs-- */
			// +cLl6dA- -+dAM--+MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_GENTLETOUCH_ENERGYGAIN].timer != -1)
				status_change_end(bl,SC_GENTLETOUCH_ENERGYGAIN,-1);
			// +cLl6dA- -+U80--+MExjmzBLMGMwZjBEMF8wiYnjlmQ-
			if(sc->data[SC_GENTLETOUCH_CHANGE].timer != -1)
				status_change_end(bl,SC_GENTLETOUCH_CHANGE,-1);
			calc_flag = 1;
			break;
		case SC_SWING:				/* +MLkwpDDzMLAwwDDzMLk- */
			val4 = (val1 * 5) +- (val2 * 2 / 10);
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_RUSH_WINDMILL:		/* +mKiOyjBrVBEwSzBjMGZ6gWSD- */
			val4 = (val1 * 6) +- (val2 / 5) +- val3;
			calc_flag = 1;
			break;
		case SC_MOONLIT_SERENADE:	/* +ZwhmDjBLMIowbjC7MOwwyjD8MMc- */
			val4 = (val1 * 5) +- (val2 / 4) +- val3;
			calc_flag = 1;
			break;
		case SC_ECHOSONG:			/* +MKgwszD8MG5rTA- */
			calc_flag = 1;
			val4 = (val1 * 6) +- (val2 / 4) +- val3;
			break;
		case SC_SYMPHONY_LOVE:		/* +YEtOujBfMGEwbnC6MG4wtzDzMNUwqTDLMPw- */
			calc_flag = 1;
			val4 = (val1 * 12) +- (val2 / 4) +- val3;
			break;
		case SC_WINKCHARM:	/* +m0Vg0TBuMKYwozDzMK8- */
		case SC_SIREN:		/* +MLswpDDsMPww8zBuWPA- */
			val3 = tick / 3000;
			tick = 3000;
			break;
		case SC_DEEP_SLEEP:		/* +W4kwiTBOMG5bUFuIVQQ- */
			val2 = tick / 2000;
			tick = 2000;
			break;
		case SC_SIRCLEOFNATURE:		/* +X6p0sDBZMIuB6nE2MG6X8w- */
			val2 = tick / 1000;
			tick = 1000;
			break;
		case SC_BEYOND_OF_WARCRY:	/* +MNMw6DDzMMkwqjDWMKYwqTD8MK8w6TCk- */
			val3 = val1 * 4;
			val4 = val1;
			if(val2 >= 3 && val2 <= 7) {
				val3 +-= (val2-2) * 4;
				val4 +-= val2-2;
			}
			calc_flag = 1;
			break;
		case SC_MELODYOFSINK:		/* +MOEw7TDHMKMw/DCqMNYwtzDzMK8- */
			val3 = val1 * 2;
			val4 = val1;
			if(val2 >= 3 && val2 <= 7) {
				val3 +-= (val2-2) * 2;
				val4 +-= val2-2;
			}
			val2 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_SONG_OF_MANA:		/* +MN4wyjBua0w- */
			val2 = tick / 5000;
			tick = 5000;
			break;
		case SC_SATURDAY_NIGHT_FEVER:	/* +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw- */
			val3 = 12000 - val1 * 2000;
			val2 = tick / val3;
			tick = val3;
			calc_flag = 1;
			break;
		case SC_FRIGG_SONG:			/* +MNUw6jDDMLAwbmtM- */
			val2 = tick / 1000;
			val3 = val1 * 5;
			val4 = val1 * 20 +- 80;
			calc_flag = 1;
			break;
		case SC_PROPERTYWALK:		/* +MNUwoTCkMKIw/A-/+MKgw7DCvMMgw6jDDMK8wpjCpMPwwrw- */
			val3 = val1 * 2 +- 6;	// +dnp1H1ALZXA-
			break;
		case SC_DIAMONDDUST:		/* +MMAwpDDkMOIw8zDJMMAwuTDI- */
			val2 = tick / 1000;
			tick = 1000;
			break;
		case SC_SPELLFIST:		/* +MLkw2jDrMNUwozC5MMg- */
			val4 = val1 * 7;	// +Vt5lcA-
			break;
		case SC_STRIKING:			/* +MLkwyDDpMKQwrTDzMLA- */
			val2 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_BLOOD_SUCKER:		/* +MNYw6TDDMMkwtTDDMKsw/A- */
			val4 = tick / 1000;
			tick = 1000;	// +MMAw4TD8MLh2enUflZOWlA-
			break;
		case SC_FIRE_EXPANSION_SMOKE_POWDER:	/* +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+cVleVQ-) */
			val2 = 25;		// +j9GN3ZbiMPuQYI3dluIwwDDhMPwwuG4bXBFzhw-
			val3 = 25;		// Flee+TgpmB3OH-
			calc_flag = 1;
			break;
		case SC_FIRE_EXPANSION_TEAR_GAS:	/* +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmTCsMLk-) */
			val2 = 25;		// Hit,Flee+bhtcEXOH-
			val3 = tick / 3000;
			tick = 3000;
			calc_flag = 1;
			break;
		case SC_MANDRAGORA:			/* +MM8wpjDqMPMwsDCqMNYw3jDzMMkw6TC0MOk- */
			val2 = val1 * 5 +- 5;		// Int+bhtcEVAk-
			val3 = 3000;		// +VvpbmopgVTFYl1KgUCQ-
			calc_flag = 1;
			break;
		case SC_BANANA_BOMB:		/* +MNAwyjDKcgZfPg- */
			if(sd){
				pc_setsit(sd);
				clif_sitting(&sd->bl, 1);
			}
			calc_flag = 1;
			break;
		case SC_MEIKYOUSISUI:		/* +Zg6T4WtibDQ- */
			val2 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_IZAYOI:			/* +U0FRbVkc- */
			if(sd) {
				val2 = sd->status.job_level/2 * val1;
			} else {
				val2 = 50 * val1;
			}
			val3 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_KAGEMUSYA:		/* +XnuIUw- -+UgaOqw-- */
			val2 = 20;		// +MMAw4TD8MLhYl1Kgc4c-
			val3 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_ZANGETSU:		/* +XnuIUw- -+a4tnCA-- */
			if(status_get_hp(bl)%2 == 0)
				val2 +-= status_get_lv(bl) / 3 +- val1 * 20; 	// HP+MExQdmVwMG5YNFQI-
			else
				val2 -= status_get_lv(bl) / 3 +- val1 * 30; 	// HP+MExZR2VwMG5YNFQI-
			if(status_get_sp(bl)%2 == 0)
				val3 +-= status_get_lv(bl) / 3 +- val1 * 20; 	// SP+MExQdmVwMG5YNFQI-
			else
				val3 -= status_get_lv(bl) / 3 +- val1 * 30; 	// SP+MExZR2VwMG5YNFQI-
			calc_flag = 1;
			break;
		case SC_GENSOU:		/* +XnuIUw- -+Zydee2Dz-- */
			val2 = val1 * 10; 	// +MMAw4TD8MLhTzVwEc4c-
			break;
		case SC_C_MARKER:		/* +MK8w6jDgML4w8zDeMPwwqzD8- */
			if(val3 < 0 || val3 >= 3)
				return 0;
			val4 = tick/1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_B_TRAP:			/* +MNAwpDDzMMkwyDDpMMMw1w- */
			val3 = val1 * 25;
			ud->state.change_speed = 1;
			break;
		case SC_HEAT_BARREL:		/* +MNIw/DDIMNAw7DDr- */
			val2 = val4 * 5;		// fixcast
			val3 = (6 +- val1 * 2) * val4; // atk
			val4 = 25 +- val1 * 5; // hit
			calc_flag = 1;
			break;
		case SC_P_ALTER:		/* +MNcw6TDBMMow4DCiMOswvzD8- */
			if(sd) {
				int idx = sd->equip_index[EQUIP_INDEX_ARROW];
				if(idx >= 0 && sd->inventory_data[idx] &&
					(sd->inventory_data[idx]->nameid == 13220 ||		// +MLUw8zCvML8w1TChMKQwyTDQMOwwwzDI-
					sd->inventory_data[idx]->nameid == 13221)		// +MLcw6zDQMPww0DDsMMMwyA-C
				)
					val2 = 10 +- 10 * val4; // atk
			}
			else
				val2 = 10 +- 10 * val4; // atk
			val3 = (int)((atn_bignumber)status_get_max_hp(bl) * (val1 * 5) / 100);	// +gBBORV6m-
			val4 = val1 +- 3;	// +Vt5lcA-
			break;
		case SC_ANTI_M_BLAST:
			val2 = 15 +- val1 * 2;
			break;
		case SC_SUHIDE:				/* +MEswTzCMMIs- */
			tick = 60 * 1000;
			break;
		case SC_ARCLOUSEDASH:		/* +MKIwrzDpMKYwuTDAMMMwtzDl- */
			val2 = 3 +- ((val1-1) / 2) * 6;	// AGI+WJdSoA-
			val3 = 5 +- (val1/2) * 5;	// +kGCN3ZbiZTtkg1iXUqA-
			ud->state.change_speed = 1;
			calc_flag = 1;
			break;
		case SC_SHRIMP:		/* +MKgw004JZic- */
			if(val2 >= 5)	// WATK/MATK+WJdSoA-
				val3 = 30;
			else if(val2 == 1)
				val3 = 5;
			else
				val3 = (val2 - 1) * 5;
			calc_flag = 1;
			break;
		case SC_FRESHSHRIMP:		/* +ZbCbrjBqMKgw0w- */
			val3 = 13000 - 2000 * val1;
			val2 = tick / val3;
			tick = val3;
			break;
		case SC_CHATTERING:			/* +MMEw4zC/MOow8zCw- */
			if(val1 >= 5)	// ATK/MATK+WJdSoA-
				val2 = 150;
			else if(val1 == 4)
				val2 = 100;
			else
				val2 = 10 +- val1 * 20;
			val3 = (val1>=5)? 35: 25;	// +eftS1ZAfXqZYl1Kg-
			ud->state.change_speed = 1;
			calc_flag = 1;
			break;
		case SC_HISS:				/* +i2ZiEg- */
			val4 = tick / 1000;
			tick = 1000;
			ud->state.change_speed = 1;
			calc_flag = 1;
			break;
		case SC_GROOMING:			/* +MLAw6zD8MN8w8zCw- */
			val2 = val1 * 10;	// Flee+WJdSoA-
			calc_flag = 1;
			break;
		case SC_SV_ROOTTWIST:	/* +MN4wvzC/MNMwbmg5MGMwUw- */
			val3 = tick / 1000;
			tick = 1000;
			break;
		case SC_CATNIPPOWDER:		/* +MKQwzDDPMMMwqzC3MOMw7zD8- */
			val2 = 30;	// +a2ZWaGU7ZINSmzBo-Matk+bhtcEQ-
			val3 = 50;	// HP+MPs-SP+gepxNlbeX6mRz1iXUqA-
			calc_flag = 1;
			break;
		case SC_BURNT:			/* +c0RwjlRq- */
			val3 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_CHILL:			/* +bDhORZcc- */
			status_change_end(bl,SC_BURNT,-1);
			break;
		case SC_MER_FLEE:		/* +UK1RdTDcMPwwyjC5-(FLEE) */
		case SC_MER_ATK:		/* +UK1RdTDcMPwwyjC5-(ATK) */
		case SC_MER_HIT:		/* +UK1RdTDcMPwwyjC5-(HIT) */
			val2 = val1 * 15;
			calc_flag = 1;
			break;
		case SC_MER_HP:			/* +UK1RdTDcMPwwyjC5-(HP) */
		case SC_MER_SP:			/* +UK1RdTDcMPwwyjC5-(SP) */
			val2 = val1 * 5;
			calc_flag = 1;
			break;
		case SC_ON_PUSH_CART:	/* +MKsw/DDI- */
			icon_val1 = val1;	// +MKsw/DDIML8wpDDXMJJuITBZ-
			ud->state.change_speed = 1;
			calc_flag = 1;
			if(sd && val2) {
				clif_cart_itemlist(sd);
				clif_cart_equiplist(sd);
				clif_updatestatus(sd,SP_CARTINFO);
			}
			val2 = 1;
			break;
		case SC_SUMMON_ELEM:	/* +MLUw4jDzMKgw7DDhMPMwvzDr- */
			val2 = tick / 10000;
			tick = 10000;
			val3 = val1 * 3 +- 2;		// SP+bYiMu5HP-
			break;
		case SC_FIRE_CLOAK:		/* +MNUwoTCkMKIw/DCvMO0w/DCv- */
		case SC_WATER_DROP:		/* +MKYwqTD8ML8w/DDJMO0wwzDX- */
		case SC_WIND_CURTAIN:	/* +MKYwozDzMMkwqzD8MMYw8w- */
		case SC_STONE_SHIELD:	/* +MLkwyDD8MPMwtzD8MOswyQ- */
			val2 = 100;		// +XF5gJ4AQYCdYl1KgUCQ-
			val3 = 100;		// +XF5gJ4AQYCduG1wRUCQ-
			calc_flag = 1;
			break;
		case SC_WATER_SCREEN:	/* +MKYwqTD8ML8w/DC5MK8w6jD8MPM- */
			val2 = tick / 5000;
			tick = 5000;
			break;
		case SC_WIND_STEP:			/* +MKYwozDzMMkwuTDGMMMw1w- */
			val2 = 10;	// Flee+WJdSoFAk-
			val3 = 50;	// +eftS1ZAfXqZYl1Kgc4c-
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_SOLID_SKIN:			/* +ML0w6jDDMMkwuTCtMPM- */
			val2 = 10;	// Def+WJdSoHOH-
			val3 = 25;	// MaxHP+WJdSoHOH-
			calc_flag = 1;
			break;
		case SC_PYROTECHNIC:	/* +MNEwpDDtMMYwrzDLMMMwrw- */
			val2 = 40;	// Atk+WJdSoFAk-
			val3 = 10;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_HEATER:	/* +MNIw/DC/MPw- */
			val2 = 80;	// Atk+WJdSoFAk-
			val3 = 20;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_TROPIC:	/* +MMgw7TDUMMMwrw- */
			val2 = 120;	// Atk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_AQUAPLAY:	/* +MKIwrzCiMNcw7DCk- */
			val2 = 40;	// Matk+WJdSoFAk-
			val3 = 10;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_COOLER:	/* +MK8w/DDpMPw- */
			val2 = 80;	// Matk+WJdSoFAk-
			val3 = 20;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_CHILLY_AIR:	/* +MK8w/DDrMKgwojD8- */
			val2 = 120;	// Matk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_GUST:	/* +MKwwuTDI- */
			val2 = 50;	// Aspd+WJdSoFAk-
			val3 = 10;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_BLAST:	/* +MNYw6TC5MMg- */
			val2 = 50;	// Aspd+WJdSoFAk-
			val3 = 10;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_WILD_STORM:	/* +MO8wpDDrMMkwuTDIMPww4A- */
			val2 = 50;	// Aspd+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_PETROLOGY:	/* +MNowyDDtMLgw/A- */
			val2 = 5;	// MaxHP+WJdSoHOH-
			val3 = 10;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_CURSED_SOIL:	/* +MKsw/DC5MMkwvTCkMOs- */
			val2 = 10;	// MaxHP+WJdSoHOH-
			val3 = 20;	// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_UPHEAVAL:	/* +MKIwwzDXMNgwpDDQMOs- */
			val2 = 15;	// MaxHP+WJdSoHOH-
			calc_flag = 1;
			break;
		case SC_TIDAL_WEAPON_OPTION:	/* +ML8wpDDAMOswpjCnMN0w8w-(+fL6XCg-) */
			val2 = 20;	// Atk+WJdSoHOH-
			calc_flag = 1;
			break;
		case SC_CIRCLE_OF_FIRE_OPTION:	/* +MLUw/DCvMOswqjDWMNUwoTCkMKI-(+fL6XCg-) */
		case SC_FIRE_CLOAK_OPTION:		/* +MNUwoTCkMKIw/DCvMO0w/DCv-(+fL6XCg-) */
		case SC_WATER_SCREEN_OPTION:	/* +MKYwqTD8ML8w/DC5MK8w6jD8MPM-(+fL6XCg-) */
		case SC_WATER_DROP_OPTION:		/* +MKYwqTD8ML8w/DDJMO0wwzDX-(+fL6XCg-) */
		case SC_WIND_STEP_OPTION:		/* +MKYwozDzMMkwuTDGMMMw1w-(+fL6XCg-) */
		case SC_WIND_CURTAIN_OPTION:	/* +MKYwozDzMMkwqzD8MMYw8w-(+fL6XCg-) */
		case SC_SOLID_SKIN_OPTION:		/* +ML0w6jDDMMkwuTCtMPM-(+fL6XCg-) */
		case SC_STONE_SHIELD_OPTION:	/* +MLkwyDD8MPMwtzD8MOswyQ-(+fL6XCg-) */
			val2 = tick / 1000;
			tick = 1000;
			val3 = 5;		// SP+bYiMu5HP-
			break;
		case SC_PYROTECHNIC_OPTION:	/* +MNEwpDDtMMYwrzDLMMMwrw-(+fL6XCg-) */
		case SC_AQUAPLAY_OPTION:	/* +MKIwrzCiMNcw7DCk-(+fL6XCg-) */
		case SC_GUST_OPTION:		/* +MKwwuTDI-(+fL6XCg-) */
		case SC_PETROLOGY_OPTION:	/* +MNowyDDtMLgw/A-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = 10;		// SP+bYiMu5HP-
			break;
		case SC_HEATER_OPTION:		/* +MNIw/DC/MPw-(+fL6XCg-) */
		case SC_COOLER_OPTION:		/* +MK8w/DDpMPw-(+fL6XCg-) */
		case SC_BLAST_OPTION:		/* +MNYw6TC5MMg-(+fL6XCg-) */
		case SC_CURSED_SOIL_OPTION:	/* +MKsw/DC5MMkwvTCkMOs-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = 20;		// SP+bYiMu5HP-
			break;
		case SC_TROPIC_OPTION:		/* +MMgw7TDUMMMwrw-(+fL6XCg-) */
		case SC_CHILLY_AIR_OPTION:	/* +MK8w/DDrMKgwojD8-(+fL6XCg-) */
		case SC_WILD_STORM_OPTION:	/* +MO8wpDDrMMkwuTDIMPww4A-(+fL6XCg-) */
		case SC_UPHEAVAL_OPTION:	/* +MKIwwzDXMNgwpDDQMOs-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = 30;		// SP+bYiMu5HP-
			break;
		case SC_PARALYZE:			/* +nrt1+g- */
			val2 = 1000;		// +XzdSNopgVTFmQpWT-(ms)
			val3 = val1 * 5;	// Def, Mdef+bhtcEXOH-
			break;
		case SC_PAIN_KILLER:		/* +MNowpDDzMK0w6TD8- */
			val4 = val1 * 100;	// +MMAw4TD8MLhuG1wRc4c-
			val4 = val4 * val2 / 100 * val3 / 150;
			calc_flag = 1;
			break;
		case SC_LIGHT_OF_REGENE:	/* +UY11HzBuUUk- */
			val2 = 20 * val1;
			break;
		case SC_OVERED_BOOST:		/* +MKow/DDQMPwwyTDWMPwwuTDI- */
			val3 = 300 +- val1 * 40;	// Flee+VvpbmlAk-
			val4 = 410 - val1 * 40;	// Aspd+VvpbmlAk-
			calc_flag = 1;
			break;
		case SC_ANGRIFFS_MODUS:		/* +MKIw8zCwMOow1TC5MOIwyTC5- */
			val2 = tick / 1000;
			val3 = 30 +- val1 * 20;	// Def+bhtcEVAk-
			val4 = 40 +- val1 * 20;	// Flee+bhtcEVAk-
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_GOLDENE_FERSE:		/* +MLQw/DDrMMcw8zDaMOswuDCn- */
			val2 = 10 +- val1 * 10;	// Flee+TgpmB1Ak-
			val3 = 6 +- val1 * 4;	// Aspd+TgpmB3OH-
			val4 = 25;			// +gFZcXmAnj/1SoDDAMOEw/DC4-
			calc_flag = 1;
			break;
		case SC_CBC:		/* +fV4wgWKA- */
			val2 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_EQC:				/* E.Q.C */
			val3 = 2 * val1;	// MaxHP+bhtcEXOH-
			val4 = 5 * val1;	// Atk+MAE-Def+bhtcEXOH-
			calc_flag = 1;
			break;
		case SC_MAGMA_FLOW:		/* +MN4wsDDeMNUw7TD8- */
			val2 = 3 * val1;	// +dnpS1XOH-
			break;
		case SC_GRANITIC_ARMOR:	/* +MLAw6TDLMMYwozDDMK8wojD8MN4w/A- */
			val2 = 2 * val1;	// +MMAw4TD8MLhuG1wRc4c-
			if(val1 >= 4)	// +bYiMuw-HP+c4c-
				val3 = (val1 - 2) * 5;
			else if(val1 >= 2)
				val3 = (val1 - 1) * 3;
			else
				val3 = 1;
			break;
		case SC_PYROCLASTIC:	/* +MNEwpDDtMK8w6TC5MMYwozDDMK8- */
			val2 = 10 * val1 +- val4;	// +j/1SoA-Atk
			val3 = 2 * val1;	// HF+c4c-
			calc_flag = 1;
			break;
		case SC_VOLCANIC_ASH:	/* +cGtccXBw- */
			val2 = val3 = val4 = 0;
			if(bl->type == BL_PC || bl->type == BL_MOB)
				val2 = 50;	// +UWhb/oxh-
			if(bl->type == BL_MOB) {
				if(status_get_race(bl) == RCT_PLANT)
					val3 = 50;	// +W/5pDXJp-Mob
				if((status_get_element(bl)%20) == ELE_WATER)
					val4 = 50;	// +W/5sNFxeYCc-Mob
			}
			calc_flag = 1;
			break;
		case SC_FULL_THROTTLE:	/* +MNUw6zC5MO0wwzDIMOs- */
			val2 = 20;
			val3 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_REBOUND:	/* +MOow0DCmMPMwyQ- */
			val2 = tick / 2000;
			tick = 2000;
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_DAMAGE_HEAL:
			switch( val1 ) {
			case 1:
				val2 = BF_WEAPON;
				break;
			case 2:
				val2 = BF_MAGIC;
				break;
			case 3:
				val2 = BF_MISC;
				break;
			}
			break;
		case SC_GRADUAL_GRAVITY:	/* +kc1Sm1iXUqA- */
		case SC_KILLING_AURA:	/* +MK0w6jDzMLAwqjD8MOk- */
			val3 = tick / 1000;
			tick = 1000;
			break;
		case SC_LUNARSTANCE:	/* +ZwgwbmnLMEg- */
			tick = 600*1000;
			calc_flag = 1;
			val2 = val1 * 10 - 5;
			break;
		case SC_UNIVERSESTANCE:	/* +W4dbmTBuacswSA- */
		case SC_SUNSTANCE:	/* +WSqWfTBuacswSA- */
		case SC_STARSTANCE:	/* +Zh8wbmnLMEg- */
			tick = 600*1000;
			calc_flag = 1;
			val2 = val1 * 5;
			break;
		case SC_LIGHTOFMOON:	/* +ZwgwblFJ- */
		case SC_LIGHTOFSUN:	/* +WSqWfTBuUUk- */
		case SC_LIGHTOFSTAR:	/* +Zh8wblFJ- */
			val2 = 5 * val1 +- 25;
			break;
		case SC_FLASHKICK:	/* +lYNRSYEa- */
		case SC_NOVAEXPLOSING:	/* +ZbBmH3IGdno- */
		case SC_GRAVITYCONTROL:	/* +kc1Sm4q/e8A- */
		case SC_CREATINGSTAR:	/* +UnVmHzBuZvg- */
		case SC_DIMENSION:	/* +ayFRQzBuZvg- */
		case SC_DIMENSION1:	/* +ayFRQzBuZvg-(+ZbBmH3IGdno-) */
			break;
		case SC_NEWMOON:		/* +ZxRnCIEa- */
			val2 = 15;	// +MMAw4TD8MLiAEGAn-
			val3 = tick / 1000;
			tick = 1000;
			break;
		case SC_FALLINGSTAR:	/* +bUFmH4Q9Tgs- */
			val2 = val1 <= 5 ? 20 : 25;
			break;
		case SC_DIMENSION2:	/* +ayFRQzBuZvg-(+Zh9eHZZNgeg-) */
			val2 = 2;		// +dv4wblALZXA-
			val3 = val4;	// +dv4wboAQTkVQJA-
			if(sd)
				clif_mshield(sd, val2);
			break;
		case SC_SOULCOLLECT:		/* +m0IwboTEek0- */
			val2 = tick;		// +ML0wpjDrMKgwyjC4MPxzcl+XZkKVkw-
			icon_tick = -1;
			break;
		case SC_SOULREAPER:			/* +m0IwblPOems- */
			val2 = val1;		// +ML0wpjDrMKgwyjC4MPxzcl+Xc4c-
			break;
		case SC_SOULUNITY:		/* +m0IwbpAjfVA- */
			{
				int i, add = 600;
				for(i = 0; i < val1; i+-+-) {
					add +-= 100;
					val2 = val2 +- add;		// HP+Vt5fqZHP-
				}
				val3 = tick / 3000;
				tick = 3000;
			}
			break;
		case SC_SOULSHADOW:		/* +X3EwbptC- */
			val2 = 70;				// Cri+WJdSoFAk-
			val3 = (val1+-1)/2 * 10;	// Aspd+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SOULFAIRY:		/* +WZZ8vjBum0I- */
			val2 = 50;				// +iMVQmQ-Matk+WJdSoFAk-
			val3 = 25 +- val1 * 5;	// +imBVMWZClZNuG1wRc4c-
			calc_flag = 1;
			break;
		case SC_SOULFALCON:		/* +nfkwbptC- */
			val2 = 50;				// +iMVQmQ-Atk+WJdSoFAk-
			val3 = 25 +- val1 * 25;	// Hit+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SOULGOLEM:		/* +MLQw/DDsMOAwbptC- */
			val2 = 100;				// +iMVQmQ-Def+WJdSoFAk-
			val3 = val1 * 10;		// +iMVQmQ-Mdef+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SOULDIVISION:	/* +m0IwblIGiMI- */
			val2 = 100;				// +MMcwozDsMKRYl1Kgc4c-
			break;
		case SC_SHA:			/* +MKgwuTDP- */
			val2 = 50;				// +eftS1ZAfXqZPTk4Lc4c-
			ud->state.change_speed = 1;
			break;
		case SC_HELPANGEL:		/* +WSlPfzBVMH5SqTBRMGY- */
			val2 = tick / 1000;
			tick = 1000;
			break;
		case SC_HANDICAPSTATE_DEEPBLIND:		/* +bwae0g- */
			calc_flag = 1;
			icon_tick = tick -= status_get_sta(bl) * 100;
			break;
		case SC_HANDICAPSTATE_DEEPSILENCE:		/* +l1lbwg- */
			val2 = 500;		// ASPD+bhtcEVAk-
			calc_flag = 1;
			icon_tick = tick -= status_get_spl(bl) * 100;
			break;
		case SC_HANDICAPSTATE_LASSITUDE:		/* +cSFsF1Kb- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			icon_tick = tick -= status_get_pow(bl) * 100;
			break;
		case SC_HANDICAPSTATE_FROSTBITE:		/* +YCVRtw- */
			val2 = 50;		// Def/Mdef+bhtcEXOH-
			calc_flag = 1;
			icon_tick = tick -= status_get_crt(bl) * 100;
			break;
		case SC_HANDICAPSTATE_SWOONING:			/* +WTF5Xg- */
			icon_tick = tick -= status_get_sta(bl) * 100;
			break;
		case SC_HANDICAPSTATE_LIGHTNINGSTRIKE:	/* +b8BtQQ- */
			val2 = 25;		// +VzBcXmAnMMAw4TD8MLhYl1Kgc4c-
			icon_tick = tick -= status_get_wis(bl) * 100;
			break;
		case SC_HANDICAPSTATE_CRYSTALLIZATION:	/* +fVBmdlMW- */
			calc_flag = 1;
			icon_tick = tick -= status_get_crt(bl) * 100;
			break;
		case SC_HANDICAPSTATE_CONFLAGRATION:	/* +cGtwfQ- */
			icon_tick = tick -= status_get_spl(bl) * 100;
			val2 = tick / 3000;
			tick = 3000;
			break;
		case SC_HANDICAPSTATE_MISFORTUNE:		/* +Tg1eeA- */
			calc_flag = 1;
			icon_tick = tick -= status_get_crt(bl) * 100;
			break;
		case SC_HANDICAPSTATE_DEADLYPOISON:		/* +Xzdr0g- */
			val2 = 25;		// +a9JcXmAnMMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			icon_tick = tick -= status_get_sta(bl) * 100;
			val3 = tick / 1000;
			tick = 1000;
			break;
		case SC_HANDICAPSTATE_DEPRESSION:		/* +YYKbMQ- */
			icon_tick = tick -= status_get_wis(bl) * 100;
			val2 = tick / 3000;
			tick = 3000;
			break;
		case SC_SERVANTWEAPON:		/* +MLUw/DD0MKEw8zDIMKYwpzDdMPM- */
			val2 = 10 * val1;		// +a2ZWaE9TZTtkg3Z6UtVzhw-
			val3 = skill_get_time2(DK_SERVANTWEAPON,val1);	// +a2ZWaE9TdR9iEGZClZM-
			val3 = (val3 < 100)? 100: val3;
			val4 = tick / val3;
			tick = val3;
			if(sd)
				pc_addball(sd,600000,MAX_BALL);
			break;
		case SC_CHARGINGPIERCE_COUNT:		/* +MMEw4zD8MLgw8zCwMLkw1DCi-(+Vt5lcA-) */
			icon_val1 = val1;	// val1(+Vt5lcA-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			break;
		case SC_CLIMAX_DES_HU:	/* +MK8w6TCkMN4wwzCvMLkwzzDqMLEw/DDz- */
			val2 = 200;			// Matk+WJdSoFAk-
			val3 = 50;			// +mKhcXmAnMMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_CLIMAX:			/* +MK8w6TCkMN4wwzCvMLk- */
			icon_val1 = val1;	// val1(+T391KDDsMNkw6w-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			break;
		case SC_CLIMAX_EARTH:	/* +MK8w6TCkMN4wwzCvMLkwrzCoMKQwrw- */
			val2 = 50;			// +VzBcXmAnMMAw4TD8MLhYl1Kgc4c-
			break;
		case SC_CLIMAX_BLOOM:	/* +MK8w6TCkMN4wwzCvMLkw1jDrMPww4A- */
			val2 = 50;			// +cGtcXmAnMMAw4TD8MLhYl1Kgc4c-
			break;
		case SC_CLIMAX_CRYIMP:	/* +MK8w6TCkMN4wwzCvMLkwpDDzMNEwrzDI- */
			val2 = 300;			// Def+WJdSoFAk-
			val3 = 50;			// Mdef+WJdSoFAk-
			val4 = 25;			// +bDRcXmAnMMAw4TD8MLhuG1wRc4c-/+bDRcXmAnMMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_POWERFUL_FAITH:		/* +XzeXbTBqT+Ff9Q- */
			val2 = 100;			// Atk+WJdSoFAk-
			val3 = (val1 > 2)? 5*(val1-2): 1+-2*(val1-1);	// P.Atk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_FIRM_FAITH:		/* +WAVW+jBqT+Ff9Q- */
			val2 = 10;			// MaxHP+WJdSoHOH-
			val3 = (val1 > 2)? 10+-30*(val1-2): 10*val1;		// Res+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SINCERE_FAITH:		/* +X+BbnzBqT+Ff9Q- */
			val2 = 10;			// Aspd+WJdSoFAk-
			val3 = (val1 > 2)? 5*(val1-2): 1+-2*(val1-1);	// +X8VOLWU7ZINYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_GUARD_STANCE:		/* +MKww/DDJMLkwvzDzMLk- */
			val2 = (val1 > 4)? 60*val1: 50*val1;		// Def+WJdSoFAk-
			val3 = 10*val1;		// Atk+bhtcEVAk-
			calc_flag = 1;
			break;
		case SC_ATTACK_STANCE:		/* +MKIwvzDDMK8wuTC/MPMwuQ- */
			val2 = (val1 > 2)? 5*(val1-2): 1+-2*(val1-1);	// P.Atk/S.Matk+WJdSoFAk-
			val3 = 10*val1;		// Def+bhtcEVAk-
			calc_flag = 1;
			break;
		case SC_HOLY_S:		/* +MNsw/DDqMPwwtzD8MOswyQ- */
			val2 = 5*val1;		// +gFZcXmAnm1Rs1TDAMOEw/DC4WJdSoHOH-
			val3 = 3*val1;		// +lcc-/+Tg1re1xeYCcwwDDhMPwwuG4bXBFzhw-
			calc_flag = 1;
			break;
		case SC_MEDIALE:		/* +MOEwxzCjMKIw6jDcMMgwpTDg- */
			//val2				// +UrlnnHvEVvI-(+MLkwrTDrT391KGZCMGuKLVua-)
			val3 = skill_get_time2(CD_MEDIALE_VOTUM,val1);	// +Vt5fqZWTlpQ-
			val3 = (val3 < 100)? 100: val3;
			val4 = tick / val3;
			tick = val3;
			skill_castend_nodamage_id(bl,bl,CD_MEDIALE_VOTUM,val1,tick,0x10|val2);
			break;
		case SC_A_VITA:			/* +MKIw6zCwMMgwpTC5MPQwozC/- */
			val2 = 5*val1;		// Mres+cSGJlnOH-
			break;
		case SC_A_TELUM:		/* +MKIw6zCwMMgwpTC5MMYw6zDg- */
			val2 = 5*val1;		// Res+cSGJlnOH-
			break;
		case SC_PRE_ACIES:		/* +MNcw7DC7MPMwuTCiMK0wqDD8MLk- */
			val2 = 5*val1;		// C.Rate+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_COMPETENTIA:	/* +MLMw8zDaMMYw8zDGMKMwog- */
			val2 = 20+-val1*2;	// P.Atk+WJdSoFAk-
			val3 = 20+-val1*2;	// S.Matk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_RELIGIO:		/* +MOww6jCuMKo- */
			// +MLUw8zDJMNUwpzC5MMYwozDQMOswTGObMEswYzBmMEQwXzCJieOWZA-
			if(sc->data[SC_SANDY_FESTIVAL].timer != -1)
				status_change_end(bl,SC_SANDY_FESTIVAL,-1);
			val2 = val1*2;		// Sta+WJdSoFAk-
			val3 = val1*2;		// Wis+WJdSoFAk-
			val4 = val1*2;		// Spl+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_BENEDICTUM:		/* +MNkwzTDHMKMwrzDIMKUw4A- */
			// +MN4w6jDzMNUwpzC5MMYwozDQMOswTGObMEswYzBmMEQwXzCJieOWZA-
			if(sc->data[SC_MARINE_FESTIVAL].timer != -1)
				status_change_end(bl,SC_MARINE_FESTIVAL,-1);
			val2 = val1*2;		// Pow+WJdSoFAk-
			val3 = val1*2;		// Con+WJdSoFAk-
			val4 = val1*2;		// Crt+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_DANCING_KNIFE:	/* +MMAw8zC3MPMwsDDKMKQw1Q- */
			val2 = skill_get_time2(SHC_DANCING_KNIFE,val1);	// +ZTtkg5WTlpQ-
			val2 = (val2 < 100)? 100: val2;
			val3 = tick / val2;
			tick = val2;
			break;
		case SC_E_SLASH_COUNT:		/* +MKgwvzD8MMow6zCrMKYw8zC/MPw- */
			icon_val1 = val1;	// val1(+Vt5lcA-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			break;
		case SC_POTENT_VENOM:		/* +MN0wxjDzMMgw2TDKMOA- */
			val2 = 20+-val1;		// Res+cSGJlnOH-
			break;
		case SC_SHADOW_WEAPON:		/* +MKgw8zDBMOMw8zDGMKMw8zCwMLcw4zDJMKY- */
			val2 = val1;		// +MLcw4zDJMKYw2jCkMPNO2E4Oc4c-
			break;
		case SC_SHADOW_SCAR:		/* +MLcw4zDJMKYw2jCkMPM- */
			icon_val1 = val1;		// val1(+Vt5lcA-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			val2 = val1 * 3;		// +j9FjpTDAMOEw/DC4WJdSoHOH-
			break;
		case SC_RUSH_QUAKE1:		/* +MK8wqDCkMK8- */
			val2 = 15;				// +cml0BjDAMOEw/DC4WJdSoHOH-
			break;
		case SC_RUSH_QUAKE2:		/* +MOkwwzC3MOU- */
			val2 = val1 * 2;		// +j9FjpQ-/+kGCN3ZbiMMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_A_MACHINE:		/* +ZTtkg4jFf25nCVK5UxY- */
			val2 = skill_get_time2(MT_A_MACHINE,val1);	// +ZTtkg5WTlpQ-
			val2 = (val2 < 100)? 100: val2;
			val3 = tick / val2;
			tick = val2;
			break;
		case SC_D_MACHINE:		/* +ljJfoYjFf25nCVK5UxY- */
			val2 = 300;			// Def+WJdSoFAk-
			val3 = ((val1 < 4)? 10+-val1*10: (val1 == 4)? 60: 100);		// Res+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SHADOW_STRIP:	/* +MLkwyDDqMMMw1zC3MOMwyTCm- */
			val2 = 50;			// Res/Mres+bhtcEVAk-
			calc_flag = 1;
			break;
		case SC_ABYSS_SLAYER:	/* +MKIw0zC5MLkw7DCkMOQw/A- */
			val2 = val1 * 2;		// P.Atk/S.Matk+WJdSoFAk-
			val3 = (val1+-1)/2 * 50;	// Hit+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_ABYSSFORCEWEAPON:		/* +MNUw7TDgMLgwojDTMLk- */
			val2 = 10 * val1;		// +MKIw0zC5dANPU2U7ZIN2elLVc4c-
			val3 = skill_get_time2(ABC_FROM_THE_ABYSS,val1);	// +MKIw0zC5dANPU3UfYhBmQpWT-
			val3 = (val3 < 100)? 100: val3;
			val4 = tick / val3;
			tick = val3;
			if(sd)
				pc_addball(sd,600000,MAX_BALL);
			break;
		case SC_WINDSIGN:	/* +MKYwozDzMMkwtTCkMPM- */
			val2 = 50 +- val1 * 10;		// AP+Vt5fqXi6c4c-
			break;
		case SC_BO_HELL_DUSTY:		/* +MNgw6zDAMLkwxjCj- */
			val2 = 20;		// +cSEw+2kNcmlfYjDAMOEw/DC4WJdSoHOH-
			val3 = 20;		// +kGCN3ZbiMMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_MYSTIC_SYMPHONY:	/* +MN8wuTDGMKMwwzCvMLcw8zDVMKkwyzD8- */
			val2 = 50;		// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_SOUNDBLEND:			/* +MLUwpjDzMMkw1jDsMPMwyQ- */
			tick = tick - 200;
			break;
		case SC_GEF_NOCTURN:		/* +MLIw1TCnMMswojDOMK8wvzD8MPM- */
			val3 = 10 * val1;
			if(val2 > 1) {		// +MNEw/DDIMMow/DBMXEUwi1g0VAg-
				val3 = val3 * 15 / 10;
			}
			calc_flag = 1;
			break;
		case SC_AIN_RHAPSODY:		/* +knFU4TBuMOkw1zC9MMcwow- */
			val3 = 10 * val1;
			if(val2 > 1) {		// +MNEw/DDIMMow/DBMXEUwi1g0VAg-
				val3 = val3 * 15 / 10;
			}
			calc_flag = 1;
			break;
		case SC_MUSICAL_INTERLUDE:	/* +MN8w5TD8MLgwqzDrMKQw8zC/MPww6zD8MMk- */
			val3 = ((val1 < 4)? 10+-val1*10: (val1 == 4)? 60: 100);		// Res+WJdSoFAk-
			if(val2 > 1) {		// +MNEw/DDIMMow/DBMXEUwi1g0VAg-
				val3 = val3 * 15 / 10;
			}
			calc_flag = 1;
			break;
		case SC_JAWAII_SERENADE:	/* +WRVxPDBRMG4wuzDsMMow/DDH- */
			val3 = 1 +- ((val1 > 1)? 2: 0) +- ((val1 > 2)? 2: 0) +- ((val1 > 3)? 3: 0) +- ((val1 > 4)? 4: 0);		// S.Matk+WJdSoFAk-
			if(val2 > 1) {		// +MNEw/DDIMMow/DBMXEUwi1g0VAg-
				val3 = val3 * 15 / 10;
			}
			val4 = 35;			// +eftS1TDHMKMw7DCkbhtcEXOH-
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_PRON_MARCH:			/* +MNcw7TDzMMYw6TDeMPwwwQ- */
			val3 = 1 +- ((val1 > 1)? 2: 0) +- ((val1 > 2)? 2: 0) +- ((val1 > 3)? 3: 0) +- ((val1 > 4)? 4: 0);		// P.Atk+WJdSoFAk-
			if(val2 > 1) {		// +MNEw/DDIMMow/DBMXEUwi1g0VAg-
				val3 = val3 * 15 / 10;
			}
			val4 = 35;			// +eftS1TDHMKMw7DCkbhtcEXOH-
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_SPELL_ENCHANTING:	/* +MLkw2jDrMKgw8zDBMOMw8zDGMKMw8zCw- */
			val2 = val1*5;	// S.Matk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SUMMON_ELEMENTAL_ARDOR:			/* +MLUw4jDzMKIw6zDJMPww6w- */
		case SC_SUMMON_ELEMENTAL_DILUVIO:		/* +MLUw4jDzMMcwozDrMNMwqg- */
		case SC_SUMMON_ELEMENTAL_PROCELLA:		/* +MLUw4jDzMNcw7TC7MOk- */
		case SC_SUMMON_ELEMENTAL_TERREMOTUS:	/* +MLUw4jDzMMYw7DDiMMgwpTC5- */
		case SC_SUMMON_ELEMENTAL_SERPENS:		/* +MLUw4jDzMLUw/DDaMPMwuQ- */
			val2 = 10;	// +XF5gJ5tUbNUwwDDhMPwwuFiXUqBzhw-
			val3 = tick / 1000;
			tick = 1000;
			calc_flag = 1;
			break;
		case SC_FLAMETECHNIC:		/* +MNUw7DCkMOAwxjCvMMswwzCv- */
		case SC_COLD_FORCE:			/* +MLMw/DDrMMkw1TCpMPwwuQ- */
		case SC_GRACE_BREEZE:		/* +MLAw7DCkMLkw1jDqMPwwug- */
		case SC_EARTH_CARE:			/* +MKIw/DC5MLEwog- */
			val2 = 75;		// +MNww6zDIfPswwDDhMPwwuFiXUqBzhw-
			calc_flag = 1;
			break;
		case SC_DEEP_POISONING:		/* +MMcwozD8MNcw3TCkMLowyzDzMLA- */
			val2 = 200;		// +MK8w6TCmMMkwrTDrMMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_FLAMEARMOR:			/* +MNUw7DCkMOAwojD8MN4w/A- */
		case SC_CRYSTAL_ARMOR:		/* +MK8w6jC5ML8w6zCiMPww3jD8- */
		case SC_EYES_OF_STORM:		/* +MKIwpDC6MKow1jC5MMgw/DDg- */
		case SC_STRONG_PROTECTION:	/* +MLkwyDDtMPMwsDDXMO0wxjCvMLcw5zDz- */
		case SC_POISON_SHIELD:		/* +MN0wpDC6MPMwtzD8MOswyQ- */
			val2 = 95;		// +XF5gJzDAMOEw/DC4jv1uG3OH-
			calc_flag = 1;
			break;
		case SC_FLAMETECHNIC_OPTION:		/* +MNUw7DCkMOAwxjCvMMswwzCv-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = skill_get_sp(EM_EL_FLAMETECHNIC,val1);		// SP+bYiMu5HP-
			break;
		case SC_FLAMEARMOR_OPTION:			/* +MNUw7DCkMOAwojD8MN4w/A-(+fL6XCg-) */
			val2 = tick / 1000;
			tick = 1000;
			val3 = skill_get_sp(EM_EL_FLAMEARMOR,val1) / 10;		// SP+bYiMu5HP-
			break;
		case SC_COLD_FORCE_OPTION:			/* +MLMw/DDrMMkw1TCpMPwwuQ-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = skill_get_sp(EM_EL_COLD_FORCE,val1);		// SP+bYiMu5HP-
			break;
		case SC_CRYSTAL_ARMOR_OPTION:		/* +MK8w6jC5ML8w6zCiMPww3jD8-(+fL6XCg-) */
			val2 = tick / 1000;
			tick = 1000;
			val3 = skill_get_sp(EM_EL_CRYSTAL_ARMOR,val1) / 10;		// SP+bYiMu5HP-
			break;
		case SC_GRACE_BREEZE_OPTION:		/* +MLAw7DCkMLkw1jDqMPwwug-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = skill_get_sp(EM_EL_GRACE_BREEZE,val1);		// SP+bYiMu5HP-
			break;
		case SC_EYES_OF_STORM_OPTION:		/* +MKIwpDC6MKow1jC5MMgw/DDg-(+fL6XCg-) */
			val2 = tick / 1000;
			tick = 1000;
			val3 = skill_get_sp(EM_EL_EYES_OF_STORM,val1) / 10;		// SP+bYiMu5HP-
			break;
		case SC_EARTH_CARE_OPTION:			/* +MKIw/DC5MLEwog-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = skill_get_sp(EM_EL_EARTH_CARE,val1);		// SP+bYiMu5HP-
			break;
		case SC_STRONG_PROTECTION_OPTION:	/* +MLkwyDDtMPMwsDDXMO0wxjCvMLcw5zDz-(+fL6XCg-) */
			val2 = tick / 1000;
			tick = 1000;
			val3 = skill_get_sp(EM_EL_STRONG_PROTECTION,val1) / 10;		// SP+bYiMu5HP-
			break;
		case SC_DEEP_POISONING_OPTION:		/* +MMcwozD8MNcw3TCkMLowyzDzMLA-(+fL6XCg-) */
			val2 = tick / 10000;
			tick = 10000;
			val3 = skill_get_sp(EM_EL_DEEP_POISONING,val1);		// SP+bYiMu5HP-
			break;
		case SC_POISON_SHIELD_OPTION:		/* +MN0wpDC6MPMwtzD8MOswyQ-(+fL6XCg-) */
			val2 = tick / 1000;
			tick = 1000;
			val3 = skill_get_sp(EM_EL_POISON_SHIELD,val1) / 10;		// SP+bYiMu5HP-
			break;
		case SC_INTENSIVE_AIM:			/* +MKQw8zDGMPMwtzDWMKgwpDDg- */
			tick = 300;
			val2 = 100;			// +iMVQmQ-Atk+WJdSoFAk-
			val3 = 250;			// Hit+WJdSoFAk-
			val4 = 50;			// Cri+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_INTENSIVE_AIM_COUNT:	/* +MKQw8zDGMPMwtzDWMKgwpDDg-(+MKswpjDzMMg-) */
			tick = 600*1000;
			icon_val1 = val1;	// val1(+Vt5lcA-)+MJJuITBXMGYwojCkMLMw84hoeTowWTCL-
			break;
		case SC_GRENADE_FRAGMENT_1:		/* +MLAw7DDNMPwwyTDVMOkwsDDhMPMwyA-(+bDRcXmAn-) */
		case SC_GRENADE_FRAGMENT_2:		/* +MLAw7DDNMPwwyTDVMOkwsDDhMPMwyA-(+mKhcXmAn-) */
		case SC_GRENADE_FRAGMENT_3:		/* +MLAw7DDNMPwwyTDVMOkwsDDhMPMwyA-(+VzBcXmAn-) */
		case SC_GRENADE_FRAGMENT_4:		/* +MLAw7DDNMPwwyTDVMOkwsDDhMPMwyA-(+cGtcXmAn-) */
		case SC_GRENADE_FRAGMENT_5:		/* +MLAw7DDNMPwwyTDVMOkwsDDhMPMwyA-(+lcdcXmAn-) */
		case SC_GRENADE_FRAGMENT_6:		/* +MLAw7DDNMPwwyTDVMOkwsDDhMPMwyA-(+gFZcXmAn-) */
			{
				int i;
				for(i = SC_GRENADE_FRAGMENT_1; i <= SC_GRENADE_FRAGMENT_6; i+-+-) {
					if(i == type)
						continue;
					if(sc->data[i].timer != -1)
						status_change_end(bl,i,-1);
				}
			}
			break;
		case SC_AUTO_FIRING_LAUNCHEREFST:	/* +MKow/DDIMNUwoTCkMKIw6jDzMLAw6TDzMMEw4zD8- */
			val2 = 450 +- val1 * 50;		// +MNkw/DC3MMMwrzCwMOwwzTD8MMl4unOH-(+TgdSBnOH-)
			val3 = 200 +- val1 * 50;		// +MNgwpDC5MMYwozDVMKEwpDCiMKQw8zC2MNsw/DDreLpzhw-(+TgdSBnOH-)
			val4 = 100 +- val1 * 50;		// +MLAw7DDNMPwwujDJMO0wwzDUMPMwsHi6c4c-(+TgdSBnOH-)
			break;
		case SC_HIDDEN_CARD:			/* +MNIwyTClMPMwqzD8MMk- */
			val2 = 5 +- val1;		// P.Atk+WJdSoFAk-
			val3 = 100 +- val1 * 5;	// +kGCN3Zbicml0BliXUqBQJA-
			calc_flag = 1;
			break;
		case SC_TALISMAN_OF_PROTECTION:	/* +W4iLd3sm- */
			val3 = tick / 3000;
			tick = 3000;
			break;
		case SC_TALISMAN_OF_WARRIOR:	/* +a2ZY63sm- */
			val2 = val1 * 2;		// P.Atk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_TALISMAN_OF_MAGICIAN:	/* +bNVeK3sm- */
			val2 = val1 * 2;		// S.Matk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_TALISMAN_OF_FIVE_ELEMENTS:	/* +TpSITHsm- */
			val2 = val1 * 2;		// +XF5gJzDiMPMwuTC/MPwwwDDhMPwwuFiXUqBzhw-
			calc_flag = 1;
			break;
		case SC_T_FIVETH_GOD:			/* +VttluU6UiEyWYw- */
			val2 = val1 * 2;		// S.Matk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_HEAVEN_AND_EARTH:		/* +WSlXMHlelwo- */
			val2 = 10 +- val1;		// +MMAw4TD8MLhYl1Kgc4c-
			calc_flag = 1;
			break;
		case SC_MARINE_FESTIVAL:	/* +MN4w6jDzMNUwpzC5MMYwozDQMOs- */
			// +MNkwzTDHMKMwrzDIMKUw4DBMY5swSzBjMGYwRDBfMImJ45Zk-
			if(sc->data[SC_BENEDICTUM].timer != -1)
				status_change_end(bl,SC_BENEDICTUM,-1);
			val2 = val1*2;		// Pow+WJdSoFAk-
			val3 = val1*2;		// Con+WJdSoFAk-
			val4 = val1*2;		// Crt+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SANDY_FESTIVAL:		/* +MLUw8zDJMNUwpzC5MMYwozDQMOs- */
			// +MOww6jCuMKowTGObMEswYzBmMEQwXzCJieOWZA-
			if(sc->data[SC_RELIGIO].timer != -1)
				status_change_end(bl,SC_RELIGIO,-1);
			val2 = val1*2;		// Sta+WJdSoFAk-
			val3 = val1*2;		// Wis+WJdSoFAk-
			val4 = val1*2;		// Spl+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_KI_SUL_RAMPAGE:		/* +ML8w/DDIMOsw6TDzMNow/DC4- */
			//val2				// +XzdTFjDVMOkwsA-(+MLkwrTDrT391KGZCMGuKLVua-)
			val3 = tick / 1000;
			tick = 1000;
			skill_castend_nodamage_id(bl,bl,SH_KI_SUL_RAMPAGE,val1,tick,0x10|val2);
			break;
		case SC_COLORS_OF_HYUN_ROK_1:		/* +MOwwpDDzMNww/DDbMPww8w-(+bDRcXmAn-) */
		case SC_COLORS_OF_HYUN_ROK_2:		/* +MOwwpDDzMNww/DDbMPww8w-(+mKhcXmAn-) */
		case SC_COLORS_OF_HYUN_ROK_3:		/* +MOwwpDDzMNww/DDbMPww8w-(+VzBcXmAn-) */
		case SC_COLORS_OF_HYUN_ROK_4:		/* +MOwwpDDzMNww/DDbMPww8w-(+cGtcXmAn-) */
		case SC_COLORS_OF_HYUN_ROK_5:		/* +MOwwpDDzMNww/DDbMPww8w-(+lcdcXmAn-) */
		case SC_COLORS_OF_HYUN_ROK_6:		/* +MOwwpDDzMNww/DDbMPww8w-(+gFZcXmAn-) */
			{
				int i;
				for(i = SC_COLORS_OF_HYUN_ROK_1; i <= SC_COLORS_OF_HYUN_ROK_6; i+-+-) {
					if(i == type)
						continue;
					if(sc->data[i].timer != -1)
						status_change_end(bl,i,-1);
				}
			}
			break;
		case SC_TEMPORARY_COMMUNION:	/* +TgmXCk4AT1M- */
			val2 = val1 * 3;		// P.Atk+WJdSoFAk-
			val3 = val1 * 3;		// S.Matk+WJdSoFAk-
			val4 = val1 * 3;		// H.Plus+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_BLESSING_OF_M_CREATURES:	/* +MGswgzCTMNYw7DDDMLcw8zCw- */
			val2 = val1 * 5;		// P.Atk+WJdSoFAk-
			val3 = val1 * 5;		// S.Matk+WJdSoFAk-
			calc_flag = 1;
			break;
		case SC_SHIELDCHAINRUSH:		/* +MLcw/DDrMMkwwTCnMPww8zDpMMMwtzDl- */
			val2 = 10;	// +cml0BjDAMOEw/DC4WJdSoHOH/whnKlufiMX/CQ-
			val3 = 20;	// +eftS1ZAfXqZPTk4Lc4c-
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_MISTYFROST:			/* +MLgw4zDDMK8w1TDtMLkwyDDOMPQwoQ- */
			val2 = 15;	// +bDRcXmAnMMAw4TD8MLhYl1Kgc4c-
			break;
		case SC_GROUNDGRAVITY:		/* +MLAw6TCmMPMwyTCwMOkw0zDGMPwwtzDnMPM- */
			val2 = 10;	// +MMAw4TD8MLhYl1Kgc4f/CGcqW5+Ixf8J-
			val3 = 30;	// +eftS1ZAfXqZPTk4Lc4c-
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_SHADOW_CLOCK:		/* +X3GWoDCM- */
			val2 = 85;	// +cml0BjD7m1Rs1TDAMOEw/DC4bhtcEXOH/whnKlufiMX/CQ-
			val3 = 30;	// +eftS1ZAfXqZYl1Kgc4c-
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_SKY_ENCHANT:		/* +WSlsFzBujqs- */
			{
				int i;
				// +ZeVR+v9eZwhsoTBvieOWZA-
				for(i = SC_RISING_SUN; i <= SC_DAWN_MOON; i+-+-) {
					if(sc->data[i].timer != -1)
						status_change_end(bl,i,-1);
				}
			}
			break;
		default:
			if(battle_config.error_log)
				printf("UnknownStatusChange [%d]+AFw-n", type);
			return 0;
	}

	if(tick <= 0)
		return 0;

	if(StatusIconChangeTable[type] != SI_BLANK)
		clif_status_change(bl,StatusIconChangeTable[type],1,icon_tick,icon_val1,icon_val2,icon_val3);	// +MKIwpDCzMPOIaHk6-

	/* +UfiXYpPhMG8wojCkMLMw85ABT+FfjDBrUeZ0BjBZMIs- */
	if(type == SC_BOSSMAPINFO) {
		struct mob_data *mmd = map[bl->m].mvpboss;
		if(sd && mmd) {
			if(mmd->bl.prev == NULL) {	// +UY1suDBNX4UwYU4t-
				int diff = DIFF_TICK(mmd->last_spawntime, current_tick);
				if(diff < 0)
					diff = 0;
				clif_bossmapinfo(sd, mmd->name, 0, 0, diff, 3);
				val3 = -1;
				val4 = -1;
			} else {			// +Ufpz/k4t-
				clif_bossmapinfo(sd, mmd->name, 0, 0, 0, 2);
				if(mmd->bl.m == bl->m) {
					clif_bossmapinfo(sd, "", mmd->bl.x, mmd->bl.y, 0, 1);
				}
				val3 = mmd->bl.m;
				val4 = mmd->bl.x +- (mmd->bl.y << 16);
			}
		}
		val2 = tick / 1000;
		tick = 1000;
	}

	/* option+MG5ZCWb0- */
	switch(type) {
		// opt1
		case SC_STONE:
		case SC_FREEZE:
		case SC_STUN:
		case SC_SLEEP:
			unit_stopattack(bl);	// +ZTtkg1Bca2I-
			skill_stop_dancing(bl,0);	// +bxRZTw-/+MMAw8zC5MG5OLWWt-
			{
				// +VAxmQjBrY5swSzCJMGowRDC5MMYw/DC/MLl1cF44MJKJ45Zk-
				int i;
				for(i = SC_STONE; i <= SC_SLEEP; i+-+-) {
					if(sc->data[i].timer != -1) {
						sc->count--;
						delete_timer(sc->data[i].timer, status_change_timer);
						sc->data[i].timer = -1;
					}
				}
			}
			if(type == SC_STONE)
				sc->opt1 = OPT1_STONECURSE_ING;
			else
				sc->opt1 = type - SC_STONE +- 1;

			if(md) {
				mob_unlocktarget(md, current_tick);
				md->attacked_id = 0;
				md->attacked_players = 0;
			}
			opt_flag = 1;
			break;
		case SC_HELLINFERNO:		/* +MNgw6zCkMPMw1TCnMOswzg- */
			sc->opt1 = OPT1_BURNNING;
			opt_flag = 1;
			break;
		case SC_WHITEIMPRISON:		/* +MNsw7zCkMMgwpDDzMNcw6jC6MPM- */
			sc->opt1 = OPT1_IMPRISON;
			opt_flag = 1;
			break;
		// opt2
		case SC_POISON:
		case SC_CURSE:
		case SC_SILENCE:
		case SC_CONFUSION:
			sc->opt2 |= OPT2_POISON<<(type-SC_POISON);
			opt_flag = 1;
			break;
		case SC_FOGWALLPENALTY:
		case SC_BLIND:
			if(sc->data[SC_FOGWALLPENALTY].timer == -1) {
				sc->opt2 |= OPT2_BLIND;
				opt_flag = 1;
				if(md && !(flag&2))
					md->target_id = 0;
			}
			break;
		case SC_ANGELUS:			/* +MKIw8zC8MOswuQ- */
			sc->opt2 |= OPT2_ANGELUS;
			opt_flag = 1;
			break;
		case SC_DPOISON:
			sc->opt2 |= OPT2_HEAVYPOISON;
			opt_flag = 1;
			break;
		case SC_BLEED:
			sc->opt2 |= OPT2_BLEEDING;
			opt_flag = 1;
			break;
		case SC_FEAR:
			sc->opt2 |= OPT2_FEAR;
			opt_flag = 1;
			break;
		// opt3
		case SC_ONEHAND:		/* 1HQ */
		case SC_TWOHANDQUICKEN:		/* 2HQ */
		case SC_SPEARQUICKEN:		/* +MLkw1DCiMK8wpDDDMLEw8w- */
		case SC_CONCENTRATION:		/* +MLMw8zC7MPMwyDDsMPwwtzDnMPM- */
		case SC_WEAPONQUICKEN:		/* +MKYwpzDdMPMwrzCkMMMwsTDz- */
			sc->opt3 |= OPT3_QUICKEN;
			opt_flag = 2;
			break;
		case SC_OVERTHRUST:		/* +MKow/DDQMPwwyDDpMLkwyA- */
		case SC_SWOO:			/* +MKgwuTCm- */
			sc->opt3 |= OPT3_OVERTHRUST;
			opt_flag = 2;
			break;
		case SC_ENERGYCOAT:		/* +MKgwyjC4MPwwszD8MMg- */
		case SC_SKE:			/* +MKgwuTCv- */
			sc->opt3 |= OPT3_ENERGYCOAT;
			opt_flag = 2;
			break;
		case SC_EXPLOSIONSPIRITS:	/* +cgaIwmziUtU- */
			sc->opt3 |= OPT3_EXPLOSIONSPIRITS;
			opt_flag = 2;
			break;
		case SC_STEELBODY:		/* +kdFSWw- */
		case SC_SKA:			/* +MKgwuTCr- */
			sc->opt3 |= OPT3_STEELBODY;
			opt_flag = 2;
			break;
		case SC_BLADESTOP:		/* +dn1SA1PWMIo- */
			sc->opt3 |= OPT3_BLADESTOP;
			opt_flag = 2;
			break;
		case SC_AURABLADE:			/* +MKow/DDpMNYw7DD8MMk- */
			sc->opt3 |= OPT3_AURABLADE;
			opt_flag = 2;
			break;
		case SC_BERSERK:		/* +MNAw/DC1MPwwrw- */
			sc->opt3 |= OPT3_BERSERK;
			opt_flag = 2;
			break;
		case SC_DANCING:			/* +MMAw8zC5-/+bxRZT04t- */
			if(sc->data[SC_DANCING].val1 != CG_MOONLIT)
				break;
			sc->opt3 |= OPT3_MOON;
			opt_flag = 2;
			break;
		case SC_MARIONETTE:		/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- */
		case SC_MARIONETTE2:		/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- */
			sc->opt3 |= OPT3_MARIONETTE;
			opt_flag = 2;
			break;
		case SC_ASSUMPTIO:		/* +MKIwuTDgMNcwxjCjMKo- */
		case SC_ASSUMPTIO2:		/* +MK0w4zDDMLcw5TCiMLkw4DDXMMYwozCq- */
			sc->opt3 |= OPT3_ASSUMPTIO;
			clif_misceffect2(bl,375);
			opt_flag = 2;
			break;
		case SC_WARM:			/* +bikwgjCK- */
			sc->opt3 |= OPT3_SUN_WARM;
			opt_flag = 2;
			break;
		case SC_KAITE:
			sc->opt3 |= OPT3_KAITE;
			opt_flag = 2;
			break;
		case SC_BUNSINJYUTSU:		/* +UgaOqzBuiFM- */
			sc->opt3 |= OPT3_BUNSIN;
			opt_flag = 2;
			break;
		case SC_MONK:			/* +MOIw8zCvMG6bQg- */
		case SC_STAR:			/* +MLEw8zC7MKQwbptC- */
		case SC_SAGE:			/* +MLsw/DC4MG6bQg- */
		case SC_CRUSADER:		/* +MK8w6zC7MKQwwDD8MG6bQg- */
		case SC_WIZARD:			/* +MKYwozC2MPwwyTBum0I- */
		case SC_PRIEST:			/* +MNcw6jD8MLkwyDBum0I- */
		case SC_ROGUE:			/* +MO0w/DCwMG6bQg- */
		case SC_ASSASIN:		/* +MKIwtTC3MPMwbptC- */
		case SC_SOULLINKER:		/* +ML0wpjDrMOow8zCrMPwwbptC- */
		case SC_KNIGHT:			/* +MMowpDDIMG6bQg- */
		case SC_ALCHEMIST:		/* +MKIw6zCxMN8wuTDIMG6bQg- */
		case SC_BARDDANCER:		/* +MNAw/DDJMGgwwDDzMLUw/DBum0I- */
		case SC_BLACKSMITH:		/* +MNYw6TDDMK8wuTDfMLkwbptC- */
		case SC_HUNTER:			/* +MM8w8zC/MPwwbptC- */
		case SC_HIGH:			/* +TgBrIU4KT02Ad2ltMG6bQg- */
		case SC_SUPERNOVICE:		/* +MLkw/DDRMPwwzjD8MNMwuTBum0I- */
		case SC_GUNNER:			/* +MKww8zC5MOow8zCsMPwwbptC- */
		case SC_NINJA:			/* +X82ABTBum0I- */
		case SC_DEATHKINGHT:		/* +MMcwuTDKMKQwyDBum0I- */
		case SC_COLLECTOR:		/* +MLMw7DCvML8w/DBum0I- */
			sc->opt3 |= OPT3_SOULLINK;
			clif_misceffect2(bl,424);
			opt_flag = 2;
			break;
		case SC_ELEMENTUNDEAD:		// +Tg1rew-
			sc->opt3 |= OPT3_UNDEAD;
			opt_flag = 2;
			break;
		// option
		case SC_SIGHT:
			sc->option |= OPTION_SIGHT;
			opt_flag = 1;
			break;
		case SC_HIDING:
			if(sd && val3 == 0)	// +lx5lrDCKMGcwajBEkBpeODBuMM8wpDDJMGowiTCiMKQwszDziGh5Og-
				clif_status_change(bl,SI_HIDING,1,icon_tick,0,0,0);
			unit_stopattack(bl);
			sc->option |= OPTION_HIDE;
			opt_flag = 1;
			break;
		case SC_CLOAKING:
		case SC_CLOAKINGEXCEED:		/* +MK8w7TD8MK0w8zCwMKgwrzC3MPwwyQ- */
		case SC_NEWMOON:			/* +ZxRnCIEa- */
		case SC__INVISIBILITY:		/* +MKQw8zDTMLgw0zDqMMYwow- */
			unit_stopattack(bl);
			sc->option |= OPTION_CLOAKING;
			opt_flag = 1;
			break;
		case SC_INVISIBLE:
		case SC__FEINTBOMB:			/* +MNUwpzCkMPMwyDDcMOA- */
			unit_stopattack(bl);
			sc->option |= OPTION_SPECIALHIDING;
			opt_flag = 1;
			break;
		case SC_REVERSEORCISH:
			sc->option |= OPTION_ORCFACE;
			opt_flag = 1;
			break;
		case SC_WEDDING:
			sc->option |= OPTION_MARRIED;
			opt_flag = 1;
			break;
		case SC_RUWACH:
			sc->option |= OPTION_RUWACH;
			opt_flag = 1;
			break;
		case SC_CHASEWALK:
			unit_stopattack(bl);
			sc->option |= (OPTION_CLOAKING | OPTION_FOOTPRINT);
			opt_flag = 1;
			break;
		case SC_FUSION:
			sc->option |= OPTION_STAR2;
			opt_flag = 1;
			break;
		case SC_SANTA:
			sc->option |= OPTION_SANTA;
			opt_flag = 1;
			break;
		case SC_SUMMER:
			sc->option |= OPTION_SUMMER;
			opt_flag = 1;
			break;
		case SC_ON_PUSH_CART:	/* +MKsw/DDI- */
#if PACKETVER < 20120201
			// +U+QwRDCvMOkwpDCiMPMwyDBvMKow1zC3MOcw8zCSZvRlsDBZMIs-
			sc->option = (sc->option & +AH4-OPTION_CARTMASK) | val1;
			opt_flag = 1;
#endif
			break;
	}

	/* option+MG5ZCWb0- */
	if(opt_flag == 1) {
		clif_changeoption(bl);
	} else if(opt_flag == 2) {
		clif_changeoption2(bl);
	}

#ifdef DYNAMIC_SC_DATA
	status_calloc_sc_data(sc);
#endif

	sc->count+-+-;	/* +MLkwxjD8ML8wuXVwXjgwbmVw- */

	sc->data[type].val1 = val1;
	sc->data[type].val2 = val2;
	sc->data[type].val3 = val3;
	sc->data[type].val4 = val4;
	/* +ML8wpDDeMPyKLVua- */
	sc->data[type].timer = add_timer(current_tick +- tick, status_change_timer, bl->id, INT2PTR(type));

	if(calc_flag) {
		// +MLkwxjD8ML8wuVGNigh7lw-
		if(sd) {
			if(!(flag&4))
				status_calc_pc(sd,0);
		} else if(hd) {
			homun_calc_status(hd);
			clif_send_homstatus(hd->msd,0);
		} else if(mcd) {
			merc_calc_status(mcd);
			clif_send_mercstatus(mcd->msd);
			clif_mercskillinfoblock(mcd->msd);
		} else if(eld) {
			elem_calc_status(eld);
			clif_send_elemstatus(eld->msd);
		}
	}
	// +igh7l1+MMGuNcDCJMFswiw-
	switch(type) {
		case SC_RUN:
		case SC_WUGDASH:
			// clif_skill_nodamage() +MG9fxTBa- clif_status_change() +MGg- clif_walkok() +MG6VkzBrVHwwc1H6MFk-
			if(type == SC_RUN)
				clif_skill_nodamage(bl,bl,TK_RUN,val1,1);
			else if(type == SC_WUGDASH)
				clif_skill_nodamage(bl,bl,RA_WUGDASH,val1,1);
			if(sd) {
				pc_runtodir(sd);
			}
			break;
		case SC_FORCEWALKING:
			unit_forcewalktodir(bl,val4);
			break;
	}

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjh9Qk6G-
 *------------------------------------------
 */
int status_change_end(struct block_list* bl, int type, int tid)
{
	struct map_session_data *sd  = NULL;
	struct mob_data         *md  = NULL;
	struct homun_data       *hd  = NULL;
	struct merc_data        *mcd = NULL;
	struct elem_data        *eld = NULL;
	struct status_change    *sc  = NULL;
	struct unit_data        *ud  = NULL;
	int opt_flag = 0, calc_flag = 0;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if(type < 0)
		return 0;
	ud = unit_bl2ud(bl);
	if(ud == NULL)
		return 0;

	if(type >= MAX_STATUSCHANGE) {
		switch(type) {
			case SC_RACECLEAR:
				status_change_race_end(bl,-1);
				break;
			case SC_RESISTCLEAR:
				status_change_resistclear(bl);
				break;
			case SC_SOUL:
			case SC_SOULCLEAR:
				status_change_soulclear(bl);
				break;
			default:
				if(battle_config.error_log)
					printf("UnknownStatusChangeEnd [%d]+AFw-n", type);
				break;
		}
		return 0;
	}

	if(sc->count <= 0 || sc->data[type].timer == -1)
		return 0;

	if(tid != -1 && sc->data[type].timer != tid)
		return 0;

	if(tid == -1)	/* +ML8wpDDeMEswiVR8MHAwjDBmMEQwajBEMGowiTC/MKQw3lJKlmQwkjBZMIs- */
		delete_timer(sc->data[type].timer,status_change_timer);

	/* +inJfUzBudXBeODCSa2NeODBrYjswWQ- */
	sc->data[type].timer = -1;
	sc->count--;

	sd  = BL_DOWNCAST( BL_PC,   bl );
	md  = BL_DOWNCAST( BL_MOB,  bl );
	hd  = BL_DOWNCAST( BL_HOM,  bl );
	mcd = BL_DOWNCAST( BL_MERC, bl );
	eld = BL_DOWNCAST( BL_ELEM, bl );

	switch(type) {	/* +dXBeODBuei6YXjBUMGgwblHmdAY- */
		case SC_PROVOKE:			/* +MNcw7TDcMMMwrw- */
		case SC_CONCENTRATE:			/* +lsZOLVKbVBFOCg- */
		case SC_BLESSING:			/* +MNYw7DDDMLcw8zCw- */
		case SC_ANGELUS:			/* +MKIw8zC8MOswuQ- */
		case SC_SIGNUMCRUCIS:			/* +MLcwsDDKMOAwrzDrMLcwuQ- */
		case SC_HIDING:
		case SC_CLOAKING:
		case SC_TWOHANDQUICKEN:			/* 2HQ */
		case SC_ONEHAND:			/* 1HQ */
		case SC_ADRENALINE:			/* +MKIwyTDsMMow6jDzMOkwwzC3MOU- */
		case SC_ENCPOISON:			/* +MKgw8zDBMOMw8zDIMN0wpDC6MPM- */
		case SC_IMPOSITIO:			/* +MKQw8zDdMLcwxjCjMKow3jDMMLk- */
		case SC_GLORIA:				/* +MLAw7TDqMKI- */
		case SC_LOUD:				/* +MOkwpjDJMNwwpDC5- */
		case SC_MINDBREAKER:			/* +MN4wpDDzMMkw1jDsMPwwqzD8- */
		case SC_PROVIDENCE:			/* +MNcw7TD0MKMwxzDzMLk- */
		case SC_SPEARQUICKEN:			/* +MLkw1DCiMK8wpDDDMLEw8w- */
		case SC_VOLCANO:
		case SC_DELUGE:
		case SC_VIOLENTGALE:
		case SC_ETERNALCHAOS:			/* +MKgwvzD8MMow6zCrMKowuQ- */
		case SC_DRUMBATTLE:			/* +YiZZKp8TMG6X/zBN- */
		case SC_NIBELUNGEN:			/* +MMsw/DDZMOsw8zCwMG5jB48q- */
		case SC_SIEGFRIED:			/* +Tg1re46rMG4wuDD8MK8w1TDqMPwwyQ- */
		case SC_EXPLOSIONSPIRITS:		/* +cgaIwmziUtU- */
		case SC_SPEEDPOTION0:			/* +WJeQHzDdMPwwtzDnMPM- */
		case SC_SPEEDPOTION1:
		case SC_SPEEDPOTION2:
		case SC_BLADESTOP_WAIT:
		case SC_CONCENTRATION:			/* +MLMw8zC7MPMwyDDsMPwwtzDnMPM- */
		case SC_TRUESIGHT:			/* +MMgwpTDrMPwwtTCkMMg- */
		case SC_SPIDERWEB:			/* +MLkw0TCkMMAw/DCmMKcwwzDW- */
		case SC_CARTBOOST:			/* +MKsw/DDIMNYw/DC5MMg- */
		case SC_INCATK:				/* ATK+TgpmBw- (+eV6RUnUo-) */
		case SC_INCMATK:			/* MATK+TgpmBw- (+eV552DBug0l1KA-) */
		case SC_WEDDING:			/* +MKYwpzDHMKMw8zCw- */
		case SC_SANTA:
		case SC_SUMMER:
		case SC_INCALLSTATUS:
		case SC_INCHIT:
		case SC_INCMHP2:
		case SC_INCMSP2:
		case SC_INCATK2:
		case SC_INCHIT2:
		case SC_INCFLEE2:
		case SC_OVERTHRUSTMAX:
		case SC_CHASEWALK:			/* +MMEwpzCkMLkwpjCpMPwwrw- */
		case SC_CHASEWALK_STR:
		case SC_BATTLEORDER:
		case SC_MEAL_INCSTR:	// +mN9Oi3Uo-
		case SC_MEAL_INCAGI:
		case SC_MEAL_INCVIT:
		case SC_MEAL_INCINT:
		case SC_MEAL_INCDEX:
		case SC_MEAL_INCLUK:
		case SC_MEAL_INCHIT:
		case SC_MEAL_INCFLEE:
		case SC_MEAL_INCFLEE2:
		case SC_MEAL_INCCRITICAL:
		case SC_MEAL_INCDEF:
		case SC_MEAL_INCMDEF:
		case SC_MEAL_INCATK:
		case SC_MEAL_INCMATK:
		case SC_MEAL_INCSTR2:	// +irKR0WWZdAZ1KA-
		case SC_MEAL_INCAGI2:
		case SC_MEAL_INCVIT2:
		case SC_MEAL_INCINT2:
		case SC_MEAL_INCDEX2:
		case SC_MEAL_INCLUK2:
		case SC_SPURT:
		case SC_SUN_COMFORT:			/* +WSqWfTBuW4lpfQ- */
		case SC_MOON_COMFORT:			/* +ZwgwbluJaX0- */
		case SC_STAR_COMFORT:			/* +Zh8wbluJaX0- */
		case SC_FUSION:				/* +WSqWfTBoZwgwaGYfMG6HjVQI- */
		case SC_ADRENALINE2:			/* +MNUw6zCiMMkw7DDKMOow8zDpMMMwtzDl- */
		case SC_RESISTWATER:
		case SC_RESISTGROUND:
		case SC_RESISTFIRE:
		case SC_RESISTWIND:
		case SC_RESISTPOISON:
		case SC_RESISTHOLY:
		case SC_RESISTDARK:
		case SC_RESISTTELEKINESIS:
		case SC_RESISTUNDEAD:
		case SC_RESISTALL:
		case SC_INVISIBLE:
		case SC_TIGEREYE:
		case SC_THE_MAGICIAN:
		case SC_STRENGTH:
		case SC_THE_DEVIL:
		case SC_THE_SUN:
		case SC_FLING:				/* +MNUw6TCkMPMwsA- */
		case SC_MADNESSCANCEL:			/* +MN4wwzDJMM0wuTCtMOMw8zC7MOkw/A- */
		case SC_ADJUSTMENT:			/* +MKIwuDDjMLkwyDDhMPMwyA- */
		case SC_INCREASING:			/* +MKQw8zCvMOow/DC4MPMwsDCiMK0w5TDpMLcw/A- */
		case SC_NEN:				/* +X/U- */
		case SC_AVOID:				/* +fcpgJVbekH8- */
		case SC_CHANGE:				/* +MOEw8zC/MOswwTCnMPMwuA- */
		case SC_DEFENCE:			/* +MMcwozDVMKcw8zC5- */
		case SC_BLOODLUST:			/* +MNYw6TDDMMkw6TC5MMg- */
		case SC_FLEET:				/* +MNUw6jD8MMgw4DD8MNY- */
		case SC_SPEED:				/* +MKow/DDQMPwwyTC5MNQw/DDJ- */
		case SC_STONESKIN:			/* +MLkwyDD8MPMwuTCtMPM- */
		case SC_ANTIMAGIC:			/* +MKIw8zDBMN4wuDDDMK8- */
		case SC_WEAPONQUICKEN:			/* +MKYwpzDdMPMwrzCkMMMwsTDz- */
		case SC_WE_FEMALE:			/* +MEIwajBfMGtcPTBPMFcwfjBZ- */
		case SC_GIANTGROWTH:		/* +MLgw4zCkMKIw8zDIMLAw7TD8MLk- */
		case SC_FIGHTINGSPIRIT:		/* +MNUwoTCkMMYwozDzMLAwuTDUMOowwzDI- */
		case SC_FEAR:				/* +YFBgFg- */
		case SC_UNLIMIT:			/* +MKIw8zDqMN8wwzDI- */
		case SC_EPICLESIS:			/* +MKgw1DCvMOwwtzC5- */
		case SC_LAUDAAGNUS:			/* +MOkwpjDAMKIwsDDMMLk- */
		case SC_LAUDARAMUS:			/* +MOkwpjDAMOkw4DC5- */
		case SC_TELEKINESIS_INTENSE:	/* +MMYw7DCtMM0wtzC5MKQw8zDGMPMwuQ- */
		case SC_VENOMIMPRESS:		/* +MNkwyjDgMKQw8zDXMOwwuQ- */
		case SC_CLOAKINGEXCEED:		/* +MK8w7TD8MK0w8zCwMKgwrzC3MPwwyQ- */
		case SC_VENOMBLEED:			/* +MNkwyjDgMNYw6jD8MMk- */
		case SC_INFRAREDSCAN:		/* +MKQw8zDVMOkw7DDDMMkwuTCtMOMw8w- */
		case SC_ANALYZE:			/* +MKIwyjDpMKQwug- */
		case SC_NEUTRALBARRIER:		/* +MMsw5TD8MMgw6TDrMNAw6jCiMPw- */
		case SC__BODYPAINT:			/* +MNwwxzCjMNowpDDzMMYwozDzMLA- */
		case SC__INVISIBILITY:		/* +MKQw8zDTMLgw0zDqMMYwow- */
		case SC__ENERVATION:		/* +MN4wuTCrMOww/DDJ/xowqDDKMNkw/DC3MOcw8w- */
		case SC__UNLUCKY:			/* +MN4wuTCrMOww/DDJ/xowojDzMOkwwzCtMPw- */
		case SC__STRIPACCESSARY:	/* +MLkwyDDqMMMw1zCiMK8wuzC1MOow/A- */
		case SC__BLOODYLUST:		/* +MNYw6TDDMMcwozDpMLkwyA- */
		case SC_SHIELDSPELL_DEF:	/* +MLcw/DDrMMkwuTDaMOs-(DEF) */
		case SC_SHIELDSPELL_MDEF:	/* +MLcw/DDrMMkwuTDaMOs-(MDEF) */
		case SC_SHIELDSPELL_REF:	/* +MLcw/DDrMMkwuTDaMOs-(+fL6TLA-) */
		case SC_PRESTIGE:			/* +MNcw7DC5MMYwozD8MLg- */
		case SC_EARTHDRIVE:			/* +MKIw/DC5MMkw6TCkMNY- */
		case SC_INSPIRATION:		/* +MKQw8zC5MNQw7DD8MLcw5zDz- */
		case SC_GENTLETOUCH_CHANGE:	/* +cLl6dA- -+U80-- */
		case SC_GENTLETOUCH_REVITALIZE:	/* +cLl6dA- -+bTs-- */
		case SC_SYMPHONY_LOVE:		/* +YEtOujBfMGEwbnC6MG4wtzDzMNUwqTDLMPw- */
		case SC_ECHOSONG:			/* +MKgwszD8MG5rTA- */
		case SC_HARMONIZE:			/* +MM8w/DDiMMowpDC6- */
		case SC_GLOOMYDAY:			/* +MOEw6TDzMLMw6jD8- */
		case SC_LERADS_DEW:			/* +MOww/DDpMLowbpcy- */
		case SC_DANCE_WITH_WUG:		/* +MMAw8zC5MKYwozC6MKYwqTD8MLA- */
		case SC_BEYOND_OF_WARCRY:	/* +MNMw6DDzMMkwqjDWMKYwqTD8MK8w6TCk- */
		case SC_MELODYOFSINK:		/* +MOEw7TDHMKMw/DCqMNYwtzDzMK8- */
		case SC_FRIGG_SONG:			/* +MNUw6jDDMLAwbmtM- */
		case SC_STRIKING:			/* +MLkwyDDpMKQwrTDzMLA- */
		case SC_FIRE_EXPANSION_SMOKE_POWDER:	/* +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+cVleVQ-) */
		case SC_FIRE_EXPANSION_TEAR_GAS:	/* +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmTCsMLk-) */
		case SC_MANDRAGORA:			/* +MM8wpjDqMPMwsDCqMNYw3jDzMMkw6TC0MOk- */
		case SC_BANANA_BOMB:		/* +MNAwyjDKcgZfPg- */
		case SC_MYSTERIOUS_POWDER:	/* +Tg1gHYtwMGp8iQ- */
		case SC_BOOST500:			/* +MNYw/DC5MMg-500 */
		case SC_FULL_SWING_K:		/* +MNUw6zC5MKQw8zCw-K */
		case SC_MANA_PLUS:			/* +MN4wyjDXMOkwuQ- */
		case SC_MUSTLE_M:			/* +MN4wwzC5MOs-M */
		case SC_LIFE_FORCE_F:		/* +MOkwpDDVMNUwqTD8MLk-F */
		case SC_PROMOTE_HEALTH_RESERCH:	/* HP+WJdSoDDdMPwwtzDnMPM- */
		case SC_ENERGY_DRINK_RESERCH:	/* SP+WJdSoDDdMPwwtzDnMPM- */
		case SC_EXTRACT_WHITE_POTION_Z:	/* +b8N+LjDbMO8wpDDIMN0w/DC3MOcw8w-Z */
		case SC_VITATA_500:			/* +MNMwvzC/-500 */
		case SC_EXTRACT_SALAMINE_JUICE:	/* +b8N+LjC1MOkw3jCkMPMwuDDlMPwwuQ- */
		case SC_SAVAGE_STEAK:		/* +MLUw2TD8MLgwbk44cTwwTQ- */
		case SC_COCKTAIL_WARG_BLOOD:	/* +MKswrzDGMOswpjCpMPwwsDDWMOkwwzDJ- */
		case SC_MINOR_BBQ:			/* +MN8wzjC/MKYw7TC5MG5yWzCrMOsw0w- */
		case SC_SIROMA_ICE_TEA:		/* +MLcw7TDeMKIwpDC5MMYwozD8- */
		case SC_DROCERA_HERB_STEAMED:	/* +MMkw7TC7MOkwbjDPMPww1nFu- */
		case SC_PUTTI_TAILS_NOODLES:	/* +MNcwxjCjMMMwyDBuMFcwYzB9nro- */
		case SC_STOMACHACHE:		/* +gXl12w- */
		case SC_MONSTER_TRANSFORM:	/* +MOIw8zC5ML8w/FkJjqs- */
		case SC_MEIKYOUSISUI:		/* +Zg6T4WtibDQ- */
		case SC_IZAYOI:				/* +U0FRbVkc- */
		case SC_KG_KAGEHUMI:		/* +XnuIUw- -+X3GODzB/-- */
		case SC_KYOMU:				/* +XnuIUw- -+hlpxITBuX3E-- */
		case SC_KAGEMUSYA:			/* +XnuIUw- -+UgaOqw-- */
		case SC_ZANGETSU:			/* +XnuIUw- -+a4tnCA-- */
		case SC_AKAITSUKI:			/* +XnuIUw- -+fQVnCA-- */
		case SC_KYOUGAKU:			/* +XnuIUw- -+mlphFQ-- */
		case SC_SHRIMP:				/* +MKgw004JZic- */
		case SC_GROOMING:			/* +MLAw6zD8MN8w8zCw- */
		case SC_NYANGGRASS:			/* +MMsw4zDzMLAw6TC5- */
		case SC_BURNT:				/* +c0RwjlRq- */
		case SC_ODINS_POWER:		/* +MKow/DDHMKMw8zBuUps- */
		case SC_MER_FLEE:			/* +UK1RdTDcMPwwyjC5-(FLEE) */
		case SC_MER_ATK:			/* +UK1RdTDcMPwwyjC5-(ATK) */
		case SC_MER_HP:				/* +UK1RdTDcMPwwyjC5-(HP) */
		case SC_MER_SP:				/* +UK1RdTDcMPwwyjC5-(SP) */
		case SC_MER_HIT:			/* +UK1RdTDcMPwwyjC5-(HIT) */
		case SC_FIRE_CLOAK:			/* +MNUwoTCkMKIw/DCvMO0w/DCv- */
		case SC_WATER_DROP:			/* +MKYwqTD8ML8w/DDJMO0wwzDX- */
		case SC_WIND_CURTAIN:		/* +MKYwozDzMMkwqzD8MMYw8w- */
		case SC_SOLID_SKIN:			/* +ML0w6jDDMMkwuTCtMPM- */
		case SC_STONE_SHIELD:		/* +MLkwyDD8MPMwtzD8MOswyQ- */
		case SC_PYROTECHNIC:		/* +MNEwpDDtMMYwrzDLMMMwrw- */
		case SC_HEATER:				/* +MNIw/DC/MPw- */
		case SC_TROPIC:				/* +MMgw7TDUMMMwrw- */
		case SC_AQUAPLAY:			/* +MKIwrzCiMNcw7DCk- */
		case SC_COOLER:				/* +MK8w/DDpMPw- */
		case SC_CHILLY_AIR:			/* +MK8w/DDrMKgwojD8- */
		case SC_GUST:				/* +MKwwuTDI- */
		case SC_BLAST:				/* +MNYw6TC5MMg- */
		case SC_WILD_STORM:			/* +MO8wpDDrMMkwuTDIMPww4A- */
		case SC_PETROLOGY:			/* +MNowyDDtMLgw/A- */
		case SC_CURSED_SOIL:		/* +MKsw/DC5MMkwvTCkMOs- */
		case SC_UPHEAVAL:			/* +MKIwwzDXMNgwpDDQMOs- */
		case SC_TIDAL_WEAPON_OPTION:	/* +ML8wpDDAMOswpjCnMN0w8w-(+fL6XCg-) */
		case SC_INVINCIBLE:			/* +MKQw8zDTMPMwtzDWMOs- */
		case SC_INVINCIBLEOFF:		/* +MKQw8zDTMPMwtzDWMOswqjDV- */
		case SC_ATKPOTION:
		case SC_MATKPOTION:
		case SC_ALMIGHTY:
		case SC_PARALYZE:			/* +nrt1+g- */
		case SC_PAIN_KILLER:		/* +MNowpDDzMK0w6TD8- */
		case SC_ANGRIFFS_MODUS:		/* +MKIw8zCwMOow1TC5MOIwyTC5- */
		case SC_GOLDENE_FERSE:		/* +MLQw/DDrMMcw8zDaMOswuDCn- */
		case SC_TINDER_BREAKER:		/* +Y1Vzcg- */
		case SC_CBC:				/* +fV4wgWKA- */
		case SC_EQC:				/* E.Q.C */
		case SC_ALL_STAT_DOWN:	/* +MKow/DDrMLkwxjD8ML8wuTDAMKYw8w- */
		case SC_LUNARSTANCE:		/* +ZwgwbmnLMEg- */
		case SC_UNIVERSESTANCE:		/* +W4dbmTBuacswSA- */
		case SC_SUNSTANCE:			/* +WSqWfTBuacswSA- */
		case SC_STARSTANCE:			/* +Zh8wbmnLMEg- */
		case SC_SOULSHADOW:		/* +X3EwbptC- */
		case SC_SOULFAIRY:		/* +WZZ8vjBum0I- */
		case SC_SOULFALCON:		/* +nfkwbptC- */
		case SC_SOULGOLEM:		/* +MLQw/DDsMOAwbptC- */
		case SC_HANDICAPSTATE_DEEPBLIND:		/* +bwae0g- */
		case SC_HANDICAPSTATE_DEEPSILENCE:		/* +l1lbwg- */
		case SC_HANDICAPSTATE_FROSTBITE:		/* +YCVRtw- */
		case SC_HANDICAPSTATE_CRYSTALLIZATION:	/* +fVBmdlMW- */
		case SC_HANDICAPSTATE_MISFORTUNE:		/* +Tg1eeA- */
		case SC_HANDICAPSTATE_DEADLYPOISON:		/* +Xzdr0g- */
		case SC_DEADLY_DEFEASANCE:		/* +MMcwwzDJMOow/DDXMO0wuDCnMK8wtzDnMPM- */
		case SC_CLIMAX_DES_HU:		/* +MK8w6TCkMN4wwzCvMLkwzzDqMLEw/DDz- */
		case SC_CLIMAX_CRYIMP:		/* +MK8w6TCkMN4wwzCvMLkwpDDzMNEwrzDI- */
		case SC_POWERFUL_FAITH:		/* +XzeXbTBqT+Ff9Q- */
		case SC_FIRM_FAITH:		/* +WAVW+jBqT+Ff9Q- */
		case SC_SINCERE_FAITH:		/* +X+BbnzBqT+Ff9Q- */
		case SC_ATTACK_STANCE:		/* +MKIwvzDDMK8wuTC/MPMwuQ- */
		case SC_HOLY_S:		/* +MNsw/DDqMPwwtzD8MOswyQ- */
		case SC_PRE_ACIES:		/* +MNcw7DC7MPMwuTCiMK0wqDD8MLk- */
		case SC_COMPETENTIA:	/* +MLMw8zDaMMYw8zDGMKMwog- */
		case SC_RELIGIO:		/* +MOww6jCuMKo- */
		case SC_BENEDICTUM:		/* +MNkwzTDHMKMwrzDIMKUw4A- */
		case SC_RUSH_QUAKE2:		/* +MOkwwzC3MOU- */
		case SC_D_MACHINE:		/* +ljJfoYjFf25nCVK5UxY- */
		case SC_SHADOW_STRIP:	/* +MLkwyDDqMMMw1zC3MOMwyTCm- */
		case SC_ABYSS_SLAYER:	/* +MKIw0zC5MLkw7DCkMOQw/A- */
		case SC_BO_HELL_DUSTY:	/* +MNgw6zDAMLkwxjCj- */
		case SC_MYSTIC_SYMPHONY:	/* +MN8wuTDGMKMwwzCvMLcw8zDVMKkwyzD8- */
		case SC_GEF_NOCTURN:		/* +MLIw1TCnMMswojDOMK8wvzD8MPM- */
		case SC_AIN_RHAPSODY:		/* +knFU4TBuMOkw1zC9MMcwow- */
		case SC_MUSICAL_INTERLUDE:	/* +MN8w5TD8MLgwqzDrMKQw8zC/MPww6zD8MMk- */
		case SC_SPELL_ENCHANTING:	/* +MLkw2jDrMKgw8zDBMOMw8zDGMKMw8zCw- */
		case SC_FLAMETECHNIC:		/* +MNUw7DCkMOAwxjCvMMswwzCv- */
		case SC_COLD_FORCE:			/* +MLMw/DDrMMkw1TCpMPwwuQ- */
		case SC_GRACE_BREEZE:		/* +MLAw7DCkMLkw1jDqMPwwug- */
		case SC_EARTH_CARE:			/* +MKIw/DC5MLEwog- */
		case SC_DEEP_POISONING:		/* +MMcwozD8MNcw3TCkMLowyzDzMLA- */
		case SC_FLAMEARMOR:			/* +MNUw7DCkMOAwojD8MN4w/A- */
		case SC_CRYSTAL_ARMOR:		/* +MK8w6jC5ML8w6zCiMPww3jD8- */
		case SC_EYES_OF_STORM:		/* +MKIwpDC6MKow1jC5MMgw/DDg- */
		case SC_STRONG_PROTECTION:	/* +MLkwyDDtMPMwsDDXMO0wxjCvMLcw5zDz- */
		case SC_POISON_SHIELD:		/* +MN0wpDC6MPMwtzD8MOswyQ- */
		case SC_HIDDEN_CARD:		/* +MNIwyTClMPMwqzD8MMk- */
		case SC_TALISMAN_OF_WARRIOR:	/* +a2ZY63sm- */
		case SC_TALISMAN_OF_MAGICIAN:	/* +bNVeK3sm- */
		case SC_TALISMAN_OF_FIVE_ELEMENTS:	/* +TpSITHsm- */
		case SC_T_FIVETH_GOD:		/* +VttluU6UiEyWYw- */
		case SC_HEAVEN_AND_EARTH:	/* +WSlXMHlelwo- */
		case SC_MARINE_FESTIVAL:	/* +MN4w6jDzMNUwpzC5MMYwozDQMOs- */
		case SC_SANDY_FESTIVAL:		/* +MLUw8zDJMNUwpzC5MMYwozDQMOs- */
		case SC_TEMPORARY_COMMUNION:	/* +TgmXCk4AT1M- */
			calc_flag = 1;
			break;
		case SC_NEWMOON:			/* +ZxRnCIEa- */
		case SC_SPEEDUP0:			/* +eftS1ZAfXqZYl1Kg-(+MKIwpDDGMOA-) */
		case SC_SPEEDUP1:			/* +MLkw1DD8MMkw3TD8MLcw5zDz- */
		case SC_WALKSPEED:			/* +eftS1ZAfXqZYl1Kg-(+MLkwrzDqMNcwyA-) */
		case SC_SLOWPOTION:			/* +eftS1ZAfXqZPTk4L-(+MKIwpDDGMOA-) */
		case SC_STEELBODY:			/* +kdFSWw- */
		case SC_INCREASEAGI:			/* +kB9epk4KZgc- */
		case SC_WINDWALK:			/* +MKYwpDDzMMkwpjCpMPwwrw- */
		case SC_INCFLEE:
		case SC_DECREASEAGI:			/* +kB9epm4bXBE- */
		case SC_QUAGMIRE:			/* +MK8woTCwMN4wpDCi- */
		case SC_MARSHOFABYSS:		/* +MN4w/DC3MOUwqjDWMKIw0zC5- */
		case SC_DEFENDER:			/* +MMcwozDVMKcw8zDAMPw- */
		case SC_GRAVITATION:
		case SC_SUITON:				/* +bDSQQQ- */
		case SC_GATLINGFEVER:			/* +MKwwyDDqMPMwsDDVMKMw/DDQMPw- */
		case SC_HALLUCINATIONWALK_POSTDELAY:	/* +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv-(+MNowyjDrMMYwow-) */
		case SC_PARALIZE:			/* +MNEw6TDpMKQwug- */
		case SC_FROSTMISTY:			/* +MNUw7TC5MMgw3zC5MMYwow- */
		case SC_CAMOUFLAGE:			/* +MKsw4jDVMOkw/DC4MOU- */
		case SC__GROOMY:			/* +MN4wuTCrMOww/DDJ/xowsDDrMPww3zD8- */
		case SC__LAZINESS:			/* +MN4wuTCrMOww/DDJ/xow7DCkMLgwzTC5- */
		case SC_SWING:				/* +MLkwpDDzMLAwwDDzMLk- */
		case SC_GN_CARTBOOST:		/* +MKsw/DDIMNYw/DC5MMg- */
		case SC_MELON_BOMB:			/* +MOEw7TDzcgZfPg- */
		case SC_HISS:				/* +i2ZiEg- */
		case SC_ARCLOUSEDASH:		/* +MKIwrzDpMKYwuTDAMMMwtzDl- */
		case SC_CHATTERING:			/* +MMEw4zC/MOow8zCw- */
		case SC_WIND_STEP:			/* +MKYwozDzMMkwuTDGMMMw1w- */
		case SC_REBOUND:			/* +MOow0DCmMPMwyQ- */
		case SC_HANDICAPSTATE_LASSITUDE:		/* +cSFsF1Kb- */
		case SC_JAWAII_SERENADE:	/* +WRVxPDBRMG4wuzDsMMow/DDH- */
		case SC_PRON_MARCH:			/* +MNcw7TDzMMYw6TDeMPwwwQ- */
		case SC_SHIELDCHAINRUSH:	/* +MLcw/DDrMMkwwTCnMPww8zDpMMMwtzDl- */
		case SC_GROUNDGRAVITY:		/* +MLAw6TCmMPMwyTCwMOkw0zDGMPwwtzDnMPM- */
		case SC_SHADOW_CLOCK:		/* +X3GWoDCM- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_ALL_RIDING:			/* +mg5OVzC3MLkwxjDg- */
			if(sd)
				clif_status_load_id(sd,SI_RIDING,0);
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_ON_PUSH_CART:		/* +MKsw/DDI- */
			calc_flag = 1;
			ud->state.change_speed = 1;
			if(sc->data[SC_CARTBOOST].timer != -1)
				status_change_end(bl, SC_CARTBOOST, -1);
			if(sc->data[SC_GN_CARTBOOST].timer != -1)
				status_change_end(bl, SC_GN_CARTBOOST, -1);
			if(sd)
				clif_cart_clear(sd);
			break;
		case SC_ELEMENTWATER:		// +bDQ-
		case SC_ELEMENTGROUND:		// +Vx8-
		case SC_ELEMENTFIRE:		// +cGs-
		case SC_ELEMENTWIND:		// +mKg-
		case SC_ELEMENTHOLY:		// +UUk-
		case SC_ELEMENTDARK:		// +lcc-
		case SC_ELEMENTELEKINESIS:	// +X/U-
		case SC_ELEMENTPOISON:		// +a9I-
		//case SC_ELEMENTUNDEAD:	// +Tg1rew-
			if(sd)
				clif_displaymessage(sd->fd, msg_txt(203));	// +ljJRdzBuXF5gJzBMUUMwa2I7MIowfjBXMF8-
			break;
		case SC_RACEUNKNOWN:
		case SC_RACEUNDEAD:
		case SC_RACEBEAST:
		case SC_RACEPLANT:
		case SC_RACEINSECT:
		case SC_RACEFISH:
		case SC_RACEDEVIL:
		case SC_RACEHUMAN:
		case SC_RACEANGEL:
		case SC_RACEDRAGON:
			if(sd)
				clif_displaymessage(sd->fd, msg_txt(205));	// +ei5lzzBMUUMwa2I7MIowfjBXMF8-
			break;
		case SC_RUN:			/* +ML8wpDDqMK4- */
		case SC_WUGDASH:		/* +MKYwqTD8MLAwwDDDMLcw5Q- */
			unit_stop_walking(bl,0);
			calc_flag = 1;
			break;
		case SC_MONK:			/* +MOIw8zCvMG6bQg- */
		case SC_STAR:			/* +MLEw8zC7MKQwbptC- */
		case SC_SAGE:			/* +MLsw/DC4MG6bQg- */
		case SC_CRUSADER:		/* +MK8w6zC7MKQwwDD8MG6bQg- */
		case SC_WIZARD:			/* +MKYwozC2MPwwyTBum0I- */
		case SC_PRIEST:			/* +MNcw6jD8MLkwyDBum0I- */
		case SC_ROGUE:			/* +MO0w/DCwMG6bQg- */
		case SC_ASSASIN:		/* +MKIwtTC3MPMwbptC- */
		case SC_SOULLINKER:		/* +ML0wpjDrMOow8zCrMPwwbptC- */
		case SC_SUPERNOVICE:		/* +MLkw/DDRMPwwzjD8MNMwuTBum0I- */
		case SC_GUNNER:			/* +MKww8zC5MOow8zCsMPwwbptC- */
		case SC_NINJA:			/* +X82ABTBum0I- */
		case SC_DEATHKINGHT:		/* +MMcwuTDKMKQwyDBum0I- */
		case SC_COLLECTOR:		/* +MLMw7DCvML8w/DBum0I- */
			if(sd && battle_config.disp_job_soul_state_change)
				clif_disp_onlyself(sd->fd, msg_txt(207));	// +m0JytmFLMEx9Qk6GMFcwfjBXMF8-
			break;
		case SC_KNIGHT:			/* +MMowpDDIMG6bQg- */
		case SC_ALCHEMIST:		/* +MKIw6zCxMN8wuTDIMG6bQg- */
		case SC_BARDDANCER:		/* +MNAw/DDJMGgwwDDzMLUw/DBum0I- */
		case SC_BLACKSMITH:		/* +MNYw6TDDMK8wuTDfMLkwbptC- */
		case SC_HUNTER:			/* +MM8w8zC/MPwwbptC- */
		case SC_HIGH:			/* +TgBrIU4KT02Ad2ltMG6bQg- */
			if(sd && battle_config.disp_job_soul_state_change)
				clif_disp_onlyself(sd->fd, msg_txt(207));	// +m0JytmFLMEx9Qk6GMFcwfjBXMF8-
			calc_flag = 1;
			break;
		case SC_POEMBRAGI:			/* +MNYw6TCu- */
		case SC_WHISTLE:			/* +U+N7Gw- */
		case SC_ASSNCROS:			/* +WRWWfTBuMKIwtTC3MPMwrzDtMLk- */
		case SC_APPLEIDUN:			/* +MKQwyTClMPMwbmeXao4- */
		case SC_HUMMING:			/* +MM8w3zDzMLA- */
		case SC_DONTFORGETME:			/* +ecEwkl/YMIwwajBEMGc- */
		case SC_FORTUNE:			/* +XniQSzBuMK0wuQ- */
		case SC_SERVICE4U:			/* +MLUw/DDTMLkw1TCpMPww5jD8- */
			calc_flag = 1;
			if(type == SC_DONTFORGETME)
				ud->state.change_speed = 1;
			// +jgowim8UWU9jAX2aMLswwzDI-
			if(sc->data[type +- SC_WHISTLE_ - SC_WHISTLE].timer == -1)
				status_change_start(bl,type +- SC_WHISTLE_ - SC_WHISTLE,sc->data[type].val1,
					sc->data[type].val2,sc->data[type].val3,sc->data[type].val4,battle_config.dance_and_play_duration,0);
			break;
		case SC_WHISTLE_:			/* +U+N7Gw- */
		case SC_ASSNCROS_:			/* +WRWWfTBuMKIwtTC3MPMwrzDtMLk- */
		case SC_APPLEIDUN_:			/* +MKQwyTClMPMwbmeXao4- */
		case SC_HUMMING_:			/* +MM8w3zDzMLA- */
		case SC_DONTFORGETME_:			/* +ecEwkl/YMIwwajBEMGc- */
		case SC_FORTUNE_:			/* +XniQSzBuMK0wuQ- */
		case SC_SERVICE4U_:			/* +MLUw/DDTMLkw1TCpMPww5jD8- */
			calc_flag = 1;
			break;
		case SC_MARIONETTE:			/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- (+gepSBg-) */
		case SC_MARIONETTE2:			/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- (+dvhiSw-) */
			{
				struct block_list *tbl = map_id2bl(sc->data[type].val2);
				if(tbl) {
					struct status_change *tsc = status_get_sc(tbl);
					int tmp = (type == SC_MARIONETTE)? SC_MARIONETTE2: SC_MARIONETTE;
					// +dvhluTBMMN4w6jCqMM0wwzDIcrZhSzBqMIkwRDBjMFcwhzBrieOWZA-
					if(tsc && tsc->data[tmp].timer != -1)
						status_change_end(tbl, tmp, -1);
				}
			}
			calc_flag = 1;
			break;
		case SC_BERSERK:			/* +MNAw/DC1MPwwrw- */
			calc_flag = 1;
			if(sd) {
				clif_status_load_id(sd,SI_INCREASEAGI,0);	// +MKIwpDCzMPNtiFO7-
				status_change_start(bl,SC_NATURAL_HEAL_STOP,0,0,0,0,skill_get_time2(LK_BERSERK,sc->data[type].val1),0);
			}
			break;
		case SC_HALLUCINATION:
			if(sd)
				clif_status_load_id(sd,SI_HALLUCINATION,0);	// +MKIwpDCzMPNtiFO7-
			break;

		case SC_ENDURE:				/* +MKQw8zDHMOUwog- */
			calc_flag = 1;
			// fall through
		case SC_AUTOGUARD:			/* +MKow/DDIMKww/DDJ- */
		case SC_REFLECTSHIELD:			/* +MOow1TDsMK8wyDC3MPww6zDJ- */
			if(sd) {
				struct map_session_data *tsd;
				int i;
				// +iKswxzCjMNww/DC3MOcw84AFMIKJ45ZkMFkwiw-
				for(i = 0; i < 5; i+-+-) {
					if(sd->dev.val1[i] && (tsd = map_id2sd(sd->dev.val1[i])) != NULL && tsd->sc.data[type].timer != -1)
						status_change_end(&tsd->bl, type, -1);
				}
			}
			break;

		case SC_DEVOTION:		/* +MMcwozDcMPwwtzDnMPM- */
			{
				struct map_session_data *dsd = map_id2sd(sc->data[type].val1);

				sc->data[type].val1 = 0;
				sc->data[type].val2 = 0;

				if(sc->data[SC_AUTOGUARD].timer != -1)
					status_change_end(bl, SC_AUTOGUARD, -1);
				if(sc->data[SC_DEFENDER].timer != -1)
					status_change_end(bl, SC_DEFENDER, -1);
				if(sc->data[SC_REFLECTSHIELD].timer != -1)
					status_change_end(bl, SC_REFLECTSHIELD, -1);
				if(sc->data[SC_ENDURE].timer != -1)
					status_change_end(bl, SC_ENDURE, -1);
				if(dsd)
					skill_devotion(dsd);
			}
			break;
		case SC_BLADESTOP:
			{
				struct block_list *tbl = map_id2bl(sc->data[type].val4);
				if(tbl) {
					struct status_change *tsc = status_get_sc(tbl);
					// +ckdluTBMUgcwjDBfMG4wZ3b4YkswbnZ9UgNytmFLMExSBzCMMGYwajBEMG4wajCJieOWZA-
					if(tsc && tsc->data[SC_BLADESTOP].timer != -1)
						status_change_end(tbl,SC_BLADESTOP,-1);
					if(sc->data[type].val2 == 2)
						clif_bladestop(map_id2bl(sc->data[type].val3),tbl->id,0);
				}
			}
			break;
		case SC_CLOSECONFINE:
		case SC_STOP:
			{
				struct block_list *tbl = map_id2bl(sc->data[type].val4);
				if(tbl) {
					struct status_change *tsc = status_get_sc(tbl);
					// +ckdluTBMUgcwjDBfMG4wZ3b4YkswTFIHMIwwZjBqMEQwbjBqMImJ45Zk-
					if(tsc && tsc->data[type].timer != -1)
						status_change_end(tbl,type,-1);
				}
				calc_flag = 1;
			}
			break;
		case SC_DANCING:
			{
				struct map_session_data *dsd;
				// +ZwhmDjCKMGAwUTBTMFMwZzCiMKQwszDzbYhTuw-
				if(sc->data[type].val1 == CG_MOONLIT)
					clif_status_change(bl,SI_MOONLIT,0,0,0,0,0);	// +MKIwpDCzMPNtiFO7-

				if(sc->data[type].val4 && (dsd = map_id2sd(sc->data[type].val4))) {
					// +VAhZTzBndvhiSzBMMEQwi1g0VAh2+GJLMG4-val4+MJI-0+MGswWTCL-
					if(dsd->sc.data[type].timer != -1)
						dsd->sc.data[type].val4 = 0;
				}
				if(sc->data[SC_LONGINGFREEDOM].timer != -1)
					status_change_end(bl,SC_LONGINGFREEDOM,-1);
			}
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_GOSPEL:
		case SC_GRAFFITI:
		case SC_WARM:
		case SC_GRAVITATION_USER:
		case SC_DANCING_KNIFE:
			{
				struct skill_unit_group *sg = map_id2sg(sc->data[type].val4);	// val4+MEw-group_id
				sc->data[type].val4 = 0;
				if(sg)
					skill_delunitgroup(sg);
			}
			break;
		case SC_NOCHAT:			/* +MMEw4zDDMMh5gWticrZhSw- */
			if(sd)
				clif_updatestatus(sd,SP_MANNER);
			break;
		case SC_SPLASHER:		/* +MNkwyjDgMLkw1zDpMMMwtzDjMPw- */
			{
				struct block_list *src = map_id2bl(sc->data[type].val3);
				if(src && tid != -1) {
					// +gepSBjBrMMAw4TD8MLj/BlRoVvI-3*3+MGswwDDhMPwwuA-
					skill_castend_damage_id(src,bl,sc->data[type].val2,sc->data[type].val1,gettick(),0);
				}
			}
			break;
		case SC_ANKLE:
		case SC_THORNS_TRAP:	/* +ML0w/DDzMMgw6TDDMNc- */
			{
				struct skill_unit_group *sg = map_id2sg(sc->data[type].val2);
				// skill_delunitgroup+MEswiQ-status_change_end +MExUfDBwMIwwajBEcLowazAB-
				// +TgB673Z6UtUwVzBmMEQwajBEToswazBXMGYwSzCJMLAw6zD8MNdSSpZkMFkwizAC-
				if(sg) {
					sg->val2 = 0;
					skill_delunitgroup(sg);
				}
			}
			break;
		case SC_SELFDESTRUCTION:	/* +gepyBg- */
			unit_stop_walking(bl,5);
			if(md) {
				md->mode &= +AH4-MD_CANMOVE;
				md->state.special_mob_ai = 2;
			}
			break;
		case SC_BUNSINJYUTSU:	/* +UgaOqzBuiFM- */
			if(sd) {
				int color = sc->data[SC_BUNSINJYUTSU].val4;
				if(color > 0)
					pc_changelook(sd, LOOK_CLOTHES_COLOR, color);
			}
			break;
		case SC_SEVENWIND:	/* +ZpYwSzBEmKg- */
			if(sd)
				clif_status_load_id(sd,sc->data[type].val2,0);
			break;
		case SC_AUTOBERSERK:
			if(sc->data[SC_PROVOKE].timer != -1 && sc->data[SC_PROVOKE].val2 == 1) {
				status_change_end(bl,SC_PROVOKE,-1);
			}
			break;
		case SC_HALLUCINATIONWALK:	/* +MM8w6zC3MM0w/DC3MOcw8zCmMKkw/DCv- */
			// +MNowyjDrMMYwo5WLWcs-
			status_change_start(bl,SC_HALLUCINATIONWALK_POSTDELAY,0,0,0,0,skill_get_time2(GC_HALLUCINATIONWALK,sc->data[type].val1),0);
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_MILLENNIUMSHIELD:	/* +MN8w7DDLMKIw4DC3MPww6zDJ- */
			if(sd)
				clif_mshield(sd, 0);
			break;
		case SC_WHITEIMPRISON:	/* +MNsw7zCkMMgwpDDzMNcw6jC6MPM- */
			{
				int dmg = 400 * sc->data[type].val1;
				clif_damage(bl,bl,gettick(),0,0,dmg,0,9,0,0);
				battle_damage(bl,bl,dmg,0,0,0);
			}
			break;
		case SC_SPELLBOOK:			/* +MLkw2jDrMNYwwzCv- */
			if(sd) {
				// +T91bWDC5MK0w62DFWDEwklIdZx9TFg-
				memset(sd->freeze_sp_skill, 0, sizeof(sd->freeze_sp_skill[0])*MAX_FREEZE_SPELL);
				sd->freeze_sp_slot = 0;		// +MLkw7TDDMMhlcDCSUh1nH1MW-
			}
			break;
		case SC_NEUTRALBARRIER_MASTER:	/* +MMsw5TD8MMgw6TDrMNAw6jCiMPw-(+T391KIAF-) */
		case SC_STEALTHFIELD_MASTER:	/* +MLkwxjDrMLkw1TCjMPww6zDJ-(+T391KIAF-) */
			{
				struct skill_unit_group *sg = map_id2sg(sc->data[type].val4);	// val4+MEw-group_id
				sc->data[type].val4 = 0;
				if(sg)
					skill_delunitgroup(sg);
				calc_flag = 1;
				ud->state.change_speed = 1;
			}
			break;
		case SC__SHADOWFORM:		/* +MLcw4zDJMKYw1TCpMPww4A- */
			{
				struct map_session_data *dsd;
				if((dsd = map_id2sd(sc->data[type].val2)) != NULL)
					dsd->shadowform_id = 0;
			}
			break;
		case SC_FORCEOFVANGUARD:	/* +MNUwqTD8MLkwqjDWMNAw8zCsMPwwyQ- */
			if(sd)
				clif_mshield(sd, 0);
			calc_flag = 1;
			break;
		case SC_BANDING:		/* +MNAw8zDHMKMw8zCw- */
			{
				struct skill_unit_group *sg = map_id2sg(sc->data[type].val4);
				sc->data[type].val4 = 0;
				if(sg)
					skill_delunitgroup(sg);
				calc_flag = 1;
			}
			break;
		case SC_RAISINGDRAGON:		/* +b1yfjWYHWSk- */
			if(sd) {
				int max = pc_checkskill(sd,MO_CALLSPIRITS);
				if(sd->spiritball.num > max)
					pc_delspiritball(sd,sd->spiritball.num-max,0);
			}
			calc_flag = 1;
			break;
		case SC_SATURDAY_NIGHT_FEVER:	/* +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw- */
			status_change_start(bl,SC_SITDOWN_FORCE,0,0,0,0,3000,0);
			calc_flag = 1;
			break;
		case SC_SPORE_EXPLOSION:	/* +MLkw3TCiMKgwrzC5MNcw7TD8MLgw5zDz- */
			{
				struct block_list *src = map_id2bl(sc->data[type].val3);
				if(src && tid != -1) {
					skill_castend_damage_id(src,bl,sc->data[type].val2,sc->data[type].val1,gettick(),0);
				}
			}
			break;
		case SC_SUMMON_ELEM:	/* +MLUw4jDzMKgw7DDhMPMwvzDr- */
			if(sd && sd->eld)
				elem_delete_data(sd);
			break;
		case SC_OVERED_BOOST:		/* +MKow/DDQMPwwyTDWMPwwuTDI- */
			if(sd) {
				int sp = status_get_max_sp(bl) / 2;
				if(sd->status.sp > sp)
					sd->status.sp -= sp;
				else
					sd->status.sp = 0;
				clif_updatestatus(sd,SP_SP);
			}
			else if(hd) {
				hd->status.hungry -= 50;
				if(hd->status.hungry <= 0) {
					hd->status.hungry = 1;	// 0+MGswbzBqMIkwajBE-
				}
			}
			calc_flag = 1;
			break;
		case SC_GRANITIC_ARMOR:		/* +MLAw6TDLMMYwozDDMK8wojD8MN4w/A- */
			{
				int dmg = (int)((atn_bignumber)status_get_max_hp(bl) * sc->data[type].val3 / 100);
				unit_heal(bl, -dmg, 0, 0, 0);
			}
			break;
		case SC_PYROCLASTIC:		/* +MNEwpDDtMK8w6TC5MMYwozDDMK8- */
			if(sd)
				pc_break_equip(sd, LOC_RARM);
			calc_flag = 1;
			break;
		case SC_FULL_THROTTLE:		/* +MNUw6zC5MO0wwzDIMOs- */
			status_change_start(bl,SC_REBOUND,0,0,0,0,10000,0);
			calc_flag = 1;
			ud->state.change_speed = 1;
			break;
		case SC_SERVANTWEAPON:		/* +MLUw/DD0MKEw8zDIMKYwpzDdMPM- */
			if(sd)
				pc_delball(sd,sd->ball.num,0);
			break;
		case SC_CHARGINGPIERCE:		/* +MMEw4zD8MLgw8zCwMLkw1DCi- */
			if(sc->data[SC_CHARGINGPIERCE_COUNT].timer != -1)
				status_change_end(bl,SC_CHARGINGPIERCE_COUNT,-1);
			break;
		case SC_GUARD_STANCE:		/* +MKww/DDJMLkwvzDzMLk- */
			if(sc->data[SC_GUARDIAN_S].timer != -1)
				status_change_end(bl,SC_GUARDIAN_S,-1);
			if(sc->data[SC_REBOUND_S].timer != -1)
				status_change_end(bl,SC_REBOUND_S,-1);
			calc_flag = 1;
			break;
		case SC_ABYSSFORCEWEAPON:		/* +MNUw7TDgMLgwojDTMLk- */
			if(sd)
				pc_delball(sd,sd->ball.num,0);
			break;
		case SC_ROSEBLOSSOM:			/* +MO0wvDDWMO0wwzC1MOA- */
			{
				struct block_list *src = map_id2bl(sc->data[type].val2);
				if(src && tid != -1) {
					skill_castend_damage_id(src,bl,TR_ROSEBLOSSOM_ATK,sc->data[type].val1,gettick(),0);
				}
			}
			break;
		case SC_SUMMON_ELEMENTAL_ARDOR:		/* +MLUw4jDzMKIw6zDJMPww6w- */
			if(sd && sd->eld && sd->eld->status.class_ == ELEMID_EM_ARDOR)
				elem_delete_data(sd);
			calc_flag = 1;
			break;
		case SC_SUMMON_ELEMENTAL_DILUVIO:		/* +MLUw4jDzMMcwozDrMNMwqg- */
			if(sd && sd->eld && sd->eld->status.class_ == ELEMID_EM_DILUVIO)
				elem_delete_data(sd);
			calc_flag = 1;
			break;
		case SC_SUMMON_ELEMENTAL_PROCELLA:		/* +MLUw4jDzMNcw7TC7MOk- */
			if(sd && sd->eld && sd->eld->status.class_ == ELEMID_EM_PROCELLA)
				elem_delete_data(sd);
			calc_flag = 1;
			break;
		case SC_SUMMON_ELEMENTAL_TERREMOTUS:	/* +MLUw4jDzMMYw7DDiMMgwpTC5- */
			if(sd && sd->eld && sd->eld->status.class_ == ELEMID_EM_TERREMOTUS)
				elem_delete_data(sd);
			calc_flag = 1;
			break;
		case SC_SUMMON_ELEMENTAL_SERPENS:		/* +MLUw4jDzMLUw/DDaMPMwuQ- */
			if(sd && sd->eld && sd->eld->status.class_ == ELEMID_EM_SERPENS)
				elem_delete_data(sd);
			calc_flag = 1;
			break;
		case SC_INTENSIVE_AIM:			/* +MKQw8zDGMPMwtzDWMKgwpDDg- */
			if(sc->data[SC_INTENSIVE_AIM_COUNT].timer != -1)
				status_change_end(bl,SC_INTENSIVE_AIM_COUNT,-1);
			calc_flag = 1;
			break;
		case SC_BLESSING_OF_M_CREATURES:	/* +MGswgzCTMNYw7DDDMLcw8zCw- */
			unit_heal(bl,0,0,-200,0);
			status_change_start(bl,SC_BLESSING_OF_M_C_DEBUFF,sc->data[type].val1,0,0,0,skill_get_time2(SH_BLESSING_OF_MYSTICAL_CREATURES,sc->data[type].val1),0);
			calc_flag = 1;
			break;
		/* option1 */
		case SC_FREEZE:
			sc->data[type].val3 = 0;
			break;

		/* option2 */
		case SC_POISON:				/* +a9I- */
		case SC_BLIND:				/* +Zpee0g- */
		case SC_CURSE:
			calc_flag = 1;
			break;
		case SC_GRAVITYCONTROL:	/* +kc1Sm4q/e8A- */
			{
				int fall_damage = sc->data[type].val2;
				if( fall_damage > 0 ){
					clif_damage(bl,bl,gettick(),0,0,fall_damage,0,9,0,0);
					battle_damage(bl,bl,fall_damage,0,0,0);
				}
			}
			break;
		case SC_DIMENSION2:	/* +ayFRQzBuZvg-(+Zh9eHZZNgeg-) */
			if(sd)
				clif_mshield(sd, 0);
			break;
	}

	if(StatusIconChangeTable[type] != SI_BLANK)	// +MKIwpDCzMPNtiFO7-
		clif_status_load(bl,StatusIconChangeTable[type],0);

	switch(type) {	/* +a2NeODBrYjswizBoME0wajBrMEtR5nQGMExfxYmB- */
		// opt1
		case SC_STONE:
		case SC_FREEZE:
		case SC_STUN:
		case SC_SLEEP:
		case SC_HELLINFERNO:
		case SC_WHITEIMPRISON:
			sc->opt1 = OPT1_NORMAL;
			opt_flag = 1;
			break;
		// opt2
		case SC_POISON:
			sc->opt2 &= +AH4-OPT2_POISON;
			opt_flag = 1;
			break;
		case SC_CURSE:
		case SC_SILENCE:
		case SC_CONFUSION:
			sc->opt2 &= +AH4-(OPT2_POISON<<(type-SC_POISON));
			opt_flag = 1;
			if(type == SC_CURSE)
				ud->state.change_speed = 1;
			break;
		case SC_FOGWALLPENALTY:
			if(sc->data[SC_BLIND].timer == -1) {
				sc->opt2 &= +AH4-OPT2_BLIND;
				opt_flag = 1;
			}
			break;
		case SC_BLIND:
			if(sc->data[SC_FOGWALLPENALTY].timer == -1) {
				sc->opt2 &= +AH4-OPT2_BLIND;
				opt_flag = 1;
			}
			break;
		case SC_ANGELUS:			/* +MKIw8zC8MOswuQ- */
			sc->opt2 &= +AH4-OPT2_ANGELUS;
			opt_flag = 1;
			break;
		case SC_DPOISON:
			sc->opt2 &= +AH4-OPT2_HEAVYPOISON;
			opt_flag = 1;
			break;
		case SC_BLEED:
			sc->opt2 &= +AH4-OPT2_BLEEDING;
			opt_flag = 1;
			break;
		case SC_FEAR:
			sc->opt2 &= +AH4-OPT2_FEAR;
			opt_flag = 1;
			break;
		// opt3
		case SC_ONEHAND:		/* 1HQ */
		case SC_TWOHANDQUICKEN:		/* 2HQ */
		case SC_SPEARQUICKEN:		/* +MLkw1DCiMK8wpDDDMLEw8w- */
		case SC_CONCENTRATION:		/* +MLMw8zC7MPMwyDDsMPwwtzDnMPM- */
		case SC_WEAPONQUICKEN:		/* +MKYwpzDdMPMwrzCkMMMwsTDz- */
			sc->opt3 &= +AH4-OPT3_QUICKEN;
			opt_flag = 2;
			break;
		case SC_OVERTHRUST:		/* +MKow/DDQMPwwyDDpMLkwyA- */
		case SC_SWOO:			/* +MKgwuTCm- */
			sc->opt3 &= +AH4-OPT3_OVERTHRUST;
			opt_flag = 2;
			if(type == SC_SWOO)
				ud->state.change_speed = 1;
			break;
		case SC_ENERGYCOAT:		/* +MKgwyjC4MPwwszD8MMg- */
		case SC_SKE:			/* +MKgwuTCv- */
			sc->opt3 &= +AH4-OPT3_ENERGYCOAT;
			opt_flag = 2;
			break;
		case SC_EXPLOSIONSPIRITS:	/* +cgaIwmziUtU- */
			sc->opt3 &= +AH4-OPT3_EXPLOSIONSPIRITS;
			opt_flag = 2;
			break;
		case SC_STEELBODY:		/* +kdFSWw- */
		case SC_SKA:			/* +MKgwuTCr- */
			sc->opt3 &= +AH4-OPT3_STEELBODY;
			opt_flag = 2;
			break;
		case SC_BLADESTOP:		/* +dn1SA1PWMIo- */
			sc->opt3 &= +AH4-OPT3_BLADESTOP;
			opt_flag = 2;
			break;
		case SC_AURABLADE:			/* +MKow/DDpMNYw7DD8MMk- */
			sc->opt3 &= +AH4-OPT3_AURABLADE;
			opt_flag = 2;
			break;
		case SC_BERSERK:		/* +MNAw/DC1MPwwrw- */
			sc->opt3 &= +AH4-OPT3_BERSERK;
			opt_flag = 2;
			break;
		case SC_DANCING:			/* +MMAw8zC5-/+bxRZT04t- */
			if(sc->data[SC_DANCING].val1 != CG_MOONLIT)
				break;
			sc->opt3 &= +AH4-OPT3_MOON;
			opt_flag = 2;
			break;
		case SC_MARIONETTE:		/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- */
		case SC_MARIONETTE2:		/* +MN4w6jCqMM0wwzDIMLMw8zDIMO0w/DDr- */
			sc->opt3 &= +AH4-OPT3_MARIONETTE;
			opt_flag = 2;
			break;
		case SC_ASSUMPTIO:		/* +MKIwuTDgMNcwxjCjMKo- */
		case SC_ASSUMPTIO2:		/* +MK0w4zDDMLcw5TCiMLkw4DDXMMYwozCq- */
			sc->opt3 &= +AH4-OPT3_ASSUMPTIO;
			opt_flag = 2;
			break;
		case SC_WARM:			/* +bikwgjCK- */
			sc->opt3 &= +AH4-OPT3_SUN_WARM;
			opt_flag = 2;
			break;
		case SC_KAITE:			/* +MKswpDDI- */
			sc->opt3 &= +AH4-OPT3_KAITE;
			opt_flag = 2;
			break;
		case SC_BUNSINJYUTSU:		/* +UgaOqzBuiFM- */
			sc->opt3 &= +AH4-OPT3_BUNSIN;
			opt_flag = 2;
			break;
		case SC_MONK:			/* +MOIw8zCvMG6bQg- */
		case SC_STAR:			/* +MLEw8zC7MKQwbptC- */
		case SC_SAGE:			/* +MLsw/DC4MG6bQg- */
		case SC_CRUSADER:		/* +MK8w6zC7MKQwwDD8MG6bQg- */
		case SC_WIZARD:			/* +MKYwozC2MPwwyTBum0I- */
		case SC_PRIEST:			/* +MNcw6jD8MLkwyDBum0I- */
		case SC_ROGUE:			/* +MO0w/DCwMG6bQg- */
		case SC_ASSASIN:		/* +MKIwtTC3MPMwbptC- */
		case SC_SOULLINKER:		/* +ML0wpjDrMOow8zCrMPwwbptC- */
		case SC_KNIGHT:			/* +MMowpDDIMG6bQg- */
		case SC_ALCHEMIST:		/* +MKIw6zCxMN8wuTDIMG6bQg- */
		case SC_BARDDANCER:		/* +MNAw/DDJMGgwwDDzMLUw/DBum0I- */
		case SC_BLACKSMITH:		/* +MNYw6TDDMK8wuTDfMLkwbptC- */
		case SC_HUNTER:			/* +MM8w8zC/MPwwbptC- */
		case SC_HIGH:			/* +TgBrIU4KT02Ad2ltMG6bQg- */
		case SC_SUPERNOVICE:		/* +MLkw/DDRMPwwzjD8MNMwuTBum0I- */
		case SC_GUNNER:			/* +MKww8zC5MOow8zCsMPwwbptC- */
		case SC_NINJA:			/* +X82ABTBum0I- */
		case SC_DEATHKINGHT:		/* +MMcwuTDKMKQwyDBum0I- */
		case SC_COLLECTOR:		/* +MLMw7DCvML8w/DBum0I- */
			sc->opt3 &= +AH4-OPT3_SOULLINK;
			opt_flag = 2;
			break;
		case SC_ELEMENTUNDEAD:		// +Tg1rew-
			sc->opt3 &= +AH4-OPT3_UNDEAD;
			opt_flag = 2;
			break;
		// option
		case SC_SIGHT:
			sc->option &= +AH4-OPTION_SIGHT;
			opt_flag = 1;
			break;
		case SC_HIDING:
			// +lx5lrDCKMGcwajBEkBpeODBuMM8wpDDJMGowiTCiMKQwszDzbYhTuw-
			if(sd && sc->data[type].val3 == 0)
				clif_status_load_id(sd,SI_HIDING,0);
			sc->option &= +AH4-OPTION_HIDE;
			opt_flag = 1;
			break;
		case SC_CLOAKING:
		case SC_CLOAKINGEXCEED:		/* +MK8w7TD8MK0w8zCwMKgwrzC3MPwwyQ- */
		case SC_NEWMOON:			/* +ZxRnCIEa- */
		case SC__INVISIBILITY:		/* +MKQw8zDTMLgw0zDqMMYwow- */
			sc->option &= +AH4-OPTION_CLOAKING;
			opt_flag = 1;
			break;
		case SC_INVISIBLE:
		case SC__FEINTBOMB:			/* +MNUwpzCkMPMwyDDcMOA- */
			sc->option &= +AH4-OPTION_SPECIALHIDING;
			opt_flag = 1;
			break;
		case SC_REVERSEORCISH:
			sc->option &= +AH4-OPTION_ORCFACE;
			opt_flag = 1;
			break;
		case SC_WEDDING:		/* +MKYwpzDHMKMw8zCw- */
			sc->option &= +AH4-OPTION_MARRIED;
			opt_flag = 1;
			break;
		case SC_RUWACH:
			sc->option &= +AH4-OPTION_RUWACH;
			opt_flag = 1;
			break;
		case SC_CHASEWALK:		/* +MMEwpzCkMLkwpjCpMPwwrw- */
			sc->option &= +AH4-(OPTION_CLOAKING | OPTION_FOOTPRINT);
			opt_flag = 1;
			break;
		case SC_FUSION:
			sc->option &= +AH4-OPTION_STAR2;
			opt_flag = 1;
			break;
		case SC_SANTA:
			sc->option &= +AH4-OPTION_SANTA;
			opt_flag = 1;
			break;
		case SC_SUMMER:
			sc->option &= +AH4-OPTION_SUMMER;
			opt_flag = 1;
			break;
		case SC_ON_PUSH_CART:
#if PACKETVER < 20120201
			// +U+QwRDCvMOkwpDCiMPMwyDBvMKow1zC3MOcw8zCSZvRlsDBZMIs-
			sc->option &= +AH4-sc->data[type].val1;
			opt_flag = 1;
#endif
			break;
	}

	/* option+MG5ZCWb0- */
	if(opt_flag == 1) {
		clif_changeoption(bl);
	} else if(opt_flag == 2) {
		clif_changeoption2(bl);
	}

#ifdef DYNAMIC_SC_DATA
	if(sc->count <= 0 && battle_config.free_sc_data_dynamically)
		status_free_sc_data(sc);
#endif

	/* +MLkwxjD8ML8wuVGNigh7lw- */
	if(sd) {
		if(calc_flag || sd->auto_status_calc_pc[type] == 1) {
			status_calc_pc(sd,0);
		}
	} else if(hd) {
		if(calc_flag) {
			homun_calc_status(hd);
			clif_send_homstatus(hd->msd,0);
		}
	} else if(mcd) {
		if(calc_flag) {
			merc_calc_status(mcd);
			clif_send_mercstatus(mcd->msd);
			clif_mercskillinfoblock(mcd->msd);
		}
	} else if(eld) {
		if(calc_flag) {
			elem_calc_status(eld);
			clif_send_elemstatus(eld->msd);
		}
	}

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuVGNigh7lzCSTgBmQlBca2IwWTCL-
 *------------------------------------------
 */
int status_calc_pc_stop_begin(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC)
		((struct map_session_data *)bl)->stop_status_calc_pc+-+-;
	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuVGNigh7lzCSUY2VizBZMIs-
 *------------------------------------------
 */
int status_calc_pc_stop_end(struct block_list *bl)
{
	nullpo_retr(0, bl);

	if(bl->type == BL_PC) {
		struct map_session_data *sd = (struct map_session_data *)bl;
		sd->stop_status_calc_pc--;
		if(sd->stop_status_calc_pc == 0 && sd->call_status_calc_pc_while_stopping > 0)
			status_calc_pc(sd,0);
		if(sd->stop_status_calc_pc < 0) {
			printf("status_calc_pc_stop_end+MExODWtjMGtUfDBzUfowVTCMMGYwRDB+MFkAXA-n");
		}
	}
	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjiVi1nLML8wpDDeMPw-
 *------------------------------------------
 */
static int status_pretimer_timer(int tid, unsigned int tick, int id, void *data)
{
	struct block_list *bl = map_id2bl(id);
	struct status_pretimer *stpt = NULL;
	struct unit_data *ud;

	if(bl == NULL)
		return 0;	// +inJfUw-ID+MEwwWTBnMGttiG7FMFcwZjBEMIs-

	nullpo_retr(0, ud = unit_bl2ud(bl));

	do {
		if(bl->prev == NULL)
			break;

		stpt = (struct status_pretimer*)data;
		stpt->timer = -1;

		if(linkdb_erase(&ud->statuspretimer, stpt) == NULL) {
			printf("status_pretimer: data lost !!+AFw-n");
			break;
		}

		if(stpt->target_id) {
			struct block_list *target = map_id2bl(stpt->target_id);
			if( target == NULL || bl->m != target->m || unit_isdead(bl) || unit_isdead(target) )
				break;
		} else {
			if(bl->m != stpt->m)
				break;
		}

		status_change_start(bl, stpt->type, stpt->val1, stpt->val2, stpt->val3, stpt->val4, stpt->tick, stpt->flag);
	} while(0);

	aFree(stpt);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjiVi1nLML8wpDDeMPwwblJKlmQ-
 *------------------------------------------
 */
int status_clearpretimer(struct block_list *bl)
{
	struct unit_data *ud;
	struct linkdb_node *node1, *node2;

	nullpo_retr(0, bl);
	nullpo_retr(0, ud = unit_bl2ud(bl));

	node1 = ud->statuspretimer;
	while(node1) {
		struct status_pretimer *stpt = (struct status_pretimer *)node1->data;
		if(stpt->timer != -1) {
			delete_timer(stpt->timer, status_pretimer_timer);
		}
		node2 = node1->next;
		aFree(stpt);
		node1 = node2;
	}
	linkdb_final(&ud->statuspretimer);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjiVi1nLML8wpDDeMPwwbjC7MMMwyA-
 *------------------------------------------
 */
int status_change_pretimer(struct block_list *bl,int type,int val1,int val2,int val3,int val4,int tick,int flag,int pre_tick)
{
	struct unit_data *ud;
	struct status_pretimer *stpt;

	nullpo_retr(1, bl);
	ud = unit_bl2ud(bl);
	if(ud == NULL)
		return 0;

	stpt = (struct status_pretimer *)aCalloc(1, sizeof(struct status_pretimer));
	stpt->timer     = add_timer(pre_tick, status_pretimer_timer, bl->id, stpt);
	stpt->target_id = bl->id;
	stpt->m         = bl->m;
	stpt->type      = type;
	stpt->val1      = val1;
	stpt->val2      = val2;
	stpt->val3      = val3;
	stpt->val4      = val4;
	stpt->tick      = tick;
	stpt->flag      = flag;

	linkdb_insert(&ud->statuspretimer, stpt, stpt);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjh9Qk6GML8wpDDeMPw-
 *
 * +ML0w/DC5MJJP7mtjMFkwi2W5MHgwbmzoYQ8-
 *
 * +MPtytmFLdXBeOH2ZfZpmQjBrMG8wAQ-add_timer() +MFcwX3b0X4wwaw- break +MFkwizBTMGg-
 * +MPtytmFLdXBeOH1CToZmQjBrMG8wAZWiZXAwbmcAX4wwazBCMIs-status_change_end() +MG4-
 *   +VHwwc1H6MFdSTTBr- return +MFcwajBEMFMwaA-
 *
 * +MFMwbv8ScLkwTFuIMIkwjDBmMEQwajBEMGgwAU7WTrowbnK2YUt1cF44MExS3WJLMGuJ45ZkMFUwjDBfMIowAQ-
 * delete_timer error+MExR+jBmME8wizBqMGkwbjDQMLAwTHZ6dR8wVzB+MFkwAg-
 *------------------------------------------
 */
int status_change_timer(int tid, unsigned int tick, int id, void *data)
{
	int type  = PTR2INT(data);
	int timer = -1;
	struct block_list *bl;
	struct map_session_data *sd  = NULL;
	struct mob_data         *md  = NULL;
	struct homun_data       *hd  = NULL;
	struct merc_data        *mcd = NULL;
	struct elem_data        *eld = NULL;
	struct status_change    *sc  = NULL;

	if(type < 0 || type >= MAX_STATUSCHANGE)
		return 0;

	if((bl = map_id2bl(id)) == NULL)
		return 0;	// +inJfUw-ID+MEwwWTBnMGttiG7FMFcwZjBEMIswaDBEMEYwbjBvMEQwSzBrMIIwQjCKMF0wRjBqMG4wZzC5MOsw/DBXMGYwfzCL-

	nullpo_retr(0, sc = status_get_sc(bl));

	sd  = BL_DOWNCAST( BL_PC,   bl );
	md  = BL_DOWNCAST( BL_MOB,  bl );
	hd  = BL_DOWNCAST( BL_HOM,  bl );
	mcd = BL_DOWNCAST( BL_MERC, bl );
	eld = BL_DOWNCAST( BL_ELEM, bl );

	if(sc->data[type].timer != tid) {
		if(battle_config.error_log)
			printf("status_change_timer %d != %d (type = %d)+AFw-n",tid,sc->data[type].timer,type);
		return 0;
	}

	map_freeblock_lock();

	switch(type) {	/* +cnlrijBqUeZ0BjBrMGowi1g0VAg- */
	case SC_MAXIMIZEPOWER:	/* +MN4wrTC3MN4wpDC6MNEw7zD8- */
	case SC_CLOAKING:	/* +MK8w7TD8MK0w8zCw- */
		if(sd) {
			if(sd->status.sp > 0) {	/* SP+UgcwjDCLMH4wZ2MBfZo- */
				sd->status.sp--;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
					sc->data[type].val2+-tick, status_change_timer,
					bl->id, data);
			}
		}
		break;

	case SC_CHASEWALK:	/* +MMEwpzCkMLkwpjCpMPwwrw- */
		if(sd) {
			int sp = 10+-sc->data[SC_CHASEWALK].val1*2;
			if(map[sd->bl.m].flag.gvg)
				sp *= 5;
			if(sd->status.sp > sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				if((+-+-sc->data[SC_CHASEWALK].val4) == 1) {
					// +MO0w/DCwMG6bQjBqMIlSuWecZkKVkw-10+UA0-
					status_change_start(
						bl, SC_CHASEWALK_STR, 1<<(sc->data[SC_CHASEWALK].val1-1), 0, 0, 0,
						((sc->data[SC_ROGUE].timer != -1)? 300000: 30000), 0
					);
				}
				timer = add_timer( /* +ML8wpDDeMPxRjYotW5o- */
					sc->data[type].val2+-tick, status_change_timer, bl->id, data);
			}
		}
		break;

	case SC_HIDING:		/* +MM8wpDDHMKMw8zCw- */
		if(sd) {		/* SP+MEwwQjBjMGYwAWZClZNSNpZQMG6VkzBvYwF9mg- */
			if(sd->status.sp > 0 && (--sc->data[type].val2) > 0) {
#ifdef PRE_RENEWAL
				if(sc->data[type].val2 % (sc->data[type].val1+-3) == 0) {
#else
				if(sc->data[type].val2 % (sc->data[type].val1+-4) == 0) {
#endif
					sd->status.sp--;
					clif_updatestatus(sd,SP_SP);
				}
				timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
					1000+-tick, status_change_timer,
					bl->id, data);
			}
		}
		break;

	case SC_SIGHTBLASTER:
	case SC_SIGHT:		/* +MLUwpDDI- */
	case SC_RUWACH:		/* +MOswojDV- */
		{
			int range;
			if(type == SC_RUWACH)
				range = 2;
			else if(type == SC_SIGHTBLASTER)
				range = 1;
			else
				range = (sc->data[type].val4 == NPC_WIDESIGHT)? 14: 3;

			map_foreachinarea(status_change_timer_sub,
				bl->m, bl->x-range, bl->y-range, bl->x+-range,bl->y+-range, BL_CHAR,
				bl, type, sc->data[type].val1, tick);

			if((--sc->data[type].val2) > 0) {
				timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
					250+-tick, status_change_timer,
					bl->id, data);
			}
		}
		break;

	case SC_SIGNUMCRUCIS:		/* +MLcwsDDKMOAwrzDrMLcwuQ- */
		{
			int race = status_get_race(bl);
			if(race == RCT_DEMON || battle_check_undead(race,status_get_elem_type(bl))) {
				timer = add_timer(1000*600+-tick, status_change_timer, bl->id, data);
			}
		}
		break;

	case SC_PROVOKE:	/* +MNcw7TDcMMMwrw-/+MKow/DDIMNAw/DC1MPwwrw- */
		if(sc->data[type].val2 != 0) {	/* +MKow/DDIMNAw/DC1MPwwr/8I/xF50jBUMGgwaw-HP+MMEwpzDDMK//CQ- */
			if(status_get_hp(bl) > status_get_max_hp(bl)>>2)	/* +UFxrYg- */
				break;
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;

	case SC_DISSONANCE:	/* +Tg1TVFSMl/M- */
		if((--sc->data[type].val2) > 0) {
			struct skill_unit *unit = map_id2su(sc->data[type].val4);
			struct block_list *src;
			if(!unit || !unit->group)
				break;
			src = map_id2bl(unit->group->src_id);
			if(!src)
				break;
			battle_skill_attack(BF_MISC,src,&unit->bl,bl,unit->group->skill_id,sc->data[type].val1,tick,0);
			timer = add_timer(
				skill_get_time2(unit->group->skill_id,unit->group->skill_lv)+-tick, status_change_timer,
				bl->id, data);
		}
		break;
	case SC_UGLYDANCE:	/* +gepSBlLdYkswajDAMPMwuQ- */
		if((--sc->data[type].val2) > 0) {
			struct skill_unit *unit = map_id2su(sc->data[type].val4);
			struct block_list *src;
			if(!unit || !unit->group)
				break;
			src = map_id2bl(unit->group->src_id);
			if(!src)
				break;
			skill_additional_effect(src,bl,unit->group->skill_id,sc->data[type].val1,0,tick);
			timer = add_timer(
				skill_get_time2(unit->group->skill_id,unit->group->skill_lv)+-tick, status_change_timer,
				bl->id, data);
		}
		break;

	case SC_LULLABY:	/* +W1BbiFUE- */
		if((--sc->data[type].val2) > 0) {
			struct skill_unit *unit = map_id2su(sc->data[type].val4);
			int interval;

			if(!unit || !unit->group || unit->group->src_id == bl->id)
				break;
#ifdef PRE_RENEWAL
			interval = skill_get_time(unit->group->skill_id,unit->group->skill_lv)/10;
#else
			interval = skill_get_time(unit->group->skill_id,unit->group->skill_lv)/15;
#endif
			skill_additional_effect(bl,bl,unit->group->skill_id,sc->data[type].val1,BF_LONG|BF_SKILL|BF_MISC,tick);
			timer = add_timer(interval+-tick, status_change_timer, bl->id, data);
		}
		break;

	case SC_STONE:
		if(sc->data[type].val2 != 0) {
			sc->data[type].val2 = 0;
			sc->data[type].val4 = 0;
			unit_stop_walking(bl,1);
			sc->opt1 = OPT1_STONECURSE;
			clif_changeoption(bl);
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		else if((--sc->data[type].val3) > 0) {
			int hp = status_get_max_hp(bl);
			if((+-+-sc->data[type].val4)%5 == 0 && status_get_hp(bl) > hp>>2) {
				hp = (hp < 100)? 1: hp/100;
				unit_heal(bl,-hp,0,0,0);
			}
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_POISON:
		if(sc->data[SC_SLOWPOISON].timer == -1 && (--sc->data[type].val3) > 0) {
			int hp    = status_get_hp(bl);
			int p_dmg = status_get_max_hp(bl);
			if(hp > p_dmg>>2) {		// +ZwBZJw-HP+MG4-25%+TuVOCg-
				p_dmg = 3 +- p_dmg*3/200;
				if(p_dmg >= hp)
					p_dmg = hp-1;	// +a9IwZzBva3swajBqMEQ-
				unit_heal(bl, -p_dmg, 0, 0, 0);
			}
		}
		if(sc->data[type].val3 > 0) {
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_DPOISON:
		if(sc->data[SC_SLOWPOISON].timer == -1 && (--sc->data[type].val3) > 0) {
			int hp = status_get_max_hp(bl);
			if(status_get_hp(bl) > hp>>2) {
				hp = 3 +- hp/50;
				unit_heal(bl, -hp, 0, 0, 0);
			}
		}
		if(sc->data[type].val3 > 0 && !unit_isdead(bl) && sc->data[type].timer != -1) {
			// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_BLEED:
		if(--sc->data[type].val3 > 0) {
			int dmg = atn_rand()%600 +- 200;
			if(md) {
				// mob+MG8-HP50+TuVOCzBrMGowiTBqMEQ-
				md->hp = (md->hp - dmg < 50)? 50: md->hp - dmg;
			} else {
				unit_heal(bl, -dmg, 0, 0, 0);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(10000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_TENSIONRELAX:	/* +MMYw8zC3MOcw8zDqMOkwwzCvMLk- */
		if(sd) {		/* HP+MExugDC/MPMwZzBqMFEwjDBwfZl9mg- */
			if(sd->status.max_hp > sd->status.hp) {
				if(sc->data[type].val2 % (sc->data[type].val1+-3) == 0)
					timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
						10000+-tick, status_change_timer,
						bl->id, data);
			} else if(sd->status.max_hp <= sd->status.hp) {
				status_change_end(&sd->bl,SC_TENSIONRELAX,-1);
			}
		}
		break;

	/* +ZkKVk1IHMIxxITBX/x//Hw- */
	case SC_AETERNA:
	case SC_TRICKDEAD:
	case SC_REJECTSWORD:		/* +MOowuDCnMK8wyDC9MPwwyQ- */
	case SC_MEMORIZE:		/* +MOEw4jDpMKQwug- */
	case SC_SACRIFICE:		/* +MLUwrzDqMNUwoTCkMLk- */
	case SC_READYSTORM:
	case SC_READYDOWN:
	case SC_READYTURN:
	case SC_READYCOUNTER:
	case SC_LUNARSTANCE:
	case SC_UNIVERSESTANCE:
	case SC_SUNSTANCE:
	case SC_STARSTANCE:
	case SC_DODGE:
	case SC_AUTOBERSERK:
	case SC_RUN:
	case SC_MARIONETTE:
	case SC_MARIONETTE2:
	case SC_REBIRTH:
	case SC_WUGDASH:
	case SC_EXEEDBREAK:
	case SC_SUHIDE:
	case SC_ALL_RIDING:	/* +mg5OVzC3MLkwxjDg- */
	case SC_ON_PUSH_CART:	/* +MKsw/DDI- */
	case SC_HAT_EFFECT:	/* +mC2IxVCZMKgw1TCnMK8wyA- */
	case SC_ACTIVE_MONSTER_TRANSFORM:	/* +MKIwrzDGMKMw1jDiMPMwuTC/MPxZCY6r- */
	case SC_INVINCIBLE:	/* +MKQw8zDTMPMwtzDWMOs- */
	case SC_STYLE_CHANGE:		/* +MLkwvzCkMOswwTCnMPMwuA- */
	case SC_GUARD_STANCE:		/* +MKww/DDJMLkwvzDzMLk- */
	case SC_ATTACK_STANCE:		/* +MKIwvzDDMK8wuTC/MPMwuQ- */
	case SC_INTENSIVE_AIM_COUNT:	/* +MKQw8zDGMPMwtzDWMKgwpDDg-(+MKswpjDzMMg-) */
		timer = add_timer(1000 * 600 +- tick, status_change_timer, bl->id, data);
		break;
	case SC_MODECHANGE:
		clif_emotion(bl,1);
		timer = add_timer(1500 +- tick, status_change_timer, bl->id, data);
		break;
	case SC_LONGINGFREEDOM:
		if(sd && sd->status.sp >= 3) {
			if(--sc->data[type].val3 <= 0)
			{
				sd->status.sp -= 3;
				clif_updatestatus(sd, SP_SP);
				sc->data[type].val3 = 3;
			}
			timer = add_timer(1000 +- tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_DANCING:
		if(sd) {	// +MMAw8zC5MLkwrTDrMG5mQpWT-SP+bYiMuw-
			int cost = 1;
			if(sc->data[type].val1 == CG_HERMODE)
				cost = 5;
			if(sd->status.sp < cost) {
				skill_stop_dancing(&sd->bl,0);
			} else if(--sc->data[type].val3 > 0) {
				int s = 0;

				switch(sc->data[type].val1) {
				case BD_RICHMANKIM:			/* +MMsw6DDrMMkwblu0- 3+edIwaw-SP1 */
				case BD_DRUMBATTLEFIELD:		/* +YiZZKp8TMG6X/zBN- 3+edIwaw-SP1 */
				case BD_RINGNIBELUNGEN:			/* +MMsw/DDZMOsw8zCwMG5jB48q- 3+edIwaw-SP1 */
				case BD_SIEGFRIED:			/* +Tg1re46rMG4wuDD8MK8w1TDqMPwwyQ- 3+edIwaw-SP1 */
				case BA_DISSONANCE:			/* +Tg1TVFSMl/M- 3+edIwZw-SP1 */
				case BA_ASSASSINCROSS:			/* +WRWWfTBuMKIwtTC3MPMwrzDtMLk- 3+edIwZw-SP1 */
				case DC_UGLYDANCE:			/* +gepSBlLdYkswajDAMPMwuQ- 3+edIwZw-SP1 */
				case CG_LONGINGFREEDOM:
					s = 3;
					break;
				case BD_LULLABY:			/* +W1BbiGtM- 4+edIwaw-SP1 */
				case BD_ETERNALCHAOS:			/* +bDiQYDBubfdsjA- 4+edIwaw-SP1 */
				case BD_ROKISWEIL:			/* +MO0wrTBuU+swcw- 4+edIwaw-SP1 */
				case DC_FORTUNEKISS:			/* +XniQSzBuMK0wuQ- 4+edIwZw-SP1 */
					s = 4;
					break;
				case BD_INTOABYSS:			/* +bfFt9TBuTi0waw- 5+edIwaw-SP1 */
				case BA_WHISTLE:			/* +U+N7Gw- 5+edIwZw-SP1 */
				case DC_HUMMING:			/* +MM8w3zDzMLA- 5+edIwZw-SP1 */
				case BA_POEMBRAGI:			/* +MNYw6TCuMG6KaQ- 5+edIwZw-SP1 */
				case DC_SERVICEFORYOU:			/* +MLUw/DDTMLkw1TCpMPww5jD8- 5+edIwZw-SP1 */
				case CG_HERMODE:			/* +MNgw6zDiMPwwyTBuZ1Y- */
					s = 5;
					break;
				case BA_APPLEIDUN:			/* +MKQwyTClMPMwbmeXao4- 6+edIwZw-SP1 */
					s = 6;
					break;
				case DC_DONTFORGETME:			/* +ecEwkl/YMIwwajBEMGcgJg- 10+edIwZw-SP1 */
				case CG_MOONLIT:			/* +ZwhmDjCKMG5OCzBn- 10+edIwZw-SP1+/x8- */
					s = 10;
					break;
				}
				if(s && (sc->data[type].val3 % s) == 0) {
					sd->status.sp -= cost;
					clif_updatestatus(sd,SP_SP);
				}
				timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
					1000+-tick, status_change_timer,
					bl->id, data);
			}
		} else if(md)
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		break;
	case SC_BERSERK:		/* +MNAw/DC1MPwwrw- */
		{
			int dmg = (int)((atn_bignumber)status_get_max_hp(bl) * 5 / 100);
			if(status_get_hp(bl) - dmg > 100) {
				if(sd) {
					sd->status.hp -= dmg;
					clif_updatestatus(sd,SP_HP);
				} else if(hd) {
					hd->status.hp -= dmg;
					clif_send_homstatus(hd->msd,0);
				} else if(mcd) {
					mcd->status.hp -= dmg;
					clif_mercupdatestatus(mcd->msd,SP_HP);
				}
				timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
					10000+-tick, status_change_timer,
					bl->id, data);
			} else {
				if(sd) {
					sd->status.hp = 100;
					clif_updatestatus(sd,SP_HP);
				} else if(hd) {
					hd->status.hp = 100;
					clif_send_homstatus(hd->msd,0);
				} else if(mcd) {
					mcd->status.hp = 100;
					clif_mercupdatestatus(mcd->msd,SP_HP);
				}
			}
		}
		break;
	case SC_NOCHAT:			/* +MMEw4zDDMMh5gWticrZhSw- */
		if(sd) {
			if(+-+-sd->status.manner && time(NULL) < (sc->data[type].val2 - 60*sd->status.manner)) {	// +lYtZyzBLMIk-status.manner+UgZ9TDBjMGYwajBEMG4wZ32ZfZo-
				clif_updatestatus(sd,SP_MANNER);
				timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o-(60+edI-) */
					60000+-tick, status_change_timer,
					bl->id, data);
			}
		}
		break;
	case SC_SELFDESTRUCTION:		/* +gepyBg- */
		if(md && unit_iscasting(&md->bl) && md->state.special_mob_ai == 3 && md->mode&MD_CANMOVE && md->speed > 0) {
			md->speed -= 5;
			if(md->speed <= 0)
				md->speed = 1;
			md->dir = sc->data[type].val4;
			unit_walktodir(&md->bl,1);	// +kB9epjBMWQkwjzCLMG4wZ2vOVt5UfDBzUfowWQ-

			/* +ML8wpDDeMPxRjYotW5o- */
			timer = add_timer(100+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_BOSSMAPINFO:			/* +UfiXYpPh- */
		if(sd && --sc->data[type].val2 > 0) {
			struct mob_data *mmd = map[sd->bl.m].mvpboss;
			if(mmd == NULL)
				break;
			if(mmd->bl.prev == NULL) {
				if(sc->data[type].val3 >= 0) {
					// +UBIwVTCMMF8wbjBnayFW3jBuUfpz/mZClZMwkjCiMMowpjDzMLkwVzBmfUJOhg-
					int diff = DIFF_TICK(mmd->last_spawntime, gettick());
					if(diff < 0)
						diff = 0;
					clif_bossmapinfo(sd, mmd->name, 0, 0, diff, 3);
					break;
				}
			} else {
				if(sc->data[type].val3 < 0) {
					// +Ufpz/jBXMF8wbjBnMKIwyjCmMPMwuQ-
					clif_bossmapinfo(sd, mmd->name, 0, 0, 0, 2);
				}
				if(sc->data[type].val3 != mmd->bl.m || sc->data[type].val4 != mmd->bl.x +- (mmd->bl.y << 16)) {
					// +Ufpz/k4tMGdep2oZMExZCVMWMFcwZjBEMIswbjBnMN8wyw-MAP+MG4wyTDDMMgwkmb0ZbA-
					if(mmd->bl.m == sd->bl.m) {
						clif_bossmapinfo(sd, "", mmd->bl.x, mmd->bl.y, 0, 1);
					} else {
						clif_bossmapinfo(sd, "", -1, -1, 0, 1);
					}
					sc->data[type].val3 = mmd->bl.m;
					sc->data[type].val4 = mmd->bl.x +- (mmd->bl.y << 16);
				}
			}
			/* +ML8wpDDeMPxRjYotW5o- */
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_CHANGE:		/* +MOEw8zC/MOswwTCnMPMwuA- */
		unit_heal(bl, -status_get_hp(bl)+-10, -status_get_sp(bl)+-10, 0, 0);	// +ZkKVk1IHMIwwbjBoME0wbjB/-HP,SP+MEw-10+MGswajCL-
		break;
	case SC_ABUNDANCE:		/* +MKIw0DDzMMAw8zC5- */
		if((--sc->data[type].val3) > 0) {
			unit_heal(bl, 0, 60, 0, 1);
			timer = add_timer(10000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_FEAR:				/* +YFBgFg- */
		if(sc->data[type].val3 > 0) {
			timer = add_timer(sc->data[type].val3+-tick, status_change_timer, bl->id, data);
			sc->data[type].val3 = 0;
		}
		break;
	case SC_WEAPONBLOCKING:		/* +MKYwpzDdMPMw1jDtMMMwrTDzMLA- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				if(sd->status.sp < 3)
					break;
				sd->status.sp -= 3;
				clif_updatestatus(sd,SP_SP);
			}
			timer = add_timer(5000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_CLOAKINGEXCEED:	/* +MK8w7TD8MK0w8zCwMKgwrzC3MPwwyQ- */
		if(sd) {
			int sp = 10 - sc->data[type].val1;
			if(sp > 0 && sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_TOXIN:		/* +MMgwrTC3MPM- */
		if((--sc->data[type].val2) > 0) {
			int damage = (int)((atn_bignumber)status_get_max_sp(bl) * 3 / 100);
			if(damage)
				unit_heal(bl, 0, -damage, 0, 0);
			clif_damage(bl,bl,tick,0,status_get_dmotion(bl),1,0,0,0,0);
			unit_skillcastcancel(bl,0);		// +imBVMVmoW7M-
			timer = add_timer(10000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_MAGICMUSHROOM:	/* +MN4wuDDDMK8w3jDDMLcw5TDrMPww4A- */
		if((--sc->data[type].val2) > 0) {
			clif_emotion(bl,18);
			if(sc->data[type].val2 % 2 == 0) {
				int damage = (int)((atn_bignumber)status_get_max_hp(bl) * 3 / 100);
				if(sd) {
					sd->skill_item.id = skill_searchrandomid(1);
					sd->skill_item.lv = 1;
					sd->skill_item.flag = 1;
					clif_item_skill(sd, sd->skill_item.id, sd->skill_item.lv, "");
				}
				if(damage)
					unit_heal(bl, -damage, 0, 0, 0);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(2000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_PYREXIA:	/* +MNEwpDDsMMMwrzC3MKI- */
		if((--sc->data[type].val2) > 0) {
			clif_damage(bl,bl,tick,0,status_get_dmotion(bl),100,0,0,0,0);
			battle_damage(bl,bl,100,0,0,0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(3000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_OBLIVIONCURSE:	/* +MKow1jDqMNMwqjDzMKsw/DC5- */
		if((--sc->data[type].val2) > 0) {
			clif_emotion(bl,1);
			timer = add_timer(2000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_LEECHEND:		/* +MOow/DDBMKgw8zDJ- */
		if((--sc->data[type].val2) > 0) {
			int damage = status_get_max_hp(bl) / 100;
			if(damage)
				unit_heal(bl, -damage, 0, 0, 0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_RENOVATIO:		/* +MOwwzjD0MKEwxjCjMKo- */
		if((--sc->data[type].val3) > 0) {
			int heal = (int)((atn_bignumber)status_get_max_hp(bl) * sc->data[type].val2 / 100);
			if(heal) {
				if(sc->data[SC_AKAITSUKI].timer != -1)
					unit_fixdamage(bl,bl,gettick(),0,status_get_dmotion(bl),heal,0,0,0,0);
				else
					unit_heal(bl,heal,0,0,1);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(5000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_SUMMONBALL1:		/* +MLUw4jDzMNww/DDr-(1+UAt27g-) */
	case SC_SUMMONBALL2:		/* +MLUw4jDzMNww/DDr-(2+UAt27g-) */
	case SC_SUMMONBALL3:		/* +MLUw4jDzMNww/DDr-(3+UAt27g-) */
	case SC_SUMMONBALL4:		/* +MLUw4jDzMNww/DDr-(4+UAt27g-) */
	case SC_SUMMONBALL5:		/* +MLUw4jDzMNww/DDr-(5+UAt27g-) */
		if((--sc->data[type].val3) > 0) {
			// SP+bYiMuw-
			if(sd) {
				int sp = 6 - sc->data[type].val1;
				if(sd->status.sp >= sp) {
					sd->status.sp -= sp;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(1000 +- tick, status_change_timer, bl->id, data);
				}
			}
			else
				timer = add_timer(1000 +- tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_HELLINFERNO:	/* +MNgw6zCkMPMw1TCnMOswzg- */
		if((--sc->data[type].val2) > 0) {
			if(+-+-sc->data[type].val3 % 3 == 0) {
				int damage = (int)((atn_bignumber)status_get_max_hp(bl) * 3 / 100);
				sc->data[type].val3 = 0;
				clif_damage(bl,bl,tick,0,0,1000 +- damage,0,9,0,0);
				battle_damage(bl,bl,1000 +- damage,0,0,0);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_SPELLBOOK:	/* +MLkw2jDrMNYwwzCv- */
		if(sd) {
			int sp = sd->freeze_sp_slot;
			if(sp > 0 && sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(10000 +- tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_ELECTRICSHOCKER:	/* +MKgw7DCvMMgw6jDDMK8wtzDnMMMwqzD8- */
		if((--sc->data[type].val2) > 0) {
			int damage = (int)((atn_bignumber)status_get_max_sp(bl) * sc->data[type].val1 * 5 / 100);
			if(damage)
				unit_heal(bl, 0, -damage, 0, 0);
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_CAMOUFLAGE:	/* +MKsw4jDVMOkw/DC4MOU- */
		if((+-+-sc->data[type].val2) > 10)
			sc->data[type].val2 = 10;
		if(sd) {
			int sp = 7 - sc->data[type].val1;
			if(sp > 0 && sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
		} else {
			timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
		}
		break;
	case SC_OVERHEAT_LIMITPOINT:		/* +MKow/DDQMPww0jD8MMhSoHGxUCQ- */
		if(sd && !pc_isgear(sd))		// +m1RcDjCuMKKXXmQtTlcwajCJMHBrYjCBMIs-
			break;
		if(sc->data[type].val2 > 0)		// +UbdTdGZClZP/CA-10+edL/CTBMa4swYzBmMEQwiw-
			sc->data[type].val2--;
		if(sc->data[type].val2 <= 0)	// +UbdTdGZClZMwTG6AToYwVzBmMEQwXzCJUqBxsVAkMJJuG3uX-
			sc->data[type].val1--;
		if(sc->data[type].val1 > 0)		// +UqBxsVAkMExrizBjMGYwRDCLMF8wgX2ZfZo-
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		break;
	case SC_OVERHEAT:			/* +MKow/DDQMPww0jD8MMg- */
		if(sd && !pc_isgear(sd))		// +m1RcDjCuMKKXXmQtTlcwajCJMHBrYjCBMIs-
			break;
		if(sc->data[SC_OVERHEAT_LIMITPOINT].timer == -1)	// +MKow/DDQMPww0jD8MMhSoHGxUCQwTFKge5cwVTCMMGYwajBRMIwwcGtiMIEwiw-
			break;
		else {
			int damage = status_get_max_hp(bl) / 100;
			int hp = status_get_hp(bl);
			if(damage && hp) {
				if(damage >= hp)
					damage = hp - 1;	// +MKow/DDQMPww0jD8MMgwZzBva3swajBqMEQ-
				unit_heal(bl, -damage, 0, 0, 0);
			}
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_MAGNETICFIELD:		/* +MN4wsDDNMMYwozDDMK8w1TCjMPww6zDJ- */
		if((--sc->data[type].val2) > 0) {
			unit_heal(bl, 0, -50, 0, 0);
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_STEALTHFIELD_MASTER:	/* +MLkwxjDrMLkw1TCjMPww6zDJ-(+T391KIAF-) */
		if(sd && !pc_isgear(sd))		// +m1RcDjCuMKKXXmQtTlcwajCJMHBrYjCBMIs-
			break;
		else if((--sc->data[type].val2) > 0) {
			int damage = status_get_max_sp(bl) / 100;
			if(damage)
				unit_heal(bl, 0, -damage, 0, 0);
			timer = add_timer(tick+-sc->data[type].val3, status_change_timer, bl->id, data);
		}
		break;
	case SC__REPRODUCE:		/* +MOow1zDtMMcw5TD8MLk- */
		if(sd) {
			int sp = 9 - (sc->data[type].val1 +- 1) / 2;
			if(sp > 0 && sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		} else {
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC__SHADOWFORM:		/* +MLcw4zDJMKYw1TCpMPww4A- */
		if((--sc->data[type].val4) > 0) {
			if(sd) {
				int sp = 11 - sc->data[type].val1;
				if(sp > 0 && sd->status.sp >= sp) {
					sd->status.sp -= sp;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
				}
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC__INVISIBILITY:		/* +MKQw8zDTMLgw0zDqMMYwow- */
		if(sd) {
			int sp = sd->status.max_sp * 5 / 100;
			if(sp > 0 && sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_REFLECTDAMAGE:		/* +MOow1TDsMK8wyDDAMOEw/DC4- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int sp = 20 +- sc->data[type].val1 * 10;
				if(sd->status.sp >= sp) {
					sd->status.sp -= sp;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(10000+-tick, status_change_timer,bl->id, data);
				}
			} else {
				timer = add_timer(10000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_FORCEOFVANGUARD:	/* +MNUwqTD8MLkwqjDWMNAw8zCsMPwwyQ- */
		if(sd) {
			int sp = 24 - sc->data[type].val1 * 4;
			if(sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(10000+-tick, status_change_timer,bl->id, data);	
			}
		}
		break;
	case SC_BANDING:		/* +MNAw8zDHMKMw8zCw- */
		if(sd) {
			int sp = 7 - sc->data[type].val1;
			if(sd->status.sp >= sp) {
				sd->status.sp -= sp;
				clif_updatestatus(sd,SP_SP);
				timer = add_timer(5000+-tick, status_change_timer,bl->id, data);	
			}
		}
		break;
	case SC_INSPIRATION:	/* +MKQw8zC5MNQw7DD8MLcw5zDz- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int hp = sd->status.max_hp / 100;
				int sp = sd->status.max_sp / 100;
				unit_heal(bl, -hp, -sp, 0, 0);
				if(sd->status.sp > 0 && !unit_isdead(bl) && sc->data[type].timer != -1) {
					// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
					timer = add_timer(6000+-tick, status_change_timer,bl->id, data);
				}
			} else {
				timer = add_timer(6000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_KINGS_GRACE:	/* +MK0w8zCwMLkwsDDsMKQwuQ- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int hp = sd->status.max_hp * sc->data[type].val4 / 100;
				unit_heal(bl, hp, 0, 0, 0);
			}
			timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
		}
		break;
	case SC_RAISINGDRAGON:		/* +b1yfjWYHWSk- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int hp = sd->status.max_hp / 100;
				if(hp > 0 && sd->status.hp >= hp) {
					sd->status.hp -= hp;
					clif_updatestatus(sd,SP_HP);
					if(sd->status.max_hp * 15 / 100 < sd->status.hp) {
						timer = add_timer(5000+-tick, status_change_timer, bl->id, data);
					}
				}
			} else {
				timer = add_timer(5000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_WINKCHARM:	/* +m0Vg0TBuMKYwozDzMK8- */
	case SC_SIREN:		/* +MLswpDDsMPww8zBuWPA- */
		if((--sc->data[type].val3) > 0) {
			clif_emotion(bl,3);
			timer = add_timer(3000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_DEEP_SLEEP:		/* +W4kwiTBOMG5bUFuIVQQ- */
		if((--sc->data[type].val2) > 0) {
			int hp, sp = 0;
			hp = (int)((atn_bignumber)status_get_max_hp(bl) * 3 / 100);
			if(sd)
				sp = (int)((atn_bignumber)status_get_max_sp(bl) * 3 / 100);
			if(hp || sp)
				unit_heal(bl, hp, sp, 0, 0);
			timer = add_timer(2000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_SIRCLEOFNATURE:		/* +X6p0sDBZMIuB6nE2MG6X8w- */
		if((--sc->data[type].val2) > 0) {
			int hp = 300 * sc->data[type].val1;
			if(sd) {
				int sp = 4 * sc->data[type].val1;
				if(sd && sd->status.sp >= sp) {
					unit_heal(bl, hp, -sp, 0, 0);
					timer = add_timer(1000+-tick, status_change_timer,bl->id, data);	
				}
			} else {
				unit_heal(bl, hp, 0, 0, 0);
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);	
			}
		}
		break;
	case SC_MELODYOFSINK:		/* +MOEw7TDHMKMw/DCqMNYwtzDzMK8- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				unit_heal(bl, 0, -(sd->status.max_sp * sc->data[type].val3 / 100), 0, 0);
			}
			timer = add_timer(1000+-tick, status_change_timer,bl->id, data);	
		}
		break;
	case SC_SONG_OF_MANA:		/* +MN4wyjBua0w- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int sp = 10 +- 2 * sc->data[type].val4;
				if(sd->status.sp +- sp > sd->status.max_sp) {
					sp = sd->status.max_sp - sd->status.sp;
					sd->status.sp = sd->status.max_sp;
				} else {
					sd->status.sp +-= sp;
				}
				clif_updatestatus(sd,SP_SP);
			}
			timer = add_timer(5000+-tick, status_change_timer,bl->id, data);	
		}
		break;
	case SC_SATURDAY_NIGHT_FEVER:		/* +MNUw6TCkMMcw/DDKMKQwyDDVMKMw/DDQMPw- */
		if(--sc->data[type].val2 > 0) {
			int hp = 0,sp = 0;
			hp = (int)((atn_bignumber)status_get_max_hp(bl) / 100);
			if(sd)
				sp = (int)((atn_bignumber)status_get_max_sp(bl) / 100);
			unit_heal(bl, -hp, -sp, 0, 0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1)
				timer = add_timer(sc->data[type].val3+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_FRIGG_SONG:			/* +MNUw6jDDMLAwbmtM- */
		if(--sc->data[type].val2 > 0) {
			unit_heal(bl, sc->data[type].val4, 0, 0, 0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1)
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_DIAMONDDUST:		/* +MMAwpDDkMOIw8zDJMMAwuTDI- */
		if(--sc->data[type].val2 > 0) {
			int hp = 0,sp = 0;
			hp = (int)((atn_bignumber)status_get_max_hp(bl) * 2 / 100);
			if(sd)
				sp = status_get_max_sp(bl) / 100;
			if(hp >= status_get_hp(bl))
				hp = status_get_hp(bl) - 1;
			unit_heal(bl, -hp, -sp, 0, 0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1)
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_STRIKING:		/* +MLkwyDDpMKQwrTDzMLA- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int sp = 6 - sc->data[type].val1;
				if(sp > 0 && sd->status.sp >= sp) {
					sd->status.sp -= sp;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
				}
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_BLOOD_SUCKER:		/* +MNYw6TDDMMkwtTDDMKsw/A- */
		if((--sc->data[type].val4) > 0) {
			struct block_list *src = map_id2bl(sc->data[type].val3);
			if(src && tid != -1) {
				skill_castend_damage_id(src,bl,sc->data[type].val2,sc->data[type].val1,gettick(),0);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_FIRE_EXPANSION_TEAR_GAS:		/* +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmQ-) */
		if((--sc->data[type].val3) > 0) {
			int hp = (int)((atn_bignumber)status_get_max_hp(bl) / 100);
			clif_emotion(bl,28);
			// +MNUwoTCkMKIw/DCoMK8wuTDRMPMwtzDnMPM-(+UKxtmQ-)+MGcwb2t7MGowajBEMFMwaDBrMFkwi/8ITu7/CQ-
			if(hp >= status_get_hp(bl))
				hp = status_get_hp(bl) - 1;
			unit_heal(bl, -hp, 0, 0, 0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1)
				timer = add_timer(3000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_MEIKYOUSISUI:		/* +Zg6T4WtibDQ- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				int hp = (int)((atn_bignumber)status_get_max_hp(&sd->bl) * (sc->data[type].val1 * 2) / 100);
				int sp = (int)((atn_bignumber)status_get_max_sp(&sd->bl) * sc->data[type].val1 / 100);
				if(sd->status.max_hp >= sd->status.hp +- hp) {
					sd->status.hp +-= hp;
					clif_updatestatus(sd,SP_HP);
				} else {
					sd->status.hp = sd->status.max_hp;
					clif_updatestatus(sd,SP_HP);
				}
				if(sd->status.max_sp >= sd->status.sp +- sp) {
					sd->status.sp +-= sp;
					clif_updatestatus(sd,SP_SP);
				} else {
					sd->status.sp = sd->status.max_sp;
					clif_updatestatus(sd,SP_SP);
				}
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_IZAYOI:		/* +U0FRbVkc- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				int sp = 1 * sc->data[type].val1;
				if(sd->status.sp >= sp) {
					sd->status.sp -= sp;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
				}
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_KAGEMUSYA:	/* +XnuIUw- -+UgaOqw-- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				if(sd->status.sp >= 1) {
					sd->status.sp -= 1;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
				}
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_C_MARKER:	/* +MK8w6jDgML4w8zDeMPwwqzD8- */
		{
			struct map_session_data *tmpsd = map_id2sd(sc->data[type].val2);
			if((--sc->data[type].val4) > 0) {
				if(tmpsd && tmpsd->c_marker[sc->data[type].val3] == bl->id) {
					clif_crimson_marker(tmpsd, bl, false);
				}
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
			else if(sd) {
				sd->c_marker[sc->data[type].val3] = 0;
				clif_crimson_marker(sd, bl, true);
			}
		}
		break;
	case SC_FRESHSHRIMP:		/* +ZbCbrjBqMKgw0w- */
		if((--sc->data[type].val2) > 0) {
			unit_heal(bl, sc->data[type].val4, 0, 0, 1);
			timer = add_timer(sc->data[type].val3+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_HISS:				/* +i2ZiEg- */
		if((--sc->data[type].val4) > 0) {
			if(sc->data[type].val1 > 0)
				sc->data[type].val1--;
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_SV_ROOTTWIST:	/* +MN4wvzC/MNMwbmg5MGMwUw- */
		if((--sc->data[type].val3) > 0) {
			struct block_list *src = map_id2bl(sc->data[type].val2);
			if(src && tid != -1) {
				battle_skill_attack(BF_MISC,src,src,bl,SU_SV_ROOTTWIST_ATK,sc->data[type].val1,tick,(0x0f<<20)|0x500);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_BURNT:			/* +c0RwjlRq- */
		if((--sc->data[type].val3) > 0) {
			int hp = 2000;
			// +c0RwjlRqMGcwb2t7MGowajBEMFMwaDBrMFkwi/8ITu7/CQ-
			if(hp >= status_get_hp(bl))
				hp = status_get_hp(bl) - 1;
			unit_heal(bl, -hp, 0, 0, 0);
			if(!unit_isdead(bl) && sc->data[type].timer != -1)
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_SUMMON_ELEM:	/* +MLUw4jDzMKgw7DDhMPMwvzDr- */
		if((--sc->data[type].val2) > 0) {
			if(sd) {
				// +U+xVmjBXMGYwRDCLfL6XCjBMMEQwajBRMIwwcH1CToY-
				if(!sd->eld)
					break;
				if(sd->status.sp >= sc->data[type].val3) {
					sd->status.sp -= sc->data[type].val3;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(10000+-tick, status_change_timer, bl->id, data);
				}
			} else {
				timer = add_timer(10000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_WATER_SCREEN:	/* +MKYwqTD8ML8w/DC5MK8w6jD8MPM- */
		if((--sc->data[type].val2) > 0) {
			int heal = (int)((atn_bignumber)status_get_max_hp(bl) * 3 / 100);
			if(heal)
				unit_heal(bl, heal, 0, 0, 1);
			timer = add_timer(5000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_CIRCLE_OF_FIRE_OPTION:	/* +MLUw/DCvMOswqjDWMNUwoTCkMKI-(+fL6XCg-) */
	case SC_FIRE_CLOAK_OPTION:		/* +MNUwoTCkMKIw/DCvMO0w/DCv-(+fL6XCg-) */
	case SC_WATER_SCREEN_OPTION:	/* +MKYwqTD8ML8w/DC5MK8w6jD8MPM-(+fL6XCg-) */
	case SC_WATER_DROP_OPTION:		/* +MKYwqTD8ML8w/DDJMO0wwzDX-(+fL6XCg-) */
	case SC_WIND_STEP_OPTION:		/* +MKYwozDzMMkwuTDGMMMw1w-(+fL6XCg-) */
	case SC_WIND_CURTAIN_OPTION:	/* +MKYwozDzMMkwqzD8MMYw8w-(+fL6XCg-) */
	case SC_SOLID_SKIN_OPTION:		/* +ML0w6jDDMMkwuTCtMPM-(+fL6XCg-) */
	case SC_STONE_SHIELD_OPTION:	/* +MLkwyDD8MPMwtzD8MOswyQ-(+fL6XCg-) */
		if((--sc->data[type].val2) > 0) {
			if(eld) {
				if(eld->status.sp >= sc->data[type].val3) {
					eld->status.sp -= sc->data[type].val3;
					clif_elemupdatestatus(eld->msd,SP_SP);
					timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
				}
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_PYROTECHNIC_OPTION:	/* +MNEwpDDtMMYwrzDLMMMwrw-(+fL6XCg-) */
	case SC_AQUAPLAY_OPTION:	/* +MKIwrzCiMNcw7DCk-(+fL6XCg-) */
	case SC_GUST_OPTION:		/* +MKwwuTDI-(+fL6XCg-) */
	case SC_PETROLOGY_OPTION:	/* +MNowyDDtMLgw/A-(+fL6XCg-) */
	case SC_HEATER_OPTION:		/* +MNIw/DC/MPw-(+fL6XCg-) */
	case SC_COOLER_OPTION:		/* +MK8w/DDpMPw-(+fL6XCg-) */
	case SC_BLAST_OPTION:		/* +MNYw6TC5MMg-(+fL6XCg-) */
	case SC_CURSED_SOIL_OPTION:	/* +MKsw/DC5MMkwvTCkMOs-(+fL6XCg-) */
	case SC_TROPIC_OPTION:		/* +MMgw7TDUMMMwrw-(+fL6XCg-) */
	case SC_CHILLY_AIR_OPTION:	/* +MK8w/DDrMKgwojD8-(+fL6XCg-) */
	case SC_WILD_STORM_OPTION:	/* +MO8wpDDrMMkwuTDIMPww4A-(+fL6XCg-) */
	case SC_UPHEAVAL_OPTION:	/* +MKIwwzDXMNgwpDDQMOs-(+fL6XCg-) */
		if((--sc->data[type].val2) > 0) {
			if(eld) {
				if(eld->status.sp >= sc->data[type].val3) {
					eld->status.sp -= sc->data[type].val3;
					clif_elemupdatestatus(eld->msd,SP_SP);
					timer = add_timer(10000+-tick, status_change_timer,bl->id, data);
				}
			} else {
				timer = add_timer(10000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_ANGRIFFS_MODUS:		/* +MKIw8zCwMOow1TC5MOIwyTC5- */
		if((--sc->data[type].val2) > 0) {
			if(hd && hd->status.sp > 0) {
				if(hd->status.hp >= 100)
					hd->status.hp -= 100;
				if(hd->status.sp < 20)
					hd->status.sp = 0;
				else
					hd->status.sp -= 20;
				clif_send_homstatus(hd->msd,0);
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_CBC:				/* +fV4wgWKA- */
		if((--sc->data[type].val2) > 0) {
			int hp = 0, sp = 0;
			if(sc->data[type].val2 % 2) {	// 2+edJrzg-
				hp = sc->data[type].val3;
			}
			if(sc->data[type].val2 % 3) {	// 3+edJrzg-
				if(bl->type != BL_MOB)
					sp = sc->data[type].val4;
				else
					hp = sc->data[type].val4 * 10;
			}
			unit_heal(bl, -hp, -sp, 0, 0);
			timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
		}
		break;
	case SC_FULL_THROTTLE:		/* +MNUw6zC5MO0wwzDIMOs- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				int sp = (int)((atn_bignumber)status_get_max_sp(&sd->bl) * 5 / 100);
				if(sp > 0) {
					if(sd->status.sp >= sp) {
						sd->status.sp -= sp;
						clif_updatestatus(sd,SP_SP);
						timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
					}
					else if(sd->status.sp > 0) {
						sd->status.sp = 0;
						clif_updatestatus(sd,SP_SP);
						timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
					}
				}
			}
		}
		break;
	case SC_REBOUND:	/* +MOow0DCmMPMwyQ- */
		clif_emotion(bl,4);
		if((--sc->data[type].val2) > 0) {
			timer = add_timer(2000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_GRADUAL_GRAVITY:	/* +kc1Sm1iXUqA- */
		if((--sc->data[type].val3) >= 0) {
			if(sd) {
				int hp = (int)((atn_bignumber)status_get_max_hp(&sd->bl) * sc->data[type].val2 / 100);
				unit_heal(bl, -hp, 0, 0, 0);
				if(!unit_isdead(bl) && sc->data[type].timer != -1) {
					// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
					timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
				}
			} else {
				timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_KILLING_AURA:	/* +MK0w6jDzMLAwqjD8MOk- */
		if((--sc->data[type].val3) > 0) {
			if(bl && tid != -1) {
				skill_castend_damage_id(bl,bl,NPC_KILLING_AURA,sc->data[type].val1,gettick(),0);
			}
			if(!unit_isdead(bl) && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_NEWMOON:	/* +ZxRnCIEa- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				int sp = 1;
				if(sp > 0 && sd->status.sp >= sp) {
					sd->status.sp -= sp;
					clif_updatestatus(sd,SP_SP);
					timer = add_timer(1000+-tick, status_change_timer,bl->id, data);
				}
			}
		}
		break;
	case SC_FLASHKICK:		/* +Zh8wblNw- */
		{
			struct map_session_data *tsd = map_id2sd(sc->data[type].val2);
			if( tsd )
				tsd->stellar_mark[sc->data[type].val3] = 0;
		}
		break;
	case SC_SOULCOLLECT:		/* +m0IwboTEek0- */
		if(sd) {
			pc_addsoulenergy(sd,600000,1);
			timer = add_timer(tick+-sc->data[type].val2, status_change_timer,bl->id, data);
		}
		break;
	case SC_SOULUNITY:		/* +m0IwbpAjfVA- */
		if((--sc->data[type].val3) > 0) {
			unit_heal(bl, sc->data[type].val2, 0, 0, 1);
			timer = add_timer(3000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_HELPANGEL:		/* +WSlPfzBVMH5SqTBRMGY- */
		if((--sc->data[type].val2) > 0) {
			unit_heal(bl, 1000, 350, 0, 1);
			timer = add_timer(1000+-tick, status_change_timer, bl->id, data);
		}
		break;
	case SC_HANDICAPSTATE_CONFLAGRATION:	/* +cGtwfQ- */
		{
			int hp = (int)((atn_bignumber)status_get_max_hp(bl) * 15 / 100);
			unit_heal(bl,-hp,0,0,0);
			if(!unit_isdead(bl) && (--sc->data[type].val2) > 0 && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(tick+-3000, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_HANDICAPSTATE_DEADLYPOISON:		/* +Xzdr0g- */
		{
			int hp = (int)((atn_bignumber)status_get_max_hp(bl) * 10 / 100);
			unit_heal(bl,-hp,0,0,0);
			if(!unit_isdead(bl) && (--sc->data[type].val3) > 0 && sc->data[type].timer != -1) {
				// +dR8wTTBmMEQwZonjlmRuCDB/MGcwajBEMGowiX2ZfZo-
				timer = add_timer(tick+-1000, status_change_timer,bl->id, data);
			}
		}
		break;
	case SC_HANDICAPSTATE_DEPRESSION:		/* +YYKbMQ- */
		unit_heal(bl,0,-(status_get_max_sp(bl)/100),0,0);
		if((--sc->data[type].val2) > 0) {
			timer = add_timer(tick+-3000, status_change_timer, bl->id, data);
		}
		break;
	case SC_SERVANTWEAPON:		/* +MLUw/DD0MKEw8zDIMKYwpzDdMPM- */
		if((--sc->data[type].val4) > 0) {
			if(sd)
				pc_addball(sd,600000,1);
			timer = add_timer(tick+-sc->data[type].val3, status_change_timer,bl->id, data);
		}
		break;
	case SC_DANCING_KNIFE:		/* +MMAw8zC3MPMwsDDKMKQw1Q- */
		if((--sc->data[type].val3) > 0) {
			skill_castend_damage_id(bl,bl,SHC_DANCING_KNIFE,sc->data[type].val1,tick,0);
			timer = add_timer(tick+-sc->data[type].val2, status_change_timer,bl->id, data);
		}
		break;
	case SC_MEDIALE:		/* +MOEwxzCjMKIw6jDcMMgwpTDg- */
		if((--sc->data[type].val4) > 0) {
			skill_castend_nodamage_id(bl,bl,CD_MEDIALE_VOTUM,sc->data[type].val1,tick,0x10|sc->data[type].val2);
			timer = add_timer(tick+-sc->data[type].val3, status_change_timer,bl->id, data);
		}
		break;
	case SC_A_MACHINE:		/* +ZTtkg4jFf25nCVK5UxY- */
		if((--sc->data[type].val3) > 0) {
			skill_castend_damage_id(bl,bl,MT_A_MACHINE,sc->data[type].val1,tick,0);
			timer = add_timer(tick+-sc->data[type].val2, status_change_timer,bl->id, data);
		}
		break;
	case SC_ABYSSFORCEWEAPON:		/* +MNUw7TDgMLgwojDTMLk- */
		if((--sc->data[type].val4) > 0) {
			if(sd)
				pc_addball(sd,600000,1);
			timer = add_timer(tick+-sc->data[type].val3, status_change_timer,bl->id, data);
		}
		break;
	case SC_SOUNDBLEND:			/* +MLUwpjDzMMkw1jDsMPMwyQ- */
		if(sc->data[type].val2 != 0) {
			struct block_list *src = map_id2bl(sc->data[type].val2);
			if(src) {
				skill_castend_damage_id(src,bl,TR_SOUNDBLEND,sc->data[type].val1,tick,0);
			}
			sc->data[type].val2 = 0;
			timer = add_timer(tick+-200, status_change_timer, bl->id, data);
		}
		break;
	case SC_SUMMON_ELEMENTAL_ARDOR:		/* +MLUw4jDzMKIw6zDJMPww6w- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				// +U+xVmjBXMGYwRDCLfL6XCjBMMEQwajBRMIwwcH1CToY-
				if(!sd->eld)
					break;
				if(sd->eld->status.class_ != ELEMID_EM_ARDOR)
					break;
			}
			timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
		}
		break;
	case SC_SUMMON_ELEMENTAL_DILUVIO:		/* +MLUw4jDzMMcwozDrMNMwqg- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				// +U+xVmjBXMGYwRDCLfL6XCjBMMEQwajBRMIwwcH1CToY-
				if(!sd->eld)
					break;
				if(sd->eld->status.class_ != ELEMID_EM_DILUVIO)
					break;
			}
			timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
		}
		break;
	case SC_SUMMON_ELEMENTAL_PROCELLA:		/* +MLUw4jDzMNcw7TC7MOk- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				// +U+xVmjBXMGYwRDCLfL6XCjBMMEQwajBRMIwwcH1CToY-
				if(!sd->eld)
					break;
				if(sd->eld->status.class_ != ELEMID_EM_PROCELLA)
					break;
			}
			timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
		}
		break;
	case SC_SUMMON_ELEMENTAL_TERREMOTUS:	/* +MLUw4jDzMMYw7DDiMMgwpTC5- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				// +U+xVmjBXMGYwRDCLfL6XCjBMMEQwajBRMIwwcH1CToY-
				if(!sd->eld)
					break;
				if(sd->eld->status.class_ != ELEMID_EM_TERREMOTUS)
					break;
			}
			timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
		}
		break;
	case SC_SUMMON_ELEMENTAL_SERPENS:		/* +MLUw4jDzMLUw/DDaMPMwuQ- */
		if((--sc->data[type].val3) > 0) {
			if(sd) {
				// +U+xVmjBXMGYwRDCLfL6XCjBMMEQwajBRMIwwcH1CToY-
				if(!sd->eld)
					break;
				if(sd->eld->status.class_ != ELEMID_EM_SERPENS)
					break;
			}
			timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
		}
		break;
	case SC_FLAMETECHNIC_OPTION:		/* +MNUw7DCkMOAwxjCvMMswwzCv-(+fL6XCg-) */
	case SC_COLD_FORCE_OPTION:			/* +MLMw/DDrMMkw1TCpMPwwuQ-(+fL6XCg-) */
	case SC_GRACE_BREEZE_OPTION:		/* +MLAw7DCkMLkw1jDqMPwwug-(+fL6XCg-) */
	case SC_EARTH_CARE_OPTION:			/* +MKIw/DC5MLEwog-(+fL6XCg-) */
	case SC_DEEP_POISONING_OPTION:		/* +MMcwozD8MNcw3TCkMLowyzDzMLA-(+fL6XCg-) */
		if((--sc->data[type].val2) > 0) {
			if(eld) {
				if(eld->status.sp >= sc->data[type].val3) {
					eld->status.sp -= sc->data[type].val3;
					clif_elemupdatestatus(eld->msd,SP_SP);
					timer = add_timer(tick+-10000, status_change_timer,bl->id, data);
				} else {
					eld->status.sp = 0;
					clif_elemupdatestatus(eld->msd,SP_SP);
					elem_change_mode(eld,ELMODE_WAIT);
				}
			} else {
				timer = add_timer(tick+-10000, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_FLAMEARMOR_OPTION:			/* +MNUw7DCkMOAwojD8MN4w/A-(+fL6XCg-) */
	case SC_CRYSTAL_ARMOR_OPTION:		/* +MK8w6jC5ML8w6zCiMPww3jD8-(+fL6XCg-) */
	case SC_EYES_OF_STORM_OPTION:		/* +MKIwpDC6MKow1jC5MMgw/DDg-(+fL6XCg-) */
	case SC_STRONG_PROTECTION_OPTION:	/* +MLkwyDDtMPMwsDDXMO0wxjCvMLcw5zDz-(+fL6XCg-) */
	case SC_POISON_SHIELD_OPTION:		/* +MN0wpDC6MPMwtzD8MOswyQ-(+fL6XCg-) */
		if((--sc->data[type].val2) > 0) {
			if(eld) {
				if(eld->status.sp >= sc->data[type].val3) {
					eld->status.sp -= sc->data[type].val3;
					clif_elemupdatestatus(eld->msd,SP_SP);
					timer = add_timer(tick+-1000, status_change_timer,bl->id, data);
				} else {
					eld->status.sp = 0;
					clif_elemupdatestatus(eld->msd,SP_SP);
					elem_change_mode(eld,ELMODE_WAIT);
				}
			} else {
				timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
			}
		}
		break;
	case SC_INTENSIVE_AIM:			/* +MKQw8zDGMPMwtzDWMKgwpDDg- */
		if(sc->data[SC_INTENSIVE_AIM_COUNT].timer != -1) {
			if(sc->data[SC_INTENSIVE_AIM_COUNT].val1 < 10) {
				status_change_start(bl,SC_INTENSIVE_AIM_COUNT,sc->data[SC_INTENSIVE_AIM_COUNT].val1+-1,0,0,0,0,0);
			}
		} else {
			status_change_start(bl,SC_INTENSIVE_AIM_COUNT,1,0,0,0,0,0);
		}
		timer = add_timer(tick+-300, status_change_timer, bl->id, data);
		break;
	case SC_TALISMAN_OF_PROTECTION:		/* +W4iLd3sm- */
		if((--sc->data[type].val3) > 0) {
			clif_misceffect_value(bl, 312, sc->data[type].val2);
			clif_misceffect_value(bl, 657, sc->data[type].val2);
			unit_heal(bl,sc->data[type].val2,0,0,1);
			timer = add_timer(tick+-3000, status_change_timer, bl->id, data);
		}
		break;
	case SC_KI_SUL_RAMPAGE:		/* +ML8w/DDIMOsw6TDzMNow/DC4- */
		if((--sc->data[type].val3) > 0) {
			skill_castend_nodamage_id(bl,bl,SH_KI_SUL_RAMPAGE,sc->data[type].val1,tick,0x10|sc->data[type].val2);
			timer = add_timer(tick+-1000, status_change_timer, bl->id, data);
		}
		break;
	}

	if(timer == -1 && sd && sd->eternal_status_change[type] > 0 && !unit_isdead(&sd->bl))
	{
		timer = add_timer(	/* +ML8wpDDeMPxRjYotW5o- */
			sd->eternal_status_change[type]+-tick, status_change_timer,
			bl->id, data);
	}

	map_freeblock_unlock();

	if(timer != -1) {
		// +ML8wpDDeMPw-ID+MJJP3VtYMFcwZn2ZfZowWTCL-
		sc->data[type].timer = timer;
		return 0;
	}

	return status_change_end(bl, type, tid);
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjgwvzCkMN4w/HvEVvJR5nQG-
 *------------------------------------------
 */
int status_change_timer_sub(struct block_list *bl, va_list ap)
{
	struct block_list *src;
	struct status_change *sc;
	int type, skilllv;
	unsigned int tick;

	nullpo_retr(0, bl);
	nullpo_retr(0, ap);
	nullpo_retr(0, src = va_arg(ap,struct block_list*));

	type    = va_arg(ap,int);
	skilllv = va_arg(ap,int);
	tick    = va_arg(ap,unsigned int);

	if(!(bl->type & (BL_PC | BL_MOB | BL_MERC)))
		return 0;

	nullpo_retr(0, sc = status_get_sc(bl));

	switch( type ) {
	case SC_SIGHT:	/* +MLUwpDDI- */
	case SC_CONCENTRATE:	/* +lsZOLVKbVBFOCg- */
		skill_detect(bl);		// +MMcwozDGMKMwrzDIUrlnnA-
		break;
	case SC_RUWACH:	/* +MOswojDV- */
		if(skill_detect(bl)) {		// +MMcwozDGMK8wyFK5Z5wwZ2a0MEQwX3b4Ykswaw-
			battle_skill_attack(BF_MAGIC,src,src,bl,AL_RUWACH,skilllv,tick,0);
		}
		break;
	case SC_SIGHTBLASTER:
		if(battle_check_target(src, bl, BCT_ENEMY) > 0 && !unit_isdead(bl)) {
			struct status_change *ssc;
			battle_skill_attack(BF_MAGIC,src,src,bl,WZ_SIGHTBLASTER,skilllv,tick,0);
			ssc = status_get_sc(src);
			if(ssc) {
				ssc->data[type].val2 = 0;
			}
			status_change_end(src,SC_SIGHTBLASTER,-1);
		}
		break;
	}
	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjhRaInjlmQ-
 *------------------------------------------
 */
int status_change_clear(struct block_list *bl,int type)
{
	struct status_change *sc;
	int i;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

#ifdef DYNAMIC_SC_DATA
	if(sc->data == dummy_sc_data)
		return 0;
#endif

	if(sc->count <= 0)
		return 0;

	status_calc_pc_stop_begin(bl);
	for(i = 0; i < MAX_STATUSCHANGE; i+-+-) {
		if(i == SC_BABY && type == 0 && unit_isdead(bl))
			continue;

		/* +dXBeODBMMEIwizBqMIkwvzCkMN4w/DCSUkqWZDBZMIs- */
		if(i == SC_DANCING) {
			skill_stop_dancing(bl,0);
		} else {
			if(i == SC_BASILICA)
				skill_basilica_cancel(bl);
			status_change_end(bl,i,-1);
		}
	}
	status_calc_pc_stop_end(bl);

	sc->opt1 = OPT1_NORMAL;
	sc->opt2 = OPT2_NORMAL;
	sc->opt3 = OPT3_NORMAL;
	sc->option &= OPTION_MASK;

	if(type != 1) {
		clif_changeoption(bl);
	}

	return 0;
}

/*==========================================
 * +cnlbmmdhTvZOCzBrMEowUTCLMLkwxjD8ML8wuXVwXjiJ45Zk-
 *------------------------------------------
 */
int status_change_release(struct block_list *bl,int mask)
{
	struct status_change *sc;
	int i;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

#ifdef DYNAMIC_SC_DATA
	if(sc->data == dummy_sc_data)
		return 0;
#endif

	if(sc->count <= 0)
		return 0;

	status_calc_pc_stop_begin(bl);
	for(i = 0; i < MAX_STATUSCHANGE; i+-+-) {
		// +dXBeODBMMEIwYzBmThQwZDDVMOkwsDBMMEIwizBqMIkwvzCkMN4w/DCSUkqWZA-
		if( (scdata_db[i].releasable & mask) && sc->data[i].timer != -1 ) {
			if(i == SC_DANCING) {
				skill_stop_dancing(bl,0);
			} else {
				if(i == SC_BASILICA)
					skill_basilica_cancel(bl);
				status_change_end(bl,i,-1);
			}
		}
	}
	status_calc_pc_stop_end(bl);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+a2ZWaDBuXF5gJw-)+fUJOhg-
 *------------------------------------------
 */
int status_enchant_elemental_end(struct block_list *bl,int type)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if( type != SC_ENCPOISON && sc->data[SC_ENCPOISON].timer != -1 )	/* +MKgw8zDBMOMw8zDIMN0wpDC6MPOJ45Zk- */
		status_change_end(bl,SC_ENCPOISON,-1);
	if( type != SC_ASPERSIO && sc->data[SC_ASPERSIO].timer != -1 )	/* +MKIwuTDaMOswtzCqieOWZA- */
		status_change_end(bl,SC_ASPERSIO,-1);
	if( type != SC_FLAMELAUNCHER && sc->data[SC_FLAMELAUNCHER].timer != -1 )	/* +MNUw7DCkMOAw6TDzMMEw44njlmQ- */
		status_change_end(bl,SC_FLAMELAUNCHER,-1);
	if( type != SC_FROSTWEAPON && sc->data[SC_FROSTWEAPON].timer != -1 )	/* +MNUw7TC5MMgwpjCnMN0w84njlmQ- */
		status_change_end(bl,SC_FROSTWEAPON,-1);
	if( type != SC_LIGHTNINGLOADER && sc->data[SC_LIGHTNINGLOADER].timer != -1 )	/* +MOkwpDDIMMsw8zCwMO0w/DDAMPyJ45Zk- */
		status_change_end(bl,SC_LIGHTNINGLOADER,-1);
	if( type != SC_SEISMICWEAPON && sc->data[SC_SEISMICWEAPON].timer != -1 )	/* +MLUwpDC5MN8wwzCvMKYwpzDdMPOJ45Zk- */
		status_change_end(bl,SC_SEISMICWEAPON,-1);
	if( type != SC_DARKELEMENT && sc->data[SC_DARKELEMENT].timer != -1 )		// +lcc-
		status_change_end(bl,SC_DARKELEMENT,-1);
	if( type != SC_ATTENELEMENT && sc->data[SC_ATTENELEMENT].timer != -1 )	// +X/U-
		status_change_end(bl,SC_ATTENELEMENT,-1);
	if( type != SC_UNDEADELEMENT && sc->data[SC_UNDEADELEMENT].timer != -1 )	// +Tg1rew-
		status_change_end(bl,SC_UNDEADELEMENT,-1);
	if( type != SC_SEVENWIND && sc->data[SC_SEVENWIND].timer != -1 )		/* +ZpYwSzBEmKiJ45Zk- */
		status_change_end(bl,SC_SEVENWIND,-1);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+T1MwblxeYCc-)+fUJOhg-
 *------------------------------------------
 */
int status_enchant_armor_elemental_end(struct block_list *bl,int type)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if( type != SC_BENEDICTIO && sc->data[SC_BENEDICTIO].timer != -1 )	// +gFZPUw-
		status_change_end(bl,SC_BENEDICTIO,-1);
	if( type != SC_ELEMENTWATER && sc->data[SC_ELEMENTWATER].timer != -1 )	// +bDQ-
		status_change_end(bl,SC_ELEMENTWATER,-1);
	if( type != SC_ELEMENTGROUND && sc->data[SC_ELEMENTGROUND].timer != -1 )	// +VzA-
		status_change_end(bl,SC_ELEMENTGROUND,-1);
	if( type != SC_ELEMENTWIND && sc->data[SC_ELEMENTWIND].timer != -1 )		// +mKg-
		status_change_end(bl,SC_ELEMENTWIND,-1);
	if( type != SC_ELEMENTFIRE && sc->data[SC_ELEMENTFIRE].timer != -1 )		// +cGs-
		status_change_end(bl,SC_ELEMENTFIRE,-1);
	if( type != SC_ELEMENTHOLY && sc->data[SC_ELEMENTHOLY].timer != -1 )	// +UUk-
		status_change_end(bl,SC_ELEMENTHOLY,-1);
	if( type != SC_ELEMENTDARK && sc->data[SC_ELEMENTDARK].timer != -1 )		// +lcc-
		status_change_end(bl,SC_ELEMENTDARK,-1);
	if( type != SC_ELEMENTELEKINESIS && sc->data[SC_ELEMENTELEKINESIS].timer != -1 )	// +X/U-
		status_change_end(bl,SC_ELEMENTELEKINESIS,-1);
	if( type != SC_ELEMENTPOISON && sc->data[SC_ELEMENTPOISON].timer != -1 )	// +a9I-
		status_change_end(bl,SC_ELEMENTPOISON,-1);
	if( type != SC_ELEMENTUNDEAD && sc->data[SC_ELEMENTUNDEAD].timer != -1 )	// +Tg1rew-
		status_change_end(bl,SC_ELEMENTUNDEAD,-1);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+ei5lz1kJZvQ-)+fUJOhg-
 *------------------------------------------
 */
int status_change_race_end(struct block_list *bl,int type)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if( type != SC_RACEUNDEAD && sc->data[SC_RACEUNDEAD].timer != -1 )
		status_change_end(bl,SC_RACEUNDEAD,-1);
	if( type != SC_RACEBEAST && sc->data[SC_RACEBEAST].timer != -1 )
		status_change_end(bl,SC_RACEBEAST,-1);
	if( type != SC_RACEPLANT && sc->data[SC_RACEPLANT].timer != -1 )
		status_change_end(bl,SC_RACEPLANT,-1);
	if( type != SC_RACEINSECT && sc->data[SC_RACEINSECT].timer != -1 )
		status_change_end(bl,SC_RACEINSECT,-1);
	if( type != SC_RACEFISH && sc->data[SC_RACEFISH].timer != -1 )
		status_change_end(bl,SC_RACEFISH,-1);
	if( type != SC_RACEDEVIL && sc->data[SC_RACEDEVIL].timer != -1 )
		status_change_end(bl,SC_RACEDEVIL,-1);
	if( type != SC_RACEHUMAN && sc->data[SC_RACEHUMAN].timer != -1 )
		status_change_end(bl,SC_RACEHUMAN,-1);
	if( type != SC_RACEANGEL && sc->data[SC_RACEANGEL].timer != -1 )
		status_change_end(bl,SC_RACEANGEL,-1);
	if( type != SC_RACEDRAGON && sc->data[SC_RACEDRAGON].timer != -1 )
		status_change_end(bl,SC_RACEDRAGON,-1);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+ei5lz1kJZvQ-)+fUJOhg-
 *------------------------------------------
 */
int status_change_resistclear(struct block_list *bl)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	status_calc_pc_stop_begin(bl);

	if(sc->data[SC_RESISTWATER].timer != -1)
		status_change_end(bl,SC_RESISTWATER,-1);
	if(sc->data[SC_RESISTGROUND].timer != -1)
		status_change_end(bl,SC_RESISTGROUND,-1);
	if(sc->data[SC_RESISTFIRE].timer != -1)
		status_change_end(bl,SC_RESISTFIRE,-1);
	if(sc->data[SC_RESISTWIND].timer != -1)
		status_change_end(bl,SC_RESISTWIND,-1);
	if(sc->data[SC_RESISTPOISON].timer != -1)
		status_change_end(bl,SC_RESISTPOISON,-1);
	if(sc->data[SC_RESISTHOLY].timer != -1)
		status_change_end(bl,SC_RESISTHOLY,-1);
	if(sc->data[SC_RESISTDARK].timer != -1)
		status_change_end(bl,SC_RESISTDARK,-1);
	if(sc->data[SC_RESISTTELEKINESIS].timer != -1)
		status_change_end(bl,SC_RESISTTELEKINESIS,-1);
	if(sc->data[SC_RESISTUNDEAD].timer != -1)
		status_change_end(bl,SC_RESISTUNDEAD,-1);

	status_calc_pc_stop_end(bl);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+m0I-)+lYtZyw-
 *------------------------------------------
 */
int status_change_soulstart(struct block_list *bl,int val1,int val2,int val3,int val4,int tick,int flag)
{
	int type = -1;
	struct map_session_data *sd;

	nullpo_retr(0, bl);

	if(bl->type != BL_PC || (sd = (struct map_session_data *)bl) == NULL)
		return 0;

	switch(sd->s_class.job) {
		case PC_JOB_MO:
		case PC_JOB_SR:
			type = SC_MONK;
			break;
		case PC_JOB_SG:
		case PC_JOB_SJ:
			type = SC_STAR;
			break;
		case PC_JOB_SA:
		case PC_JOB_SO:
			type = SC_SAGE;
		 	break;
		case PC_JOB_CR:
		case PC_JOB_LG:
			type = SC_CRUSADER;
			break;
		case PC_JOB_WZ:
		case PC_JOB_WL:
			type = SC_WIZARD;
			break;
		case PC_JOB_PR:
		case PC_JOB_AB:
			type = SC_PRIEST;
			break;
		case PC_JOB_RG:
		case PC_JOB_SC:
			type = SC_ROGUE;
			break;
		case PC_JOB_AS:
		case PC_JOB_GC:
			type = SC_ASSASIN;
			break;
		case PC_JOB_SL:
		case PC_JOB_SP:
			type = SC_SOULLINKER;
			break;
		case PC_JOB_KN:
		case PC_JOB_RK:
			type = SC_KNIGHT;
			break;
		case PC_JOB_AM:
		case PC_JOB_GN:
			type = SC_ALCHEMIST;
			break;
		case PC_JOB_BA:
		case PC_JOB_DC:
		case PC_JOB_MI:
		case PC_JOB_WA:
			type = SC_BARDDANCER;
			break;
		case PC_JOB_BS:
		case PC_JOB_NC:
			type = SC_BLACKSMITH;
			break;
		case PC_JOB_HT:
		case PC_JOB_RA:
			type = SC_HUNTER;
			break;
		case PC_JOB_SNV:
		case PC_JOB_ESNV:
			type = SC_SUPERNOVICE;
			break;
		case PC_JOB_GS:
			type = SC_GUNNER;
			break;
		case PC_JOB_NJ:
			type = SC_NINJA;
			break;
		case PC_JOB_DE:
			type = SC_DEATHKINGHT;
			break;
		case PC_JOB_DA:
			type = SC_COLLECTOR;
			break;
		default:
			if(sd->s_class.upper == PC_UPPER_HIGH && sd->s_class.job >= PC_JOB_SM && sd->s_class.job <= PC_JOB_TF)
				type = SC_HIGH;
			break;
	}
	if(type >= 0)
		status_change_start(bl,type,val1,val2,val3,val4,tick,flag);
	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+m0I-)+fUJOhg-
 *------------------------------------------
 */
int status_change_soulclear(struct block_list *bl)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if(sc->data[SC_MONK].timer != -1)
		status_change_end(bl,SC_MONK,-1);
	if(sc->data[SC_STAR].timer != -1)
		status_change_end(bl,SC_STAR,-1);
	if(sc->data[SC_SAGE].timer != -1)
		status_change_end(bl,SC_SAGE,-1);
	if(sc->data[SC_CRUSADER].timer != -1)
		status_change_end(bl,SC_CRUSADER,-1);
	if(sc->data[SC_WIZARD].timer != -1)
		status_change_end(bl,SC_WIZARD,-1);
	if(sc->data[SC_PRIEST].timer != -1)
		status_change_end(bl,SC_PRIEST,-1);
	if(sc->data[SC_ROGUE].timer != -1)
		status_change_end(bl,SC_ROGUE,-1);
	if(sc->data[SC_ASSASIN].timer != -1)
		status_change_end(bl,SC_ASSASIN,-1);
	if(sc->data[SC_SOULLINKER].timer != -1)
		status_change_end(bl,SC_SOULLINKER,-1);
	if(sc->data[SC_KNIGHT].timer != -1)
		status_change_end(bl,SC_KNIGHT,-1);
	if(sc->data[SC_ALCHEMIST].timer != -1)
		status_change_end(bl,SC_ALCHEMIST,-1);
	if(sc->data[SC_BARDDANCER].timer != -1)
		status_change_end(bl,SC_BARDDANCER,-1);
	if(sc->data[SC_BLACKSMITH].timer != -1)
		status_change_end(bl,SC_BLACKSMITH,-1);
	if(sc->data[SC_HUNTER].timer != -1)
		status_change_end(bl,SC_HUNTER,-1);
	if(sc->data[SC_HIGH].timer != -1)
		status_change_end(bl,SC_HIGH,-1);
	if(sc->data[SC_SUPERNOVICE].timer != -1)
		status_change_end(bl,SC_SUPERNOVICE,-1);
	if(sc->data[SC_GUNNER].timer != -1)
		status_change_end(bl,SC_GUNNER,-1);
	if(sc->data[SC_NINJA].timer != -1)
		status_change_end(bl,SC_NINJA,-1);
	if(sc->data[SC_DEATHKINGHT].timer != -1)
		status_change_end(bl,SC_DEATHKINGHT,-1);
	if(sc->data[SC_COLLECTOR].timer != -1)
		status_change_end(bl,SC_COLLECTOR,-1);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+fL6XCg-)+fUJOhg-
 *------------------------------------------
 */
int status_change_elemclear(struct block_list *bl)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if(sc->data[SC_CIRCLE_OF_FIRE].timer != -1)
		status_change_end(bl,SC_CIRCLE_OF_FIRE,-1);
	if(sc->data[SC_CIRCLE_OF_FIRE_OPTION].timer != -1)
		status_change_end(bl,SC_CIRCLE_OF_FIRE_OPTION,-1);
	if(sc->data[SC_FIRE_CLOAK].timer != -1)
		status_change_end(bl,SC_FIRE_CLOAK,-1);
	if(sc->data[SC_FIRE_CLOAK_OPTION].timer != -1)
		status_change_end(bl,SC_FIRE_CLOAK_OPTION,-1);
	if(sc->data[SC_WATER_SCREEN].timer != -1)
		status_change_end(bl,SC_WATER_SCREEN,-1);
	if(sc->data[SC_WATER_SCREEN_OPTION].timer != -1)
		status_change_end(bl,SC_WATER_SCREEN_OPTION,-1);
	if(sc->data[SC_WATER_DROP].timer != -1)
		status_change_end(bl,SC_WATER_DROP,-1);
	if(sc->data[SC_WATER_DROP_OPTION].timer != -1)
		status_change_end(bl,SC_WATER_DROP_OPTION,-1);
	//if(sc->data[SC_WATER_BARRIER].timer != -1)
	//	status_change_end(bl,SC_WATER_BARRIER,-1);
	if(sc->data[SC_WIND_STEP].timer != -1)
		status_change_end(bl,SC_WIND_STEP,-1);
	if(sc->data[SC_WIND_STEP_OPTION].timer != -1)
		status_change_end(bl,SC_WIND_STEP_OPTION,-1);
	if(sc->data[SC_WIND_CURTAIN].timer != -1)
		status_change_end(bl,SC_WIND_CURTAIN,-1);
	if(sc->data[SC_WIND_CURTAIN_OPTION].timer != -1)
		status_change_end(bl,SC_WIND_CURTAIN_OPTION,-1);
	//if(sc->data[SC_ZEPHYR].timer != -1)
	//	status_change_end(bl,SC_ZEPHYR,-1);
	if(sc->data[SC_SOLID_SKIN].timer != -1)
		status_change_end(bl,SC_SOLID_SKIN,-1);
	if(sc->data[SC_SOLID_SKIN_OPTION].timer != -1)
		status_change_end(bl,SC_SOLID_SKIN_OPTION,-1);
	if(sc->data[SC_STONE_SHIELD].timer != -1)
		status_change_end(bl,SC_STONE_SHIELD,-1);
	if(sc->data[SC_STONE_SHIELD_OPTION].timer != -1)
		status_change_end(bl,SC_STONE_SHIELD_OPTION,-1);
	//if(sc->data[SC_POWER_OF_GAIA].timer != -1)
	//	status_change_end(bl,SC_POWER_OF_GAIA,-1);
	if(sc->data[SC_PYROTECHNIC].timer != -1)
		status_change_end(bl,SC_PYROTECHNIC,-1);
	if(sc->data[SC_PYROTECHNIC_OPTION].timer != -1)
		status_change_end(bl,SC_PYROTECHNIC_OPTION,-1);
	if(sc->data[SC_HEATER].timer != -1)
		status_change_end(bl,SC_HEATER,-1);
	if(sc->data[SC_HEATER_OPTION].timer != -1)
		status_change_end(bl,SC_HEATER_OPTION,-1);
	if(sc->data[SC_TROPIC].timer != -1)
		status_change_end(bl,SC_TROPIC,-1);
	if(sc->data[SC_TROPIC_OPTION].timer != -1)
		status_change_end(bl,SC_TROPIC_OPTION,-1);
	if(sc->data[SC_AQUAPLAY].timer != -1)
		status_change_end(bl,SC_AQUAPLAY,-1);
	if(sc->data[SC_AQUAPLAY_OPTION].timer != -1)
		status_change_end(bl,SC_AQUAPLAY_OPTION,-1);
	if(sc->data[SC_COOLER].timer != -1)
		status_change_end(bl,SC_COOLER,-1);
	if(sc->data[SC_COOLER_OPTION].timer != -1)
		status_change_end(bl,SC_COOLER_OPTION,-1);
	if(sc->data[SC_CHILLY_AIR].timer != -1)
		status_change_end(bl,SC_CHILLY_AIR,-1);
	if(sc->data[SC_CHILLY_AIR_OPTION].timer != -1)
		status_change_end(bl,SC_CHILLY_AIR_OPTION,-1);
	if(sc->data[SC_GUST].timer != -1)
		status_change_end(bl,SC_GUST,-1);
	if(sc->data[SC_GUST_OPTION].timer != -1)
		status_change_end(bl,SC_GUST_OPTION,-1);
	if(sc->data[SC_BLAST].timer != -1)
		status_change_end(bl,SC_BLAST,-1);
	if(sc->data[SC_BLAST_OPTION].timer != -1)
		status_change_end(bl,SC_BLAST_OPTION,-1);
	if(sc->data[SC_WILD_STORM].timer != -1)
		status_change_end(bl,SC_WILD_STORM,-1);
	if(sc->data[SC_WILD_STORM_OPTION].timer != -1)
		status_change_end(bl,SC_WILD_STORM_OPTION,-1);
	if(sc->data[SC_PETROLOGY].timer != -1)
		status_change_end(bl,SC_PETROLOGY,-1);
	if(sc->data[SC_PETROLOGY_OPTION].timer != -1)
		status_change_end(bl,SC_PETROLOGY_OPTION,-1);
	if(sc->data[SC_CURSED_SOIL].timer != -1)
		status_change_end(bl,SC_CURSED_SOIL,-1);
	if(sc->data[SC_CURSED_SOIL_OPTION].timer != -1)
		status_change_end(bl,SC_CURSED_SOIL_OPTION,-1);
	if(sc->data[SC_UPHEAVAL].timer != -1)
		status_change_end(bl,SC_UPHEAVAL,-1);
	if(sc->data[SC_UPHEAVAL_OPTION].timer != -1)
		status_change_end(bl,SC_UPHEAVAL_OPTION,-1);
	if(sc->data[SC_TIDAL_WEAPON].timer != -1)
		status_change_end(bl,SC_TIDAL_WEAPON,-1);
	if(sc->data[SC_TIDAL_WEAPON_OPTION].timer != -1)
		status_change_end(bl,SC_TIDAL_WEAPON_OPTION,-1);
	//if(sc->data[SC_ROCK_CRUSHER].timer != -1)
	//	status_change_end(bl,SC_ROCK_CRUSHER,-1);
	//if(sc->data[SC_ROCK_CRUSHER_ATK].timer != -1)
	//	status_change_end(bl,SC_ROCK_CRUSHER_ATK,-1);
	if(sc->data[SC_FLAMETECHNIC].timer != -1)
		status_change_end(bl,SC_FLAMETECHNIC,-1);
	if(sc->data[SC_FLAMETECHNIC_OPTION].timer != -1)
		status_change_end(bl,SC_FLAMETECHNIC_OPTION,-1);
	if(sc->data[SC_FLAMEARMOR].timer != -1)
		status_change_end(bl,SC_FLAMEARMOR,-1);
	if(sc->data[SC_FLAMEARMOR_OPTION].timer != -1)
		status_change_end(bl,SC_FLAMEARMOR_OPTION,-1);
	if(sc->data[SC_COLD_FORCE].timer != -1)
		status_change_end(bl,SC_COLD_FORCE,-1);
	if(sc->data[SC_COLD_FORCE_OPTION].timer != -1)
		status_change_end(bl,SC_COLD_FORCE_OPTION,-1);
	if(sc->data[SC_CRYSTAL_ARMOR].timer != -1)
		status_change_end(bl,SC_CRYSTAL_ARMOR,-1);
	if(sc->data[SC_CRYSTAL_ARMOR_OPTION].timer != -1)
		status_change_end(bl,SC_CRYSTAL_ARMOR_OPTION,-1);
	if(sc->data[SC_GRACE_BREEZE].timer != -1)
		status_change_end(bl,SC_GRACE_BREEZE,-1);
	if(sc->data[SC_GRACE_BREEZE_OPTION].timer != -1)
		status_change_end(bl,SC_GRACE_BREEZE_OPTION,-1);
	if(sc->data[SC_EYES_OF_STORM].timer != -1)
		status_change_end(bl,SC_EYES_OF_STORM,-1);
	if(sc->data[SC_EYES_OF_STORM_OPTION].timer != -1)
		status_change_end(bl,SC_EYES_OF_STORM_OPTION,-1);
	if(sc->data[SC_EARTH_CARE].timer != -1)
		status_change_end(bl,SC_EARTH_CARE,-1);
	if(sc->data[SC_EARTH_CARE_OPTION].timer != -1)
		status_change_end(bl,SC_EARTH_CARE_OPTION,-1);
	if(sc->data[SC_STRONG_PROTECTION].timer != -1)
		status_change_end(bl,SC_STRONG_PROTECTION,-1);
	if(sc->data[SC_STRONG_PROTECTION_OPTION].timer != -1)
		status_change_end(bl,SC_STRONG_PROTECTION_OPTION,-1);
	if(sc->data[SC_DEEP_POISONING].timer != -1)
		status_change_end(bl,SC_DEEP_POISONING,-1);
	if(sc->data[SC_DEEP_POISONING_OPTION].timer != -1)
		status_change_end(bl,SC_DEEP_POISONING_OPTION,-1);
	if(sc->data[SC_POISON_SHIELD].timer != -1)
		status_change_end(bl,SC_POISON_SHIELD,-1);
	if(sc->data[SC_POISON_SHIELD_OPTION].timer != -1)
		status_change_end(bl,SC_POISON_SHIELD_OPTION,-1);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+Uc19UDD7d/NTFjD7d2F3IA-)+fUJOhg-
 *------------------------------------------
 */
int status_change_attacked_end(struct block_list *bl)
{
	struct status_change *sc;

	nullpo_retr(0, bl);
	nullpo_retr(0, sc = status_get_sc(bl));

	if(sc->data[SC_FREEZE].timer != -1)
		status_change_end(bl,SC_FREEZE,-1);
	if(sc->data[SC_STONE].timer != -1 && sc->data[SC_STONE].val2 == 0)
		status_change_end(bl,SC_STONE,-1);
	if(sc->data[SC_SLEEP].timer != -1)
		status_change_end(bl,SC_SLEEP,-1);
	if(sc->data[SC_WINKCHARM].timer != -1)
		status_change_end(bl,SC_WINKCHARM,-1);
	if(sc->data[SC_DEEP_SLEEP].timer != -1)
		status_change_end(bl,SC_DEEP_SLEEP,-1);
	if(sc->data[SC_SIREN].timer != -1)
		status_change_end(bl,SC_SIREN,-1);
	if(sc->data[SC_HANDICAPSTATE_FROSTBITE].timer != -1)
		status_change_end(bl,SC_HANDICAPSTATE_FROSTBITE,-1);
	if(sc->data[SC_HANDICAPSTATE_SWOONING].timer != -1)
		status_change_end(bl,SC_HANDICAPSTATE_SWOONING,-1);
	if(sc->data[SC_HANDICAPSTATE_LIGHTNINGSTRIKE].timer != -1)
		status_change_end(bl,SC_HANDICAPSTATE_LIGHTNINGSTRIKE,-1);
	if(sc->data[SC_HANDICAPSTATE_CRYSTALLIZATION].timer != -1)
		status_change_end(bl,SC_HANDICAPSTATE_CRYSTALLIZATION,-1);

	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+MM8wpDDJ-)+fUJOhg-
 *------------------------------------------
 */
int status_change_hidden_end(struct block_list *bl)
{
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);

	if(sc) {
		if(sc->option > 0) {
			if(sc->option & OPTION_HIDE)
				status_change_end(bl,SC_HIDING,-1);
			if((sc->option & (OPTION_CLOAKING | OPTION_FOOTPRINT)) == OPTION_CLOAKING)
				status_change_end(bl,SC_CLOAKING,-1);
			if((sc->option & (OPTION_CLOAKING | OPTION_FOOTPRINT)) == (OPTION_CLOAKING | OPTION_FOOTPRINT))
				status_change_end(bl,SC_CHASEWALK,-1);
		}
	 	if(sc->data[SC_CAMOUFLAGE].timer != -1)
			status_change_end(bl,SC_CAMOUFLAGE,-1);
	 	if(sc->data[SC_STEALTHFIELD].timer != -1)
			status_change_end(bl,SC_STEALTHFIELD,-1);
	 	if(sc->data[SC_SUHIDE].timer != -1)
			status_change_end(bl,SC_SUHIDE,-1);
	 	if(sc->data[SC_NEWMOON].timer != -1)
			status_change_end(bl,SC_NEWMOON,-1);
	}
	return 0;
}

/*==========================================
 * +MLkwxjD8ML8wuXVwXjg-(+MN4wwzDXluKBMWZC-)+fUJOhg-
 *------------------------------------------
 */
int status_change_removemap_end(struct block_list *bl)
{
	struct status_change *sc;

	nullpo_retr(0, bl);

	sc = status_get_sc(bl);
	if(sc == NULL || sc->count <= 0)
		return 0;

	status_calc_pc_stop_begin(bl);

	if(sc->data[SC_BLADESTOP].timer != -1)
		status_change_end(bl, SC_BLADESTOP, -1);
	if(sc->data[SC_GRAFFITI].timer != -1)
		status_change_end(bl, SC_GRAFFITI, -1);
	if(sc->data[SC_ANKLE].timer != -1)
		status_change_end(bl, SC_ANKLE, -1);
	if(sc->data[SC_GOSPEL].timer != -1)
		status_change_end(bl, SC_GOSPEL, -1);
	if(sc->data[SC_GRAVITATION_USER].timer != -1)
		status_change_end(bl, SC_GRAVITATION_USER, -1);
	if(sc->data[SC_TRICKDEAD].timer != -1)
		status_change_end(bl, SC_TRICKDEAD, -1);
	if(sc->data[SC_RUN].timer != -1)
		status_change_end(bl, SC_RUN, -1);
	if(sc->data[SC_MARIONETTE].timer != -1)
		status_change_end(bl, SC_MARIONETTE, -1);
	if(sc->data[SC_MARIONETTE2].timer != -1)
		status_change_end(bl, SC_MARIONETTE2, -1);
	if(sc->data[SC_BASILICA].timer != -1) {
		skill_basilica_cancel(bl);
		status_change_end(bl, SC_BASILICA, -1);
	}
	if(sc->data[SC_WUGDASH].timer != -1)
		status_change_end(bl, SC_WUGDASH, -1);

	if(battle_config.homun_statuschange_reset) {
		if(sc->data[SC_AVOID].timer != -1)
			status_change_end(bl, SC_AVOID, -1);
		if(sc->data[SC_CHANGE].timer != -1)
			status_change_end(bl, SC_CHANGE, -1);
		if(sc->data[SC_DEFENCE].timer != -1)
			status_change_end(bl, SC_DEFENCE, -1);
		if(sc->data[SC_BLOODLUST].timer != -1)
			status_change_end(bl, SC_BLOODLUST, -1);
		if(sc->data[SC_FLEET].timer != -1)
			status_change_end(bl, SC_FLEET, -1);
		if(sc->data[SC_SPEED].timer != -1)
			status_change_end(bl, SC_SPEED, -1);
	}
	status_calc_pc_stop_end(bl);

	return 0;
}

/*==========================================
 * +MKIwpDDGMOAwazCIMIswuTDGMPwwvzC5dXBeOJWLWcs-
 *------------------------------------------
 */
int status_change_addeff_start(struct block_list *src, struct block_list *bl, int id, int rate, int type, unsigned int tick)
{
	struct map_session_data *sd = NULL;

	static const int sc2[] = {
		MG_STONECURSE,MG_FROSTDIVER,NPC_STUNATTACK,
		NPC_SLEEPATTACK,TF_POISON,NPC_CURSEATTACK,
		NPC_SILENCEATTACK,0,NPC_BLINDATTACK,LK_HEADCRUSH,
		NPC_WIDEHEALTHFEAR,NPC_WIDEBODYBURNNING,NPC_WIDE_DEEP_SLEEP,
		NPC_WIDEFROSTMISTY,NPC_WIDECOLD,NPC_WIDESIREN
	};
	static const int sc_id[] = {
		SC_STONE,SC_FREEZE,SC_STUN,
		SC_SLEEP,SC_POISON,SC_CURSE,
		SC_SILENCE,SC_CONFUSION,SC_BLIND,SC_BLEED,
		SC_FEAR,SC_HELLINFERNO,SC_DEEP_SLEEP,
		SC_FROSTMISTY,SC_DIAMONDDUST,SC_SIREN
	};

	nullpo_retr(0, src);
	nullpo_retr(0, bl);

	if(src->type != BL_PC || !(bl->type & BL_CHAR))
		return 0;

	sd = (struct map_session_data *)src;

	if(atn_rand() % 10000 < status_change_rate(bl,sc_id[id],rate,sd->status.base_level)) {

		if(battle_config.battle_log) {
			if(type==1)
				printf("PC %d skill_skilladdeff: card+MGswiDCLcrZhS3VwXjh2elLV- %d %d %d+AFw-n",sd->bl.id,sd->skill_addeff.id[id],id,rate);
			else if(type==2)
				printf("PC %d skill_addeff: card+MGswiDCLcrZhS3VwXjh2elLV- %d %d+AFw-n",sd->bl.id,id,rate);
			else if(type==3)
				printf("PC %d skill_addeff2: card+MGswiDCLcrZhS3VwXjh2elLV- %d %d+AFw-n",sd->bl.id,id,rate);
			else if(type==4)
				printf("PC %d magic_addeff: card+MGswiDCLcrZhS3VwXjh2elLV- %d %d+AFw-n",sd->bl.id,id,rate);
			else
				printf("PC %d skill_addreveff: card+MGswiDCLdXBeOHZ6UtU- %d %d+AFw-n",sd->bl.id,id,sd->addreveff[id]);
		}

		switch(sc_id[id]) {
			case SC_STONE:
			case SC_FREEZE:
			case SC_STUN:
			case SC_SLEEP:
			case SC_POISON:
			case SC_CURSE:
			case SC_SILENCE:
			case SC_CONFUSION:
			case SC_BLIND:
			case SC_BLEED:
				if(type)
					status_change_pretimer(bl,sc_id[id],7,0,0,0,((sc_id[id] == SC_CONFUSION)? 10000+-7000: skill_get_time2(sc2[id],7)),0,tick+-status_get_amotion(src)+-500);
				else
					status_change_start(bl,sc_id[id],7,0,0,0,((sc_id[id] == SC_CONFUSION)? 10000+-7000: skill_get_time2(sc2[id],7)),0);
				break;
			case SC_FEAR:
				if(type)
					status_change_pretimer(bl,sc_id[id],1,0,0,0,skill_get_time2(sc2[id],1),0,tick+-status_get_amotion(src)+-500);
				else
					status_change_start(bl,sc_id[id],1,0,0,0,skill_get_time2(sc2[id],1),0);
				break;
			case SC_FROSTMISTY:
			case SC_HELLINFERNO:
				if(type)
					status_change_pretimer(bl,sc_id[id],1,0,0,0,10000+-skill_get_time2(sc2[id],1),0,tick+-status_get_amotion(src)+-500);
				else
					status_change_start(bl,sc_id[id],1,0,0,0,10000+-skill_get_time2(sc2[id],1),0);
				break;
			case SC_DEEP_SLEEP:
			case SC_DIAMONDDUST:
				if(type)
					status_change_pretimer(bl,sc_id[id],1,0,0,0,3000+-skill_get_time2(sc2[id],1),0,tick+-status_get_amotion(src)+-500);
				else
					status_change_start(bl,sc_id[id],1,0,0,0,3000+-skill_get_time2(sc2[id],1),0);
				break;
			case SC_SIREN:
				if(type)
					status_change_pretimer(bl,sc_id[id],1,src->id,0,0,skill_get_time2(sc2[id],1),0,tick+-status_get_amotion(src)+-500);
				else
					status_change_start(bl,sc_id[id],1,src->id,0,0,skill_get_time2(sc2[id],1),0);
				break;
		}
	}
	return 0;
}

/*==========================================
 *
 *------------------------------------------
 */
static void status_split_atoi(char *str, int *num1, int *num2)
{
	int i, val[2];

	for (i=0; i<2; i+-+-) {
		if(str) {
			val[i] = atoi(str);
			str = strchr(str,':');
			if (str)
				*str+-+-=0;
		} else {
			val[i] = 0;
		}
	}
	if(val[0])
		*num1 = val[0];
	if(val[1])
		*num2 = val[1];
	return;
}

/*==========================================
 *
 *------------------------------------------
 */
static int status_split_atoi2(char *str,int *val,int num)
{
	int i, max = 0;

	for (i=0; i<num; i+-+-) {
		if (str) {
			val[i] = max = atoi(str);
			str = strchr(str,':');
			if (str)
				*str+-+-=0;
		} else {
			val[i] = max;
		}
	}
	return i;
}

/*==========================================
 * +MMcw/DC/MNkw/DC5iq0wf4+8MH8-
 *------------------------------------------
 */
int status_readdb(void)
{
	int i,j,k,m;
	FILE *fp;
	char line[1024],*p;
#ifdef PRE_RENEWAL
	const char *filename[] = {
		"db/job_db1.txt", "db/pre/job_db1_pre.txt", "db/addon/job_db1_add.txt"
	};
	const int max = 3;
#else
	const char *filename[] = {
		"db/job_db1.txt", "db/addon/job_db1_add.txt"
	};
	const int max = 2;
#endif
	const char *filename2;

	memset(&job_db, 0, sizeof(job_db));

	// JOB+iNxrY2VwUCQ-1
	for(m = 0; m < max; m+-+-) {
		fp = fopen(filename[m], "r");
		if(fp == NULL) {
			if(m > 0)
				continue;
			printf("status_readdb: open [%s] failed !+AFw-n", filename[m]);
			return 1;
		}

		while(fgets(line,1020,fp)){
			char *split[10+-WT_MAX];
			int hp_coefficient, sp_coefficient;
			int hp_coefficient2, sigma;

			if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
				continue;
			if(line[0]=='/' && line[1]=='/')
				continue;
			memset(split,0,sizeof(split));
			for(j=0,p=line;j<9+-WT_MAX && p;j+-+-){
				split[j]=p;
				p=strchr(p,',');
				if(p) *p+-+-=0;
			}
			if(j < 9)
				continue;

			i = atoi(split[0]);
			if(i < 0 || i >= PC_JOB_MAX)
				continue;

			status_split_atoi2(split[1],job_db[i].max_joblv,PC_UPPER_MAX);
			status_split_atoi2(split[2],job_db[i].base_exp_table,PC_UPPER_MAX);
			status_split_atoi2(split[3],job_db[i].job_exp_table,PC_UPPER_MAX);
			job_db[i].max_weight_base = atoi(split[4]);
			if((hp_coefficient = atoi(split[5])) < 0)
				hp_coefficient = 0;
			if((hp_coefficient2 = atoi(split[6])) < 0)
				hp_coefficient2 = 500;
			sigma = 0;
			for(j = 1; j <= MAX_LEVEL; j+-+-) {
				// +V/pnLA-HP = 35 +- BaseLevel * Job+UA1zhw- +- Job+MNww/DDKMLk-
				job_db[i].hp_base[j-1] = (3500 +- j * hp_coefficient2 +- sigma) / 100;
				sigma +-= hp_coefficient * (j +- 1) +- 50;
				sigma -= sigma % 100;
			}

			if((sp_coefficient = atoi(split[7])) < 0)
				sp_coefficient = 100;
			for(j = 1; j <= MAX_LEVEL; j+-+-) {
				// +V/pnLA-SP = 10 +- BaseLevel * Job+T8JlcA-
				job_db[i].sp_base[j-1] = (1000 +- j * sp_coefficient) / 100;
			}

			job_db[i].max_ap = atoi(split[8]);

			for(j=0; j<=WT_MAX && split[9+-j]; j+-+-) {
				job_db[i].aspd_base[j] = atoi(split[9+-j]);
			}
		}
		fclose(fp);
		printf("read %s done+AFw-n", filename[m]);
	}

	// +V/pnLA-HP+UAtSJYotW5o-
	filename2 = "db/job_hp_db.txt";
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0]=='/' && line[1]=='/')
			continue;
		for(j=0,p=line;j<PC_JOB_MAX && p;j+-+-){
			if(sscanf(p,"%d",&k) == 0)
				break;
			if(k > 0) {
				job_db[j].hp_base[i] = k;
			}
			p=strchr(p,',');
			if(p) *p+-+-=0;
		}
		if(+-+-i >= MAX_LEVEL)
			break;
	}
	fclose(fp);
	printf("read %s done+AFw-n", filename2);

	// +V/pnLA-SP+UAtSJYotW5o-
	filename2 = "db/job_sp_db.txt";
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0]=='/' && line[1]=='/')
			continue;
		for(j=0,p=line;j<PC_JOB_MAX && p;j+-+-){
			if(sscanf(p,"%d",&k) == 0)
				break;
			if(k > 0) {
				job_db[j].sp_base[i] = k;
			}
			p=strchr(p,',');
			if(p) *p+-+-=0;
		}
		if(+-+-i >= MAX_LEVEL)
			break;
	}
	fclose(fp);
	printf("read %s done+AFw-n", filename2);

	// JOB+MNww/DDKMLk-
	filename2 = "db/job_db2.txt";
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0]=='/' && line[1]=='/')
			continue;
		for(j=0,p=line;j<MAX_LEVEL && p;j+-+-){
			if(sscanf(p,"%d",&k)==0)
				break;
			job_db[i].bonus[PC_UPPER_NORMAL][j] = k;
			job_db[i].bonus[PC_UPPER_BABY][j] = k;
			p=strchr(p,',');
			if(p) p+-+-;
		}
		if(+-+-i >= PC_JOB_MAX)
			break;
	}
	fclose(fp);
	printf("read %s done+AFw-n", filename2);

	// JOB+MNww/DDKMLk-2 +juJ1H4B3dSg-
	filename2 = "db/job_db2-2.txt";
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0]=='/' && line[1]=='/')
			continue;
		for(j=0,p=line;j<MAX_LEVEL && p;j+-+-){
			if(sscanf(p,"%d",&k)==0)
				break;
			job_db[i].bonus[PC_UPPER_HIGH][j] = k;
			p=strchr(p,',');
			if(p) p+-+-;
		}
		if(+-+-i >= PC_JOB_MAX)
			break;
	}
	fclose(fp);
	printf("read %s done+AFw-n", filename2);

	// +fL6TLDDHMPwwvzDGMPww1jDr-
	for(i=0; i<MAX_WEAPON_LEVEL+-1; i+-+-) {
		for(j=0; j<MAX_REFINE; j+-+-)
			refine_db[i].safety_bonus[j] = 0;
		refine_db[i].over_bonus   = 0;
		refine_db[i].limit        = MAX_REFINE;
		for(j=0; j<MAX_REFINE; j+-+-)
			refine_db[i].per[j] = 0;
	}

#ifdef PRE_RENEWAL
	filename2 = "db/pre/refine_db_pre.txt";
#else
	filename2 = "db/refine_db.txt";
#endif
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		char *split[MAX_REFINE+-3];
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0]=='/' && line[1]=='/')
			continue;
		if(atoi(line) <= 0)
			continue;
		memset(split,0,sizeof(split));
		for(j=0,p=line;j<MAX_REFINE+-3 && p;j+-+-){
			split[j]=p;
			p=strchr(p,',');
			if(p) *p+-+-=0;
		}
		for(j=0; j<MAX_REFINE; j+-+-) {
			refine_db[i].safety_bonus[j] = atoi(split[0]) * (j+-1);	// +fL6TLDDcMPwwyjC5-
		}
		refine_db[i].over_bonus   = atoi(split[1]);	// +kE5ScHy+kyww3DD8MMowuQ-
		refine_db[i].limit        = atoi(split[2]);	// +W4lRaHy+kyyWUHVM-
		for(j=0; j<MAX_REFINE && split[j+-3]; j+-+-) {
			status_split_atoi(split[j+-3], &refine_db[i].per[j], &refine_db[i].safety_bonus[j]);
		}
		if(+-+-i > MAX_WEAPON_LEVEL)
			break;
	}
	fclose(fp);
	printf("read %s done+AFw-n", filename2);

	// +MLUwpDC6iNxrYzDGMPww1jDr-
	for(i=0; i<MAX_SIZE_FIX; i+-+-) {
		for(j=0; j<WT_MAX; j+-+-)
			atkmods[i][j] = 100;
	}

	filename2 = "db/size_fix.txt";
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		char *split[WT_MAX];
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0]=='/' && line[1]=='/')
			continue;
		if(atoi(line) <= 0)
			continue;
		memset(split,0,sizeof(split));
		for(j=0,p=line;j<WT_MAX && p;j+-+-){
			split[j]=p;
			p=strchr(p,',');
			if(p) *p+-+-=0;
		}
		for(j=0; j<WT_MAX && split[j]; j+-+-) {
			atkmods[i][j] = atoi(split[j]);
		}
		if(+-+-i > MAX_SIZE_FIX)
			break;
	}
	fclose(fp);
	printf("read %s done+AFw-n", filename2);

	// +MLkwxjD8ML8wuXVwXjgwxjD8MNYw6w-
	memset(&scdata_db, 0, sizeof(scdata_db));
	filename2 = "db/scdata_db.txt";
	fp = fopen(filename2, "r");
	if(fp == NULL) {
		printf("status_readdb: open [%s] failed !+AFw-n", filename2);
		return 1;
	}
	i=0;
	while(fgets(line,1020,fp)){
		char *split[6];
		if(line[0] == '+AFw-0' || line[0] == '+AFw-r' || line[0] == '+AFw-n')
			continue;
		if(line[0] == '/' && line[1] == '/')
			continue;
		memset(split,0,sizeof(split));
		for(j=0,p=line;j<6 && p;j+-+-){
			split[j]=p;
			p=strchr(p,',');
			if(p) *p+-+-=0;
		}
		j = atoi(split[0]);
		if(j < 0 || j >= MAX_STATUSCHANGE)
			continue;
		scdata_db[j].save       = (short)atoi(split[2]);
		scdata_db[j].releasable = atoi(split[3]);
		scdata_db[j].disable    = atoi(split[4]);
		scdata_db[j].copyable   = atoi(split[5]);
		i+-+-;
	}
	fclose(fp);
	printf("read %s done (count=%d)+AFw-n", filename2, i);

#ifdef DYNAMIC_SC_DATA
	for(i=0; i<MAX_STATUSCHANGE; i+-+-) {
		dummy_sc_data[i].timer = -1;
		dummy_sc_data[i].val1  = 0;
		dummy_sc_data[i].val2  = 0;
		dummy_sc_data[i].val3  = 0;
		dummy_sc_data[i].val4  = 0;
	}
	printf("status_readdb: enable dynamic sc_data.+AFw-n");
#endif

	return 0;
}

/*==========================================
 * +MLkwrTDrlaJPwlIdZx9TFlHmdAY-
 *------------------------------------------
 */
int do_init_status(void)
{
	status_readdb();

	add_timer_func_list(status_change_timer);
	add_timer_func_list(status_pretimer_timer);

	return 0;
}
