#include "angband.h"

/****************************************************************
 * Klackon
 ****************************************************************/
static power_info _klackon_powers[] =
{
	{ A_DEX, {9, 9, 50, spit_acid_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _klackon_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _klackon_powers);
}
static void _klackon_calc_bonuses(void)
{
	p_ptr->resist_conf = TRUE;
	p_ptr->resist_acid = TRUE;
	p_ptr->pspeed += (p_ptr->lev) / 10;
}
static void _klackon_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_CONF);
	add_flag(flgs, TR_RES_ACID);
	if (p_ptr->lev > 9)
		add_flag(flgs, TR_SPEED);
}
static void _klackon_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _klackon_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Resist Acid\n");
	fprintf(fff, "  * Resist Confusion\n");
	fprintf(fff, "  * +L/10 Speed\n");
}
race_t *klackon_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Klackon";
		me.desc = "Klackons are bizarre semi-intelligent ant-like insectoid creatures. "
					"They make great fighters, but their mental abilities are severely limited. "
					"Obedient and well-ordered, they can never be confused. They are also very "
					"nimble, and become faster as they advance levels. They are also very acidic, "
					"inherently resisting acid, and capable of spitting acid at higher levels.";

		me.stats[A_STR] =  2;
		me.stats[A_INT] = -1;
		me.stats[A_WIS] = -1;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] =  2;
		me.stats[A_CHR] =  1;
		
		me.skills.dis = 10;
		me.skills.dev = 5;
		me.skills.sav = 3;
		me.skills.stl = 0;
		me.skills.srh = -1;
		me.skills.fos = 10;
		me.skills.thn = 5;
		me.skills.thb = 5;

		me.hd = 12;
		me.exp = 205;
		me.infra = 2;

		me.calc_bonuses = _klackon_calc_bonuses;
		me.get_powers = _klackon_get_powers;
		me.get_flags = _klackon_get_flags;
		me.spoiler_dump = _klackon_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Kobold
 ****************************************************************/
static power_info _kobold_powers[] =
{
	{ A_DEX, {12, 8, 50, poison_dart_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _kobold_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _kobold_powers);
}
static void _kobold_calc_bonuses(void)
{
	p_ptr->resist_pois = TRUE;
}
static void _kobold_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_POIS);
}
static void _kobold_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _kobold_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Resist Poison\n");
}
race_t *kobold_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Kobold";
		me.desc = "Kobolds are a weak goblin race. They love poisoned weapons, and can learn to throw "
					"poisoned darts (of which they carry an unlimited supply). They are also inherently "
					"resistant to poison, although they are not one of the more powerful races.";

		me.stats[A_STR] =  1;
		me.stats[A_INT] = -1;
		me.stats[A_WIS] =  0;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] =  0;
		me.stats[A_CHR] = -2;
		
		me.skills.dis = -2;
		me.skills.dev = -3;
		me.skills.sav = -1;
		me.skills.stl = -1;
		me.skills.srh =  1;
		me.skills.fos =  8;
		me.skills.thn = 10;
		me.skills.thb = -8;

		me.hd = 9;
		me.exp = 90;
		me.infra = 3;

		me.calc_bonuses = _kobold_calc_bonuses;
		me.get_powers = _kobold_get_powers;
		me.get_flags = _kobold_get_flags;
		me.spoiler_dump = _kobold_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Kutar
 ****************************************************************/
static power_info _kutar_powers[] =
{
	{ A_CHR, {20, 15, 70, kutar_expand_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _kutar_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _kutar_powers);
}
static void _kutar_calc_bonuses(void)
{
	p_ptr->resist_conf = TRUE;
}
static void _kutar_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_CONF);
}
static void _kutar_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _kutar_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Resist Confusion\n");
}
race_t *kutar_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Kutar";
		me.desc = "A Kutar is an expressionless animal-like living creature. The word 'kuta' means "
					"'absentmindedly' or 'vacantly'. Their absentmindedness hurts their searching and "
					"perception skills, but renders them incapable of being confused. Their unearthly "
					"calmness and serenity make them among the most stealthy of any race. Kutars, "
					"although expressionless, are beautiful and so have a high charisma. Members of "
					"this race can learn to expand their body horizontally. This increases armour class, "
					"but renders them vulnerable to magical attacks.";

		me.stats[A_STR] =  0;
		me.stats[A_INT] = -1;
		me.stats[A_WIS] = -1;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] =  2;
		me.stats[A_CHR] =  0;
		
		me.skills.dis = -2;
		me.skills.dev = 5;
		me.skills.sav = 5;
		me.skills.stl = 5;
		me.skills.srh = -2;
		me.skills.fos = 6;
		me.skills.thn = 0;
		me.skills.thb = -5;

		me.hd = 11;
		me.exp = 175;
		me.infra = 0;

		me.calc_bonuses = _kutar_calc_bonuses;
		me.get_powers = _kutar_get_powers;
		me.get_flags = _kutar_get_flags;
		me.spoiler_dump = _kutar_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Mindflayer
 ****************************************************************/
