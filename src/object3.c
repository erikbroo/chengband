#include "angband.h"

#include <math.h>

/* New code for object values.  Designed by Dave.
   p is price
   q is ???
   y is ???
   w is base weapon cost
   a is base armor cost

   Anyway, the spec is here: http://code.google.com/p/chengband/wiki/ArtifactScoring
   I named variables to match.
*/

debug_hook cost_calc_hook = NULL;

static s32b _avg_dam(object_type *o_ptr)
{
	return o_ptr->dd * (o_ptr->ds + 1)/2;
}

static double _calc_cost(double cost, int count)
{
	/* It gets harder to add new stuff to an enchanted item */
	return cost * (1.0 + count/10.0);
}

static double _check_flag_and_score(u32b flgs[TR_FLAG_SIZE], u32b flg, u32b score, int *count)
{
	double result = 0.0;
	if (have_flag(flgs, flg))
	{
		(*count)++;
		result += _calc_cost(score, *count);
	}
	return result;
}

static s32b _activation_p(object_type *o_ptr)
{
	/* Wouldn't it be nice if we had a system for activations?  (Other than
	   gigantic switch statements, that is) ... Sigh */
	if (o_ptr->art_name && (have_flag(o_ptr->art_flags, TR_ACTIVATE)))
	{
		switch (o_ptr->xtra2)
		{
		case ACT_WIZ_LITE: return 20000;
		case ACT_SUNLIGHT: return 250;
		case ACT_BO_MISS_1: return 250;
		case ACT_BA_POIS_1: return 300;
		case ACT_BO_ELEC_1: return 250;
		case ACT_BO_ACID_1: return 250;
		case ACT_BO_COLD_1: return 250;
		case ACT_BO_FIRE_1: return 250;
		case ACT_BA_COLD_1: return 750;
		case ACT_BA_FIRE_1: return 1000;
		case ACT_DRAIN_1: return 500;
		case ACT_BA_COLD_2: return 1250;
		case ACT_BA_ELEC_2: return 1500;
		case ACT_DRAIN_2: return 750;
		case ACT_VAMPIRE_1: return 1000;
		case ACT_BO_MISS_2: return 1000;
		case ACT_BA_FIRE_2: return 1750;
		case ACT_BA_COLD_3: return 2500;
		case ACT_BA_ELEC_3: return 2500;
		case ACT_WHIRLWIND: return 7500;
		case ACT_VAMPIRE_2: return 2500;
		case ACT_CALL_CHAOS: return 5000;
		case ACT_ROCKET: return 5000;
		case ACT_DISP_EVIL: return 4000;
		case ACT_DISP_GOOD: return 3500;
		case ACT_BA_MISS_3: return 5000;
		case ACT_CONFUSE: return 500;
		case ACT_SLEEP: return 750;
		case ACT_QUAKE: return 600;
		case ACT_TERROR: return 2500;
		case ACT_TELE_AWAY: return 2000;
		case ACT_BANISH_EVIL: return 2000;
		case ACT_GENOCIDE: return 10000;
		case ACT_MASS_GENO: return 10000;
		case ACT_CHARM_ANIMAL: return 7500;
		case ACT_CHARM_UNDEAD: return 10000;
		case ACT_CHARM_OTHER: return 10000;
		case ACT_CHARM_ANIMALS: return 12500;
		case ACT_CHARM_OTHERS: return 17500;
		case ACT_SUMMON_ANIMAL: return 10000;
		case ACT_SUMMON_PHANTOM: return 12000;
		case ACT_SUMMON_ELEMENTAL: return 15000;
		case ACT_SUMMON_DEMON: return 20000;
		case ACT_SUMMON_UNDEAD: return 20000;
		case ACT_CURE_LW: return 500;
		case ACT_CURE_MW: return 750;
		case ACT_CURE_POISON: return 1000;
		case ACT_REST_LIFE: return 7500;
		case ACT_REST_ALL: return 15000;
		case ACT_CURE_700: return 10000;
		case ACT_CURE_1000: return 15000;
		case ACT_ESP: return 1500;
		case ACT_BERSERK: return 800;
		case ACT_PROT_EVIL: return 5000;
		case ACT_RESIST_ALL: return 5000;
		case ACT_SPEED: return 15000;
		case ACT_XTRA_SPEED: return 25000;
		case ACT_WRAITH: return 25000;
		case ACT_INVULN: return 25000;
		case ACT_LIGHT: return 150;
		case ACT_MAP_LIGHT: return 500;
		case ACT_DETECT_ALL: return 1000;
		case ACT_DETECT_XTRA: return 12500;
		case ACT_ID_FULL: return 10000;
		case ACT_ID_PLAIN: return 1250;
		case ACT_RUNE_EXPLO: return 4000;
		case ACT_RUNE_PROT: return 10000;
		case ACT_SATIATE: return 2000;
		case ACT_DEST_DOOR: return 100;
		case ACT_STONE_MUD: return 1000;
		case ACT_RECHARGE: return 1000;
		case ACT_ALCHEMY: return 10000;
		case ACT_DIM_DOOR: return 10000;
		case ACT_TELEPORT: return 2000;
		case ACT_RECALL: return 7500;
		}
	}
	else if (object_is_fixed_artifact(o_ptr))
	{
		switch (o_ptr->name1)
		{
		case ART_GALADRIEL: return 1500;
		case ART_STONE_LORE: return 5000;
		case ART_ELENDIL: return 5000;
		case ART_JUDGE: return 20000;
		case ART_CARLAMMAS: return 10000;
		case ART_INGWE: return 2000;
		case ART_YATA: return 2000;
		case ART_FRAKIR: return 1000;
		case ART_TULKAS: return 15000;
		case ART_NARYA: return 10000;
		case ART_NENYA: return 10000;
		case ART_VILYA: return 12000;
		case ART_GOURYU: return 12000;
		case ART_POWER: return 20000;
		case ART_AHO: return 20000;
		case ART_RAZORBACK: return 10000;
		case ART_BLADETURNER: return 25000;
		case ART_SOULKEEPER: return 20000;
		case ART_DUELIST: return 2000;
		case ART_LOHENGRIN: return 20000;
		case ART_DAERON: return 20000;
		case ART_JULIAN: return 10000;
		case ART_CASPANION: return 400;
		case ART_DOR:
		case ART_TERROR:
		case ART_STONEMASK: return 100;
		case ART_HOLHENNETH: return 1000;
		case ART_AMBER: return 20000;
		case ART_SARUMAN: return 20000;
		case ART_COLLUIN: return 20000;
		case ART_SEIRYU: return 20000;
		case ART_MAGLOR: return 30000;
		case ART_BLOODRIP: return 5000;
		case ART_HOLCOLLETH: return 200;
		case ART_THINGOL: return 1000;
		case ART_COLANNON: return 2000;
		case ART_LUTHIEN: return 2000;
		case ART_HEAVENLY_MAIDEN: return 1000;
		case ART_CAMMITHRIM: return 300;
		case ART_FINGOLFIN: return 1000;
		case ART_FEANOR: return 15000;
		case ART_FLORA: return 400;
		case ART_NARTHANC: return 300;
		case ART_NIMTHANC: return 300;
		case ART_DETHANC: return 300;
		case ART_RILIA: return 400;
		case ART_NUMAHOKO: return 1000;
		case ART_FIONA: return 400;
		case ART_KUSANAGI: return 1500;
		case ART_WEREWINDLE: return 3000;
		case ART_KAMUI: return 1500;
		case ART_RINGIL: return 1000;
		case ART_DAWN: return 2000;
		case ART_ANDURIL: return 500;
		case ART_THEODEN: return 1000;
		case ART_RUNESPEAR: return 1000;
		case ART_AEGLOS: return 1000;
		case ART_DESTINY: return 3000;
		case ART_NAIN: return 3000;
		case ART_EONWE: return 25000;
		case ART_LOTHARANG: return 200;
		case ART_ULMO: return 2500;
		case ART_AVAVIR: return 1500;
		case ART_MAGATAMA: return 1500;
		case ART_TOTILA: return 300;
		case ART_FIRESTAR: return 500;
		case ART_AEGIR: return 25000;
		case ART_DEFENDER_OF_THE_CROWN: return 10000;
		case ART_MONKEY_KING: return 30000;
		case ART_MAUL_OF_VICE: return 60000;
		case ART_GOTHMOG: return 500;
		case ART_TARATOL: return 15000;
		case ART_ERIRIL: return 1200;
		case ART_GANDALF: return 10000;
		case ART_TURMIL: return 800;
		case ART_BRAND: 
		case ART_HELLFIRE: return 5000;
		case ART_CRIMSON: return 2000;
		case ART_PALANTIR: return 12000;
		case ART_BOROMIR: return 1000;
		case ART_FARAMIR: return 1000;
		case ART_HIMRING: return 10000;
		case ART_ICANUS: return 1200;
		case ART_HURIN: return 17000;
		case ART_GIL_GALAD: return 3000;
		case ART_YENDOR: return 1200;
		case ART_MURAMASA: return 50000;
		case ART_FLY_STONE: return 4000;
		case ART_TAIKOBO: return 2;
		case ART_JONES: return 700;
		case ART_ARRYU: return 1500;
		case ART_GAEBOLG: return 1200;
		case ART_INROU: return 15000;
		case ART_HYOUSIGI: return -2000;
		case ART_MATOI:
		case ART_AEGISFANG: return 3000;
		case ART_EARENDIL: return 2000;
		case ART_BOLISHOI: return 400;
		case ART_ARUNRUTH: return 300;
		case ART_BLOOD: return 20000;
		case ART_KESHO: return 7000;
		case ART_MOOK: return 5000;
		case ART_HERMIT: return 10000;
		case ART_JIZO: return 2000;
		case ART_FUNDIN: return 1500;
		case ART_AESCULAPIUS: return 12000;
		case ART_NIGHT: return 3000;
		case ART_HELL: return 3000;
		case ART_SACRED_KNIGHTS: return 1111;
		case ART_CHARMED: return 15000;
		case ART_ZEUS: return 10000;
		case ART_HOLY_GRAIL: return 5000;
		case ART_POSEIDON: return 5000;
		case ART_HADES: return 12000;
		case ART_ATHENA: return 7000;
		case ART_ARES: return 3000;
		case ART_HERMES: return 50000;
		case ART_APOLLO: return 10000;
		case ART_ARTEMIS: return 3000;
		case ART_HEPHAESTUS: return 4000;
		case ART_HERA: return 17000;
		case ART_DEMETER: return 20000;
		case ART_APHRODITE: return 5000;
		case ART_BALLISTA: return 6000;
		case ART_KAMIKAZE_ROBE: return 9000;
		case ART_RAILGUN: return 20000;
		case ART_STOMPER: return 4000; 
		case ART_GONG: return 5000;
		case ART_STONE_OF_WAR: return 7000;
		/* Hack: These activate, but also have *very* valuable hidden powers */
		case ART_STONE_OF_NATURE: return 100000;
		case ART_STONE_OF_LIFE: return 100000;
		case ART_STONE_OF_SORCERY: return 100000;
		case ART_STONE_OF_CHAOS: return 100000;
		case ART_STONE_OF_DEATH: return 100000;
		case ART_STONE_OF_TRUMP: return 100000;
		case ART_STONE_OF_DAEMON: return 100000;
		case ART_STONE_OF_CRUSADE: return 100000;
		case ART_STONE_OF_CRAFT: return 125000;
		}
	}
	
	switch (o_ptr->name2)
	{
	case EGO_TRUMP: return 1500;
	case EGO_LITE_ILLUMINATION: return 200;
	case EGO_EARTHQUAKES: return 700;
	case EGO_JUMP: return 1000;
	case EGO_DAEMON: return 8000;
	}

	if (o_ptr->tval == TV_DRAG_ARMOR)
	{
		switch (o_ptr->sval)
		{
		case SV_DRAGON_BLUE:
		case SV_DRAGON_WHITE:
		case SV_DRAGON_BLACK:
		case SV_DRAGON_GREEN:
		case SV_DRAGON_RED: return 7000;
		case SV_DRAGON_MULTIHUED: return 12000;
		case SV_DRAGON_BRONZE: return 7000;
		case SV_DRAGON_GOLD: return 8000;
		case SV_DRAGON_CHAOS: return 9000;
		case SV_DRAGON_LAW: return 10000;
		case SV_DRAGON_BALANCE: return 12000;
		case SV_DRAGON_SHINING: return 7000;
		case SV_DRAGON_POWER: return 20000;
		}
	}
	else if (o_ptr->tval == TV_RING)
	{
		if (object_is_ego(o_ptr))
		{
			switch (o_ptr->name2)
			{
			case EGO_RING_HERO: return 1500;
			case EGO_RING_MAGIC_MIS: return 200;
			case EGO_RING_FIRE_BOLT: return 500;
			case EGO_RING_COLD_BOLT: return 500;
			case EGO_RING_ELEC_BOLT: return 500;
			case EGO_RING_ACID_BOLT: return 500;
			case EGO_RING_MANA_BOLT: return 900;
			case EGO_RING_FIRE_BALL: return 700;
			case EGO_RING_COLD_BALL: return 700;
			case EGO_RING_ELEC_BALL: return 700;
			case EGO_RING_ACID_BALL: return 700;
			case EGO_RING_MANA_BALL: return 1200;
			case EGO_RING_DRAGON_F: return 1500;
			case EGO_RING_DRAGON_C: return 1500;
			case EGO_RING_M_DETECT: return 2000;
			case EGO_RING_D_SPEED: return 15000;
			case EGO_RING_BERSERKER: return 5000;
			case EGO_RING_TELE_AWAY: return 3000;
			case EGO_RING_TRUE: return 35000;
			}
		}
		switch (o_ptr->sval)
		{
		case SV_RING_ACID:
		case SV_RING_ICE:
		case SV_RING_FLAMES:
		case SV_RING_ELEC: return 7000;
		}
	}
	else if (o_ptr->tval == TV_AMULET)
	{
		if (object_is_ego(o_ptr))
		{
			switch (o_ptr->name2)
			{
			case EGO_AMU_IDENT: return 1500;
			case EGO_AMU_CHARM: return 600;
			case EGO_AMU_JUMP: return 2000;
			case EGO_AMU_TELEPORT: return 1500;
			case EGO_AMU_D_DOOR: return 7000;
			case EGO_AMU_RES_FIRE_:
			case EGO_AMU_RES_COLD_:
			case EGO_AMU_RES_ELEC_:
			case EGO_AMU_RES_ACID_: return 7000;
			case EGO_AMU_DETECTION: return 4000;
			case EGO_AMU_RESISTANCE: return 50000;
			}
		}
	}
	return 0;
}


