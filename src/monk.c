#include "angband.h"

static int _max_tries(int lvl, u32b defense)
{
	int tries = 0;
	if (defense & KAMAE_BYAKKO)
		tries = (lvl < 3 ? 1 : lvl / 3);
	else if (defense & KAMAE_SUZAKU)
		tries = 1;
	else if (defense & KAMAE_GENBU)
		tries = 1;
	else
		tries = (lvl < 7 ? 1 : lvl / 7);

	return tries;
}

static int _get_attack_idx(int lvl, u32b defense)
{
	int tries = _max_tries(lvl, defense);
	int i;
	int attack_idx = 0;
	int best_attack_idx = 0;
	int min_level;

	if (p_ptr->stun || p_ptr->confused)
		return 0; /* Punch for 1d4 */

	for (i = 0; i < tries; i++)
	{
		martial_arts *ma_ptr;
		do
		{
			attack_idx = randint0(MAX_MA);
			ma_ptr = &ma_blows[attack_idx];

			if (p_ptr->pclass == CLASS_FORCETRAINER && ma_ptr->min_level > 1) 
				min_level = ma_ptr->min_level + 3;
			else min_level = ma_ptr->min_level;
		}
		while (min_level > lvl || randint1(lvl) < ma_ptr->chance);

		if (ma_ptr->min_level > ma_blows[best_attack_idx].min_level)
			best_attack_idx = attack_idx;
	}

	return best_attack_idx;
}

typedef struct _attack_s {
	int count;
	int mul;
	int to_d;
} _attack_t;

void _get_attack_counts(int tot, _attack_t *counts)
{
	int i;
	_attack_t *_attack_ptr;
	critical_t crit;

	for (i = 0; i < MAX_MA; i++)
	{
		_attack_ptr = &counts[i];
		_attack_ptr->count = 0;
		_attack_ptr->mul = 0;
		_attack_ptr->to_d = 0;
	}

	for (i = 0; i < tot; i++)
	{
		int attack_idx = _get_attack_idx(p_ptr->lev, p_ptr->special_defense);
		martial_arts *ma_ptr = &ma_blows[attack_idx];

		_attack_ptr = &counts[attack_idx];
		_attack_ptr->count++;
		
		/* Crits depend on the attack chosen. The following won't be stable
		   for attacks that occur infrequently, but hopefully things will just
		   average out */
		crit = monk_get_critical(ma_ptr);

		if (crit.desc)
		{
			_attack_ptr->mul += crit.mul;
			_attack_ptr->to_d += crit.to_d;
		}
		else
			_attack_ptr->mul += 100;
	}
}

static int _get_weight(void)
{
	int weight = 8;
	if (p_ptr->special_defense & KAMAE_SUZAKU) weight = 4;
	if ((p_ptr->pclass == CLASS_FORCETRAINER) && (p_ptr->magic_num1[0]))
	{
		weight += (p_ptr->magic_num1[0]/30);
		if (weight > 20) weight = 20;
	}
	return weight * p_ptr->lev;
}

critical_t monk_get_critical(martial_arts *ma_ptr)
{
	int min_level = ma_ptr->min_level;
	int weight = _get_weight();
	
	if (p_ptr->pclass == CLASS_FORCETRAINER) min_level = MAX(1, min_level - 3);

	return critical_norm(_get_weight(), min_level, p_ptr->weapon_info[0].to_h, 0);
}

int monk_get_attack_idx(void)
{
	return _get_attack_idx(p_ptr->lev, p_ptr->special_defense);
}

