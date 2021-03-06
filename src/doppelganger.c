#include "angband.h"

void mimic_race(int new_race)
{
	int  old_race = p_ptr->mimic_form;

	if (p_ptr->prace != RACE_DOPPELGANGER) return;
	if (p_ptr->tim_mimic) return;
	if (new_race == old_race) return;
	
	if (old_race == RACE_HUMAN || old_race == RACE_DEMIGOD)
	{
		int i, idx;
		for (i = 0; i < MAX_DEMIGOD_POWERS; i++)
		{
			idx = p_ptr->demigod_power[i];
			if (idx >= 0)
			{
				mut_unlock(idx);
				mut_lose(idx);
			/*	Lose the mutation, but not the choice!
				p_ptr->demigod_power[i] = -1; */
			}
		}
	}

	/* Shifting form causes mutations to vanish! */
	mut_lose_all();

	if (new_race == MIMIC_NONE)
		msg_print("You resume your true form.");
	else
	{
		race_t *race_ptr = get_race_t_aux(new_race, 0);
		if (is_a_vowel(race_ptr->name[0]))
			msg_format("You turn into an %s!", race_ptr->name);
		else
			msg_format("You turn into a %s!", race_ptr->name);
	}

	p_ptr->mimic_form = new_race;
	p_ptr->expfact = calc_exp_factor();
	check_experience();

	if (new_race == RACE_HUMAN || new_race == RACE_DEMIGOD)
	{
		get_race_t()->gain_level(p_ptr->lev);	/* This is OK ... Just make sure we get to choose racial powers on mimicry */
	}

	if (new_race == RACE_BEASTMAN)
	{
		int i;
		mut_gain_random(mut_good_pred);
		for (i = 2; i <= p_ptr->lev; i++)
		{
			if (one_in_(5))
				mut_gain_random(NULL);
		}
	}

	p_ptr->redraw |= (PR_BASIC | PR_STATUS);
	p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA);

	handle_stuff();
}

typedef struct {
	int level;
	int fail;
	int race;
} _form_t;

static int _form_cost(int exp)
{
	return exp / 5;
}

static int _form_upkeep(int exp)
{
	int result = 0;
	if (exp >= 175)
		result += 1 + (exp - 175)/25;
	return result;
}

static void _pay_cost(int cost)
{
	if (p_ptr->csp < cost)
	{
		int dam = cost - p_ptr->csp;
		p_ptr->csp = 0;
		take_hit(DAMAGE_USELIFE, dam, T("concentrating too hard", "過度の集中"), -1);
	}
	else 
		p_ptr->csp -= cost;

	p_ptr->redraw |= (PR_MANA | PR_HP);
}

void mimic_upkeep(void)
{
	int cost;

	if (p_ptr->prace != RACE_DOPPELGANGER) return;
	if (p_ptr->tim_mimic) return;
	if (p_ptr->mimic_form == MIMIC_NONE) return;

	cost = _form_upkeep(get_race_t()->exp);
	if (cost > p_ptr->csp + p_ptr->chp)
		mimic_race(MIMIC_NONE);
	else
		_pay_cost(cost);
}

bool mimic_no_regen(void)
{
	if (p_ptr->prace == RACE_DOPPELGANGER && !p_ptr->tim_mimic && p_ptr->mimic_form != MIMIC_NONE) return TRUE;
	return FALSE;
}

static _form_t _forms[] = 
{
	{  1, 30, RACE_SNOTLING }, 
	{  3, 30, RACE_YEEK }, 
	{  5, 30, RACE_KOBOLD }, 
	{  7, 40, RACE_IMP }, 
	{  8, 40, RACE_HUMAN }, 
	{  9, 40, RACE_SKELETON }, 
	{ 10, 40, RACE_GNOME }, 
	{ 11, 40, RACE_ENT }, 
	{ 12, 40, RACE_DWARF }, 
	{ 13, 40, RACE_SHADOW_FAIRY }, 
	{ 14, 40, RACE_MIND_FLAYER }, 
	{ 15, 40, RACE_BARBARIAN }, 
	{ 16, 40, RACE_HALF_TROLL }, 
	{ 17, 50, RACE_BEASTMAN }, 
	{ 18, 50, RACE_HOBBIT }, 
	{ 19, 50, RACE_CYCLOPS }, 
	{ 20, 50, RACE_HALF_OGRE }, 
	{ 21, 50, RACE_HALF_GIANT }, 
	{ 22, 50, RACE_NIBELUNG }, 
	{ 23, 50, RACE_DARK_ELF }, 
	{ 24, 50, RACE_ZOMBIE }, 
	{ 25, 50, RACE_GOLEM }, 
	{ 26, 50, RACE_SPRITE }, 
	{ 27, 60, RACE_DRACONIAN }, 
	{ 28, 60, RACE_TONBERRY }, 
	{ 29, 60, RACE_BALROG }, 
	{ 30, 60, RACE_KUTAR }, 
	{ 31, 60, RACE_DUNADAN }, 
	{ 32, 70, RACE_KLACKON }, 
	{ 33, 70, RACE_HIGH_ELF }, 
	{ 34, 70, RACE_AMBERITE }, 
	{ 36, 75, RACE_DEMIGOD }, 
	{ 38, 75, RACE_ARCHON }, 
	{ 40, 75, RACE_VAMPIRE }, 
	{ 42, 80, RACE_HALF_TITAN }, 
	{ 45, 80, RACE_SPECTRE }, 
	{  0,  0, MIMIC_NONE }
};

