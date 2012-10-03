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

void _get_attack_counts(int lvl, u32b defense, int tot, int *counts)
{
	int i;

	for (i = 0; i < MAX_MA; i++)
		counts[i] = 0;

	for (i = 0; i < tot; i++)
		counts[_get_attack_idx(lvl, defense)]++;
}

void monk_get_attack_counts(int tot, int *counts)
{
	_get_attack_counts(p_ptr->lev, p_ptr->special_defense, tot, counts);
}

void monk_dump_avg_dam(FILE *fff)
{
	int lvl;
	const int tot = 10*1000;

	fprintf(fff, "int monk_ave_damage[PY_MAX_LEVEL+1][3] =\n");
	fprintf(fff, "{\n");
	fprintf(fff, "  {0, 0, 0},\n");

	for (lvl = 1; lvl <= 50; lvl++)
	{
		int normal = 0, strong = 0, weak = 0;
		int normal_counts[MAX_MA], strong_counts[MAX_MA], weak_counts[MAX_MA];
		int i;

		_get_attack_counts(lvl, 0, tot, normal_counts);
		_get_attack_counts(lvl, KAMAE_BYAKKO, tot, strong_counts);
		_get_attack_counts(lvl, KAMAE_GENBU, tot, weak_counts);

		for (i = 0; i < MAX_MA; i++)
		{
			martial_arts *ma_ptr = &ma_blows[i];
			int           dam = ma_ptr->dd * (ma_ptr->ds + 1) * 100 / 2;

			normal += dam * normal_counts[i] / tot;
			strong += dam * strong_counts[i] / tot;
			weak   += dam * weak_counts[i]   / tot;
		}

		fprintf(fff, "  {%d, %d, %d},  /* L%d */\n", normal, strong, weak, lvl);
	}

	fprintf(fff, "};\n");
}


int monk_get_attack_idx(void)
{
	return _get_attack_idx(p_ptr->lev, p_ptr->special_defense);
}

void monk_display_attack_info(int row, int col)
{
	int counts[MAX_MA];
	int i;
	const int tot = 10 * 1000;
	char buf[128];
	int tot_dam = 0;
	int blows = p_ptr->weapon_info[0].num_blow;
	int to_d = p_ptr->weapon_info[0].to_d * 100;
	int r = row, c = col;

	sprintf(buf, "%-15s %6s %6s %6s", "Attack", "Dice", "Pct", "Dam");
	c_put_str(TERM_YELLOW, buf, r++, c);

	monk_get_attack_counts(tot, counts);
	for (i = 0; i < MAX_MA; i++)
	{
		martial_arts *ma_ptr = &ma_blows[i];
		int dd = ma_ptr->dd + p_ptr->weapon_info[0].to_dd;
		int ds = ma_ptr->ds + p_ptr->weapon_info[0].to_ds;
		char tmp[20];
		int dam = dd * (ds + 1) * 100 * counts[i] / (2 * tot);

		tot_dam += dam;

		sprintf(tmp, "%dd%d", dd, ds);
		sprintf(buf, "%-15s %6s %3d.%02d%% %2d.%02d", ma_ptr->name, tmp, counts[i]/100, counts[i]%100, dam/100, dam%100);
		put_str(buf, r++, c);
	}

	sprintf(buf, "%20s %2d.%02d", "Total:", tot_dam/100, tot_dam%100);
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
