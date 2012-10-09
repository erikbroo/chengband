
/* File: misc.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: misc code */

#include "angband.h"

/*
 * Wrap calculation of AC bonuses from Dex
 */
static int calc_adj_dex_ta(void)
{
	/* The old way:
	 * return ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	 */

int z, bonus;

	bonus = ((int)(adj_dex_ta[p_ptr->stat_ind[A_DEX]]) - 128);
	
	z = 20;

	if (inventory[INVEN_BODY].k_idx) z -= 5;

	if (inventory[INVEN_LARM].k_idx && 
	    (k_info[inventory[INVEN_LARM].k_idx].tval == TV_SHIELD || k_info[inventory[INVEN_LARM].k_idx].tval == TV_CARD))
	{
		z -= 2;
	}

	if (!inventory[INVEN_LARM].k_idx && p_ptr->ryoute)
		z -= 2;

	if (inventory[INVEN_RARM].k_idx && 
	    (k_info[inventory[INVEN_RARM].k_idx].tval == TV_SHIELD  || k_info[inventory[INVEN_RARM].k_idx].tval == TV_CARD))
	{
		z -= 2;
	}

	if (inventory[INVEN_HEAD].k_idx) z -= 1;
	if (inventory[INVEN_HANDS].k_idx) z -= 1;
	if (inventory[INVEN_FEET].k_idx) z -= 1;

	bonus = bonus * z / 20;

	return bonus;
}

/*
 * Converts stat num into a six-char (right justified) string
 */
void cnv_stat(int val, char *out_val)
{
	/* Above 18 */
	if (val > 18)
	{
		int bonus = (val - 18);

		if (bonus >= 220)
		{
			sprintf(out_val, "18/%3s", "***");
		}
		else if (bonus >= 100)
		{
			sprintf(out_val, "18/%03d", bonus);
		}
		else
		{
			sprintf(out_val, " 18/%02d", bonus);
		}
	}

	/* From 3 to 18 */
	else
	{
		sprintf(out_val, "    %2d", val);
	}
}



/*
 * Modify a stat value by a "modifier", return new value
 *
 * Stats go up: 3,4,...,17,18,18/10,18/20,...,18/220
 * Or even: 18/13, 18/23, 18/33, ..., 18/220
 *
 * Stats go down: 18/220, 18/210,..., 18/10, 18, 17, ..., 3
 * Or even: 18/13, 18/03, 18, 17, ..., 3
 */
s16b modify_stat_value(int value, int amount)
{
	int    i;

	/* Reward */
	if (amount > 0)
	{
		/* Apply each point */
		for (i = 0; i < amount; i++)
		{
			/* One point at a time */
			if (value < 18) value++;

			/* Ten "points" at a time */
			else value += 10;
		}
	}

	/* Penalty */
	else if (amount < 0)
	{
		/* Apply each point */
		for (i = 0; i < (0 - amount); i++)
		{
			/* Ten points at a time */
			if (value >= 18+10) value -= 10;

			/* Hack -- prevent weirdness */
			else if (value > 18) value = 18;

			/* One point at a time */
			else if (value > 3) value--;
		}
	}

	/* Return new value */
	return (value);
}



/*
 * Print character info at given row, column in a 13 char field
 */
static void prt_field(cptr info, int row, int col)
{
	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", row, col);

	/* Dump the info itself */
	c_put_str(TERM_L_BLUE, info, row, col);
}


/*
 *  Whether daytime or not
 */
bool is_daytime(void)
{
	s32b len = TURNS_PER_TICK * TOWN_DAWN;
	if ((turn % len) < (len / 2))
		return TRUE;
	else
		return FALSE;
}

/*
 * Extract day, hour, min
 */
void extract_day_hour_min(int *day, int *hour, int *min)
{
	const s32b A_DAY = TURNS_PER_TICK * TOWN_DAWN;
	s32b turn_in_today = (turn + A_DAY / 4) % A_DAY;

	switch (p_ptr->start_race)
	{
	case RACE_VAMPIRE:
	case RACE_SKELETON:
	case RACE_ZOMBIE:
	case RACE_SPECTRE:
		*day = (turn - A_DAY * 3 / 4) / A_DAY + 1;
		break;
	default:
		*day = (turn + A_DAY / 4) / A_DAY + 1;
		break;
	}
	*hour = (24 * turn_in_today / A_DAY) % 24;
	*min = (1440 * turn_in_today / A_DAY) % 60;
}

/*
 * Print time
 */
void prt_time(void)
{
	int day, hour, min;

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DAY, COL_DAY);

	extract_day_hour_min(&day, &hour, &min);

	/* Dump the info itself */
#ifdef JP
	if (day < 1000) c_put_str(TERM_WHITE, format("%2d����", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "***����", ROW_DAY, COL_DAY);
#else
	if (day < 1000) c_put_str(TERM_WHITE, format("Day%3d", day), ROW_DAY, COL_DAY);
	else c_put_str(TERM_WHITE, "Day***", ROW_DAY, COL_DAY);
#endif

	c_put_str(TERM_WHITE, format("%2d:%02d", hour, min), ROW_DAY, COL_DAY+7);
}


cptr map_name(void)
{
	if (p_ptr->inside_quest && is_fixed_quest_idx(p_ptr->inside_quest)
	    && (quest[p_ptr->inside_quest].flags & QUEST_FLAG_PRESET))
#ifdef JP
		return "��������";
#else
		return "Quest";
#endif
	else if (p_ptr->wild_mode)
#ifdef JP
		return "�Ͼ�";
#else
		return "Surface";
#endif
	else if (p_ptr->inside_arena)
#ifdef JP
		return "���꡼��";
#else
		return "Arena";
#endif
	else if (p_ptr->inside_battle)
#ifdef JP
		return "Ʈ����";
#else
		return "Monster Arena";
#endif
	else if (!dun_level && p_ptr->town_num)
		return town[p_ptr->town_num].name;
	else
		return d_name+d_info[dungeon_type].name;
}

/*
 * Print dungeon
 */
static void prt_dungeon(void)
{
	int col;
	char buf[100];

	/* Dump 13 spaces to clear */
	c_put_str(TERM_WHITE, "             ", ROW_DUNGEON, COL_DUNGEON);

	my_strcpy(buf, map_name(), 13);

	col = COL_DUNGEON + 6 - strlen(buf)/2;
	if (col < 0) col = 0;

	/* Dump the info itself */
	c_put_str(TERM_L_UMBER, buf,
		  ROW_DUNGEON, col);
}




/*
 * Print character stat in given row, column
 */
static void prt_stat(int stat)
{
	char tmp[32];

	/* Display "injured" stat */
	if (p_ptr->stat_cur[stat] < p_ptr->stat_max[stat])
	{
		put_str(stat_names_reduced[stat], ROW_STAT + stat, 0);
		cnv_stat(p_ptr->stat_use[stat], tmp);
		c_put_str(TERM_YELLOW, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Display "healthy" stat */
	else
	{
		put_str(stat_names[stat], ROW_STAT + stat, 0);
		cnv_stat(p_ptr->stat_use[stat], tmp);
		c_put_str(TERM_L_GREEN, tmp, ROW_STAT + stat, COL_STAT + 6);
	}

	/* Indicate natural maximum */
	if (p_ptr->stat_max[stat] == p_ptr->stat_max_max[stat])
	{
#ifdef JP
		/* ���ܸ�ˤ��֤�ʤ��褦��ɽ�����֤��ѹ� */
		put_str("!", ROW_STAT + stat, 5);
#else
		put_str("!", ROW_STAT + stat, 3);
#endif

	}
}


/*
 *  Data structure for status bar
 */
#define BAR_TSUYOSHI 0
#define BAR_HALLUCINATION 1
#define BAR_BLINDNESS 2
#define BAR_PARALYZE 3
#define BAR_CONFUSE 4
#define BAR_POISONED 5
#define BAR_AFRAID 6
#define BAR_LEVITATE 7
#define BAR_REFLECTION 8
#define BAR_PASSWALL 9
#define BAR_WRAITH 10
#define BAR_PROTEVIL 11
#define BAR_KAWARIMI 12
#define BAR_MAGICDEFENSE 13
#define BAR_EXPAND 14
#define BAR_STONESKIN 15
#define BAR_MULTISHADOW 16
#define BAR_REGMAGIC 17
#define BAR_ULTIMATE 18
#define BAR_INVULN 19
#define BAR_IMMACID 20
#define BAR_RESACID 21
#define BAR_IMMELEC 22
#define BAR_RESELEC 23
#define BAR_IMMFIRE 24
#define BAR_RESFIRE 25
#define BAR_IMMCOLD 26
#define BAR_RESCOLD 27
#define BAR_RESPOIS 28
#define BAR_RESNETH 29
#define BAR_RESTIME 30
#define BAR_DUSTROBE 31
#define BAR_SHFIRE 32
#define BAR_TOUKI 33
#define BAR_SHHOLY 34
#define BAR_EYEEYE 35
#define BAR_BLESSED 36
#define BAR_HEROISM 37
#define BAR_BERSERK 38
#define BAR_ATTKFIRE 39
#define BAR_ATTKCOLD 40
#define BAR_ATTKELEC 41
#define BAR_ATTKACID 42
#define BAR_ATTKPOIS 43
#define BAR_ATTKCONF 44
#define BAR_SENSEUNSEEN 45
#define BAR_TELEPATHY 46
#define BAR_REGENERATION 47
#define BAR_INFRAVISION 48
#define BAR_STEALTH 49
#define BAR_SUPERSTEALTH 50
#define BAR_RECALL 51
#define BAR_ALTER 52
#define BAR_SHCOLD 53
#define BAR_SHELEC 54
#define BAR_SHSHADOW 55
#define BAR_MIGHT 56
#define BAR_BUILD 57
#define BAR_ANTIMULTI 58
#define BAR_ANTITELE 59
#define BAR_ANTIMAGIC 60
#define BAR_PATIENCE 61
#define BAR_REVENGE 62
#define BAR_RUNESWORD 63
#define BAR_VAMPILIC 64
#define BAR_CURE 65
#define BAR_ESP_EVIL 66
#define BAR_SPEED_ESSENTIA 67
#define BAR_BLOOD_SHIELD 68
#define BAR_BLOOD_SEEK 69
#define BAR_BLOOD_REVENGE 70
#define BAR_BLOOD_SIGHT 71
#define BAR_BLOOD_FEAST 72
#define BAR_NO_SPELLS 73
#define BAR_TIME_SPURT 74
#define BAR_SPECIAL 75
#define BAR_DUELIST 76
#define BAR_SHOT_ON_THE_RUN 77
#define BAR_RAPID_SHOT 78
#define BAR_FLYING_DAGGER 79
#define BAR_SHADOW_STANCE 80
#define BAR_FRENZY_STANCE 81
#define BAR_GENJI 82
#define BAR_FORCE 83
#define BAR_COMBAT_EXPERTISE 84
#define BAR_STONE_BONES 85
#define BAR_TRADE_BLOWS 86
#define BAR_POWER_ATTACK 87
#define BAR_VICIOUS_STRIKE 88
#define BAR_BURNING_BLADE 89
#define BAR_ICE_BLADE 90
#define BAR_THUNDER_BLADE 91
#define BAR_BLOOD_BLADE 92
#define BAR_HOLY_BLADE 93
#define BAR_ORDER_BLADE 94
#define BAR_WILD_BLADE 95
#define BAR_MANY_STRIKE 96
#define BAR_PIERCING_STRIKE 97
#define BAR_TRIP 98
#define BAR_ENTRENCHED 99
#define BAR_ENLARGE_WEAPON 100
#define BAR_FLURRY_OF_BLOWS 101
#define BAR_GREATER_FLURRY 102
#define BAR_STRENGTH_OF_THE_UNDERTAKER 103
#define BAR_STOICISM 104
#define BAR_INDUSTRIOUS_MORTICIAN 105
#define BAR_SHIELD_BASH 106
#define BAR_BULWARK 107
#define BAR_BLOOD_RITE 108
#define BAR_WEAPON_GRAFT 109
#define BAR_PSIONIC_CLARITY 110
#define BAR_PSIONIC_BLENDING 111
#define BAR_PSIONIC_SHIELDING 112
#define BAR_PSIONIC_COMBAT 113
#define BAR_MENTAL_FORTRESS 114
#define BAR_MINDSPRING 115
#define BAR_PSIONIC_FORESIGHT 116
#define BAR_RES_DISENCHANTMENT 117
#define BAR_SPELL_REACTION 118
#define BAR_RESIST_CURSES 119
#define BAR_ARMOR_OF_FURY 120
#define BAR_SPELL_TURNING 121
#define BAR_FASTING 122
#define BAR_SUSTAIN_STR 123
#define BAR_SUSTAIN_INT 124
#define BAR_SUSTAIN_WIS 125
#define BAR_SUSTAIN_DEX 126
#define BAR_SUSTAIN_CON 127
#define BAR_SUSTAIN_CHR 128
#define BAR_HOLD_LIFE 129
#define BAR_TRANSCENDENCE 130
#define BAR_THE_WORLD 131
#define BAR_DARK_STALKER 132
#define BAR_NIMBLE_DODGE 133
#define BAR_STEALTHY_SNIPE 134
#define BAR_WEAPON_AS_SHIELD 135
#define BAR_CURSED_WOUNDS 136
#define BAR_NO_EARTHQUAKE 137
#define BAR_DEATH_FORCE 138
#define BAR_KILLING_SPREE 139
#define BAR_SLAY_SENTIENT 140
#define BAR_QUICK_WALK 141
#define BAR_INVEN_PROT 142
#define BAR_SHRIKE 143
#define BAR_WEAPONMASTERY 144

static struct {
	byte attr;
	cptr sstr;
	cptr lstr;
} bar[]
#ifdef JP
= {
	{TERM_YELLOW, "��", "�Ĥ褷"},
	{TERM_VIOLET, "��", "����"},
	{TERM_L_DARK, "��", "����"},
	{TERM_RED, "��", "����"},
	{TERM_VIOLET, "��", "����"},
	{TERM_GREEN, "��", "��"},
	{TERM_BLUE, "��", "����"},
	{TERM_L_BLUE, "��", "��ͷ"},
	{TERM_SLATE, "ȿ", "ȿ��"},
	{TERM_SLATE, "��", "��ȴ��"},
	{TERM_L_DARK, "ͩ", "ͩ��"},
	{TERM_SLATE, "��", "�ɼ�"},
	{TERM_VIOLET, "��", "�Ѥ���"},
	{TERM_YELLOW, "��", "��ˡ��"},
	{TERM_L_UMBER, "��", "����"},
	{TERM_WHITE, "��", "��ȩ"},
	{TERM_L_BLUE, "ʬ", "ʬ��"},
	{TERM_SLATE, "��", "��ˡ�ɸ�"},
	{TERM_YELLOW, "��", "���"},
	{TERM_YELLOW, "̵", "̵Ũ"},
	{TERM_L_GREEN, "��", "���ȱ�"},
	{TERM_GREEN, "��", "�ѻ�"},
	{TERM_L_BLUE, "��", "���ȱ�"},
	{TERM_BLUE, "��", "����"},
	{TERM_L_RED, "��", "���ȱ�"},
	{TERM_RED, "��", "�Ѳ�"},
	{TERM_WHITE, "��", "���ȱ�"},
	{TERM_SLATE, "��", "����"},
	{TERM_GREEN, "��", "����"},
	{TERM_L_DARK, "��", "���Ϲ�"},
	{TERM_L_BLUE, "��", "�ѻ���"},
	{TERM_L_DARK, "��", "��������"},
	{TERM_L_RED, "��", "�Х�����"},
	{TERM_WHITE, "Ʈ", "Ʈ��"},
	{TERM_WHITE, "��", "��������"},
	{TERM_VIOLET, "��", "�ܤˤ���"},
	{TERM_WHITE, "��", "��ʡ"},
	{TERM_WHITE, "ͦ", "ͦ"},
	{TERM_RED, "��", "����"},
	{TERM_L_RED, "��", "�����"},
	{TERM_WHITE, "��", "�����"},
	{TERM_L_BLUE, "��", "�����"},
	{TERM_SLATE, "��", "�����"},
	{TERM_L_GREEN, "��", "�����"},
	{TERM_RED, "��", "�����Ƿ�"},
	{TERM_L_BLUE, "��", "Ʃ����"},
	{TERM_ORANGE, "��", "�ƥ�ѥ�"},
	{TERM_L_BLUE, "��", "����"},
	{TERM_L_RED, "��", "�ֳ�"},
	{TERM_UMBER, "��", "��̩"},
	{TERM_YELLOW, "��", "Ķ��̩"},
	{TERM_WHITE, "��", "����"},
	{TERM_WHITE, "��", "��������"},
	/* Hex */
	{TERM_WHITE, "��", "ɹ������"},
	{TERM_BLUE, "��", "�ť�����"},
	{TERM_L_DARK, "��", "�ƥ�����"},
	{TERM_YELLOW, "��", "���϶���"},
	{TERM_RED, "��", "���ζ���"},
	{TERM_L_DARK, "��", "ȿ����"},
	{TERM_ORANGE, "��", "ȿ�ƥ��"},
	{TERM_RED, "��", "ȿ��ˡ"},
	{TERM_SLATE, "��", "����"},
	{TERM_SLATE, "��", "���"},
	{TERM_L_DARK, "��", "�����"},
	{TERM_RED, "��", "�۷��Ƿ�"},
	{TERM_WHITE, "��", "����"},
	{TERM_L_DARK, "��", "�ٰ�����"},

	ERROR(Todo: Translate all the new stuff!!)

	{0, NULL, NULL}
};
#else
= {
	{TERM_YELLOW, "Ts", "Tsuyoshi"},
	{TERM_VIOLET, "Ha", "Halluc"},
	{TERM_L_DARK, "Bl", "Blind"},
	{TERM_RED, "Pa", "Paralyzed"},
	{TERM_VIOLET, "Cf", "Confused"},
	{TERM_GREEN, "Po", "Poisoned"},
	{TERM_YELLOW, "Af", "Afraid"},
	{TERM_L_BLUE, "Lv", "Levit"},
	{TERM_SLATE, "Rf", "Reflect"},
	{TERM_SLATE, "Pw", "PassWall"},
	{TERM_L_DARK, "Wr", "Wraith"},
	{TERM_SLATE, "Ev", "PrtEvl"},
	{TERM_VIOLET, "Kw", "Kawarimi"},
	{TERM_YELLOW, "Md", "MgcArm"},
	{TERM_L_UMBER, "Eh", "Expand"},
	{TERM_WHITE, "Ss", "StnSkn"},
	{TERM_L_BLUE, "Ms", "MltShdw"},
	{TERM_SLATE, "Rm", "ResMag"},
	{TERM_YELLOW, "Ul", "Ultima"},
	{TERM_YELLOW, "Iv", "Invuln"},
	{TERM_L_GREEN, "IAc", "ImmAcid"},
	{TERM_GREEN, "Ac", "Acid"},
	{TERM_L_BLUE, "IEl", "ImmElec"},
	{TERM_BLUE, "El", "Elec"},
	{TERM_L_RED, "IFi", "ImmFire"},
	{TERM_RED, "Fi", "Fire"},
	{TERM_WHITE, "ICo", "ImmCold"},
	{TERM_SLATE, "Co", "Cold"},
	{TERM_GREEN, "Po", "Pois"},
	{TERM_L_DARK, "Nt", "Nthr"},
	{TERM_L_BLUE, "Ti", "Time"},
	{TERM_L_DARK, "Mr", "Mirr"},
	{TERM_L_RED, "SFi", "SFire"},
	{TERM_WHITE, "Fo", "Force"},
	{TERM_WHITE, "Ho", "Holy"},
	{TERM_VIOLET, "Ee", "EyeEye"},
	{TERM_WHITE, "Bs", "Bless"},
	{TERM_WHITE, "He", "Hero"},
	{TERM_RED, "Br", "Berserk"},
	{TERM_L_RED, "BFi", "BFire"},
	{TERM_WHITE, "BCo", "BCold"},
	{TERM_L_BLUE, "BEl", "BElec"},
	{TERM_SLATE, "BAc", "BAcid"},
	{TERM_L_GREEN, "BPo", "BPois"},
	{TERM_RED, "TCf", "TchCnf"},
	{TERM_L_BLUE, "Se", "SInv"},
	{TERM_ORANGE, "Te", "Telepa"},
	{TERM_L_BLUE, "Rg", "Regen"},
	{TERM_L_RED, "If", "Infr"},
	{TERM_UMBER, "Sl", "Stealth"},
	{TERM_YELLOW, "Stlt", "Stealth"},
	{TERM_WHITE, "Rc", "Recall"},
	{TERM_WHITE, "Al", "Alter"},
	/* Hex */
	{TERM_WHITE, "SCo", "SCold"},
	{TERM_BLUE, "SEl", "SElec"},
	{TERM_L_DARK, "SSh", "SShadow"},
	{TERM_YELLOW, "EMi", "ExMight"},
	{TERM_RED, "Gi", "Giant"},
	{TERM_L_DARK, "AMl", "AntiMulti"},
	{TERM_ORANGE, "AT", "AntiTele"},
	{TERM_RED, "AM", "AntiMagic"},
	{TERM_SLATE, "Pa", "Patience"},
	{TERM_SLATE, "Rv", "Revenge"},
	{TERM_L_DARK, "Rs", "RuneSword"},
	{TERM_RED, "Vm", "Vampiric"},
	{TERM_WHITE, "Cu", "Cure"},
	{TERM_L_DARK, "ET", "EvilTele"},
	{TERM_RED, "At", "Attacks"},
	{TERM_ORANGE, "Sh", "Shield"},
	{TERM_YELLOW, "Sk", "Seek"},
	{TERM_RED, "Rv", "Revenge"},
	{TERM_L_BLUE, "Si", "Sight"},
	{TERM_WHITE, "Fs", "Feast"},
	{TERM_VIOLET, "NS", "No Spells"},
	{TERM_YELLOW, "Ts", "Spurt"},
	{TERM_L_BLUE, "Sp", "Special"},
	{TERM_YELLOW, "", "Duelist Target Goes Here!"},
	{TERM_L_BLUE, "Rn", "Shoot on Run"},
	{TERM_L_BLUE, "Rp", "Rapid Shot"},
	{TERM_L_BLUE, "FD", "Flying Dagger"},
	{TERM_L_BLUE, "Sw", "Shadow"},
	{TERM_L_BLUE, "Fz", "Frenzy"},
	{TERM_YELLOW, "Gj", "Genji"},
	{TERM_L_BLUE, "Fc", "Force"},
	{TERM_L_BLUE, "Ex", "Combat Expertise"},
	{TERM_UMBER, "SB", "Stone Bones"},
	{TERM_L_BLUE, "Tr", "Trade Blows"},
	{TERM_L_BLUE, "Pw", "Power Attack"},
	{TERM_RED, "Vs", "Stumbling 'n Bumbling"},
	{TERM_RED, "BB", "Burning Blade"},
	{TERM_BLUE, "IB", "Ice Blade"},
	{TERM_YELLOW, "TB", "Thunder Blade"},
	{TERM_RED, "Bl", "Blood Blade"},
	{TERM_WHITE, "HB", "Holy Blade"},
	{TERM_ORANGE, "OB", "Order Blade"},
	{TERM_GREEN, "WB", "Wild Blade"},
	{TERM_L_BLUE, "MS", "Many Strike"},
	{TERM_L_BLUE, "PS", "Piercing Strike"},
	{TERM_L_BLUE, "Trp", "Trip"},
	{TERM_UMBER, "En", "Entrenched"},
	{TERM_RED, "EW", "Enlarge"},
	{TERM_L_RED, "Fl", "Flurry"},
	{TERM_RED, "Fl", "FLURRY"},
	{TERM_UMBER, "Str", "Undertaker"},
	{TERM_ORANGE, "Sc", "Stoicism"},
	{TERM_YELLOW, "At", "Mortician"},
	{TERM_L_BLUE, "SB", "Shield Bash"},
	{TERM_L_BLUE, "Bw", "Bulwark"},
	{TERM_RED, "Rt", "Rite"},
	{TERM_WHITE, "Gft", "Graft"},
	{TERM_YELLOW, "Cl", "Clarity"},
	{TERM_L_DARK, "Bl", "Blending"},
	{TERM_ORANGE, "Sh", "Shielding"},
	{TERM_RED, "Ct", "Combat"},
	{TERM_VIOLET, "Ft", "Fortress"},
	{TERM_GREEN, "MS", "Mindspring"},
	{TERM_YELLOW, "Fs", "Foresight"},
	{TERM_L_DARK, "Dis", "Disenchant"},
	{TERM_L_BLUE, "Rct", "Reaction"},
	{TERM_YELLOW, "RC", "Curses"},
	{TERM_RED, "Fy", "Fury"},
	{TERM_GREEN, "Tn", "Turning"},
	{TERM_GREEN, "Fs", "Fasting"},
	{TERM_YELLOW, "(Str", "SustStr"},
	{TERM_YELLOW, "(Int", "SustInt"},
	{TERM_YELLOW, "(Wis", "SustWis"},
	{TERM_YELLOW, "(Dex", "SustDex"},
	{TERM_YELLOW, "(Con", "SustCon"},
	{TERM_YELLOW, "(Chr", "SustChr"},
	{TERM_YELLOW, "(Lf", "HoldLife"},
	{TERM_WHITE, "Tr", "Transcendence"},
	{TERM_L_BLUE, "ST", "StopTime"},
	{TERM_L_DARK, "DS", "Stealth"},
	{TERM_L_BLUE, "ND", "Dodge"},
	{TERM_UMBER, "SS", "Snipe"},
	{TERM_L_BLUE, "WS", "Shield"},
	{TERM_RED, "CW", "Wounds"},
	{TERM_YELLOW, "Eq-", "NoQuake"},
	{TERM_L_DARK, "DF", "DeathForce"},
	{TERM_VIOLET, "KS", "*KILL*"},
	{TERM_L_BLUE, "SS", "SlaySentient"},
	{TERM_YELLOW, "QW", "Quickwalk"},
	{TERM_L_BLUE, "IP", "InvenProt"},
	{TERM_YELLOW, "Sk", "Shrike"},
	{TERM_L_BLUE, "Wp", "Weapon"},
	{0, NULL, NULL}
};
#endif

#define ADD_FLG(FLG) (bar_flags[FLG / 32] |= (1L << (FLG % 32)))
#define IS_FLG(FLG) (bar_flags[FLG / 32] & (1L << (FLG % 32)))


/*
 *  Show status bar
 */
static void prt_status(void)
{
	u32b bar_flags[7];
	int wid, hgt, row_statbar, max_col_statbar;
	int i, col = 0, num = 0;
	int space = 2;

	Term_get_size(&wid, &hgt);
	row_statbar = hgt + ROW_STATBAR;
	max_col_statbar = wid + MAX_COL_STATBAR;

	Term_erase(0, row_statbar, max_col_statbar);

	for (i = 0; i < 7; i++)
		bar_flags[i] = 0L;

	/* Tsuyoshi  */
	if (p_ptr->tsuyoshi) ADD_FLG(BAR_TSUYOSHI);

	/* Hallucinating */
	if (p_ptr->image) ADD_FLG(BAR_HALLUCINATION);

	/* Blindness */
	if (p_ptr->blind) ADD_FLG(BAR_BLINDNESS);

	/* Paralysis */
	if (p_ptr->paralyzed) ADD_FLG(BAR_PARALYZE);

	/* Confusion */
	if (p_ptr->confused) ADD_FLG(BAR_CONFUSE);

	/* Posioned */
	if (p_ptr->poisoned) ADD_FLG(BAR_POISONED);

	/* Times see-invisible */
	if (p_ptr->tim_invis) ADD_FLG(BAR_SENSEUNSEEN);

	/* Timed esp */
	if (IS_TIM_ESP()) ADD_FLG(BAR_TELEPATHY);

	/* Timed regenerate */
	if (p_ptr->tim_regen) ADD_FLG(BAR_REGENERATION);

	/* Timed infra-vision */
	if (IS_TIM_INFRA()) ADD_FLG(BAR_INFRAVISION);

	/* Protection from evil */
	if (IS_PROT_EVIL()) ADD_FLG(BAR_PROTEVIL);

	/* Invulnerability */
	if (IS_INVULN()) ADD_FLG(BAR_INVULN);

	/* Wraith form */
	if (IS_WRAITH()) ADD_FLG(BAR_WRAITH);

	/* Kabenuke */
	if (IS_PASSWALL()) ADD_FLG(BAR_PASSWALL);

	if (p_ptr->tim_reflect) ADD_FLG(BAR_REFLECTION);

	/* Heroism */
	if (IS_HERO()) ADD_FLG(BAR_HEROISM);

	/* Super Heroism / berserk */
	if (IS_SHERO()) ADD_FLG(BAR_BERSERK);

	/* Blessed */
	if (IS_BLESSED()) ADD_FLG(BAR_BLESSED);

	/* Shield */
	if (p_ptr->magicdef) ADD_FLG(BAR_MAGICDEFENSE);

	if (p_ptr->tsubureru) ADD_FLG(BAR_EXPAND);

	if (IS_STONE_SKIN()) ADD_FLG(BAR_STONESKIN);
	
	if (p_ptr->special_defense & NINJA_KAWARIMI) ADD_FLG(BAR_KAWARIMI);

	/* Oppose Acid */
	if (p_ptr->special_defense & DEFENSE_ACID) ADD_FLG(BAR_IMMACID);
	if (IS_OPPOSE_ACID()) ADD_FLG(BAR_RESACID);

	/* Oppose Lightning */
	if (p_ptr->special_defense & DEFENSE_ELEC) ADD_FLG(BAR_IMMELEC);
	if (IS_OPPOSE_ELEC()) ADD_FLG(BAR_RESELEC);

	/* Oppose Fire */
	if (p_ptr->special_defense & DEFENSE_FIRE) ADD_FLG(BAR_IMMFIRE);
	if (IS_OPPOSE_FIRE()) ADD_FLG(BAR_RESFIRE);

	/* Oppose Cold */
	if (p_ptr->special_defense & DEFENSE_COLD) ADD_FLG(BAR_IMMCOLD);
	if (IS_OPPOSE_COLD()) ADD_FLG(BAR_RESCOLD);

	/* Oppose Poison */
	if (IS_OPPOSE_POIS()) ADD_FLG(BAR_RESPOIS);

	/* Word of Recall */
	if (p_ptr->word_recall) ADD_FLG(BAR_RECALL);

	/* Alter realiry */
	if (p_ptr->alter_reality) ADD_FLG(BAR_ALTER);

	/* Resist time */
	if (p_ptr->tim_res_time) ADD_FLG(BAR_RESTIME);

	if (p_ptr->multishadow) ADD_FLG(BAR_MULTISHADOW);

	/* Confusing Hands */
	if (p_ptr->special_attack & ATTACK_CONFUSE) ADD_FLG(BAR_ATTKCONF);

	if (IS_RESIST_MAGIC()) ADD_FLG(BAR_REGMAGIC);

	/* Ultimate-resistance */
	if (p_ptr->ult_res) ADD_FLG(BAR_ULTIMATE);

	/* tim levitation */
	if (p_ptr->tim_levitation) ADD_FLG(BAR_LEVITATE);

	if (p_ptr->tim_res_nether) ADD_FLG(BAR_RESNETH);
	if (p_ptr->tim_res_disenchantment) ADD_FLG(BAR_RES_DISENCHANTMENT);
	
	if (p_ptr->tim_spell_reaction) ADD_FLG(BAR_SPELL_REACTION);
	if (p_ptr->tim_resist_curses) ADD_FLG(BAR_RESIST_CURSES);
	if (p_ptr->tim_armor_of_fury) ADD_FLG(BAR_ARMOR_OF_FURY);
	if (p_ptr->tim_spell_turning) ADD_FLG(BAR_SPELL_TURNING);

	if (p_ptr->dustrobe) ADD_FLG(BAR_DUSTROBE);

	/* Mahouken */
	if (p_ptr->special_attack & ATTACK_FIRE) ADD_FLG(BAR_ATTKFIRE);
	if (p_ptr->special_attack & ATTACK_COLD) ADD_FLG(BAR_ATTKCOLD);
	if (p_ptr->special_attack & ATTACK_ELEC) ADD_FLG(BAR_ATTKELEC);
	if (p_ptr->special_attack & ATTACK_ACID) ADD_FLG(BAR_ATTKACID);
	if (p_ptr->special_attack & ATTACK_POIS) ADD_FLG(BAR_ATTKPOIS);
	if (p_ptr->special_defense & NINJA_S_STEALTH) ADD_FLG(BAR_SUPERSTEALTH);

	if (p_ptr->tim_sh_fire) ADD_FLG(BAR_SHFIRE);
	if (p_ptr->tim_sh_elements)
	{
		ADD_FLG(BAR_SHFIRE);
		if (p_ptr->lev >= 25)	
			ADD_FLG(BAR_SHCOLD);
		if (p_ptr->lev >= 35)
			ADD_FLG(BAR_SHELEC);
	}
	if (p_ptr->tim_weaponmastery) ADD_FLG(BAR_WEAPONMASTERY);

	/* tim stealth */
	if (IS_TIM_STEALTH()) ADD_FLG(BAR_STEALTH);

	if (p_ptr->tim_sh_touki) ADD_FLG(BAR_TOUKI);

	/* Holy aura */
	if (p_ptr->tim_sh_holy) ADD_FLG(BAR_SHHOLY);

	/* An Eye for an Eye */
	if (IS_REVENGE()) ADD_FLG(BAR_EYEEYE);

	if (p_ptr->tim_spurt) ADD_FLG(BAR_TIME_SPURT);
	if (p_ptr->tim_speed_essentia) ADD_FLG(BAR_SPEED_ESSENTIA);
	if (p_ptr->tim_shrike) ADD_FLG(BAR_SHRIKE);
	if (p_ptr->tim_blood_shield) ADD_FLG(BAR_BLOOD_SHIELD);
	if (p_ptr->tim_blood_seek) ADD_FLG(BAR_BLOOD_SEEK);
	if (p_ptr->tim_blood_sight) ADD_FLG(BAR_BLOOD_SIGHT);
	if (p_ptr->tim_blood_feast) ADD_FLG(BAR_BLOOD_FEAST);
	if (p_ptr->tim_blood_rite) ADD_FLG(BAR_BLOOD_RITE);
	if (p_ptr->tim_no_spells) ADD_FLG(BAR_NO_SPELLS);
	if (p_ptr->tim_blood_revenge) ADD_FLG(BAR_BLOOD_REVENGE);
	if (p_ptr->tim_genji) ADD_FLG(BAR_GENJI);
	if (p_ptr->tim_force) ADD_FLG(BAR_FORCE);
	if (p_ptr->pclass == CLASS_WEAPONMASTER)
	{
		switch (weaponmaster_get_toggle())
		{
		case TOGGLE_SHOT_ON_THE_RUN:
			ADD_FLG(BAR_SHOT_ON_THE_RUN);
			break;
		case TOGGLE_RAPID_SHOT:
			ADD_FLG(BAR_RAPID_SHOT);
			break;
		case TOGGLE_FLYING_DAGGER_STANCE:
			ADD_FLG(BAR_FLYING_DAGGER);
			break;
		case TOGGLE_SHADOW_STANCE:
			ADD_FLG(BAR_SHADOW_STANCE);
			break;
		case TOGGLE_FRENZY_STANCE:
			ADD_FLG(BAR_FRENZY_STANCE);
			break;
		case TOGGLE_COMBAT_EXPERTISE:
			ADD_FLG(BAR_COMBAT_EXPERTISE);
			break;
		case TOGGLE_STONE_BONES:
			ADD_FLG(BAR_STONE_BONES);
			break;
		case TOGGLE_TRADE_BLOWS:
			ADD_FLG(BAR_TRADE_BLOWS);
			break;
		case TOGGLE_POWER_ATTACK:
			ADD_FLG(BAR_POWER_ATTACK);
			break;
		case TOGGLE_BURNING_BLADE:
			ADD_FLG(BAR_BURNING_BLADE);
			break;
		case TOGGLE_ICE_BLADE:
			ADD_FLG(BAR_ICE_BLADE);
			break;
		case TOGGLE_THUNDER_BLADE:
			ADD_FLG(BAR_THUNDER_BLADE);
			break;
		case TOGGLE_BLOOD_BLADE:
			ADD_FLG(BAR_BLOOD_BLADE);
			break;
		case TOGGLE_HOLY_BLADE:
			ADD_FLG(BAR_HOLY_BLADE);
			break;
		case TOGGLE_ORDER_BLADE:
			ADD_FLG(BAR_ORDER_BLADE);
			break;
		case TOGGLE_WILD_BLADE:
			ADD_FLG(BAR_WILD_BLADE);
			break;
		case TOGGLE_MANY_STRIKE:
			ADD_FLG(BAR_MANY_STRIKE);
			break;
		case TOGGLE_PIERCING_STRIKE:
			ADD_FLG(BAR_PIERCING_STRIKE);
			break;
		case TOGGLE_TRIP:
			ADD_FLG(BAR_TRIP);
			break;
		case TOGGLE_FLURRY_OF_BLOWS:
			ADD_FLG(BAR_FLURRY_OF_BLOWS);
			break;
		case TOGGLE_GREATER_FLURRY:
			ADD_FLG(BAR_GREATER_FLURRY);
			break;
		case TOGGLE_STRENGTH_OF_THE_UNDERTAKER:
			ADD_FLG(BAR_STRENGTH_OF_THE_UNDERTAKER);
			break;
		case TOGGLE_STOICISM:
			ADD_FLG(BAR_STOICISM);
			break;
		case TOGGLE_INDUSTRIOUS_MORTICIAN:
			ADD_FLG(BAR_INDUSTRIOUS_MORTICIAN);
			break;
		case TOGGLE_SHIELD_BASH:
			ADD_FLG(BAR_SHIELD_BASH);
			break;
		case TOGGLE_BULWARK:
			ADD_FLG(BAR_BULWARK);
			break;
		case TOGGLE_SHIELD_REVENGE:
			ADD_FLG(BAR_EYEEYE);
			break;
		}

		if (p_ptr->entrenched) ADD_FLG(BAR_ENTRENCHED);
	}

	if (p_ptr->pclass == CLASS_MAULER)
	{
		switch (mauler_get_toggle())
		{
		case TOGGLE_WEAPON_AS_SHIELD:
			ADD_FLG(BAR_WEAPON_AS_SHIELD);
			break;
		case TOGGLE_CURSED_WOUNDS:
			ADD_FLG(BAR_CURSED_WOUNDS);
			break;
		case TOGGLE_NO_EARTHQUAKE:
			ADD_FLG(BAR_NO_EARTHQUAKE);
			break;
		case TOGGLE_DEATH_FORCE:
		{
			int a = TERM_L_DARK;
			if (p_ptr->ryoute)
			{
				if (p_ptr->fast >= 100)
					a = TERM_RED;
				else if (p_ptr->fast >= 50)
					a = TERM_ORANGE;
				else if (p_ptr->fast >= 20)
					a = TERM_YELLOW;
				else if (p_ptr->fast >= 6)
					a = TERM_SLATE;
			}
			bar[BAR_DEATH_FORCE].attr = a;
			if (p_ptr->wizard)
			{
				static char wizard_death_buf[100];
				strnfmt(wizard_death_buf, 100, "DeathForce(%d)", p_ptr->fast);
				bar[BAR_DEATH_FORCE].lstr = wizard_death_buf;
			}
			else
				bar[BAR_DEATH_FORCE].lstr = "DeathForce";

			ADD_FLG(BAR_DEATH_FORCE);
			break;
		}
		}
	}
	if (p_ptr->pclass == CLASS_PSION)
	{
		if (psion_weapon_graft()) ADD_FLG(BAR_WEAPON_GRAFT);
		if (psion_clarity()) ADD_FLG(BAR_PSIONIC_CLARITY);
		if (psion_blending()) ADD_FLG(BAR_PSIONIC_BLENDING);
		if (psion_shielding()) ADD_FLG(BAR_PSIONIC_SHIELDING);
		if (psion_combat()) ADD_FLG(BAR_PSIONIC_COMBAT);
		if (psion_mental_fortress()) ADD_FLG(BAR_MENTAL_FORTRESS);
		if (psion_mindspring()) ADD_FLG(BAR_MINDSPRING);
		if (psion_foresight()) ADD_FLG(BAR_PSIONIC_FORESIGHT);
	}

	if (p_ptr->sense_artifact) ADD_FLG(BAR_SPECIAL);

	if (p_ptr->pclass == CLASS_DUELIST)
	{
		static char duelist_buffer[100];
		ADD_FLG(BAR_DUELIST);
		if (p_ptr->duelist_target_idx)
			bar[BAR_DUELIST].attr = TERM_YELLOW;
		else
			bar[BAR_DUELIST].attr = TERM_L_DARK;
		strnfmt(duelist_buffer, 100, "%^s", duelist_current_challenge());
		bar[BAR_DUELIST].lstr = duelist_buffer;
	}

	if (p_ptr->tim_building_up) ADD_FLG(BAR_BUILD);
	if (p_ptr->tim_vicious_strike) ADD_FLG(BAR_VICIOUS_STRIKE);
	if (p_ptr->tim_enlarge_weapon) ADD_FLG(BAR_ENLARGE_WEAPON);

	if (p_ptr->fasting) ADD_FLG(BAR_FASTING);
	if (p_ptr->tim_sustain_str) ADD_FLG(BAR_SUSTAIN_STR);
	if (p_ptr->tim_sustain_int) ADD_FLG(BAR_SUSTAIN_INT);
	if (p_ptr->tim_sustain_wis) ADD_FLG(BAR_SUSTAIN_WIS);
	if (p_ptr->tim_sustain_dex) ADD_FLG(BAR_SUSTAIN_DEX);
	if (p_ptr->tim_sustain_con) ADD_FLG(BAR_SUSTAIN_CON);
	if (p_ptr->tim_sustain_chr) ADD_FLG(BAR_SUSTAIN_CHR);
	if (p_ptr->tim_hold_life) ADD_FLG(BAR_HOLD_LIFE);
	if (p_ptr->tim_transcendence) ADD_FLG(BAR_TRANSCENDENCE);
	if (p_ptr->tim_quick_walk) ADD_FLG(BAR_QUICK_WALK);
	if (p_ptr->tim_inven_prot) ADD_FLG(BAR_INVEN_PROT);

	if (p_ptr->tim_dark_stalker) ADD_FLG(BAR_DARK_STALKER);
	if (p_ptr->tim_nimble_dodge) ADD_FLG(BAR_NIMBLE_DODGE);
	if (p_ptr->tim_stealthy_snipe) ADD_FLG(BAR_STEALTHY_SNIPE);

	if (p_ptr->tim_killing_spree) ADD_FLG(BAR_KILLING_SPREE);
	if (p_ptr->tim_slay_sentient) ADD_FLG(BAR_SLAY_SENTIENT);

	if (world_player) ADD_FLG(BAR_THE_WORLD);

	/* Hex spells */
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling(HEX_BLESS)) ADD_FLG(BAR_BLESSED);
		if (hex_spelling(HEX_DEMON_AURA)) { ADD_FLG(BAR_SHFIRE); ADD_FLG(BAR_REGENERATION); }
		if (hex_spelling(HEX_XTRA_MIGHT)) ADD_FLG(BAR_MIGHT);
		if (hex_spelling(HEX_DETECT_EVIL)) ADD_FLG(BAR_ESP_EVIL);
		if (hex_spelling(HEX_ICE_ARMOR)) ADD_FLG(BAR_SHCOLD);
		if (hex_spelling(HEX_RUNESWORD)) ADD_FLG(BAR_RUNESWORD);
		if (hex_spelling(HEX_BUILDING)) ADD_FLG(BAR_BUILD);
		if (hex_spelling(HEX_ANTI_TELE)) ADD_FLG(BAR_ANTITELE);
		if (hex_spelling(HEX_SHOCK_CLOAK)) ADD_FLG(BAR_SHELEC);
		if (hex_spelling(HEX_SHADOW_CLOAK)) ADD_FLG(BAR_SHSHADOW);
		if (hex_spelling(HEX_CONFUSION)) ADD_FLG(BAR_ATTKCONF);
		if (hex_spelling(HEX_EYE_FOR_EYE)) ADD_FLG(BAR_EYEEYE);
		if (hex_spelling(HEX_ANTI_MULTI)) ADD_FLG(BAR_ANTIMULTI);
		if (hex_spelling(HEX_VAMP_BLADE)) ADD_FLG(BAR_VAMPILIC);
		if (hex_spelling(HEX_ANTI_MAGIC)) ADD_FLG(BAR_ANTIMAGIC);
		if (hex_spelling(HEX_CURE_LIGHT) ||
			hex_spelling(HEX_CURE_SERIOUS) ||
			hex_spelling(HEX_CURE_CRITICAL)) ADD_FLG(BAR_CURE);

		if (p_ptr->magic_num2[2])
		{
			if (p_ptr->magic_num2[1] == 1) ADD_FLG(BAR_PATIENCE);
			if (p_ptr->magic_num2[1] == 2) ADD_FLG(BAR_REVENGE);
		}
	}

	/* Calculate length */
	for (i = 0; bar[i].sstr; i++)
	{
		if (IS_FLG(i))
		{
			col += strlen(bar[i].lstr) + 1;
			num++;
		}
	}

	/* If there are not excess spaces for long strings, use short one */
	if (col - 1 > max_col_statbar)
	{
		space = 0;
		col = 0;

		for (i = 0; bar[i].sstr; i++)
		{
			if (IS_FLG(i))
			{
				col += strlen(bar[i].sstr);
			}
		}

		/* If there are excess spaces for short string, use more */
		if (col - 1 <= max_col_statbar - (num-1))
		{
			space = 1;
			col += num - 1;
		}
	}


	/* Centering display column */
	col = (max_col_statbar - col) / 2;

	/* Display status bar */
	for (i = 0; bar[i].sstr; i++)
	{
		if (IS_FLG(i))
		{
			cptr str;
			if (space == 2) str = bar[i].lstr;
			else str = bar[i].sstr;

			c_put_str(bar[i].attr, str, row_statbar, col);
			col += strlen(str);
			if (space > 0) col++;
			if (col > max_col_statbar) break;
		}
	}
}