void monk_display_attack_info(int row, int col)
{
	_attack_t counts[MAX_MA];
	int i;
	const int tot = 10 * 1000;
	char buf[128];
	int tot_dam = 0;
	int tot_mul = 0;
	int tot_to_d = 0;
	int blows = p_ptr->weapon_info[0].num_blow;
	int to_d = p_ptr->weapon_info[0].to_d * 100;
	int r = row, c = col;
	critical_t crit;

	sprintf(buf, "%-15s %6s %6s %7s", "Attack", "Dice", "Pct", "Dam");
	c_put_str(TERM_YELLOW, buf, r++, c);

	_get_attack_counts(tot, counts);
	for (i = 0; i < MAX_MA; i++)
	{
		martial_arts *ma_ptr = &ma_blows[i];
		int dd = ma_ptr->dd + p_ptr->weapon_info[0].to_dd;
		int ds = ma_ptr->ds + p_ptr->weapon_info[0].to_ds;
		char tmp[20];
		int dam = dd * (ds + 1) * 100 * counts[i].count / (2 * tot);

		tot_dam += dam;
		tot_mul += counts[i].mul;
		tot_to_d += counts[i].to_d;

		sprintf(tmp, "%dd%d", dd, ds);
		sprintf(buf, "%-15s %6s %3d.%02d%% %3d.%02d", ma_ptr->name, tmp, counts[i].count/100, counts[i].count%100, dam/100, dam%100);
		put_str(buf, r++, c);
	}

	sprintf(buf, "%20s %3d.%02d  +%3d", "Total:", tot_dam/100, tot_dam%100, to_d/100);
	put_str(buf, r++, c + 10);

	crit.mul = tot_mul/tot;
	crit.to_d = tot_to_d*100/tot;
	sprintf(buf, "%20s %3d.%02dx +%3d.%02d", "Criticals:", crit.mul/100, crit.mul%100, crit.to_d/100, crit.to_d%100);
	put_str(buf, r++, c + 10);

	/* Account for criticals in all that follows ... */
	tot_dam = tot_dam * crit.mul/100;
	to_d = to_d + crit.to_d;
	sprintf(buf, "%20s %3d.%02d  +%3d.%02d", "One Strike:", tot_dam/100, tot_dam%100, to_d/100, to_d%100);
	put_str(buf, r++, c + 10);

	r = row;
	c = col + 40;

	c_put_str(TERM_YELLOW, "Your Fists", r++, c);

	sprintf(buf, "Number of Blows: %d", blows);
	put_str(buf, r++, c);

	sprintf(buf, "To Hit:  0  50 100 150 200 (AC)");
	put_str(buf, r++, c);

	sprintf(buf, "        %2d  %2d  %2d  %2d  %2d (%%)", 
		hit_chance(0, 0, 0), 
		hit_chance(0, 0, 50), 
		hit_chance(0, 0, 100), 
		hit_chance(0, 0, 150), 
		hit_chance(0, 0, 200)
	);

	put_str(buf, r++, c);

	r++;
	c_put_str(TERM_YELLOW, "Average Damage:", r++, c);
	sprintf(buf, "One Strike: %d.%02d", (tot_dam + to_d)/100, (tot_dam + to_d)%100);
	put_str(buf, r++, c+1);
	sprintf(buf, "One Attack: %d.%02d", blows*(tot_dam + to_d)/100, (blows*(tot_dam + to_d))%100);
	put_str(buf, r++, c+1);

	if (p_ptr->weapon_info[0].brand_acid || (p_ptr->special_attack & ATTACK_ACID))
	{
		sprintf(buf, " %d.%02d", blows*(tot_dam*17/10 + to_d)/100, (blows*(tot_dam*17/10 + to_d))%100);
		c_put_str(TERM_RED, "      Acid:", r, c+1);
		put_str(buf, r++, c+12);
	}
	if (p_ptr->weapon_info[0].brand_elec || (p_ptr->special_attack & ATTACK_ELEC))
	{
		sprintf(buf, " %d.%02d", blows*(tot_dam*17/10 + to_d)/100, (blows*(tot_dam*17/10 + to_d))%100);
		c_put_str(TERM_RED, "      Elec:", r, c+1);
		put_str(buf, r++, c+12);
	}
	if (p_ptr->weapon_info[0].brand_fire || (p_ptr->special_attack & ATTACK_FIRE))
	{
		sprintf(buf, " %d.%02d", blows*(tot_dam*17/10 + to_d)/100, (blows*(tot_dam*17/10 + to_d))%100);
		c_put_str(TERM_RED, "      Fire:", r, c+1);
		put_str(buf, r++, c+12);
	}
	if (p_ptr->weapon_info[0].brand_cold || (p_ptr->special_attack & ATTACK_COLD))
	{
		sprintf(buf, " %d.%02d", blows*(tot_dam*17/10 + to_d)/100, (blows*(tot_dam*17/10 + to_d))%100);
		c_put_str(TERM_RED, "      Cold:", r, c+1);
		put_str(buf, r++, c+12);
	}
	if (p_ptr->weapon_info[0].brand_pois || (p_ptr->special_attack & ATTACK_POIS))
	{
		sprintf(buf, " %d.%02d", blows*(tot_dam*17/10 + to_d)/100, (blows*(tot_dam*17/10 + to_d))%100);
		c_put_str(TERM_RED, "      Pois:", r, c+1);
		put_str(buf, r++, c+12);
	}
}