static power_info _mindflayer_powers[] =
{
	{ A_INT, {5, 3, 50, mind_blast_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _mindflayer_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _mindflayer_powers);
}
static void _mindflayer_calc_bonuses(void)
{
	p_ptr->sustain_int = TRUE;
	p_ptr->sustain_wis = TRUE;
	if (p_ptr->lev >= 15) p_ptr->see_inv = TRUE;
	if (p_ptr->lev >= 30) p_ptr->telepathy = TRUE;
}
static void _mindflayer_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_SUST_INT);
	add_flag(flgs, TR_SUST_WIS);
	if (p_ptr->lev >= 15)
		add_flag(flgs, TR_SEE_INVIS);
	if (p_ptr->lev >= 30)
		add_flag(flgs, TR_TELEPATHY);
}
static void _mindflayer_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _mindflayer_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Sustain Intelligence\n");
	fprintf(fff, "  * Sustain Wisdom\n");
	fprintf(fff, "  * See Invisible at L15\n");
	fprintf(fff, "  * Telepathy at L30\n");
}
race_t *mindflayer_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Mindflayer";
		me.desc = "Mindflayers are a secretive and mysterious ancient race. Their civilization may well "
					"be older than any other on our planet, and their intelligence and wisdom are "
					"naturally sustained, and are so great that they enable Mindflayers to become more "
					"powerful spellcasters than any other race, even if their physical attributes are "
					"a good deal less admirable.  As they advance levels, they gain the powers of "
					"See Invisible and Telepathy.";

		me.stats[A_STR] = -3;
		me.stats[A_INT] =  4;
		me.stats[A_WIS] =  4;
		me.stats[A_DEX] =  0;
		me.stats[A_CON] = -2;
		me.stats[A_CHR] = -1;
		
		me.skills.dis = 10;
		me.skills.dev = 15;
		me.skills.sav = 9;
		me.skills.stl = 2;
		me.skills.srh = 5;
		me.skills.fos = 12;
		me.skills.thn = -10;
		me.skills.thb = -5;

		me.hd = 9;
		me.exp = 145;
		me.infra = 4;

		me.calc_bonuses = _mindflayer_calc_bonuses;
		me.get_powers = _mindflayer_get_powers;
		me.get_flags = _mindflayer_get_flags;
		me.spoiler_dump = _mindflayer_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Mithril-Golem
 ****************************************************************/
static void _mithril_golem_calc_bonuses(void)
{
	p_ptr->free_act = TRUE;
	p_ptr->see_inv = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->resist_pois = TRUE;
	p_ptr->resist_shard = TRUE;
	p_ptr->reflect = TRUE;
	p_ptr->pspeed -= 2;
	p_ptr->to_a += 20;
	p_ptr->dis_to_a += 20;
}
static void _mithril_golem_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_FREE_ACT);
	add_flag(flgs, TR_SEE_INVIS);
	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_RES_POIS);
	add_flag(flgs, TR_SPEED);
	add_flag(flgs, TR_RES_SHARDS);
	add_flag(flgs, TR_REFLECT);
}
static void _mithril_golem_spoiler_dump(FILE *fff)
{
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * -2 to Speed\n");
	fprintf(fff, "  * +20 to Armor Class\n");
	fprintf(fff, "  * Free Action\n");
	fprintf(fff, "  * See Invisible\n");
	fprintf(fff, "  * Hold Life\n");
	fprintf(fff, "  * Resist Poison\n");
	fprintf(fff, "  * Resist Shards\n");
	fprintf(fff, "  * Reflection\n");
}
race_t *mithril_golem_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Mithril-Golem";
		me.desc = "";

		me.stats[A_STR] =  5;
		me.stats[A_INT] =  1;
		me.stats[A_WIS] =  1;
		me.stats[A_DEX] = -3;
		me.stats[A_CON] =  5;
		me.stats[A_CHR] =  2;
		
		me.skills.dis =  0;
		me.skills.dev =  0;
		me.skills.sav = 25;
		me.skills.stl = -3;
		me.skills.srh = -2;
		me.skills.fos = 5;
		me.skills.thn = 50;
		me.skills.thb = -10;

		me.hd = 13;
		me.exp = 500;
		me.infra = 4;
		me.flags = RACE_IS_NONLIVING;

		me.calc_bonuses = _mithril_golem_calc_bonuses;
		me.get_flags = _mithril_golem_get_flags;
		me.spoiler_dump = _mithril_golem_spoiler_dump;
		init = TRUE;
	}

	return &me;
}