static s32b _aura_p(u32b flgs[TR_FLAG_SIZE])
{
	s32b p = 0, ct = 0;
	if (have_flag(flgs, TR_SH_FIRE)) ct++;
	if (have_flag(flgs, TR_SH_ELEC)) ct++;
	if (have_flag(flgs, TR_SH_COLD)) ct++;
	switch (ct)
	{
	case 0: p = 0; break;
	case 1: p = 2000; break;
	case 2: p = 5000; break;
	case 3: p = 10000; break;
	default: p = 10000; break; /* Do we add more auras and forget to update? */
	}
	return p;
}

static s32b _stats_q(u32b flgs[TR_FLAG_SIZE], int pval)
{
	s32b y = 0, ct = 0, q = 0;

	pval = MIN(pval, 10); /* Iron Crown of the Serpent is +125 */

	if (have_flag(flgs, TR_SPELL_POWER)) 
		return 5000 * ABS(pval); /* Hack! */
	else
	{
		if (have_flag(flgs, TR_STR)) {y += 12; ct++;}
		if (have_flag(flgs, TR_DEX)) {y += 12; ct++;}
		if (have_flag(flgs, TR_CON)) {y += 12; ct++;}
	}

	ct = 0;
	if (have_flag(flgs, TR_INT)) {y += 12; ct++;}
	if (have_flag(flgs, TR_WIS)) {y += 12; ct++;}
	if (have_flag(flgs, TR_CHR)) {y += 12; ct++;}


	if (have_flag(flgs, TR_MAGIC_MASTERY)) y += 9;
	if (have_flag(flgs, TR_STEALTH)) y += 6;

	if (y != 0)
	{
		if (pval < 0)
			y /= 3;

		/*q = 2*(400 + y * ABS(y))*(1 + pval * ABS(pval));*/
		q = 2*(400 + y * ABS(y))*(1 + pval * ABS(pval));
	}

	if (have_flag(flgs, TR_SPELL_CAP))
		q += 1000 * pval;

	return q;
}

