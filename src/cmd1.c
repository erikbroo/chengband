/* File: cmd1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Movement commands (part 1) */

#include "angband.h"
#define MAX_VAMPIRIC_DRAIN 50

static void _rune_sword_kill(object_type *o_ptr, monster_race *r_ptr)
{
	if (o_ptr->curse_flags & TRC_PERMA_CURSE)
	{
		bool feed = FALSE;
		bool unique = (r_ptr->flags1 & RF1_UNIQUE);
						
		switch (randint1(4))
		{
		case 1:
			if ((r_ptr->level > o_ptr->to_h + o_ptr->to_d + 15))
			{
				/*msg_print("Boost ToHit?");*/
				if (unique || one_in_(6))
				{
					if (o_ptr->to_h < 50 || one_in_(666))
					{
						feed = TRUE;
						o_ptr->to_h++;
					}
				}
			}
			break;

		case 2:
			if ((r_ptr->level > o_ptr->to_h + o_ptr->to_d + 15))
			{
				/*msg_print("Boost ToDam?");*/
				if (unique || one_in_(6))
				{
					if (o_ptr->to_d < 50 || one_in_(666))
					{
						feed = TRUE;
						o_ptr->to_d++;
					}
				}
			}
			break;

		case 3:
			if ((r_ptr->level > o_ptr->dd * o_ptr->ds))
			{
				/*msg_print("Boost dd?");*/
				if (one_in_((o_ptr->dd + 1) * (o_ptr->ds + 1)) && (unique || one_in_(6)))
				{
					if (o_ptr->dd < 9 || one_in_(666))
					{
						feed = TRUE;
						o_ptr->dd++;
					}
				}
			}
			break;

		case 4:
			if ((r_ptr->level > o_ptr->dd * o_ptr->ds))
			{
				/*msg_print("Boost ds?");*/
				if (one_in_((o_ptr->dd + 1) * (o_ptr->ds + 1)) && (unique || one_in_(6)))
				{
					if (o_ptr->ds < 9 || one_in_(666))
					{
						feed = TRUE;
						o_ptr->ds++;
					}
				}
			}
			break;
		}

		if (unique && one_in_(200 / MAX(r_ptr->level, 1)))
		{
			switch (randint1(11))
			{
			case 1:
				if (one_in_(6)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_BRAND_POIS); 
				} 
				break;
			case 2:
				if (one_in_(6)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_BRAND_FIRE);
				} 
				break;
			case 3:
				if (one_in_(6)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_BRAND_COLD); 
				} 
				break;
			case 4:
				if (one_in_(26)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_BRAND_ELEC); 
				} 
				break;
			case 5:
				if (one_in_(26)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_UNDEAD); 
				} 
				break;
			case 6:
				if (one_in_(26)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_DEMON); 
				} 
				break;
			case 7:
				if (one_in_(13)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_DRAGON); 
				} 
				break;
			case 8:
				if (one_in_(13)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_TROLL); 
				} 
				break;
			case 9:
				if (one_in_(13)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_GIANT); 
				} 
				break;
			case 10:
				if (one_in_(66)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_HUMAN); 
				} 
				break;
			case 11:
				if (one_in_(666)) 
				{ 
					feed = TRUE; 
					add_flag(o_ptr->art_flags, TR_SLAY_EVIL); 
				} 
				break;
			}
		}

		if (feed)
		{
			if ((o_ptr->curse_flags & TRC_TY_CURSE) == 0
			  && o_ptr->dd * o_ptr->ds > 50 )
			{  
				o_ptr->curse_flags |= TRC_TY_CURSE;
				msg_print("Your Rune Sword seeks to dominate you!");
			}
			else if ((o_ptr->curse_flags & TRC_AGGRAVATE) == 0
				   && o_ptr->dd * o_ptr->ds > 30 )
			{
				o_ptr->curse_flags |= TRC_AGGRAVATE;
				msg_print("The thirst of your sword redoubles!");
			}
			else
				msg_print("Your rune sword grows more powerful!");
		}
	}
	else
		msg_print("Only cursed Rune Swords may feed.");
}

static void death_scythe_miss(object_type *o_ptr, int hand, int mode)
{
	u32b flgs[TR_FLAG_SIZE];
	int k;
	critical_t crit;

	/* Sound */
	sound(SOUND_HIT);

	/* Message */
#ifdef JP
	msg_print("振り回した大鎌が自分自身に返ってきた！");
#else
	msg_print("Your scythe returns to you!");
#endif

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	k = damroll(o_ptr->dd + p_ptr->weapon_info[hand].to_dd, o_ptr->ds + p_ptr->weapon_info[hand].to_ds);
	{
		int mult;
		switch (p_ptr->mimic_form)
		{
		case MIMIC_NONE:
			switch (p_ptr->prace)
			{
				case RACE_YEEK:
				case RACE_KLACKON:
				case RACE_HUMAN:
				case RACE_AMBERITE:
				case RACE_DUNADAN:
				case RACE_BARBARIAN:
				case RACE_BEASTMAN:
				case RACE_DEMIGOD:
					mult = 25;break;
				case RACE_SNOTLING:
				case RACE_HALF_TROLL:
				case RACE_HALF_OGRE:
				case RACE_HALF_GIANT:
				case RACE_HALF_TITAN:
				case RACE_CYCLOPS:
				case RACE_IMP:
				case RACE_SKELETON:
				case RACE_ZOMBIE:
				case RACE_VAMPIRE:
				case RACE_SPECTRE:
				case RACE_BALROG:
				case RACE_DRACONIAN:
				case RACE_TONBERRY:
					mult = 30;break;
				default:
					mult = 10;break;
			}
			break;
		case MIMIC_DEMON:
		case MIMIC_DEMON_LORD:
		case MIMIC_VAMPIRE:
			mult = 30;break;
		default:
			mult = 10;break;
		}

		if (p_ptr->align < 0 && mult < 20)
			mult = 20;
		if (p_ptr->resist_acid <= 0 && mult < 25)
			mult = 25;
		if (p_ptr->resist_elec <= 0 && mult < 25)
			mult = 25;
		if (p_ptr->resist_fire <= 0 && mult < 25)
			mult = 25;
		if (p_ptr->resist_cold <= 0 && mult < 25)
			mult = 25;
		if (p_ptr->resist_pois <= 0 && mult < 25)
			mult = 25;

		if (p_ptr->tim_slay_sentient && p_ptr->ryoute)
		{
			if (prace_is_(RACE_DEMIGOD) && p_ptr->psubrace == DEMIGOD_POSEIDON)
			{
			}
			else if (mut_present(MUT_WEIRD_MIND))
			{
			}
			else if (mult < 20) mult = 20;
		}

		if ((have_flag(flgs, TR_FORCE_WEAPON) || p_ptr->tim_force) && (p_ptr->csp > (p_ptr->msp / 30)))
		{
			p_ptr->csp -= (1+(p_ptr->msp / 30));
			p_ptr->redraw |= (PR_MANA);
			mult = mult * 3 / 2 + 10;
		}

		if (mauler_get_toggle() == TOGGLE_DEATH_FORCE && p_ptr->ryoute)
		{
			int cost = 1 + (o_ptr->dd * o_ptr->ds) / 9;
			if (p_ptr->fast >= cost)
			{
				set_fast(p_ptr->fast - cost, TRUE);
				mult = mult * 3 / 2 + 10;
			}
		}

		k *= mult;
		k /= 10;
	}

	crit = critical_norm(o_ptr->weight, o_ptr->to_h, p_ptr->weapon_info[hand].to_h, mode);
	if (crit.desc)
	{
		k = k * crit.mul/100 + crit.to_d;
		msg_print(crit.desc);
	}

	if (one_in_(6))
	{
		int mult = 2;
#ifdef JP
		msg_format("グッサリ切り裂かれた！");
#else
		msg_format("Your weapon cuts deep into yourself!");
#endif
		/* Try to increase the damage */
		while (one_in_(4))
		{
			mult++;
		}

		k *= mult;
	}
	k += (p_ptr->weapon_info[hand].to_d + o_ptr->to_d);

	if (k < 0) k = 0;

#ifdef JP
	take_hit(DAMAGE_FORCE, k, "死の大鎌", -1);
#else
	take_hit(DAMAGE_FORCE, k, "Death scythe", -1);
#endif

	redraw_stuff();
}

/*
 * Determine if the player "hits" a monster (normal combat).
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_fire(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (p_ptr->personality == PERS_LAZY)
		if (one_in_(20)) return (FALSE);

	/* Never hit */
	if (chance <= 0) return (FALSE);

	/* Invisible monsters are harder to hit */
	if (!vis) chance = (chance + 1) / 2;

	/* Power competes against armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Determine if the player "hits" a monster (normal combat).
 *
 * Note -- Always miss 5%, always hit 5%, otherwise random.
 */
bool test_hit_norm(int chance, int ac, int vis)
{
	int k;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- Instant miss or hit */
	if (k < 10) return (k < 5);

	if (p_ptr->personality == PERS_LAZY)
		if (one_in_(20)) return (FALSE);

	/* Wimpy attack never hits */
	if (chance <= 0) return (FALSE);

	/* Penalize invisible targets */
	if (!vis) chance = (chance + 1) / 2;

	/* Power must defeat armor */
	if (randint0(chance) < (ac * 3 / 4)) return (FALSE);

	/* Assume hit */
	return (TRUE);
}



/*
 * Critical hits (from objects thrown by player)
 * Factor in item weight, total plusses, and player level.
 */
s16b critical_shot(int weight, int plus, int dam)
{
	int i, k;

	/* Extract "shot" power */
	i = ((p_ptr->to_h_b + plus) * 4) + (p_ptr->lev * 2);

	/* Snipers can shot more critically with crossbows */
	if (p_ptr->concent) i += ((i * p_ptr->concent) / 5);
	if ((p_ptr->pclass == CLASS_SNIPER) && (p_ptr->tval_ammo == TV_BOLT)) i *= 2;

	/* Critical hit */
	if (randint1(5000) <= i)
	{
		k = weight * randint1(500);

		if (k < 900)
		{
#ifdef JP
			msg_print("手ごたえがあった！");
#else
			msg_print("It was a good hit!");
#endif

			dam += (dam / 2);
		}
		else if (k < 1350)
		{
#ifdef JP
			msg_print("かなりの手ごたえがあった！");
#else
			msg_print("It was a great hit!");
#endif

			dam *= 2;
		}
		else
		{
#ifdef JP
			msg_print("会心の一撃だ！");
#else
			msg_print("It was a superb hit!");
#endif

			dam *= 3;
		}
	}

	return (dam);
}



/*
 * Critical hits (by player)
 *
 * Factor in weapon weight, total plusses, player level.
 */
critical_t critical_norm(int weight, int plus, s16b meichuu, int mode)
{
	critical_t result = {0};
	int i;
	int roll = (p_ptr->pclass == CLASS_NINJA) ? 4444 : 5000;

	if (p_ptr->enhanced_crit)
	{
		weight = weight * 3 / 2;
		weight += 300;
	}

	/* Hack: Two handed wielding gets more crits.   Dual wielding, less */
	                    /* v----- I'm pretty sure monks get "ryoute" as well ... */
	if (p_ptr->ryoute && buki_motteruka(INVEN_RARM))
	{
		if (!p_ptr->omoi) /* Two handed, but weapon is otherwise too heavy to use with just one hand, so no bonus */
			roll = roll * 2 / 3;
	}
	else if (p_ptr->migite && p_ptr->hidarite)
	{
		roll = roll * 3 / 2;
	}

	/* Extract "blow" power */
	i = (weight + (meichuu * 3 + plus * 5) + (p_ptr->lev * 3));

	/* Mauler L45: Destroyer - +(W/20)% chance of crits 
	   (e.g. a 40 lb heavy lance gets +20% chance to crit). */
	if ( p_ptr->pclass == CLASS_MAULER
	  && p_ptr->ryoute )
	{
		int pct = weight / 20;
		i += roll * pct / 100;
	}

	/* Chance */
	if ((randint1(roll) <= i) || (mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN))
	{
		int k = weight + randint1(650);
		if ((mode == HISSATSU_MAJIN) || (mode == HISSATSU_3DAN)) k+= randint1(650);

		if (k < 400)
		{
			result.desc = T("It was a good hit!", "手ごたえがあった！");
			result.mul = 200;
			result.to_d = 5;
		}
		else if (k < 700)
		{
			result.desc = T("It was a great hit!", "かなりの手ごたえがあった！");
			result.mul = 200;
			result.to_d = 10;
		}
		else if (k < 900)
		{
			result.desc = T("It was a superb hit!", "会心の一撃だ！");
			result.mul = 300;
			result.to_d = 15;
		}
		else if (k < 1300)
		{
			result.desc = T("It was a *GREAT* hit!", "最高の会心の一撃だ！");
			result.mul = 300;
			result.to_d = 20;
		}
		else
		{
			result.desc = T("It was a *SUPERB* hit!", "比類なき最高の会心の一撃だ！");
			result.mul = 350;
			result.to_d = 25;
		}
	}

	return result;
}



/*
 * Extract the "total damage" from a given object hitting a given monster.
 *
 * Note that "flasks of oil" do NOT do fire damage, although they
 * certainly could be made to do so.  XXX XXX
 *
 * Note that most brands and slays are x3, except Slay Animal (x2),
 * Slay Evil (x2), and Kill dragon (x5).
 */

s16b tot_dam_aux_monk(int tdam, monster_type *m_ptr)
{
	int mult = 10;
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	const int monk_elem_slay = 17;

	if ( p_ptr->weapon_info[0].brand_acid
		|| (p_ptr->special_attack & ATTACK_ACID) )
	{
		if (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)
		{
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK);
		}
		else
		{
			if (mult < monk_elem_slay) mult = monk_elem_slay;
		}
	}

	if ( p_ptr->weapon_info[0].brand_elec
		|| (p_ptr->special_attack & ATTACK_ELEC) )
	{
		if (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK)
		{
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK);
		}
		else
		{
			if (mult < monk_elem_slay) mult = monk_elem_slay;
		}
	}

	if ( p_ptr->weapon_info[0].brand_fire
		|| (p_ptr->special_attack & ATTACK_FIRE) )
	{
		if (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)
		{
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK);
		}
		else
		{
			if (mult < monk_elem_slay) mult = monk_elem_slay;
		}
	}

	if ( p_ptr->weapon_info[0].brand_cold
		|| (p_ptr->special_attack & ATTACK_COLD) )
	{
		if (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK)
		{
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK);
		}
		else
		{
			if (mult < monk_elem_slay) mult = monk_elem_slay;
		}
	}

	if ( p_ptr->weapon_info[0].brand_pois
		|| (p_ptr->special_attack & ATTACK_POIS) )
	{
		if (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)
		{
			if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK);
		}
		else
		{
			if (mult < monk_elem_slay) mult = monk_elem_slay;
		}
	}

	return (tdam * mult / 10);
}

#define _MAX_CHAOS_SLAYS 14

int _chaos_slays[_MAX_CHAOS_SLAYS] = {
	TR_SLAY_ANIMAL,
	TR_SLAY_EVIL,
	TR_SLAY_UNDEAD,
	TR_SLAY_DEMON,
	TR_SLAY_ORC,
	TR_SLAY_TROLL,
	TR_SLAY_GIANT,
	TR_SLAY_DRAGON,
	TR_SLAY_HUMAN,
	TR_BRAND_POIS,
	TR_BRAND_ACID,
	TR_BRAND_ELEC,
	TR_BRAND_FIRE,
	TR_BRAND_COLD,
};	