/* Yucky menu code modified from spells.c ... But I didn't want to create
   a new spell object for each racial form since all we really need is the
   level and the race's integer identifier.
*/
static int _col_height(int ct)
{
	int  w, h;
	int result = ct;

	Term_get_size(&w, &h);

	h -= 9; /* Room for browsing */
	if (result > h)
	{
		result = (ct + 1)/2;
	}

	return result;
}

static void _list_forms(int ct)
{
	char temp[140];
	int  i;
	int  y = 1;
	int  x = 10;
	int  col_height = _col_height(ct);
	int  col_width;

	Term_erase(x, y, 255);

	if (col_height == ct)
	{
		Term_erase(x, y, 255);
		put_str("Lv Cost Fail", y, x + 29);
	}
	else
	{
		col_width = 42;
		x = 1;
		Term_erase(x, y, 255);
		put_str("Lv Cost Fail", y, x + 29);
		put_str("Lv Cost Fail", y, x + col_width + 29);
	}

	for (i = 0; i < ct; i++)
	{
		char   letter = '\0';
		byte   attr = TERM_WHITE;
		int    level = _forms[i].level;
		int    race_idx = _forms[i].race;
		race_t *race_ptr = get_race_t_aux(race_idx, 0);
		int    cost = _form_cost(race_ptr->exp);
		int    fail = calculate_fail_rate(level, _forms[i].fail, p_ptr->stat_ind[A_DEX]);

		if (i < 26)
			letter = I2A(i);
		else if (i < 52)
			letter = 'A' + i - 26;
		else
			letter = '0' + i - 52;

		sprintf(temp, "  %c) ", letter);

		strcat(temp, format("%-23.23s %2d %4d %3d%%", 
							race_ptr->name,
							level,
							cost,
							fail));

		if (fail == 100)
			attr = TERM_L_DARK;

		if (i < col_height)
		{
			c_prt(attr, temp, y + i + 1, x);
		}
		else
		{
			c_prt(attr, temp, y + (i - col_height) + 1, (x + col_width));
		}
	}
}

static void _describe_form(int idx, int col_height)
{
	char tmp[80*9];
	int i, line;
	int    race_idx = _forms[idx].race;
	race_t *race_ptr = get_race_t_aux(race_idx, 0);

	/* 2 lines below list of spells, X lines for description */
	for (i = 0; i < 11; i++)
		Term_erase(12, col_height + i + 2, 255);

	/* Get the description, and line break it (max X lines) */
	roff_to_buf(race_ptr->desc, 80, tmp, sizeof(tmp));

	for(i = 0, line = col_height + 3; tmp[i]; i += 1+strlen(&tmp[i]))
	{
		prt(&tmp[i], line, 15);
		line++;
	}
}

static int _choose_form_imp(int ct)
{
	int choice = -1;
	char prompt1[140];
	char prompt2[140];
	bool describe = FALSE;

	strnfmt(prompt1, 78, "Mimic which %s? (Type '?' to Browse) ", "Race");
	strnfmt(prompt2, 78, "Browse which %s? (Type '?' to Use)", "Race");
	_list_forms(ct);

	for (;;)
	{
		char ch = '\0';

		/* Prompt User */
		choice = -1;
		if (!get_com(describe ? prompt2 : prompt1, &ch, FALSE)) break;

		if (ch == '?')
		{
			describe = !describe;
			if (!get_com(describe ? prompt2 : prompt1, &ch, FALSE)) break;
		}

		if (isupper(ch))
			choice = ch - 'A' + 26;
		else if (islower(ch))
			choice = ch - 'a';
		else if (ch >= '0' && ch <= '9')
			choice = ch - '0' + 52;

		/* Valid Choice? */
		if (choice < 0 || choice >= ct)
		{
			bell();
			continue;
		}

		if (describe)
		{
			_describe_form(choice, _col_height(ct));
			continue;
		}

		/* Good to go! */
		break;
	}
	
	return choice;
}

static int _choose_form(void)
{
	int choice = -1;
	int ct = 0;
	int i;

	for (i = 0; ; i++)
	{
		if (_forms[i].race == MIMIC_NONE) break;
		if (_forms[i].level > p_ptr->lev) break;
		ct++;
	}

	if (ct == 0) return -1;

	if (REPEAT_PULL(&choice))
	{
		if (choice >= 0 && choice < ct)
			return choice;
	}

	screen_save();
	choice = _choose_form_imp(ct);
	REPEAT_PUSH(choice);
	screen_load();

	return choice;
}

