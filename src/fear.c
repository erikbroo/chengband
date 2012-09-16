#include "angband.h"

int _get_level(int amount)
{
	if (amount >= FEAR_PETRIFIED)
		return FEAR_PETRIFIED;
	if (amount >= FEAR_TERRIFIED)
		return FEAR_TERRIFIED;
	if (amount >= FEAR_SCARED)
		return FEAR_SCARED;
	if (amount >= FEAR_NERVOUS)
		return FEAR_NERVOUS;
	if (amount >= FEAR_UNEASY)
		return FEAR_UNEASY;

	return FEAR_BOLD;
}

int  fear_level_p(void)
{
	return _get_level(p_ptr->afraid);
}

void fear_clear_p(void)
{
	fear_set_p(0);
}

void fear_decrease_p(void)
{
	int lvl = fear_level_p();
	switch (lvl)
	{
	case FEAR_UNEASY:
		fear_set_p(FEAR_BOLD);
		break;

	case FEAR_NERVOUS:
		fear_set_p(FEAR_UNEASY);
		break;

	case FEAR_SCARED:
		fear_set_p(FEAR_NERVOUS);
		break;

	case FEAR_TERRIFIED:
		fear_set_p(FEAR_SCARED);
		break;

	case FEAR_PETRIFIED:
		fear_set_p(FEAR_TERRIFIED);
		break;
	}
}

bool fear_add_p(int amount)
{
	return fear_set_p(p_ptr->afraid + amount);
}

static cptr _get_level_name(int v)
{
	int lvl = _get_level(v);
	switch (lvl)
	{
	case FEAR_UNEASY: return "uneasy";
	case FEAR_NERVOUS: return "nervous";
	case FEAR_SCARED: return "scared";
	case FEAR_TERRIFIED: return "terrified";
	case FEAR_PETRIFIED: return "petrified";
	}

	return "bold";
}

bool fear_set_p(int v)
{
	int old_lvl, new_lvl;
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->is_dead) return FALSE;

	old_lvl = _get_level(p_ptr->afraid);
	new_lvl = _get_level(v);

	if (new_lvl > old_lvl)
	{
		msg_format("You feel %s.", _get_level_name(new_lvl));
		if (p_ptr->special_defense & KATA_MASK)
		{
			msg_print(T("Your posture gets loose.", "�������줿��"));
			p_ptr->special_defense &= ~KATA_MASK;
			p_ptr->update |= PU_BONUS;
			p_ptr->update |= PU_MONSTERS;
			p_ptr->redraw |= PR_STATE;
			p_ptr->redraw |= PR_STATUS;
			p_ptr->action = ACTION_NONE;
		}
		notice = TRUE;
		p_ptr->counter = FALSE;
		chg_virtue(V_VALOUR, -1);
	}
	else if (new_lvl < old_lvl)
	{
		if (new_lvl == FEAR_BOLD)
			msg_print("Your fears finally subside.");
		else
		{
			msg_format("You are no longer %s, but you still feel %s.", 
						_get_level_name(old_lvl), _get_level_name(new_lvl));
		}
		notice = TRUE;
	}

	p_ptr->afraid = v;
	p_ptr->redraw |= PR_STATUS;
	if (!notice) return FALSE;
	if (disturb_state) disturb(0, 0);
	handle_stuff();
	return TRUE;
}

/* Odds that 1dL1 <= 1dL2 */
static double _save_odds(int l1, int l2)
{
	double w1 = 1.0/(double)l1;
	double t = 0.0;
	double p = 0.0;
	int i, j;

	/* Consider each possibility on a 1dL1 roll */
	for (i = 1; i <= l1; i++)
	{
		/* To save, 1dL2 must roll >= i. How many ways? */
		j = l2 - (i - 1);

		/* Stop when all hope is lost */
		if (j <= 0) break;

		/* The odds of this are: */
		p = (double)j/(double)l2;

		/* Weight this event */
		t += p*w1;
	}
	return t;
}