s16b tot_dam_aux(object_type *o_ptr, int tdam, monster_type *m_ptr, s16b hand, int mode, bool thrown)
{
	int mult = 10;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	int chaos_slay = 0;
	bool hephaestus_hack = FALSE;

	u32b flgs[TR_FLAG_SIZE];
	char o_name[MAX_NLEN];

	/* Extract the flags */
	object_flags(o_ptr, flgs);

	/* Chaos Weapons now have random slay effects, and the slay so
	   chosen will augment any existing slay of the same type. */
	if (have_flag(flgs, TR_CHAOTIC))
	{
		chaos_slay = _chaos_slays[randint0(_MAX_CHAOS_SLAYS)];
		object_desc(o_name, o_ptr, OD_NAME_ONLY);
	}

	/* Some "weapons" and "ammo" do extra damage */
	switch (o_ptr->tval)
	{
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DIGGING:
		{
			if (p_ptr->tim_blood_seek && monster_living(r_ptr))
			{
				if (mult < 20) mult = 20;
			}

			if (r_ptr->flags3 & RF3_ANIMAL)
			{
				if (chaos_slay == TR_SLAY_ANIMAL)
				{
					msg_format("%s slays animals.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ANIMAL;
					if (have_flag(flgs, TR_KILL_ANIMAL))
					{
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_ANIMAL))
					{
						if (mult < 35) mult = 35;
					}
					else
					{
						if (mult < 25) mult = 25;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_ANIMAL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ANIMAL;
						if (mult < 40) mult = 40;
					}
					else if (have_flag(flgs, TR_SLAY_ANIMAL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ANIMAL;
						if (mult < 25) mult = 25;
					}
				}
			}

			if (r_ptr->flags3 & RF3_EVIL)
			{
				if (chaos_slay == TR_SLAY_EVIL)
				{
					msg_format("%s slays evil.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_EVIL;
					if (have_flag(flgs, TR_KILL_EVIL))
					{
						if (mult < 45) mult = 45;
					}
					else if (have_flag(flgs, TR_SLAY_EVIL) || weaponmaster_get_toggle() == TOGGLE_HOLY_BLADE)
					{
						if (mult < 30) mult = 30;
					}
					else
					{
						if (mult < 20) mult = 20;
					}

				}
				else
				{
					if (have_flag(flgs, TR_KILL_EVIL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_EVIL;
						if (mult < 35) mult = 35;
					}
					else if (have_flag(flgs, TR_SLAY_EVIL) || weaponmaster_get_toggle() == TOGGLE_HOLY_BLADE)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_EVIL;
						if (mult < 20) mult = 20;
					}
				}
			}

			if (r_ptr->flags2 & RF2_HUMAN)
			{
				if (chaos_slay == TR_SLAY_HUMAN)
				{
					msg_format("%s slays humans.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_HUMAN;
					if (have_flag(flgs, TR_KILL_HUMAN))
					{
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_HUMAN))
					{
						if (mult < 35) mult = 35;
					}
					else
					{
						if (mult < 25) mult = 25;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_HUMAN))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_HUMAN;
						if (mult < 40) mult = 40;
					}
					else if (have_flag(flgs, TR_SLAY_HUMAN))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_HUMAN;
						if (mult < 25) mult = 25;
					}
				}
			}

			if (r_ptr->flags3 & RF3_UNDEAD)
			{
				if (chaos_slay == TR_SLAY_UNDEAD)
				{
					msg_format("%s slays undead.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_UNDEAD;
					if (have_flag(flgs, TR_KILL_UNDEAD))
					{
						if (mult < 60) mult = 60;
					}
					else if (have_flag(flgs, TR_SLAY_UNDEAD))
					{
						if (mult < 40) mult = 40;
					}
					else
					{
						if (mult < 30) mult = 30;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_UNDEAD))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_UNDEAD;
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_UNDEAD))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_UNDEAD;
						if (mult < 30) mult = 30;
					}
				}
			}

			if (r_ptr->flags3 & RF3_DEMON)
			{
				if (chaos_slay == TR_SLAY_DEMON)
				{
					msg_format("%s slays demons.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_DEMON;
					if (have_flag(flgs, TR_KILL_DEMON))
					{
						if (mult < 60) mult = 60;
					}
					else if (have_flag(flgs, TR_SLAY_DEMON))
					{
						if (mult < 40) mult = 40;
					}
					else
					{
						if (mult < 30) mult = 30;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_DEMON))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_DEMON;
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_DEMON))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_DEMON;
						if (mult < 30) mult = 30;
					}
				}
			}

			if (r_ptr->flags3 & RF3_ORC)
			{
				if (chaos_slay == TR_SLAY_ORC)
				{
					msg_format("%s slays orcs.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ORC;
					if (have_flag(flgs, TR_KILL_ORC))
					{
						if (mult < 60) mult = 60;
					}
					else if (have_flag(flgs, TR_SLAY_ORC))
					{
						if (mult < 40) mult = 40;
					}
					else
					{
						if (mult < 30) mult = 30;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_ORC))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ORC;
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_ORC))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_ORC;
						if (mult < 30) mult = 30;
					}
				}
			}


			if (r_ptr->flags3 & RF3_TROLL)
			{
				if (chaos_slay == TR_SLAY_TROLL)
				{
					msg_format("%s slays trolls.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_TROLL;
					if (have_flag(flgs, TR_KILL_TROLL))
					{
						if (mult < 60) mult = 60;
					}
					else if (have_flag(flgs, TR_SLAY_TROLL))
					{
						if (mult < 40) mult = 40;
					}
					else
					{
						if (mult < 30) mult = 30;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_TROLL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_TROLL;
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_TROLL))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_TROLL;
						if (mult < 30) mult = 30;
					}
				}
			}

			if (r_ptr->flags3 & RF3_GIANT)
			{
				if (chaos_slay == TR_SLAY_GIANT)
				{
					msg_format("%s slays giants.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_GIANT;
					if (have_flag(flgs, TR_KILL_GIANT))
					{
						if (mult < 60) mult = 60;
					}
					else if (have_flag(flgs, TR_SLAY_GIANT))
					{
						if (mult < 40) mult = 40;
					}
					else
					{
						if (mult < 30) mult = 30;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_GIANT))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_GIANT;
						if (mult < 50) mult = 50;
					}
					else if (have_flag(flgs, TR_SLAY_GIANT))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_GIANT;
						if (mult < 30) mult = 30;
					}
				}
			}

			if (r_ptr->flags3 & RF3_DRAGON)
			{
				if (chaos_slay == TR_SLAY_DRAGON)
				{
					msg_format("%s slays dragons.", o_name);
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_DRAGON;
					if (have_flag(flgs, TR_KILL_DRAGON))
					{
						if (mult < 60) mult = 60;
						if ((o_ptr->name1 == ART_NOTHUNG) && (m_ptr->r_idx == MON_FAFNER))
							mult *= 3;
					}
					else if (have_flag(flgs, TR_SLAY_DRAGON))
					{
						if (mult < 40) mult = 40;
					}
					else
					{
						if (mult < 30) mult = 30;
					}
				}
				else
				{
					if (have_flag(flgs, TR_KILL_DRAGON))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_DRAGON;
						if (mult < 50) mult = 50;
						if ((o_ptr->name1 == ART_NOTHUNG) && (m_ptr->r_idx == MON_FAFNER))
							mult *= 3;
					}
					else if (have_flag(flgs, TR_SLAY_DRAGON))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_DRAGON;
						if (mult < 30) mult = 30;
					}
				}
			}


			/* Hex - Slay Good (Runesword) */
			if (hex_spelling(HEX_RUNESWORD) &&
			    (r_ptr->flags3 & RF3_GOOD))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_GOOD;
				}

				if (mult < 20) mult = 20;
			}

			/* Brand (Acid) */
			if ( have_flag(flgs, TR_BRAND_ACID) 
			  || p_ptr->weapon_info[hand].brand_acid
			  || ((p_ptr->special_attack & ATTACK_ACID) && !thrown)
			  || chaos_slay == TR_BRAND_ACID)
			{
				if (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ACID_MASK);
				}
				else if (chaos_slay == TR_BRAND_ACID)
				{
					msg_format("%s is covered in acid.", o_name);
					if ( have_flag(flgs, TR_BRAND_ACID) 
					  || p_ptr->weapon_info[hand].brand_acid 
					  || ((p_ptr->special_attack & (ATTACK_ACID)) && !thrown))
					{
						if (mult < 35) mult = 35;
					}
					else
					{
						if (mult < 25) mult = 25;
					}
				}
				else
				{
					if (mult < 25) mult = 25;
				}
			}

			/* Brand (Elec) */
			if ( have_flag(flgs, TR_BRAND_ELEC) 
			 || p_ptr->weapon_info[hand].brand_elec
			 || ((p_ptr->special_attack & ATTACK_ELEC) && !thrown) 
			 || mode == HISSATSU_ELEC
			 || chaos_slay == TR_BRAND_ELEC )
			{
				if (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK);
				}
				else if (chaos_slay == TR_BRAND_ELEC)
				{
					msg_format("%s is covered in electricity.", o_name);
					if (have_flag(flgs, TR_BRAND_ELEC) || p_ptr->weapon_info[hand].brand_elec)
					{
						if (mode == HISSATSU_ELEC)
						{
							if (mult < 80) mult = 80;
						}
						else
						{
							if (mult < 35) mult = 35;
						}
					}
					else if ((p_ptr->special_attack & ATTACK_ELEC) && !thrown)
					{
						if (mode == HISSATSU_ELEC)
						{
							if (mult < 80) mult = 80;
						}
						else
						{
							if (mult < 35) mult = 35;
						}
					}
					else if (mode == HISSATSU_ELEC)
					{
						if (mult < 60) mult = 60;
					}
					else
					{
						if (mult < 35) mult = 35;
					}
				}
				else
				{
					if (have_flag(flgs, TR_BRAND_ELEC) || p_ptr->weapon_info[hand].brand_elec)
					{
						if (mode == HISSATSU_ELEC)
						{
							if (mult < 70) mult = 70;
						}
						else
						{
							if (mult < 25) mult = 25;
						}
					}
					else if ((p_ptr->special_attack & ATTACK_ELEC) && !thrown)
					{
						if (mode == HISSATSU_ELEC)
						{
							if (mult < 70) mult = 70;
						}
						else
						{
							if (mult < 25) mult = 25;
						}
					}
					else if (mode == HISSATSU_ELEC)
					{
						if (mult < 50) mult = 50;
					}
				}
			}

			/* Brand (Fire) */
			if ( have_flag(flgs, TR_BRAND_FIRE) 
			 || p_ptr->weapon_info[hand].brand_fire
			 || ((p_ptr->special_attack & ATTACK_FIRE) && !thrown) 
			 || mode == HISSATSU_FIRE
			 || chaos_slay == TR_BRAND_FIRE )
			{
				if (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK);
				}
				else if (chaos_slay == TR_BRAND_FIRE)
				{
					int tmp = 0;
					msg_format("%s is covered in fire.", o_name);
					if (have_flag(flgs, TR_BRAND_FIRE) || p_ptr->weapon_info[hand].brand_fire)
					{
						if (mode == HISSATSU_FIRE)
							tmp = 45;
						else
							tmp = 35;
					}
					else if ((p_ptr->special_attack & ATTACK_FIRE) && !thrown)
					{
						if (mode == HISSATSU_FIRE)
							tmp = 45;
						else
							tmp = 35;
					}
					else if (mode == HISSATSU_FIRE)
					{
						tmp = 35;
					}
					else
					{
						tmp = 25;
					}

					if (r_ptr->flags3 & RF3_HURT_FIRE)
					{	
						tmp *= 2;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_HURT_FIRE;
					}
					if (mult < tmp) mult = tmp;
				}
				else
				{
					int tmp = 0;
					if (have_flag(flgs, TR_BRAND_FIRE) || p_ptr->weapon_info[hand].brand_fire)
					{
						if (mode == HISSATSU_FIRE)
							tmp = 35;
						else
							tmp = 25;
					}
					else if ((p_ptr->special_attack & ATTACK_FIRE) && !thrown)
					{
						if (mode == HISSATSU_FIRE)
							tmp = 35;
						else
							tmp = 25;
					}
					else if (mode == HISSATSU_FIRE)
					{
						tmp = 25;
					}

					if (tmp > 0)
					{
						if (r_ptr->flags3 & RF3_HURT_FIRE)
						{	
							tmp *= 2;
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_HURT_FIRE;
						}
						if (mult < tmp) mult = tmp;
					}
				}
			}

			/* Brand (Cold) */
			if ( have_flag(flgs, TR_BRAND_COLD) 
			  || p_ptr->weapon_info[hand].brand_cold
			  || ((p_ptr->special_attack & ATTACK_COLD) && !thrown) 
			  || mode == HISSATSU_COLD
			  || chaos_slay == TR_BRAND_COLD ) 
			{
				if (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK);
				}
				else if (chaos_slay == TR_BRAND_COLD)
				{
					int tmp = 0;
					msg_format("%s is covered in frost.", o_name);
					if (have_flag(flgs, TR_BRAND_COLD) || p_ptr->weapon_info[hand].brand_cold)
					{
						if (mode == HISSATSU_COLD)
							tmp = 45;
						else
							tmp = 35;
					}
					else if ((p_ptr->special_attack & ATTACK_COLD) && !thrown)
					{
						if (mode == HISSATSU_COLD)
							tmp = 45;
						else
							tmp = 35;
					}
					else if (mode == HISSATSU_COLD)
					{
						tmp = 35;
					}
					else
					{
						tmp = 25;
					}

					if (r_ptr->flags3 & RF3_HURT_COLD)
					{	
						tmp *= 2;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_HURT_COLD;
					}
					if (mult < tmp) mult = tmp;
				}
				else
				{
					int tmp = 0;
					if (have_flag(flgs, TR_BRAND_COLD) || p_ptr->weapon_info[hand].brand_cold)
					{
						if (mode == HISSATSU_COLD)
							tmp = 35;
						else
							tmp = 25;
					}
					else if ((p_ptr->special_attack & ATTACK_COLD) && !thrown)
					{
						if (mode == HISSATSU_COLD)
							tmp = 35;
						else
							tmp = 25;
					}
					else if (mode == HISSATSU_COLD)
					{
						tmp = 25;
					}

					if (tmp > 0)
					{
						if (r_ptr->flags3 & RF3_HURT_COLD)
						{	
							tmp *= 2;
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_HURT_COLD;
						}
						if (mult < tmp) mult = tmp;
					}
				}
			}

			/* Brand (Poison) */
			if ( have_flag(flgs, TR_BRAND_POIS) 
			  || p_ptr->weapon_info[hand].brand_pois
			  || ((p_ptr->special_attack & ATTACK_POIS) && !thrown) 
			  || mode == HISSATSU_POISON
			  || chaos_slay == TR_BRAND_POIS )
			{
				if (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_POIS_MASK);
				}
				else if (chaos_slay == TR_BRAND_POIS)
				{
					msg_format("%s is covered in poison.", o_name);
					if (have_flag(flgs, TR_BRAND_POIS) || p_ptr->weapon_info[hand].brand_pois)
					{
						if (mode == HISSATSU_POISON)
						{
							if (mult < 45) mult = 45;
						}
						else
						{
							if (mult < 35) mult = 35;
						}
					}
					else if ((p_ptr->special_attack & ATTACK_POIS) && !thrown)
					{
						if (mode == HISSATSU_POISON)
						{
							if (mult < 45) mult = 45;
						}
						else
						{
							if (mult < 35) mult = 35;
						}
					}
					else if (mode == HISSATSU_POISON)
					{
						if (mult < 35) mult = 35;
					}
					else
					{
						if (mult < 25) mult = 25;
					}
				}
				else
				{
					if (have_flag(flgs, TR_BRAND_POIS) || p_ptr->weapon_info[hand].brand_pois)
					{
						if (mode == HISSATSU_POISON)
						{
							if (mult < 35) mult = 35;
						}
						else
						{
							if (mult < 25) mult = 25;
						}
					}
					else if ((p_ptr->special_attack & ATTACK_POIS) && !thrown)
					{
						if (mode == HISSATSU_POISON)
						{
							if (mult < 35) mult = 35;
						}
						else
						{
							if (mult < 25) mult = 25;
						}
					}
					else if (mode == HISSATSU_POISON)
					{
						if (mult < 25) mult = 25;
					}
				}
			}

			if ((mode == HISSATSU_ZANMA) && !monster_living(r_ptr) && (r_ptr->flags3 & RF3_EVIL))
			{
				if (mult < 15) mult = 25;
				else if (mult < 50) mult = MIN(50, mult+20);
			}
			if (mode == HISSATSU_UNDEAD)
			{
				if (r_ptr->flags3 & RF3_UNDEAD)
				{
					if (is_original_ap_and_seen(m_ptr))
					{
						r_ptr->r_flags3 |= RF3_UNDEAD;
					}
					if (mult == 10) mult = 70;
					else if (mult < 140) mult = MIN(140, mult+60);
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = MIN(60, mult+30);
			}
			if ((mode == HISSATSU_SEKIRYUKA) && p_ptr->cut && monster_living(r_ptr))
			{
				int tmp = MIN(100, MAX(10, p_ptr->cut / 10));
				if (mult < tmp) mult = tmp;
			}
			if ((mode == HISSATSU_HAGAN) && (r_ptr->flags3 & RF3_HURT_ROCK))
			{
				if (is_original_ap_and_seen(m_ptr))
				{
					r_ptr->r_flags3 |= RF3_HURT_ROCK;
				}
				if (mult == 10) mult = 40;
				else if (mult < 60) mult = 60;
			}
			if (p_ptr->tim_slay_sentient && p_ptr->ryoute)
			{
				if (r_ptr->flags3 & RF3_NO_STUN)
				{
					if (is_original_ap_and_seen(m_ptr))	r_ptr->r_flags3 |= RF3_NO_STUN;
				}
				else
				{
					if (mult < 20) mult = 20;
				}
			}
			if (mult > 10 && prace_is_(RACE_DEMIGOD) && p_ptr->psubrace == DEMIGOD_HEPHAESTUS)
			{
				hephaestus_hack = TRUE;
				mult += 10;
			}

			if (have_flag(flgs, TR_FORCE_WEAPON) || p_ptr->tim_force)
			{
				int cost = 0;
				int dd = o_ptr->dd + p_ptr->weapon_info[hand].to_dd;
				int ds = o_ptr->ds + p_ptr->weapon_info[hand].to_ds;
				
				if (p_ptr->pclass == CLASS_SAMURAI)
					cost = (1 + (dd * ds * 2 / 7));
				else
					cost = (1 + (dd * ds / 7));

				if (p_ptr->csp >= cost)
				{
					p_ptr->csp -= cost;
					p_ptr->redraw |= (PR_MANA);
					mult = mult * 3 / 2 + 10;
				}
			}
			if (mauler_get_toggle() == TOGGLE_DEATH_FORCE &&  p_ptr->ryoute)
			{
				int cost = 1 + (o_ptr->dd * o_ptr->ds) / 9;
				if (p_ptr->fast >= cost)
				{
					set_fast(p_ptr->fast - cost, TRUE);
					mult = mult * 3 / 2 + 10;
				}
			}
			if (mult > 10 && prace_is_(RACE_DEMIGOD) && p_ptr->psubrace == DEMIGOD_HEPHAESTUS && !hephaestus_hack)
			{
				mult += 10;
			}

			if (p_ptr->tim_blood_feast)
			{
				take_hit(DAMAGE_ATTACK, 15, "blood feast", -1);
			}
			break;
		}
	}
	if (mult > 150) mult = 150;

	/* Return the total damage */
	return (tdam * mult / 10);
}


/*
 * Search for hidden things
 */
void search(void)
{
	int y, x, chance;

	s16b this_o_idx, next_o_idx = 0;

	cave_type *c_ptr;


	/* Start with base search ability */
	chance = p_ptr->skills.srh;

	/* Penalize various conditions */
	if (p_ptr->blind || no_lite()) chance = chance / 10;
	if (p_ptr->confused || p_ptr->image) chance = chance / 10;

	/* Search the nearby grids, which are always in bounds */
	for (y = (py - 1); y <= (py + 1); y++)
	{
		for (x = (px - 1); x <= (px + 1); x++)
		{
			/* Sometimes, notice things */
			if (randint0(100) < chance)
			{
				/* Access the grid */
				c_ptr = &cave[y][x];

				/* Invisible trap */
				if (c_ptr->mimic && is_trap(c_ptr->feat))
				{
					/* Pick a trap */
					disclose_grid(y, x);

					/* Message */
#ifdef JP
					msg_print("トラップを発見した。");
#else
					msg_print("You have found a trap.");
#endif

					/* Disturb */
					disturb(0, 0);
				}

				/* Secret door */
				if (is_hidden_door(c_ptr))
				{
					/* Message */
#ifdef JP
					msg_print("隠しドアを発見した。");
#else
					msg_print("You have found a secret door.");
#endif

					/* Disclose */
					disclose_grid(y, x);

					/* Disturb */
					disturb(0, 0);
				}

				/* Scan all objects in the grid */
				for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					object_type *o_ptr;

					/* Acquire object */
					o_ptr = &o_list[this_o_idx];

					/* Acquire next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Skip non-chests */
					if (o_ptr->tval != TV_CHEST) continue;

					/* Skip non-trapped chests */
					if (!chest_traps[o_ptr->pval]) continue;

					/* Identify once */
					if (!object_is_known(o_ptr))
					{
						/* Message */
#ifdef JP
						msg_print("箱に仕掛けられたトラップを発見した！");
#else
						msg_print("You have discovered a trap on the chest!");
#endif

						/* Know the trap */
						object_known(o_ptr);

						/* Notice it */
						disturb(0, 0);
					}
				}
			}
		}
	}
}


/*
 * Helper routine for py_pickup() and py_pickup_floor().
 *
 * Add the given dungeon object to the character's inventory.
 *
 * Delete the object afterwards.
 */
void py_pickup_aux(int o_idx)
{
	int slot, i;

#ifdef JP
/*
 * アイテムを拾った際に「２つのケーキを持っている」
 * "You have two cakes." とアイテムを拾った後の合計のみの表示がオリジナル
 * だが、違和感が
 * あるという指摘をうけたので、「〜を拾った、〜を持っている」という表示
 * にかえてある。そのための配列。
 */
	char o_name[MAX_NLEN];
	char old_name[MAX_NLEN];
	char kazu_str[80];
	int hirottakazu;
#else
	char o_name[MAX_NLEN];
#endif

	object_type *o_ptr;

	o_ptr = &o_list[o_idx];

#ifdef JP
	/* Describe the object */
	object_desc(old_name, o_ptr, OD_NAME_ONLY);
	object_desc_kosuu(kazu_str, o_ptr);
	hirottakazu = o_ptr->number;
#endif
	/* Carry the object */
	slot = inven_carry(o_ptr);

	/* Get the object again */
	o_ptr = &inventory[slot];

	/* Delete the object */
	delete_object_idx(o_idx);

	if (p_ptr->personality == PERS_MUNCHKIN || mut_present(MUT_LOREMASTER))
	{
		bool old_known = identify_item(o_ptr);

		if (mut_present(MUT_LOREMASTER))
			o_ptr->ident |= (IDENT_MENTAL);

		/* Auto-inscription/destroy */
		autopick_alter_item(slot, (bool)(destroy_identify && !old_known));

		/* If it is destroyed, don't pick it up */
		if (o_ptr->marked & OM_AUTODESTROY) return;
	}

	/* Describe the object */
	object_desc(o_name, o_ptr, 0);

	/* Message */
#ifdef JP
	if ((o_ptr->name1 == ART_CRIMSON) && (p_ptr->personality == PERS_COMBAT))
	{
		msg_format("こうして、%sは『クリムゾン』を手に入れた。", player_name);
		msg_print("しかし今、『混沌のサーペント』の放ったモンスターが、");
		msg_format("%sに襲いかかる．．．", player_name);
	}
	else
	{
		if (plain_pickup)
		{
			msg_format("%s(%c)を持っている。",o_name, index_to_label(slot));
		}
		else
		{
			if (o_ptr->number > hirottakazu) {
			    msg_format("%s拾って、%s(%c)を持っている。",
			       kazu_str, o_name, index_to_label(slot));
			} else {
				msg_format("%s(%c)を拾った。", o_name, index_to_label(slot));
			}
		}
	}
	strcpy(record_o_name, old_name);
#else
	msg_format("You have %s (%c).", o_name, index_to_label(slot));
	strcpy(record_o_name, o_name);
#endif

	/* Runes confer benefits even when in inventory */
	p_ptr->update |= PU_BONUS;

	/* Hack: Archaeologists Instantly Pseudo-ID artifacts on pickup */
	if ( p_ptr->pclass == CLASS_ARCHAEOLOGIST 
	  && object_is_artifact(o_ptr) 
	  && !object_is_known(o_ptr) )
	{
		/* Suppress you are leaving something special behind message ... */
		if (p_ptr->sense_artifact)
		{
			p_ptr->sense_artifact = FALSE;	/* There may be more than one? */
			p_ptr->redraw |= PR_STATUS;
		}

		if (!(o_ptr->ident & (IDENT_SENSE)))
		{
			msg_format("You feel that the %s is %s...", o_name, game_inscriptions[FEEL_SPECIAL]);	
		
			o_ptr->ident |= (IDENT_SENSE);
			o_ptr->feeling = FEEL_SPECIAL;
		}
	}

	record_turn = turn;


	/* Check if completed a quest */
	for (i = 0; i < max_quests; i++)
	{
		if ((quest[i].type == QUEST_TYPE_FIND_ARTIFACT) &&
		    (quest[i].status == QUEST_STATUS_TAKEN) &&
			   (quest[i].k_idx == o_ptr->name1 || quest[i].k_idx == o_ptr->name3))
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, i, NULL);
			quest[i].status = QUEST_STATUS_COMPLETED;
			quest[i].complev = (byte)p_ptr->lev;
#ifdef JP
			msg_print("クエストを達成した！");
#else
			msg_print("You completed your quest!");
#endif

			msg_print(NULL);
		}
	}
}


/*
 * Player "wants" to pick up an object or gold.
 * Note that we ONLY handle things that can be picked up.
 * See "move_player()" for handling of other things.
 */
void carry(bool pickup)
{
	cave_type *c_ptr = &cave[py][px];

	s16b this_o_idx, next_o_idx = 0;

	char	o_name[MAX_NLEN];

	/* Recenter the map around the player */
	verify_panel();

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Automatically pickup/destroy/inscribe items */
	autopick_pickup_items(c_ptr);


#ifdef ALLOW_EASY_FLOOR

	if (easy_floor)
	{
		py_pickup_floor(pickup);
		return;
	}

#endif /* ALLOW_EASY_FLOOR */

	/* Scan the pile of objects */
	for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

#ifdef ALLOW_EASY_SENSE /* TNB */

		/* Option: Make item sensing easy */
		if (easy_sense)
		{
			/* Sense the object */
			(void)sense_object(o_ptr);
		}

#endif /* ALLOW_EASY_SENSE -- TNB */

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- disturb */
		disturb(0, 0);

		/* Pick up gold */
		if (o_ptr->tval == TV_GOLD)
		{
			int value = (long)o_ptr->pval;

			/* Delete the gold */
			delete_object_idx(this_o_idx);

			/* Message */
#ifdef JP
		msg_format(" $%ld の価値がある%sを見つけた。",
			   (long)value, o_name);
#else
			msg_format("You collect %ld gold pieces worth of %s.",
				   (long)value, o_name);
#endif


			sound(SOUND_SELL);

			/* Collect the gold */
			p_ptr->au += value;

			/* Redraw gold */
			p_ptr->redraw |= (PR_GOLD);

			/* Window stuff */
			p_ptr->window |= (PW_PLAYER);
		}

		/* Pick up objects */
		else
		{
			/* Hack - some objects were handled in autopick_pickup_items(). */
			if (o_ptr->marked & OM_NOMSG)
			{
				/* Clear the flag. */
				o_ptr->marked &= ~OM_NOMSG;
			}
			/* Describe the object */
			else if (!pickup)
			{
#ifdef JP
				msg_format("%sがある。", o_name);
#else
				msg_format("You see %s.", o_name);
#endif

			}

			/* Note that the pack is too full */
			else if (!inven_carry_okay(o_ptr))
			{
#ifdef JP
				msg_format("ザックには%sを入れる隙間がない。", o_name);
#else
				msg_format("You have no room for %s.", o_name);
#endif

			}

			/* Pick up the item (if requested and allowed) */
			else
			{
				int okay = TRUE;

				/* Hack -- query every item */
				if (carry_query_flag)
				{
					char out_val[MAX_NLEN+20];
#ifdef JP
					sprintf(out_val, "%sを拾いますか? ", o_name);
#else
					sprintf(out_val, "Pick up %s? ", o_name);
#endif

					okay = get_check(out_val);
				}

				/* Attempt to pick up an object. */
				if (okay)
				{
					/* Pick up the object */
					py_pickup_aux(this_o_idx);
				}
			}
		}
	}
}


/*
 * Determine if a trap affects the player.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match trap power against player armor.
 */