static void _mimic_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		if (p_ptr->mimic_form != MIMIC_NONE)
			var_set_string(res, "Stop Mimicry");
		else
			var_set_string(res, "Mimic");
		break;
	case SPELL_SPOIL_NAME:
		var_set_string(res, "Mimic");
		break;
	case SPELL_DESC:
		var_set_string(res, "You lose the benefits of your current race to take on a new form.");
		break;
	case SPELL_CAST:
		var_set_bool(res, TRUE);
		/* Drawback: Casting polymorph magic will block racial shape shifting until the spell wears off */
		if (p_ptr->tim_mimic)
		{
			msg_print("Something seems to be interfering with your racial shape shifting!");
		}
		/* Drawback: Before shifting to a new form, they must revert to their original form. */
		else if (p_ptr->mimic_form != MIMIC_NONE)
		{
			mimic_race(MIMIC_NONE);
		}
		else
		{
			int i = _choose_form();
			if (i != -1)
			{
			int    level = _forms[i].level;
			int    race_idx = _forms[i].race;
			race_t *race_ptr = get_race_t_aux(race_idx, 0);
			int    cost = _form_cost(race_ptr->exp);
			int    fail = calculate_fail_rate(level, _forms[i].fail, p_ptr->stat_ind[A_DEX]);

				if (cost > p_ptr->csp + p_ptr->chp)
				{
					msg_print("Choosing this form will kill you. You need to rest first!");
					return;
				}

				if (randint0(100) >= fail)
					mimic_race(race_idx);
				else
					msg_print("You failed to concentrate hard enough!");

				_pay_cost(cost);
			}
			else
			{
				/* No charge if user cancels the menu */
				var_set_bool(res, FALSE);
			}
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static power_info _powers[] = 
{
	{A_DEX, {1, 0, 0, _mimic_spell}},
	{-1, {-1, -1, -1, NULL}}
};

static int _get_powers(spell_info* spells, int max)
{
	return get_powers_aux(spells, max, _powers);
}

static void _spoiler_dump(FILE *fff) 
{ 
	int i;

	spoil_powers_aux(fff, _powers);

	fprintf(fff, "\n== Mimic Forms ==\n");
	fprintf(fff, "||  || *Lvl* || *Mana* || *Fail* || *Upkeep* ||\n");
	for (i = 0; ; i++)
	{
		int    level = _forms[i].level;
		int    race_idx = _forms[i].race;
		race_t *race_ptr = NULL;
		int    cost;
		int    upkeep;
		int    fail;
		cptr   spoiler;

		if (race_idx == MIMIC_NONE) break;

		spoiler = race_spoiler_page(race_idx);
		race_ptr = get_race_t_aux(race_idx, 0);
		cost = _form_cost(race_ptr->exp);
		upkeep = _form_upkeep(race_ptr->exp);
		fail = _forms[i].fail;

		if (spoiler)
		{
			fprintf(fff, "||[%s %s]||%d||%d||%d||%d||\n", 
				spoiler,
				race_ptr->name, 
				level,
				cost, 
				fail,
				upkeep
			);
		}
		else
		{
			fprintf(fff, "||%s||%d||%d||%d||%d||\n", 
				race_ptr->name, 
				level,
				cost, 
				fail,
				upkeep
			);
		}
	}

	fprintf(fff, "\n\n");
	fprintf(fff, "  * Mimic fail rates are determined by Dexterity.\n");
	fprintf(fff, "  * While mimicking, a player will not regenerate.\n");
	fprintf(fff, "  * If the player rests, they will revert to their true form.\n");
	fprintf(fff, "  * To assume a new form, the player must first revert to their true form.\n");
	fprintf(fff, "  * The player loses all mutations each time they shape shift.\n");
	fprintf(fff, "  * When mimicking a Beastman, the player will gain random mutations as if they "
						"leveled up from 1 to their current level as a Beastman\n");
	fprintf(fff, "  * When mimicking a Human or Demigod, the player may choose [SpecialAbilities abilities] "
						"if appropriate. However, once this choice is made, it is remembered, and the same "
						"choice will be used for all future transformations to this race.\n");
}

race_t *doppelganger_get_race_t(void)
{
	static race_t me = {0};
	static bool init = FALSE;

	if (!init)
	{
		me.name = "Doppelganger";
		me.desc = "Doppelgangers a truly pathetic creatures. While nobody has actually ever seen "
					"their true form, they are rumored to be small, plain, pathetic creatures. "
					"Their one saving grace is the ability to mimic other races.";
		
		me.stats[A_STR] = -3;
		me.stats[A_INT] = -3;
		me.stats[A_WIS] = -3;
		me.stats[A_DEX] = -3;
		me.stats[A_CON] = -3;
		me.stats[A_CHR] = -3;
		
		me.skills.dis = -20;
		me.skills.dev = -20;
		me.skills.sav = -20;
		me.skills.stl = 5;
		me.skills.srh = -10;
		me.skills.fos = 10;
		me.skills.thn = -20;
		me.skills.thb = -20;

		me.hd = 6;
		me.exp = 150;
		me.infra = 0;

		me.get_powers = _get_powers;
		me.spoiler_dump = _spoiler_dump;
		init = TRUE;
	}

	return &me;
}