static s32b _speed_p(int pval)
{
	int result = 0;

	switch (pval)
	{
	case 1: result = 10000; break;
	case 2: result = 20000; break;
	case 3: result = 35000; break;
	case 4: result = 60000; break;
	case 5: result = 90000; break;
	default:
		if (pval < 0)
			result = 10000 * pval; /* Otherwise, we destroy values for Chainsword */
		else
			result = 90000 + 40000*(pval - 5);
		break;
	}
	return result;
}

static s32b _abilities_q(u32b flgs[TR_FLAG_SIZE])
{
/*
	s32b y,q;
	y = 0;

	if (have_flag(flgs, TR_SLOW_DIGEST)) y += 7;
	if (have_flag(flgs, TR_REGEN)) y += 25;
	if (have_flag(flgs, TR_WARNING)) y += 10;
	if (have_flag(flgs, TR_LITE)) y += 5;
	 
	if (have_flag(flgs, TR_SUST_STR)) y += 20;
	if (have_flag(flgs, TR_SUST_INT)) y += 20;
	if (have_flag(flgs, TR_SUST_WIS)) y += 20;
	if (have_flag(flgs, TR_SUST_DEX)) y += 20;
	if (have_flag(flgs, TR_SUST_CON)) y += 30;
	if (have_flag(flgs, TR_SUST_CHR)) y += 20;

	if (have_flag(flgs, TR_RES_FEAR)) y += 15;
	if (have_flag(flgs, TR_FREE_ACT)) y += 20;
	if (have_flag(flgs, TR_HOLD_LIFE)) y += 20;
	if (have_flag(flgs, TR_SEE_INVIS)) y += 15;
	if (have_flag(flgs, TR_LEVITATION)) y += 30;

	if (have_flag(flgs, TR_TELEPATHY)) y += 150;
	if (have_flag(flgs, TR_ESP_ANIMAL)) y += 10;
	if (have_flag(flgs, TR_ESP_UNDEAD)) y += 10;
	if (have_flag(flgs, TR_ESP_DEMON)) y += 10;
	if (have_flag(flgs, TR_ESP_ORC)) y += 7;
	if (have_flag(flgs, TR_ESP_TROLL)) y += 7;
	if (have_flag(flgs, TR_ESP_GIANT)) y += 7;
	if (have_flag(flgs, TR_ESP_DRAGON)) y += 10;
	if (have_flag(flgs, TR_ESP_HUMAN)) y += 12;
	if (have_flag(flgs, TR_ESP_EVIL)) y += 55;
	if (have_flag(flgs, TR_ESP_GOOD)) y += 6;
	if (have_flag(flgs, TR_ESP_NONLIVING)) y += 12;
	if (have_flag(flgs, TR_ESP_UNIQUE)) y += 20;

	if (have_flag(flgs, TR_EASY_SPELL)) y += 10;
	if (have_flag(flgs, TR_DEC_MANA)) y += 120;
	if (have_flag(flgs, TR_THROW)) y += 2;
	if (have_flag(flgs, TR_REFLECT)) y += 90;

	q = 100*y + 2*y*y;
	return q;
*/
	double cost = 0.0;
	int count = 0;

	/*  Sorry, Dave.  Chris is going berserk again ...
		These are what I would expect to pay 
	*/
	cost += _check_flag_and_score(flgs, TR_THROW, 100, &count);
	cost += _check_flag_and_score(flgs, TR_LITE, 300, &count);
	cost += _check_flag_and_score(flgs, TR_SLOW_DIGEST, 500, &count);
	cost += _check_flag_and_score(flgs, TR_WARNING, 700, &count);
	cost += _check_flag_and_score(flgs, TR_RES_FEAR, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_ORC, 700, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_TROLL, 700, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_GIANT, 700, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_GOOD, 700, &count);
	cost += _check_flag_and_score(flgs, TR_SEE_INVIS, 800, &count);
	cost += _check_flag_and_score(flgs, TR_SUST_STR, 1000, &count);
	cost += _check_flag_and_score(flgs, TR_SUST_INT, 1000, &count);
	cost += _check_flag_and_score(flgs, TR_SUST_WIS, 1000, &count);
	cost += _check_flag_and_score(flgs, TR_SUST_DEX, 1000, &count);
	cost += _check_flag_and_score(flgs, TR_SUST_CHR, 1000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_ANIMAL, 2000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_UNDEAD, 2000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_DEMON, 2000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_DRAGON, 2000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_HUMAN, 2000, &count);
	cost += _check_flag_and_score(flgs, TR_EASY_SPELL, 2000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_NONLIVING, 2200, &count);
	cost += _check_flag_and_score(flgs, TR_SUST_CON, 3000, &count);
	cost += _check_flag_and_score(flgs, TR_LEVITATION, 4000, &count);
	cost += _check_flag_and_score(flgs, TR_HOLD_LIFE, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_FREE_ACT, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_REGEN, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_UNIQUE, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_ESP_EVIL, 15000, &count);
	cost += _check_flag_and_score(flgs, TR_REFLECT, 15000, &count);
	cost += _check_flag_and_score(flgs, TR_DEC_MANA, 20000, &count);
	cost += _check_flag_and_score(flgs, TR_TELEPATHY, 25000, &count);
	cost += _check_flag_and_score(flgs, TR_WEAPONMASTERY, 25000, &count);

	/* Code later inflates based on item quality.  This factor is pure fudge */
	cost /= 1.0;

	return (u32b) cost;

}