/* Effective monster level for fear calculations */
static int _r_level(monster_race *r_ptr)
{
int ml = r_ptr->level;
	
	if (r_ptr->flags2 & RF2_POWERFUL)
		ml += 10;

	if (r_ptr->flags1 & RF1_UNIQUE)
		ml += 5;

	return ml;
}

/* Effective threat level for player recovery */
int fear_threat_level(void)
{
	int dl = MIN(dun_level + 2, 127);
	int ml = 0;
	int i;

	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		if (!m_ptr->r_idx) continue;
		if (!m_ptr->ml) continue;
		if (!(r_ptr->flags2 & RF2_AURA_FEAR)) continue;
		if (!projectable(py, px, m_ptr->fy, m_ptr->fx)) continue;
		
		ml = MAX(ml, _r_level(r_ptr)/MAX(1, m_ptr->cdis - 2));
	}

	return MAX(dl, ml);
}

/* Permissable Player Actions */
bool fear_allow_device(void)
{
	if (p_ptr->afraid && !fear_save_p(p_ptr->afraid)) return FALSE;
	return TRUE;
}

bool fear_allow_magic(void)
{
	if (p_ptr->afraid && !fear_save_p(p_ptr->afraid/2)) return FALSE;
	return TRUE;
}

bool fear_allow_melee(void)
{
	if (p_ptr->afraid && !fear_save_p(5*p_ptr->afraid)) return FALSE;
	return TRUE;
}

bool fear_allow_shoot(void)
{
	if (p_ptr->afraid && !fear_save_p(3*p_ptr->afraid)) return FALSE;
	return TRUE;
}

/* Fear Saving Throws */
bool fear_save_p(int ml)
{
	bool result = FALSE;
	int pl;

	if (ml <= 1) return TRUE;

	/* Immunity to Fear? */
	if (!ml) return TRUE;
	if (p_ptr->pclass == CLASS_BERSERKER) return TRUE;
	if (inventory[INVEN_HEAD].name1 == ART_ARES) return TRUE;

	pl = p_ptr->lev + adj_stat_save[p_ptr->stat_ind[A_CHR]];
	if (prace_is_(RACE_SNOTLING)) ml *= 2;

	if (randint1(ml) <= randint1(pl)) result = TRUE;
	else if (p_ptr->resist_fear && randint1(ml) <= randint1(pl)) result = TRUE;

#ifndef _DEBUG
	if (p_ptr->wizard)
#endif
	{
		double s = _save_odds(ml, pl);

		if (p_ptr->resist_fear)
		{
			double f = 1.0 - s;
			double s2 = 1.0 - (f*f);
			msg_format("Fear: 1d%d <= 1d%d => %.2lf%%, %.2lf%% %s", ml, pl, s*100.0, s2*100.0, result?"Y":"N");
		}
		else
			msg_format("Fear: 1d%d <= 1d%d => %.2lf%% %s", ml, pl, s*100.0, result?"Y":"N");
	}

	return result;
}

bool fear_save_m(monster_type *m_ptr)
{
	int           pl = p_ptr->lev;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int           ml = _r_level(r_ptr);
	bool          result = FALSE;
	
	/* Player may not exert their force of will out of sight! */
	if (projectable(py, px, m_ptr->fy, m_ptr->fx))
		pl += adj_stat_save[p_ptr->stat_ind[A_CHR]];

	if (randint1(pl) <= randint1(ml)) result = TRUE;

#ifndef _DEBUG
	if (p_ptr->wizard)
#endif
	{
		double s = _save_odds(pl, ml);
		msg_format("MonFear: 1d%d <= 1d%d => %.2lf%% %s", pl, ml, s*100.0, result?"Y":"N");
	}

	return result;
}