/*
 * Prints "title", including "wizard" or "winner" as needed.
 */
static void prt_title(void)
{
	cptr p = "";
	char str[14];

	/* Wizard */
	if (p_ptr->wizard)
	{
#ifdef JP
		/* �����ڤ��ؤ���ǽ �ι� */
		p = "[����������]";
#else
		p = "[=-WIZARD-=]";
#endif

	}

	/* Winner */
	else if (p_ptr->total_winner || (p_ptr->lev > PY_MAX_LEVEL))
	{
		if (p_ptr->arena_number > MAX_ARENA_MONS + 2)
		{
#ifdef JP
			/* �����ڤ��ؤ���ǽ �ι� */
			p = "*����������*";
#else
			p = "*TRUEWINNER*";
#endif
		}
		else
		{
#ifdef JP
			/* �����ڤ��ؤ���ǽ �ι� */
			p = "***������***";
#else
			p = "***WINNER***";
#endif
		}
	}

	/* Normal */
	else
	{
		my_strcpy(str, player_title[p_ptr->pclass][(p_ptr->lev - 1) / 5], sizeof(str));
		p = str;
	}

	prt_field(p, ROW_TITLE, COL_TITLE);
}


/*
 * Prints level
 */
static void prt_level(void)
{
	char tmp[32];

#ifdef JP
	sprintf(tmp, "%5d", p_ptr->lev);
#else
	sprintf(tmp, "%6d", p_ptr->lev);
#endif


	if (p_ptr->lev >= p_ptr->max_plv)
	{
#ifdef JP
		put_str("��٥� ", ROW_LEVEL, 0);
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 7);
#else
		put_str("LEVEL ", ROW_LEVEL, 0);
		c_put_str(TERM_L_GREEN, tmp, ROW_LEVEL, COL_LEVEL + 6);
#endif

	}
	else
	{
#ifdef JP
		put_str("x��٥�", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 7);
#else
		put_str("Level ", ROW_LEVEL, 0);
		c_put_str(TERM_YELLOW, tmp, ROW_LEVEL, COL_LEVEL + 6);
#endif

	}
}


/*
 * Display the experience
 */
static void prt_exp(void)
{
	char out_val[32];

	if ((!exp_need)||(p_ptr->prace == RACE_ANDROID))
	{
#ifdef JP
	(void)sprintf(out_val, "%7ld", (long)p_ptr->exp);
#else
	(void)sprintf(out_val, "%8ld", (long)p_ptr->exp);
#endif
	}
	else
	{
		if (p_ptr->lev >= PY_MAX_LEVEL)
		{
			(void)sprintf(out_val, "********");
		}
		else
		{
#ifdef JP
			(void)sprintf(out_val, "%7ld", exp_requirement(p_ptr->lev) - p_ptr->exp);
#else      
			(void)sprintf(out_val, "%8ld", exp_requirement(p_ptr->lev) - p_ptr->exp);
#endif
		}
	}

	if (p_ptr->exp >= p_ptr->max_exp)
	{
#ifdef JP
		if (p_ptr->prace == RACE_ANDROID) put_str("���� ", ROW_EXP, 0);
		else put_str("�и� ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 5);
#else
		if (p_ptr->prace == RACE_ANDROID) put_str("Cst ", ROW_EXP, 0);
		else put_str("EXP ", ROW_EXP, 0);
		c_put_str(TERM_L_GREEN, out_val, ROW_EXP, COL_EXP + 4);
#endif

	}
	else
	{
#ifdef JP
		put_str("x�и�", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 5);
#else
		put_str("Exp ", ROW_EXP, 0);
		c_put_str(TERM_YELLOW, out_val, ROW_EXP, COL_EXP + 4);
#endif

	}
}

/*
 * Prints current gold
 */
static void prt_gold(void)
{
	char tmp[32];

#ifdef JP
	put_str("�� ", ROW_GOLD, COL_GOLD);
#else
	put_str("AU ", ROW_GOLD, COL_GOLD);
#endif

	sprintf(tmp, "%9ld", (long)p_ptr->au);
	c_put_str(TERM_L_GREEN, tmp, ROW_GOLD, COL_GOLD + 3);
}



/*
 * Prints current AC
 */
static void prt_ac(void)
{
	char tmp[32];

#ifdef JP
/* AC ��ɽ���������ѹ����Ƥ��� */
	put_str(" ����(     )", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 6);
#else
	put_str("Cur AC ", ROW_AC, COL_AC);
	sprintf(tmp, "%5d", p_ptr->dis_ac + p_ptr->dis_to_a);
	c_put_str(TERM_L_GREEN, tmp, ROW_AC, COL_AC + 7);
#endif

}


/*
 * Prints Cur/Max hit points
 */
static void prt_hp(void)
{
/* �ҥåȥݥ���Ȥ�ɽ����ˡ���ѹ� */
	char tmp[32];
  
	byte color;
  
	/* �����ȥ� */
/*	put_str(" �ȣС��ͣ�", ROW_HPMP, COL_HPMP); */

	put_str("HP", ROW_CURHP, COL_CURHP);

	/* ���ߤΥҥåȥݥ���� */
	sprintf(tmp, "%4ld", p_ptr->chp);

	if (p_ptr->chp >= p_ptr->mhp)
	{
		color = TERM_L_GREEN;
	}
	else if (p_ptr->chp > (p_ptr->mhp * hitpoint_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP+3);

	/* ���ڤ� */
	put_str( "/", ROW_CURHP, COL_CURHP + 7 );

	/* ����ҥåȥݥ���� */
	sprintf(tmp, "%4ld", p_ptr->mhp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURHP, COL_CURHP + 8 );
}


/*
 * Prints players max/cur spell points
 */
static void prt_sp(void)
{
/* �ޥ��å��ݥ���Ȥ�ɽ����ˡ���ѹ����Ƥ��� */
	char tmp[32];
	byte color;


	/* Do not show mana unless it matters */
	if (!mp_ptr->spell_book) return;

	/* �����ȥ� */
/*	put_str(" �ͣ� / ����", ROW_MAXSP, COL_MAXSP); */

#ifdef JP
	put_str("MP", ROW_CURSP, COL_CURSP);
#else
	put_str("SP", ROW_CURSP, COL_CURSP);
#endif

	/* ���ߤΥޥ��å��ݥ���� */
	sprintf(tmp, "%4ld", p_ptr->csp);

	if (p_ptr->csp >= p_ptr->msp)
	{
		color = TERM_L_GREEN;
	}
	else if (p_ptr->csp > (p_ptr->msp * mana_warn) / 10)
	{
		color = TERM_YELLOW;
	}
	else
	{
		color = TERM_RED;
	}

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP+3);

	/* ���ڤ� */
	put_str( "/", ROW_CURSP, COL_CURSP + 7 );

	/* ����ޥ��å��ݥ���� */
	sprintf(tmp, "%4ld", p_ptr->msp);
	color = TERM_L_GREEN;

	c_put_str(color, tmp, ROW_CURSP, COL_CURSP + 8);
}

static void prt_fear(void)
{
	int lvl = fear_level_p();

	switch (lvl)
	{
	case FEAR_BOLD:
		put_str(               "          ", ROW_FEAR, COL_FEAR);
		break;
	case FEAR_UNEASY:
		c_put_str(TERM_L_UMBER,"Uneasy    ", ROW_FEAR, COL_FEAR);
		break;
	case FEAR_NERVOUS:
		c_put_str(TERM_YELLOW, "Nervous   ", ROW_FEAR, COL_FEAR);
		break;
	case FEAR_SCARED:
		c_put_str(TERM_ORANGE, "Scared    ", ROW_FEAR, COL_FEAR);
		break;
	case FEAR_TERRIFIED:
		c_put_str(TERM_RED,    "Terrified ", ROW_FEAR, COL_FEAR);
		break;
	case FEAR_PETRIFIED:
		c_put_str(TERM_VIOLET, "Petrified ", ROW_FEAR, COL_FEAR);
		break;
	}
}


/*
 * Prints depth in stat area
 */
static void prt_depth(void)
{
	char depths[32];
	int wid, hgt, row_depth, col_depth;
	byte attr = TERM_WHITE;

	Term_get_size(&wid, &hgt);
	col_depth = wid + COL_DEPTH;
	row_depth = hgt + ROW_DEPTH;

	if (!dun_level)
	{
#ifdef JP
		strcpy(depths, "�Ͼ�");
#else
		strcpy(depths, "Surf.");
#endif
	}
	else if (p_ptr->inside_quest && !dungeon_type)
	{
#ifdef JP
		strcpy(depths, "�Ͼ�");
#else
		strcpy(depths, "Quest");
#endif
	}
	else
	{
#ifdef JP
		if (depth_in_feet) (void)sprintf(depths, "%d ft", dun_level * 50);
		else (void)sprintf(depths, "%d ��", dun_level);
#else
		if (depth_in_feet) (void)sprintf(depths, "%d ft", dun_level * 50);
		else (void)sprintf(depths, "Lev %d", dun_level);
#endif


		/* Get color of level based on feeling  -JSV- */
		switch (p_ptr->feeling)
		{
		case  0: attr = TERM_SLATE;   break; /* Unknown */
		case  1: attr = TERM_L_BLUE;  break; /* Special */
		case  2: attr = TERM_VIOLET;  break; /* Horrible visions */
		case  3: attr = TERM_RED;     break; /* Very dangerous */
		case  4: attr = TERM_L_RED;   break; /* Very bad feeling */
		case  5: attr = TERM_ORANGE;  break; /* Bad feeling */
		case  6: attr = TERM_YELLOW;  break; /* Nervous */
		case  7: attr = TERM_L_UMBER; break; /* Luck is turning */
		case  8: attr = TERM_L_WHITE; break; /* Don't like */
		case  9: attr = TERM_WHITE;   break; /* Reasonably safe */
		case 10: attr = TERM_WHITE;   break; /* Boring place */
		}
	}

	/* Right-Adjust the "depth", and clear old values */
	c_prt(attr, format("%7s", depths), row_depth, col_depth);
}


/*
 * Prints status of hunger
 */
static void prt_hunger(void)
{
	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
#ifdef JP
		c_put_str(TERM_RED, "���  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_RED, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "���  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_ORANGE, "Weak  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "��ʢ  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_YELLOW, "Hungry", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Normal */
	else if (p_ptr->food < PY_FOOD_FULL)
	{
		c_put_str(TERM_L_GREEN, "      ", ROW_HUNGRY, COL_HUNGRY);
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
#ifdef JP
		c_put_str(TERM_L_GREEN, "��ʢ  ", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_L_GREEN, "Full  ", ROW_HUNGRY, COL_HUNGRY);
#endif

	}

	/* Gorged */
	else
	{
#ifdef JP
		c_put_str(TERM_GREEN, "���᤮", ROW_HUNGRY, COL_HUNGRY);
#else
		c_put_str(TERM_GREEN, "Gorged", ROW_HUNGRY, COL_HUNGRY);
#endif

	}
}


/*
 * Prints Searching, Resting, Paralysis, or 'count' status
 * Display is always exactly 10 characters wide (see below)
 *
 * This function was a major bottleneck when resting, so a lot of
 * the text formatting code was optimized in place below.
 */
static void prt_state(void)
{
	byte attr = TERM_WHITE;

	char text[20];

	/* Repeating */
	if (command_rep)
	{
		if (command_rep > 999)
		{
#ifdef JP
sprintf(text, "%2d00", command_rep / 100);
#else
			(void)sprintf(text, "%2d00", command_rep / 100);
#endif

		}
		else
		{
#ifdef JP
sprintf(text, "  %2d", command_rep);
#else
			(void)sprintf(text, "  %2d", command_rep);
#endif

		}
	}

	/* Action */
	else
	{
		switch(p_ptr->action)
		{
			case ACTION_SEARCH:
			{
#ifdef JP
				strcpy(text, "õ��");
#else
				strcpy(text, "Sear");
#endif
				break;
			}
			case ACTION_REST:
			{
				int i;

				/* Start with "Rest" */
#ifdef JP
				strcpy(text, "    ");
#else
				strcpy(text, "    ");
#endif


				/* Extensive (timed) rest */
				if (resting >= 1000)
				{
					i = resting / 100;
					text[3] = '0';
					text[2] = '0';
					text[1] = '0' + (i % 10);
					text[0] = '0' + (i / 10);
				}

				/* Long (timed) rest */
				else if (resting >= 100)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					i = i / 10;
					text[2] = '0' + (i % 10);
					text[1] = '0' + (i / 10);
				}

				/* Medium (timed) rest */
				else if (resting >= 10)
				{
					i = resting;
					text[3] = '0' + (i % 10);
					text[2] = '0' + (i / 10);
				}

				/* Short (timed) rest */
				else if (resting > 0)
				{
					i = resting;
					text[3] = '0' + (i);
				}

				/* Rest until healed */
				else if (resting == -1)
				{
					text[0] = text[1] = text[2] = text[3] = '*';
				}

				/* Rest until done */
				else if (resting == -2)
				{
					text[0] = text[1] = text[2] = text[3] = '&';
				}
				break;
			}
			case ACTION_LEARN:
			{
#ifdef JP
				strcpy(text, "�ؽ�");
#else
				strcpy(text, "lear");
#endif
				if (new_mane) attr = TERM_L_RED;
				break;
			}
			case ACTION_FISH:
			{
#ifdef JP
				strcpy(text, "���");
#else
				strcpy(text, "fish");
#endif
				break;
			}
			case ACTION_KAMAE:
			{
				int i;
				for (i = 0; i < MAX_KAMAE; i++)
					if (p_ptr->special_defense & (KAMAE_GENBU << i)) break;
				switch (i)
				{
					case 0: attr = TERM_GREEN;break;
					case 1: attr = TERM_WHITE;break;
					case 2: attr = TERM_L_BLUE;break;
					case 3: attr = TERM_L_RED;break;
				}
				strcpy(text, kamae_shurui[i].desc);
				break;
			}
			case ACTION_KATA:
			{
				int i;
				for (i = 0; i < MAX_KATA; i++)
					if (p_ptr->special_defense & (KATA_IAI << i)) break;
				strcpy(text, kata_shurui[i].desc);
				break;
			}
			case ACTION_SING:
			{
#ifdef JP
				strcpy(text, "��  ");
#else
				strcpy(text, "Sing");
#endif
				break;
			}
			case ACTION_HAYAGAKE:
			{
#ifdef JP
				strcpy(text, "®��");
#else
				strcpy(text, "Fast");
#endif
				break;
			}
			case ACTION_SPELL:
			{
#ifdef JP
				strcpy(text, "�Ӿ�");
#else
				strcpy(text, "Spel");
#endif
				break;
			}
			default:
			{
				strcpy(text, "    ");
				break;
			}
		}
	}

	/* Display the info (or blanks) */
	c_put_str(attr, format("%5.5s",text), ROW_STATE, COL_STATE);
}


/*
 * Prints the speed of a character.			-CJS-
 */
static void prt_speed(void)
{
	int i = p_ptr->pspeed;
	bool is_fast = IS_FAST();

	byte attr = TERM_WHITE;
	char buf[32] = "";
	int wid, hgt, row_speed, col_speed;

	Term_get_size(&wid, &hgt);
	col_speed = wid + COL_SPEED;
	row_speed = hgt + ROW_SPEED;

	/* Hack -- Visually "undo" the Search Mode Slowdown */
	if (p_ptr->action == ACTION_SEARCH && !IS_LIGHT_SPEED()) i += 10;

	/* Fast */
	if (i > 110)
	{
		if (p_ptr->riding)
		{
			monster_type *m_ptr = &m_list[p_ptr->riding];
			if (MON_FAST(m_ptr) && !MON_SLOW(m_ptr)) attr = TERM_L_BLUE;
			else if (MON_SLOW(m_ptr) && !MON_FAST(m_ptr)) attr = TERM_VIOLET;
			else attr = TERM_GREEN;
		}
		else if ((is_fast && !p_ptr->slow) || IS_LIGHT_SPEED()) attr = TERM_YELLOW;
		else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_GREEN;
#ifdef JP
		sprintf(buf, "%s(+%d)", (p_ptr->riding ? "����" : "��®"), (i - 110));
#else
		sprintf(buf, "Fast(+%d)", (i - 110));
#endif

	}

	/* Slow */
	else if (i < 110)
	{
		if (p_ptr->riding)
		{
			monster_type *m_ptr = &m_list[p_ptr->riding];
			if (MON_FAST(m_ptr) && !MON_SLOW(m_ptr)) attr = TERM_L_BLUE;
			else if (MON_SLOW(m_ptr) && !MON_FAST(m_ptr)) attr = TERM_VIOLET;
			else attr = TERM_RED;
		}
		else if (is_fast && !p_ptr->slow) attr = TERM_YELLOW;
		else if (p_ptr->slow && !is_fast) attr = TERM_VIOLET;
		else attr = TERM_L_UMBER;
#ifdef JP
		sprintf(buf, "%s(-%d)", (p_ptr->riding ? "����" : "��®"), (110 - i));
#else
		sprintf(buf, "Slow(-%d)", (110 - i));
#endif
	}
	else if (p_ptr->riding)
	{
		attr = TERM_GREEN;
#ifdef JP
		strcpy(buf, "������");
#else
		strcpy(buf, "Riding");
#endif
	}

	/* Display the speed */
	c_put_str(attr, format("%-9s", buf), row_speed, col_speed);
}


static void prt_study(void)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if (p_ptr->new_spells)
	{
#ifdef JP
		put_str("�ؽ�", row_study, col_study);
#else
		put_str("Stud", row_study, col_study);
#endif

	}
	else
	{
		put_str("    ", row_study, col_study);
	}
}