static s32b _brands_q(u32b flgs[TR_FLAG_SIZE])
{
	double cost = 0.0;
	int count = 0;

	/* These are what I would expect to pay */
	cost += _check_flag_and_score(flgs, TR_BRAND_ACID, 12000, &count);
	cost += _check_flag_and_score(flgs, TR_BRAND_ELEC, 15000, &count);
	cost += _check_flag_and_score(flgs, TR_BRAND_FIRE, 8000, &count);
	cost += _check_flag_and_score(flgs, TR_BRAND_COLD, 8000, &count);

	return (u32b) cost;
}

static s32b _resistances_q(u32b flgs[TR_FLAG_SIZE])
{
	/*
	s32b y, q;

	y = 0;

	if (have_flag(flgs, TR_RES_ACID)) y += 10;
	if (have_flag(flgs, TR_RES_ELEC)) y += 10;
	if (have_flag(flgs, TR_RES_FIRE)) y += 10;
	if (have_flag(flgs, TR_RES_COLD)) y += 10;

	if ( have_flag(flgs, TR_RES_ACID)
	  && have_flag(flgs, TR_RES_ELEC)
	  && have_flag(flgs, TR_RES_FIRE)
	  && have_flag(flgs, TR_RES_COLD) ) 
	{
		y += 20;
	}

	if (have_flag(flgs, TR_RES_LITE)) y += 15;
	if (have_flag(flgs, TR_RES_DARK)) y += 15;
	if (have_flag(flgs, TR_RES_BLIND)) y += 15;

	if (have_flag(flgs, TR_RES_POIS)) y += 20;
	if (have_flag(flgs, TR_RES_CONF)) y += 20;
	if (have_flag(flgs, TR_RES_NEXUS)) y += 20;

	if (have_flag(flgs, TR_RES_NETHER)) y += 25;
	if (have_flag(flgs, TR_RES_CHAOS)) y += 25;
	if (have_flag(flgs, TR_RES_SOUND)) y += 25;

	if (have_flag(flgs, TR_RES_SHARDS)) y += 30;
	if (have_flag(flgs, TR_RES_DISEN)) y += 30;

	if (have_flag(flgs, TR_IM_ACID)) y += 70;
	if (have_flag(flgs, TR_IM_ELEC)) y += 90;
	if (have_flag(flgs, TR_IM_FIRE)) y += 70;
	if (have_flag(flgs, TR_IM_COLD)) y += 90;

	if (have_flag(flgs, TR_RES_TIME)) y += 40;

	q = 1000*y/10 + (20*y/10)*(y*y/100);
	return q;
	I'm confused by the cubic.  Let's try something completely different;
	*/
	double cost = 0.0;
	int count = 0;

	/* These are what I would expect to pay */
	cost += _check_flag_and_score(flgs, TR_RES_ACID, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_ELEC, 6000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_FIRE, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_COLD, 6000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_POIS, 10000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_LITE, 8000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_DARK, 8000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_BLIND, 5000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_CONF, 8000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_NETHER, 10000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_NEXUS, 8000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_CHAOS, 10000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_SOUND, 10000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_SHARDS, 20000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_DISEN, 15000, &count);
	cost += _check_flag_and_score(flgs, TR_RES_TIME, 25000, &count);

	cost += _check_flag_and_score(flgs, TR_IM_ACID, 100000, &count);
	cost += _check_flag_and_score(flgs, TR_IM_ELEC, 130000, &count);
	cost += _check_flag_and_score(flgs, TR_IM_FIRE, 120000, &count);
	cost += _check_flag_and_score(flgs, TR_IM_COLD, 140000, &count);

	return (u32b) cost;
}