/****************************************************************
 * Nibelung
 ****************************************************************/
static power_info _nibelung_powers[] =
{
	{ A_WIS, {10, 5, 50, detect_doors_stairs_traps_spell}},
	{ A_CHR, {10, 5, 50, detect_treasure_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _nibelung_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _nibelung_powers);
}
static void _nibelung_calc_bonuses(void)
{
	p_ptr->resist_disen = TRUE;
	p_ptr->resist_dark = TRUE;
}
static void _nibelung_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_DISEN);
	add_flag(flgs, TR_RES_DARK);
}
static void _nibelung_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _nibelung_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Resist Dark\n");
	fprintf(fff, "  * Resist Disenchantment\n");
}
race_t *nibelung_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Nibelung";
		me.desc = "The hated and persecuted race of nocturnal dwarves, these cave-dwellers are "
					"not much bothered by darkness. Their natural inclination to magical items "
					"has made them immune to effects which could drain away magical energy.";

		me.stats[A_STR] =  1;
		me.stats[A_INT] = -1;
		me.stats[A_WIS] =  2;
		me.stats[A_DEX] =  0;
		me.stats[A_CON] =  2;
		me.stats[A_CHR] = -2;
		
		me.skills.dis =  3;
		me.skills.dev =  5;
		me.skills.sav =  6;
		me.skills.stl =  1;
		me.skills.srh =  5;
		me.skills.fos = 10;
		me.skills.thn =  9;
		me.skills.thb =  0;

		me.hd = 11;
		me.exp = 165;
		me.infra = 5;

		me.calc_bonuses = _nibelung_calc_bonuses;
		me.get_powers = _nibelung_get_powers;
		me.get_flags = _nibelung_get_flags;
		me.spoiler_dump = _nibelung_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Shadow-Fairy
 ****************************************************************/
static void _shadow_fairy_calc_bonuses(void)
{
	p_ptr->levitation = TRUE;
}
static void _shadow_fairy_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_LEVITATION);
}
static void _shadow_fairy_get_vulnerabilities(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_LITE);
}
static void _shadow_fairy_spoiler_dump(FILE *fff)
{
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Levitation\n");
	fprintf(fff, "  * Vulnerability to Light\n");
	fprintf(fff, "  * Resist Aggravation\n");
}
race_t *shadow_fairy_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Shadow-Fairy";
		me.desc = "Shadow Fairies are one of the several fairy races. They have wings, and can fly over "
					"traps that may open up beneath them. Shadow Fairies must beware of sunlight, as "
					"they are vulnerable to bright light. They are physically weak, but have advantages "
					"in using magic and are amazingly stealthy. Shadow Fairies have a wonderful advantage "
					"in that they never aggravate monsters (If their equipment normally aggravates monsters, "
					"they only suffer a penalty to stealth, but if they aggravate by their personality "
					"itself, the advantage will be lost).";

		me.stats[A_STR] = -2;
		me.stats[A_INT] =  2;
		me.stats[A_WIS] =  2;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] = -1;
		me.stats[A_CHR] = -3;
		
		me.skills.dis =  7;
		me.skills.dev =  8;
		me.skills.sav =  0;
		me.skills.stl =  6;
		me.skills.srh = 12;
		me.skills.fos = 15;
		me.skills.thn =-10;
		me.skills.thb = -5;

		me.hd = 7;
		me.exp = 140;
		me.infra = 4;

		me.calc_bonuses = _shadow_fairy_calc_bonuses;
		me.get_flags = _shadow_fairy_get_flags;
		me.get_vulnerabilities = _shadow_fairy_get_vulnerabilities;
		me.spoiler_dump = _shadow_fairy_spoiler_dump;
		init = TRUE;
	}

	return &me;
}


/****************************************************************
 * Skeleton
 ****************************************************************/