static int _check_hit(int power)
{
	int k, ac;

	/* Percentile dice */
	k = randint0(100);

	/* Hack -- 5% hit, 5% miss */
	if (k < 10) return (k < 5);

	if (p_ptr->personality == PERS_LAZY)
		if (one_in_(20)) return (TRUE);

	/* Paranoia -- No power */
	if (power <= 0) return (FALSE);

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;

	/* Power competes against Armor */
	if (randint1(power) > ((ac * 3) / 4)) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Handle player hitting a real trap
 */
static void hit_trap(bool break_trap)
{
	int i, num, dam;
	int x = px, y = py;

	/* Get the cave grid */
	cave_type *c_ptr = &cave[y][x];
	feature_type *f_ptr = &f_info[c_ptr->feat];
	int trap_feat_type = have_flag(f_ptr->flags, FF_TRAP) ? f_ptr->subtype : NOT_TRAP;

#ifdef JP
	cptr name = "トラップ";
#else
	cptr name = "a trap";
#endif

	/* Disturb the player */
	disturb(0, 0);

	cave_alter_feat(y, x, FF_HIT_TRAP);

	/* Analyze XXX XXX XXX */
	switch (trap_feat_type)
	{
		case TRAP_TRAPDOOR:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし戸を飛び越えた。");
#else
				msg_print("You fly over a trap door.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("落とし戸に落ちた！");
				if ((p_ptr->personality == PERS_COMBAT) || (inventory[INVEN_BOW].name1 == ART_CRIMSON))
					msg_print("くっそ〜！");
#else
				msg_print("You have fallen through a trap door!");
#endif

				sound(SOUND_FALL);
				dam = damroll(2, 8);
#ifdef JP
				name = "落とし戸";
#else
				name = "a trap door";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);

				/* Still alive and autosave enabled */
				if (autosave_l && (p_ptr->chp >= 0))
					do_cmd_save_game(TRUE);

#ifdef JP
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "落とし戸に落ちた");
#else
				do_cmd_write_nikki(NIKKI_BUNSHOU, 0, "You have fallen through a trap door!");
#endif
				prepare_change_floor_mode(CFM_SAVE_FLOORS | CFM_DOWN | CFM_RAND_PLACE | CFM_RAND_CONNECT);

				/* Leaving */
				p_ptr->leaving = TRUE;
			}
			break;
		}

		case TRAP_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("落とし穴を飛び越えた。");