/* Recovery from Fear */
void fear_recover_p(void)
{
	if (p_ptr->afraid)
	{
		int threat = fear_threat_level();
		if (fear_save_p(threat + p_ptr->afraid / 10))
		{
			if (fear_save_p(threat + p_ptr->afraid))
				fear_clear_p();
			else
				fear_decrease_p();
		}
		else if (p_ptr->afraid >= FEAR_SCARED && !fear_save_p(threat/4))
		{
			if (p_ptr->afraid >= FEAR_PETRIFIED)
			{
				p_ptr->energy_need += 100 * TURNS_PER_TICK / 10;
				msg_print("You are scared stiff!");
				disturb(1, 0);
			}
			else if (p_ptr->afraid >= FEAR_TERRIFIED)
			{
				p_ptr->energy_need += 60 * TURNS_PER_TICK / 10;
				msg_print("You shudder uncontrollably!");
				disturb(1, 0);
			}
			else
			{
				p_ptr->energy_need += 30 * TURNS_PER_TICK / 10;
				msg_print("You tremble in terror!");
				disturb(1, 0);
			}
		}
	}
}

/* Handle the Terrifying Aura of Fear! */
#define _AURA_ML   0x01
#define _AURA_LOS  0x02

static void _apply_aura_p(monster_type *m_ptr, int options)
{
	monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];

	if (r_ptr->flags2 & RF2_AURA_FEAR)
	{
		int r_level;

		if ((options & _AURA_ML) && !m_ptr->ml) return;
		if ((options & _AURA_LOS) && !projectable(py, px, m_ptr->fy, m_ptr->fx)) return;

		r_level = _r_level(r_ptr);
		if (!fear_save_p(r_level/MAX(1, m_ptr->cdis-2)))
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);
			msg_format("You behold the terrifying visage of %s!", m_name);
			r_ptr->r_flags2 |= RF2_AURA_FEAR;
			fear_add_p(r_level/MAX(1, m_ptr->cdis-2));
		}
	}
}

void fear_process_p(void)
{
	int i;
	for (i = 1; i < m_max; i++)
	{
		monster_type *m_ptr = &m_list[i];
		if (!m_ptr->r_idx) continue;
		_apply_aura_p(m_ptr, _AURA_ML | _AURA_LOS);
	}
}

void fear_update_m(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];
	if (r_ptr->flags2 & RF2_AURA_FEAR)
		_apply_aura_p(m_ptr, _AURA_ML);
}

void fear_p_touch_m(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];

	if (r_ptr->flags2 & RF2_AURA_FEAR)
	{
		int r_level = _r_level(r_ptr);
		if (!fear_save_p(r_level))
		{
			r_ptr->r_flags2 |= RF2_AURA_FEAR;
			fear_add_p(r_level);
		}
	}
}

bool fear_p_hurt_m(int m_idx, int dam)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];
	bool          result = FALSE;

	/* Apply Aura of Fear to the Player for non-melee damage */
	if (!melee_hack && (r_ptr->flags2 & RF2_AURA_FEAR))
	{
		int r_level = _r_level(r_ptr);
		if (!fear_save_p(r_level))
		{
			r_ptr->r_flags2 |= RF2_AURA_FEAR;
			fear_add_p(r_level/MAX(1, m_ptr->cdis - 2));
		}
	}

	if (!MON_MONFEAR(m_ptr) && !(r_ptr->flags3 & (RF3_NO_FEAR)))
	{
		int percentage = (100 * m_ptr->hp) / m_ptr->maxhp;
		int n = 10;

		n = n * adj_fear_m[p_ptr->stat_ind[A_CHR]] / 100;

		if ((n >= percentage) || (dam >= m_ptr->hp && randint0(100) < 80))
		{
			if (!fear_save_m(m_ptr))
			{
				if (m_ptr->mflag2 & MFLAG2_ENCLOSED)
				{
					char m_name[80];
					monster_desc(m_name, m_ptr, 0);
					msg_format("%^s is enclosed and unable to run away!", m_name);
				}
				else
				{
					result = TRUE;
					set_monster_monfear(m_idx, randint1(10) + 20);
				}
			}
		}
	}
	return result;
}