static power_info _skeleton_powers[] =
{
	{ A_WIS, {30, 30, 70, restore_life_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _skeleton_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _skeleton_powers);
}
static void _skeleton_calc_bonuses(void)
{
	p_ptr->resist_shard = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->see_inv = TRUE;
	p_ptr->resist_pois = TRUE;
	if (p_ptr->lev >= 10) p_ptr->resist_cold = TRUE;
}
static void _skeleton_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_SEE_INVIS);
	add_flag(flgs, TR_RES_SHARDS);
	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_RES_POIS);
	if (p_ptr->lev >= 10)
		add_flag(flgs, TR_RES_COLD);
}
static void _skeleton_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _skeleton_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * See Invisible\n");
	fprintf(fff, "  * Hold Life\n");
	fprintf(fff, "  * Resist Poison\n");
	fprintf(fff, "  * Resist Shards\n");
	fprintf(fff, "  * Resist Cold at L10\n");
}
race_t *skeleton_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Skeleton";
		me.desc = "There are two types of skeletons: the ordinary, warrior-like skeletons, and the "
					"spell-using skeletons, which are also called liches. As undead beings, skeletons "
					"need to worry very little about poison or attacks that can drain life. They do "
					"not really use eyes for perceiving things, and are thus not fooled by invisibility. "
					"Their bones are resistant to sharp shrapnel, and they will quickly become resistant "
					"to cold. Although the magical effects of these will affect the skeleton even "
					"without entering the skeleton's (non-existent) belly, the potion or food itself "
					"will fall through the skeleton's jaws, giving no nutritional benefit. They can "
					"absorb mana from staves and wands as their energy source.";

		me.stats[A_STR] =  0;
		me.stats[A_INT] =  1;
		me.stats[A_WIS] = -2;
		me.stats[A_DEX] =  0;
		me.stats[A_CON] =  1;
		me.stats[A_CHR] =  1;
		
		me.skills.dis = -5;
		me.skills.dev = 0;
		me.skills.sav = 3;
		me.skills.stl = -1;
		me.skills.srh = -1;
		me.skills.fos = 8;
		me.skills.thn = 10;
		me.skills.thb = 0;

		me.hd = 10;
		me.exp = 115;
		me.infra = 2;
		me.flags = RACE_IS_NONLIVING | RACE_IS_UNDEAD;

		me.calc_bonuses = _skeleton_calc_bonuses;
		me.get_powers = _skeleton_get_powers;
		me.get_flags = _skeleton_get_flags;
		me.spoiler_dump = _skeleton_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Snotling
 ****************************************************************/
static void _devour_flesh_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Devour Flesh");
		break;
	case SPELL_DESC:
		var_set_string(res, "Devour flesh (yours) in order to fill your belly.");
		break;
	case SPELL_CAST:
		msg_print("You devour your own flesh!");
		set_food(PY_FOOD_MAX - 1);
		set_cut(p_ptr->cut + CUT_SEVERE, FALSE);
		take_hit(DAMAGE_USELIFE, p_ptr->mhp / 3, "devouring your own flesh", -1);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static power_info _snotling_powers[] =
{
	{ A_CHR, {1, 0, 0, _devour_flesh_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _snotling_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _snotling_powers);
}
static void _snotling_calc_bonuses(void)
{
}
static void _snotling_get_flags(u32b flgs[TR_FLAG_SIZE])
{
}
static void _snotling_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _snotling_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * *Bullied* - Every time you are attacked (melee, ranged, or spell), you must make a save or be in Fear. If you resist fear, then this still applies but only 1 in 7.\n");
	fprintf(fff, "  * *Recoil* - Whenever you fire a missile weapon, you are pushed 1 square back in the opposite direction.  If you would be pushed into a wall, you take damage instead.\n");
	fprintf(fff, "  * *Mushroom Addict* - Eating any Mushroom gives +10 speed, Stone Skin, Heroism, and Giant Strength for X+dX turns (where X is native level of mushroom), in addition to normal mushroom effects.\n");
	fprintf(fff, "  * *Treasured Mushroom* - You can read a Scroll of Artifact Creation on a mushroom to make it `The Eternal Mushroom of xxx` (where xxx is whatever you used it on, presumably Restoring).  If you eat the Eternal Mushroom, it goes into `(charging)` for 100 rounds, and then becomes available for eating again!\n");
}
race_t *snotling_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Snotling";
		me.desc = "Snotlings are greenskins, and are cousins of Goblins and Orcs, smaller than the "
					"former and dumber than the latter, often used by them as cannon fodder, food "
					"or even cannon missiles. They are on the lowest rung of greenskin society and "
					"are bullied by all. Snotlings often wield mushrooms or sticks into battle.";

		me.stats[A_STR] = -1;
		me.stats[A_INT] = -3;
		me.stats[A_WIS] = -2;
		me.stats[A_DEX] = -1;
		me.stats[A_CON] = -1;
		me.stats[A_CHR] = -2;
		
		me.skills.dis = -3;
		me.skills.dev = -3;
		me.skills.sav = -2;
		me.skills.stl = 1;
		me.skills.srh = 0;
		me.skills.fos = 7;
		me.skills.thn = -5;
		me.skills.thb = -5;

		me.hd = 8;
		me.exp = 65;
		me.infra = 3;

		me.calc_bonuses = _snotling_calc_bonuses;
		me.get_powers = _snotling_get_powers;
		me.get_flags = _snotling_get_flags;
		me.spoiler_dump = _snotling_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Spectre
 ****************************************************************/