#else
				msg_print("You fly over a pit trap.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("落とし穴に落ちてしまった！");
#else
				msg_print("You have fallen into a pit!");
#endif

				dam = damroll(2, 6);
#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif

				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case TRAP_SPIKED_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("トゲのある落とし穴を飛び越えた。");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
				msg_print("スパイクが敷かれた落とし穴に落ちてしまった！");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif

				dam = damroll(2, 6);

				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("スパイクが刺さった！");
#else
					msg_print("You are impaled!");
#endif


#ifdef JP
					name = "トゲのある落とし穴";
#else
					name = "a spiked pit";
#endif

					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam), FALSE);
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}
			break;
		}

		case TRAP_POISON_PIT:
		{
			if (p_ptr->levitation)
			{
#ifdef JP
				msg_print("トゲのある落とし穴を飛び越えた。");
#else
				msg_print("You fly over a spiked pit.");
#endif

			}
			else
			{
#ifdef JP
			msg_print("スパイクが敷かれた落とし穴に落ちてしまった！");
#else
				msg_print("You fall into a spiked pit!");
#endif


				/* Base damage */
				dam = damroll(2, 6);

#ifdef JP
				name = "落とし穴";
#else
				name = "a pit trap";
#endif


				/* Extra spike damage */
				if (randint0(100) < 50)
				{
#ifdef JP
					msg_print("毒を塗られたスパイクが刺さった！");
#else
					msg_print("You are impaled on poisonous spikes!");
#endif


#ifdef JP
					name = "トゲのある落とし穴";
#else
					name = "a spiked pit";
#endif


					dam = dam * 2;
					(void)set_cut(p_ptr->cut + randint1(dam), FALSE);

					if (p_ptr->resist_pois)
					{
#ifdef JP
						msg_print("しかし毒の影響はなかった！");
#else
						msg_print("The poison does not affect you!");
#endif

					}

					else
					{
						dam = dam * 2;
						(void)set_poisoned(p_ptr->poisoned + randint1(dam), FALSE);
					}
				}

				/* Take the damage */
				take_hit(DAMAGE_NOESCAPE, dam, name, -1);
			}

			break;
		}

		case TRAP_TY_CURSE:
		{
#ifdef JP
			msg_print("何かがピカッと光った！");
#else
			msg_print("There is a flash of shimmering light!");
#endif

			num = 2 + randint1(3);
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, 0, (PM_ALLOW_GROUP | PM_ALLOW_UNIQUE | PM_NO_PET));
			}

			if (dun_level > randint1(100)) /* No nasty effect for low levels */
			{
				bool stop_ty = FALSE;
				int count = 0;

				do
				{
					stop_ty = activate_ty_curse(stop_ty, &count);
				}
				while (one_in_(6));
			}
			break;
		}

		case TRAP_TELEPORT:
		{
#ifdef JP
			msg_print("テレポート・トラップにひっかかった！");
#else
			msg_print("You hit a teleport trap!");
#endif

			teleport_player(100, TELEPORT_PASSIVE);
			break;
		}

		case TRAP_FIRE:
		{
#ifdef JP
			msg_print("炎に包まれた！");
#else
			msg_print("You are enveloped in flames!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)fire_dam(dam, "炎のトラップ", -1);
#else
			(void)fire_dam(dam, "a fire trap", -1);
#endif

			break;
		}

		case TRAP_ACID:
		{
#ifdef JP
			msg_print("酸が吹きかけられた！");
#else
			msg_print("You are splashed with acid!");
#endif

			dam = damroll(4, 6);
#ifdef JP
			(void)acid_dam(dam, "酸のトラップ", -1);
#else
			(void)acid_dam(dam, "an acid trap", -1);
#endif

			break;
		}

		case TRAP_SLOW:
		{
			if (_check_hit(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)set_slow(p_ptr->slow + randint0(20) + 20, FALSE);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_STR:
		{
			if (_check_hit(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_STR);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_DEX:
		{
			if (_check_hit(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_DEX);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_LOSE_CON:
		{
			if (_check_hit(125))
			{
#ifdef JP
				msg_print("小さなダーツが飛んできて刺さった！");
#else
				msg_print("A small dart hits you!");
#endif

				dam = damroll(1, 4);
#ifdef JP
				take_hit(DAMAGE_ATTACK, dam, "ダーツの罠", -1);
#else
				take_hit(DAMAGE_ATTACK, dam, "a dart trap", -1);
#endif

				if (!CHECK_MULTISHADOW()) (void)do_dec_stat(A_CON);
			}
			else
			{
#ifdef JP
				msg_print("小さなダーツが飛んできた！が、運良く当たらなかった。");
#else
				msg_print("A small dart barely misses you.");
#endif

			}
			break;
		}

		case TRAP_BLIND:
		{
#ifdef JP
			msg_print("黒いガスに包み込まれた！");
#else
			msg_print("A black gas surrounds you!");
#endif

			if (!p_ptr->resist_blind)
			{
				(void)set_blind(p_ptr->blind + randint0(50) + 25, FALSE);
			}
			break;
		}

		case TRAP_CONFUSE:
		{
#ifdef JP
			msg_print("きらめくガスに包み込まれた！");
#else
			msg_print("A gas of scintillating colors surrounds you!");
#endif

			if (!p_ptr->resist_conf)
			{
				(void)set_confused(p_ptr->confused + randint0(20) + 10, FALSE);
			}
			break;
		}

		case TRAP_POISON:
		{
#ifdef JP
			msg_print("刺激的な緑色のガスに包み込まれた！");
#else
			msg_print("A pungent green gas surrounds you!");
#endif

			if (p_ptr->resist_pois <= 0)
			{
				(void)set_poisoned(p_ptr->poisoned + randint0(20) + 10, FALSE);
			}
			break;
		}

		case TRAP_SLEEP:
		{
#ifdef JP
			msg_print("奇妙な白い霧に包まれた！");
#else
			msg_print("A strange white mist surrounds you!");
#endif

			if (!p_ptr->free_act)
			{
#ifdef JP
msg_print("あなたは眠りに就いた。");
#else
				msg_print("You fall asleep.");
#endif


				if (ironman_nightmare)
				{
#ifdef JP
msg_print("身の毛もよだつ光景が頭に浮かんだ。");
#else
					msg_print("A horrible vision enters your mind.");
#endif


					/* Pick a nightmare */
					get_mon_num_prep(get_nightmare, NULL);

					/* Have some nightmares */
					have_nightmare(get_mon_num(MAX_DEPTH));

					/* Remove the monster restriction */
					get_mon_num_prep(NULL, NULL);
				}
				(void)set_paralyzed(p_ptr->paralyzed + randint0(10) + 5, FALSE);
			}
			break;
		}

		case TRAP_TRAPS:
		{
#ifdef JP
msg_print("まばゆい閃光が走った！");
#else
			msg_print("There is a bright flash of light!");
#endif

			/* Make some new traps */
			project(0, 1, y, x, 0, GF_MAKE_TRAP, PROJECT_HIDE | PROJECT_JUMP | PROJECT_GRID, -1);

			break;
		}

		case TRAP_ALARM:
		{
#ifdef JP
			msg_print("けたたましい音が鳴り響いた！");
#else
			msg_print("An alarm sounds!");
#endif

			aggravate_monsters(0);

			break;
		}

		case TRAP_OPEN:
		{
#ifdef JP
			msg_print("大音響と共にまわりの壁が崩れた！");
#else
			msg_print("Suddenly, surrounding walls are opened!");
#endif
			(void)project(0, 3, y, x, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x - 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			(void)project(0, 3, y, x + 4, 0, GF_DISINTEGRATE, PROJECT_GRID | PROJECT_HIDE, -1);
			aggravate_monsters(0);

			break;
		}

		case TRAP_ARMAGEDDON:
		{
			static int levs[10] = {0, 0, 20, 10, 5, 3, 2, 1, 1, 1};
			int evil_idx = 0, good_idx = 0;

			int lev;
#ifdef JP
			msg_print("突然天界の戦争に巻き込まれた！");
#else
			msg_print("Suddenly, you are surrounded by immotal beings!");
#endif

			/* Summon Demons and Angels */
			for (lev = dun_level; lev >= 20; lev -= 1 + lev/16)
			{
				num = levs[MIN(lev/10, 9)];
				for (i = 0; i < num; i++)
				{
					int x1 = rand_spread(x, 7);
					int y1 = rand_spread(y, 5);

					/* Skip illegal grids */
					if (!in_bounds(y1, x1)) continue;

					/* Require line of projection */
					if (!projectable(py, px, y1, x1)) continue;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_EVIL, (PM_NO_PET)))
						evil_idx = hack_m_idx_ii;

					if (summon_specific(0, y1, x1, lev, SUMMON_ARMAGE_GOOD, (PM_NO_PET)))
					{
						good_idx = hack_m_idx_ii;
					}

					/* Let them fight each other */
					if (evil_idx && good_idx)
					{
						monster_type *evil_ptr = &m_list[evil_idx];
						monster_type *good_ptr = &m_list[good_idx];
						evil_ptr->target_y = good_ptr->fy;
						evil_ptr->target_x = good_ptr->fx;
						good_ptr->target_y = evil_ptr->fy;
						good_ptr->target_x = evil_ptr->fx;
					}
				}
			}
			break;
		}

		case TRAP_PIRANHA:
		{
#ifdef JP
			msg_print("突然壁から水が溢れ出した！ピラニアがいる！");
#else
			msg_print("Suddenly, the room is filled with water with piranhas!");
#endif

			/* Water fills room */
			fire_ball_hide(GF_WATER_FLOW, 0, 1, 10);

			/* Summon Piranhas */
			num = 1 + dun_level/20;
			for (i = 0; i < num; i++)
			{
				(void)summon_specific(0, y, x, dun_level, SUMMON_PIRANHAS, (PM_ALLOW_GROUP | PM_NO_PET));
			}
			break;
		}
	}

	if (break_trap && is_trap(c_ptr->feat))
	{
		cave_alter_feat(y, x, FF_DISARM);
#ifdef JP
		msg_print("トラップを粉砕した。");
#else
		msg_print("You destroyed the trap.");
#endif
	}
}


void touch_zap_player(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	if (r_ptr->flags2 & RF2_AURA_REVENGE)
	{
		if (p_ptr->lightning_reflexes)
		{
			msg_print("You strike so fast that you avoid a retaliatory strike.");
		}
		else
		{
			retaliation_hack = TRUE;
			make_attack_normal(m_idx);
			retaliation_count++; /* Indexes which blow to use per retaliation, but start at 0 ... See py_attack() for initialization.*/
			retaliation_hack = FALSE;
		}
	}

	if (r_ptr->flags2 & RF2_AURA_FIRE)
	{
		if (p_ptr->lightning_reflexes)
		{
			msg_print("You strike so fast that you avoid getting burned.");
		}
		else
		{
			int dam = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
			dam -= dam * p_ptr->resist_fire / 100;

			if (dam > 0)
			{
				char buf[80];
				

				monster_desc(buf, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	#ifdef JP
				msg_print("突然とても熱くなった！");
	#else
				msg_print("You are suddenly very hot!");
	#endif

				take_hit(DAMAGE_NOESCAPE, dam, buf, -1);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_FIRE;
				handle_stuff();
			}
		}
	}

	if (r_ptr->flags3 & RF3_AURA_COLD)
	{
		if (p_ptr->lightning_reflexes)
		{
			msg_print("You strike so fast that you avoid getting frozen.");
		}
		else
		{
			int dam = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
			dam -= dam * p_ptr->resist_cold / 100;

			if (dam > 0)
			{
				char buf[80];

				monster_desc(buf, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	#ifdef JP
				msg_print("突然とても寒くなった！");
	#else
				msg_print("You are suddenly very cold!");
	#endif

				take_hit(DAMAGE_NOESCAPE, dam, buf, -1);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_AURA_COLD;
				handle_stuff();
			}
		}
	}

	if (r_ptr->flags2 & RF2_AURA_ELEC)
	{
		if (p_ptr->lightning_reflexes)
		{
			msg_print("You strike so fast that you avoid getting zapped.");
		}
		else
		{
			int dam = damroll(1 + (r_ptr->level / 26), 1 + (r_ptr->level / 17));
			dam -= dam * p_ptr->resist_elec / 100;

			if (dam > 0)
			{
				char buf[80];

				monster_desc(buf, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	#ifdef JP
				msg_print("電撃をくらった！");
	#else
				msg_print("You get zapped!");
	#endif

				take_hit(DAMAGE_NOESCAPE, dam, buf, -1);
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags2 |= RF2_AURA_ELEC;
				handle_stuff();
			}
		}
	}
}


static void natural_attack(s16b m_idx, int attack, bool *fear, bool *mdeath)
{
	int             k, bonus, chance;
	int             n_weight = 0;
	monster_type    *m_ptr = &m_list[m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	int             dss, ddd;

	cptr            atk_desc;
	critical_t      crit;

	switch (attack)
	{
		case MUT_SCORPION_TAIL:
			dss = 3;
			ddd = 7;
			n_weight = 5;
#ifdef JP
			atk_desc = "尻尾";
#else
			atk_desc = "tail";
#endif

			break;
		case MUT_HORNS:
			dss = 2;
			ddd = 6;
			n_weight = 15;
#ifdef JP
			atk_desc = "角";
#else
			atk_desc = "horns";
#endif

			break;
		case MUT_BEAK:
			dss = 2;
			ddd = 4;
			n_weight = 5;
#ifdef JP
			atk_desc = "クチバシ";
#else
			atk_desc = "beak";
#endif

			break;
		case MUT_TRUNK:
			dss = 1;
			ddd = 4;
			n_weight = 35;
#ifdef JP
			atk_desc = "象の鼻";
#else
			atk_desc = "trunk";
#endif

			break;
		case MUT_TENTACLES:
			dss = 2;
			ddd = 5;
			n_weight = 5;
#ifdef JP
			atk_desc = "触手";
#else
			atk_desc = "tentacles";
#endif

			break;
		default:
			dss = ddd = n_weight = 1;
#ifdef JP
			atk_desc = "未定義の部位";
#else
			atk_desc = "undefined body part";
#endif

	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);


	/* Calculate the "attack quality" */
	bonus = p_ptr->to_h_m;
	bonus += (p_ptr->lev * 6 / 5);
	chance = (p_ptr->skills.thn + (bonus * BTH_PLUS_ADJ));

	/* Test for hit */
	if ((!(r_ptr->flags2 & RF2_QUANTUM) || !randint0(2)) && test_hit_norm(chance, MON_AC(r_ptr, m_ptr), m_ptr->ml))
	{
		/* Sound */
		sound(SOUND_HIT);

#ifdef JP
		msg_format("%sを%sで攻撃した。", m_name, atk_desc);
#else
		msg_format("You hit %s with your %s.", m_name, atk_desc);
#endif


		k = damroll(ddd, dss);
		crit = critical_norm(n_weight, bonus, (s16b)bonus, 0);
		if (crit.desc)
		{
			k = k * crit.mul/100 + crit.to_d;
			msg_print(crit.desc);
		}

		/* Apply the player damage bonuses */
		k += p_ptr->to_d_m;

		/* No negative damage */
		if (k < 0) k = 0;

		/* Modify the damage */
		k = mon_damage_mod(m_ptr, k, FALSE);

		/* Hack: Monster AC now reduces damage */
		k -= (k * ((MON_AC(r_ptr, m_ptr) < 200) ? MON_AC(r_ptr, m_ptr) : 200) / 1200);

		/* Anger the monster */
		if (k > 0) anger_monster(m_ptr);

		/* Damage, check for fear and mdeath */
		switch (attack)
		{
			case MUT_SCORPION_TAIL:
				project(0, 0, m_ptr->fy, m_ptr->fx, k, GF_POIS, PROJECT_KILL, -1);
				*mdeath = (m_ptr->r_idx == 0);
				break;
			case MUT_HORNS:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT_BEAK:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT_TRUNK:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			case MUT_TENTACLES:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
				break;
			default:
				*mdeath = mon_take_hit(m_idx, k, fear, NULL);
		}

		touch_zap_player(m_idx);
	}
	/* Player misses */
	else
	{
		/* Sound */
		sound(SOUND_MISS);

		/* Message */
#ifdef JP
			msg_format("ミス！ %sにかわされた。", m_name);
#else
		msg_format("You miss %s.", m_name);
#endif

	}
}

/*
 * Player attacks a (poor, defenseless) creature        -RAK-
 *
 * If no "weapon" is available, then "punch" the monster one time.
 */

static int drain_left = MAX_VAMPIRIC_DRAIN;
bool melee_hack = FALSE;
static bool fear_stop = FALSE;

static int calculate_dir(int sx, int sy, int tx, int ty)
{
	int dir;
	for (dir = 0; dir <= 9; dir++)
	{
		int x = sx + ddx[dir];
		int y = sy + ddy[dir];

		if (x == tx && y == ty) return dir;
	}
	return 5;
}

static int get_next_dir(int dir)
{
	switch (dir)
	{
	case 1: return 4;
	case 4: return 7;
	case 7: return 8;
	case 8: return 9;
	case 9: return 6;
	case 6: return 3;
	case 3: return 2;
	case 2: return 1;
	}
	return 5;
}

static bool py_attack_aux(int y, int x, bool *fear, bool *mdeath, s16b hand, int mode)
{
	int		num = 0, k, bonus, chance, vir;
	int     to_h = 0, to_d = 0;
	int     touch_ct = 0;
	critical_t       crit;
	cave_type       *c_ptr = &cave[y][x];

	monster_type    *m_ptr = 0;
	monster_race    *r_ptr = 0;

	/* Access the weapon */
	object_type     *o_ptr = &inventory[INVEN_RARM + hand];

	char            m_name[MAX_NLEN];
	char			o_name[MAX_NLEN];

	bool            success_hit = FALSE;
	bool            backstab = FALSE;
	bool            vorpal_cut = FALSE;
	int             chaos_effect = 0;
	bool            stab_fleeing = FALSE;
	bool            fuiuchi = FALSE;
	bool            monk_attack = FALSE;
	bool			duelist_attack = FALSE;
	bool            do_quake = FALSE;
	bool            weak = FALSE;
	bool            drain_msg = TRUE;
	int             drain_result = 0, drain_heal = 0;
	bool            can_drain = FALSE;
	int             num_blow;
	u32b flgs[TR_FLAG_SIZE]; /* A massive hack -- life-draining weapons */
	bool            is_human;
	bool            is_lowlevel;
	bool            zantetsu_mukou, e_j_mukou;
	int				knock_out = 0;
	int				dd, ds;
	bool			hit_ct = 0;

	if (!c_ptr->m_idx)
	{
		msg_print("You swing wildly at nothing.");
		return FALSE;
	}

	m_ptr = &m_list[c_ptr->m_idx];
	r_ptr = &r_info[m_ptr->r_idx];
	is_human = (r_ptr->d_char == 'p');
	is_lowlevel = (r_ptr->level < (p_ptr->lev - 15));

	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	if (p_ptr->painted_target)
	{
		p_ptr->painted_target_idx = 0;
		p_ptr->painted_target_ct = 0;
	}

	if (weaponmaster_get_toggle() == TOGGLE_SHIELD_BASH)
	{
		dd = 3;
		ds = k_info[o_ptr->k_idx].ac;
		to_h = o_ptr->to_a;
		to_d = o_ptr->to_a;

		to_h += 2*o_ptr->to_h;
		to_d += 2*o_ptr->to_d;
	}
	else
	{
		dd = o_ptr->dd;
		ds = o_ptr->ds;
		to_h = o_ptr->to_h;
		to_d = o_ptr->to_d;
	}

	switch (p_ptr->pclass)
	{
	case CLASS_DUELIST:
		if (p_ptr->pclass == CLASS_DUELIST && c_ptr->m_idx == p_ptr->duelist_target_idx)
			duelist_attack = TRUE;
		break;

	case CLASS_WEAPONMASTER:
		if (!p_ptr->sneak_attack)
			break;
		/* vvvvv FALL THRU vvvvvv */
	case CLASS_ROGUE:
	case CLASS_NINJA:
		if (buki_motteruka(INVEN_RARM + hand) && !p_ptr->weapon_info[hand].icky_wield)
		{
			int tmp = p_ptr->lev * 6 + (p_ptr->skills.stl + 10) * 4;
			if (p_ptr->monlite && (mode != HISSATSU_NYUSIN)) tmp /= 3;
			if (p_ptr->cursed & TRC_AGGRAVATE) tmp /= 2;
			if (r_ptr->level > (p_ptr->lev * p_ptr->lev / 20 + 10)) tmp /= 3;
			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
			{
				/* Can't backstab creatures that we can't see, right? */
				backstab = TRUE;
			}
			else if ((p_ptr->special_defense & NINJA_S_STEALTH) && (randint0(tmp) > (r_ptr->level+20)) && m_ptr->ml && !(r_ptr->flagsr & RFR_RES_ALL))
			{
				fuiuchi = TRUE;
			}
			else if (MON_MONFEAR(m_ptr) && m_ptr->ml)
			{
				stab_fleeing = TRUE;
			}
		}
		break;

	case CLASS_SCOUT:
		if (p_ptr->ambush && buki_motteruka(INVEN_RARM + hand) && !p_ptr->weapon_info[hand].icky_wield)
		{
			if (MON_CSLEEP(m_ptr) && m_ptr->ml)
				backstab = TRUE;
		}
		break;

	case CLASS_MONK:
	case CLASS_FORCETRAINER:
	case CLASS_BERSERKER:
		if ((empty_hands(TRUE) & EMPTY_HAND_RARM) && !p_ptr->riding) monk_attack = TRUE;
		break;
	}

	if (!o_ptr->k_idx) /* Empty hand */
	{
		if ((r_ptr->level + 10) > p_ptr->lev)
		{
			if (p_ptr->skill_exp[GINOU_SUDE] < s_info[p_ptr->pclass].s_max[GINOU_SUDE])
			{
				if (p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_BEGINNER)
					p_ptr->skill_exp[GINOU_SUDE] += 40;
				else if ((p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_SKILLED))
					p_ptr->skill_exp[GINOU_SUDE] += 5;
				else if ((p_ptr->skill_exp[GINOU_SUDE] < WEAPON_EXP_EXPERT) && (p_ptr->lev > 19))
					p_ptr->skill_exp[GINOU_SUDE] += 1;
				else if ((p_ptr->lev > 34))
					if (one_in_(3)) p_ptr->skill_exp[GINOU_SUDE] += 1;
				p_ptr->update |= (PU_BONUS);
			}
		}
	}
	else if (object_is_melee_weapon(o_ptr))
	{
		if ((r_ptr->level + 10) > p_ptr->lev)
			skills_weapon_gain(inventory[INVEN_RARM+hand].tval, inventory[INVEN_RARM+hand].sval);
	}

	/* Disturb the monster */
	set_monster_csleep(c_ptr->m_idx, 0);

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Calculate the "attack quality" */
	bonus = p_ptr->weapon_info[hand].to_h + to_h;
	if (mode == WEAPONMASTER_ENCLOSE) bonus -= 10;
	if (mode == WEAPONMASTER_KNOCK_BACK) bonus -= 20;
	if (mode == WEAPONMASTER_REAPING) bonus -= 40;
	if (mode == WEAPONMASTER_CUNNING_STRIKE) bonus += 20;
	if (mode == WEAPONMASTER_SMITE_EVIL && hand == 0 && (r_ptr->flags3 & RF3_EVIL)) bonus += 200;

	chance = (p_ptr->skills.thn + (bonus * BTH_PLUS_ADJ));
	if (mode == HISSATSU_IAI) chance += 60;
	if (p_ptr->special_defense & KATA_KOUKIJIN) chance += 150;

	if (p_ptr->sutemi) chance = MAX(chance * 3 / 2, chance + 60);

	vir = virtue_number(V_VALOUR);
	if (vir)
	{
		chance += (p_ptr->virtues[vir - 1]/10);
	}

	zantetsu_mukou = ((o_ptr->name1 == ART_ZANTETSU) && (r_ptr->d_char == 'j'));
	e_j_mukou = ((o_ptr->name1 == ART_EXCALIBUR_J) && (r_ptr->d_char == 'S'));

	if ((mode == HISSATSU_KYUSHO) || (mode == HISSATSU_MINEUCHI) || (mode == HISSATSU_3DAN) || (mode == HISSATSU_IAI)) num_blow = 1;
	else if (mode == HISSATSU_COLD) num_blow = p_ptr->weapon_info[hand].num_blow+2;
	else num_blow = p_ptr->weapon_info[hand].num_blow;

	if (mode == WEAPONMASTER_CUNNING_STRIKE) num_blow = (num_blow + 1)/2;

	/* Hack -- DOKUBARI always hit once 
	   Note, this is set in calc_bonuses(), but the mode of attack (e.g Samurai's +2 attack cold strike)
	   can alter the number of blows, so we need to duplicate these checks here!
	*/
	if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) num_blow = 1;
	if (o_ptr->name1 == ART_EVISCERATOR) num_blow = 1;

	/* Attack once for each legal blow */
	while ((num++ < num_blow) && !p_ptr->is_dead)
	{
	bool do_whirlwind = FALSE;

		/* We now check fear on every blow, and only lose energy equal to the number of blows attempted.
		   Monsters with AURA_FEAR can induce fear any time the player damages them!
		 */
		if (p_ptr->afraid)
		{
			if (!fear_allow_melee(c_ptr->m_idx))
			{
				if (m_ptr->ml)
					msg_format(T("You are too afraid to attack %s!", "恐くて%sを攻撃できない！"), m_name);
				else
					msg_format (T("There is something scary in your way!", "そっちには何か恐いものがいる！"));

				fear_stop = TRUE;
				if (p_ptr->migite && p_ptr->hidarite)
				{
					if (hand) energy_use = energy_use*3/5+energy_use*num*2/(p_ptr->weapon_info[hand].num_blow*5);
					else energy_use = energy_use*num*3/(p_ptr->weapon_info[hand].num_blow*5);
				}
				else
				{
					energy_use = energy_use*num/p_ptr->weapon_info[hand].num_blow;
				}
				break;
			}
		}

		/* Weaponmaster Whirlwind turns a normal strike into a sweeping whirlwind strike */
		if (p_ptr->whirlwind && mode == 0)
		{
			if (one_in_(5))
				do_whirlwind = TRUE;
		}

		if (((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
		{
			int n = 1;

			if (p_ptr->migite && p_ptr->hidarite)
			{
				n *= 2;
			}
			if (mode == HISSATSU_3DAN)
			{
				n *= 2;
			}

			success_hit = one_in_(n);
		}
		else if ((p_ptr->pclass == CLASS_NINJA) && ((backstab || fuiuchi) && !(r_ptr->flagsr & RFR_RES_ALL))) success_hit = TRUE;
		else if (duelist_attack) success_hit = TRUE;
		else if (weaponmaster_get_toggle() == TOGGLE_BURNING_BLADE) success_hit = TRUE;
		else success_hit = test_hit_norm(chance, MON_AC(r_ptr, m_ptr), m_ptr->ml);

		if (mode == HISSATSU_MAJIN)
		{
			if (one_in_(2))
				success_hit = FALSE;
		}

		/* Test for hit */
		if (success_hit)
		{
			int vorpal_chance = 
				((o_ptr->name1 == ART_VORPAL_BLADE) || 
				 (o_ptr->name1 == ART_CHAINSWORD) ||
				 (o_ptr->name1 == ART_MURAMASA)) ? 2 : 4;

			if (o_ptr->name1 == ART_EVISCERATOR && !duelist_attack) num_blow++;

			hit_ct++;
			sound(SOUND_HIT);

			/* Uber mega-hack ... aren't they all?! */
			if (strcmp(weaponmaster_speciality1_name(), "Staves") == 0)
			{
				bool update = FALSE;
				if (p_ptr->elaborate_defense == 0) update = TRUE;
				p_ptr->elaborate_defense = 1;
				if (update)
				{
					p_ptr->update |= PU_BONUS;
					handle_stuff();
				}
			}

			/* Message */
#ifdef JP
			if (backstab) msg_format("あなたは冷酷にも眠っている無力な%sを突き刺した！", m_name);
			else if (fuiuchi) msg_format("不意を突いて%sに強烈な一撃を喰らわせた！", m_name);
			else if (stab_fleeing) msg_format("逃げる%sを背中から突き刺した！", m_name);
			else if (!monk_attack) msg_format("%sを攻撃した。", m_name);
#else
			if (backstab) msg_format("You cruelly stab %s!", m_name);
			else if (fuiuchi) msg_format("You make surprise attack, and hit %s with a powerful blow!", m_name);
			else if (stab_fleeing) msg_format("You backstab %s!",  m_name);
			else if (duelist_attack) msg_format("You land a perfect strike against %s.", m_name);
			else if (!monk_attack) msg_format("You hit %s.", m_name);
#endif

			/* Hack -- bare hands do one damage */
			k = 1;

			object_flags(o_ptr, flgs);

			if ((have_flag(flgs, TR_CHAOTIC)) && one_in_(7))
			{
				if (one_in_(10))
				chg_virtue(V_CHANCE, 1);

				if (randint1(5) < 4)
				{
					chaos_effect = 1;
				}
				else
				{
					if (one_in_(5))
						chaos_effect = 3;
				}
			}

			/* Vampiric drain */
			if ((have_flag(flgs, TR_VAMPIRIC)) 
			 || (chaos_effect == 1) 
			 || (mode == HISSATSU_DRAIN) 
			 || hex_spelling(HEX_VAMP_BLADE)
			 || weaponmaster_get_toggle() == TOGGLE_BLOOD_BLADE )
			{
				/* Only drain "living" monsters */
				if (monster_living(r_ptr))
					can_drain = TRUE;
				else
					can_drain = FALSE;
			}

			vorpal_cut = FALSE;
			if (!zantetsu_mukou) /* No jelly cuts with Zantetsuken */
			{
				if (have_flag(flgs, TR_VORPAL) && p_ptr->vorpal && vorpal_chance > 3) vorpal_chance = 3;
				if (have_flag(flgs, TR_VORPAL) || hex_spelling(HEX_RUNESWORD) || p_ptr->vorpal)
				{
					if (randint1(vorpal_chance*3/2) == 1)
						vorpal_cut = TRUE;
				}
			}

			if (monk_attack)
			{
				int special_effect = 0, stun_effect = 0;
				martial_arts *ma_ptr = &ma_blows[monk_get_attack_idx()];
				int resist_stun = 0;

				if (r_ptr->flags1 & RF1_UNIQUE) resist_stun += (10*r_ptr->level);
				if (r_ptr->flags3 & RF3_NO_CONF) resist_stun += 33;
				if (r_ptr->flags3 & RF3_NO_SLEEP) resist_stun += 33;
				if ((r_ptr->flags3 & RF3_UNDEAD) || (r_ptr->flags3 & RF3_NONLIVING))
					resist_stun += 66;

				k = damroll(ma_ptr->dd + p_ptr->weapon_info[hand].to_dd, ma_ptr->ds + p_ptr->weapon_info[hand].to_ds);
				k = tot_dam_aux_monk(k, m_ptr);

				if (p_ptr->special_attack & ATTACK_SUIKEN) k *= 2; /* Drunken Boxing! */

				if (ma_ptr->effect == MA_KNEE)
				{
					if (r_ptr->flags1 & RF1_MALE)
					{
#ifdef JP
						msg_format("%sに金的膝蹴りをくらわした！", m_name);
#else
						msg_format("You hit %s in the groin with your knee!", m_name);
#endif

						sound(SOUND_PAIN);
						special_effect = MA_KNEE;
					}
					else
						msg_format(ma_ptr->desc, m_name);
				}

				else if (ma_ptr->effect == MA_SLOW)
				{
					if (!((r_ptr->flags1 & RF1_NEVER_MOVE) ||
					    my_strchr("~#{}.UjmeEv$,DdsbBFIJQSXclnw!=?", r_ptr->d_char)))
					{
#ifdef JP
						msg_format("%sの足首に関節蹴りをくらわした！", m_name);
#else
						msg_format("You kick %s in the ankle.", m_name);
#endif

						special_effect = MA_SLOW;
					}
					else msg_format(ma_ptr->desc, m_name);
				}
				else
				{
					if (ma_ptr->effect)
					{
						stun_effect = (ma_ptr->effect / 2) + randint1(ma_ptr->effect / 2);
					}

					msg_format(ma_ptr->desc, m_name);
				}


				crit = monk_get_critical(ma_ptr);
				if (crit.desc)
				{
					k = k * crit.mul/100 + crit.to_d;
					msg_print(crit.desc);
				}

				if ((special_effect == MA_KNEE) && ((k + p_ptr->weapon_info[hand].to_d) < m_ptr->hp))
				{
#ifdef JP
					msg_format("%^sは苦痛にうめいている！", m_name);
#else
					msg_format("%^s moans in agony!", m_name);
#endif

					stun_effect = 7 + randint1(13);
					resist_stun /= 3;
				}

				else if ((special_effect == MA_SLOW) && ((k + p_ptr->weapon_info[hand].to_d) < m_ptr->hp))
				{
					if (!(r_ptr->flags1 & RF1_UNIQUE) &&
					    (randint1(p_ptr->lev) > r_ptr->level) &&
					    m_ptr->mspeed > 60)
					{
#ifdef JP
						msg_format("%^sは足をひきずり始めた。", m_name);
#else
						msg_format("%^s starts limping slower.", m_name);
#endif

						m_ptr->mspeed -= 10;
					}
				}

				/* Massive Hack: Monk stunning is now greatly biffed! */
				if (r_ptr->flags1 & RF1_UNIQUE) stun_effect /= 2;
				if (r_ptr->flags3 & RF3_NO_STUN) stun_effect = 0;
				if (r_ptr->flagsr & RFR_RES_SOUN) stun_effect = 0;

				if (mon_save_p(m_ptr->r_idx, A_DEX))
					stun_effect = 0;

				if (stun_effect && ((k + p_ptr->weapon_info[hand].to_d) < m_ptr->hp))
				{
					if (p_ptr->lev > randint1(r_ptr->level + resist_stun + 10))
					{
						if (MON_STUNNED(m_ptr))
							stun_effect /= 4;

						if (stun_effect == 0)
						{
							/* No message */
						}
						else if (set_monster_stunned(c_ptr->m_idx, stun_effect + MON_STUNNED(m_ptr)))
						{
#ifdef JP
							msg_format("%^sはフラフラになった。", m_name);
#else
							msg_format("%^s is stunned.", m_name);
#endif
						}
						else
						{
#ifdef JP
							msg_format("%^sはさらにフラフラになった。", m_name);
#else
							msg_format("%^s is more stunned.", m_name);
#endif
						}
					}
				}
			}

			/* Handle normal weapon */
			else if (o_ptr->k_idx)
			{
				if (weaponmaster_get_toggle() == TOGGLE_ORDER_BLADE)
					k = (dd + p_ptr->weapon_info[hand].to_dd) * (ds + p_ptr->weapon_info[hand].to_ds);
				else
					k = damroll(dd + p_ptr->weapon_info[hand].to_dd, ds + p_ptr->weapon_info[hand].to_ds);
				k = tot_dam_aux(o_ptr, k, m_ptr, hand, mode, FALSE);

				if (backstab)
				{
					if (p_ptr->pclass == CLASS_SCOUT)
						k *= 3;
					else
						k *= (3 + (p_ptr->lev / 20));
				}
				else if (fuiuchi)
				{
					k = k*(5+(p_ptr->lev*2/25))/2;
				}
				else if (stab_fleeing)
				{
					k = (3 * k) / 2;
				}

				if ((p_ptr->impact[hand] && ((k > 50) || one_in_(7))) ||
					 (chaos_effect == 2) || (mode == HISSATSU_QUAKE))
				{
					do_quake = TRUE;
				}

				if ((!(o_ptr->tval == TV_SWORD) || !(o_ptr->sval == SV_DOKUBARI)) 
				 && !(mode == HISSATSU_KYUSHO)
				 && weaponmaster_get_toggle() != TOGGLE_ORDER_BLADE 
				 && !have_flag(flgs, TR_ORDER) )
				{
					int bonus = 0;
					if (mode == WEAPONMASTER_SMITE_EVIL && hand == 0 && (r_ptr->flags3 & RF3_EVIL)) bonus = 200;
					crit = critical_norm(o_ptr->weight, to_h, p_ptr->weapon_info[hand].to_h + bonus, mode);
					if (crit.desc)
					{
						k = k * crit.mul/100 + crit.to_d;
						msg_print(crit.desc);
					}
				}

				drain_result = k;

				if (vorpal_cut)
				{
					int mult = 2;

					if ((o_ptr->name1 == ART_CHAINSWORD) && !one_in_(2))
					{
						char chainsword_noise[1024];
#ifdef JP
						if (!get_rnd_line("chainswd_j.txt", 0, chainsword_noise))
#else
						if (!get_rnd_line("chainswd.txt", 0, chainsword_noise))
#endif
						{
							msg_print(chainsword_noise);
						}
					}

					if (o_ptr->name1 == ART_VORPAL_BLADE)
					{
#ifdef JP
						msg_print("目にも止まらぬヴォーパルブレード、手錬の早業！");
#else
						msg_print("Your Vorpal Blade goes snicker-snack!");
#endif
					}
					else
					{
#ifdef JP
						msg_format("%sをグッサリ切り裂いた！", m_name);
#else
						msg_format("Your weapon cuts deep into %s!", m_name);
#endif
					}

					/* Try to increase the damage */
					while (one_in_(vorpal_chance))
					{
						mult++;
					}

					k *= mult;

					/* Ouch! */
					if (((r_ptr->flagsr & RFR_RES_ALL) ? k/100 : k) > m_ptr->hp)
					{
#ifdef JP
						msg_format("%sを真っ二つにした！", m_name);
#else
						msg_format("You cut %s in half!", m_name);
#endif
					}
					else
					{
						switch (mult)
						{
#ifdef JP
						case 2: msg_format("%sを斬った！", m_name); break;
						case 3: msg_format("%sをぶった斬った！", m_name); break;
						case 4: msg_format("%sをメッタ斬りにした！", m_name); break;
						case 5: msg_format("%sをメッタメタに斬った！", m_name); break;
						case 6: msg_format("%sを刺身にした！", m_name); break;
						case 7: msg_format("%sを斬って斬って斬りまくった！", m_name); break;
						default: msg_format("%sを細切れにした！", m_name); break;
#else
						case 2: msg_format("You gouge %s!", m_name); break;
						case 3: msg_format("You maim %s!", m_name); break;
						case 4: msg_format("You carve %s!", m_name); break;
						case 5: msg_format("You cleave %s!", m_name); break;
						case 6: msg_format("You smite %s!", m_name); break;
						case 7: msg_format("You eviscerate %s!", m_name); break;
						default: msg_format("You shred %s!", m_name); break;
#endif
						}
					}
					drain_result = drain_result * 3 / 2;
				}

				k += to_d;
				drain_result += to_d;
			}

			/* Apply the player damage bonuses */
			k += p_ptr->weapon_info[hand].to_d;
			drain_result += p_ptr->weapon_info[hand].to_d;

			if ((mode == HISSATSU_SUTEMI) || (mode == HISSATSU_3DAN)) k *= 2;
			if ((mode == HISSATSU_SEKIRYUKA) && !monster_living(r_ptr)) k = 0;
			if ((mode == HISSATSU_SEKIRYUKA) && !p_ptr->cut) k /= 2;

			/* No negative damage */
			if (k < 0) k = 0;

			if ((mode == HISSATSU_ZANMA) && !(!monster_living(r_ptr) && (r_ptr->flags3 & RF3_EVIL)))
			{
				k = 0;
			}

			if (zantetsu_mukou)
			{
#ifdef JP
				msg_print("こんな軟らかいものは切れん！");
#else
				msg_print("You cannot cut such a elastic thing!");
#endif
				k = 0;
			}

			if (e_j_mukou)
			{
#ifdef JP
				msg_print("蜘蛛は苦手だ！");
#else
				msg_print("Spiders are difficult for you to deal with!");
#endif
				k /= 2;
			}

			if (mode == HISSATSU_MINEUCHI)
			{
				int tmp = (10 + randint1(15) + p_ptr->lev / 5);

				k = 0;
				anger_monster(m_ptr);

				if (!(r_ptr->flags3 & (RF3_NO_STUN)))
				{
					/* Get stunned */
					if (MON_STUNNED(m_ptr))
					{
#ifdef JP
						msg_format("%sはひどくもうろうとした。", m_name);
#else
						msg_format("%s is more dazed.", m_name);
#endif

						tmp /= 2;
					}
					else
					{
#ifdef JP
						msg_format("%s はもうろうとした。", m_name);
#else
						msg_format("%s is dazed.", m_name);
#endif
					}

					/* Apply stun */
					(void)set_monster_stunned(c_ptr->m_idx, MON_STUNNED(m_ptr) + tmp);
				}
				else
				{
#ifdef JP
					msg_format("%s には効果がなかった。", m_name);
#else
					msg_format("%s is not effected.", m_name);
#endif
				}
			}

			/* Modify the damage */
			k = mon_damage_mod(
				m_ptr, 
				k, 
				(o_ptr->tval == TV_POLEARM && o_ptr->sval == SV_DEATH_SCYTHE) 
				|| (p_ptr->pclass == CLASS_BERSERKER && one_in_(2))
				|| mode == WEAPONMASTER_CRUSADERS_STRIKE
			);

			/* Hack: Monster AC now reduces damage */
			{
				int ac = MON_AC(r_ptr, m_ptr); 
				int adj;
				ac = MIN(ac, 200);
				adj = k * ac / 1200;
				k -= adj;
			}

			if (duelist_attack)
			{
				/* Duelist: Careful Aim */
				if (duelist_attack && 
					p_ptr->lev >= 10) 
				{
					k = k * 2;
				}
				if ( p_ptr->lev >= 15	/* Hamstring */
					&& !(r_ptr->flags1 & (RF1_UNIQUE))
					&& !mon_save_p(m_ptr->r_idx, A_DEX) )
				{
					msg_format("You hamstring %s.", m_name);
					set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 50);
				}
				if ( p_ptr->lev >= 20	/* Wounding Strike */
				    && !mon_save_p(m_ptr->r_idx, A_DEX) )
				{
					msg_format("%^s is dealt a wounding strike.", m_name);
					k += MIN(m_ptr->hp / 5, p_ptr->lev * 10);
					drain_result = k;
				}
				if ( p_ptr->lev >= 25	/* Stunning Blow */
				    && !(r_ptr->flags3 & (RF3_NO_STUN))
					&& !mon_save_p(m_ptr->r_idx, A_DEX) )
				{
					msg_format("%^s is dealt a stunning blow.", m_name);
					set_monster_stunned(c_ptr->m_idx, MAX(MON_STUNNED(m_ptr), 2));
				}
				if ( p_ptr->lev >= 40	/* Greater Wounding Strike */
				    && !mon_save_p(m_ptr->r_idx, A_DEX) )
				{
					msg_format("%^s is dealt a *WOUNDING* strike.", m_name);
					k += MIN(m_ptr->hp * 2 / 5, (p_ptr->lev - 20) * 50);
					drain_result = k;
				}
			}
			else if (p_ptr->pclass == CLASS_DUELIST && p_ptr->lev >= 30)
			{
				/* Duelist: Careful Aim vs a non-target */
				k = k * 3 / 2;
			}

			if (((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) || (mode == HISSATSU_KYUSHO))
			{
				if ((randint1(randint1(r_ptr->level/7)+5) == 1) && (!(r_ptr->flags1 & RF1_UNIQUE) || m_ptr->r_idx == MON_HAGURE2) && !(r_ptr->flags7 & RF7_UNIQUE2))
				{
					k = m_ptr->hp + 1;
#ifdef JP
					msg_format("%sの急所を突き刺した！", m_name);
#else
					msg_format("You hit %s on a fatal spot!", m_name);
#endif
				}
				else k = 1;
			}
			else if ((p_ptr->pclass == CLASS_NINJA) && buki_motteruka(INVEN_RARM + hand) && !p_ptr->weapon_info[hand].icky_wield && ((p_ptr->cur_lite <= 0) || one_in_(7)))
			{
				int maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
				if (one_in_(backstab ? 13 : (stab_fleeing || fuiuchi) ? 15 : 27))
				{
					k *= 5;
					drain_result *= 2;
#ifdef JP
					msg_format("刃が%sに深々と突き刺さった！", m_name);
#else
					msg_format("You critically injured %s!", m_name);
#endif
				}
				else if (((m_ptr->hp < maxhp/2) && one_in_((p_ptr->weapon_info[0].num_blow+p_ptr->weapon_info[1].num_blow+1)*10)) || ((one_in_(666) || ((backstab || fuiuchi) && one_in_(11))) && !(r_ptr->flags1 & RF1_UNIQUE) && !(r_ptr->flags7 & RF7_UNIQUE2)))
				{
					if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || (m_ptr->hp >= maxhp/2))
					{
						k = MAX(k*5, m_ptr->hp/2);
						drain_result *= 2;
#ifdef JP
						msg_format("%sに致命傷を負わせた！", m_name);
#else
						msg_format("You fatally injured %s!", m_name);
#endif
					}
					else
					{
						k = m_ptr->hp + 1;
#ifdef JP
						msg_format("刃が%sの急所を貫いた！", m_name);
#else
						msg_format("You hit %s on a fatal spot!", m_name);
#endif
					}
				}
			}

			if (k <= 0) can_drain = FALSE;

			if (drain_result > m_ptr->hp)
				drain_result = m_ptr->hp;

			if (have_flag(flgs, TR_WILD))
				wild_weapon_strike();

			if (mode == ROGUE_ASSASSINATE)
			{
				if ((r_ptr->flags1 & RF1_UNIQUE) || (r_ptr->flags7 & RF7_UNIQUE2) || mon_save_p(m_ptr->r_idx, A_DEX))
				{
					k = MAX(k*5, m_ptr->hp/2);
					drain_result *= 2;
					msg_format("You fatally injured %s!", m_name);
				}
				else
				{
					k = m_ptr->hp + 1;
					msg_format("You hit %s on a fatal spot!", m_name);
				}
			}

			if (p_ptr->pclass == CLASS_WEAPONMASTER)
			{
				if (mode == WEAPONMASTER_STRIKE_VULNERABILITY)
					k *= 2;

				if (mode == WEAPONMASTER_VICIOUS_STRIKE)
					k *= 2;

				switch(weaponmaster_get_toggle())
				{
				case TOGGLE_SHIELD_BASH:
					if (one_in_(12))
					{
						if (r_ptr->flagsr & RFR_RES_ALL)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_ALL;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (r_ptr->flags3 & RF3_NO_STUN)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_STUN;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (mon_save_p(m_ptr->r_idx, A_STR))
						{
							msg_format(T("%^s resists.", "%^sには効果がなかった。"), m_name);
						}
						else
						{
							msg_format(T("%^s is stunned.", ), m_name);
							set_monster_stunned(c_ptr->m_idx, MAX(MON_STUNNED(m_ptr), 2));
						}
					}
					break;

				case TOGGLE_BURNING_BLADE:
					if (r_ptr->flagsr & RFR_RES_ALL)
					{
						msg_format("%^s is immune.", m_name);
						k = 0;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
					}
					else if (r_ptr->flagsr & RFR_IM_FIRE)
					{
						msg_format("%^s resists alot.", m_name);
						k /= 9;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_FIRE);
					}
					else if (r_ptr->flags3 & (RF3_HURT_FIRE))
					{
						msg_format("%^s is hit hard.", m_name);
						k *= 2;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_HURT_FIRE);
					}
					break;
				case TOGGLE_ICE_BLADE:
					if (r_ptr->flagsr & RFR_RES_ALL)
					{
						msg_format("%^s is immune.", m_name);
						k = 0;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
					}
					else if (r_ptr->flagsr & RFR_IM_COLD)
					{
						msg_format("%^s resists alot.", m_name);
						k /= 9;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_COLD);
					}
					else 
					{
						if (r_ptr->flags3 & (RF3_HURT_COLD))
						{
							msg_format("%^s is hit hard.", m_name);
							k *= 2;
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= (RF3_HURT_COLD);
						}
						if (one_in_(5)
							&& !(r_ptr->flags1 & (RF1_UNIQUE))
							&& !mon_save_p(m_ptr->r_idx, A_STR) )
						{
							msg_format("%^s is slowed by the cold.", m_name);
							set_monster_slow(c_ptr->m_idx, MON_SLOW(m_ptr) + 50);
						}
					}
					break;
				case TOGGLE_THUNDER_BLADE:
					if (r_ptr->flagsr & RFR_RES_ALL)
					{
						msg_format("%^s is immune.", m_name);
						k = 0;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_RES_ALL);
					}
					else if (r_ptr->flagsr & RFR_IM_ELEC)
					{
						msg_format("%^s resists alot.", m_name);
						k /= 9;
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= (RFR_IM_ELEC);
					}
					else 
					{
						if (one_in_(5)
							&& !(r_ptr->flags3 & (RF3_NO_STUN))
							&& !mon_save_p(m_ptr->r_idx, A_STR) )
						{
							msg_format("%^s is shocked convulsively.", m_name);
							set_monster_stunned(c_ptr->m_idx, MAX(MON_STUNNED(m_ptr), 4));
						}
					}
					break;
				case TOGGLE_WILD_BLADE:
					wild_weapon_strike(); /* TODO: Add EGO_WILD_BLADE and TR_WILD for weapons */
					break;
				}
			}

			if (mode == WEAPONMASTER_CRUSADERS_STRIKE)
				k = k * 3 / 2;

			if (mode == WEAPONMASTER_REAPING)
			{
				int              start_dir, x2, y2;
				int				 dir;
				cave_type       *c_ptr2;
				monster_type    *m_ptr2;
				bool             fear2 = FALSE;
				int				 ct = 0;

				k *= 2;

				/* First hit the chosen target */			
				if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
				{
					*mdeath = TRUE;
					ct += 20;
				}

				msg_format("Your swing your %s about, reaping a harvest of death!", o_name);
			
				/* Next hit all adjacent targets in a swinging circular arc */
				start_dir = calculate_dir(px, py, x, y);
				dir = start_dir;

				for (;;)
				{
					dir = get_next_dir(dir);
					if (dir == start_dir || dir == 5) break;

					x2 = px + ddx[dir];
					y2 = py + ddy[dir];
					c_ptr2 = &cave[y2][x2];
					m_ptr2 = &m_list[c_ptr2->m_idx];

					if (c_ptr2->m_idx && (m_ptr2->ml || cave_have_flag_bold(y2, x2, FF_PROJECT)))
					{
						if (mon_take_hit(c_ptr2->m_idx, k, &fear2, NULL))
							ct += 10;
					}
				}

				/* Finally, gain Wraithform */
				set_wraith_form(p_ptr->wraith_form + ct/10, FALSE);

				if (p_ptr->wizard)
					msg_print("****END REAPING****");
			}
			/* Damage, check for fear and death */
			else if (mon_take_hit(c_ptr->m_idx, k, fear, NULL))
			{
				*mdeath = TRUE;

				/* Mauler L15: Splattering - Whenever you kill a monster, your last 
				   strike damage is applied to all foes within radius 2 ball of recently 
				   deceased enemy. */
				if ( p_ptr->pclass == CLASS_MAULER
				  && p_ptr->ryoute
				  && p_ptr->lev >= 15 )
				{
					project(0, 2, y, x, k, 
					        GF_BLOOD, PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL | PROJECT_FULL_DAM, -1);
				}

				if ( o_ptr->tval == TV_SWORD
				  && o_ptr->sval == SV_RUNESWORD
				  /*&& monster_living(r_ptr)*/ )
				{
					_rune_sword_kill(o_ptr, r_ptr);
				}

				if (mode == WEAPONMASTER_ABSORB_SOUL)
				{
					bool do_it = TRUE;

					if (do_it && r_ptr->level <= o_ptr->to_h + o_ptr->to_d)
					{
						msg_print("The soul is too weak!");
						do_it = FALSE;
					}

					if ( do_it 
					  && (object_is_artifact(o_ptr) || have_flag(o_ptr->art_flags, TR_SIGNATURE))
					  && (r_ptr->flags1 & RF1_UNIQUE) == 0 
					  && !one_in_(3) )
					{
						msg_print("The soul got away!");
						do_it = FALSE;
					}

					if (do_it)
					{
						msg_print("Your weapon feeds on the soul of the newly departed!");
						o_ptr->to_h++;
						o_ptr->to_d++;
					} 
				}

				if (duelist_attack)
				{
					p_ptr->duelist_target_idx = 0;
					p_ptr->redraw |= PR_STATUS;

					if (p_ptr->lev >= 35)	/* Endless Duel */
					{
						/* Hacks so that get_aim_dir() actually allows user to select a new target */
						target_who = 0;
						command_dir = 0;
						msg_print("Your chosen target is vanquished!  Select another.");
						duelist_issue_challenge();
					}
					else
						msg_print("Your chosen target is vanquished!");
				}

				if ((p_ptr->pclass == CLASS_BERSERKER || mut_present(MUT_FANTASTIC_FRENZY) || p_ptr->tim_shrike) && energy_use)
				{
					if (p_ptr->migite && p_ptr->hidarite)
					{
						if (hand) energy_use = energy_use*3/5+energy_use*num*2/(p_ptr->weapon_info[hand].num_blow*5);
						else energy_use = energy_use*num*3/(p_ptr->weapon_info[hand].num_blow*5);
					}
					else
					{
						energy_use = energy_use*num/p_ptr->weapon_info[hand].num_blow;
					}
				}
				if ((o_ptr->name1 == ART_ZANTETSU) && is_lowlevel)
#ifdef JP
					msg_print("またつまらぬものを斬ってしまった．．．");
#else
					msg_print("Sigh... Another trifling thing I've cut....");
#endif
				break;
			}
			else
			{
				if (mauler_get_toggle() == TOGGLE_CURSED_WOUNDS && p_ptr->ryoute)
				{
					int amt = (k+2)/3;
					m_ptr->maxhp -= amt;
					msg_format("%^s seems weakened.", m_name);
				}

				if (mode == MELEE_AWESOME_BLOW)
				{
					int dir = calculate_dir(px, py, x, y);
					if (dir != 5)
					{
						int ct = 0;
						int max = 3;
						int m_idx = c_ptr->m_idx;
						int ty = y, tx = x;
						int oy = y, ox = x;

						if (p_ptr->pclass == CLASS_RAGE_MAGE)
						{
							if (p_ptr->shero)
								max = 6;
						}
						else if (p_ptr->pclass == CLASS_MAULER)
						{
							int w = o_ptr->weight;
							max = MIN(p_ptr->lev/5, w/40);
						}
						
						for (ct = 0; ct < max; ct++) 
						{
							y += ddy[dir];
							x += ddx[dir];
							if (!cave_empty_bold(y, x))
							{
								int dam = 50;

								if ( cave[y][x].m_idx 
								  || cave_have_flag_bold(y, x, FF_TREE)
								  || cave[y][x].feat == feat_rubble
								  || cave[y][x].feat == feat_dark_pit )
								{
									dam = 25;
								}
								msg_format("%^s is wounded.", m_name);							
								mon_take_hit(m_idx, dam * (max - ct), fear, NULL);
								break;
							}
							else
							{
								ty = y;
								tx = x;
							}
							if (ty != oy || tx != ox)
							{
								int m_idx = cave[oy][ox].m_idx;

								cave[oy][ox].m_idx = 0;
								cave[ty][tx].m_idx = m_idx;
								m_ptr->fy = ty;
								m_ptr->fx = tx;
	
								update_mon(m_idx, TRUE);
								lite_spot(oy, ox);
								lite_spot(ty, tx);
								
								oy = ty;
								ox = tx;
	
								if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
									p_ptr->update |= PU_MON_LITE;
							}
						}
					}
				}
					
				if (mode == WEAPONMASTER_ENCLOSE)
				{
					if (!(m_ptr->mflag2 & MFLAG2_ENCLOSED))
					{
						msg_format("%^s seems unable to run away.", m_name);
						m_ptr->mflag2 |= MFLAG2_ENCLOSED;
					}
				}

				if (mode == WEAPONMASTER_CRUSADERS_STRIKE)
				{
					msg_format("Your Crusader's Strike drains life from %s!", m_name);
					hp_player(k);
				}
			
				/* Clubmaster Hacks.  We do these effects *after* the monster takes damage. */
				if ( p_ptr->pclass == CLASS_WEAPONMASTER
				  && strcmp(weaponmaster_speciality1_name(), "Clubs") == 0
				  && p_ptr->speciality1_equip )
				{
					int odds = 5;
				
					if (mode == WEAPONMASTER_CUNNING_STRIKE)
						odds = 2;

					if (one_in_(odds))
					{
						if (r_ptr->flagsr & RFR_RES_ALL)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_ALL;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (r_ptr->flags3 & RF3_NO_CONF)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (mon_save_p(m_ptr->r_idx, A_STR))
						{
							msg_format(T("%^s resists.", "%^sには効果がなかった。"), m_name);
						}
						else
						{
							msg_format(T("%^s appears confused.", "%^sは混乱したようだ。"), m_name);
							set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
						}
					}

					if (p_ptr->lev >= 20 && one_in_(odds))
					{
						if (r_ptr->flagsr & RFR_RES_ALL)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_ALL;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (r_ptr->flags3 & RF3_NO_SLEEP)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_SLEEP;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (mon_save_p(m_ptr->r_idx, A_STR))
						{
							msg_format(T("%^s resists.", "%^sには効果がなかった。"), m_name);
						}
						else
						{
							msg_format(T("%^s is knocked out.", ), m_name);
							knock_out++;		
							/* No more retaliation this round! */					
							retaliation_count = 100; /* Any number >= 4 will do ... */
						}
					}

					if ((p_ptr->lev >= 45 || mode == WEAPONMASTER_CUNNING_STRIKE) && one_in_(odds))
					{
						if (r_ptr->flagsr & RFR_RES_ALL)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_ALL;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (r_ptr->flags3 & RF3_NO_STUN)
						{
							if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_STUN;
							msg_format(T("%^s is immune.", "%^sには効果がなかった。"), m_name);
						}
						else if (mon_save_p(m_ptr->r_idx, A_STR))
						{
							msg_format(T("%^s resists.", "%^sには効果がなかった。"), m_name);
						}
						else
						{
							msg_format(T("%^s is stunned.", ), m_name);
							set_monster_stunned(c_ptr->m_idx, MAX(MON_STUNNED(m_ptr), 2));
						}
					}
				}
			}

			/* Anger the monster */
			if (k > 0) anger_monster(m_ptr);

			touch_zap_player(c_ptr->m_idx);
			touch_ct++;

			/* Are we draining it?  A little note: If the monster is
			dead, the drain does not work... */

			if (can_drain && (drain_result > 0))
			{
				if (o_ptr->name1 == ART_MURAMASA)
				{
					if (is_human)
					{
						int to_h = o_ptr->to_h;
						int to_d = o_ptr->to_d;
						int i, flag;

						flag = 1;
						for (i = 0; i < to_h + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_h++;

						flag = 1;
						for (i = 0; i < to_d + 3; i++) if (one_in_(4)) flag = 0;
						if (flag) to_d++;

						if (o_ptr->to_h != to_h || o_ptr->to_d != to_d)
						{
#ifdef JP
							msg_print("妖刀は血を吸って強くなった！");
#else
							msg_print("Muramasa sucked blood, and became more powerful!");
#endif
							o_ptr->to_h = to_h;
							o_ptr->to_d = to_d;

							if (p_ptr->pclass == CLASS_BLOOD_KNIGHT)
							{
								/*
								msg_print("Muramasa shares the blood with you!");
								p_ptr->blood_points += 100;
								if (p_ptr->blood_points > 1000)
									p_ptr->blood_points = 1000;
								p_ptr->redraw |= PR_BLOOD_POINTS;
								*/
							}
						}
					}
				}
				else
				{
					if (drain_result > 5) /* Did we really hurt it? */
					{
						drain_heal = damroll(2, drain_result / 6);

						/* Hex */
						if (hex_spelling(HEX_VAMP_BLADE)) drain_heal *= 2;

						if (cheat_xtra || p_ptr->wizard)
						{
						/*	msg_format("Draining left: %d  amount: %d", drain_left, drain_heal); */
						}

						if (drain_left)
						{
							if (drain_heal < drain_left)
							{
								drain_left -= drain_heal;
							}
							else
							{
								drain_heal = drain_left;
								drain_left = 0;
							}

							if (drain_msg)
							{
#ifdef JP
								msg_format("刃が%sから生命力を吸い取った！", m_name);
#else
								msg_format("Your weapon drains life from %s!", m_name);
#endif

								drain_msg = FALSE;
							}

							drain_heal = (drain_heal * mutant_regenerate_mod) / 100;

							hp_player_aux(drain_heal);
						}
					}
				}
				m_ptr->maxhp -= (k+7)/8;
				if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;
				if (m_ptr->maxhp < 1) m_ptr->maxhp = 1;
				weak = TRUE;
			}
			can_drain = FALSE;
			drain_result = 0;

			/* Confusion attack */
			if ((p_ptr->special_attack & ATTACK_CONFUSE) || (chaos_effect == 3) || (mode == HISSATSU_CONF) || hex_spelling(HEX_CONFUSION))
			{
				/* Cancel glowing hands */
				if (p_ptr->special_attack & ATTACK_CONFUSE)
				{
					p_ptr->special_attack &= ~(ATTACK_CONFUSE);
#ifdef JP
					msg_print("手の輝きがなくなった。");
#else
					msg_print("Your hands stop glowing.");
#endif
					p_ptr->redraw |= (PR_STATUS);

				}

				/* Confuse the monster */
				if (r_ptr->flags3 & RF3_NO_CONF)
				{
					if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_NO_CONF;

#ifdef JP
					msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else if (randint0(100) < r_ptr->level)
				{
#ifdef JP
					msg_format("%^sには効果がなかった。", m_name);
#else
					msg_format("%^s is unaffected.", m_name);
#endif

				}
				else
				{
#ifdef JP
					msg_format("%^sは混乱したようだ。", m_name);
#else
					msg_format("%^s appears confused.", m_name);
#endif

					(void)set_monster_confused(c_ptr->m_idx, MON_CONFUSED(m_ptr) + 10 + randint0(p_ptr->lev) / 5);
				}
			}

			else if (chaos_effect == 4)
			{
				bool resists_tele = FALSE;

				if (r_ptr->flagsr & RFR_RES_TELE)
				{
					if (r_ptr->flags1 & RF1_UNIQUE)
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
						msg_format("%^sには効果がなかった。", m_name);
#else
						msg_format("%^s is unaffected!", m_name);
#endif

						resists_tele = TRUE;
					}
					else if (r_ptr->level > randint1(100))
					{
						if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
#ifdef JP
						msg_format("%^sは抵抗力を持っている！", m_name);
#else
						msg_format("%^s resists!", m_name);
#endif

						resists_tele = TRUE;
					}
				}

				if (!resists_tele)
				{
#ifdef JP
					msg_format("%^sは消えた！", m_name);
#else
					msg_format("%^s disappears!", m_name);
#endif

					teleport_away(c_ptr->m_idx, 50, TELEPORT_PASSIVE);
					num = num_blow + 1; /* Can't hit it anymore! */
					*mdeath = TRUE;
				}
			}

			else if ((chaos_effect == 5) && (randint1(90) > r_ptr->level))
			{
				if (!(r_ptr->flags1 & (RF1_UNIQUE | RF1_QUESTOR)) &&
				    !(r_ptr->flagsr & RFR_EFF_RES_CHAO_MASK))
				{
					if (polymorph_monster(y, x))
					{
#ifdef JP
						msg_format("%^sは変化した！", m_name);
#else
						msg_format("%^s changes!", m_name);
#endif

						*fear = FALSE;
						weak = FALSE;
					}
					else
					{
#ifdef JP
						msg_format("%^sには効果がなかった。", m_name);
#else
						msg_format("%^s is unaffected.", m_name);
#endif
					}

					/* Hack -- Get new monster */
					m_ptr = &m_list[c_ptr->m_idx];

					/* Oops, we need a different name... */
					monster_desc(m_name, m_ptr, 0);

					/* Hack -- Get new race */
					r_ptr = &r_info[m_ptr->r_idx];
				}
			}
			else if (o_ptr->name1 == ART_G_HAMMER)
			{
				monster_type *m_ptr = &m_list[c_ptr->m_idx];

				if (m_ptr->hold_o_idx)
				{
					object_type *q_ptr = &o_list[m_ptr->hold_o_idx];
					char o_name[MAX_NLEN];

					object_desc(o_name, q_ptr, OD_NAME_ONLY);
					q_ptr->held_m_idx = 0;
					q_ptr->marked = OM_TOUCHED;
					m_ptr->hold_o_idx = q_ptr->next_o_idx;
					q_ptr->next_o_idx = 0;
#ifdef JP
					msg_format("%sを奪った。", o_name);
#else
					msg_format("You snatched %s.", o_name);
#endif
					inven_carry(q_ptr);
				}
			}

			if ( p_ptr->pclass == CLASS_DUELIST
			  && o_ptr->tval == TV_POLEARM 
			  && o_ptr->sval == SV_DEATH_SCYTHE
			  && !one_in_(3) )
			{
				death_scythe_miss(o_ptr, hand, mode);
			}
		}

		/* Player misses */
		else
		{
			backstab = FALSE; /* Clumsy! */
			fuiuchi = FALSE; /* Clumsy! */

			if (o_ptr->name1 == ART_EVISCERATOR && !duelist_attack && one_in_(2)) num_blow++;

			if ((o_ptr->tval == TV_POLEARM) && (o_ptr->sval == SV_DEATH_SCYTHE) && one_in_(3))
			{
				msg_format(T("You miss %s.", "ミス！ %sにかわされた。"), m_name);
				death_scythe_miss(o_ptr, hand, mode);
			}
			else
			{
				/* Sound */
				sound(SOUND_MISS);

				/* Message */
#ifdef JP
				msg_format("ミス！ %sにかわされた。", m_name);
#else
				msg_format("You miss %s.", m_name);
#endif
			}
		}
		backstab = FALSE;
		fuiuchi = FALSE;

		/* Hack: Blood Knights might damage themselves with the Blood Feast (or monster auras).  Since this
		   class gets more attacks as health diminishes, it is only fair that we recalc and bump up attacks.
		   This might be the player's last attack before death, and we wouldn't want to short change them! */
		if (p_ptr->pclass == CLASS_BLOOD_KNIGHT)
		{
			num_blow = p_ptr->weapon_info[hand].num_blow;
			if ((o_ptr->tval == TV_SWORD) && (o_ptr->sval == SV_DOKUBARI)) num_blow = 1;
		}

		/* Hack: Whirlwind first attacks chosen monster, than attempts to strike
		   all other monsters adjacent.*/
		if (do_whirlwind)
		{
			int              start_dir, x2, y2;
			int				 dir;
			cave_type       *c_ptr2;
			monster_type    *m_ptr2;
			bool			fear2 = FALSE;
			bool			mdeath2 = FALSE;

			msg_format("Your swing your %s about, striking all nearby foes.", o_name);
			
			start_dir = calculate_dir(px, py, x, y);
			dir = start_dir;

			for (;;)
			{
				dir = get_next_dir(dir);
				if (dir == start_dir || dir == 5) break;

				x2 = px + ddx[dir];
				y2 = py + ddy[dir];
				c_ptr2 = &cave[y2][x2];
				m_ptr2 = &m_list[c_ptr2->m_idx];

				if (c_ptr2->m_idx && (m_ptr2->ml || cave_have_flag_bold(y2, x2, FF_PROJECT)))
					py_attack_aux(y2, x2, &fear2, &mdeath2, hand, WEAPONMASTER_WHIRLWIND);
			}

			if (p_ptr->wizard)
				msg_print("****END WHIRLWIND****");
		}

		if (mode == WEAPONMASTER_RETALIATION) break;
		if (mode == WEAPONMASTER_CRUSADERS_STRIKE) break;
		if (mode == WEAPONMASTER_STRIKE_VULNERABILITY) break;
		if (mode == WEAPONMASTER_MANY_STRIKE) break;
		if (mode == WEAPONMASTER_PIERCING_STRIKE) break;
		if (mode == WEAPONMASTER_PROXIMITY_ALERT) break;
		if (mode == WEAPONMASTER_WHIRLWIND) break;
		if (mode == WEAPONMASTER_REAPING) break;
		if (mode == WEAPONMASTER_ABSORB_SOUL) break;
		if (mode == MELEE_AWESOME_BLOW) break;
		if (mode == ROGUE_ASSASSINATE) break;
	}

	if (mode == WEAPONMASTER_KNOCK_BACK && hit_ct)
	{
		int dir = calculate_dir(px, py, x, y);
		if (dir != 5)
		{
			int i;
			int msec = delay_factor * delay_factor * delay_factor;
			for (i = 0; i < hit_ct; i++)
			{
				int ty = y, tx = x;
				int oy = y, ox = x;

				y += ddy[dir];
				x += ddx[dir];
				if (cave_empty_bold(y, x))
				{
					ty = y;
					tx = x;
				}
				if (ty != oy || tx != ox)
				{
					int m_idx = cave[oy][ox].m_idx;

					cave[oy][ox].m_idx = 0;
					cave[ty][tx].m_idx = m_idx;
					m_ptr->fy = ty;
					m_ptr->fx = tx;
	
					Term_fresh();
					update_mon(m_idx, TRUE);
					lite_spot(oy, ox);
					lite_spot(ty, tx);
	
					if (r_info[m_ptr->r_idx].flags7 & (RF7_LITE_MASK | RF7_DARK_MASK))
						p_ptr->update |= PU_MON_LITE;
					
					Term_xtra(TERM_XTRA_DELAY, msec);
					Term_fresh();
				}
				else
					break;
			}
		}
	}

	/* Sleep counter ticks down in energy units ... Also, *lots* of code
	   will just wake the monster back up, so we need to be very careful
	   about when we call this.  */
	if (knock_out && !(*mdeath))
		set_monster_csleep(c_ptr->m_idx, MON_CSLEEP(m_ptr) + 500);

	if (weaponmaster_get_toggle() == TOGGLE_TRIP && mode == 0 && !(*mdeath) && !fear_stop)
	{
		if (test_hit_norm(chance, MON_AC(r_ptr, m_ptr), m_ptr->ml))
		{
			if (m_ptr->mflag2 & MFLAG2_TRIPPED)
				msg_format("%^s is already tripped up.", m_name);
			else if ( !(r_ptr->flags1 & RF1_UNIQUE)
			       || !mon_save_p(m_ptr->r_idx, A_STR) )
			{
				msg_format("%^s cries 'Help, I've fallen and I can't get up!'", m_name);
				m_ptr->mflag2 |= MFLAG2_TRIPPED;
			}
			else
				msg_format("%^s nimbly dodges your attempt to trip.", m_name);
		}
		else
			msg_format("You attempt to trip %^s but miss.", m_name);
	}

	if (weak && !(*mdeath))
	{
#ifdef JP
		msg_format("%sは弱くなったようだ。", m_name);
#else
		msg_format("%^s seems weakened.", m_name);
#endif
	}
	if (drain_left != MAX_VAMPIRIC_DRAIN)
	{
		if (one_in_(4))
		{
			chg_virtue(V_UNLIFE, 1);
		}
	}

	if (touch_ct && !(*mdeath))
		fear_p_touch_m(m_ptr);

	/* Mega-Hack -- apply earthquake brand */
	if (do_quake)
	{
		if (mauler_get_toggle() == TOGGLE_NO_EARTHQUAKE && p_ptr->ryoute)
		{
		}
		else
		{
			earthquake(py, px, 10);
			if (!cave[y][x].m_idx) *mdeath = TRUE;
		}
	}

	return success_hit;
}

bool random_opponent(int *y, int *x)
{
	int dirs[9];
	int ct = 0;
	int i, tx, ty;
	cave_type *c_ptr;

	for (i = 0; i < 8; i++)
	{
		ty = py + ddy_ddd[i];
		tx = px + ddx_ddd[i];
		c_ptr = &cave[ty][tx];
		if (c_ptr->m_idx)
		{
			dirs[ct] = i;
			ct++;
		}
	}

	if (ct)
	{
		i = randint0(ct);
		*y = py + ddy_ddd[dirs[i]];
		*x = px + ddx_ddd[dirs[i]];
		return TRUE;
	}
	return FALSE;
}

bool py_attack(int y, int x, int mode)
{
	bool            fear = FALSE;
	bool            mdeath = FALSE;
	bool            stormbringer = FALSE;

	cave_type       *c_ptr = &cave[y][x];
	monster_type    *m_ptr = &m_list[c_ptr->m_idx];
	monster_race    *r_ptr = &r_info[m_ptr->r_idx];
	char            m_name[80];

	/* Disturb the player */
	disturb(0, 0);

	energy_use = 100;

	if (!p_ptr->migite && 
	    !p_ptr->hidarite &&
	    !mut_present(MUT_HORNS) &&
		!mut_present(MUT_BEAK) &&
		!mut_present(MUT_SCORPION_TAIL) &&
		!mut_present(MUT_TRUNK) &&
		!mut_present(MUT_TENTACLES))
	{
#ifdef JP
		msg_format("%s攻撃できない。", (empty_hands(FALSE) == EMPTY_HAND_NONE) ? "両手がふさがって" : "");
#else
		msg_print("You cannot do attacking.");
#endif
		energy_use = 0;
		return FALSE;
	}

	/* Extract monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	if (m_ptr->ml)
	{
		/* Auto-Recall if possible and visible */
		if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

		/* Track a new monster */
		health_track(c_ptr->m_idx);
	}

	if ((r_ptr->flags1 & RF1_FEMALE) &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image || !m_ptr->ml))
	{
		if ((inventory[INVEN_RARM].name1 == ART_ZANTETSU) || (inventory[INVEN_LARM].name1 == ART_ZANTETSU))
		{
#ifdef JP
			msg_print("拙者、おなごは斬れぬ！");
#else
			msg_print("I can not attack women!");
#endif
			return FALSE;
		}
	}

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
	{
#ifdef JP
		msg_print("なぜか攻撃することができない。");
#else
		msg_print("Something prevent you from attacking.");
#endif
		return FALSE;
	}

	/* Stop if friendly */
	if (!is_hostile(m_ptr) &&
	    !(p_ptr->stun || p_ptr->confused || p_ptr->image ||
	    IS_SHERO() || !m_ptr->ml))
	{
		if (inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
		if (stormbringer)
		{
#ifdef JP
			msg_format("黒い刃は強欲に%sを攻撃した！", m_name);
#else
			msg_format("Your black blade greedily attacks %s!", m_name);
#endif
			chg_virtue(V_INDIVIDUALISM, 1);
			chg_virtue(V_HONOUR, -1);
			chg_virtue(V_JUSTICE, -1);
			chg_virtue(V_COMPASSION, -1);
		}
		else if (p_ptr->pclass != CLASS_BERSERKER)
		{
#ifdef JP
			if (get_check("本当に攻撃しますか？"))
#else
			if (get_check("Really hit it? "))
#endif
			{
				chg_virtue(V_INDIVIDUALISM, 1);
				chg_virtue(V_HONOUR, -1);
				chg_virtue(V_JUSTICE, -1);
				chg_virtue(V_COMPASSION, -1);
			}
			else
			{
#ifdef JP
				msg_format("%sを攻撃するのを止めた。", m_name);
#else
				msg_format("You stop to avoid hitting %s.", m_name);
#endif
				return FALSE;
			}
		}
	}

	if (MON_CSLEEP(m_ptr)) /* It is not honorable etc to attack helpless victims */
	{
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_COMPASSION, -1);
		if (!(r_ptr->flags3 & RF3_EVIL) || one_in_(5)) chg_virtue(V_HONOUR, -1);
	}

	/* TODO: Skills should be applied later ... */
	if (p_ptr->migite && p_ptr->hidarite)
	{
		if ((p_ptr->skill_exp[GINOU_NITOURYU] < s_info[p_ptr->pclass].s_max[GINOU_NITOURYU]) && ((p_ptr->skill_exp[GINOU_NITOURYU] - 1000) / 200 < r_ptr->level))
		{
			if (p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_BEGINNER)
				p_ptr->skill_exp[GINOU_NITOURYU] += 80;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_SKILLED)
				p_ptr->skill_exp[GINOU_NITOURYU] += 4;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_EXPERT)
				p_ptr->skill_exp[GINOU_NITOURYU] += 1;
			else if(p_ptr->skill_exp[GINOU_NITOURYU] < WEAPON_EXP_MASTER)
				if (one_in_(3)) p_ptr->skill_exp[GINOU_NITOURYU] += 1;
			p_ptr->update |= (PU_BONUS);
		}
	}

	if (p_ptr->riding)
	{
		int cur = p_ptr->skill_exp[GINOU_RIDING];
		int max = s_info[p_ptr->pclass].s_max[GINOU_RIDING];

		if (cur < max)
		{
			int ridinglevel = r_info[m_list[p_ptr->riding].r_idx].level;
			int targetlevel = r_ptr->level;
			int inc = 0;

			if ((cur / 200 - 5) < targetlevel)
				inc += 1;

			/* Extra experience */
			if ((cur / 100) < ridinglevel)
			{
				if ((cur / 100 + 15) < ridinglevel)
					inc += 1 + (ridinglevel - (cur / 100 + 15));
				else
					inc += 1;
			}

			if (inc)
			{
				p_ptr->skill_exp[GINOU_RIDING] = MIN(max, cur + inc);
				p_ptr->update |= PU_BONUS;
			}
		}
	}

	riding_t_m_idx = c_ptr->m_idx;

	drain_left = MAX_VAMPIRIC_DRAIN;
	retaliation_count = 0;
	melee_hack = TRUE;
	fear_stop = FALSE;

	if (weaponmaster_get_toggle() == TOGGLE_FRENZY_STANCE)
	{
		object_type rarm, larm;
		int i, j;
		object_copy(&rarm, &inventory[INVEN_RARM]);
		object_copy(&larm, &inventory[INVEN_LARM]);

		/* Attack with equipped weapons */
		drain_left = MAX_VAMPIRIC_DRAIN;
		if (p_ptr->migite) 
			py_attack_aux(y, x, &fear, &mdeath, 0, mode);

		if (!fear_stop)
		{
			drain_left = MAX_VAMPIRIC_DRAIN;
			if (p_ptr->hidarite && !mdeath) 
				py_attack_aux(y, x, &fear, &mdeath, 1, mode);
		}

		/* Attack with inventory weapons as if single wielding */
		/* Sorry, no more vampirism!! */
		if (!fear_stop)
		{
			weaponmaster_get_frenzy_items();
			for (i = 0; i < MAX_FRENZY_ITEMS; i++)
			{
				int item = frenzy_items[i];
				object_type copy, blank;

				object_wipe(&blank);
			
				if (item < 0) break;
				if (mdeath) break;
			
				object_copy(&copy, &inventory[item]);
				copy.number = 1;
				object_copy(&inventory[INVEN_RARM], &copy);
				if (p_ptr->hidarite)
					object_copy(&inventory[INVEN_LARM], &blank);

				p_ptr->update |= PU_BONUS;
				handle_stuff();

				py_attack_aux(y, x, &fear, &mdeath, 0, mode);
			}
			for (j = 0; j < i; j++)
			{
				int item = frenzy_items[j];
				if (object_is_artifact(&inventory[item]))
				{
					 if (one_in_(3))
					 {
						blast_object(&inventory[item]);
					 }
				}
				else
				{
					inven_item_increase(item, -1);
					inven_item_describe(item);
					inven_item_optimize(item);
				}	
			}

			object_copy(&inventory[INVEN_RARM], &rarm);
			object_copy(&inventory[INVEN_LARM], &larm);
			p_ptr->update |= PU_BONUS;
			handle_stuff();
		}
	}
	else if (weaponmaster_get_toggle() == TOGGLE_MANY_STRIKE && mode == 0)
	{
		int i;
		bool stop = FALSE;
		int msec = delay_factor * delay_factor * delay_factor;
		drain_left = MAX_VAMPIRIC_DRAIN;
		if (p_ptr->migite) 
		{
			for (i = 0; i < p_ptr->weapon_info[0].num_blow; i++)
			{
				{
					char c = object_char(&inventory[INVEN_RARM]);
					byte a = object_attr(&inventory[INVEN_RARM]);

					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					Term_fresh();
				}
				py_attack_aux(y, x, &fear, &mdeath, 0, WEAPONMASTER_MANY_STRIKE);
				if (fear_stop || !random_opponent(&y, &x))
				{
					stop = TRUE;
					break;
				}
			}
		}
		drain_left = MAX_VAMPIRIC_DRAIN;
		if (p_ptr->hidarite && !stop)
		{
			for (i = 0; i < p_ptr->weapon_info[1].num_blow; i++)
			{
				{
					char c = object_char(&inventory[INVEN_LARM]);
					byte a = object_attr(&inventory[INVEN_LARM]);

					print_rel(c, a, y, x);
					move_cursor_relative(y, x);
					Term_fresh();
					Term_xtra(TERM_XTRA_DELAY, msec);
					lite_spot(y, x);
					Term_fresh();
				}
				py_attack_aux(y, x, &fear, &mdeath, 1, WEAPONMASTER_MANY_STRIKE);
				if (fear_stop || !random_opponent(&y, &x)) break;
			}
		}
	}
	else if (weaponmaster_get_toggle() == TOGGLE_PIERCING_STRIKE && mode == 0)
	{
	u16b	path[512];
	int		ct = project_path(path, 3, py, px, y, x, PROJECT_PATH | PROJECT_THRU);
	int		msec = delay_factor * delay_factor * delay_factor;

		int i, j;
		bool stop = FALSE;
		drain_left = MAX_VAMPIRIC_DRAIN;
		if (p_ptr->migite) 
		{
			for (i = 0; i < p_ptr->weapon_info[0].num_blow; i++)
			{
				if (p_ptr->wizard)
					msg_format("Attack #%d", i+1);

				for (j = 0; j < ct; j++)
				{
					int nx, ny;
					ny = GRID_Y(path[j]);
					nx = GRID_X(path[j]);

					if (p_ptr->wizard)
						msg_format("  Step #%d", j+1);

					if (!cave_have_flag_bold(ny, nx, FF_PROJECT)
					 && !cave[ny][nx].m_idx) 
					{
						break;
					}

					if (!cave[ny][nx].m_idx) break;

					if (panel_contains(ny, nx) && player_can_see_bold(ny, nx))
					{
						char c = object_char(&inventory[INVEN_RARM]);
						byte a = object_attr(&inventory[INVEN_RARM]);
						int msec = delay_factor * delay_factor * delay_factor;

						print_rel(c, a, ny, nx);
						move_cursor_relative(ny, nx);
						Term_fresh();
						Term_xtra(TERM_XTRA_DELAY, msec);
						lite_spot(ny, nx);
						Term_fresh();
					}
					else
						Term_xtra(TERM_XTRA_DELAY, msec);
					
					if (!py_attack_aux(ny, nx, &fear, &mdeath, 0, WEAPONMASTER_PIERCING_STRIKE) || fear_stop) break;
				}
			}
		}
		drain_left = MAX_VAMPIRIC_DRAIN;
		if (p_ptr->hidarite && !fear_stop)
		{
			for (i = 0; i < p_ptr->weapon_info[1].num_blow; i++)
			{
				if (p_ptr->wizard)
					msg_format("Attack #%d", i+1);

				for (j = 0; j < ct; j++)
				{
					int nx, ny;
					ny = GRID_Y(path[j]);
					nx = GRID_X(path[j]);

					if (p_ptr->wizard)
						msg_format("  Step #%d", j+1);

					if (!cave_have_flag_bold(ny, nx, FF_PROJECT)
					 && !cave[ny][nx].m_idx) 
					{
						break;
					}

					if (!cave[ny][nx].m_idx) break;

					if (panel_contains(ny, nx) && player_can_see_bold(ny, nx))
					{
						char c = object_char(&inventory[INVEN_LARM]);
						byte a = object_attr(&inventory[INVEN_LARM]);
						int msec = delay_factor * delay_factor * delay_factor;

						print_rel(c, a, ny, nx);
						move_cursor_relative(ny, nx);
						Term_fresh();
						Term_xtra(TERM_XTRA_DELAY, msec);
						lite_spot(ny, nx);
						Term_fresh();
					}
					else
						Term_xtra(TERM_XTRA_DELAY, msec);
					
					if (!py_attack_aux(ny, nx, &fear, &mdeath, 1, WEAPONMASTER_PIERCING_STRIKE) || fear_stop) break;
				}
			}
		}
	}
	else
	{
		if (p_ptr->migite) 
			py_attack_aux(y, x, &fear, &mdeath, 0, mode);
		
		drain_left = MAX_VAMPIRIC_DRAIN;
		if (p_ptr->hidarite && !mdeath && !fear_stop) 
			py_attack_aux(y, x, &fear, &mdeath, 1, mode);
	}

	/* Mutations which yield extra 'natural' attacks */
	if (!mdeath && !fear_stop)
	{
		if (mut_present(MUT_HORNS) && !mdeath && !fear_stop)
			natural_attack(c_ptr->m_idx, MUT_HORNS, &fear, &mdeath);
		if (mut_present(MUT_BEAK) && !mdeath && !fear_stop)
			natural_attack(c_ptr->m_idx, MUT_BEAK, &fear, &mdeath);
		if (mut_present(MUT_SCORPION_TAIL) && !mdeath && !fear_stop)
			natural_attack(c_ptr->m_idx, MUT_SCORPION_TAIL, &fear, &mdeath);
		if (mut_present(MUT_TRUNK) && !mdeath && !fear_stop)
			natural_attack(c_ptr->m_idx, MUT_TRUNK, &fear, &mdeath);
		if (mut_present(MUT_TENTACLES) && !mdeath && !fear_stop)
			natural_attack(c_ptr->m_idx, MUT_TENTACLES, &fear, &mdeath);
	}
	else if (p_ptr->cleave && !fear_stop)
	{
		int y = 0, x = 0, i, dir = 0;
		cave_type *c_ptr;

		melee_hack = FALSE;
		for (i = 1; i <= 2; i++) /* TODO: Tweak the number of attempts */
		{
			dir = randint0(8);
			y = py + ddy_ddd[dir];
			x = px + ddx_ddd[dir];
			c_ptr = &cave[y][x];

			if (c_ptr->m_idx)
			{
				msg_print("You attempt to cleave another foe!");
				py_attack(y, x, 0);
				break;
			}
		}
		return TRUE;
	}

	melee_hack = FALSE;

	/* Hack -- delay fear messages */
	if (fear && m_ptr->ml && !mdeath)
	{
		sound(SOUND_FLEE);
		msg_format(T("%^s flees in terror!", "%^sは恐怖して逃げ出した！"), m_name);
	}

	if ((p_ptr->special_defense & KATA_IAI) && ((mode != HISSATSU_IAI) || mdeath))
	{
		set_action(ACTION_NONE);
	}

	return mdeath;
}


bool pattern_seq(int c_y, int c_x, int n_y, int n_x)
{
	feature_type *cur_f_ptr = &f_info[cave[c_y][c_x].feat];
	feature_type *new_f_ptr = &f_info[cave[n_y][n_x].feat];
	bool is_pattern_tile_cur = have_flag(cur_f_ptr->flags, FF_PATTERN);
	bool is_pattern_tile_new = have_flag(new_f_ptr->flags, FF_PATTERN);
	int pattern_type_cur, pattern_type_new;

	if (!is_pattern_tile_cur && !is_pattern_tile_new) return TRUE;

	pattern_type_cur = is_pattern_tile_cur ? cur_f_ptr->subtype : NOT_PATTERN_TILE;
	pattern_type_new = is_pattern_tile_new ? new_f_ptr->subtype : NOT_PATTERN_TILE;

	if (pattern_type_new == PATTERN_TILE_START)
	{
		if (!is_pattern_tile_cur && !p_ptr->confused && !p_ptr->stun && !p_ptr->image)
		{
#ifdef JP
			if (get_check("パターンの上を歩き始めると、全てを歩かなければなりません。いいですか？"))
#else
			if (get_check("If you start walking the Pattern, you must walk the whole way. Ok? "))
#endif
				return TRUE;
			else
				return FALSE;
		}
		else
			return TRUE;
	}
	else if ((pattern_type_new == PATTERN_TILE_OLD) ||
		 (pattern_type_new == PATTERN_TILE_END) ||
		 (pattern_type_new == PATTERN_TILE_WRECKED))
	{
		if (is_pattern_tile_cur)
		{
			return TRUE;
		}
		else
		{
#ifdef JP
			msg_print("パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_new == PATTERN_TILE_TELEPORT) ||
		 (pattern_type_cur == PATTERN_TILE_TELEPORT))
	{
		return TRUE;
	}
	else if (pattern_type_cur == PATTERN_TILE_START)
	{
		if (is_pattern_tile_new)
			return TRUE;
		else
		{
#ifdef JP
			msg_print("パターンの上は正しい順序で歩かねばなりません。");
#else
			msg_print("You must walk the Pattern in correct order.");
#endif

			return FALSE;
		}
	}
	else if ((pattern_type_cur == PATTERN_TILE_OLD) ||
		 (pattern_type_cur == PATTERN_TILE_END) ||
		 (pattern_type_cur == PATTERN_TILE_WRECKED))
	{
		if (!is_pattern_tile_new)
		{
#ifdef JP
			msg_print("パターンを踏み外してはいけません。");
#else
			msg_print("You may not step off from the Pattern.");
#endif

			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		if (!is_pattern_tile_cur)
		{
#ifdef JP
			msg_print("パターンの上を歩くにはスタート地点から歩き始めなくてはなりません。");
#else
			msg_print("You must start walking the Pattern from the startpoint.");
#endif

			return FALSE;
		}
		else
		{
			byte ok_move = PATTERN_TILE_START;
			switch (pattern_type_cur)
			{
				case PATTERN_TILE_1:
					ok_move = PATTERN_TILE_2;
					break;
				case PATTERN_TILE_2:
					ok_move = PATTERN_TILE_3;
					break;
				case PATTERN_TILE_3:
					ok_move = PATTERN_TILE_4;
					break;
				case PATTERN_TILE_4:
					ok_move = PATTERN_TILE_1;
					break;
				default:
					if (p_ptr->wizard)
#ifdef JP
						msg_format("おかしなパターン歩行、%d。", pattern_type_cur);
#else
						msg_format("Funny Pattern walking, %d.", pattern_type_cur);
#endif

					return TRUE; /* Goof-up */
			}

			if ((pattern_type_new == ok_move) ||
			    (pattern_type_new == pattern_type_cur))
				return TRUE;
			else
			{
				if (!is_pattern_tile_new)
#ifdef JP
					msg_print("パターンを踏み外してはいけません。");
#else
					msg_print("You may not step off from the Pattern.");
#endif
				else
#ifdef JP
					msg_print("パターンの上は正しい順序で歩かねばなりません。");
#else
					msg_print("You must walk the Pattern in correct order.");
#endif

				return FALSE;
			}
		}
	}
}


bool player_can_enter(s16b feature, u16b mode)
{
	feature_type *f_ptr = &f_info[feature];

	if (p_ptr->riding) return monster_can_cross_terrain(feature, &r_info[m_list[p_ptr->riding].r_idx], mode | CEM_RIDING);

	/* Pattern */
	if (have_flag(f_ptr->flags, FF_PATTERN))
	{
		if (!(mode & CEM_P_CAN_ENTER_PATTERN)) return FALSE;
	}

	/* "CAN" flags */
	if (have_flag(f_ptr->flags, FF_CAN_FLY) && p_ptr->levitation) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_SWIM) && p_ptr->can_swim) return TRUE;
	if (have_flag(f_ptr->flags, FF_CAN_PASS) && p_ptr->pass_wall) return TRUE;

	if (!have_flag(f_ptr->flags, FF_MOVE)) return FALSE;

	return TRUE;
}


/*
 * Move the player
 */
bool move_player_effect(int ny, int nx, u32b mpe_mode)
{
	cave_type *c_ptr = &cave[ny][nx];
	feature_type *f_ptr = &f_info[c_ptr->feat];

	if (!(mpe_mode & MPE_STAYING))
	{
		int oy = py;
		int ox = px;
		cave_type *oc_ptr = &cave[oy][ox];
		int om_idx = oc_ptr->m_idx;
		int nm_idx = c_ptr->m_idx;

		/* Move the player */
		py = ny;
		px = nx;

		/* Hack -- For moving monster or riding player's moving */
		if (!(mpe_mode & MPE_DONT_SWAP_MON))
		{
			/* Swap two monsters */
			c_ptr->m_idx = om_idx;
			oc_ptr->m_idx = nm_idx;

			if (om_idx > 0) /* Monster on old spot (or p_ptr->riding) */
			{
				monster_type *om_ptr = &m_list[om_idx];
				om_ptr->fy = ny;
				om_ptr->fx = nx;
				update_mon(om_idx, TRUE);
			}

			if (nm_idx > 0) /* Monster on new spot */
			{
				monster_type *nm_ptr = &m_list[nm_idx];
				nm_ptr->fy = oy;
				nm_ptr->fx = ox;
				update_mon(nm_idx, TRUE);
			}
		}

		/* Redraw old spot */
		lite_spot(oy, ox);

		/* Redraw new spot */
		lite_spot(ny, nx);

		/* Check for new panel (redraw map) */
		verify_panel();

		if (mpe_mode & MPE_FORGET_FLOW)
		{
			forget_flow();

			/* Mega-Hack -- Forget the view */
			p_ptr->update |= (PU_UN_VIEW);

			/* Redraw map */
			p_ptr->redraw |= (PR_MAP);
		}

		/* Update stuff */
		p_ptr->update |= (PU_VIEW | PU_LITE | PU_FLOW | PU_MON_LITE | PU_DISTANCE);

		{
			class_t *class_ptr = get_class_t();
			if (class_ptr && class_ptr->move_player)
				class_ptr->move_player();
		}

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);

		/* Remove "unsafe" flag */
		if ((!p_ptr->blind && !no_lite()) || !is_trap(c_ptr->feat)) c_ptr->info &= ~(CAVE_UNSAFE);

		/* For get everything when requested hehe I'm *NASTY* */
		if (dun_level && (d_info[dungeon_type].flags1 & DF1_FORGET)) wiz_dark();

		/* Handle stuff */
		if (mpe_mode & MPE_HANDLE_STUFF) handle_stuff();

		if (p_ptr->pclass == CLASS_NINJA || p_ptr->tim_superstealth)
		{
			if (c_ptr->info & (CAVE_GLOW)) set_superstealth(FALSE);
			else if (p_ptr->cur_lite <= 0) set_superstealth(TRUE);
		}

		if ((p_ptr->action == ACTION_HAYAGAKE) &&
		    (!have_flag(f_ptr->flags, FF_PROJECT) ||
		     (!p_ptr->levitation && have_flag(f_ptr->flags, FF_DEEP))))
		{
#ifdef JP
			msg_print("ここでは素早く動けない。");
#else
			msg_print("You cannot run in here.");
#endif
			set_action(ACTION_NONE);

			if (weaponmaster_get_toggle() == TOGGLE_SHADOW_STANCE)
			{
				msg_print("Your shadow stance is disrupted!");
				weaponmaster_set_toggle(TOGGLE_NONE);
			}
		}
	}

	if (mpe_mode & MPE_ENERGY_USE)
	{
		if (music_singing(MUSIC_WALL))
		{
			(void)project(0, 0, py, px, (60 + p_ptr->lev), GF_DISINTEGRATE,
				PROJECT_KILL | PROJECT_ITEM, -1);

			if (!player_bold(ny, nx) || p_ptr->is_dead || p_ptr->leaving) return FALSE;
		}

		/* Spontaneous Searching */
		if ((p_ptr->skills.fos >= 50) || (0 == randint0(50 - p_ptr->skills.fos)))
		{
			search();
		}

		/* Continuous Searching */
		if (p_ptr->action == ACTION_SEARCH)
		{
			search();
		}
	}

	/* Handle "objects" */
	if (!(mpe_mode & MPE_DONT_PICKUP))
	{
		carry((mpe_mode & MPE_DO_PICKUP) ? TRUE : FALSE);
	}

	/* Handle "store doors" */
	if (have_flag(f_ptr->flags, FF_STORE))
	{
		/* Disturb */
		disturb(0, 0);

		energy_use = 0;
		/* Hack -- Enter store */
		command_new = SPECIAL_KEY_STORE;
	}

	/* Handle "building doors" -KMW- */
	else if (have_flag(f_ptr->flags, FF_BLDG))
	{
		/* Disturb */
		disturb(0, 0);

		energy_use = 0;
		/* Hack -- Enter building */
		command_new = SPECIAL_KEY_BUILDING;
	}

	/* Handle quest areas -KMW- */
	else if (have_flag(f_ptr->flags, FF_QUEST_ENTER))
	{
		/* Disturb */
		disturb(0, 0);

		energy_use = 0;
		/* Hack -- Enter quest level */
		command_new = SPECIAL_KEY_QUEST;
	}

	else if (have_flag(f_ptr->flags, FF_QUEST_EXIT))
	{
		if (quest[p_ptr->inside_quest].type == QUEST_TYPE_FIND_EXIT)
		{
			if (record_fix_quest) do_cmd_write_nikki(NIKKI_FIX_QUEST_C, p_ptr->inside_quest, NULL);
			quest[p_ptr->inside_quest].status = QUEST_STATUS_COMPLETED;
			quest[p_ptr->inside_quest].complev = (byte)p_ptr->lev;
#ifdef JP
			msg_print("クエストを達成した！");
#else
			msg_print("You accomplished your quest!");
#endif

			msg_print(NULL);
		}

		leave_quest_check();

		p_ptr->inside_quest = c_ptr->special;
		dun_level = 0;
		p_ptr->oldpx = 0;
		p_ptr->oldpy = 0;

		p_ptr->leaving = TRUE;
	}

	/* Set off a trap */
	else if (have_flag(f_ptr->flags, FF_HIT_TRAP) && !(mpe_mode & MPE_STAYING))
	{
		/* Disturb */
		disturb(0, 0);

		/* Hidden trap */
		if (c_ptr->mimic || have_flag(f_ptr->flags, FF_SECRET))
		{
			/* Message */
#ifdef JP
			msg_print("トラップだ！");
#else
			msg_print("You found a trap!");
#endif

			/* Pick a trap */
			disclose_grid(py, px);
		}

		/* Hit the trap */
		hit_trap((mpe_mode & MPE_BREAK_TRAP) ? TRUE : FALSE);

		if (!player_bold(ny, nx) || p_ptr->is_dead || p_ptr->leaving) return FALSE;
	}

	/* Warn when leaving trap detected region */
	if (!(mpe_mode & MPE_STAYING) && (disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(c_ptr->info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(c_ptr->info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect) disturb(0, 0);
		}
	}

	return player_bold(ny, nx) && !p_ptr->is_dead && !p_ptr->leaving;
}


bool trap_can_be_ignored(int feat)
{
	feature_type *f_ptr = &f_info[feat];

	if (!have_flag(f_ptr->flags, FF_TRAP)) return TRUE;

	switch (f_ptr->subtype)
	{
	case TRAP_TRAPDOOR:
	case TRAP_PIT:
	case TRAP_SPIKED_PIT:
	case TRAP_POISON_PIT:
		if (p_ptr->levitation) return TRUE;
		break;
	case TRAP_TELEPORT:
		if (p_ptr->anti_tele) return TRUE;
		break;
	case TRAP_FIRE:
		if (p_ptr->resist_fire == RESIST_IMMUNE) return TRUE;
		break;
	case TRAP_ACID:
		if (p_ptr->resist_acid == RESIST_IMMUNE) return TRUE;
		break;
	case TRAP_BLIND:
		if (p_ptr->resist_blind) return TRUE;
		break;
	case TRAP_CONFUSE:
		if (p_ptr->resist_conf) return TRUE;
		break;
	case TRAP_POISON:
		if (p_ptr->resist_pois) return TRUE;
		break;
	case TRAP_SLEEP:
		if (p_ptr->free_act) return TRUE;
		break;
	}

	return FALSE;
}


/*
 * Determine if a "boundary" grid is "floor mimic"
 */
#define boundary_floor(C, F, MF) \
	((C)->mimic && permanent_wall(F) && \
	 (have_flag((MF)->flags, FF_MOVE) || have_flag((MF)->flags, FF_CAN_FLY)) && \
	 have_flag((MF)->flags, FF_PROJECT) && \
	 !have_flag((MF)->flags, FF_OPEN))

/*
 * Move player in the given direction, with the given "pickup" flag.
 *
 * This routine should (probably) always induce energy expenditure.
 *
 * Note that moving will *always* take a turn, and will *always* hit
 * any monster which might be in the destination grid.  Previously,
 * moving into walls was "free" and did NOT hit invisible monsters.
 */
void move_player(int dir, bool do_pickup, bool break_trap)
{
	/* Find the result of moving */
	int y = py + ddy[dir];
	int x = px + ddx[dir];

	/* Examine the destination */
	cave_type *c_ptr = &cave[y][x];

	feature_type *f_ptr = &f_info[c_ptr->feat];

	monster_type *m_ptr;

	monster_type *riding_m_ptr = &m_list[p_ptr->riding];
	monster_race *riding_r_ptr = &r_info[p_ptr->riding ? riding_m_ptr->r_idx : 0]; /* Paranoia */

	char m_name[80];

	bool p_can_enter = player_can_enter(c_ptr->feat, CEM_P_CAN_ENTER_PATTERN);
	bool p_can_kill_walls = FALSE;
	bool stormbringer = FALSE;
	bool shadow_strike = FALSE;
	bool oktomove = TRUE;
	bool do_past = FALSE;

	/* Exit the area */
	if (!dun_level && !p_ptr->wild_mode &&
		((x == 0) || (x == MAX_WID - 1) ||
		 (y == 0) || (y == MAX_HGT - 1)))
	{
		/* Can the player enter the grid? */
		if (c_ptr->mimic && player_can_enter(c_ptr->mimic, 0))
		{
			/* Hack: move to new area */
			if ((y == 0) && (x == 0))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = cur_hgt - 3;
				p_ptr->oldpx = cur_wid - 3;
				ambush_flag = FALSE;
			}

			else if ((y == 0) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y--;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = cur_hgt - 3;
				p_ptr->oldpx = 2;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == 0))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x--;
				p_ptr->oldpy = 2;
				p_ptr->oldpx = cur_wid - 3;
				ambush_flag = FALSE;
			}

			else if ((y == MAX_HGT - 1) && (x == MAX_WID - 1))
			{
				p_ptr->wilderness_y++;
				p_ptr->wilderness_x++;
				p_ptr->oldpy = 2;
				p_ptr->oldpx = 2;
				ambush_flag = FALSE;
			}

			else if (y == 0)
			{
				p_ptr->wilderness_y--;
				p_ptr->oldpy = cur_hgt - 3;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (y == MAX_HGT - 1)
			{
				p_ptr->wilderness_y++;
				p_ptr->oldpy = 2;
				p_ptr->oldpx = x;
				ambush_flag = FALSE;
			}

			else if (x == 0)
			{
				p_ptr->wilderness_x--;
				p_ptr->oldpx = cur_wid - 3;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}

			else if (x == MAX_WID - 1)
			{
				p_ptr->wilderness_x++;
				p_ptr->oldpx = 2;
				p_ptr->oldpy = y;
				ambush_flag = FALSE;
			}

			p_ptr->leaving = TRUE;
			energy_use = 100;

			return;
		}

		/* "Blocked" message appears later */
		/* oktomove = FALSE; */
		p_can_enter = FALSE;
	}

	/* Get the monster */
	m_ptr = &m_list[c_ptr->m_idx];


	if (inventory[INVEN_RARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;
	if (inventory[INVEN_LARM].name1 == ART_STORMBRINGER) stormbringer = TRUE;

	/* Player can not walk through "walls"... */
	/* unless in Shadow Form */
	p_can_kill_walls = p_ptr->kill_wall && have_flag(f_ptr->flags, FF_HURT_DISI) &&
		(!p_can_enter || !have_flag(f_ptr->flags, FF_LOS)) &&
		!have_flag(f_ptr->flags, FF_PERMANENT);

	/* Hack -- attack monsters */
	if (c_ptr->m_idx && (m_ptr->ml || p_can_enter || p_can_kill_walls))
	{
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Attack -- only if we can see it OR it is not in a wall */
		if (!is_hostile(m_ptr) &&
			!(p_ptr->confused || p_ptr->image || !m_ptr->ml || p_ptr->stun ||
			(mut_present(MUT_BERS_RAGE) && IS_SHERO())) &&
			pattern_seq(py, px, y, x) && (p_can_enter || p_can_kill_walls))
		{
			/* Disturb the monster */
			(void)set_monster_csleep(c_ptr->m_idx, 0);

			/* Extract monster name (or "it") */
			monster_desc(m_name, m_ptr, 0);

			if (m_ptr->ml)
			{
				/* Auto-Recall if possible and visible */
				if (!p_ptr->image) monster_race_track(m_ptr->ap_r_idx);

				/* Track a new monster */
				health_track(c_ptr->m_idx);
			}

			/* displace? */
			if ((stormbringer && (randint1(1000) > 666)) || (p_ptr->pclass == CLASS_BERSERKER))
			{
				py_attack(y, x, 0);
				oktomove = FALSE;
			}
			else if (monster_can_cross_terrain(cave[py][px].feat, r_ptr, 0))
			{
				do_past = TRUE;
			}
			else
			{
#ifdef JP
				msg_format("%^sが邪魔だ！", m_name);
#else
				msg_format("%^s is in your way!", m_name);
#endif

				energy_use = 0;
				oktomove = FALSE;
			}

			/* now continue on to 'movement' */
		}
		else
		{
			py_attack(y, x, 0);
			if (weaponmaster_get_toggle() == TOGGLE_SHADOW_STANCE)
				shadow_strike = TRUE;
			else
				oktomove = FALSE;
		}
	}

	if (oktomove && p_ptr->riding)
	{
		if (riding_r_ptr->flags1 & RF1_NEVER_MOVE)
		{
#ifdef JP
			msg_print("動けない！");
#else
			msg_print("Can't move!");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (MON_MONFEAR(riding_m_ptr))
		{
			char m_name[80];

			/* Acquire the monster name */
			monster_desc(m_name, riding_m_ptr, 0);

			/* Dump a message */
#ifdef JP
			msg_format("%sが恐怖していて制御できない。", m_name);
#else
			msg_format("%^s is too scared to control.", m_name);
#endif
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (p_ptr->riding_ryoute)
		{
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (have_flag(f_ptr->flags, FF_CAN_FLY) && (riding_r_ptr->flags7 & RF7_CAN_FLY))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_CAN_SWIM) && (riding_r_ptr->flags7 & RF7_CAN_SWIM))
		{
			/* Allow moving */
		}
		else if (have_flag(f_ptr->flags, FF_WATER) &&
			!(riding_r_ptr->flags7 & RF7_AQUATIC) &&
			(have_flag(f_ptr->flags, FF_DEEP) || (riding_r_ptr->flags2 & RF2_AURA_FIRE)))
		{
#ifdef JP
			msg_format("%sの上に行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Can't swim.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (!have_flag(f_ptr->flags, FF_WATER) && (riding_r_ptr->flags7 & RF7_AQUATIC))
		{
#ifdef JP
			msg_format("%sから上がれない。", f_name + f_info[get_feat_mimic(&cave[py][px])].name);
#else
			msg_print("Can't land.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}
		else if (have_flag(f_ptr->flags, FF_LAVA) && !(riding_r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK))
		{
#ifdef JP
			msg_format("%sの上に行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
			msg_print("Too hot to go through.");
#endif
			energy_use = 0;
			oktomove = FALSE;
			disturb(0, 0);
		}

		if (oktomove && MON_STUNNED(riding_m_ptr) && one_in_(2))
		{
			char m_name[80];
			monster_desc(m_name, riding_m_ptr, 0);
#ifdef JP
			msg_format("%sが朦朧としていてうまく動けない！",m_name);
#else
			msg_format("You cannot control stunned %s!",m_name);
#endif
			oktomove = FALSE;
			disturb(0, 0);
		}
	}

	if (!oktomove)
	{
	}

	else if (!have_flag(f_ptr->flags, FF_MOVE) && have_flag(f_ptr->flags, FF_CAN_FLY) && !p_ptr->levitation)
	{
#ifdef JP
		msg_format("空を飛ばないと%sの上には行けない。", f_name + f_info[get_feat_mimic(c_ptr)].name);
#else
		msg_format("You need to fly to go through the %s.", f_name + f_info[get_feat_mimic(c_ptr)].name);
#endif

		if (!shadow_strike)
			energy_use = 0;
		running = 0;
		oktomove = FALSE;
	}

	/*
	 * Player can move through trees and
	 * has effective -10 speed
	 * Rangers can move without penality
	 */
	else if (have_flag(f_ptr->flags, FF_TREE) && !p_can_kill_walls)
	{
		if ( p_ptr->pclass != CLASS_RANGER
		  && !prace_is_(RACE_ENT) 
		  && !p_ptr->levitation 
		  && (!p_ptr->riding || !(riding_r_ptr->flags8 & RF8_WILD_WOOD))) 
		{
			energy_use *= 2;
		}
	}

#ifdef ALLOW_EASY_DISARM /* TNB */

	/* Disarm a visible trap */
	else if ((do_pickup != easy_disarm) && have_flag(f_ptr->flags, FF_DISARM) && !c_ptr->mimic)
	{
		if (!trap_can_be_ignored(c_ptr->feat))
		{
			(void)do_cmd_disarm_aux(y, x, dir);
			return;
		}
	}

#endif /* ALLOW_EASY_DISARM -- TNB */

	/* Player can not walk through "walls" unless in wraith form...*/
	else if (!p_can_enter && !p_can_kill_walls)
	{
		/* Feature code (applying "mimic" field) */
		s16b feat = get_feat_mimic(c_ptr);
		feature_type *mimic_f_ptr = &f_info[feat];
		cptr name = f_name + mimic_f_ptr->name;

		oktomove = FALSE;

		/* Disturb the player */
		disturb(0, 0);

		/* Notice things in the dark */
		if (!(c_ptr->info & CAVE_MARK) && !player_can_see_bold(y, x))
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("それ以上先には進めないようだ。");
#else
				msg_print("You feel you cannot go any more.");
#endif
			}

			/* Wall (or secret door) */
			else
			{
#ifdef JP
				msg_format("%sが行く手をはばんでいるようだ。", name);
#else
				msg_format("You feel %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				c_ptr->info |= (CAVE_MARK);
				lite_spot(y, x);
			}
		}

		/* Notice things */
		else
		{
			/* Boundary floor mimic */
			if (boundary_floor(c_ptr, f_ptr, mimic_f_ptr))
			{
#ifdef JP
				msg_print("それ以上先には進めない。");
#else
				msg_print("You cannot go any more.");
#endif

				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
				{
					if (!shadow_strike)
						energy_use = 0;
				}
			}

			/* Wall (or secret door) */
			else
			{
#ifdef ALLOW_EASY_OPEN
				/* Closed doors */
				if (easy_open && is_closed_door(feat) && easy_open_door(y, x))
				{
					/* Hack. Try to deduce what happened since easy_open_door hides this.
					   Try to repeat attempting to unlock the door, but do a quick check
					   for jammed doors so we don't waste 99 turns. Also, only make
					   99 attempts to pick the lock ... But using command_rep would be
					   unwise since we will then run thru the door once we pick the lock! */
					if (always_repeat)
					{
						static int _repeat_count = 0;

						cave_type *c_ptr = &cave[y][x];
						feature_type *f_ptr = &f_info[c_ptr->feat];

						if (is_closed_door(c_ptr->feat) && have_flag(f_ptr->flags, FF_OPEN))
						{
							if (_repeat_count == 0)
								_repeat_count = 99;
							else
								--_repeat_count;

							if (_repeat_count)
								command_rep = 1;
						}
						else
							_repeat_count = 0;
					}
					return;
				}
#endif /* ALLOW_EASY_OPEN */

#ifdef JP
				msg_format("%sが行く手をはばんでいる。", name);
#else
				msg_format("There is %s %s blocking your way.",
					is_a_vowel(name[0]) ? "an" : "a", name);
#endif

				/*
				 * Well, it makes sense that you lose time bumping into
				 * a wall _if_ you are confused, stunned or blind; but
				 * typing mistakes should not cost you a turn...
				 */
				if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
				{
					if (!shadow_strike)
						energy_use = 0;
				}
			}
		}

		/* Sound */
		if (!boundary_floor(c_ptr, f_ptr, mimic_f_ptr)) sound(SOUND_HITWALL);
	}

	/* Normal movement */
	if (oktomove && !pattern_seq(py, px, y, x))
	{
		if (!(p_ptr->confused || p_ptr->stun || p_ptr->image))
		{
			if (!shadow_strike)
				energy_use = 0;
		}

		/* To avoid a loop with running */
		disturb(0, 0);

		oktomove = FALSE;
	}

	/* Normal movement */
	if (oktomove)
	{
		u32b mpe_mode = MPE_ENERGY_USE;

		if (p_ptr->warning)
		{
			if (!process_warning(x, y))
			{
				if (!shadow_strike)
					energy_use = 25;
				return;
			}
		}

		if (do_past)
		{
#ifdef JP
			msg_format("%sを押し退けた。", m_name);
#else
			msg_format("You push past %s.", m_name);
#endif
		}

		/* Change oldpx and oldpy to place the player well when going back to big mode */
		if (p_ptr->wild_mode)
		{
			if (ddy[dir] > 0)  p_ptr->oldpy = 1;
			if (ddy[dir] < 0)  p_ptr->oldpy = MAX_HGT - 2;
			if (ddy[dir] == 0) p_ptr->oldpy = MAX_HGT / 2;
			if (ddx[dir] > 0)  p_ptr->oldpx = 1;
			if (ddx[dir] < 0)  p_ptr->oldpx = MAX_WID - 2;
			if (ddx[dir] == 0) p_ptr->oldpx = MAX_WID / 2;
		}

		if (p_can_kill_walls)
		{
			cave_alter_feat(y, x, FF_HURT_DISI);

			/* Update some things -- similar to GF_KILL_WALL */
			p_ptr->update |= (PU_FLOW);
		}

		/* Sound */
		/* sound(SOUND_WALK); */

#ifdef ALLOW_EASY_DISARM /* TNB */

		if (do_pickup != always_pickup) mpe_mode |= MPE_DO_PICKUP;

#else /* ALLOW_EASY_DISARM -- TNB */

		if (do_pickup) mpe_mode |= MPE_DO_PICKUP;

#endif /* ALLOW_EASY_DISARM -- TNB */

		if (break_trap) mpe_mode |= MPE_BREAK_TRAP;

		/* Move the player */
		(void)move_player_effect(y, x, mpe_mode);
	}
}


static bool ignore_avoid_run;

/*
 * Hack -- Check for a "known wall" (see below)
 */
static int see_wall(int dir, int y, int x)
{
	cave_type   *c_ptr;

	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are not known walls */
	if (!in_bounds2(y, x)) return (FALSE);

	/* Access grid */
	c_ptr = &cave[y][x];

	/* Must be known to the player */
	if (c_ptr->info & (CAVE_MARK))
	{
		/* Feature code (applying "mimic" field) */
		s16b         feat = get_feat_mimic(c_ptr);
		feature_type *f_ptr = &f_info[feat];

		/* Wall grids are known walls */
		if (!player_can_enter(feat, 0)) return !have_flag(f_ptr->flags, FF_DOOR);

		/* Don't run on a tree unless explicitly requested */
		if (have_flag(f_ptr->flags, FF_AVOID_RUN) && !ignore_avoid_run)
			return TRUE;

		/* Don't run in a wall */
		if (!have_flag(f_ptr->flags, FF_MOVE) && !have_flag(f_ptr->flags, FF_CAN_FLY))
			return !have_flag(f_ptr->flags, FF_DOOR);
	}

	return FALSE;
}


/*
 * Hack -- Check for an "unknown corner" (see below)
 */
static int see_nothing(int dir, int y, int x)
{
	/* Get the new location */
	y += ddy[dir];
	x += ddx[dir];

	/* Illegal grids are unknown */
	if (!in_bounds2(y, x)) return (TRUE);

	/* Memorized grids are always known */
	if (cave[y][x].info & (CAVE_MARK)) return (FALSE);

	/* Viewable door/wall grids are known */
	if (player_can_see_bold(y, x)) return (FALSE);

	/* Default */
	return (TRUE);
}





/*
 * The running algorithm:                       -CJS-
 *
 * In the diagrams below, the player has just arrived in the
 * grid marked as '@', and he has just come from a grid marked
 * as 'o', and he is about to enter the grid marked as 'x'.
 *
 * Of course, if the "requested" move was impossible, then you
 * will of course be blocked, and will stop.
 *
 * Overview: You keep moving until something interesting happens.
 * If you are in an enclosed space, you follow corners. This is
 * the usual corridor scheme. If you are in an open space, you go
 * straight, but stop before entering enclosed space. This is
 * analogous to reaching doorways. If you have enclosed space on
 * one side only (that is, running along side a wall) stop if
 * your wall opens out, or your open space closes in. Either case
 * corresponds to a doorway.
 *
 * What happens depends on what you can really SEE. (i.e. if you
 * have no light, then running along a dark corridor is JUST like
 * running in a dark room.) The algorithm works equally well in
 * corridors, rooms, mine tailings, earthquake rubble, etc, etc.
 *
 * These conditions are kept in static memory:
 * find_openarea         You are in the open on at least one
 * side.
 * find_breakleft        You have a wall on the left, and will
 * stop if it opens
 * find_breakright       You have a wall on the right, and will
 * stop if it opens
 *
 * To initialize these conditions, we examine the grids adjacent
 * to the grid marked 'x', two on each side (marked 'L' and 'R').
 * If either one of the two grids on a given side is seen to be
 * closed, then that side is considered to be closed. If both
 * sides are closed, then it is an enclosed (corridor) run.
 *
 * LL           L
 * @x          LxR
 * RR          @R
 *
 * Looking at more than just the immediate squares is
 * significant. Consider the following case. A run along the
 * corridor will stop just before entering the center point,
 * because a choice is clearly established. Running in any of
 * three available directions will be defined as a corridor run.
 * Note that a minor hack is inserted to make the angled corridor
 * entry (with one side blocked near and the other side blocked
 * further away from the runner) work correctly. The runner moves
 * diagonally, but then saves the previous direction as being
 * straight into the gap. Otherwise, the tail end of the other
 * entry would be perceived as an alternative on the next move.
 *
 * #.#
 * ##.##
 * .@x..
 * ##.##
 * #.#
 *
 * Likewise, a run along a wall, and then into a doorway (two
 * runs) will work correctly. A single run rightwards from @ will
 * stop at 1. Another run right and down will enter the corridor
 * and make the corner, stopping at the 2.
 *
 * ##################
 * o@x       1
 * ########### ######
 * #2          #
 * #############
 *
 * After any move, the function area_affect is called to
 * determine the new surroundings, and the direction of
 * subsequent moves. It examines the current player location
 * (at which the runner has just arrived) and the previous
 * direction (from which the runner is considered to have come).
 *
 * Moving one square in some direction places you adjacent to
 * three or five new squares (for straight and diagonal moves
 * respectively) to which you were not previously adjacent,
 * marked as '!' in the diagrams below.
 *
 *   ...!              ...
 *   .o@!  (normal)    .o.!  (diagonal)
 *   ...!  (east)      ..@!  (south east)
 *                      !!!
 *
 * You STOP if any of the new squares are interesting in any way:
 * for example, if they contain visible monsters or treasure.
 *
 * You STOP if any of the newly adjacent squares seem to be open,
 * and you are also looking for a break on that side. (that is,
 * find_openarea AND find_break).
 *
 * You STOP if any of the newly adjacent squares do NOT seem to be
 * open and you are in an open area, and that side was previously
 * entirely open.
 *
 * Corners: If you are not in the open (i.e. you are in a corridor)
 * and there is only one way to go in the new squares, then turn in
 * that direction. If there are more than two new ways to go, STOP.
 * If there are two ways to go, and those ways are separated by a
 * square which does not seem to be open, then STOP.
 *
 * Otherwise, we have a potential corner. There are two new open
 * squares, which are also adjacent. One of the new squares is
 * diagonally located, the other is straight on (as in the diagram).
 * We consider two more squares further out (marked below as ?).
 *
 * We assign "option" to the straight-on grid, and "option2" to the
 * diagonal grid, and "check_dir" to the grid marked 's'.
 *
 * ##s
 * @x?
 * #.?
 *
 * If they are both seen to be closed, then it is seen that no benefit
 * is gained from moving straight. It is a known corner.  To cut the
 * corner, go diagonally, otherwise go straight, but pretend you
 * stepped diagonally into that next location for a full view next
 * time. Conversely, if one of the ? squares is not seen to be closed,
 * then there is a potential choice. We check to see whether it is a
 * potential corner or an intersection/room entrance.  If the square
 * two spaces straight ahead, and the space marked with 's' are both
 * unknown space, then it is a potential corner and enter if
 * find_examine is set, otherwise must stop because it is not a
 * corner. (find_examine option is removed and always is TRUE.)
 */




/*
 * Hack -- allow quick "cycling" through the legal directions
 */
static byte cycle[] =
{ 1, 2, 3, 6, 9, 8, 7, 4, 1, 2, 3, 6, 9, 8, 7, 4, 1 };

/*
 * Hack -- map each direction into the "middle" of the "cycle[]" array
 */
static byte chome[] =
{ 0, 8, 9, 10, 7, 0, 11, 6, 5, 4 };

/*
 * The direction we are running
 */
static byte find_current;

/*
 * The direction we came from
 */
static byte find_prevdir;

/*
 * We are looking for open area
 */
static bool find_openarea;

/*
 * We are looking for a break
 */
static bool find_breakright;
static bool find_breakleft;



/*
 * Initialize the running algorithm for a new direction.
 *
 * Diagonal Corridor -- allow diaginal entry into corridors.
 *
 * Blunt Corridor -- If there is a wall two spaces ahead and
 * we seem to be in a corridor, then force a turn into the side
 * corridor, must be moving straight into a corridor here. ???
 *
 * Diagonal Corridor    Blunt Corridor (?)
 *       # #                  #
 *       #x#                 @x#
 *       @p.                  p
 */
static void run_init(int dir)
{
	int             row, col, deepleft, deepright;
	int             i, shortleft, shortright;


	/* Save the direction */
	find_current = dir;

	/* Assume running straight */
	find_prevdir = dir;

	/* Assume looking for open area */
	find_openarea = TRUE;

	/* Assume not looking for breaks */
	find_breakright = find_breakleft = FALSE;

	/* Assume no nearby walls */
	deepleft = deepright = FALSE;
	shortright = shortleft = FALSE;

	p_ptr->run_py = py;
	p_ptr->run_px = px;

	/* Find the destination grid */
	row = py + ddy[dir];
	col = px + ddx[dir];

	ignore_avoid_run = cave_have_flag_bold(row, col, FF_AVOID_RUN);

	/* Extract cycle index */
	i = chome[dir];

	/* Check for walls */
	if (see_wall(cycle[i+1], py, px))
	{
		find_breakleft = TRUE;
		shortleft = TRUE;
	}
	else if (see_wall(cycle[i+1], row, col))
	{
		find_breakleft = TRUE;
		deepleft = TRUE;
	}

	/* Check for walls */
	if (see_wall(cycle[i-1], py, px))
	{
		find_breakright = TRUE;
		shortright = TRUE;
	}
	else if (see_wall(cycle[i-1], row, col))
	{
		find_breakright = TRUE;
		deepright = TRUE;
	}

	/* Looking for a break */
	if (find_breakleft && find_breakright)
	{
		/* Not looking for open area */
		find_openarea = FALSE;

		/* Hack -- allow angled corridor entry */
		if (dir & 0x01)
		{
			if (deepleft && !deepright)
			{
				find_prevdir = cycle[i - 1];
			}
			else if (deepright && !deepleft)
			{
				find_prevdir = cycle[i + 1];
			}
		}

		/* Hack -- allow blunt corridor entry */
		else if (see_wall(cycle[i], row, col))
		{
			if (shortleft && !shortright)
			{
				find_prevdir = cycle[i - 2];
			}
			else if (shortright && !shortleft)
			{
				find_prevdir = cycle[i + 2];
			}
		}
	}
}


/*
 * Update the current "run" path
 *
 * Return TRUE if the running should be stopped
 */
static bool run_test(void)
{
	int         prev_dir, new_dir, check_dir = 0;
	int         row, col;
	int         i, max, inv;
	int         option = 0, option2 = 0;
	cave_type   *c_ptr;
	s16b        feat;
	feature_type *f_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;


	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		s16b this_o_idx, next_o_idx = 0;

		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];

		/* Feature code (applying "mimic" field) */
		feat = get_feat_mimic(c_ptr);
		f_ptr = &f_info[feat];

		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}

		/* Visible objects abort running */
		for (this_o_idx = c_ptr->o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Acquire object */
			o_ptr = &o_list[this_o_idx];

			/* Acquire next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Visible object */
			if (o_ptr->marked & OM_FOUND) return (TRUE);
		}

		/* Assume unknown */
		inv = TRUE;

		/* Check memorized grids */
		if (c_ptr->info & (CAVE_MARK))
		{
			bool notice = have_flag(f_ptr->flags, FF_NOTICE);

			if (notice && have_flag(f_ptr->flags, FF_MOVE))
			{
				/* Open doors */
				if (find_ignore_doors && have_flag(f_ptr->flags, FF_DOOR) && have_flag(f_ptr->flags, FF_CLOSE))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Stairs */
				else if (find_ignore_stairs && have_flag(f_ptr->flags, FF_STAIRS))
				{
					/* Option -- ignore */
					notice = FALSE;
				}

				/* Lava */
				else if (have_flag(f_ptr->flags, FF_LAVA) && (p_ptr->resist_fire == RESIST_IMMUNE || IS_INVULN()))
				{
					/* Ignore */
					notice = FALSE;
				}

				/* Deep water */
				else if (have_flag(f_ptr->flags, FF_WATER) && have_flag(f_ptr->flags, FF_DEEP) &&
				         (p_ptr->levitation || p_ptr->can_swim || (p_ptr->total_weight <= weight_limit())))
				{
					/* Ignore */
					notice = FALSE;
				}
			}

			/* Interesting feature */
			if (notice) return (TRUE);

			/* The grid is "visible" */
			inv = FALSE;
		}

		/* Analyze unknown grids and floors considering mimic */
		if (inv || !see_wall(0, row, col))
		{
			/* Looking for open area */
			if (find_openarea)
			{
				/* Nothing */
			}

			/* The first new direction. */
			else if (!option)
			{
				option = new_dir;
			}

			/* Three new directions. Stop running. */
			else if (option2)
			{
				return (TRUE);
			}

			/* Two non-adjacent new directions.  Stop running. */
			else if (option != cycle[chome[prev_dir] + i - 1])
			{
				return (TRUE);
			}

			/* Two new (adjacent) directions (case 1) */
			else if (new_dir & 0x01)
			{
				check_dir = cycle[chome[prev_dir] + i - 2];
				option2 = new_dir;
			}

			/* Two new (adjacent) directions (case 2) */
			else
			{
				check_dir = cycle[chome[prev_dir] + i + 1];
				option2 = option;
				option = new_dir;
			}
		}

		/* Obstacle, while looking for open area */
		else
		{
			if (find_openarea)
			{
				if (i < 0)
				{
					/* Break to the right */
					find_breakright = TRUE;
				}

				else if (i > 0)
				{
					/* Break to the left */
					find_breakleft = TRUE;
				}
			}
		}
	}

	/* Looking for open area */
	if (find_openarea)
	{
		/* Hack -- look again */
		for (i = -max; i < 0; i++)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cycle[chome[prev_dir] + i], py, px))
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}
		}

		/* Hack -- look again */
		for (i = max; i > 0; i--)
		{
			/* Unknown grid or non-wall */
			if (!see_wall(cycle[chome[prev_dir] + i], py, px))
			{
				/* Looking to break left */
				if (find_breakleft)
				{
					return (TRUE);
				}
			}

			/* Obstacle */
			else
			{
				/* Looking to break right */
				if (find_breakright)
				{
					return (TRUE);
				}
			}
		}
	}

	/* Not looking for open area */
	else
	{
		/* No options */
		if (!option)
		{
			return (TRUE);
		}

		/* One option */
		else if (!option2)
		{
			/* Primary option */
			find_current = option;

			/* No other options */
			find_prevdir = option;
		}

		/* Two options, examining corners */
		else if (!find_cut)
		{
			/* Primary option */
			find_current = option;

			/* Hack -- allow curving */
			find_prevdir = option2;
		}

		/* Two options, pick one */
		else
		{
			/* Get next location */
			row = py + ddy[option];
			col = px + ddx[option];

			/* Don't see that it is closed off. */
			/* This could be a potential corner or an intersection. */
			if (!see_wall(option, row, col) ||
			    !see_wall(check_dir, row, col))
			{
				/* Can not see anything ahead and in the direction we */
				/* are turning, assume that it is a potential corner. */
				if (see_nothing(option, row, col) &&
				    see_nothing(option2, row, col))
				{
					find_current = option;
					find_prevdir = option2;
				}

				/* STOP: we are next to an intersection or a room */
				else
				{
					return (TRUE);
				}
			}

			/* This corner is seen to be enclosed; we cut the corner. */
			else if (find_cut)
			{
				find_current = option2;
				find_prevdir = option2;
			}

			/* This corner is seen to be enclosed, and we */
			/* deliberately go the long way. */
			else
			{
				find_current = option;
				find_prevdir = option2;
			}
		}
	}

	/* About to hit a known wall, stop */
	if (see_wall(find_current, py, px))
	{
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}



/*
 * Take one step along the current "run" path
 */
void run_step(int dir)
{
	/* Start running */
	if (dir)
	{
		/* Ignore AVOID_RUN on a first step */
		ignore_avoid_run = TRUE;

		/* Hack -- do not start silly run */
		if (see_wall(dir, py, px))
		{
			/* Message */
#ifdef JP
			msg_print("その方向には走れません。");
#else
			msg_print("You cannot run in that direction.");
#endif

			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}

		/* Initialize */
		run_init(dir);
	}

	/* Keep running */
	else
	{
		/* Update run */
		if (run_test())
		{
			/* Disturb */
			disturb(0, 0);

			/* Done */
			return;
		}
	}

	/* Decrease the run counter */
	if (--running <= 0) return;

	/* Take time */
	energy_use = 100;

	/* Move the player, using the "pickup" flag */
#ifdef ALLOW_EASY_DISARM /* TNB */

	move_player(find_current, FALSE, FALSE);

#else /* ALLOW_EASY_DISARM -- TNB */

	move_player(find_current, always_pickup, FALSE);

#endif /* ALLOW_EASY_DISARM -- TNB */

	if (player_bold(p_ptr->run_py, p_ptr->run_px))
	{
		p_ptr->run_py = 0;
		p_ptr->run_px = 0;
		disturb(0, 0);
	}
}


#ifdef TRAVEL
/*
 * Test for traveling
 */
static bool travel_test(void)
{
	int prev_dir, new_dir, check_dir = 0;
	int row, col;
	int i, max;
	bool stop = TRUE;
	cave_type *c_ptr;

	/* Where we came from */
	prev_dir = find_prevdir;

	/* Range of newly adjacent grids */
	max = (prev_dir & 0x01) + 1;

	for (i = 0; i < 8; i++)
	{
		if (travel.cost[py+ddy_ddd[i]][px+ddx_ddd[i]] < travel.cost[py][px]) stop = FALSE;
	}

	if (stop) return (TRUE);

	/* break run when leaving trap detected region */
	if ((disturb_trap_detect || alert_trap_detect)
	    && p_ptr->dtrap && !(cave[py][px].info & CAVE_IN_DETECT))
	{
		/* No duplicate warning */
		p_ptr->dtrap = FALSE;

		/* You are just on the edge */
		if (!(cave[py][px].info & CAVE_UNSAFE))
		{
			if (alert_trap_detect)
			{
#ifdef JP
				msg_print("* 注意:この先はトラップの感知範囲外です！ *");
#else
				msg_print("*Leaving trap detect region!*");
#endif
			}

			if (disturb_trap_detect)
			{
				/* Break Run */
				return(TRUE);
			}
		}
	}

	/* Cannot travel when blind */
	if (p_ptr->blind || no_lite())
	{
#ifdef JP
		msg_print("目が見えない！");
#else
		msg_print("You cannot see!");
#endif
		return (TRUE);
	}

	/* Look at every newly adjacent square. */
	for (i = -max; i <= max; i++)
	{
		/* New direction */
		new_dir = cycle[chome[prev_dir] + i];

		/* New location */
		row = py + ddy[new_dir];
		col = px + ddx[new_dir];

		/* Access grid */
		c_ptr = &cave[row][col];


		/* Visible monsters abort running */
		if (c_ptr->m_idx)
		{
			monster_type *m_ptr = &m_list[c_ptr->m_idx];

			/* Visible monster */
			if (m_ptr->ml) return (TRUE);
		}
	}

	/* Failure */
	return (FALSE);
}


/*
 * Travel command
 */
void travel_step(void)
{
	int i;
	int dir = travel.dir;
	int old_run = travel.run;

	find_prevdir = dir;

	/* disturb */
	if (travel_test())
	{
		if (travel.run == 255)
		{
#ifdef JP
			msg_print("道筋が見つかりません！");
#else
			msg_print("No route is found!");
#endif
		}
		disturb(0, 0);
		return;
	}

	energy_use = 100;

	for (i = 1; i <= 9; i++)
	{
		if (i == 5) continue;

		if (travel.cost[py+ddy[i]][px+ddx[i]] < travel.cost[py+ddy[dir]][px+ddx[dir]])
		{
			dir = i;
		}
	}

	/* Close door */
	if (!easy_open && is_closed_door(cave[py+ddy[dir]][px+ddx[dir]].feat))
	{
		disturb(0, 0);
		return;
	}

	travel.dir = dir;
	move_player(dir, always_pickup, easy_disarm);
	travel.run = old_run;

	if ((py == travel.y) && (px == travel.x))
		travel.run = 0;
	else
		travel.run--;
}
#endif