static bool _monk_check_spell(void)
{
	if (p_ptr->pclass != CLASS_WILD_TALENT && !(empty_hands(TRUE) & EMPTY_HAND_RARM))
	{
		msg_print(T("You need to be bare hand.", "素手じゃないとできません。"));
		return FALSE;
	}
	if (p_ptr->riding)
	{
		msg_print(T("You need to get off a pet.", "乗馬中はできません。"));
		return FALSE;
	}
	return TRUE;
}

static bool choose_kamae(void)
{
	char choice;
	int new_kamae = 0;
	int i;
	char buf[80];

	if (p_ptr->confused)
	{
#ifdef JP
		msg_print("混乱していて構えられない！");
#else
		msg_print("Too confused.");
#endif
		return FALSE;
	}

	/* Save screen */
	screen_save();

#ifdef JP
	prt(" a) 構えをとく", 2, 20);
#else
	prt(" a) No form", 2, 20);
#endif

	for (i = 0; i < MAX_KAMAE; i++)
	{
		if (p_ptr->lev >= kamae_shurui[i].min_level)
		{
			sprintf(buf," %c) %-12s  %s",I2A(i+1), kamae_shurui[i].desc, kamae_shurui[i].info);
			prt(buf, 3+i, 20);
		}
	}

	prt("", 1, 0);
#ifdef JP
	prt("        どの構えをとりますか？", 1, 14);
#else
	prt("        Choose Form: ", 1, 14);
#endif

	while(1)
	{
		choice = inkey();

		if (choice == ESCAPE)
		{
			screen_load();
			return FALSE;
		}
		else if ((choice == 'a') || (choice == 'A'))
		{
			if (p_ptr->action == ACTION_KAMAE)
			{
				set_action(ACTION_NONE);
			}
			else
#ifdef JP
				msg_print("もともと構えていない。");
#else
				msg_print("You are not assuming a posture.");
#endif
			screen_load();
			return TRUE;
		}
		else if ((choice == 'b') || (choice == 'B'))
		{
			new_kamae = 0;
			break;
		}
		else if (((choice == 'c') || (choice == 'C')) && (p_ptr->lev > 29))
		{
			new_kamae = 1;
			break;
		}
		else if (((choice == 'd') || (choice == 'D')) && (p_ptr->lev > 34))
		{
			new_kamae = 2;
			break;
		}
		else if (((choice == 'e') || (choice == 'E')) && (p_ptr->lev > 39))
		{
			new_kamae = 3;
			break;
		}
	}
	set_action(ACTION_KAMAE);

	if (p_ptr->special_defense & (KAMAE_GENBU << new_kamae))
	{
#ifdef JP
		msg_print("構え直した。");
#else
		msg_print("You reassume a posture.");
#endif
	}
	else
	{
		p_ptr->special_defense &= ~(KAMAE_MASK);
		p_ptr->update |= (PU_BONUS);
		p_ptr->redraw |= (PR_STATE);
#ifdef JP
		msg_format("%sの構えをとった。",kamae_shurui[new_kamae].desc);
#else
		msg_format("You assume a posture of %s form.",kamae_shurui[new_kamae].desc);
#endif
		p_ptr->special_defense |= (KAMAE_GENBU << new_kamae);
	}
	p_ptr->redraw |= PR_STATE;
	screen_load();
	return TRUE;
}

void monk_double_attack_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Double Attack", "百裂拳"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (_monk_check_spell())
		{
			int x, y, dir = 0;

			if (!get_rep_dir(&dir, FALSE)) return;

			y = py + ddy[dir];
			x = px + ddx[dir];
			if (cave[y][x].m_idx)
			{
				if (one_in_(2)) msg_print(T("Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!",
					                        "あーたたたたたたたたたたたたたたたたたたたたたた！！！" ));
				else msg_print(T("Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!",
					                "オラオラオラオラオラオラオラオラオラオラオラオラ！！！"));

				py_attack(y, x, 0);
				if (cave[y][x].m_idx)
				{
					handle_stuff();
					py_attack(y, x, 0);
				}
			}
			else
			{
				msg_print(T("You don't see any monster in this direction", "その方向にはモンスターはいません。"));
				msg_print(NULL);
			}
			var_set_bool(res, TRUE);
		}
		break;
	case SPELL_ENERGY:
		var_set_int(res, 100 + ENERGY_NEED());
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void monk_posture_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Assume a Posture", "構える"));
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if ( _monk_check_spell()
		  && choose_kamae() )
		{
			p_ptr->update |= (PU_BONUS);
			var_set_bool(res, TRUE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