static power_info _spectre_powers[] =
{
	{ A_INT, {4, 6, 50, scare_monster_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _spectre_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _spectre_powers);
}
static void _spectre_calc_bonuses(void)
{
	p_ptr->levitation = TRUE;
	p_ptr->resist_neth = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->see_inv = TRUE;
	p_ptr->resist_pois = TRUE;
	p_ptr->slow_digest = TRUE;
	p_ptr->resist_cold = TRUE;
	p_ptr->pass_wall = TRUE;
}
static void _spectre_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_LEVITATION);
	add_flag(flgs, TR_RES_COLD);
	add_flag(flgs, TR_SEE_INVIS);
	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_RES_NETHER);
	add_flag(flgs, TR_RES_POIS);
	add_flag(flgs, TR_SLOW_DIGEST);
}
static void _spectre_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _spectre_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Slow Digestion\n");
	fprintf(fff, "  * Levitation\n");
	fprintf(fff, "  * See Invisible\n");
	fprintf(fff, "  * Hold Life\n");
	fprintf(fff, "  * Resist Cold\n");
	fprintf(fff, "  * Resist Poison\n");
	fprintf(fff, "  * *Resist* Nether\n");
	fprintf(fff, "  * Pass Wall _(Note: Damage 1d(MHP/24) per round)_\n");
}
race_t *spectre_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Spectre";
		me.desc = "Another powerful undead creature: the Spectre is a ghastly apparition, surrounded by "
					"an unearthly green glow. They exist only partially on our plane of existence: "
					"half-corporeal, they can pass through walls, although the density of the wall "
					"will hurt them in the process of doing this. As undead, they have a firm hold "
					"on their life force, see invisible, and resist poison and cold. They also resist "
					"nether. Spectres make superb spellcasters, but their physical form is very weak. "
					"They gain very little nutrition from the food of mortals, but can absorb mana "
					"from staves and wands as their energy source.";

		me.stats[A_STR] = -5;
		me.stats[A_INT] =  4;
		me.stats[A_WIS] =  2;
		me.stats[A_DEX] =  2;
		me.stats[A_CON] = -2;
		me.stats[A_CHR] = -3;
		
		me.skills.dis = 10;
		me.skills.dev = 15;
		me.skills.sav = 12;
		me.skills.stl =  5;
		me.skills.srh =  5;
		me.skills.fos = 14;
		me.skills.thn =-15;
		me.skills.thb = -5;

		me.hd = 7;
		me.exp = 300;
		me.infra = 5;
		me.flags = RACE_IS_NONLIVING | RACE_IS_UNDEAD;

		me.calc_bonuses = _spectre_calc_bonuses;
		me.get_powers = _spectre_get_powers;
		me.get_flags = _spectre_get_flags;
		me.spoiler_dump = _spectre_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Sprite
 ****************************************************************/
static power_info _sprite_powers[] =
{
	{ A_INT, {12, 12, 50, sleeping_dust_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _sprite_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _sprite_powers);
}
static void _sprite_calc_bonuses(void)
{
	p_ptr->levitation = TRUE;
	p_ptr->resist_lite = TRUE;
	p_ptr->pspeed += (p_ptr->lev) / 10;
}
static void _sprite_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_LITE);
	add_flag(flgs, TR_LEVITATION);
	if (p_ptr->lev >= 10)
		add_flag(flgs, TR_SPEED);
}
static void _sprite_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _sprite_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Levitation\n");
	fprintf(fff, "  * Resist Light\n");
	fprintf(fff, "  * +L/10 Speed\n");
}
race_t *sprite_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Sprite";
		me.desc = "One of the several fairy races, Sprites are very small. They have tiny wings and can "
					"fly over traps that may open up beneath them. They enjoy sunlight intensely, and "
					"need worry little about light based attacks. Although physically among the weakest "
					"races, Sprites are very talented in magic, and can become highly skilled wizards. "
					"Sprites have the special power of spraying Sleeping Dust, and at higher levels they "
					"learn to fly faster.";

		me.stats[A_STR] = -4;
		me.stats[A_INT] =  3;
		me.stats[A_WIS] =  3;
		me.stats[A_DEX] =  3;
		me.stats[A_CON] = -2;
		me.stats[A_CHR] = -2;
		
		me.skills.dis = 10;
		me.skills.dev =  8;
		me.skills.sav =  6;
		me.skills.stl =  4;
		me.skills.srh = 10;
		me.skills.fos = 10;
		me.skills.thn =-12;
		me.skills.thb =  0;

		me.hd = 7;
		me.exp = 185;
		me.infra = 4;

		me.calc_bonuses = _sprite_calc_bonuses;
		me.get_powers = _sprite_get_powers;
		me.get_flags = _sprite_get_flags;
		me.spoiler_dump = _sprite_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Tonberry
 ****************************************************************/