static void prt_imitation(void)
{
	int wid, hgt, row_study, col_study;

	Term_get_size(&wid, &hgt);
	col_study = wid + COL_STUDY;
	row_study = hgt + ROW_STUDY;

	if (p_ptr->pclass == CLASS_IMITATOR)
	{
		if (p_ptr->mane_num)
		{
			byte attr;
			if (new_mane) attr = TERM_L_RED;
			else attr = TERM_WHITE;
#ifdef JP
			c_put_str(attr, "�ޤ�", row_study, col_study);
#else
			c_put_str(attr, "Imit", row_study, col_study);
#endif
		}
		else
		{
			put_str("    ", row_study, col_study);
		}
	}
}


static void prt_cut(void)
{
	int c = p_ptr->cut;

	if (c > 1000)
	{
#ifdef JP
		c_put_str(TERM_L_RED, "��̿��      ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_L_RED, "Mortal wound", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 200)
	{
#ifdef JP
		c_put_str(TERM_RED, "�Ҥɤ�����  ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Deep gash   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "�Ž�        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_RED, "Severe cut  ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "���Ѥʽ�    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Nasty cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 25)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "�Ҥɤ���    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_ORANGE, "Bad cut     ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c > 10)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "�ڽ�        ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Light cut   ", ROW_CUT, COL_CUT);
#endif

	}
	else if (c)
	{
#ifdef JP
		c_put_str(TERM_YELLOW, "�������    ", ROW_CUT, COL_CUT);
#else
		c_put_str(TERM_YELLOW, "Graze       ", ROW_CUT, COL_CUT);
#endif

	}
	else
	{
		put_str("            ", ROW_CUT, COL_CUT);
	}
}



static void prt_stun(void)
{
	int s = p_ptr->stun;

	if (s > 100)
	{
#ifdef JP
		c_put_str(TERM_RED, "�ռ�������  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_RED, "Knocked out ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s > 50)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "�Ҥɤ�ۯ۰  ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Heavy stun  ", ROW_STUN, COL_STUN);
#endif

	}
	else if (s)
	{
#ifdef JP
		c_put_str(TERM_ORANGE, "ۯ۰        ", ROW_STUN, COL_STUN);
#else
		c_put_str(TERM_ORANGE, "Stun        ", ROW_STUN, COL_STUN);
#endif

	}
	else
	{
		put_str("            ", ROW_STUN, COL_STUN);
	}
}



/*
 * Redraw the "monster health bar"	-DRS-
 * Rather extensive modifications by	-BEN-
 *
 * The "monster health bar" provides visual feedback on the "health"
 * of the monster currently being "tracked".  There are several ways
 * to "track" a monster, including targetting it, attacking it, and
 * affecting it (and nobody else) with a ranged attack.
 *
 * Display the monster health bar (affectionately known as the
 * "health-o-meter").  Clear health bar if nothing is being tracked.
 * Auto-track current target monster when bored.  Note that the
 * health-bar stops tracking any monster that "disappears".
 */
static void health_redraw(bool riding)
{
	s16b health_who;
	int row, col;
	monster_type *m_ptr;

	if (riding)
	{
		health_who = p_ptr->riding;
		row = ROW_RIDING_INFO;
		col = COL_RIDING_INFO;
	}
	else
	{
		health_who = p_ptr->health_who;
		row = ROW_INFO;
		col = COL_INFO;
	}

	m_ptr = &m_list[health_who];

	/* Not tracking */
	if (!health_who)
	{
		/* Erase the health bar */
		Term_erase(col, row, 12);
	}

	/* Tracking an unseen monster */
	else if (!m_ptr->ml)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a hallucinatory monster */
	else if (p_ptr->image)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a dead monster (???) */
	else if (m_ptr->hp < 0)
	{
		/* Indicate that the monster health is "unknown" */
		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");
	}

	/* Tracking a visible monster */
	else if (m_ptr->maxhp > 0 && m_ptr->max_maxhp > 0)
	{
		/* Extract the "percent" of health */
		int pct = 100L * m_ptr->hp / m_ptr->maxhp;
		int pct2 = 100L * m_ptr->hp / m_ptr->max_maxhp;

		/* Convert percent into "health" */
		int len = (pct2 < 10) ? 1 : (pct2 < 90) ? (pct2 / 10 + 1) : 10;

		/* Default to almost dead */
		byte attr = TERM_RED;

		if (MON_INVULNER(m_ptr)) attr = TERM_WHITE;
		else if (MON_CSLEEP(m_ptr)) attr = TERM_BLUE;
		else if (MON_STUNNED(m_ptr)) attr = TERM_L_BLUE;
		else if (MON_CONFUSED(m_ptr)) attr = TERM_UMBER;
		else if (MON_MONFEAR(m_ptr)) attr = TERM_VIOLET;
		else if (pct >= 100) attr = TERM_L_GREEN;
		else if (pct >= 60) attr = TERM_YELLOW;
		else if (pct >= 25) attr = TERM_ORANGE;
		else if (pct >= 10) attr = TERM_L_RED;

		Term_putstr(col, row, 12, TERM_WHITE, "[----------]");

		if (m_ptr->ego_whip_ct)
			Term_putstr(col + 1, row, len, attr, "wwwwwwwwww");
		else
			Term_putstr(col + 1, row, len, attr, "**********");
	}
}



/*
 * Display basic info (mostly left of map)
 */
static void prt_frame_basic(void)
{
	int i;

	/* Race and Class */
	{
		char buf1[100];
		char buf2[100];
		race_t *race_ptr = get_race_t();
		if (race_ptr->mimic)
		{
			sprintf(buf1, "[%s]", race_ptr->name);
			my_strcpy(buf2, buf1, 13);
		}
		else
			my_strcpy(buf2, race_ptr->name, 13);

		prt_field(buf2, ROW_RACE, COL_RACE);
	}


	/* Title */
	prt_title();

	/* Level/Experience */
	prt_level();
	prt_exp();

	/* All Stats */
	for (i = 0; i < 6; i++) prt_stat(i);

	/* Armor */
	prt_ac();

	/* Hitpoints */
	prt_hp();

	/* Spellpoints */
	prt_sp();

	/* Gold */
	prt_gold();

	/* Current depth */
	prt_depth();

	/* Special */
	health_redraw(FALSE);
	health_redraw(TRUE);
}


/*
 * Display extra info (mostly below map)
 */
static void prt_frame_extra(void)
{
	/* Cut/Stun */
	prt_cut();
	prt_stun();
	prt_fear();

	/* Food */
	prt_hunger();

	/* State */
	prt_state();

	/* Speed */
	prt_speed();

	/* Study spells */
	prt_study();

	prt_imitation();

	prt_status();
}


/*
 * Hack -- display inventory in sub-windows
 */
static void fix_inven(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_INVEN))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display inventory */
		display_inven();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display equipment in sub-windows
 */
static void fix_equip(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_EQUIP))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display equipment */
		display_equip();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display equipment in sub-windows
 */
static void fix_spell(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_SPELL))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display spell list */
		display_spell_list();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display character in sub-windows
 */
static void fix_player(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_PLAYER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		update_playtime();

		/* Display player */
		display_player(0);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}



/*
 * Hack -- display recent messages in sub-windows
 *
 * XXX XXX XXX Adjust for width and split messages
 */
static void fix_message(void)
{
	int j, i;
	int w, h;
	int x, y;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MESSAGE))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Get size */
		Term_get_size(&w, &h);

		/* Dump messages */
		for (i = 0; i < h; i++)
		{
			/* Dump the message on the appropriate line */
			Term_putstr(0, (h - 1) - i, -1, (byte)((i < now_message) ? TERM_WHITE : TERM_SLATE), message_str((s16b)i));

			/* Cursor */
			Term_locate(&x, &y);

			/* Clear to end of line */
			Term_erase(x, y, 255);
		}

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display overhead view in sub-windows
 *
 * Note that the "player" symbol does NOT appear on the map.
 */
static void fix_overhead(void)
{
	int j;

	int cy, cx;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;
		int wid, hgt;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OVERHEAD))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Full map in too small window is useless  */
		Term_get_size(&wid, &hgt);
		if (wid > COL_MAP + 2 && hgt > ROW_MAP + 2)
		{
			/* Redraw map */
			display_map(&cy, &cx);

			/* Fresh */
			Term_fresh();
		}

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display dungeon view in sub-windows
 */
static void fix_dungeon(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_DUNGEON))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Redraw dungeon view */
		display_dungeon();

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display monster recall in sub-windows
 */
static void fix_monster(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_MONSTER))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->monster_race_idx) display_roff(p_ptr->monster_race_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Hack -- display object recall in sub-windows
 */
static void fix_object(void)
{
	int j;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		term *old = Term;

		/* No window */
		if (!angband_term[j]) continue;

		/* No relevant flags */
		if (!(window_flag[j] & (PW_OBJECT))) continue;

		/* Activate */
		Term_activate(angband_term[j]);

		/* Display monster race info */
		if (p_ptr->object_kind_idx) display_koff(p_ptr->object_kind_idx);

		/* Fresh */
		Term_fresh();

		/* Restore */
		Term_activate(old);
	}
}


/*
 * Calculate number of spells player should have, and forget,
 * or remember, spells until that number is properly reflected.
 *
 * Note that this function induces various "status" messages,
 * which must be bypasses until the character is created.
 */