void fear_terrify_p(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];
	int           r_level = _r_level(r_ptr);

	if (fear_save_p(r_level))
		msg_print(T("You stand your ground!", "���������ݤ˿�����ʤ��ä���"));
	else
		fear_add_p(r_level);
}

void fear_scare_p(monster_type *m_ptr)
{
	monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];
	int           r_level = _r_level(r_ptr);

	if (fear_save_p(r_level))
		msg_print(T("You refuse to be frightened.", "���������ݤ˿�����ʤ��ä���"));
	else
		fear_add_p(r_level);
}

/* Monster Fear */
bool fear_process_m(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	if (MON_MONFEAR(m_ptr))
	{
		if (fear_save_m(m_ptr))
		{
			bool recovered = FALSE;
			if (fear_save_m(m_ptr))
			{
				set_monster_monfear(m_idx, 0);
				recovered = TRUE;
			}
			else
			{
			monster_race *r_ptr = &r_info[m_ptr->ap_r_idx];
			int           r_level = _r_level(r_ptr);

				recovered = set_monster_monfear(m_idx, 
					MON_MONFEAR(m_ptr) - randint1(r_ptr->level / 20 + 1));
			}

			if (recovered && is_seen(m_ptr))
			{
				char m_name[80];
				char m_poss[80];

				monster_desc(m_poss, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE);
				monster_desc(m_name, m_ptr, 0);

				msg_format("%^s recovers %s courage.", m_name, m_poss);
			}
		}
		else if (!fear_save_m(m_ptr) && !fear_save_m(m_ptr))
		{
			if (is_seen(m_ptr))
			{
				char m_name[80];
				monster_desc(m_name, m_ptr, 0);
				msg_format("%^s is scared stiff!", m_name);
			}
			return FALSE;
		}
	}
	return TRUE;
}

#define HURT_0   0
#define HURT_25 25
#define HURT_50 50
#define HURT_65 65
#define HURT_80 80
#define HURT_90 90
#define HURT_95 95

static int _get_hurt_level(int chp)
{
	int pct = (p_ptr->mhp - MAX(chp, 0)) * 100 / p_ptr->mhp;

	if (pct >= HURT_95)
		return HURT_95;
	if (pct >= HURT_90)
		return HURT_90;
	if (pct >= HURT_80)
		return HURT_80;
	if (pct >= HURT_65)
		return HURT_65;
	if (pct >= HURT_50)
		return HURT_50;
	if (pct >= HURT_25)
		return HURT_25;

	return HURT_0;
}

void fear_heal_p(int old_hp, int new_hp)
{
	if (p_ptr->pclass != CLASS_BLOOD_KNIGHT && p_ptr->pclass != CLASS_BLOOD_MAGE && p_ptr->afraid)
	{
		int old_hurt = _get_hurt_level(old_hp);
		int new_hurt = _get_hurt_level(new_hp);

		if (new_hurt < old_hurt && fear_save_p(fear_threat_level()))
			fear_decrease_p();
	}
}

void fear_hurt_p(int old_hp, int new_hp)
{
	if (p_ptr->pclass != CLASS_BLOOD_KNIGHT && p_ptr->pclass != CLASS_BLOOD_MAGE)
	{
		int old_hurt = _get_hurt_level(old_hp);
		int new_hurt = _get_hurt_level(new_hp);
		if (new_hurt > old_hurt)
		{
			if ( !fear_save_p(fear_threat_level())
			  || (new_hurt > HURT_50 && !fear_save_p(fear_threat_level())) )
			{
				fear_add_p(new_hurt);
			}
			else
				msg_format("You stand your ground!");
		}
	}
}