static void _tonberry_calc_bonuses(void)
{
	int dam = 15+(p_ptr->lev/10);

	p_ptr->sustain_str = TRUE;
	p_ptr->sustain_con = TRUE;
	p_ptr->resist_fear = TRUE;
	p_ptr->pspeed -= 3;
	p_ptr->to_d_m  += dam;

	if (buki_motteruka(INVEN_LARM) && buki_motteruka(INVEN_RARM))
	{
		p_ptr->weapon_info[0].to_d += (dam + 1) / 2;
		p_ptr->weapon_info[1].to_d += dam/2;
		p_ptr->weapon_info[0].dis_to_d += (dam + 1) / 2;
		p_ptr->weapon_info[1].dis_to_d += dam/2;
	}
	else if (buki_motteruka(INVEN_LARM)) /* Cursed in offhand, shield in leading hand */
	{
		p_ptr->weapon_info[1].to_d += dam;
		p_ptr->weapon_info[1].dis_to_d += dam;
	}
	else
	{
		p_ptr->weapon_info[0].to_d += dam;
		p_ptr->weapon_info[0].dis_to_d += dam;
	}
}
static void _tonberry_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_SUST_STR);
	add_flag(flgs, TR_SUST_CON);
	add_flag(flgs, TR_SPEED);
	add_flag(flgs, TR_RES_FEAR);
}
static void _tonberry_spoiler_dump(FILE *fff) 
{ 
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * +15+L/10 to Damage\n");
	fprintf(fff, "  * -3 to Speed\n");
	fprintf(fff, "  * Sustain Strength\n");
	fprintf(fff, "  * Sustain Constitution\n");
	fprintf(fff, "  * Resist Fear\n");
}
race_t *tonberry_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Tonberry";
		me.desc = "Tonberries are lizard-like creatures who possess enormous strength "
					"and have a preference for cleavers and large knives. They are, "
					"however, sluggish in their movements and reactions; young and "
					"inexperienced tonberries are often preyed on by the other races. "
					"They possess human-like intelligence, but rarely become mages due "
					"to their culture and physiology.";
		
		me.stats[A_STR] =  4;
		me.stats[A_INT] =  0;
		me.stats[A_WIS] = -2;
		me.stats[A_DEX] = -4;
		me.stats[A_CON] =  5;
		me.stats[A_CHR] =  0;
		
		me.skills.dis = -5;
		me.skills.dev =  3;
		me.skills.sav =  3;
		me.skills.stl =  1;
		me.skills.srh = -2;
		me.skills.fos =  5;
		me.skills.thn = 20;
		me.skills.thb =-10;

		me.hd = 13;
		me.exp = 195;
		me.infra = 2;

		me.calc_bonuses = _tonberry_calc_bonuses;
		me.get_flags = _tonberry_get_flags;
		me.spoiler_dump = _tonberry_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Vampire
 ****************************************************************/