static void calc_spells(void)
{
	int			i, j, k, levels;
	int			num_allowed;
	int                     num_boukyaku = 0;

	magic_type		*s_ptr;
	int which;
	int bonus = 0;


	cptr p;

	/* Hack -- must be literate */
	if (!mp_ptr->spell_book) return;

	/* Hack -- wait for creation */
	if (!character_generated) return;

	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	if ((p_ptr->pclass == CLASS_SORCERER) || 
	    (p_ptr->pclass == CLASS_RED_MAGE) || 
		(p_ptr->pclass == CLASS_ARCHAEOLOGIST)|| 
		(p_ptr->pclass == CLASS_TIME_LORD)|| 
		(p_ptr->pclass == CLASS_WARLOCK)|| 
		(p_ptr->pclass == CLASS_DUELIST)|| 
		(p_ptr->pclass == CLASS_RUNE_KNIGHT)|| 
		(p_ptr->pclass == CLASS_BLOOD_KNIGHT)|| 
		(p_ptr->pclass == CLASS_MINDCRAFTER)|| 
		(p_ptr->pclass == CLASS_PSION) ||
		(p_ptr->pclass == CLASS_WILD_TALENT))
	{
		p_ptr->new_spells = 0;
		return;
	}

	p = spell_category_name(mp_ptr->spell_book);

	/* Determine the number of spells allowed */
	levels = p_ptr->lev - mp_ptr->spell_first + 1;

	/* Hack -- no negative spells */
	if (levels < 0) levels = 0;

	/* Extract total allowed spells */
	num_allowed = (adj_mag_study[p_ptr->stat_ind[mp_ptr->spell_stat]] * levels / 2);

	if ((p_ptr->pclass != CLASS_SAMURAI) && (mp_ptr->spell_book != TV_LIFE_BOOK))
	{
		bonus = 4;
	}
	if (p_ptr->pclass == CLASS_SAMURAI || p_ptr->pclass == CLASS_RAGE_MAGE)
	{
		num_allowed = 32;
	}
	else if (p_ptr->realm2 == REALM_NONE)
	{
		num_allowed = (num_allowed+1)/2;
		if (num_allowed>(32+bonus)) num_allowed = 32+bonus;
	}
	else if ((p_ptr->pclass == CLASS_MAGE) || (p_ptr->pclass == CLASS_BLOOD_MAGE) || (p_ptr->pclass == CLASS_PRIEST))
	{
		if (num_allowed>(96+bonus)) num_allowed = 96+bonus;
	}
	else
	{
		if (num_allowed>(80+bonus)) num_allowed = 80+bonus;
	}

	/* Count the number of spells we know */
	for (j = 0; j < 64; j++)
	{
		/* Count known spells */
		if ((j < 32) ?
		    (p_ptr->spell_forgotten1 & (1L << j)) :
		    (p_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			num_boukyaku++;
		}
	}

	/* See how many spells we must forget or may learn */
	p_ptr->new_spells = num_allowed + p_ptr->add_spells + num_boukyaku - p_ptr->learned_spells;

	/* Forget spells which are too hard */
	for (i = 63; i >= 0; i--)
	{
		/* Efficiency -- all done */
		if (!p_ptr->spell_learned1 && !p_ptr->spell_learned2) break;
		if (p_ptr->pclass == CLASS_RAGE_MAGE) break;

		/* Access the spell */
		j = p_ptr->spell_order[i];

		/* Skip non-spells */
		if (j >= 99) continue;


		/* Get the spell */
		if (!is_magic((j < 32) ? p_ptr->realm1 : p_ptr->realm2))
		{
			if (j < 32)
				s_ptr = &technic_info[p_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[p_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		else if (j < 32)
			s_ptr = &mp_ptr->info[p_ptr->realm1-1][j];
		else
			s_ptr = &mp_ptr->info[p_ptr->realm2-1][j%32];

		/* Skip spells we are allowed to know */
		if (s_ptr->slevel <= p_ptr->lev) continue;

		/* Is it known? */
		if ((j < 32) ?
		    (p_ptr->spell_learned1 & (1L << j)) :
		    (p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 |= (1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				p_ptr->spell_learned1 &= ~(1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Message */
#ifdef JP
			msg_format("%s��%s��˺��Ƥ��ޤä���",
				   do_spell(which, j%32, SPELL_NAME), p );
#else
			msg_format("You have forgotten the %s of %s.", p,
			do_spell(which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Forget spells if we know too many spells */
	for (i = 63; i >= 0; i--)
	{
		/* Stop when possible */
		if (p_ptr->new_spells >= 0) break;

		if (p_ptr->pclass == CLASS_RAGE_MAGE) break;

		/* Efficiency -- all done */
		if (!p_ptr->spell_learned1 && !p_ptr->spell_learned2) break;

		/* Get the (i+1)th spell learned */
		j = p_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) continue;

		/* Forget it (if learned) */
		if ((j < 32) ?
		    (p_ptr->spell_learned1 & (1L << j)) :
		    (p_ptr->spell_learned2 & (1L << (j - 32))))
		{
			/* Mark as forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 |= (1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_forgotten2 |= (1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* No longer known */
			if (j < 32)
			{
				p_ptr->spell_learned1 &= ~(1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_learned2 &= ~(1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Message */
#ifdef JP
			msg_format("%s��%s��˺��Ƥ��ޤä���",
				   do_spell(which, j%32, SPELL_NAME), p );
#else
			msg_format("You have forgotten the %s of %s.", p,
				   do_spell(which, j%32, SPELL_NAME));
#endif


			/* One more can be learned */
			p_ptr->new_spells++;
		}
	}


	/* Check for spells to remember */
	for (i = 0; i < 64; i++)
	{
		/* None left to remember */
		if (p_ptr->new_spells <= 0) break;
		if (p_ptr->pclass == CLASS_RAGE_MAGE) break;

		/* Efficiency -- all done */
		if (!p_ptr->spell_forgotten1 && !p_ptr->spell_forgotten2) break;

		/* Get the next spell we learned */
		j = p_ptr->spell_order[i];

		/* Skip unknown spells */
		if (j >= 99) break;

		/* Access the spell */
		if (!is_magic((j < 32) ? p_ptr->realm1 : p_ptr->realm2))
		{
			if (j < 32)
				s_ptr = &technic_info[p_ptr->realm1 - MIN_TECHNIC][j];
			else
				s_ptr = &technic_info[p_ptr->realm2 - MIN_TECHNIC][j%32];
		}
		else if (j<32)
			s_ptr = &mp_ptr->info[p_ptr->realm1-1][j];
		else
			s_ptr = &mp_ptr->info[p_ptr->realm2-1][j%32];

		/* Skip spells we cannot remember */
		if (s_ptr->slevel > p_ptr->lev) continue;

		/* First set of spells */
		if ((j < 32) ?
		    (p_ptr->spell_forgotten1 & (1L << j)) :
		    (p_ptr->spell_forgotten2 & (1L << (j - 32))))
		{
			/* No longer forgotten */
			if (j < 32)
			{
				p_ptr->spell_forgotten1 &= ~(1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_forgotten2 &= ~(1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Known once more */
			if (j < 32)
			{
				p_ptr->spell_learned1 |= (1L << j);
				which = p_ptr->realm1;
			}
			else
			{
				p_ptr->spell_learned2 |= (1L << (j - 32));
				which = p_ptr->realm2;
			}

			/* Message */
#ifdef JP
			msg_format("%s��%s��פ��Ф�����",
				   do_spell(which, j%32, SPELL_NAME), p );
#else
			msg_format("You have remembered the %s of %s.",
				   p, do_spell(which, j%32, SPELL_NAME));
#endif


			/* One less can be learned */
			p_ptr->new_spells--;
		}
	}

	k = 0;

	if (p_ptr->realm2 == REALM_NONE)
	{
		/* Count spells that can be learned */
		for (j = 0; j < 32; j++)
		{
			if (!is_magic(p_ptr->realm1)) s_ptr = &technic_info[p_ptr->realm1-MIN_TECHNIC][j];
			else s_ptr = &mp_ptr->info[p_ptr->realm1-1][j];

			/* Skip spells we cannot remember */
			if (s_ptr->slevel > p_ptr->lev) continue;

			/* Skip spells we already know */
			if (p_ptr->spell_learned1 & (1L << j))
			{
				continue;
			}

			/* Count it */
			k++;
		}
		if (k>32) k = 32;
		if ( p_ptr->new_spells > k 
		  && (mp_ptr->spell_book == TV_LIFE_BOOK 
		   || mp_ptr->spell_book == TV_HISSATSU_BOOK
		   || mp_ptr->spell_book == TV_RAGE_BOOK))
		{
			p_ptr->new_spells = k;
		}
	}

	if (p_ptr->new_spells < 0) p_ptr->new_spells = 0;

	/* Spell count changed */
	if (p_ptr->old_spells != p_ptr->new_spells)
	{
		/* Message if needed */
		if (p_ptr->new_spells && p_ptr->pclass != CLASS_RAGE_MAGE)
		{
			/* Message */
#ifdef JP
			if( p_ptr->new_spells < 10 ){
				msg_format("���� %d �Ĥ�%s��ؤ٤롣", p_ptr->new_spells, p);
			}else{
				msg_format("���� %d �Ĥ�%s��ؤ٤롣", p_ptr->new_spells, p);
			}
#else
			msg_format("You can learn %d more %s%s.",
				   p_ptr->new_spells, p,
				   (p_ptr->new_spells != 1) ? "s" : "");
#endif

		}

		/* Save the new_spells value */
		p_ptr->old_spells = p_ptr->new_spells;

		/* Redraw Study Status */
		p_ptr->redraw |= (PR_STUDY);

		/* Redraw object recall */
		p_ptr->window |= (PW_OBJECT);
	}
}


/*
 * Calculate maximum mana.  You do not need to know any spells.
 * Note that mana is lowered by heavy (or inappropriate) armor.
 *
 * This function induces status messages.
 */
static int _racial_mana_adjust(int i)
{               
	int     result = 0;
	race_t *race_ptr;

	/* When doppelgangers mimic, their assumed form affects their mana */
	if (p_ptr->prace == RACE_DOPPELGANGER)
		race_ptr = get_race_t();
	/* but when anybody else mimics, we continue to use their true original race */
	else
		race_ptr = get_true_race_t();
	
	/* psion's best racial modifier wins */
	if (p_ptr->pclass == CLASS_PSION)
	{
		result = MAX(race_ptr->stats[A_INT], 
					MAX(race_ptr->stats[A_WIS], race_ptr->stats[A_CHR]));
	}
	else
		result = race_ptr->stats[i];

	if (result > 5) result = 5;
	if (result < -5) result = -5;

	return result;
}

static void calc_mana(void)
{
	int		msp, levels, cur_wgt, max_wgt;

	object_type	*o_ptr;


	/* Hack -- Must be literate */
	if (!mp_ptr->spell_book) return;
	if (p_ptr->pclass == CLASS_BLOOD_MAGE)
	{ 
		p_ptr->msp = 0;
		p_ptr->csp = 0;
		p_ptr->redraw |= (PR_MANA);
		return;
	}

	if (p_ptr->pclass == CLASS_MINDCRAFTER ||
	    p_ptr->pclass == CLASS_PSION ||
	    p_ptr->pclass == CLASS_MIRROR_MASTER ||
		p_ptr->pclass == CLASS_RUNE_KNIGHT ||
	    p_ptr->pclass == CLASS_BLUE_MAGE ||
		p_ptr->pclass == CLASS_SCOUT)
	{
		levels = p_ptr->lev;
	}
	else
	{
		if (mp_ptr->spell_first > p_ptr->lev)
		{
			p_ptr->msp = 0;
			p_ptr->redraw |= (PR_MANA);
			return;
		}

		/* Extract "effective" player level */
		levels = (p_ptr->lev - mp_ptr->spell_first) + 1;
	}

	if (p_ptr->pclass == CLASS_SAMURAI)
	{
		msp = (adj_mag_mana[p_ptr->stat_ind[mp_ptr->spell_stat]] + 10) * 2;
		if (msp) msp += (msp * _racial_mana_adjust(mp_ptr->spell_stat) / 20);
	}
	else
	{
		/* Extract total mana */
		if (p_ptr->pclass == CLASS_ARCHAEOLOGIST)
		{
			int stat_idx = archaeologist_spell_stat_idx();
			msp = adj_mag_mana[stat_idx] * (levels+3) / 4;
		}
		else if (p_ptr->pclass == CLASS_PSION)
		{
			int stat_idx = psion_spell_stat_idx();
			msp = adj_mag_mana[stat_idx] * (levels+3) / 4;
		}
		else
			msp = adj_mag_mana[p_ptr->stat_ind[mp_ptr->spell_stat]] * (levels+3) / 4;

		/* Hack -- usually add one mana */
		if (msp) msp++;

		if (msp) 
		{
			int adj = _racial_mana_adjust(mp_ptr->spell_stat);
			msp += (msp * adj / 20);
		}

		if (msp && (p_ptr->personality == PERS_MUNCHKIN)) msp += msp/2;

		/* Hack: High mages et. al. have a 25% mana bonus */
		if (msp && 
		    (p_ptr->pclass == CLASS_HIGH_MAGE || p_ptr->pclass == CLASS_WILD_TALENT || p_ptr->pclass == CLASS_RAGE_MAGE))
		{
			msp += msp / 4;
		}

		if (msp && (p_ptr->pclass == CLASS_SORCERER)) msp += msp*(25+p_ptr->lev)/100;

		if (msp && p_ptr->pclass == CLASS_RUNE_KNIGHT) msp += msp/2;
	}

	/* Only mages are affected */
	if (mp_ptr->spell_xtra & MAGIC_GLOVE_REDUCE_MANA)
	{
		u32b flgs[TR_FLAG_SIZE];

		/* Assume player is not encumbered by gloves */
		p_ptr->cumber_glove = FALSE;

		/* Get the gloves */
		o_ptr = &inventory[INVEN_HANDS];

		/* Examine the gloves */
		object_flags(o_ptr, flgs);

		/* Normal gloves hurt mage-type spells */
		if (o_ptr->k_idx &&
		    !(have_flag(flgs, TR_FREE_ACT)) &&
		    !(have_flag(flgs, TR_MAGIC_MASTERY)) &&
		    !((have_flag(flgs, TR_DEX)) && (o_ptr->pval > 0)))
		{
			/* Encumbered */
			p_ptr->cumber_glove = TRUE;

			/* Reduce mana */
			msp = (3 * msp) / 4;
		}
	}


	/* Assume player not encumbered by armor */
	p_ptr->cumber_armor = FALSE;

	/* Weigh the armor */
	cur_wgt = 0;
	if(inventory[INVEN_RARM].tval> TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight;
	if(inventory[INVEN_LARM].tval> TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight;
	cur_wgt += inventory[INVEN_BODY].weight;
	cur_wgt += inventory[INVEN_HEAD].weight;
	cur_wgt += inventory[INVEN_OUTER].weight;
	cur_wgt += inventory[INVEN_HANDS].weight;
	cur_wgt += inventory[INVEN_FEET].weight;

	/* Subtract a percentage of maximum mana. */
	switch (p_ptr->pclass)
	{
		/* For these classes, mana is halved if armour 
		 * is 30 pounds over their weight limit. */
		case CLASS_MAGE:
		case CLASS_NECROMANCER:
		case CLASS_BLOOD_MAGE:
		case CLASS_HIGH_MAGE:
		case CLASS_BLUE_MAGE:
		case CLASS_MONK:
		case CLASS_FORCETRAINER:
		case CLASS_SORCERER:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight;
			break;
		}

		/* Mana halved if armour is 40 pounds over weight limit. */
		case CLASS_PRIEST:
		case CLASS_BARD:
		case CLASS_TOURIST:
		case CLASS_SCOUT:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight*2/3;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight*2/3;
			break;
		}

		case CLASS_MINDCRAFTER:
		case CLASS_PSION:
		case CLASS_BEASTMASTER:
		case CLASS_MIRROR_MASTER:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/2;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/2;
			break;
		}

		/* Mana halved if armour is 50 pounds over weight limit. */
		case CLASS_ROGUE:
		case CLASS_RANGER:
		case CLASS_RED_MAGE:
		case CLASS_WARRIOR_MAGE:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/3;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/3;
			break;
		}

		/* Mana halved if armour is 60 pounds over weight limit. */
		case CLASS_PALADIN:
		case CLASS_CHAOS_WARRIOR:
		case CLASS_RAGE_MAGE:
		{
			if (inventory[INVEN_RARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_RARM].weight/5;
			if (inventory[INVEN_LARM].tval <= TV_SWORD) cur_wgt += inventory[INVEN_LARM].weight/5;
			break;
		}

		/* For new classes created, but not yet added to this formula. */
		default:
		{
			break;
		}
	}

	/* Determine the weight allowance */
	max_wgt = mp_ptr->spell_weight;
	
	/* Heavy armor penalizes mana by a percentage.  -LM- */
	if ((cur_wgt - max_wgt) > 0)
	{
		/* Encumbered */
		p_ptr->cumber_armor = TRUE;

		/* Subtract a percentage of maximum mana. */
		switch (p_ptr->pclass)
		{
			/* For these classes, mana is halved if armour 
			 * is 30 pounds over their weight limit. */
			case CLASS_MAGE:
			case CLASS_NECROMANCER:
			case CLASS_BLOOD_MAGE:
			case CLASS_HIGH_MAGE:
			case CLASS_BLUE_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 600;
				break;
			}

			/* Mana halved if armour is 40 pounds over weight limit. */
			case CLASS_PRIEST:
			case CLASS_MINDCRAFTER:
			case CLASS_PSION:
			case CLASS_BEASTMASTER:
			case CLASS_BARD:
			case CLASS_FORCETRAINER:
			case CLASS_TOURIST:
			case CLASS_MIRROR_MASTER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}

			case CLASS_SORCERER:
			{
				msp -= msp * (cur_wgt - max_wgt) / 900;
				break;
			}

			/* Mana halved if armour is 50 pounds over weight limit. */
			case CLASS_ROGUE:
			case CLASS_RANGER:
			case CLASS_MONK:
			case CLASS_RED_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1000;
				break;
			}

			/* Mana halved if armour is 60 pounds over weight limit. */
			case CLASS_PALADIN:
			case CLASS_CHAOS_WARRIOR:
			case CLASS_WARRIOR_MAGE:
			case CLASS_RAGE_MAGE:
			{
				msp -= msp * (cur_wgt - max_wgt) / 1200;
				break;
			}

			case CLASS_SAMURAI:
			{
				p_ptr->cumber_armor = FALSE;
				break;
			}

			/* For new classes created, but not yet added to this formula. */
			default:
			{
				msp -= msp * (cur_wgt - max_wgt) / 800;
				break;
			}
		}
	}

	/* Mana can never be negative */
	if (msp < 0) msp = 0;

	/* Spell Capacity increases 10% per point */
	msp = spell_cap(msp);

	/* Maximum mana has changed */
	if (p_ptr->msp != msp)
	{
		int delta = p_ptr->msp - p_ptr->csp;
		int csp = msp - delta;

		/* Save new mana */
		p_ptr->msp = msp;

		/* Preserve the amount of used up mana whenever the total changes */
		if (p_ptr->csp > 0 && csp >= 0)
		{
			p_ptr->csp = csp;
		}

		/* Enforce maximum */
		if ((p_ptr->csp >= msp) && (p_ptr->pclass != CLASS_SAMURAI))
		{
			p_ptr->csp = msp;
			p_ptr->csp_frac = 0;
		}

		/* Display mana later */
		p_ptr->redraw |= (PR_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
		p_ptr->window |= (PW_SPELL);
	}


	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	/* Take note when "glove state" changes */
	if (p_ptr->old_cumber_glove != p_ptr->cumber_glove)
	{
		/* Message */
		if (p_ptr->cumber_glove)
		{
#ifdef JP
			msg_print("�꤬ʤ���Ƽ�ʸ�������ˤ������������롣");
#else
			msg_print("Your covered hands feel unsuitable for spellcasting.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("���μ�ξ��֤ʤ顢���äȼ�ʸ�������䤹����������");
#else
			msg_print("Your hands feel more suitable for spellcasting.");
#endif

		}

		/* Save it */
		p_ptr->old_cumber_glove = p_ptr->cumber_glove;
	}


	/* Take note when "armor state" changes */
	if (p_ptr->old_cumber_armor != p_ptr->cumber_armor)
	{
		/* Message */
		if (p_ptr->cumber_armor)
		{
#ifdef JP
			msg_print("�����νŤ���ư�����ߤ��ʤäƤ��ޤäƤ��롣");
#else
			msg_print("The weight of your equipment encumbers your movement.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("���äȳڤ��Τ�ư������褦�ˤʤä���");
#else
			msg_print("You feel able to move more freely.");
#endif

		}

		/* Save it */
		p_ptr->old_cumber_armor = p_ptr->cumber_armor;
	}
}



/*
 * Calculate the players (maximal) hit points
 * Adjust current hitpoints if necessary
 */
static void calc_hitpoints(void)
{
	int bonus, mhp;
	race_t *race_ptr = get_race_t();

	bonus = ((int)(adj_con_mhp[p_ptr->stat_ind[A_CON]]) - 128) * p_ptr->lev / 4;
	bonus += p_ptr->lev * 2;

	/* Calculate hitpoints */
	mhp = p_ptr->player_hp[p_ptr->lev - 1];

	/* Rescale HP if player is mimicking another race */
	if (race_ptr->mimic)
	{
		int hd;
		if (p_ptr->pclass == CLASS_SORCERER)
			hd = race_ptr->hd/2 + cp_ptr->c_mhp + ap_ptr->a_mhp;
		else
			hd = race_ptr->hd + cp_ptr->c_mhp + ap_ptr->a_mhp;
		mhp = mhp * hd / p_ptr->hitdie;
	}

	if (p_ptr->pclass == CLASS_SORCERER)
	{
		if (p_ptr->lev < 30)
			mhp = (mhp * (45+p_ptr->lev) / 100);
		else
			mhp = (mhp * 75 / 100);
		bonus = (bonus * 65 / 100);
	}

	mhp += bonus;

	if (p_ptr->pclass == CLASS_BERSERKER)
	{
		mhp = mhp*(110+(((p_ptr->lev + 40) * (p_ptr->lev + 40) - 1550) / 110))/100;
	}

	/* Always have at least one hitpoint per level */
	if (mhp < p_ptr->lev + 1) mhp = p_ptr->lev + 1;

	/* Factor in the hero / superhero settings */
	if (IS_HERO()) mhp += 10;
	if (IS_SHERO() && (p_ptr->pclass != CLASS_BERSERKER)) mhp += 30;
	if (p_ptr->tsuyoshi) mhp += 50;
	if (p_ptr->pclass == CLASS_WARLOCK && p_ptr->psubclass == PACT_UNDEAD) mhp += 100 * p_ptr->lev/50;

	/* Factor in the hex spell settings */
	if (hex_spelling(HEX_XTRA_MIGHT)) mhp += 15;
	if (hex_spelling(HEX_BUILDING)) mhp += 60;
	if (p_ptr->tim_building_up) 
	{
		mhp += 10 + p_ptr->lev/2;
	}
	if (mut_present(MUT_UNYIELDING))
		mhp += 3 * p_ptr->lev / 2;

	/* New maximum hitpoints */
	if (p_ptr->mhp != mhp)
	{
		/* Keep proportional hp ... I changed this for polymorph spells */
		p_ptr->chp = mhp * p_ptr->chp / p_ptr->mhp;
		p_ptr->chp_frac = 0;

		/* Save the new max-hitpoints */
		p_ptr->mhp = mhp;

		/* Display hitpoints (later) */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}
}



/*
 * Extract and set the current "lite radius"
 *
 * SWD: Experimental modification: multiple light sources have additive effect.
 *
 */
static void calc_torch(void)
{
	int i;
	object_type *o_ptr;
	u32b flgs[TR_FLAG_SIZE];

	/* Assume no light */
	p_ptr->cur_lite = 0;

	if (prace_is_(RACE_DEMIGOD) && p_ptr->psubrace == DEMIGOD_APOLLO)
		p_ptr->cur_lite++;

	/* Loop through all wielded items */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		o_ptr = &inventory[i];

		/* Examine actual lites */
		if ((i == INVEN_LITE) && (o_ptr->k_idx) && (o_ptr->tval == TV_LITE))
		{
			if (o_ptr->name2 == EGO_LITE_DARKNESS || have_flag(o_ptr->art_flags, TR_DARKNESS))
			{
				if (o_ptr->sval == SV_LITE_TORCH)
				{
					p_ptr->cur_lite -= 1;
				}

				/* Lanterns (with fuel) provide more lite */
				else if (o_ptr->sval == SV_LITE_LANTERN)
				{
					p_ptr->cur_lite -= 2;
				}

				else
				{
					p_ptr->cur_lite -= 3;
				}
			}
			/* Torches (with fuel) provide some lite */
			else if ((o_ptr->sval == SV_LITE_TORCH) && (o_ptr->xtra4 > 0))
			{
				p_ptr->cur_lite += 1;
			}

			/* Lanterns (with fuel) provide more lite */
			else if ((o_ptr->sval == SV_LITE_LANTERN) && (o_ptr->xtra4 > 0))
			{
				p_ptr->cur_lite += 2;
			}

			else if (o_ptr->sval == SV_LITE_FEANOR)
			{
				p_ptr->cur_lite += 2;
			}

			/* Artifact Lites provide permanent, bright, lite */
			else if (o_ptr->name1 || o_ptr->art_name || o_ptr->name3)
			{
				p_ptr->cur_lite += 3;
			}

			if (o_ptr->name2 == EGO_LITE_SHINE) p_ptr->cur_lite++;

			if (o_ptr->sval == SV_LITE_EYE)
			{
				p_ptr->cur_lite -= 10;
			}
		}
		else
		{
			/* Skip empty slots */
			if (!o_ptr->k_idx) continue;

			/* Extract the flags */
			object_flags(o_ptr, flgs);

			/* does this item glow? */
			if (have_flag(flgs, TR_LITE))
			{
				if ((o_ptr->name2 == EGO_DARK) || (o_ptr->name1 == ART_NIGHT)) p_ptr->cur_lite--;
				else p_ptr->cur_lite++;
			}
		}

	}

	/* max radius is 14 (was 5) without rewriting other code -- */
	/* see cave.c:update_lite() and defines.h:LITE_MAX */
	if (d_info[dungeon_type].flags1 & DF1_DARKNESS && p_ptr->cur_lite > 1)
		p_ptr->cur_lite = 1;

	/*
	 * check if the player doesn't have light radius, 
	 * but does weakly glow as an intrinsic.
	 */
	if (p_ptr->cur_lite <= 0 && p_ptr->lite) p_ptr->cur_lite++;

	if (p_ptr->cur_lite > 14) p_ptr->cur_lite = 14;
	if (p_ptr->cur_lite < 0) p_ptr->cur_lite = 0;

	/* end experimental mods */

	if (p_ptr->tim_superstealth)
	{
		p_ptr->cur_lite -= 3;
		if (p_ptr->cur_lite < 0)
			p_ptr->cur_lite = 0;
	}

	/* Notice changes in the "lite radius" */
	if (p_ptr->old_lite != p_ptr->cur_lite)
	{
		/* Update stuff */
		/* Hack -- PU_MON_LITE for monsters' darkness */
		p_ptr->update |= (PU_LITE | PU_MON_LITE | PU_MONSTERS);

		/* Remember the old lite */
		p_ptr->old_lite = p_ptr->cur_lite;

		if ((p_ptr->cur_lite > 0) && (p_ptr->special_defense & NINJA_S_STEALTH))
			set_superstealth(FALSE);
	}
}



/*
 * Computes current weight limit.
 */
u32b weight_limit(void)
{
	u32b i;

	/* Weight limit based only on strength */
	i = (u32b)adj_str_wgt[p_ptr->stat_ind[A_STR]] * 50; /* Constant was 100 */
	if (p_ptr->pclass == CLASS_BERSERKER) i = i * 3 / 2;

	/* Return the result */
	return i;
}


bool buki_motteruka(int i)
{
	bool result = FALSE;
	if (inventory[i].k_idx)
	{
		/* With shield bash, weapons are no longer weapons, but now shields
		   are weapons.  Also, only one shield becomes a weapon, and that could
		   be your right hand (dual wielding shields) or your left hand. */
		if (weaponmaster_get_toggle() == TOGGLE_SHIELD_BASH)
		{
			if (object_is_shield(&inventory[i]))
			{
				if (i == INVEN_LARM  /* Dual wielding shields?  The right hand shield is a weapon, but not the left */
				  && inventory[INVEN_RARM].k_idx 
				  && object_is_shield(&inventory[INVEN_RARM]) )
				{
					result = FALSE;
				}
				else
					result = TRUE;
			}
		}
		else if (object_is_melee_weapon(&inventory[i])) 
			result = TRUE;
	}
	return result;
}


/*
 * Calculate the players current "state", taking into account
 * not only race/class intrinsics, but also objects being worn
 * and temporary spell effects.
 *
 * See also calc_mana() and calc_hitpoints().
 *
 * Take note of the new "speed code", in particular, a very strong
 * player will start slowing down as soon as he reaches 150 pounds,
 * but not until he reaches 450 pounds will he be half as fast as
 * a normal kobold.  This both hurts and helps the player, hurts
 * because in the old days a player could just avoid 300 pounds,
 * and helps because now carrying 300 pounds is not very painful.
 *
 * The "weapon" and "bow" do *not* add to the bonuses to hit or to
 * damage, since that would affect non-combat things.  These values
 * are actually added in later, at the appropriate place.
 *
 * This function induces various "status" messages.
 */
void calc_bonuses(void)
{
	int             i, j, hold, neutral[2];
	s16b            old_speed = p_ptr->pspeed;
	int             default_hand = 0;
	int             empty_hands_status = empty_hands(TRUE);
	int             extra_blows[2];
	int             extra_shots;
	object_type     *o_ptr;
	u32b flgs[TR_FLAG_SIZE];
	bool            yoiyami = FALSE;
	bool            down_saving = FALSE;
#if 0
	bool            have_dd_s = FALSE, have_dd_t = FALSE;
#endif
	bool            have_sw = FALSE, have_kabe = FALSE;
	bool            easy_2weapon = FALSE;
	bool            riding_levitation = FALSE;
	s16b this_o_idx, next_o_idx = 0;
	class_t *class_ptr = get_class_t();
	race_t *race_ptr = get_race_t();

	/* Save the old vision stuff */
	bool old_telepathy = p_ptr->telepathy;
	bool old_esp_animal = p_ptr->esp_animal;
	bool old_esp_undead = p_ptr->esp_undead;
	bool old_esp_demon = p_ptr->esp_demon;
	bool old_esp_orc = p_ptr->esp_orc;
	bool old_esp_troll = p_ptr->esp_troll;
	bool old_esp_giant = p_ptr->esp_giant;
	bool old_esp_dragon = p_ptr->esp_dragon;
	bool old_esp_human = p_ptr->esp_human;
	bool old_esp_evil = p_ptr->esp_evil;
	bool old_esp_good = p_ptr->esp_good;
	bool old_esp_nonliving = p_ptr->esp_nonliving;
	bool old_esp_unique = p_ptr->esp_unique;
	bool old_esp_magical = p_ptr->esp_magical;
	bool old_see_inv = p_ptr->see_inv;
	bool old_mighty_throw = p_ptr->mighty_throw;

	/* Save the old armor class */
	s16b old_dis_ac = p_ptr->dis_ac;
	s16b old_dis_to_a = p_ptr->dis_to_a;


	/* Clear extra blows/shots */
	extra_blows[0] = extra_blows[1] = extra_shots = 0;
	if (p_ptr->tim_speed_essentia)
		extra_shots += 10;

	/* Clear the stat modifiers */
	for (i = 0; i < 6; i++) p_ptr->stat_add[i] = 0;


	/* Clear the Displayed/Real armor class */
	p_ptr->dis_ac = p_ptr->ac = 0;

	/* Clear the Displayed/Real Bonuses */
	p_ptr->dis_to_h_b = p_ptr->to_h_b = 0;
	p_ptr->dis_to_d_b = p_ptr->to_d_b = 0;
	p_ptr->dis_to_a = p_ptr->to_a = 0;
	p_ptr->to_h_m = 0;
	p_ptr->to_d_m = 0;
	p_ptr->easy_2weapon = FALSE;
	if (p_ptr->tim_genji) easy_2weapon = TRUE;
	if (mut_present(MUT_AMBIDEXTROUS)) easy_2weapon = TRUE;
	p_ptr->speciality1_equip = FALSE;
	p_ptr->speciality2_equip = FALSE;
	p_ptr->sneak_attack = FALSE;

	p_ptr->to_m_chance = 0;

	for (i = 0; i < 2; i++)
	{
		p_ptr->weapon_info[i].dis_to_h = 0;
		p_ptr->weapon_info[i].to_h = 0;
		p_ptr->weapon_info[i].dis_to_d = 0;
		p_ptr->weapon_info[i].to_d = 0;

		p_ptr->weapon_info[i].to_dd = 0;
		p_ptr->weapon_info[i].to_ds = 0;
		p_ptr->weapon_info[i].brand_acid = FALSE;
		p_ptr->weapon_info[i].brand_fire = FALSE;
		p_ptr->weapon_info[i].brand_cold = FALSE;
		p_ptr->weapon_info[i].brand_elec = FALSE;
		p_ptr->weapon_info[i].brand_pois = FALSE;

		p_ptr->weapon_info[i].num_blow = 1;
	}

	/* Start with "normal" speed */
	p_ptr->pspeed = 110;

	/* Start with a single shot per turn */
	p_ptr->num_fire = 100;

	/* Reset the "xtra" tval */
	p_ptr->tval_xtra = 0;

	/* Reset the "ammo" tval */
	p_ptr->tval_ammo = 0;

	/* Clear all the flags */
	p_ptr->cursed = 0L;
	p_ptr->bless_blade = FALSE;
	p_ptr->xtra_might = FALSE;
	p_ptr->impact[0] = FALSE;
	p_ptr->impact[1] = FALSE;
	p_ptr->pass_wall = FALSE;
	p_ptr->kill_wall = FALSE;
	p_ptr->dec_mana = FALSE;
	p_ptr->spell_power = 0;
	p_ptr->device_power = 0;
	p_ptr->spell_cap = 0;
	p_ptr->easy_spell = FALSE;
	p_ptr->heavy_spell = FALSE;
	p_ptr->see_inv = FALSE;
	p_ptr->free_act = FALSE;
	p_ptr->slow_digest = FALSE;
	p_ptr->regenerate = FALSE;
	p_ptr->can_swim = FALSE;
	p_ptr->levitation = FALSE;
	p_ptr->hold_life = FALSE;
	p_ptr->telepathy = FALSE;
	p_ptr->esp_animal = FALSE;
	p_ptr->esp_undead = FALSE;
	p_ptr->esp_demon = FALSE;
	p_ptr->esp_orc = FALSE;
	p_ptr->esp_troll = FALSE;
	p_ptr->esp_giant = FALSE;
	p_ptr->esp_dragon = FALSE;
	p_ptr->esp_human = FALSE;
	p_ptr->esp_evil = FALSE;
	p_ptr->esp_good = FALSE;
	p_ptr->esp_nonliving = FALSE;
	p_ptr->esp_unique = FALSE;
	p_ptr->esp_magical = FALSE;
	p_ptr->lite = FALSE;
	p_ptr->sustain_str = FALSE;
	p_ptr->sustain_int = FALSE;
	p_ptr->sustain_wis = FALSE;
	p_ptr->sustain_con = FALSE;
	p_ptr->sustain_dex = FALSE;
	p_ptr->sustain_chr = FALSE;
	p_ptr->resist_acid = FALSE;
	p_ptr->resist_elec = FALSE;
	p_ptr->resist_fire = FALSE;
	p_ptr->resist_cold = FALSE;
	p_ptr->resist_pois = FALSE;
	p_ptr->resist_conf = FALSE;
	p_ptr->resist_sound = FALSE;
	p_ptr->resist_lite = FALSE;
	p_ptr->resist_dark = FALSE;
	p_ptr->resist_chaos = FALSE;
	p_ptr->resist_disen = FALSE;
	p_ptr->resist_shard = FALSE;
	p_ptr->resist_nexus = FALSE;
	p_ptr->resist_blind = FALSE;
	p_ptr->resist_neth = FALSE;
	p_ptr->resist_time = FALSE;
	p_ptr->resist_fear = FALSE;
	p_ptr->reflect = FALSE;
	p_ptr->sh_fire = FALSE;
	p_ptr->sh_elec = FALSE;
	p_ptr->sh_cold = FALSE;
	p_ptr->anti_magic = FALSE;
	p_ptr->anti_tele = FALSE;
	p_ptr->warning = FALSE;
	p_ptr->mighty_throw = FALSE;
	p_ptr->see_nocto = FALSE;
	p_ptr->easy_realm1 = REALM_NONE;

	p_ptr->magic_absorption = 0;
	p_ptr->magic_resistance = 0;
	p_ptr->good_luck = FALSE;
	p_ptr->rune_regen = FALSE;
	p_ptr->rune_elem_prot = FALSE;
	p_ptr->rune_mind = FALSE;
	
	p_ptr->immune_acid = FALSE;
	p_ptr->immune_elec = FALSE;
	p_ptr->immune_fire = FALSE;
	p_ptr->immune_cold = FALSE;

	p_ptr->ryoute = FALSE;
	p_ptr->omoi = FALSE;
	p_ptr->migite = FALSE;
	p_ptr->hidarite = FALSE;
	p_ptr->no_flowed = FALSE;

	p_ptr->unlimited_quiver = FALSE;
	p_ptr->return_ammo = FALSE;
	p_ptr->big_shot = FALSE;
	p_ptr->painted_target = FALSE;
	p_ptr->enhanced_crit = FALSE;
	p_ptr->cleave = FALSE;
	p_ptr->constant_hero = FALSE;
	p_ptr->vorpal = FALSE;
	p_ptr->whirlwind = FALSE;
	p_ptr->entrenched = FALSE;
	p_ptr->lightning_reflexes = FALSE;
	p_ptr->inven_prot = FALSE;
	p_ptr->ambush = FALSE;
	p_ptr->peerless_stealth = FALSE;
	p_ptr->open_terrain_ct = 0;

	p_ptr->quick_walk = FALSE;

	p_ptr->align = friend_align;
	p_ptr->maul_of_vice = FALSE;


	if (p_ptr->tim_sustain_str) p_ptr->sustain_str = TRUE;
	if (p_ptr->tim_sustain_int) p_ptr->sustain_int = TRUE;
	if (p_ptr->tim_sustain_wis) p_ptr->sustain_wis = TRUE;
	if (p_ptr->tim_sustain_dex) p_ptr->sustain_dex = TRUE;
	if (p_ptr->tim_sustain_con) p_ptr->sustain_con = TRUE;
	if (p_ptr->tim_sustain_chr) p_ptr->sustain_chr = TRUE;
	if (p_ptr->tim_hold_life) p_ptr->hold_life = TRUE;
	if (p_ptr->tim_inven_prot) p_ptr->inven_prot = TRUE;
	if (p_ptr->tim_quick_walk) p_ptr->quick_walk = TRUE;

	if (mut_present(MUT_FLEET_OF_FOOT)) p_ptr->quick_walk = TRUE;

	/* Base infravision (purely racial) */
	p_ptr->see_infra = race_ptr->infra;

	/* calc_skills() */
	{
		skills_t c_base = {0};
		skills_t c_extra = {0};
		skills_t a_extra = ap_ptr->skills;

		if (class_ptr)
		{
			c_base = class_ptr->base_skills;
			c_extra = class_ptr->extra_skills;
		}
		else
		{
			c_base = cp_ptr->base_skills;
			c_extra = cp_ptr->extra_skills;
		}

		skills_scale(&c_extra, p_ptr->lev, 10);

		a_extra.stl = 0; /* Hengband never gave extra personality stealth with level ... */
		skills_scale(&a_extra, p_ptr->lev, 50);

		skills_init(&p_ptr->skills);
		skills_add(&p_ptr->skills, &c_base);
		skills_add(&p_ptr->skills, &c_extra);
		skills_add(&p_ptr->skills, &race_ptr->skills);
		skills_add(&p_ptr->skills, &ap_ptr->skills);
		skills_add(&p_ptr->skills, &a_extra);
	}
	if (p_ptr->personality == PERS_FEARLESS)
		p_ptr->resist_fear = TRUE;

	/* Base skill -- combat (throwing) */
	p_ptr->skill_tht = p_ptr->skills.thb;

	/* Base skill -- digging */
	p_ptr->skill_dig = 0;

	if (buki_motteruka(INVEN_RARM)) p_ptr->migite = TRUE;
	if (buki_motteruka(INVEN_LARM))
	{
		p_ptr->hidarite = TRUE;
		if (!p_ptr->migite) default_hand = 1;
	}

	if (CAN_TWO_HANDS_WIELDING())
	{
		if (p_ptr->migite && (empty_hands(FALSE) == EMPTY_HAND_LARM) &&
			object_allow_two_hands_wielding(&inventory[INVEN_RARM]))
		{
			p_ptr->ryoute = TRUE;
		}
		else if (p_ptr->hidarite && (empty_hands(FALSE) == EMPTY_HAND_RARM) &&
			object_allow_two_hands_wielding(&inventory[INVEN_LARM]))
		{
			p_ptr->ryoute = TRUE;
		}
		else
		{
			switch (p_ptr->pclass)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if (empty_hands(FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))
				{
					p_ptr->migite = TRUE;
					p_ptr->ryoute = TRUE;
				}
				break;
			case CLASS_WEAPONMASTER:
				if (strcmp(weaponmaster_speciality1_name(), "Shields") == 0)
				{
					if (weaponmaster_get_toggle() == TOGGLE_SHIELD_BASH)
					{
						p_ptr->ryoute = TRUE;
					}
					else if ( inventory[INVEN_LARM].k_idx
					  && object_is_shield(&inventory[INVEN_LARM])
					  && buki_motteruka(INVEN_RARM) 
					  && object_allow_two_hands_wielding(&inventory[INVEN_RARM]) )
					{
						p_ptr->ryoute = TRUE;
					}
				}
				break;
			}
		}
	}

	if (!p_ptr->migite && !p_ptr->hidarite)
	{
		if (empty_hands_status & EMPTY_HAND_RARM) p_ptr->migite = (p_ptr->pclass != CLASS_NECROMANCER);
		else if (empty_hands_status == EMPTY_HAND_LARM)
		{
			p_ptr->hidarite = TRUE;
			default_hand = 1;
		}
	}

	if (p_ptr->special_defense & KAMAE_MASK)
	{
		if (p_ptr->pclass != CLASS_WILD_TALENT && !(empty_hands_status & EMPTY_HAND_RARM))
		{
			set_action(ACTION_NONE);
		}
	}

	if (p_ptr->tim_superstealth)
		p_ptr->see_nocto = TRUE;

	if (inventory[INVEN_LITE].k_idx)
	{
		switch (inventory[INVEN_LITE].name1)
		{
		case ART_EYE_OF_VECNA:
			p_ptr->see_nocto = TRUE;
			break;
		case ART_STONE_OF_NATURE:
			p_ptr->easy_realm1 = REALM_NATURE;
			break;
		case ART_STONE_OF_LIFE:
			p_ptr->easy_realm1 = REALM_LIFE;
			break;
		case ART_STONE_OF_SORCERY:
			p_ptr->easy_realm1 = REALM_SORCERY;
			break;
		case ART_STONE_OF_CHAOS:
			p_ptr->easy_realm1 = REALM_CHAOS;
			break;
		case ART_STONE_OF_DEATH:
			p_ptr->easy_realm1 = REALM_DEATH;
			break;
		case ART_STONE_OF_TRUMP:
			p_ptr->easy_realm1 = REALM_TRUMP;
			break;
		case ART_STONE_OF_DAEMON:
			p_ptr->easy_realm1 = REALM_DAEMON;
			break;
		case ART_STONE_OF_CRUSADE:
			p_ptr->easy_realm1 = REALM_CRUSADE;
			break;
		case ART_STONE_OF_CRAFT:
			p_ptr->easy_realm1 = REALM_CRAFT;
			break;
		}
	}

	switch (p_ptr->pclass)
	{
	case CLASS_HIGH_MAGE:
		p_ptr->spell_power += p_ptr->base_spell_power; 
		p_ptr->device_power += p_ptr->base_spell_power; 
		p_ptr->stat_add[A_STR] -= p_ptr->base_spell_power;
		p_ptr->stat_add[A_INT] += p_ptr->base_spell_power;
		p_ptr->stat_add[A_DEX] -= p_ptr->base_spell_power;
		p_ptr->stat_add[A_CON] -= p_ptr->base_spell_power;
		break;
	case CLASS_WARRIOR:
		if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 44) p_ptr->regenerate = TRUE;
		break;
	case CLASS_PALADIN:
		if (p_ptr->lev > 39) p_ptr->resist_fear = TRUE;
		break;
	case CLASS_CHAOS_WARRIOR:
		if (p_ptr->lev > 29) p_ptr->resist_chaos = TRUE;
		if (p_ptr->lev > 39) p_ptr->resist_fear = TRUE;
		break;

	case CLASS_MONK:
	case CLASS_FORCETRAINER:
		/* Unencumbered Monks become faster every 10 levels */
		if (!(heavy_armor()))
		{
			if (p_ptr->personality != PERS_MUNCHKIN)
				p_ptr->pspeed += (p_ptr->lev) / 10;

			/* Free action if unencumbered at level 25 */
			if  (p_ptr->lev > 24)
				p_ptr->free_act = TRUE;
		}
		break;
	case CLASS_SORCERER:
		p_ptr->to_a -= 50;
		p_ptr->dis_to_a -= 50;
		break;
	case CLASS_BARD:
		p_ptr->resist_sound = TRUE;
		break;
	case CLASS_SAMURAI:
		if (p_ptr->lev > 29) p_ptr->resist_fear = TRUE;
		break;
	case CLASS_BERSERKER:
		p_ptr->shero = 1;
		p_ptr->sustain_str = TRUE;
		p_ptr->sustain_dex = TRUE;
		p_ptr->sustain_con = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->pspeed += 2;
		if (p_ptr->lev > 29) p_ptr->pspeed++;
		if (p_ptr->lev > 39) p_ptr->pspeed++;
		if (p_ptr->lev > 44) p_ptr->pspeed++;
		if (p_ptr->lev > 49) p_ptr->pspeed++;
		p_ptr->to_a += 10+p_ptr->lev/2;
		p_ptr->dis_to_a += 10+p_ptr->lev/2;
		p_ptr->skill_dig += (100+p_ptr->lev*8);
		if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
		p_ptr->redraw |= PR_STATUS;
		break;
	case CLASS_MIRROR_MASTER:
		if (p_ptr->lev > 39) p_ptr->reflect = TRUE;
		break;
	case CLASS_NINJA:
		/* Unencumbered Ninjas become faster every 10 levels */
		if (heavy_armor())
		{
			p_ptr->pspeed -= (p_ptr->lev) / 10;
			p_ptr->skills.stl -= (p_ptr->lev)/10;
		}
		else if ((!inventory[INVEN_RARM].k_idx || p_ptr->migite) &&
			        (!inventory[INVEN_LARM].k_idx || p_ptr->hidarite))
		{
			p_ptr->pspeed += 3;
			if (p_ptr->personality != PERS_MUNCHKIN)
				p_ptr->pspeed += (p_ptr->lev) / 10;
			p_ptr->skills.stl += (p_ptr->lev)/10;

			/* Free action if unencumbered at level 25 */
			if  (p_ptr->lev > 24)
				p_ptr->free_act = TRUE;
		}
		if ((!inventory[INVEN_RARM].k_idx || p_ptr->migite) &&
			(!inventory[INVEN_LARM].k_idx || p_ptr->hidarite))
		{
			p_ptr->to_a += p_ptr->lev/2+5;
			p_ptr->dis_to_a += p_ptr->lev/2+5;
		}
		p_ptr->slow_digest = TRUE;
		p_ptr->resist_fear = TRUE;
		if (p_ptr->lev > 19) p_ptr->resist_pois = TRUE;
		if (p_ptr->lev > 24) p_ptr->sustain_dex = TRUE;
		if (p_ptr->lev > 29) p_ptr->see_inv = TRUE;
		if (p_ptr->lev > 44)
		{
			p_ptr->oppose_pois = 1;
			p_ptr->redraw |= PR_STATUS;
		}
		p_ptr->see_nocto = TRUE;
		break;

	}

	if (p_ptr->ult_res || (p_ptr->special_defense & KATA_MUSOU))
	{
		p_ptr->see_inv = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->slow_digest = TRUE;
		p_ptr->regenerate = TRUE;
		p_ptr->levitation = TRUE;

		if (p_ptr->special_defense & KATA_MUSOU)
		{
			p_ptr->hold_life = TRUE;
			p_ptr->sustain_str = TRUE;
			p_ptr->sustain_int = TRUE;
			p_ptr->sustain_wis = TRUE;
			p_ptr->sustain_con = TRUE;
			p_ptr->sustain_dex = TRUE;
			p_ptr->sustain_chr = TRUE;
		}

		p_ptr->telepathy = TRUE;
		p_ptr->lite = TRUE;
		p_ptr->resist_acid = TRUE;
		p_ptr->resist_elec = TRUE;
		p_ptr->resist_fire = TRUE;
		p_ptr->resist_cold = TRUE;
		p_ptr->resist_pois = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->resist_sound = TRUE;
		p_ptr->resist_lite = TRUE;
		p_ptr->resist_dark = TRUE;
		p_ptr->resist_chaos = TRUE;
		p_ptr->resist_disen = TRUE;
		p_ptr->resist_shard = TRUE;
		p_ptr->resist_nexus = TRUE;
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_neth = TRUE;
		p_ptr->resist_fear = TRUE;
		p_ptr->reflect = TRUE;
		p_ptr->sh_fire = TRUE;
		p_ptr->sh_elec = TRUE;
		p_ptr->sh_cold = TRUE;
		p_ptr->to_a += 100;
		p_ptr->dis_to_a += 100;
	}
	/* Temporary shield */
	else if (p_ptr->tsubureru || IS_STONE_SKIN() || p_ptr->magicdef)
	{
		p_ptr->to_a += 50;
		p_ptr->dis_to_a += 50;
	}

	if (p_ptr->tim_res_nether)
	{
		p_ptr->resist_neth = TRUE;
	}
	
	if (p_ptr->tim_res_disenchantment)
		p_ptr->resist_disen = TRUE;
	
	if (p_ptr->tim_sh_fire)
	{
		p_ptr->sh_fire = TRUE;
	}

	if (p_ptr->tim_sh_elements)
	{
		p_ptr->sh_fire = TRUE;
		if (p_ptr->lev >= 25)
			p_ptr->sh_cold = TRUE;
		if (p_ptr->lev >= 35)
			p_ptr->sh_elec = TRUE;
	}

	if (p_ptr->tim_res_time)
	{
		p_ptr->resist_time = TRUE;
	}

	/* Sexy Gal */
	if (p_ptr->personality == PERS_SEXY) p_ptr->cursed |= (TRC_AGGRAVATE);
	if (p_ptr->personality == PERS_LAZY) p_ptr->to_m_chance += 10;
	if (p_ptr->personality == PERS_SHREWD) p_ptr->to_m_chance -= 3;
	if ((p_ptr->personality == PERS_PATIENT) || (p_ptr->personality == PERS_MIGHTY)) p_ptr->to_m_chance++;

	/* Lucky man 
	   TODO: This will become a birth event!
	*/
	if ( p_ptr->personality == PERS_LUCKY
	  && !mut_present(MUT_GOOD_LUCK) )
	{
		mut_gain(MUT_GOOD_LUCK);
		mut_lock(MUT_GOOD_LUCK);
	}

	if (mut_present(MUT_GOOD_LUCK))
		p_ptr->good_luck = TRUE;

	if (p_ptr->personality == PERS_MUNCHKIN)
	{
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_conf  = TRUE;
		p_ptr->hold_life = TRUE;
		if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;

		if ((p_ptr->prace != RACE_KLACKON) && (p_ptr->prace != RACE_SPRITE))
			/* Munchkin become faster */
			p_ptr->pspeed += (p_ptr->lev) / 10 + 5;
	}

	if (music_singing(MUSIC_WALL))
	{
		p_ptr->kill_wall = TRUE;
	}

	/* Hack -- apply racial/class stat maxes */
	/* Apply the racial modifiers */
	for (i = 0; i < 6; i++)
	{
		if (class_ptr)
			p_ptr->stat_add[i] += (race_ptr->stats[i] + class_ptr->stats[i] + ap_ptr->a_adj[i]);
		else
			p_ptr->stat_add[i] += (race_ptr->stats[i] + cp_ptr->c_adj[i] + ap_ptr->a_adj[i]);
	}


	/* I'm adding the mutations here for the lack of a better place... */
	mut_calc_bonuses();
	if (mut_present(MUT_ILL_NORM))
		p_ptr->stat_add[A_CHR] = 0;

	if (p_ptr->tsuyoshi)
	{
		p_ptr->stat_add[A_STR] += 4;
		p_ptr->stat_add[A_CON] += 4;
	}

	/* Some Runes work when placed in Inventory */
	for (i = 0; i < INVEN_RARM; i++)
	{
		o_ptr = &inventory[i];
		if (!o_ptr->k_idx) continue;
		if (o_ptr->name1 == ART_MAUL_OF_VICE)
			p_ptr->maul_of_vice = TRUE;
		if (o_ptr->rune_flags & RUNE_ELEMENTAL_PROTECTION) 
			p_ptr->rune_elem_prot = TRUE;
		if (o_ptr->rune_flags & RUNE_GOOD_FORTUNE) 
			p_ptr->good_luck = TRUE;
	}

	/* Scan the usable inventory */
	for (i = INVEN_RARM; i < INVEN_TOTAL; i++)
	{
		int bonus_to_h, bonus_to_d;
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Extract the item flags */
		object_flags(o_ptr, flgs);

		p_ptr->cursed |= (o_ptr->curse_flags & (0xFFFFFFF0L));
		if (o_ptr->name1 == ART_CHAINSWORD) p_ptr->cursed |= TRC_CHAINSWORD;

		if (o_ptr->name1 == ART_MAUL_OF_VICE)
			p_ptr->maul_of_vice = TRUE;

		/* Runes */
		if (o_ptr->rune_flags & RUNE_ABSORPTION)
		{
			p_ptr->magic_resistance += 25;
			p_ptr->magic_absorption += 5;
		}
		if (o_ptr->rune_flags & RUNE_REGENERATION) p_ptr->rune_regen = TRUE;
		if (o_ptr->rune_flags & RUNE_DEFLECTION)
		{
			p_ptr->to_a += 5 + p_ptr->lev/2;
			p_ptr->dis_to_a += 5 + p_ptr->lev/2;
		}
		if (o_ptr->rune_flags & RUNE_STASIS)
		{
			if (p_ptr->lev >= 15)
				p_ptr->sustain_str = TRUE;
		
			if (p_ptr->lev >= 20)
				p_ptr->sustain_dex = TRUE;
		
			if (p_ptr->lev >= 25)
				p_ptr->sustain_int = TRUE;

			if (p_ptr->lev >= 30)
				p_ptr->sustain_con = TRUE;

			if (p_ptr->lev >= 35)
			{
				p_ptr->sustain_wis = TRUE;
				p_ptr->sustain_chr = TRUE;
			}

			if (p_ptr->lev >= 40)
				p_ptr->hold_life = TRUE;

			if (p_ptr->lev >= 45)
				p_ptr->resist_disen = TRUE;

			if (p_ptr->lev >= 50)
				p_ptr->resist_time = TRUE;
		}
		if (o_ptr->rune_flags & RUNE_BODY)
		{
			p_ptr->stat_add[A_STR] += 4;
			p_ptr->stat_add[A_DEX] += 4;
			p_ptr->stat_add[A_CON] += 4;
			p_ptr->resist_nexus = TRUE;
		}
		if (o_ptr->rune_flags & RUNE_MIND)
		{
			p_ptr->stat_add[A_INT] += 4;
			p_ptr->telepathy = TRUE;
			p_ptr->resist_conf = TRUE;
			p_ptr->resist_chaos = TRUE;
			p_ptr->rune_mind = TRUE;
		}

		/* Affect stats */
		if (have_flag(flgs, TR_STR)) p_ptr->stat_add[A_STR] += o_ptr->pval;
		if (have_flag(flgs, TR_INT)) p_ptr->stat_add[A_INT] += o_ptr->pval;
		if (have_flag(flgs, TR_WIS)) p_ptr->stat_add[A_WIS] += o_ptr->pval;
		if (have_flag(flgs, TR_DEX)) p_ptr->stat_add[A_DEX] += o_ptr->pval;
		if (have_flag(flgs, TR_CON)) p_ptr->stat_add[A_CON] += o_ptr->pval;
		if (have_flag(flgs, TR_CHR)) p_ptr->stat_add[A_CHR] += o_ptr->pval;

		if (have_flag(flgs, TR_MAGIC_MASTERY))
		{
		    p_ptr->skills.dev += 8*o_ptr->pval;
			p_ptr->device_power += o_ptr->pval;
		}

		/* Affect stealth */
		if (have_flag(flgs, TR_STEALTH)) p_ptr->skills.stl += o_ptr->pval;

		/* Affect searching ability (factor of five) */
		if (have_flag(flgs, TR_SEARCH)) p_ptr->skills.srh += (o_ptr->pval * 5);

		/* Affect searching frequency (factor of five) */
		if (have_flag(flgs, TR_SEARCH)) p_ptr->skills.fos += (o_ptr->pval * 5);

		/* Affect infravision */
		if (have_flag(flgs, TR_INFRA)) p_ptr->see_infra += o_ptr->pval;

		/* Affect digging (factor of 20) */
		if (have_flag(flgs, TR_TUNNEL)) p_ptr->skill_dig += (o_ptr->pval * 20);

		/* Affect speed */
		if (have_flag(flgs, TR_SPEED)) p_ptr->pspeed += o_ptr->pval;

		/* Affect blows */
		if (have_flag(flgs, TR_BLOWS))
		{
			if (p_ptr->pclass == CLASS_MAULER && o_ptr->pval > 0)
			{
				/*Mauler: Cannot gain Extra Blows from equipment 
				  (penalties to blows, such as from Master Tonberry, still apply) */
			}
			else
			{
				if((i == INVEN_RARM || i == INVEN_RIGHT) && !p_ptr->ryoute) 
					extra_blows[0] += o_ptr->pval;
				else if((i == INVEN_LARM || i == INVEN_LEFT) && !p_ptr->ryoute) 
					extra_blows[1] += o_ptr->pval;
				else /* Shiva's Jacket, Ares' Helm, Berserker Gloves */
				{
					if (p_ptr->migite && p_ptr->hidarite)
					{
						extra_blows[0] += (o_ptr->pval + 1) / 2;
						extra_blows[1] += o_ptr->pval / 2;
					}
					else
					{
						extra_blows[default_hand] += o_ptr->pval;
					}
				}
			}
		}

		switch (i)
		{
		case INVEN_RIGHT: 
			if (have_flag(flgs, TR_BRAND_FIRE))   p_ptr->weapon_info[0].brand_fire = TRUE;
			if (have_flag(flgs, TR_BRAND_COLD))   p_ptr->weapon_info[0].brand_cold = TRUE;
			if (have_flag(flgs, TR_BRAND_ELEC))   p_ptr->weapon_info[0].brand_elec = TRUE;
			if (have_flag(flgs, TR_BRAND_ACID))   p_ptr->weapon_info[0].brand_acid = TRUE;
			if (have_flag(flgs, TR_BRAND_POIS))   p_ptr->weapon_info[0].brand_pois = TRUE;
			break;

		case INVEN_LEFT:
			if (have_flag(flgs, TR_BRAND_FIRE))
			{
				p_ptr->weapon_info[1].brand_fire = TRUE;
				if (p_ptr->ryoute)
					p_ptr->weapon_info[0].brand_fire = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_COLD))
			{
				p_ptr->weapon_info[1].brand_cold = TRUE;
				if (p_ptr->ryoute)
					p_ptr->weapon_info[0].brand_cold = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_ELEC))
			{
				p_ptr->weapon_info[1].brand_elec = TRUE;
				if (p_ptr->ryoute)
					p_ptr->weapon_info[0].brand_elec = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_ACID))
			{
				p_ptr->weapon_info[1].brand_acid = TRUE;
				if (p_ptr->ryoute)
					p_ptr->weapon_info[0].brand_acid = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_POIS))
			{
				p_ptr->weapon_info[1].brand_pois = TRUE;
				if (p_ptr->ryoute)
					p_ptr->weapon_info[0].brand_pois = TRUE;
			}
			break;

		case INVEN_HANDS:
			if (have_flag(flgs, TR_BRAND_FIRE))
			{
				p_ptr->weapon_info[0].brand_fire = TRUE;
				p_ptr->weapon_info[1].brand_fire = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_COLD))
			{
				p_ptr->weapon_info[0].brand_cold = TRUE;
				p_ptr->weapon_info[1].brand_cold = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_ELEC))
			{
				p_ptr->weapon_info[0].brand_elec = TRUE;
				p_ptr->weapon_info[1].brand_elec = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_ACID))
			{
				p_ptr->weapon_info[0].brand_acid = TRUE;
				p_ptr->weapon_info[1].brand_acid = TRUE;
			}
			if (have_flag(flgs, TR_BRAND_POIS))
			{
				p_ptr->weapon_info[0].brand_pois = TRUE;
				p_ptr->weapon_info[1].brand_pois = TRUE;
			}
			break;
		}

		/* Hack -- cause earthquakes */
		if (have_flag(flgs, TR_IMPACT)) p_ptr->impact[(i == INVEN_RARM) ? 0 : 1] = TRUE;

		/* Boost shots */
		if (have_flag(flgs, TR_XTRA_SHOTS)) extra_shots += 10;
		if (o_ptr->name2 == EGO_SNIPER) extra_shots += 5;

		/* Various flags */
		if (have_flag(flgs, TR_AGGRAVATE))   p_ptr->cursed |= TRC_AGGRAVATE;
		if (have_flag(flgs, TR_DRAIN_EXP))   p_ptr->cursed |= TRC_DRAIN_EXP;
		if (have_flag(flgs, TR_TY_CURSE))    p_ptr->cursed |= TRC_TY_CURSE;
		if (have_flag(flgs, TR_DEC_MANA))    p_ptr->dec_mana = TRUE;
		if (have_flag(flgs, TR_SPELL_POWER)) p_ptr->spell_power -= o_ptr->pval;	/*Increasing Spell Power always comes with decreasing STR, DEX, CON*/
		if (have_flag(flgs, TR_SPELL_CAP))   p_ptr->spell_cap += o_ptr->pval;
		if (have_flag(flgs, TR_BLESSED))     p_ptr->bless_blade = TRUE;
		if (have_flag(flgs, TR_XTRA_MIGHT))  p_ptr->xtra_might = TRUE;
		if (have_flag(flgs, TR_SLOW_DIGEST)) p_ptr->slow_digest = TRUE;
		if (have_flag(flgs, TR_REGEN))       p_ptr->regenerate = TRUE;
		if (have_flag(flgs, TR_TELEPATHY))   p_ptr->telepathy = TRUE;
		if (have_flag(flgs, TR_ESP_ANIMAL))  p_ptr->esp_animal = TRUE;
		if (have_flag(flgs, TR_ESP_UNDEAD))  p_ptr->esp_undead = TRUE;
		if (have_flag(flgs, TR_ESP_DEMON))   p_ptr->esp_demon = TRUE;
		if (have_flag(flgs, TR_ESP_ORC))     p_ptr->esp_orc = TRUE;
		if (have_flag(flgs, TR_ESP_TROLL))   p_ptr->esp_troll = TRUE;
		if (have_flag(flgs, TR_ESP_GIANT))   p_ptr->esp_giant = TRUE;
		if (have_flag(flgs, TR_ESP_DRAGON))  p_ptr->esp_dragon = TRUE;
		if (have_flag(flgs, TR_ESP_HUMAN))   p_ptr->esp_human = TRUE;
		if (have_flag(flgs, TR_ESP_EVIL))    p_ptr->esp_evil = TRUE;
		if (have_flag(flgs, TR_ESP_GOOD))    p_ptr->esp_good = TRUE;
		if (have_flag(flgs, TR_ESP_NONLIVING)) p_ptr->esp_nonliving = TRUE;
		if (have_flag(flgs, TR_ESP_UNIQUE))  p_ptr->esp_unique = TRUE;

		if (have_flag(flgs, TR_SEE_INVIS))   p_ptr->see_inv = TRUE;
		if (have_flag(flgs, TR_LEVITATION))     p_ptr->levitation = TRUE;
		if (have_flag(flgs, TR_FREE_ACT))    p_ptr->free_act = TRUE;
		if (have_flag(flgs, TR_HOLD_LIFE))   p_ptr->hold_life = TRUE;
		if (have_flag(flgs, TR_WARNING)){
			if (!o_ptr->inscription || !(my_strchr(quark_str(o_ptr->inscription),'$')))
			  p_ptr->warning = TRUE;
		}

		if (have_flag(flgs, TR_TELEPORT))
		{
			if (object_is_cursed(o_ptr)) p_ptr->cursed |= TRC_TELEPORT;
			else
			{
				cptr insc = quark_str(o_ptr->inscription);

				if (o_ptr->inscription && my_strchr(insc, '.'))
				{
					/*
					 * {.} will stop random teleportation.
					 */
				}
				else
				{
					/* Controlled random teleportation */
					p_ptr->cursed |= TRC_TELEPORT_SELF;
				}
			}
		}

		/* Immunity flags */
		if (have_flag(flgs, TR_IM_FIRE)) p_ptr->immune_fire = TRUE;
		if (have_flag(flgs, TR_IM_ACID)) p_ptr->immune_acid = TRUE;
		if (have_flag(flgs, TR_IM_COLD)) p_ptr->immune_cold = TRUE;
		if (have_flag(flgs, TR_IM_ELEC)) p_ptr->immune_elec = TRUE;

		/* Resistance flags */
		if (have_flag(flgs, TR_RES_ACID))   p_ptr->resist_acid = TRUE;
		if (have_flag(flgs, TR_RES_ELEC))   p_ptr->resist_elec = TRUE;
		if (have_flag(flgs, TR_RES_FIRE))   p_ptr->resist_fire = TRUE;
		if (have_flag(flgs, TR_RES_COLD))   p_ptr->resist_cold = TRUE;
		if (have_flag(flgs, TR_RES_POIS))   p_ptr->resist_pois = TRUE;
		if (have_flag(flgs, TR_RES_FEAR))   p_ptr->resist_fear = TRUE;
		if (have_flag(flgs, TR_RES_CONF))   p_ptr->resist_conf = TRUE;
		if (have_flag(flgs, TR_RES_SOUND))  p_ptr->resist_sound = TRUE;
		if (have_flag(flgs, TR_RES_LITE))   p_ptr->resist_lite = TRUE;
		if (have_flag(flgs, TR_RES_DARK))   p_ptr->resist_dark = TRUE;
		if (have_flag(flgs, TR_RES_CHAOS))  p_ptr->resist_chaos = TRUE;
		if (have_flag(flgs, TR_RES_DISEN))  p_ptr->resist_disen = TRUE;
		if (have_flag(flgs, TR_RES_SHARDS)) p_ptr->resist_shard = TRUE;
		if (have_flag(flgs, TR_RES_NEXUS))  p_ptr->resist_nexus = TRUE;
		if (have_flag(flgs, TR_RES_BLIND))  p_ptr->resist_blind = TRUE;
		if (have_flag(flgs, TR_RES_NETHER)) p_ptr->resist_neth = TRUE;
		if (have_flag(flgs, TR_RES_TIME))   p_ptr->resist_time = TRUE;

		if (have_flag(flgs, TR_REFLECT))  p_ptr->reflect = TRUE;
		if (have_flag(flgs, TR_SH_FIRE))  p_ptr->sh_fire = TRUE;
		if (have_flag(flgs, TR_SH_ELEC))  p_ptr->sh_elec = TRUE;
		if (have_flag(flgs, TR_SH_COLD))  p_ptr->sh_cold = TRUE;
		if (have_flag(flgs, TR_NO_MAGIC)) p_ptr->anti_magic = TRUE;
		if (have_flag(flgs, TR_NO_TELE))  p_ptr->anti_tele = TRUE;

		/* Sustain flags */
		if (have_flag(flgs, TR_SUST_STR)) p_ptr->sustain_str = TRUE;
		if (have_flag(flgs, TR_SUST_INT)) p_ptr->sustain_int = TRUE;
		if (have_flag(flgs, TR_SUST_WIS)) p_ptr->sustain_wis = TRUE;
		if (have_flag(flgs, TR_SUST_DEX)) p_ptr->sustain_dex = TRUE;
		if (have_flag(flgs, TR_SUST_CON)) p_ptr->sustain_con = TRUE;
		if (have_flag(flgs, TR_SUST_CHR)) p_ptr->sustain_chr = TRUE;

		if (o_ptr->name2 == EGO_YOIYAMI) yoiyami = TRUE;
		if (o_ptr->name2 == EGO_GENJI) easy_2weapon = TRUE;
		if (o_ptr->name1 == ART_MASTER_TONBERRY) easy_2weapon = TRUE;
		if (o_ptr->name2 == EGO_RING_RES_TIME) p_ptr->resist_time = TRUE;
		if (o_ptr->name2 == EGO_RING_THROW) p_ptr->mighty_throw = TRUE;
		if (have_flag(flgs, TR_EASY_SPELL)) p_ptr->easy_spell = TRUE;
		if (o_ptr->name2 == EGO_AMU_FOOL) p_ptr->heavy_spell = TRUE;
		if (o_ptr->name2 == EGO_AMU_NAIVETY) down_saving = TRUE;

		if (o_ptr->curse_flags & TRC_LOW_MAGIC)
		{
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				p_ptr->to_m_chance += 10;
			}
			else
			{
				p_ptr->to_m_chance += 3;
			}
		}

		if (o_ptr->tval == TV_CAPTURE) continue;

		/* Modify the base armor class */
		p_ptr->ac += o_ptr->ac;

		/* The base armor class is always known */
		p_ptr->dis_ac += o_ptr->ac;

		/* Apply the bonuses to armor class */
		p_ptr->to_a += o_ptr->to_a;

		/* Apply the mental bonuses to armor class, if known */
		if (object_is_known(o_ptr)) p_ptr->dis_to_a += o_ptr->to_a;

		if (o_ptr->curse_flags & TRC_LOW_MELEE)
		{
			int slot = i - INVEN_RARM;
			if (slot < 2)
			{
				if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					p_ptr->weapon_info[slot].to_h -= 15;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->weapon_info[slot].dis_to_h -= 15;
				}
				else
				{
					p_ptr->weapon_info[slot].to_h -= 5;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->weapon_info[slot].dis_to_h -= 5;
				}
			}
			else
			{
				if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
				{
					p_ptr->to_h_b -= 15;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_h_b -= 15;
				}
				else
				{
					p_ptr->to_h_b -= 5;
					if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_h_b -= 5;
				}
			}
		}

		if (o_ptr->curse_flags & TRC_LOW_AC)
		{
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE)
			{
				p_ptr->to_a -= 30;
				if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_a -= 30;
			}
			else
			{
				p_ptr->to_a -= 10;
				if (o_ptr->ident & IDENT_MENTAL) p_ptr->dis_to_a -= 10;
			}
		}

		/* Hack -- do not apply "weapon" bonuses */
		if (i == INVEN_RARM && (buki_motteruka(i) || object_is_melee_weapon(o_ptr))) continue;
		if (i == INVEN_LARM && (buki_motteruka(i) || object_is_melee_weapon(o_ptr))) continue;

		/* Hack -- do not apply "bow" bonuses */
		if (i == INVEN_BOW) continue;

		bonus_to_h = o_ptr->to_h;
		bonus_to_d = o_ptr->to_d;

		/* Hack -- Sniper gloves apply damage bonus to missiles only */
		if (o_ptr->name2 == EGO_SNIPER)
		{
			bonus_to_d = 0;
			p_ptr->to_d_b += o_ptr->to_d;
			if (object_is_known(o_ptr))
				p_ptr->dis_to_d_b += o_ptr->to_d;
		}

		if (p_ptr->pclass == CLASS_NINJA)
		{
			if (o_ptr->to_h > 0) bonus_to_h = (o_ptr->to_h+1)/2;
			if (o_ptr->to_d > 0) bonus_to_d = (o_ptr->to_d+1)/2;
		}

		/* To Bow and Natural attack */
		p_ptr->to_h_b += bonus_to_h;
		p_ptr->to_h_m += bonus_to_h;

		if ( o_ptr->name1 != ART_MASTER_TONBERRY
		  && o_ptr->name2 != EGO_POWER )
		{
			p_ptr->to_d_b += bonus_to_d;
			if (object_is_known(o_ptr))
				p_ptr->dis_to_d_b += bonus_to_d;
		}

		/* Apply the mental bonuses tp hit/damage, if known */
		if (object_is_known(o_ptr)) p_ptr->dis_to_h_b += bonus_to_h;

		/* To Melee */
		if ((i == INVEN_LEFT || i == INVEN_RIGHT) && !p_ptr->ryoute)
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->weapon_info[i-INVEN_RIGHT].to_h += bonus_to_h;
			p_ptr->weapon_info[i-INVEN_RIGHT].to_d += bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				p_ptr->weapon_info[i-INVEN_RIGHT].dis_to_h += bonus_to_h;
				p_ptr->weapon_info[i-INVEN_RIGHT].dis_to_d += bonus_to_d;
			}

			if (have_flag(flgs, TR_WEAPONMASTERY))
				p_ptr->weapon_info[i-INVEN_RIGHT].to_dd += o_ptr->pval;
		}
		else if (p_ptr->migite && p_ptr->hidarite)
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->weapon_info[0].to_h += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
			p_ptr->weapon_info[1].to_h += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
			p_ptr->weapon_info[0].to_d += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
			p_ptr->weapon_info[1].to_d += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;

			/* Apply the mental bonuses tp hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				p_ptr->weapon_info[0].dis_to_h += (bonus_to_h > 0) ? (bonus_to_h+1)/2 : bonus_to_h;
				p_ptr->weapon_info[1].dis_to_h += (bonus_to_h > 0) ? bonus_to_h/2 : bonus_to_h;
				p_ptr->weapon_info[0].dis_to_d += (bonus_to_d > 0) ? (bonus_to_d+1)/2 : bonus_to_d;
				p_ptr->weapon_info[1].dis_to_d += (bonus_to_d > 0) ? bonus_to_d/2 : bonus_to_d;
			}

			if (have_flag(flgs, TR_WEAPONMASTERY))
				p_ptr->weapon_info[i-INVEN_RIGHT].to_dd += o_ptr->pval;
		}
		else
		{
			/* Apply the bonuses to hit/damage */
			p_ptr->weapon_info[default_hand].to_h += bonus_to_h;
			p_ptr->weapon_info[default_hand].to_d += bonus_to_d;

			/* Apply the mental bonuses to hit/damage, if known */
			if (object_is_known(o_ptr))
			{
				p_ptr->weapon_info[default_hand].dis_to_h += bonus_to_h;
				p_ptr->weapon_info[default_hand].dis_to_d += bonus_to_d;
			}

			if (have_flag(flgs, TR_WEAPONMASTERY))
				p_ptr->weapon_info[default_hand].to_dd += o_ptr->pval;
		}
	}

	if (old_mighty_throw != p_ptr->mighty_throw)
	{
		/* Redraw average damege display of Shuriken */
		p_ptr->window |= PW_INVEN;
	}

	if (p_ptr->cursed & TRC_TELEPORT) p_ptr->cursed &= ~(TRC_TELEPORT_SELF);

	/* Monks get extra ac for armour _not worn_ */
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER)) && !heavy_armor())
	{
		if (!(inventory[INVEN_BODY].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev * 3) / 2;
			p_ptr->dis_to_a += (p_ptr->lev * 3) / 2;
		}
		if (!(inventory[INVEN_OUTER].k_idx) && (p_ptr->lev > 15))
		{
			p_ptr->to_a += ((p_ptr->lev - 13) / 3);
			p_ptr->dis_to_a += ((p_ptr->lev - 13) / 3);
		}
		if (!(inventory[INVEN_LARM].k_idx) && (p_ptr->lev > 10))
		{
			p_ptr->to_a += ((p_ptr->lev - 8) / 3);
			p_ptr->dis_to_a += ((p_ptr->lev - 8) / 3);
		}
		if (!(inventory[INVEN_HEAD].k_idx) && (p_ptr->lev > 4))
		{
			p_ptr->to_a += (p_ptr->lev - 2) / 3;
			p_ptr->dis_to_a += (p_ptr->lev - 2) / 3;
		}
		if (!(inventory[INVEN_HANDS].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev / 2);
			p_ptr->dis_to_a += (p_ptr->lev / 2);
		}
		if (!(inventory[INVEN_FEET].k_idx))
		{
			p_ptr->to_a += (p_ptr->lev / 3);
			p_ptr->dis_to_a += (p_ptr->lev / 3);
		}
	}
	if ( (p_ptr->pclass == CLASS_MONK && !heavy_armor())
	  || p_ptr->pclass == CLASS_WILD_TALENT)
	{
		/* Massive Hacks:  The Wild Talent gets monk posture benefits without monk
		   restrictions, so we duplicate a bunch of code here that is set later for monks
		   after checking weapon status */
		if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			p_ptr->stat_add[A_STR] += 2;
			p_ptr->stat_add[A_DEX] += 2;
			p_ptr->stat_add[A_CON] -= 3;
			
			if (p_ptr->pclass == CLASS_WILD_TALENT)
			{
				p_ptr->to_a -= 40;
				p_ptr->dis_to_a -= 40;
			}
		}
		else if (p_ptr->special_defense & KAMAE_SEIRYU)
		{
			if (p_ptr->pclass == CLASS_WILD_TALENT)
			{
				p_ptr->to_a -= 50;
				p_ptr->dis_to_a -= 50;
				p_ptr->resist_acid = TRUE;
				p_ptr->resist_fire = TRUE;
				p_ptr->resist_elec = TRUE;
				p_ptr->resist_cold = TRUE;
				p_ptr->resist_pois = TRUE;
				p_ptr->sh_fire = TRUE;
				p_ptr->sh_elec = TRUE;
				p_ptr->sh_cold = TRUE;
				p_ptr->levitation = TRUE;
			}
		}
		else if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->stat_add[A_INT] -= 1;
			p_ptr->stat_add[A_WIS] -= 1;
			p_ptr->stat_add[A_DEX] -= 2;
			p_ptr->stat_add[A_CON] += 3;

			if (p_ptr->pclass == CLASS_WILD_TALENT)
			{
				p_ptr->to_a += (p_ptr->lev*p_ptr->lev)/50;
				p_ptr->dis_to_a += (p_ptr->lev*p_ptr->lev)/50;
				p_ptr->reflect = TRUE;
			}
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			p_ptr->stat_add[A_STR] -= 2;
			p_ptr->stat_add[A_INT] += 1;
			p_ptr->stat_add[A_WIS] += 1;
			p_ptr->stat_add[A_DEX] += 2;
			p_ptr->stat_add[A_CON] -= 2;
			if (p_ptr->pclass == CLASS_WILD_TALENT)
			{
				p_ptr->levitation = TRUE;
			}
		}
	}


	if (p_ptr->special_defense & KATA_KOUKIJIN)
	{
		for (i = 0; i < 6; i++)
			p_ptr->stat_add[i] += 5;
		p_ptr->to_a -= 50;
		p_ptr->dis_to_a -= 50;
	}

	/* Hack -- aura of fire also provides light */
	if (p_ptr->sh_fire) p_ptr->lite = TRUE;

	if (p_ptr->tim_building_up)
	{
		int amt = 4 * p_ptr->lev / 50; /* 13, 25, 38, 50 */
		p_ptr->stat_add[A_STR] += amt;
		p_ptr->stat_add[A_DEX] += amt;
		p_ptr->stat_add[A_CON] += amt;
		p_ptr->skills.thn += 60*p_ptr->lev/50;
	}

	/* Hex bonuses */
	if (p_ptr->realm1 == REALM_HEX)
	{
		if (hex_spelling_any()) p_ptr->skills.stl -= (1 + p_ptr->magic_num2[0]);
		if (hex_spelling(HEX_DETECT_EVIL)) p_ptr->esp_evil = TRUE;
		if (hex_spelling(HEX_XTRA_MIGHT)) p_ptr->stat_add[A_STR] += 4;
		if (hex_spelling(HEX_BUILDING))
		{
			p_ptr->stat_add[A_STR] += 4;
			p_ptr->stat_add[A_DEX] += 4;
			p_ptr->stat_add[A_CON] += 4;
		}
		if (hex_spelling(HEX_DEMON_AURA))
		{
			p_ptr->sh_fire = TRUE;
			p_ptr->regenerate = TRUE;
		}
		if (hex_spelling(HEX_ICE_ARMOR))
		{
			p_ptr->sh_cold = TRUE; 
			p_ptr->to_a += 30;
			p_ptr->dis_to_a += 30;
		}
		if (hex_spelling(HEX_SHOCK_CLOAK))
		{
			p_ptr->sh_elec = TRUE;
			p_ptr->pspeed += 3;
		}
		for (i = INVEN_RARM; i <= INVEN_FEET; i++)
		{
			int ac = 0;
			o_ptr = &inventory[i];
			if (!o_ptr->k_idx) continue;
			if (!object_is_armour(o_ptr)) continue;
			if (!object_is_cursed(o_ptr)) continue;
			ac += 5;
			if (o_ptr->curse_flags & TRC_HEAVY_CURSE) ac += 7;
			if (o_ptr->curse_flags & TRC_PERMA_CURSE) ac += 13;
			p_ptr->to_a += ac;
			p_ptr->dis_to_a += ac;
		}
	}

	/* Apply temporary "stun" */
	if (p_ptr->stun > 50)
	{
		p_ptr->weapon_info[0].to_h -= 20;
		p_ptr->weapon_info[1].to_h -= 20;
		p_ptr->to_h_b  -= 20;
		p_ptr->to_h_m  -= 20;
		p_ptr->weapon_info[0].dis_to_h -= 20;
		p_ptr->weapon_info[1].dis_to_h -= 20;
		p_ptr->dis_to_h_b  -= 20;
		p_ptr->weapon_info[0].to_d -= 20;
		p_ptr->weapon_info[1].to_d -= 20;
		p_ptr->to_d_m -= 20;
		p_ptr->to_d_b -= 20;
		p_ptr->weapon_info[0].dis_to_d -= 20;
		p_ptr->weapon_info[1].dis_to_d -= 20;
	}
	else if (p_ptr->stun)
	{
		p_ptr->weapon_info[0].to_h -= 5;
		p_ptr->weapon_info[1].to_h -= 5;
		p_ptr->to_h_b -= 5;
		p_ptr->to_h_m -= 5;
		p_ptr->weapon_info[0].dis_to_h -= 5;
		p_ptr->weapon_info[1].dis_to_h -= 5;
		p_ptr->dis_to_h_b -= 5;
		p_ptr->weapon_info[0].to_d -= 5;
		p_ptr->weapon_info[1].to_d -= 5;
		p_ptr->to_d_m -= 5;
		p_ptr->to_d_b -= 5;
		p_ptr->weapon_info[0].dis_to_d -= 5;
		p_ptr->weapon_info[1].dis_to_d -= 5;
	}

	/* Wraith form */
	if (IS_WRAITH())
	{
		p_ptr->reflect = TRUE;
		p_ptr->pass_wall = TRUE;
	}

	if (IS_PASSWALL())
	{
		p_ptr->pass_wall = TRUE;
	}

	/* Temporary blessing */
	if (IS_BLESSED())
	{
		p_ptr->to_a += 5;
		p_ptr->dis_to_a += 5;
		p_ptr->weapon_info[0].to_h += 10;
		p_ptr->weapon_info[1].to_h += 10;
		p_ptr->to_h_b  += 10;
		p_ptr->to_h_m  += 10;
		p_ptr->weapon_info[0].dis_to_h += 10;
		p_ptr->weapon_info[1].dis_to_h += 10;
		p_ptr->dis_to_h_b += 10;
	}

	if (p_ptr->magicdef)
	{
		p_ptr->resist_blind = TRUE;
		p_ptr->resist_conf = TRUE;
		p_ptr->reflect = TRUE;
		p_ptr->free_act = TRUE;
		p_ptr->levitation = TRUE;
	}

	if (mut_present(MUT_WEAPON_SKILLS))
	{
		p_ptr->weapon_info[0].to_h += 8;
		p_ptr->weapon_info[1].to_h += 8;
		p_ptr->weapon_info[0].dis_to_h += 8;
		p_ptr->weapon_info[1].dis_to_h += 8;
		p_ptr->to_h_b  += 8;
		p_ptr->dis_to_h_b += 8;
	}

	/* Temporary "Beserk" */
	if (IS_SHERO())
	{
		p_ptr->weapon_info[0].to_h += 12;
		p_ptr->weapon_info[1].to_h += 12;
		p_ptr->to_h_b  -= 12;
		p_ptr->to_h_m  += 12;
		p_ptr->weapon_info[0].to_d += 3+(p_ptr->lev/5);
		p_ptr->weapon_info[1].to_d += 3+(p_ptr->lev/5);
		p_ptr->to_d_m  += 3+(p_ptr->lev/5);
		p_ptr->weapon_info[0].dis_to_h += 12;
		p_ptr->weapon_info[1].dis_to_h += 12;
		p_ptr->dis_to_h_b  -= 12;
		p_ptr->weapon_info[0].dis_to_d += 3+(p_ptr->lev/5);
		p_ptr->weapon_info[1].dis_to_d += 3+(p_ptr->lev/5);
		p_ptr->to_a -= 10;
		p_ptr->dis_to_a -= 10;
		p_ptr->skills.stl -= 7;
		p_ptr->skills.dev -= 20;
		p_ptr->skills.sav -= 30;
		p_ptr->skills.srh -= 15;
		p_ptr->skills.fos -= 15;
		p_ptr->skill_tht -= 20;
		p_ptr->skill_dig += 30;
	}

	/* Temporary "fast" */
	if (IS_FAST())
	{
		if (p_ptr->pclass == CLASS_TIME_LORD)
			p_ptr->pspeed += 15;
		else
			p_ptr->pspeed += 10;
	}
	else if (p_ptr->tim_spurt)
	{
		/* The Time Lord's baby speed spell shouldn't stack with other forms of haste ... */
		p_ptr->pspeed += 5;
	}

	/* Temporary "slow" */
	if (p_ptr->slow)
	{
		if (p_ptr->pclass == CLASS_TIME_LORD)
			p_ptr->pspeed -= 5;
		else
			p_ptr->pspeed -= 10;
	}

	/* Temporary "telepathy" */
	if (IS_TIM_ESP())
	{
		p_ptr->telepathy = TRUE;
	}
	if (p_ptr->tim_esp_magical)
		p_ptr->esp_magical = TRUE;

	if (p_ptr->ele_immune)
	{
		if (p_ptr->special_defense & DEFENSE_ACID)
			p_ptr->immune_acid = TRUE;
		else if (p_ptr->special_defense & DEFENSE_ELEC)
			p_ptr->immune_elec = TRUE;
		else if (p_ptr->special_defense & DEFENSE_FIRE)
			p_ptr->immune_fire = TRUE;
		else if (p_ptr->special_defense & DEFENSE_COLD)
			p_ptr->immune_cold = TRUE;
	}

	/* Temporary see invisible */
	if (p_ptr->tim_invis)
	{
		p_ptr->see_inv = TRUE;
	}

	/* Temporary infravision boost */
	if (IS_TIM_INFRA())
	{
		p_ptr->see_infra+=3;
	}

	/* Temporary regeneration boost */
	if (p_ptr->tim_regen)
	{
		p_ptr->regenerate = TRUE;
	}

	/* Temporary levitation */
	if (p_ptr->tim_levitation)
	{
		p_ptr->levitation = TRUE;
	}

	/* Temporary reflection */
	if (p_ptr->tim_reflect)
	{
		p_ptr->reflect = TRUE;
	}

	/* Hack -- Telepathy Change */
	if (p_ptr->telepathy != old_telepathy)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	if ((p_ptr->esp_animal != old_esp_animal) ||
	    (p_ptr->esp_undead != old_esp_undead) ||
	    (p_ptr->esp_demon != old_esp_demon) ||
	    (p_ptr->esp_orc != old_esp_orc) ||
	    (p_ptr->esp_troll != old_esp_troll) ||
	    (p_ptr->esp_giant != old_esp_giant) ||
	    (p_ptr->esp_dragon != old_esp_dragon) ||
	    (p_ptr->esp_human != old_esp_human) ||
	    (p_ptr->esp_evil != old_esp_evil) ||
	    (p_ptr->esp_good != old_esp_good) ||
	    (p_ptr->esp_nonliving != old_esp_nonliving) ||
	    (p_ptr->esp_unique != old_esp_unique) ||
	    p_ptr->esp_magical != old_esp_magical )
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Hack -- See Invis Change */
	if (p_ptr->see_inv != old_see_inv)
	{
		p_ptr->update |= (PU_MONSTERS);
	}

	/* Call the class hook after scanning equipment but before calculating encumbrance, et. al.*/
	if (class_ptr != NULL && class_ptr->calc_bonuses != NULL)
		class_ptr->calc_bonuses();

	if (race_ptr != NULL && race_ptr->calc_bonuses != NULL)
		race_ptr->calc_bonuses();

	/* Temporary "Hero" ... moved after class processing for the Swordmaster */
	if (IS_HERO())
	{
		p_ptr->weapon_info[0].to_h += 12;
		p_ptr->weapon_info[1].to_h += 12;
		p_ptr->to_h_b  += 12;
		p_ptr->to_h_m  += 12;
		p_ptr->weapon_info[0].dis_to_h += 12;
		p_ptr->weapon_info[1].dis_to_h += 12;
		p_ptr->dis_to_h_b  += 12;
	}

	/* Hack -- Hero/Shero -> Res fear */
	if (IS_HERO() || IS_SHERO())
	{
		p_ptr->resist_fear = TRUE;
	}

	/* Calculate stats after calling class hook */
	for (i = 0; i < 6; i++)
	{
		int top, use, ind;

		/* Extract the new "stat_use" value for the stat */
		top = modify_stat_value(p_ptr->stat_max[i], p_ptr->stat_add[i]);

		/* Notice changes */
		if (p_ptr->stat_top[i] != top)
		{
			/* Save the new value */
			p_ptr->stat_top[i] = top;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Extract the new "stat_use" value for the stat */
		use = modify_stat_value(p_ptr->stat_cur[i], p_ptr->stat_add[i]);

		if ((i == A_CHR) && mut_present(MUT_ILL_NORM))
		{
			/* 10 to 18/90 charisma, guaranteed, based on level */
			if (use < 8 + 2 * p_ptr->lev)
			{
				use = 8 + 2 * p_ptr->lev;
			}
		}

		/* Notice changes */
		if (p_ptr->stat_use[i] != use)
		{
			/* Save the new value */
			p_ptr->stat_use[i] = use;

			/* Redisplay the stats later */
			p_ptr->redraw |= (PR_STATS);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}


		/* Values: 3, 4, ..., 17 */
		if (use <= 18) ind = (use - 3);

		/* Ranges: 18/00-18/09, ..., 18/210-18/219 */
		else if (use <= 18+219) ind = (15 + (use - 18) / 10);

		/* Range: 18/220+ */
		else ind = (37);

		/* Notice changes */
		if (p_ptr->stat_ind[i] != ind)
		{
			/* Save the new index */
			p_ptr->stat_ind[i] = ind;

			/* Change in CON affects Hitpoints */
			if (i == A_CON)
			{
				p_ptr->update |= (PU_HP);
			}

			/* Change in INT may affect Mana/Spells */
			else if (i == A_INT)
			{
				if (mp_ptr->spell_stat == A_INT)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_WIS)
			{
				if (mp_ptr->spell_stat == A_WIS)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Change in WIS may affect Mana/Spells */
			else if (i == A_CHR)
			{
				if (mp_ptr->spell_stat == A_CHR)
				{
					p_ptr->update |= (PU_MANA | PU_SPELLS);
				}
			}

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}
	}

	/* Bloating slows the player down (a little) */
	if (p_ptr->food >= PY_FOOD_MAX) p_ptr->pspeed -= 10;

	if (p_ptr->special_defense & KAMAE_SUZAKU) p_ptr->pspeed += 10;

	if ((p_ptr->migite && (empty_hands_status & EMPTY_HAND_RARM)) ||
	    (p_ptr->hidarite && (empty_hands_status & EMPTY_HAND_LARM)))
	{
		p_ptr->weapon_info[default_hand].to_h += (p_ptr->skill_exp[GINOU_SUDE] - WEAPON_EXP_BEGINNER) / 200;
		p_ptr->weapon_info[default_hand].dis_to_h += (p_ptr->skill_exp[GINOU_SUDE] - WEAPON_EXP_BEGINNER) / 200;
	}

	if (buki_motteruka(INVEN_RARM) && buki_motteruka(INVEN_LARM))
	{
		int penalty1, penalty2, to_d;
		int skill = p_ptr->skill_exp[GINOU_NITOURYU];

		if (p_ptr->tim_genji && skill < 7000)
			skill = 7000;

		penalty1 = ((120 - skill / 160) - (130 - inventory[INVEN_RARM].weight) / 8);
		penalty2 = ((120 - skill / 160) - (130 - inventory[INVEN_LARM].weight) / 8);
		if ((inventory[INVEN_RARM].name1 == ART_QUICKTHORN) && (inventory[INVEN_LARM].name1 == ART_TINYTHORN))
		{
			penalty1 = penalty1 / 2 - 5;
			penalty2 = penalty2 / 2 - 5;
			p_ptr->pspeed += 7;
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
		}
		/* Hack: easy_2weapon means Genji.  p_ptr->easy_2weapon means a Daggermaster dual
		   wielding favored weapons */
		if (easy_2weapon && p_ptr->easy_2weapon)
		{
			penalty1 = 0;
			penalty2 = 0;
		}
		else if (easy_2weapon || p_ptr->easy_2weapon)
		{
			if (penalty1 > 0) penalty1 /= 2;
			if (penalty2 > 0) penalty2 /= 2;
		}
		else if ((inventory[INVEN_LARM].tval == TV_SWORD) && ((inventory[INVEN_LARM].sval == SV_MAIN_GAUCHE) || (inventory[INVEN_LARM].sval == SV_WAKIZASHI)))
		{
			penalty1 = MAX(0, penalty1 - 10);
			penalty2 = MAX(0, penalty2 - 10);
		}
		if ((inventory[INVEN_RARM].name1 == ART_MUSASI_KATANA) && (inventory[INVEN_LARM].name1 == ART_MUSASI_WAKIZASI))
		{
			penalty1 = MIN(0, penalty1);
			penalty2 = MIN(0, penalty2);
			p_ptr->to_a += 10;
			p_ptr->dis_to_a += 10;
		}
		else
		{
			if ((inventory[INVEN_RARM].name1 == ART_MUSASI_KATANA) && (penalty1 > 0))
				penalty1 /= 2;
			if ((inventory[INVEN_LARM].name1 == ART_MUSASI_WAKIZASI) && (penalty2 > 0))
				penalty2 /= 2;
		}
		if (inventory[INVEN_RARM].tval == TV_POLEARM) penalty1 += 10;
		if (inventory[INVEN_LARM].tval == TV_POLEARM) penalty2 += 10;
		p_ptr->weapon_info[0].to_h -= penalty1;
		p_ptr->weapon_info[1].to_h -= penalty2;
		p_ptr->weapon_info[0].dis_to_h -= penalty1;
		p_ptr->weapon_info[1].dis_to_h -= penalty2;

		/* Hack: Dual Wielding now decreases the deadliness of strikes as you need to
		   focus your concentration on both weapons. Genji eliminates this penalty.
		   To summarize:
		   * Two Handed Wielding -> 3/2 Strength Bonus
		   * One Handed Wielding -> Normal Strength Bonus
		   * Dual Wielding       -> 2/3 Strength Bonus
		*/
		to_d = ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
		if (to_d > 0 && !easy_2weapon && !p_ptr->easy_2weapon)
		{
			to_d /= 3;
			p_ptr->weapon_info[0].to_d -= to_d;
			p_ptr->weapon_info[0].dis_to_d -= to_d;
			p_ptr->weapon_info[1].to_d -= to_d;
			p_ptr->weapon_info[1].dis_to_d -= to_d;
		}

		/* Hack: Berserking now prorates damage bonus when dual wielding unless
		   player has Genji bonus.
		*/
		if (IS_SHERO() && !easy_2weapon && !p_ptr->easy_2weapon && p_ptr->pclass != CLASS_BERSERKER)
		{
			to_d = 3+(p_ptr->lev/5);
			p_ptr->weapon_info[0].to_d -= to_d/2;
			p_ptr->weapon_info[0].dis_to_d -= to_d/2;
			p_ptr->weapon_info[1].to_d -= (to_d + 1)/2;
			p_ptr->weapon_info[1].dis_to_d -= (to_d + 1)/2;
		}
	}

	/* Extract the current weight (in tenth pounds) */
	j = p_ptr->total_weight;

	if (!p_ptr->riding)
	{
		/* Extract the "weight limit" (in tenth pounds) */
		i = (int)weight_limit();
	}
	else
	{
		monster_type *riding_m_ptr = &m_list[p_ptr->riding];
		monster_race *riding_r_ptr = &r_info[riding_m_ptr->r_idx];
		int speed = riding_m_ptr->mspeed;

		if (riding_m_ptr->mspeed > 110)
		{
			p_ptr->pspeed = 110 + (s16b)((speed - 110) * (p_ptr->skill_exp[GINOU_RIDING] * 3 + p_ptr->lev * 160L - 10000L) / (22000L));
			if (p_ptr->pspeed < 110) p_ptr->pspeed = 110;
		}
		else
		{
			p_ptr->pspeed = speed;
		}
		p_ptr->pspeed += (p_ptr->skill_exp[GINOU_RIDING] + p_ptr->lev *160L)/3200;
		if (MON_FAST(riding_m_ptr)) p_ptr->pspeed += 10;
		if (MON_SLOW(riding_m_ptr)) p_ptr->pspeed -= 10;
		riding_levitation = (riding_r_ptr->flags7 & RF7_CAN_FLY) ? TRUE : FALSE;
		if (riding_r_ptr->flags7 & (RF7_CAN_SWIM | RF7_AQUATIC)) p_ptr->can_swim = TRUE;

		if (!(riding_r_ptr->flags2 & RF2_PASS_WALL)) p_ptr->pass_wall = FALSE;
		if (riding_r_ptr->flags2 & RF2_KILL_WALL) p_ptr->kill_wall = TRUE;

		if (p_ptr->skill_exp[GINOU_RIDING] < RIDING_EXP_SKILLED) j += (150 * 3 * (RIDING_EXP_SKILLED - p_ptr->skill_exp[GINOU_RIDING])) / RIDING_EXP_SKILLED;

		/* Extract the "weight limit" */
		i = 1500 + riding_r_ptr->level * 25;
	}

	/* XXX XXX XXX Apply "encumbrance" from weight */
	if (j > i) p_ptr->pspeed -= ((j - i) / (i / 5));

	/* Searching slows the player down */
	if (p_ptr->action == ACTION_SEARCH) p_ptr->pspeed -= 10;

	/* Actual Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->to_a += calc_adj_dex_ta();
	p_ptr->weapon_info[0].to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->weapon_info[1].to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_d_m  += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->weapon_info[0].to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->weapon_info[1].to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h_b  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->to_h_m  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->weapon_info[0].to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->weapon_info[1].to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h_b  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->to_h_m  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);

	/* Displayed Modifier Bonuses (Un-inflate stat bonuses) */
	p_ptr->dis_to_a += calc_adj_dex_ta();
	p_ptr->weapon_info[0].dis_to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->weapon_info[1].dis_to_d += ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->weapon_info[0].dis_to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->weapon_info[1].dis_to_h += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->dis_to_h_b  += ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);
	p_ptr->weapon_info[0].dis_to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->weapon_info[1].dis_to_h += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);
	p_ptr->dis_to_h_b  += ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128);


	/* Obtain the "hold" value */
	hold = adj_str_hold[p_ptr->stat_ind[A_STR]];

	/* Examine the "current bow" */
	o_ptr = &inventory[INVEN_BOW];


	/* Assume not heavy */
	p_ptr->heavy_shoot = FALSE;

	/* It is hard to carry a heavy bow */
	if (hold < o_ptr->weight / 10)
	{
		/* Hard to wield a heavy bow */
		p_ptr->to_h_b  += 2 * (hold - o_ptr->weight / 10);
		p_ptr->dis_to_h_b  += 2 * (hold - o_ptr->weight / 10);

		/* Heavy Bow */
		p_ptr->heavy_shoot = TRUE;
	}

	/* Compute "extra shots" if needed */
	if (o_ptr->k_idx)
	{
		/* Analyze the launcher */
		switch (o_ptr->sval)
		{
			case SV_SLING:
			{
				p_ptr->tval_ammo = TV_SHOT;
				break;
			}

			case SV_SHORT_BOW:
			case SV_LONG_BOW:
			case SV_NAMAKE_BOW:
			{
				p_ptr->tval_ammo = TV_ARROW;
				break;
			}

			case SV_LIGHT_XBOW:
			case SV_HEAVY_XBOW:
			{
				p_ptr->tval_ammo = TV_BOLT;
				break;
			}
			case SV_CRIMSON:
			case SV_RAILGUN:
			case SV_HARP:
			{
				p_ptr->tval_ammo = TV_NO_AMMO;
				break;
			}
		}

		/* Apply special flags */
		if (o_ptr->k_idx && !p_ptr->heavy_shoot)
		{
			/* Extra shots */
			p_ptr->num_fire += (extra_shots * 10);

			/* Hack -- Rangers love Bows */
			if ((p_ptr->pclass == CLASS_RANGER) &&
			    (p_ptr->tval_ammo == TV_ARROW))
			{
				p_ptr->num_fire += (p_ptr->lev * 4);
			}

			if ((p_ptr->pclass == CLASS_CAVALRY) &&
			    (p_ptr->tval_ammo == TV_ARROW))
			{
				p_ptr->num_fire += (p_ptr->lev * 3);
			}

			if (p_ptr->pclass == CLASS_ARCHER)
			{
				if (p_ptr->tval_ammo == TV_ARROW)
					p_ptr->num_fire += ((p_ptr->lev * 5)+50);
				else if ((p_ptr->tval_ammo == TV_BOLT) || (p_ptr->tval_ammo == TV_SHOT))
					p_ptr->num_fire += (p_ptr->lev * 4);
			}

			/*
			 * Addendum -- also "Reward" high level warriors,
			 * with _any_ missile weapon -- TY
			 */
			if (p_ptr->pclass == CLASS_WARRIOR &&
			   (p_ptr->tval_ammo <= TV_BOLT) &&
			   (p_ptr->tval_ammo >= TV_SHOT))
			{
				p_ptr->num_fire += (p_ptr->lev * 2);
			}
			if (p_ptr->pclass == CLASS_WARLOCK &&
				p_ptr->psubclass == PACT_ABERRATION &&
				p_ptr->tval_ammo <= TV_BOLT &&
				p_ptr->tval_ammo >= TV_SHOT)
			{
				p_ptr->num_fire += (p_ptr->lev * 2);
			}
			if ((p_ptr->pclass == CLASS_ROGUE) &&
			    (p_ptr->tval_ammo == TV_SHOT))
			{
				p_ptr->num_fire += (p_ptr->lev * 4);
			}

			if (p_ptr->pclass == CLASS_SCOUT &&
			    !heavy_armor() &&
				p_ptr->tval_ammo <= TV_BOLT &&
				p_ptr->tval_ammo >= TV_SHOT)
			{
				p_ptr->num_fire += (p_ptr->lev * 3);
			}

			/* Snipers love Cross bows */
			if ((p_ptr->pclass == CLASS_SNIPER) &&
				(p_ptr->tval_ammo == TV_BOLT))
			{
				p_ptr->to_h_b += (10 + (p_ptr->lev / 5));
				p_ptr->dis_to_h_b += (10 + (p_ptr->lev / 5));
			}

			if (p_ptr->num_fire < 0) p_ptr->num_fire = 0;
		}
	}

	/* Blows Calculation */
	for(i = 0 ; i < 2 ; i++)
	{
		weapon_info_t *info_ptr = &p_ptr->weapon_info[i];		
		/* Examine the "main weapon" */
		o_ptr = &inventory[INVEN_RARM+i];

		object_flags(o_ptr, flgs);

		/* Assume not heavy */
		info_ptr->heavy_wield = FALSE;
		info_ptr->icky_wield = FALSE; 
		info_ptr->riding_wield = FALSE;

		if (!buki_motteruka(INVEN_RARM+i)) {info_ptr->num_blow=1;continue;}

		if (p_ptr->tim_enlarge_weapon)
		{
			info_ptr->to_dd += 2;
			info_ptr->to_ds += 2;

			info_ptr->dis_to_h -= 20;
			info_ptr->to_h -= 20;
		}

		if (p_ptr->tim_weaponmastery)
			info_ptr->to_dd += p_ptr->lev/23;

		/* It is hard to hold a heavy weapon */
		if (p_ptr->ryoute)
			hold *= 2;

		if (hold < o_ptr->weight / 10)
		{
			/* Hard to wield a heavy weapon */
			info_ptr->to_h += 2 * (hold - o_ptr->weight / 10);
			info_ptr->dis_to_h += 2 * (hold - o_ptr->weight / 10);

			/* Heavy weapon */
			info_ptr->heavy_wield = TRUE;
		}
		else if (p_ptr->ryoute && (hold < o_ptr->weight/5)) 
		{
			if (p_ptr->pclass != CLASS_MAULER)
				p_ptr->omoi = TRUE;
		}

		if ((i == 1) && (o_ptr->tval == TV_SWORD) && ((o_ptr->sval == SV_MAIN_GAUCHE) || (o_ptr->sval == SV_WAKIZASHI)))
		{
			p_ptr->to_a += 5;
			p_ptr->dis_to_a += 5;
		}

		/* Normal weapons */
		if (o_ptr->k_idx && !info_ptr->heavy_wield)
		{
			int str_index, dex_index;
			int num = 0, wgt = 0, mul = 0, div = 0;

			switch (p_ptr->pclass)
			{
				case CLASS_WARRIOR:
					num = 6; wgt = 70; mul = 5; 
					if (p_ptr->lev >= 40) 
					{
						mul = 6;
						num++;
					}
					break;

				case CLASS_MAULER:
					num = 3; wgt = 280; mul = 3; break;

				case CLASS_BERSERKER:
					num = 6; wgt = 70; mul = 7; break;

				case CLASS_RAGE_MAGE:
					num = 3; wgt = 70; mul = 3; break;
					
				case CLASS_MAGE:
				case CLASS_NECROMANCER:
				case CLASS_BLOOD_MAGE:
				case CLASS_HIGH_MAGE:
				case CLASS_BLUE_MAGE:
					num = 3; wgt = 100; mul = 2; break;

				case CLASS_WARLOCK:
					num = 3; wgt = 70; mul = 2; 
					if (p_ptr->psubclass == PACT_DRAGON) 
					{
						mul = 4;
						if (p_ptr->lev >= 35) num = 5;
						else num = 4;
					}
					break;

				case CLASS_PSION:
					num = 3; wgt = 100; mul = 3; break;

				case CLASS_PRIEST:
				case CLASS_MAGIC_EATER:
				case CLASS_MINDCRAFTER:
					num = 5; wgt = 100; mul = 3; break;

				case CLASS_ROGUE:
					num = 5; wgt = 40; mul = 3; break;

				case CLASS_SCOUT:
					num = 4; wgt = 70; mul = 2; break;

				case CLASS_RANGER:
					num = 5; wgt = 70; mul = 4; break;

				case CLASS_PALADIN:
				case CLASS_SAMURAI:
					num = 5; wgt = 70; mul = 4; break;

				case CLASS_SMITH:
				case CLASS_RUNE_KNIGHT:
					num = 5; wgt = 150; mul = 5; break;

				case CLASS_WEAPONMASTER:
					num = 5; wgt = 70; mul = 5; break;

				case CLASS_WARRIOR_MAGE:
				case CLASS_RED_MAGE:
					num = 5; wgt = 70; mul = 3; break;

				case CLASS_CHAOS_WARRIOR:
					num = 5; wgt = 70; mul = 4; break;

				case CLASS_MONK:
					num = 5; wgt = 60; mul = 3; break;

				case CLASS_TOURIST:
				case CLASS_TIME_LORD:
					num = 4; wgt = 100; mul = 3; break;

				case CLASS_ARCHAEOLOGIST:
				{
					num = 5; wgt = 70; mul = 3; 
					if (p_ptr->lev >= 40 && archaeologist_is_favored_weapon(o_ptr))
					{
						num++;
						mul = 4;
					}
					break;
				}
				case CLASS_BLOOD_KNIGHT:
					num = 3; wgt = 150; mul = 3; break;

				case CLASS_DUELIST:
					num = 1; wgt = 70; mul = 4; break;
					
				case CLASS_IMITATOR:
					num = 5; wgt = 70; mul = 4; break;

				case CLASS_WILD_TALENT:
					num = 4; wgt = 70; mul = 4; break;

				case CLASS_BEASTMASTER:
					num = 5; wgt = 70; mul = 3; break;

				case CLASS_CAVALRY:
					if ((p_ptr->riding) && (have_flag(flgs, TR_RIDING))) {num = 5; wgt = 70; mul = 4;}
					else {num = 5; wgt = 100; mul = 3;}
					break;

				case CLASS_SORCERER:
					num = 1; wgt = 1; mul = 1; break;

				case CLASS_ARCHER:
				case CLASS_BARD:
					num = 4; wgt = 70; mul = 2; break;

				case CLASS_FORCETRAINER:
					num = 4; wgt = 60; mul = 2; break;

				case CLASS_MIRROR_MASTER:
				case CLASS_SNIPER:
					num = 3; wgt = 100; mul = 3; break;

				case CLASS_NINJA:
					num = 4; wgt = 20; mul = 1; break;
			}

			/* Hex - extra mights gives +1 bonus to max blows */
			if (hex_spelling(HEX_XTRA_MIGHT) || hex_spelling(HEX_BUILDING)) { num++; wgt /= 2; mul += 2; }
			if (p_ptr->tim_building_up && p_ptr->pclass != CLASS_MAULER) 
			{ 
				if (num < 5 && p_ptr->lev >= 40) 
					num++; 
				wgt /= 2; 
				mul += 2; 
			}
			/* Battle Magi (Craft) can get 4 blows on assuming the correct form */
			else if (prace_is_(MIMIC_COLOSSUS))
			{
				if (num < 4) 
					num++; 
				wgt /= 2; 
				mul = MAX(mul, 5);
			}
			else if (prace_is_(MIMIC_MITHRIL_GOLEM))
			{
				if (num < 4) 
					num++; 
				mul = MAX(mul, 4);
			}
			else if (prace_is_(MIMIC_CLAY_GOLEM) || prace_is_(MIMIC_IRON_GOLEM))
			{
				mul = MAX(mul, 3);
			}

			/* Enforce a minimum "weight" (tenth pounds) */
			div = ((o_ptr->weight < wgt) ? wgt : o_ptr->weight);

			/* Access the strength vs weight */
			str_index = (adj_str_blow[p_ptr->stat_ind[A_STR]] * mul / div);

			if (p_ptr->ryoute && !p_ptr->omoi) str_index++;
			if (p_ptr->pclass == CLASS_NINJA) str_index = MAX(0, str_index-1);

			/* Maximal value */
			if (str_index > 11) str_index = 11;

			/* Index by dexterity */
			dex_index = (adj_dex_blow[p_ptr->stat_ind[A_DEX]]);

			/* Maximal value */
			if (dex_index > 11) dex_index = 11;

			/* Use the blows table */
			p_ptr->weapon_info[i].num_blow = blows_table[str_index][dex_index];

			/* Maximal value */
			if (p_ptr->weapon_info[i].num_blow > num) p_ptr->weapon_info[i].num_blow = num;

			/* Add in the "bonus blows" */
			p_ptr->weapon_info[i].num_blow += extra_blows[i];

			/* Various spells and effects for extra blows */
			if (p_ptr->tim_speed_essentia)
				p_ptr->weapon_info[i].num_blow += 2;
			
			if (p_ptr->pclass == CLASS_BERSERKER)
			{
				p_ptr->weapon_info[i].num_blow += (p_ptr->lev / 23);
			}
			else if ((p_ptr->pclass == CLASS_ROGUE) && (o_ptr->weight < 50) && (p_ptr->stat_ind[A_DEX] >= 30)) p_ptr->weapon_info[i].num_blow ++;

			if (p_ptr->special_defense & KATA_FUUJIN) p_ptr->weapon_info[i].num_blow -= 1;

			if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) p_ptr->weapon_info[i].num_blow = 1;

			if (class_ptr != NULL && class_ptr->calc_weapon_bonuses != NULL)
				class_ptr->calc_weapon_bonuses(o_ptr, info_ptr);

			if (o_ptr->name1 == ART_EVISCERATOR && p_ptr->weapon_info[i].num_blow > 1) p_ptr->weapon_info[i].num_blow = 1;

			/* Require at least one blow
			   CTK: No ... negative attacks are now zero attacks!
			 */
			if (p_ptr->weapon_info[i].num_blow < 1) p_ptr->weapon_info[i].num_blow = 0;

			/* Boost digging skill by weapon weight */
			p_ptr->skill_dig += (o_ptr->weight / 10);
		}

		/* Assume okay */
		/* Priest weapon penalty for non-blessed edged weapons */
		if ((p_ptr->pclass == CLASS_PRIEST) && (!(have_flag(flgs, TR_BLESSED))) &&
		    ((o_ptr->tval == TV_SWORD) || (o_ptr->tval == TV_POLEARM)))
		{
			/* Reduce the real bonuses */
			p_ptr->weapon_info[i].to_h -= 2;
			p_ptr->weapon_info[i].to_d -= 2;

			/* Reduce the mental bonuses */
			p_ptr->weapon_info[i].dis_to_h -= 2;
			p_ptr->weapon_info[i].dis_to_d -= 2;

			/* Icky weapon */
			p_ptr->weapon_info[i].icky_wield = TRUE;
		}
		else if (p_ptr->pclass == CLASS_BERSERKER)
		{
			p_ptr->weapon_info[i].to_h += p_ptr->lev/5;
			p_ptr->weapon_info[i].to_d += p_ptr->lev/6;
			p_ptr->weapon_info[i].dis_to_h += p_ptr->lev/5;
			p_ptr->weapon_info[i].dis_to_d += p_ptr->lev/6;
			if (((i == 0) && !p_ptr->hidarite) || p_ptr->ryoute)
			{
				p_ptr->weapon_info[i].to_h += p_ptr->lev/5;
				p_ptr->weapon_info[i].to_d += p_ptr->lev/6;
				p_ptr->weapon_info[i].dis_to_h += p_ptr->lev/5;
				p_ptr->weapon_info[i].dis_to_d += p_ptr->lev/6;
			}
		}
		else if (p_ptr->pclass == CLASS_SORCERER)
		{
			if (!((o_ptr->tval == TV_HAFTED) && ((o_ptr->sval == SV_WIZSTAFF) || (o_ptr->sval == SV_NAMAKE_HAMMER))))
			{
				/* Reduce the real bonuses */
				p_ptr->weapon_info[i].to_h -= 200;
				p_ptr->weapon_info[i].to_d -= 200;

				/* Reduce the mental bonuses */
				p_ptr->weapon_info[i].dis_to_h -= 200;
				p_ptr->weapon_info[i].dis_to_d -= 200;

				/* Icky weapon */
				p_ptr->weapon_info[i].icky_wield = TRUE;
			}
			else
			{
				/* Reduce the real bonuses */
				p_ptr->weapon_info[i].to_h -= 30;
				p_ptr->weapon_info[i].to_d -= 10;

				/* Reduce the mental bonuses */
				p_ptr->weapon_info[i].dis_to_h -= 30;
				p_ptr->weapon_info[i].dis_to_d -= 10;
			}
		}
		/* Hex bonuses */
		if (p_ptr->realm1 == REALM_HEX)
		{
			if (object_is_cursed(o_ptr))
			{
				if (o_ptr->curse_flags & (TRC_CURSED)) { p_ptr->weapon_info[i].to_h += 5; p_ptr->weapon_info[i].dis_to_h += 5; }
				if (o_ptr->curse_flags & (TRC_HEAVY_CURSE)) { p_ptr->weapon_info[i].to_h += 7; p_ptr->weapon_info[i].dis_to_h += 7; }
				if (o_ptr->curse_flags & (TRC_PERMA_CURSE)) { p_ptr->weapon_info[i].to_h += 13; p_ptr->weapon_info[i].dis_to_h += 13; }
				if (o_ptr->curse_flags & (TRC_TY_CURSE)) { p_ptr->weapon_info[i].to_h += 5; p_ptr->weapon_info[i].dis_to_h += 5; }
				if (hex_spelling(HEX_RUNESWORD))
				{
					if (o_ptr->curse_flags & (TRC_CURSED)) { p_ptr->weapon_info[i].to_d += 5; p_ptr->weapon_info[i].dis_to_d += 5; }
					if (o_ptr->curse_flags & (TRC_HEAVY_CURSE)) { p_ptr->weapon_info[i].to_d += 7; p_ptr->weapon_info[i].dis_to_d += 7; }
					if (o_ptr->curse_flags & (TRC_PERMA_CURSE)) { p_ptr->weapon_info[i].to_d += 13; p_ptr->weapon_info[i].dis_to_d += 13; }
				}
			}
		}
		if (p_ptr->riding)
		{
			if ((o_ptr->tval == TV_POLEARM) && ((o_ptr->sval == SV_LANCE) || (o_ptr->sval == SV_HEAVY_LANCE)))
			{
				p_ptr->weapon_info[i].to_h +=15;
				p_ptr->weapon_info[i].dis_to_h +=15;
				p_ptr->weapon_info[i].to_dd += 2;
			}
			else if (!(have_flag(flgs, TR_RIDING)))
			{
				int penalty;
				if ((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY))
				{
					penalty = 5;
				}
				else
				{
					penalty = r_info[m_list[p_ptr->riding].r_idx].level - p_ptr->skill_exp[GINOU_RIDING] / 80;
					penalty += 30;
					if (penalty < 30) penalty = 30;
				}
				p_ptr->weapon_info[i].to_h -= penalty;
				p_ptr->weapon_info[i].dis_to_h -= penalty;

				/* Riding weapon */
				p_ptr->weapon_info[i].riding_wield = TRUE;
			}
		}
	}

	if (p_ptr->riding)
	{
		int penalty = 0;

		p_ptr->riding_ryoute = FALSE;

		if (p_ptr->ryoute || (empty_hands(FALSE) == EMPTY_HAND_NONE)) p_ptr->riding_ryoute = TRUE;
		else if (p_ptr->pet_extra_flags & PF_RYOUTE)
		{
			switch (p_ptr->pclass)
			{
			case CLASS_MONK:
			case CLASS_FORCETRAINER:
			case CLASS_BERSERKER:
				if ((empty_hands(FALSE) != EMPTY_HAND_NONE) && !buki_motteruka(INVEN_RARM) && !buki_motteruka(INVEN_LARM))
					p_ptr->riding_ryoute = TRUE;
				break;
			}
		}

		if ((p_ptr->pclass == CLASS_BEASTMASTER) || (p_ptr->pclass == CLASS_CAVALRY))
		{
			if (p_ptr->tval_ammo != TV_ARROW) penalty = 5;
		}
		else
		{
			penalty = r_info[m_list[p_ptr->riding].r_idx].level - p_ptr->skill_exp[GINOU_RIDING] / 80;
			penalty += 30;
			if (penalty < 30) penalty = 30;
		}
		if (p_ptr->tval_ammo == TV_BOLT) penalty *= 2;
		p_ptr->to_h_b -= penalty;
		p_ptr->dis_to_h_b -= penalty;
	}

	/* Different calculation for monks with empty hands */
	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) &&
		(empty_hands_status & EMPTY_HAND_RARM) && !p_ptr->hidarite)
	{
		int blow_base = p_ptr->lev + adj_dex_blow[p_ptr->stat_ind[A_DEX]];
		p_ptr->weapon_info[0].num_blow = 0;

		if (p_ptr->pclass == CLASS_FORCETRAINER)
		{
			if (blow_base > 18) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 31) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 44) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 58) p_ptr->weapon_info[0].num_blow++;
			if (p_ptr->magic_num1[0])
			{
				p_ptr->weapon_info[0].to_d += (p_ptr->magic_num1[0]/5);
				p_ptr->weapon_info[0].dis_to_d += (p_ptr->magic_num1[0]/5);
			}
		}
		else
		{
			/*if (blow_base > 12) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 22) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 31) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 39) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 46) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 53) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 59) p_ptr->weapon_info[0].num_blow++;*/
			if (blow_base > 12) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 22) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 31) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 41) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 51) p_ptr->weapon_info[0].num_blow++;
			if (blow_base > 59) p_ptr->weapon_info[0].num_blow++;
		}

		if (heavy_armor() && (p_ptr->pclass != CLASS_BERSERKER))
			p_ptr->weapon_info[0].num_blow /= 2;
		else
		{
			p_ptr->weapon_info[0].to_h += (p_ptr->lev / 3);
			p_ptr->weapon_info[0].dis_to_h += (p_ptr->lev / 3);

			p_ptr->weapon_info[0].to_d += (p_ptr->lev / 6);
			p_ptr->weapon_info[0].dis_to_d += (p_ptr->lev / 6);
		}

		if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			p_ptr->to_a -= 40;
			p_ptr->dis_to_a -= 40;
			
		}
		else if (p_ptr->special_defense & KAMAE_SEIRYU)
		{
			p_ptr->to_a -= 50;
			p_ptr->dis_to_a -= 50;
			p_ptr->resist_acid = TRUE;
			p_ptr->resist_fire = TRUE;
			p_ptr->resist_elec = TRUE;
			p_ptr->resist_cold = TRUE;
			p_ptr->resist_pois = TRUE;
			p_ptr->sh_fire = TRUE;
			p_ptr->sh_elec = TRUE;
			p_ptr->sh_cold = TRUE;
			p_ptr->levitation = TRUE;
		}
		else if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->to_a += (p_ptr->lev*p_ptr->lev)/50;
			p_ptr->dis_to_a += (p_ptr->lev*p_ptr->lev)/50;
			p_ptr->reflect = TRUE;
			p_ptr->weapon_info[0].num_blow -= 2;
			if ((p_ptr->pclass == CLASS_MONK) && (p_ptr->lev > 42)) p_ptr->weapon_info[0].num_blow--;
			if (p_ptr->weapon_info[0].num_blow < 0) p_ptr->weapon_info[0].num_blow = 0;
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			p_ptr->weapon_info[0].to_h -= (p_ptr->lev / 3);
			p_ptr->weapon_info[0].to_d -= (p_ptr->lev / 6);

			p_ptr->weapon_info[0].dis_to_h -= (p_ptr->lev / 3);
			p_ptr->weapon_info[0].dis_to_d -= (p_ptr->lev / 6);
			p_ptr->weapon_info[0].num_blow /= 2;
			p_ptr->levitation = TRUE;
		}

		p_ptr->weapon_info[0].num_blow += 1+extra_blows[0];
	}

	/* Hack for Wild Talents assuming Monk Postures */
	if (p_ptr->pclass == CLASS_WILD_TALENT)
	{
		if (p_ptr->special_defense & KAMAE_GENBU)
		{
			p_ptr->weapon_info[0].num_blow -= 2;
			p_ptr->weapon_info[1].num_blow -= 2;
			if (p_ptr->weapon_info[0].num_blow < 0) p_ptr->weapon_info[0].num_blow = 0;
			if (p_ptr->weapon_info[1].num_blow < 0) p_ptr->weapon_info[1].num_blow = 0;
		}
		else if (p_ptr->special_defense & KAMAE_BYAKKO)
		{
			/* Hack: This should "strengthen your attacks" */
			p_ptr->weapon_info[0].num_blow++;
			p_ptr->weapon_info[1].num_blow++;
		}
		else if (p_ptr->special_defense & KAMAE_SUZAKU)
		{
			for (i = 0; i < 2; ++i)
			{
				p_ptr->weapon_info[i].to_h -= (p_ptr->lev / 3);
				p_ptr->weapon_info[i].to_d -= (p_ptr->lev / 6);

				p_ptr->weapon_info[i].dis_to_h -= (p_ptr->lev / 3);
				p_ptr->weapon_info[i].dis_to_d -= (p_ptr->lev / 6);
				p_ptr->weapon_info[i].num_blow /= 2;
			}
		}
	}

	if (p_ptr->riding) p_ptr->levitation = riding_levitation;

	monk_armour_aux = FALSE;

	if (heavy_armor())
	{
		monk_armour_aux = TRUE;
	}

	for (i = 0; i < 2; i++)
	{
		if (buki_motteruka(INVEN_RARM+i))
		{
			int tval = inventory[INVEN_RARM+i].tval - TV_WEAPON_BEGIN;
			int sval = inventory[INVEN_RARM+i].sval;

			if (inventory[INVEN_RARM+i].tval == TV_SHIELD)
			{
				int skill_hack = 6000;
				/* TODO: We should add item skills for shields for the shieldmaster 
				   For now, just give them [Skilled] status ... */
				p_ptr->weapon_info[i].to_h += (skill_hack - WEAPON_EXP_BEGINNER) / 200;
				p_ptr->weapon_info[i].dis_to_h += (skill_hack - WEAPON_EXP_BEGINNER) / 200;
			}
			else
			{
				p_ptr->weapon_info[i].to_h += (p_ptr->weapon_exp[tval][sval] - WEAPON_EXP_BEGINNER) / 200;
				p_ptr->weapon_info[i].dis_to_h += (p_ptr->weapon_exp[tval][sval] - WEAPON_EXP_BEGINNER) / 200;
				if ((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER))
				{
					if (!s_info[p_ptr->pclass].w_max[tval][sval])
					{
						p_ptr->weapon_info[i].to_h -= 40;
						p_ptr->weapon_info[i].dis_to_h -= 40;
						p_ptr->weapon_info[i].icky_wield = TRUE;
					}
				}
				else if (p_ptr->pclass == CLASS_NINJA)
				{
					if ((s_info[CLASS_NINJA].w_max[tval][sval] <= WEAPON_EXP_BEGINNER) || (inventory[INVEN_LARM-i].tval == TV_SHIELD))
					{
						p_ptr->weapon_info[i].to_h -= 40;
						p_ptr->weapon_info[i].dis_to_h -= 40;
						p_ptr->weapon_info[i].icky_wield = TRUE;
						p_ptr->weapon_info[i].num_blow /= 2;
						if (p_ptr->weapon_info[i].num_blow < 1) p_ptr->weapon_info[i].num_blow = 1;
					}
				}
			}

			if (inventory[INVEN_RARM + i].name1 == ART_IRON_BALL) p_ptr->align -= 1000;
		}
	}

	/* Maximum speed is (+99). (internally it's 110 + 99) */
	/* Temporary lightspeed forces to be maximum speed */
	if ((IS_LIGHT_SPEED() && !p_ptr->riding) || (p_ptr->pspeed > 209))
	{
		p_ptr->pspeed = 209;
	}

	/* Minimum speed is (-99). (internally it's 110 - 99) */
	if (p_ptr->pspeed < 11)
		p_ptr->pspeed = 11;

	/* Display the speed (if needed) */
	if (p_ptr->pspeed != old_speed)
		p_ptr->redraw |= (PR_SPEED);

	if (yoiyami)
	{
		if (p_ptr->to_a > (0 - p_ptr->ac))
			p_ptr->to_a = 0 - p_ptr->ac;
		if (p_ptr->dis_to_a > (0 - p_ptr->dis_ac))
			p_ptr->dis_to_a = 0 - p_ptr->dis_ac;
	}

	/* Redraw armor (if needed) */
	if ((p_ptr->dis_ac != old_dis_ac) || (p_ptr->dis_to_a != old_dis_to_a))
	{
		/* Redraw */
		p_ptr->redraw |= (PR_ARMOR);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER);
	}


	if (p_ptr->ryoute && !p_ptr->omoi)
	{
		int bonus_to_h=0, bonus_to_d=0;
		bonus_to_d = ((int)(adj_str_td[p_ptr->stat_ind[A_STR]]) - 128) * 3/4;
		bonus_to_h = ((int)(adj_str_th[p_ptr->stat_ind[A_STR]]) - 128) + ((int)(adj_dex_th[p_ptr->stat_ind[A_DEX]]) - 128);

		p_ptr->weapon_info[default_hand].to_h += MAX(bonus_to_h,1);
		p_ptr->weapon_info[default_hand].dis_to_h += MAX(bonus_to_h,1);
		p_ptr->weapon_info[default_hand].to_d += MAX(bonus_to_d,1);
		p_ptr->weapon_info[default_hand].dis_to_d += MAX(bonus_to_d,1);
	}

	if (((p_ptr->pclass == CLASS_MONK) || (p_ptr->pclass == CLASS_FORCETRAINER) || (p_ptr->pclass == CLASS_BERSERKER)) && (empty_hands(FALSE) == (EMPTY_HAND_RARM | EMPTY_HAND_LARM))) p_ptr->ryoute = FALSE;

	/* Affect Skill -- stealth (bonus one) */
	p_ptr->skills.stl += 1;

	if (IS_TIM_STEALTH()) p_ptr->skills.stl += 99;

	if (p_ptr->tim_dark_stalker)
		p_ptr->skills.stl += 2 + p_ptr->lev/10;

	/* Affect Skill -- disarming (DEX and INT) */
	p_ptr->skills.dis += adj_dex_dis[p_ptr->stat_ind[A_DEX]];
	p_ptr->skills.dis += adj_int_dis[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- magic devices (INT) */
	p_ptr->skills.dev += adj_int_dev[p_ptr->stat_ind[A_INT]];

	/* Affect Skill -- saving throw (WIS) */
	p_ptr->skills.sav += adj_wis_sav[p_ptr->stat_ind[A_WIS]];

	/* Affect Skill -- digging (STR) */
	p_ptr->skill_dig += adj_str_dig[p_ptr->stat_ind[A_STR]];


	if ((prace_is_(RACE_SHADOW_FAIRY)) && (p_ptr->personality != PERS_SEXY) && (p_ptr->cursed & TRC_AGGRAVATE))
	{
		p_ptr->cursed &= ~(TRC_AGGRAVATE);
		p_ptr->skills.stl = MIN(p_ptr->skills.stl - 3, (p_ptr->skills.stl + 2) / 2);
	}

	/* Peerless Stealth is just like the Shadow Fairy, but can even negate the
	   aggravation of Sexy characters! */
	if (p_ptr->peerless_stealth && p_ptr->cursed & TRC_AGGRAVATE)
	{
		p_ptr->cursed &= ~(TRC_AGGRAVATE);
		p_ptr->skills.stl = MIN(p_ptr->skills.stl - 3, (p_ptr->skills.stl + 2) / 2);
	}

	/* Limit Skill -- stealth from 0 to 30 */
	if (p_ptr->skills.stl > 30) p_ptr->skills.stl = 30;
	if (p_ptr->skills.stl < 0) p_ptr->skills.stl = 0;

	/* Limit Skill -- digging from 1 up */
	if (p_ptr->skill_dig < 1) p_ptr->skill_dig = 1;

	if (p_ptr->anti_magic && (p_ptr->skills.sav < (90 + p_ptr->lev))) p_ptr->skills.sav = 90 + p_ptr->lev;

	if (p_ptr->tsubureru) p_ptr->skills.sav = 10;

	if ((p_ptr->ult_res || IS_RESIST_MAGIC() || p_ptr->magicdef) && (p_ptr->skills.sav < (95 + p_ptr->lev))) p_ptr->skills.sav = 95 + p_ptr->lev;

	if (down_saving) p_ptr->skills.sav /= 2;

	/* Hack -- Each elemental immunity includes resistance */
	if (p_ptr->immune_acid) p_ptr->resist_acid = TRUE;
	if (p_ptr->immune_elec) p_ptr->resist_elec = TRUE;
	if (p_ptr->immune_fire) p_ptr->resist_fire = TRUE;
	if (p_ptr->immune_cold) p_ptr->resist_cold = TRUE;

	/* Determine player alignment */
	for (i = 0, j = 0; i < 8; i++)
	{
		switch (p_ptr->vir_types[i])
		{
		case V_JUSTICE:
			p_ptr->align += p_ptr->virtues[i] * 2;
			break;
		case V_CHANCE:
			/* Do nothing */
			break;
		case V_NATURE:
		case V_HARMONY:
			neutral[j++] = i;
			break;
		case V_UNLIFE:
			p_ptr->align -= p_ptr->virtues[i];
			break;
		default:
			p_ptr->align += p_ptr->virtues[i];
			break;
		}
	}

	for (i = 0; i < j; i++)
	{
		if (p_ptr->align > 0)
		{
			p_ptr->align -= p_ptr->virtues[neutral[i]] / 2;
			if (p_ptr->align < 0) p_ptr->align = 0;
		}
		else if (p_ptr->align < 0)
		{
			p_ptr->align += p_ptr->virtues[neutral[i]] / 2;
			if (p_ptr->align > 0) p_ptr->align = 0;
		}
	}

	/* Hack -- handle "xtra" mode */
	if (character_xtra) return;

	/* Take note when "heavy bow" changes */
	if (p_ptr->old_heavy_shoot != p_ptr->heavy_shoot)
	{
		/* Message */
		if (p_ptr->heavy_shoot)
		{
#ifdef JP
			msg_print("����ʽŤ��ݤ��������Ƥ���Τ����Ѥ���");
#else
			msg_print("You have trouble wielding such a heavy bow.");
#endif

		}
		else if (inventory[INVEN_BOW].k_idx)
		{
#ifdef JP
			msg_print("���εݤʤ��������Ƥ��Ƥ�ɤ��ʤ���");
#else
			msg_print("You have no trouble wielding your bow.");
#endif

		}
		else
		{
#ifdef JP
			msg_print("�Ť��ݤ���������Ϥ������Τ��ڤˤʤä���");
#else
			msg_print("You feel relieved to put down your heavy bow.");
#endif

		}

		/* Save it */
		p_ptr->old_heavy_shoot = p_ptr->heavy_shoot;
	}

	for (i = 0 ; i < 2 ; i++)
	{
		/* Take note when "heavy weapon" changes */
		if (p_ptr->old_heavy_wield[i] != p_ptr->weapon_info[i].heavy_wield)
		{
			/* Message */
			if (p_ptr->weapon_info[i].heavy_wield)
			{
#ifdef JP
				msg_print("����ʽŤ������������Ƥ���Τ����Ѥ���");
#else
				msg_print("You have trouble wielding such a heavy weapon.");
#endif

			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("����ʤ��������Ƥ��Ƥ�ɤ��ʤ���");
#else
				msg_print("You have no trouble wielding your weapon.");
#endif

			}
			else if (p_ptr->weapon_info[1-i].heavy_wield)
			{
#ifdef JP
				msg_print("�ޤ���郎�Ť���");
#else
				msg_print("You have still trouble wielding a heavy weapon.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("�Ť�������������Ϥ������Τ��ڤˤʤä���");
#else
				msg_print("You feel relieved to put down your heavy weapon.");
#endif

			}

			/* Save it */
			p_ptr->old_heavy_wield[i] = p_ptr->weapon_info[i].heavy_wield;
		}

		/* Take note when "heavy weapon" changes */
		if (p_ptr->old_riding_wield[i] != p_ptr->weapon_info[i].riding_wield)
		{
			/* Message */
			if (p_ptr->weapon_info[i].riding_wield)
			{
#ifdef JP
				msg_print("�������Ͼ�����˻Ȥ��ˤϤफ�ʤ��褦����");
#else
				msg_print("This weapon is not suitable for use while riding.");
#endif

			}
			else if (!p_ptr->riding)
			{
#ifdef JP
				msg_print("������������ǻȤ��䤹����");
#else
				msg_print("This weapon was not suitable for use while riding.");
#endif

			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("����ʤ������ˤԤä������");
#else
				msg_print("This weapon is suitable for use while riding.");
#endif

			}
			/* Save it */
			p_ptr->old_riding_wield[i] = p_ptr->weapon_info[i].riding_wield;
		}

		/* Take note when "illegal weapon" changes */
		if (p_ptr->old_icky_wield[i] != p_ptr->weapon_info[i].icky_wield)
		{
			/* Message */
			if (p_ptr->weapon_info[i].icky_wield)
			{
#ifdef JP
				msg_print("���������Ϥɤ��⼫ʬ�ˤդ��路���ʤ��������롣");
#else
				msg_print("You do not feel comfortable with your weapon.");
#endif
				if (hack_mind)
				{
					chg_virtue(V_FAITH, -1);
				}
			}
			else if (buki_motteruka(INVEN_RARM+i))
			{
#ifdef JP
				msg_print("���������ϼ�ʬ�ˤդ��路���������롣");
#else
				msg_print("You feel comfortable with your weapon.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("������Ϥ��������ʬ�ȵ����ڤˤʤä���");
#else
				msg_print("You feel more comfortable after removing your weapon.");
#endif

			}

			/* Save it */
			p_ptr->old_icky_wield[i] = p_ptr->weapon_info[i].icky_wield;
		}
	}

	if (p_ptr->riding && (p_ptr->old_riding_ryoute != p_ptr->riding_ryoute))
	{
		/* Message */
		if (p_ptr->riding_ryoute)
		{
#ifdef JP
			msg_format("%s�Ϥ����ʤ���", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "ξ�꤬�դ����äƤ���" : "");
#else
			msg_print("You are using both hand for fighting, and you can't control a riding pet.");
#endif
		}
		else
		{
#ifdef JP
			msg_format("%s�Ϥ�����褦�ˤʤä���", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "�꤬������" : "");
#else
			msg_print("You began to control riding pet with one hand.");
#endif
		}

		p_ptr->old_riding_ryoute = p_ptr->riding_ryoute;
	}

	if ((p_ptr->pclass == CLASS_MONK 
	  || p_ptr->pclass == CLASS_FORCETRAINER
	  || p_ptr->pclass == CLASS_NINJA
	  || p_ptr->pclass == CLASS_SCOUT) && (monk_armour_aux != monk_notify_aux))
	{
		if (heavy_armor())
		{
#ifdef JP
msg_print("�������Ť��ƥХ�󥹤���ʤ���");
#else
			msg_print("The weight of your armor disrupts your balance.");
#endif

			if (hack_mind)
			{
				chg_virtue(V_HARMONY, -1);
			}
		}
		else
#ifdef JP
msg_print("�Х�󥹤��Ȥ��褦�ˤʤä���");
#else
			msg_print("You regain your balance.");
#endif

		monk_notify_aux = monk_armour_aux;
	}

	for (i = 0; i < INVEN_PACK; i++)
	{
#if 0
		if ((inventory[i].tval == TV_SORCERY_BOOK) && (inventory[i].sval == 2)) have_dd_s = TRUE;
		if ((inventory[i].tval == TV_TRUMP_BOOK) && (inventory[i].sval == 1)) have_dd_t = TRUE;
#endif
		if ((inventory[i].tval == TV_NATURE_BOOK) && (inventory[i].sval == 2)) have_sw = TRUE;
		if ((inventory[i].tval == TV_CRAFT_BOOK) && (inventory[i].sval == 2)) have_kabe = TRUE;
	}
	for (this_o_idx = cave[py][px].o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

#if 0
		if ((o_ptr->tval == TV_SORCERY_BOOK) && (o_ptr->sval == 3)) have_dd_s = TRUE;
		if ((o_ptr->tval == TV_TRUMP_BOOK) && (o_ptr->sval == 1)) have_dd_t = TRUE;
#endif
		if ((o_ptr->tval == TV_NATURE_BOOK) && (o_ptr->sval == 2)) have_sw = TRUE;
		if ((o_ptr->tval == TV_CRAFT_BOOK) && (o_ptr->sval == 2)) have_kabe = TRUE;
	}

	if (p_ptr->pass_wall && !p_ptr->kill_wall) p_ptr->no_flowed = TRUE;
#if 0
	if (have_dd_s && ((p_ptr->realm1 == REALM_SORCERY) || (p_ptr->realm2 == REALM_SORCERY) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_SORCERY-1][SPELL_DD_S];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	if (have_dd_t && ((p_ptr->realm1 == REALM_TRUMP) || (p_ptr->realm2 == REALM_TRUMP) || (p_ptr->pclass == CLASS_SORCERER) || (p_ptr->pclass == CLASS_RED_MAGE)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_TRUMP-1][SPELL_DD_T];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}
#endif
	if (have_sw && ((p_ptr->realm1 == REALM_NATURE) || (p_ptr->realm2 == REALM_NATURE) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_NATURE-1][SPELL_SW];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	if (have_kabe && ((p_ptr->realm1 == REALM_CRAFT) || (p_ptr->realm2 == REALM_CRAFT) || (p_ptr->pclass == CLASS_SORCERER)))
	{
		magic_type *s_ptr = &mp_ptr->info[REALM_CRAFT-1][SPELL_KABE];
		if (p_ptr->lev >= s_ptr->slevel) p_ptr->no_flowed = TRUE;
	}

	/* Apply some maximums ... */
	if (p_ptr->magic_resistance > 25)
		p_ptr->magic_resistance = 25;

	/* Hack: Vicious Strike should not put AC below 0, but I can't find out a better
	   way to achieve this! */
	if (p_ptr->ac + p_ptr->to_a < 0 && p_ptr->tim_vicious_strike)
	{
		int delta = -(p_ptr->ac + p_ptr->to_a);
		p_ptr->to_a += delta;
		p_ptr->dis_to_a += delta;
	}
}



/*
 * Handle "p_ptr->notice"
 */
void notice_stuff(void)
{
	/* Notice stuff */
	if (!p_ptr->notice) return;


	/* Actually do auto-destroy */
	if (p_ptr->notice & (PN_AUTODESTROY))
	{
		p_ptr->notice &= ~(PN_AUTODESTROY);
		autopick_delayed_alter();
	}

	/* Combine the pack */
	if (p_ptr->notice & (PN_COMBINE))
	{
		p_ptr->notice &= ~(PN_COMBINE);
		combine_pack();
	}

	/* Reorder the pack */
	if (p_ptr->notice & (PN_REORDER))
	{
		p_ptr->notice &= ~(PN_REORDER);
		reorder_pack();
	}
}


/*
 * Handle "p_ptr->update"
 */
void update_stuff(void)
{
	/* Update stuff */
	if (!p_ptr->update) return;


	if (p_ptr->update & (PU_BONUS))
	{
		p_ptr->update &= ~(PU_BONUS);
		calc_bonuses();
	}

	if (p_ptr->update & (PU_TORCH))
	{
		p_ptr->update &= ~(PU_TORCH);
		calc_torch();
	}

	if (p_ptr->update & (PU_HP))
	{
		p_ptr->update &= ~(PU_HP);
		calc_hitpoints();
	}

	if (p_ptr->update & (PU_MANA))
	{
		p_ptr->update &= ~(PU_MANA);
		calc_mana();
	}

	if (p_ptr->update & (PU_SPELLS))
	{
		p_ptr->update &= ~(PU_SPELLS);
		calc_spells();
	}


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;


	if (p_ptr->update & (PU_UN_LITE))
	{
		p_ptr->update &= ~(PU_UN_LITE);
		forget_lite();
	}

	if (p_ptr->update & (PU_UN_VIEW))
	{
		p_ptr->update &= ~(PU_UN_VIEW);
		forget_view();
	}

	if (p_ptr->update & (PU_VIEW))
	{
		p_ptr->update &= ~(PU_VIEW);
		update_view();
	}

	if (p_ptr->update & (PU_LITE))
	{
		p_ptr->update &= ~(PU_LITE);
		update_lite();
	}


	if (p_ptr->update & (PU_FLOW))
	{
		p_ptr->update &= ~(PU_FLOW);
		update_flow();
	}

	if (p_ptr->update & (PU_DISTANCE))
	{
		p_ptr->update &= ~(PU_DISTANCE);

		/* Still need to call update_monsters(FALSE) after update_mon_lite() */ 
		/* p_ptr->update &= ~(PU_MONSTERS); */

		update_monsters(TRUE);
	}

	if (p_ptr->update & (PU_MON_LITE))
	{
		p_ptr->update &= ~(PU_MON_LITE);
		update_mon_lite();
	}

	/*
	 * Mega-Hack -- Delayed visual update
	 * Only used if update_view(), update_lite() or update_mon_lite() was called
	 */
	if (p_ptr->update & (PU_DELAY_VIS))
	{
		p_ptr->update &= ~(PU_DELAY_VIS);
		delayed_visual_update();
	}

	if (p_ptr->update & (PU_MONSTERS))
	{
		p_ptr->update &= ~(PU_MONSTERS);
		update_monsters(FALSE);
	}
}


/*
 * Handle "p_ptr->redraw"
 */
void redraw_stuff(void)
{
	/* Redraw stuff */
	if (!p_ptr->redraw) return;


	/* Character is not ready yet, no screen updates */
	if (!character_generated) return;


	/* Character is in "icky" mode, no screen updates */
	if (character_icky) return;



	/* Hack -- clear the screen */
	if (p_ptr->redraw & (PR_WIPE))
	{
		p_ptr->redraw &= ~(PR_WIPE);
		msg_print(NULL);
		Term_clear();
	}


	if (p_ptr->redraw & (PR_MAP))
	{
		p_ptr->redraw &= ~(PR_MAP);
		prt_map();
	}


	if (p_ptr->redraw & (PR_BASIC))
	{
		p_ptr->redraw &= ~(PR_BASIC);
		p_ptr->redraw &= ~(PR_MISC | PR_TITLE | PR_STATS);
		p_ptr->redraw &= ~(PR_LEV | PR_EXP | PR_GOLD);
		p_ptr->redraw &= ~(PR_ARMOR | PR_HP | PR_MANA);
		p_ptr->redraw &= ~(PR_DEPTH | PR_HEALTH | PR_UHEALTH);
		prt_frame_basic();
		prt_time();
		prt_dungeon();
	}

	if (p_ptr->redraw & (PR_EQUIPPY))
	{
		p_ptr->redraw &= ~(PR_EQUIPPY);
		print_equippy(); /* To draw / delete equippy chars */
	}

	if (p_ptr->redraw & (PR_MISC))
	{
		char buf[100];
		race_t *race_ptr = get_race_t();
		p_ptr->redraw &= ~(PR_MISC);
		if (race_ptr->mimic)
			sprintf(buf, "[%s]", race_ptr->name);
		else
			sprintf(buf, "%s", race_ptr->name);
		prt_field(buf, ROW_RACE, COL_RACE);
	}

	if (p_ptr->redraw & (PR_TITLE))
	{
		p_ptr->redraw &= ~(PR_TITLE);
		prt_title();
	}

	if (p_ptr->redraw & (PR_LEV))
	{
		p_ptr->redraw &= ~(PR_LEV);
		prt_level();
	}

	if (p_ptr->redraw & (PR_EXP))
	{
		p_ptr->redraw &= ~(PR_EXP);
		prt_exp();
	}

	if (p_ptr->redraw & (PR_STATS))
	{
		p_ptr->redraw &= ~(PR_STATS);
		prt_stat(A_STR);
		prt_stat(A_INT);
		prt_stat(A_WIS);
		prt_stat(A_DEX);
		prt_stat(A_CON);
		prt_stat(A_CHR);
	}

	if (p_ptr->redraw & (PR_STATUS))
	{
		p_ptr->redraw &= ~(PR_STATUS);
		prt_status();
	}

	if (p_ptr->redraw & (PR_ARMOR))
	{
		p_ptr->redraw &= ~(PR_ARMOR);
		prt_ac();
	}

	if (p_ptr->redraw & (PR_HP))
	{
		p_ptr->redraw &= ~(PR_HP);
		prt_hp();
	}

	if (p_ptr->redraw & (PR_MANA))
	{
		p_ptr->redraw &= ~(PR_MANA);
		prt_sp();
	}

	if (p_ptr->redraw & (PR_GOLD))
	{
		p_ptr->redraw &= ~(PR_GOLD);
		prt_gold();
	}

	if (p_ptr->redraw & (PR_DEPTH))
	{
		p_ptr->redraw &= ~(PR_DEPTH);
		prt_depth();
	}

	if (p_ptr->redraw & (PR_HEALTH))
	{
		p_ptr->redraw &= ~(PR_HEALTH);
		health_redraw(FALSE);
	}

	if (p_ptr->redraw & (PR_UHEALTH))
	{
		p_ptr->redraw &= ~(PR_UHEALTH);
		health_redraw(TRUE);
	}


	if (p_ptr->redraw & (PR_EXTRA))
	{
		p_ptr->redraw &= ~(PR_EXTRA);
		p_ptr->redraw &= ~(PR_CUT | PR_STUN | PR_FEAR);
		p_ptr->redraw &= ~(PR_HUNGER);
		p_ptr->redraw &= ~(PR_STATE | PR_SPEED | PR_STUDY | PR_IMITATION | PR_STATUS);
		prt_frame_extra();
	}

	if (p_ptr->redraw & (PR_CUT))
	{
		p_ptr->redraw &= ~(PR_CUT);
		prt_cut();
	}

	if (p_ptr->redraw & (PR_STUN))
	{
		p_ptr->redraw &= ~(PR_STUN);
		prt_stun();
	}

	if (p_ptr->redraw & PR_FEAR)
	{
		p_ptr->redraw &= ~PR_FEAR;
		prt_fear();
	}

	if (p_ptr->redraw & (PR_HUNGER))
	{
		p_ptr->redraw &= ~(PR_HUNGER);
		prt_hunger();
	}

	if (p_ptr->redraw & (PR_STATE))
	{
		p_ptr->redraw &= ~(PR_STATE);
		prt_state();
	}

	if (p_ptr->redraw & (PR_SPEED))
	{
		p_ptr->redraw &= ~(PR_SPEED);
		prt_speed();
	}

	if (p_ptr->pclass == CLASS_IMITATOR)
	{
		if (p_ptr->redraw & (PR_IMITATION))
		{
			p_ptr->redraw &= ~(PR_IMITATION);
			prt_imitation();
		}
	}
	else if (p_ptr->redraw & (PR_STUDY))
	{
		p_ptr->redraw &= ~(PR_STUDY);
		prt_study();
	}
}


/*
 * Handle "p_ptr->window"
 */
void window_stuff(void)
{
	int j;

	u32b mask = 0L;


	/* Nothing to do */
	if (!p_ptr->window) return;

	/* Scan windows */
	for (j = 0; j < 8; j++)
	{
		/* Save usable flags */
		if (angband_term[j]) mask |= window_flag[j];
	}

	/* Apply usable flags */
	p_ptr->window &= mask;

	/* Nothing to do */
	if (!p_ptr->window) return;


	/* Display inventory */
	if (p_ptr->window & (PW_INVEN))
	{
		p_ptr->window &= ~(PW_INVEN);
		fix_inven();
	}

	/* Display equipment */
	if (p_ptr->window & (PW_EQUIP))
	{
		p_ptr->window &= ~(PW_EQUIP);
		fix_equip();
	}

	/* Display spell list */
	if (p_ptr->window & (PW_SPELL))
	{
		p_ptr->window &= ~(PW_SPELL);
		fix_spell();
	}

	/* Display player */
	if (p_ptr->window & (PW_PLAYER))
	{
		p_ptr->window &= ~(PW_PLAYER);
		fix_player();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_MESSAGE))
	{
		p_ptr->window &= ~(PW_MESSAGE);
		fix_message();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_OVERHEAD))
	{
		p_ptr->window &= ~(PW_OVERHEAD);
		fix_overhead();
	}

	/* Display overhead view */
	if (p_ptr->window & (PW_DUNGEON))
	{
		p_ptr->window &= ~(PW_DUNGEON);
		fix_dungeon();
	}

	/* Display monster recall */
	if (p_ptr->window & (PW_MONSTER))
	{
		p_ptr->window &= ~(PW_MONSTER);
		fix_monster();
	}

	/* Display object recall */
	if (p_ptr->window & (PW_OBJECT))
	{
		p_ptr->window &= ~(PW_OBJECT);
		fix_object();
	}
}


/*
 * Handle "p_ptr->update" and "p_ptr->redraw" and "p_ptr->window"
 */
void handle_stuff(void)
{
	/* Update stuff */
	if (p_ptr->update) update_stuff();

	/* Redraw stuff */
	if (p_ptr->redraw) redraw_stuff();

	/* Window stuff */
	if (p_ptr->window) window_stuff();
}


s16b empty_hands(bool riding_control)
{
	s16b status = EMPTY_HAND_NONE;

	if (!inventory[INVEN_RARM].k_idx) status |= EMPTY_HAND_RARM;
	if (!inventory[INVEN_LARM].k_idx) status |= EMPTY_HAND_LARM;

	if (riding_control && (status != EMPTY_HAND_NONE) && p_ptr->riding && !(p_ptr->pet_extra_flags & PF_RYOUTE))
	{
		if (status & EMPTY_HAND_LARM) status &= ~(EMPTY_HAND_LARM);
		else if (status & EMPTY_HAND_RARM) status &= ~(EMPTY_HAND_RARM);
	}

	return status;
}


bool heavy_armor(void)
{
	u16b monk_arm_wgt = 0;

	if (p_ptr->pclass != CLASS_MONK
	 && p_ptr->pclass != CLASS_FORCETRAINER
	 && p_ptr->pclass != CLASS_NINJA
	 && p_ptr->pclass != CLASS_SCOUT)
	{
		return FALSE;
	}

	/* Weight the armor */
	if(inventory[INVEN_RARM].tval > TV_SWORD) monk_arm_wgt += inventory[INVEN_RARM].weight;
	if(inventory[INVEN_LARM].tval > TV_SWORD) monk_arm_wgt += inventory[INVEN_LARM].weight;
	monk_arm_wgt += inventory[INVEN_BODY].weight;
	monk_arm_wgt += inventory[INVEN_HEAD].weight;
	monk_arm_wgt += inventory[INVEN_OUTER].weight;
	monk_arm_wgt += inventory[INVEN_HANDS].weight;
	monk_arm_wgt += inventory[INVEN_FEET].weight;

	return (monk_arm_wgt > (100 + (p_ptr->lev * 4)));
}