s32b _finalize_p(s32b p, u32b flgs[TR_FLAG_SIZE], object_type *o_ptr)
{
	char dbg_msg[512];
	s32b y;

	y = _activation_p(o_ptr);
	if (y != 0)
	{
		p += y;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Activation: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_AGGRAVATE))
	{
		p = p * 8 / 10;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Aggravate: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_NO_TELE) && o_ptr->tval != TV_AMULET)
	{
		p = p * 7 / 10;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * No Tele: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_NO_MAGIC) && o_ptr->tval != TV_AMULET)
	{
		p = p * 9 / 10;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * No Magic: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_DRAIN_EXP))
	{
		p = p * 9 / 10;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Drain XP: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_TY_CURSE))
	{
		p = p * 8 / 10;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * AFC: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (o_ptr->curse_flags & TRC_PERMA_CURSE)
	{
		p = p * 8 / 10;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Perm Curse: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (!object_is_artifact(o_ptr))
	{
		p = p * 3 / 4;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Not Artifact: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Negative values don't make much sense, and some code
	   was using unsigned integers for values (e.g. Androids) */
	if (p < 0)
		p = 0;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Result: %d", p);
		cost_calc_hook(dbg_msg);
	}

	return p;
}

s32b jewelry_cost(object_type *o_ptr)
{
	s32b j, y, q, p;
	u32b flgs[TR_FLAG_SIZE];
	char dbg_msg[512];

	object_flags(o_ptr, flgs);

	if (cost_calc_hook)
	{
		char buf[MAX_NLEN];
		identify_item(o_ptr); /* Well, let's assume a developer is debugging :) */
		o_ptr->ident |= (IDENT_MENTAL); 
		object_desc(buf, o_ptr, 0);
		sprintf(dbg_msg, "Scoring `%s` ...", buf);
		cost_calc_hook(dbg_msg);
	}

	switch (o_ptr->tval)
	{
	case TV_LITE:		
		j = 1000;
		break;
	case TV_RING:
		j = 400;
		break;
	case TV_AMULET:
		j = 800;
		break;
	default:
		return 0;
	}

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Base Cost: j = %d", j);
		cost_calc_hook(dbg_msg);
	}

	/* Resistances */
	q = _resistances_q(flgs);
	p = j + q;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Resistances: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Abilities */
	q = _abilities_q(flgs);
	if (have_flag(flgs, TR_NO_MAGIC)) q += 7000;
	if (have_flag(flgs, TR_NO_TELE)) q += 5000;
	p += q;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Abilities: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Brands */
	q = _brands_q(flgs);
	p += q;

	if (cost_calc_hook && q)
	{
		sprintf(dbg_msg, "  * Brands: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Other Bonuses */
	y = 0;
	if (have_flag(flgs, TR_SEARCH)) y += 100;
	if (have_flag(flgs, TR_INFRA)) y += 500;

	if (y != 0)
	{
		q = y*o_ptr->pval;
		p += q;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Other Crap: y = %d, q = %d, p = %d", y, q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Speed */
	if (have_flag(flgs, TR_SPEED))
	{
		p += _speed_p(o_ptr->pval);

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Speed: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_BLOWS))
	{
		p += 90 * 1000 * o_ptr->pval;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Blows: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (have_flag(flgs, TR_XTRA_SHOTS))
	{
		p += 30000;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Shots: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Stats */
	q = _stats_q(flgs, o_ptr->pval);
	if (q != 0)
	{
		p += q;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Stats/Stealth: q = %d, p = %d", q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Auras */
	y = _aura_p(flgs);
	if (y != 0)
	{
		p += y;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Auras: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* +AC */
	if (o_ptr->to_a)
	{
		double x = o_ptr->to_a * ABS(o_ptr->to_a);
		double p2 = p;

		p2 = p2*(1000.0+x)/1000.0 + 20.0*x;
		p = (s32b)p2;

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * +AC: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* (+x,+y) */
	if (o_ptr->to_h != 0 || o_ptr->to_d != 0)
	{
		int x = o_ptr->to_h * ABS(o_ptr->to_h);
		int y = o_ptr->to_d * ABS(o_ptr->to_d);

		p += 100 * x;
		p += 200 * y;

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * (+x,+y): p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	p = _finalize_p(p, flgs, o_ptr);
	return p;
}

s32b armor_cost(object_type *o_ptr)
{
	s32b a, y, q, p;
	u32b flgs[TR_FLAG_SIZE];
	char dbg_msg[512];

	object_flags(o_ptr, flgs);

	if (cost_calc_hook)
	{
		char buf[MAX_NLEN];
		identify_item(o_ptr); /* Well, let's assume a developer is debugging :) */
		o_ptr->ident |= (IDENT_MENTAL); 
		object_desc(buf, o_ptr, 0);
		sprintf(dbg_msg, "Scoring `%s` ...", buf);
		cost_calc_hook(dbg_msg);
	}

	/* Base Cost */
	y = o_ptr->ac;
	a = y * y * 15;
	if (o_ptr->to_a <= 3 && !object_is_artifact(o_ptr) && !object_is_ego(o_ptr))
		a /= 3;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Base Cost: a = %d", a);
		cost_calc_hook(dbg_msg);
	}

	/* +AC ... Note, negative ac should decrease the cost! */
	a += 500*o_ptr->to_a + 20 * o_ptr->to_a * ABS(o_ptr->to_a);

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * +AC: a = %d", a);
		cost_calc_hook(dbg_msg);
	}

	/* Weight
	{
		double wgt = o_ptr->weight;
		double tmp = 0.0;
		double factor = 0.0;

		wgt /= 10.0;
		tmp = sqrt(wgt);
		if (tmp > 3.0) tmp = 3.0;

		factor = (115.0 - 15.0 * tmp)/100.0;
		tmp = a;

		a = (int)(tmp * factor);
		if (a < 0)
			a = 0;
	}

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Weight: a = %d", a);
		cost_calc_hook(dbg_msg);
	} */

	/* Resistances */
	q = _resistances_q(flgs);
	p = a + q;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Resistances: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Abilities */
	q = _abilities_q(flgs);
	if (o_ptr->name2 == EGO_SNIPER)
		q += 20000;
	p += q;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Abilities: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Brands */
	q = _brands_q(flgs);
	p += q;

	if (cost_calc_hook && q)
	{
		sprintf(dbg_msg, "  * Brands: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Speed */
	if (have_flag(flgs, TR_SPEED))
	{
		p += _speed_p(o_ptr->pval);

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Speed: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Stats */
	q = _stats_q(flgs, o_ptr->pval);
	if (q != 0)
	{
		p += q;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Stats/Stealth: q = %d, p = %d", q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Auras */
	y = _aura_p(flgs);
	if (y != 0)
	{
		p += y;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Auras: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Extra Attacks */
	if (have_flag(flgs, TR_BLOWS))
	{
		p += 300 * 1000 * o_ptr->pval; /* Just for show ... Shiva's Jacket and Ares */

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Blows: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Genji? This will become TR_2WEAPON someday ... */
	if (o_ptr->name2 == EGO_GENJI || o_ptr->name1 == ART_MASTER_TONBERRY)
	{
		p += 20000;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Genji: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* (+x,+y) */
	if (o_ptr->to_h != 0 || o_ptr->to_d != 0)
	{
		int x = o_ptr->to_h * ABS(o_ptr->to_h);
		int y = o_ptr->to_d * ABS(o_ptr->to_d);

		p += 100 * x;
		p += 200 * y;

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * (+x,+y): p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	p = _finalize_p(p, flgs, o_ptr);
	return p;
}

s32b weapon_cost(object_type *o_ptr)
{
	s32b y, w, p, q;
	u32b flgs[TR_FLAG_SIZE];
	char dbg_msg[512];

	object_flags(o_ptr, flgs);

	/* Hacks for objects with "hidden" powers */
	if (o_ptr->tval == TV_SWORD && o_ptr->sval == SV_DOKUBARI)
		return 10000;
	if (o_ptr->tval == TV_SWORD && o_ptr->sval == SV_RUNESWORD)
		return 100000;

	if (cost_calc_hook)
	{
		char buf[MAX_NLEN];
		identify_item(o_ptr); /* Well, let's assume a developer is debugging :) */
		o_ptr->ident |= (IDENT_MENTAL); 
		object_desc(buf, o_ptr, 0);
		sprintf(dbg_msg, "Scoring `%s` ...", buf);
		cost_calc_hook(dbg_msg);
	}
	{
		double d = (double)o_ptr->dd * ((double)o_ptr->ds + 1.0)/2;
		double s = 1.0;

		/* Figure average damage per strike. Not really because we are stacking slays
		   albeit weighted by my off the cuff estimates of utility */
		if (have_flag(flgs, TR_KILL_ORC)) s += (4.0 * .01);
		else if (have_flag(flgs, TR_SLAY_ORC)) s += (2.0 * .01);

		if (have_flag(flgs, TR_KILL_TROLL)) s += (4.0 * .02);
		else if (have_flag(flgs, TR_SLAY_TROLL)) s += (2.0 * .02);

		if (have_flag(flgs, TR_KILL_ANIMAL)) s += (3.0 * .025);
		else if (have_flag(flgs, TR_SLAY_ANIMAL)) s += (1.5 * .025);

		if (have_flag(flgs, TR_KILL_HUMAN)) s += (3.0 * .1);
		else if (have_flag(flgs, TR_SLAY_HUMAN)) s += (1.5 * .1);

		if (have_flag(flgs, TR_KILL_UNDEAD)) s += (4.0 * .1);
		else if (have_flag(flgs, TR_SLAY_UNDEAD)) s += (2.0 * .1);

		if (have_flag(flgs, TR_KILL_DEMON)) s += (4.0 * .15);
		else if (have_flag(flgs, TR_SLAY_DEMON)) s += (2.0 * .15);

		if (have_flag(flgs, TR_KILL_GIANT)) s += (4.0 * .075);
		else if (have_flag(flgs, TR_SLAY_GIANT)) s += (2.0 * 0.075);

		if (have_flag(flgs, TR_KILL_DRAGON)) s += (4.0 * .1);
		else if (have_flag(flgs, TR_SLAY_DRAGON)) s += (2.0 * .1);

		if (have_flag(flgs, TR_BRAND_POIS)) s += (1.5 * .1);
		if (have_flag(flgs, TR_BRAND_ACID)) s += (1.5 * .15);
		if (have_flag(flgs, TR_BRAND_ELEC)) s += (1.5 * .2);
		if (have_flag(flgs, TR_BRAND_FIRE)) s += (1.5 * .1);
		if (have_flag(flgs, TR_BRAND_COLD)) s += (1.5 * .1);
		
		if (have_flag(flgs, TR_CHAOTIC)) s += 0.4;

		if (have_flag(flgs, TR_KILL_EVIL)) s += (2.5 * 0.8);
		else if (have_flag(flgs, TR_SLAY_EVIL)) s += (1.0 * 0.8);

		if (have_flag(flgs, TR_FORCE_WEAPON))
		{
			s = (s * 1.50 + 1.0) * 0.25 + s * 0.75;
		}

		if (have_flag(flgs, TR_VORPAL))
		{
			if ( o_ptr->art_name == ART_VORPAL_BLADE 
			  || o_ptr->art_name == ART_CHAINSWORD
			  || o_ptr->art_name == ART_MURAMASA )
			{
				s *= 1.67;
			}
			else
				s *= 1.22;
		}

		d = d*s + (double)o_ptr->to_d;
		if (d < 1.0)
			d = 1.0;

		if (have_flag(flgs, TR_BLOWS))
			d += (d + 15.0)*o_ptr->pval/6.0;

		if (have_flag(flgs, TR_VAMPIRIC)) 
			d *= 1.1;

		w = d * d * d;

		if (have_flag(flgs, TR_VAMPIRIC)) 
			w += 5000;

		if (have_flag(flgs, TR_CHAOTIC)) 
			w += 3000;

		if (have_flag(flgs, TR_IMPACT)) 
			w += 250;
	
		if (have_flag(flgs, TR_WILD))
			w += 10000;

	}

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Base Cost: w = %d", w);
		cost_calc_hook(dbg_msg);
	}

	if (o_ptr->to_h <= 10)
		w += 100 * o_ptr->to_h;
	else
		w += 10 * o_ptr->to_h * o_ptr->to_h;


	/* Resistances */
	q = _resistances_q(flgs)/2;
	p = w + q;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Resistances: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Abilities */
	q = _abilities_q(flgs)/2;
	p += q;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Abilities: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Speed */
	if (have_flag(flgs, TR_SPEED))
	{
		p += _speed_p(o_ptr->pval);
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Speed: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Stats */
	q = _stats_q(flgs, o_ptr->pval)/2;
	if (q != 0)
	{
		p += q;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Stats/Stealth: q = %d, p = %d", q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Other Bonuses */
	y = 0;
	if (have_flag(flgs, TR_SEARCH)) y += 100;
	if (have_flag(flgs, TR_INFRA)) y += 500;
	if (have_flag(flgs, TR_TUNNEL)) 
	{
		if (o_ptr->tval == TV_DIGGING && o_ptr->pval == 1)
		{
			/* ?? Shovels and picks ... */
			y += 150;
		}
		else
			y += 1000;
	}

	if (y != 0)
	{
		q = y*o_ptr->pval;
		p += q;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Other Crap: y = %d, q = %d, p = %d", y, q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Auras */
	y = _aura_p(flgs);
	if (y != 0)
	{
		p += y;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Auras: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* AC Bonus */
	if (o_ptr->to_a != 0)
	{
	/*	p += 500*o_ptr->to_a + o_ptr->to_a * ABS(o_ptr->to_a) * 30; */
		p += 500*o_ptr->to_a;

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * AC: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	if (object_allow_two_hands_wielding(o_ptr))
		p += 75;

	p += o_ptr->weight;
	
	p = _finalize_p(p, flgs, o_ptr);
	return p;
}

static s32b _avg_dam_bow(int sval, int to_d, bool might)
{
	s32b d = 0;
	s32b m = bow_tmul(sval);

	if (might)
		m++;

	switch (sval)
	{
	case SV_SLING:
		d = m*2 + m*MAX(0, to_d);
		break;

	case SV_SHORT_BOW:
		d = m*5/2 + m*MAX(0, to_d);
		break;

	case SV_LONG_BOW:
		d = m*5/2 + m*MAX(0, to_d) + m;
		break;

	case SV_NAMAKE_BOW:
		d = m*18 + m*MAX(0, to_d);
		break;

	case SV_LIGHT_XBOW:
		d = m*3 + m*MAX(0, to_d);
		break;

	case SV_HEAVY_XBOW:
		d = m*3 + m*MAX(0, to_d) + m;
		break;

	case SV_HARP:
		d = 25;
		break;

	default:
		d = 50; /* Gun */
	}

	return d;
}

s32b bow_cost(object_type *o_ptr)
{
	s32b y, w, p, q, t;
	u32b flgs[TR_FLAG_SIZE];
	char dbg_msg[512];

	object_flags(o_ptr, flgs);

	if (cost_calc_hook)
	{
		char buf[MAX_NLEN];
		identify_item(o_ptr); /* Well, let's assume a developer is debugging :) */
		o_ptr->ident |= (IDENT_MENTAL); 
		object_desc(buf, o_ptr, 0);
		sprintf(dbg_msg, "Scoring `%s` ...", buf);
		cost_calc_hook(dbg_msg);
	}

	/* Base Cost calculated from expected damage output */
	t = _avg_dam_bow(o_ptr->sval, o_ptr->to_d, have_flag(flgs, TR_XTRA_MIGHT));
	w = t * t * 5;
	if (have_flag(flgs, TR_XTRA_SHOTS))
		w = w * 3 / 2;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Base Cost: w = %d", w);
		cost_calc_hook(dbg_msg);
	}

	/* (+x,+y) */
	if (o_ptr->to_h < 0) {}
	else if (o_ptr->to_h <= 10)
		w += 100 * o_ptr->to_h;
	else
		w += 10 * o_ptr->to_h * o_ptr->to_h;

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * (+x,+y): w = %d", w);
		cost_calc_hook(dbg_msg);
	}

	/* Resistances */
	q = _resistances_q(flgs)/2;
	p = w + q + (q/100)*w/200;
	/*p = w + q*(1+w/20000);*/

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Resistances: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Abilities */
	q = _abilities_q(flgs);
	p += q + (q/100)*w/400;
	/*p += q*(1+w/20000);*/

	if (cost_calc_hook)
	{
		sprintf(dbg_msg, "  * Abilities: q = %d, p = %d", q, p);
		cost_calc_hook(dbg_msg);
	}

	/* Speed */
	if (have_flag(flgs, TR_SPEED))
	{
		p += _speed_p(o_ptr->pval);

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Speed: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Stats */
	q = _stats_q(flgs, o_ptr->pval);
	if (q != 0)
	{
		p += q + (q/100)*w/100;
		/*p += q*(1 + w/10000);*/
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Stats/Stealth: q = %d, p = %d", q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Other Bonuses */
	y = 0;
	if (have_flag(flgs, TR_SEARCH)) y += 100;
	if (have_flag(flgs, TR_INFRA)) y += 500;
	if (y != 0)
	{
		q = y*o_ptr->pval;
		p += q + (q/100)*w/300;
		/*p += q*(1 + w/30000);*/
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Other Crap: y = %d, q = %d, p = %d", y, q, p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* Auras */
	y = _aura_p(flgs);
	if (y != 0)
	{
		p += y;
		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * Auras: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	/* AC Bonus */
	if (o_ptr->to_a != 0)
	{
		p += 500*o_ptr->to_a + o_ptr->to_a * ABS(o_ptr->to_a) * 30;

		if (cost_calc_hook)
		{
			sprintf(dbg_msg, "  * AC: p = %d", p);
			cost_calc_hook(dbg_msg);
		}
	}

	p = _finalize_p(p, flgs, o_ptr);
	return p;
}

s32b new_object_cost(object_type *o_ptr)
{
	if (object_is_melee_weapon(o_ptr)) return weapon_cost(o_ptr);
	else if (o_ptr->tval == TV_BOW) return bow_cost(o_ptr);
	else if (object_is_armour(o_ptr) || object_is_shield(o_ptr)) return armor_cost(o_ptr);
	else if (object_is_jewelry(o_ptr) || (o_ptr->tval == TV_LITE && object_is_artifact(o_ptr))) return jewelry_cost(o_ptr);
	return 0;
}