static power_info _vampire_powers[] =
{
	{ A_CON, {2, 1, 60, vampirism_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _vampire_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _vampire_powers);
}
static void _vampire_calc_bonuses(void)
{
	p_ptr->resist_dark = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->resist_neth = TRUE;
	p_ptr->resist_cold = TRUE;
	p_ptr->resist_pois = TRUE;
	if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;
}
static void _vampire_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_RES_DARK);
	add_flag(flgs, TR_RES_NETHER);
	if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
	add_flag(flgs, TR_RES_POIS);
	add_flag(flgs, TR_RES_COLD);
}
static void _vampire_get_immunities(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_DARK);
}
static void _vampire_get_vulnerabilities(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_LITE);
}
static void _vampire_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _vampire_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Resist Cold\n");
	fprintf(fff, "  * Resist Poison\n");
	fprintf(fff, "  * Resist Nether\n");
	fprintf(fff, "  * Hold Life\n");
	fprintf(fff, "  * Immunity to Dark\n");
	fprintf(fff, "  * Vulnerability to Light\n");
	fprintf(fff, "  * +1 Light Radius (unless a Ninja)\n");
}
race_t *vampire_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Vampire";
		me.desc = "One of the mightier undead creatures, the Vampire is an awe-inspiring sight. Yet this "
					"dread creature has a serious weakness: the bright rays of sun are its bane, and it "
					"will need to flee the surface to the deep recesses of earth until the sun finally "
					"sets. Darkness, on the other hand, only makes the Vampire stronger. As undead, the "
					"Vampire has a firm hold on its life force, and resists nether attacks. The Vampire "
					"also resists cold and poison based attacks. It is, however, susceptible to its "
					"perpetual hunger for fresh blood, which can only be satiated by sucking the blood "
					"from a nearby monster.";

		me.stats[A_STR] =  3;
		me.stats[A_INT] =  3;
		me.stats[A_WIS] = -1;
		me.stats[A_DEX] = -1;
		me.stats[A_CON] =  1;
		me.stats[A_CHR] =  2;
		
		me.skills.dis = 4;
		me.skills.dev = 8;
		me.skills.sav = 6;
		me.skills.stl = 4;
		me.skills.srh = 1;
		me.skills.fos = 8;
		me.skills.thn = 5;
		me.skills.thb = 0;

		me.hd = 11;
		me.exp = 245;
		me.infra = 5;
		me.flags = RACE_IS_NONLIVING | RACE_IS_UNDEAD;

		me.calc_bonuses = _vampire_calc_bonuses;
		me.get_powers = _vampire_get_powers;
		me.get_flags = _vampire_get_flags;
		me.get_immunities = _vampire_get_immunities;
		me.get_vulnerabilities = _vampire_get_vulnerabilities;
		me.spoiler_dump = _vampire_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Vampire-Lord (cf Polymorph Vampire)
 ****************************************************************/
static power_info _vampire_lord_powers[] =
{
	{ A_CON, {2, 1, 60, vampirism_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _vampire_lord_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _vampire_lord_powers);
}
static void _vampire_lord_calc_bonuses(void)
{
	p_ptr->resist_dark = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->resist_neth = TRUE;
	p_ptr->resist_cold = TRUE;
	p_ptr->resist_pois = TRUE;
	if (p_ptr->pclass != CLASS_NINJA) p_ptr->lite = TRUE;

	p_ptr->see_inv = TRUE;
	p_ptr->pspeed += 3;
	p_ptr->to_a += 10;
	p_ptr->dis_to_a += 10;
}
static void _vampire_lord_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_RES_DARK);
	add_flag(flgs, TR_RES_NETHER);
	if (p_ptr->pclass != CLASS_NINJA) add_flag(flgs, TR_LITE);
	add_flag(flgs, TR_RES_POIS);
	add_flag(flgs, TR_RES_COLD);
	add_flag(flgs, TR_SEE_INVIS);
	add_flag(flgs, TR_SPEED);
}
static void _vampire_lord_get_immunities(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_DARK);
}
static void _vampire_lord_get_vulnerabilities(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_LITE);
}
static void _vampire_lord_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _vampire_lord_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * +3 Speed\n");
	fprintf(fff, "  * +10 Armor Class\n");
	fprintf(fff, "  * Resist Cold\n");
	fprintf(fff, "  * Resist Poison\n");
	fprintf(fff, "  * Resist Nether\n");
	fprintf(fff, "  * Hold Life\n");
	fprintf(fff, "  * Immunity to Dark\n");
	fprintf(fff, "  * Vulnerability to Light\n");
	fprintf(fff, "  * See Invisible\n");
	fprintf(fff, "  * +1 Light Radius (unless a Ninja)\n");
}
race_t *vampire_lord_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Vampire-Lord";
		me.desc = "";

		me.stats[A_STR] =  4;
		me.stats[A_INT] =  4;
		me.stats[A_WIS] =  1;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] =  2;
		me.stats[A_CHR] =  3;
		
		me.skills.dis = 6;
		me.skills.dev = 12;
		me.skills.sav = 8;
		me.skills.stl = 6;
		me.skills.srh = 2;
		me.skills.fos = 12;
		me.skills.thn = 30;
		me.skills.thb = 20;

		me.hd = 11;
		me.exp = 300;
		me.infra = 5;
		me.flags = RACE_IS_NONLIVING | RACE_IS_UNDEAD;

		me.calc_bonuses = _vampire_lord_calc_bonuses;
		me.get_powers = _vampire_lord_get_powers;
		me.get_flags = _vampire_lord_get_flags;
		me.get_immunities = _vampire_lord_get_immunities;
		me.get_vulnerabilities = _vampire_lord_get_vulnerabilities;
		me.spoiler_dump = _vampire_lord_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Yeek
 ****************************************************************/
static power_info _yeek_powers[] =
{
	{ A_WIS, {15, 15, 50, scare_monster_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _yeek_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _yeek_powers);
}
static void _yeek_calc_bonuses(void)
{
	p_ptr->resist_acid = TRUE;
	if (p_ptr->lev >= 20) p_ptr->immune_acid = TRUE;
}
static void _yeek_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_RES_ACID);
}
static void _yeek_get_immunities(u32b flgs[TR_FLAG_SIZE])
{
	if (p_ptr->lev >= 20) 
		add_flag(flgs, TR_RES_ACID);
}
static void _yeek_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _yeek_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Resist Acid\n");
	fprintf(fff, "  * Immunity to Acid at L20\n");
}
race_t *yeek_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Yeek";
		me.desc = "Yeeks are among the most pathetic creatures. Fortunately, their horrible screams "
					"can scare away less confident foes, and their skin becomes more and more resistant "
					"to acid, as they gain experience. But having said that, even a mediocre monster "
					"can wipe the proverbial floor with an unwary Yeek.";

		me.stats[A_STR] = -2;
		me.stats[A_INT] =  1;
		me.stats[A_WIS] = -2;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] = -2;
		me.stats[A_CHR] = -4;
		
		me.skills.dis = 2;
		me.skills.dev = 4;
		me.skills.sav = 6;
		me.skills.stl = 3;
		me.skills.srh = 5;
		me.skills.fos = 15;
		me.skills.thn = -5;
		me.skills.thb = -5;

		me.hd = 7;
		me.exp = 70;
		me.infra = 2;

		me.calc_bonuses = _yeek_calc_bonuses;
		me.get_powers = _yeek_get_powers;
		me.get_flags = _yeek_get_flags;
		me.get_immunities = _yeek_get_immunities;
		me.spoiler_dump = _yeek_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

/****************************************************************
 * Zombie
 ****************************************************************/
static power_info _zombie_powers[] =
{
	{ A_WIS, {30, 30, 70, restore_life_spell}},
	{ -1, {-1, -1, -1, NULL} }
};
static int _zombie_get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _zombie_powers);
}
static void _zombie_calc_bonuses(void)
{
	p_ptr->resist_neth = TRUE;
	p_ptr->hold_life = TRUE;
	p_ptr->see_inv = TRUE;
	p_ptr->resist_pois = TRUE;
	p_ptr->slow_digest = TRUE;
	if (p_ptr->lev >= 5) p_ptr->resist_cold = TRUE;
}
static void _zombie_get_flags(u32b flgs[TR_FLAG_SIZE])
{
	add_flag(flgs, TR_SEE_INVIS);
	add_flag(flgs, TR_HOLD_LIFE);
	add_flag(flgs, TR_RES_NETHER);
	add_flag(flgs, TR_RES_POIS);
	add_flag(flgs, TR_SLOW_DIGEST);
	if (p_ptr->lev >= 5)
		add_flag(flgs, TR_RES_COLD);
}
static void _zombie_spoiler_dump(FILE *fff)
{
	spoil_powers_aux(fff, _zombie_powers);
	fprintf(fff, "\n== Abilities ==\n");
	fprintf(fff, "  * Slow Digestion\n");
	fprintf(fff, "  * See Invisible\n");
	fprintf(fff, "  * Hold Life\n");
	fprintf(fff, "  * Resist Poison\n");
	fprintf(fff, "  * Resist Nether\n");
	fprintf(fff, "  * Resist Cold at L5\n");
}
race_t *zombie_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Zombie";
		me.desc = "Much like Skeletons, Zombies too are undead horrors: they are resistant to life-draining "
					"attacks, and can learn to restore their life-force. Like skeletons, they become "
					"resistant to cold-based attacks (actually earlier than skeletons), resist poison "
					"and can see invisible. While still vulnerable to cuts (unlike skeletons), Zombies "
					"are resistant to Nether. Like Golems, they gain very little nutrition from the food "
					"of mortals, but can absorb mana from staves and wands as their energy source.";

		me.stats[A_STR] =  2;
		me.stats[A_INT] = -6;
		me.stats[A_WIS] = -6;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] =  4;
		me.stats[A_CHR] = -3;
		
		me.skills.dis = -5;
		me.skills.dev = -5;
		me.skills.sav = 5;
		me.skills.stl = -1;
		me.skills.srh = -1;
		me.skills.fos = 5;
		me.skills.thn = 15;
		me.skills.thb = 0;

		me.hd = 13;
		me.exp = 180;
		me.infra = 2;
		me.flags = RACE_IS_NONLIVING | RACE_IS_UNDEAD;

		me.calc_bonuses = _zombie_calc_bonuses;
		me.get_powers = _zombie_get_powers;
		me.get_flags = _zombie_get_flags;
		me.spoiler_dump = _zombie_spoiler_dump;
		init = TRUE;
	}

	return &me;
}

