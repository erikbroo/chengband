/* File: melee1.c */

/* Purpose: Monster attacks */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Critical blow.  All hits that do 95% of total possible damage,
 * and which also do at least 20 damage, or, sometimes, N damage.
 * This is used only to determine "cuts" and "stuns".
 */
static int monster_critical(int dice, int sides, int dam)
{
	int max = 0;
	int total = dice * sides;

	/* Must do at least 95% of perfect */
	if (dam < total * 19 / 20) return (0);

	/* Weak blows rarely work */
	if ((dam < 20) && (randint0(100) >= dam)) return (0);

	/* Perfect damage */
	if ((dam >= total) && (dam >= 40)) max++;

	/* Super-charge */
	if (dam >= 20)
	{
		while (randint0(100) < 2) max++;
	}

	/* Critical damage */
	if (dam > 45) return (6 + max);
	if (dam > 33) return (5 + max);
	if (dam > 25) return (4 + max);
	if (dam > 18) return (3 + max);
	if (dam > 11) return (2 + max);
	return (1 + max);
}





/*
 * Determine if a monster attack against the player succeeds.
 * Always miss 5% of the time, Always hit 5% of the time.
 * Otherwise, match monster power against player armor.
 */
int check_hit(int power, int level, int stun, int m_idx)
{
	int i, k, ac;

	/* Percentile dice */
	k = randint0(100);

	if (stun && one_in_(2)) return FALSE;

	/* Hack -- Always miss or hit */
	if (k < 10) return (k < 5);

	/* Calculate the "attack quality" */
	i = (power + (level * 3));

	/* Total armor */
	ac = p_ptr->ac + p_ptr->to_a;
	if (p_ptr->special_attack & ATTACK_SUIKEN) ac += (p_ptr->lev * 2);

	if ( p_ptr->pclass == CLASS_DUELIST
	  && p_ptr->duelist_target_idx == m_idx )
	{
		ac += 100;
	}

	/* Power and Level compete against Armor */
	/* Hack: Calc was AC * 3/4 ... */
	if ((i > 0) && (randint1(i) > ((ac * 21) / 32))) return (TRUE);

	/* Assume miss */
	return (FALSE);
}



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_insult[] =
{
#ifdef JP
	"�����ʤ����������",
	"�����ʤ�������������",
	"�����ʤ�����Τ�����",
	"�����ʤ��򿫤᤿��",
	"�����ʤ����������",
	"�����ʤ��β����٤ä���",
	"�������ʿȤ֤�򤷤���",
	"�����ʤ���ܤ���ȸ���������",
	"�����ʤ���ѥ饵���ȸƤФ�ꤷ����",
	"�����ʤ��򥵥��ܡ�������������"
#else
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
	"calls you a parasite!",
	"calls you a cyborg!"
#endif

};



/*
 * Hack -- possible "insult" messages
 */
static cptr desc_moan[] =
{
#ifdef JP
	"�ϲ������ᤷ��Ǥ���褦����",
	"����λ������򸫤ʤ��ä����ȿҤͤƤ��롣",
	"����ĥ�꤫��ФƹԤ��ȸ��äƤ��롣",
	"�ϥ��Υ����ɤ��Ȥ��줤�Ƥ��롣"
#else
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
#endif

};

int reduce_melee_dam_p(int dam)
{
	int result = dam;
	switch (weaponmaster_get_toggle())
	{
	case TOGGLE_STONE_BONES:
		result -= p_ptr->lev * p_ptr->lev / 50;
		break;

	case TOGGLE_BULWARK:
		result -= (result + 2)/3;
		break;
	}

	if (result < 0) result = 0;
	return result;
}

/* Apologies for the following hacks ... I'm too lazy to refactor monster attack code! */
bool nemesis_hack = FALSE;
bool retaliation_hack = FALSE;
int retaliation_count = 0;

static int _aura_dam_p(void)
{
	return 2 + damroll(1 + (p_ptr->lev / 10), 2 + (p_ptr->lev / 10));
}

/*
 * Attack the player via physical attacks.
 */
bool make_attack_normal(int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	int ap_cnt, ht_cnt;

	int i, k, tmp, ac, rlev;
	int do_cut, do_stun;

	s32b gold;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char m_name[80];

	char ddesc[80];

	bool blinked;
	bool touched = FALSE, fear = FALSE, alive = TRUE;
	bool explode = FALSE;
	bool do_silly_attack = (one_in_(2) && p_ptr->image);
	int get_damage = 0;
#ifdef JP
	int abbreviate = 0;
#endif

	/* Not allowed to attack */
	if (r_ptr->flags1 & (RF1_NEVER_BLOW)) return (FALSE);

	if (d_info[dungeon_type].flags1 & DF1_NO_MELEE) return (FALSE);

	/* ...nor if friendly */
	if (!is_hostile(m_ptr)) return FALSE;

	/* Extract the effective monster level */
	rlev = r_ptr->melee_level;
	if (!rlev)
		rlev = r_ptr->level;
	if (!rlev)
		rlev = 1; /* Townies */

	/* Get the monster name (or "it") */
	monster_desc(m_name, m_ptr, 0);

	/* Get the "died from" information (i.e. "a kobold") */
	monster_desc(ddesc, m_ptr, MD_IGNORE_HALLU | MD_ASSUME_VISIBLE | MD_INDEF_VISIBLE);

	if (p_ptr->special_defense & KATA_IAI)
	{
#ifdef JP
		msg_print("��꤬�����������������᤯���򿶤�ä���");
#else
		msg_format("You took sen, draw and cut in one motion before %s move.", m_name);
#endif
		if (py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_IAI)) return TRUE;
	}

	if ((p_ptr->special_defense & NINJA_KAWARIMI) && (randint0(55) < (p_ptr->lev*3/5+20)))
	{
		if (kawarimi(TRUE)) return TRUE;
	}

	/* Assume no blink */
	blinked = FALSE;

	/* Scan through all four blows */
	nemesis_hack = FALSE;
	ht_cnt = 0;
	for (ap_cnt = 0; ap_cnt < 4; ap_cnt++)
	{
		bool obvious = FALSE;

		int power = 0;
		int damage = 0;

		cptr act = NULL;

		int effect;
		int method;
		int d_dice;
		int d_side;

		/* Revenge aura only gives a single retaliatory attempt per player strike 
		   We'll cycle thru monster attacks on each revenge strike, and the revenge
		   will stop after the monster runs out of attacks.  So 20 attack players need
		   not fear insta-death (as much).  All the hackery is communicated between
		   here, py_attack_* and touch_zap_player().  Enjoy! 
		   (Note: If I had a mon_attack_p(int m_idx, int blow_idx), we could
		   avoid all this nonsense ... )
		 */
		if (retaliation_hack)
		{
			ap_cnt = retaliation_count;
			if (ap_cnt >= 4) return FALSE;
		}

		/* Extract the attack infomation */
		effect = r_ptr->blow[ap_cnt].effect;
		method = r_ptr->blow[ap_cnt].method;
		d_dice = r_ptr->blow[ap_cnt].d_dice;
		d_side = r_ptr->blow[ap_cnt].d_side;

		if (!m_ptr->r_idx) break;

		/* Call off the attacks on the Duelist's Nemesis power */
		if (nemesis_hack) break;

		/* Hack -- no more attacks */
		if (!method) break;

		if (is_pet(m_ptr) && (r_ptr->flags1 & RF1_UNIQUE) && (method == RBM_EXPLODE))
		{
			method = RBM_HIT;
			d_dice /= 10;
		}

		/* Stop if player is dead or gone */
		if (!p_ptr->playing || p_ptr->is_dead) break;
		if (distance(py, px, m_ptr->fy, m_ptr->fx) > 1) break;

		/* Handle "leaving" */
		if (p_ptr->leaving) break;

		if (method == RBM_SHOOT)
		{
			if (retaliation_hack) break;
			 continue;
		}

		if (retaliation_hack)
		{
			if (m_ptr->ml)
				msg_format("%^s retaliates!", m_name);
			if (is_original_ap_and_seen(m_ptr))
				r_ptr->r_flags2 |= RF2_AURA_REVENGE;
		}

		/* Extract the attack "power" */
		power = mbe_info[effect].power;

		/* Total armor */
		ac = p_ptr->ac + p_ptr->to_a;

		/* Monster hits player */
		if (!effect || check_hit(power, rlev, MON_STUNNED(m_ptr), m_idx))
		{
			/* Always disturbing */
			disturb(1, 0);

			/* Hack -- Apply "protection from evil" */
			if (IS_PROT_EVIL() &&
			    (r_ptr->flags3 & RF3_EVIL) &&
				!mon_save_p(m_ptr->r_idx, A_WIS) &&
				!one_in_(3))
			{
				/* Remember the Evil-ness */
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flags3 |= RF3_EVIL;

				/* Message */
#ifdef JP
				if (abbreviate)
				    msg_format("���ष����");
				else
				    msg_format("%^s�Ϸ��व�줿��", m_name);
				abbreviate = 1;/*�����ܰʹߤϾ�ά */
#else
				msg_format("%^s is repelled.", m_name);
#endif


				/* Hack -- Next attack */
				if (retaliation_hack) break;
				continue;
			}
			ht_cnt++;

			/* Assume no cut or stun */
			do_cut = do_stun = 0;

			/* Describe the attack method */
			switch (method)
			{
				case RBM_HIT:
				{
#ifdef JP
					act = "����줿��";
#else
					act = "hits you.";
#endif

					do_cut = do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

				case RBM_TOUCH:
				{
#ifdef JP
					act = "����줿��";
#else
					act = "touches you.";
#endif

					touched = TRUE;
					sound(SOUND_TOUCH);
					break;
				}

				case RBM_PUNCH:
				{
#ifdef JP
					act = "�ѥ�����줿��";
#else
					act = "punches you.";
#endif

					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

				case RBM_KICK:
				{
#ifdef JP
					act = "����줿��";
#else
					act = "kicks you.";
#endif

					touched = TRUE;
					do_stun = 1;
					sound(SOUND_HIT);
					break;
				}

				case RBM_CLAW:
				{
#ifdef JP
					act = "�Ҥä����줿��";
#else
					act = "claws you.";
#endif

					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

				case RBM_BITE:
				{
#ifdef JP
					act = "���ޤ줿��";
#else
					act = "bites you.";
#endif

					do_cut = 1;
					touched = TRUE;
					sound(SOUND_BITE);
					break;
				}

				case RBM_STING:
				{
#ifdef JP
					act = "�ɤ��줿��";
#else
					act = "stings you.";
#endif

					touched = TRUE;
					sound(SOUND_STING);
					break;
				}

				case RBM_SLASH:
				{
#ifdef JP
					act = "�¤�줿��";
#else
					act = "slashes you.";
#endif

					touched = TRUE;
					do_cut = 1;
					sound(SOUND_CLAW);
					break;
				}

				case RBM_BUTT:
				{
#ifdef JP
					act = "�Ѥ��ͤ��줿��";
#else
					act = "butts you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_HIT);
					break;
				}

				case RBM_CRUSH:
				{
#ifdef JP
					act = "�������ꤵ�줿��";
#else
					act = "crushes you.";
#endif

					do_stun = 1;
					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

				case RBM_ENGULF:
				{
#ifdef JP
					act = "���߹��ޤ줿��";
#else
					act = "engulfs you.";
#endif

					touched = TRUE;
					sound(SOUND_CRUSH);
					break;
				}

				case RBM_CHARGE:
				{
#ifdef JP
					abbreviate = -1;
					act = "��������褳������";
#else
					act = "charges you.";
#endif

					touched = TRUE;
					sound(SOUND_BUY); /* Note! This is "charges", not "charges at". */
					break;
				}

				case RBM_CRAWL:
				{
#ifdef JP
					abbreviate = -1;
					act = "���Τξ���礤��ä���";
#else
					act = "crawls on you.";
#endif

					touched = TRUE;
					sound(SOUND_SLIME);
					break;
				}

				case RBM_DROOL:
				{
#ifdef JP
					act = "�����򤿤餵�줿��";
#else
					act = "drools on you.";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_SPIT:
				{
#ifdef JP
					act = "�ä��Ǥ��줿��";
#else
					act = "spits on you.";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_EXPLODE:
				{
#ifdef JP
					abbreviate = -1;
					act = "����ȯ������";
#else
					act = "explodes.";
#endif

					explode = TRUE;
					break;
				}

				case RBM_GAZE:
				{
#ifdef JP
					act = "�ˤ�ޤ줿��";
#else
					act = "gazes at you.";
#endif

					break;
				}

				case RBM_WAIL:
				{
#ifdef JP
					act = "�㤭���Ф줿��";
#else
					act = "wails at you.";
#endif

					sound(SOUND_WAIL);
					break;
				}

				case RBM_SPORE:
				{
#ifdef JP
					act = "˦�Ҥ����Ф��줿��";
#else
					act = "releases spores at you.";
#endif

					sound(SOUND_SLIME);
					break;
				}

				case RBM_XXX4:
				{
#ifdef JP
					abbreviate = -1;
					act = "�� XXX4 ��ȯ�ͤ�����";
#else
					act = "projects XXX4's at you.";
#endif

					break;
				}

				case RBM_BEG:
				{
#ifdef JP
					act = "��򤻤��ޤ줿��";
#else
					act = "begs you for money.";
#endif

					sound(SOUND_MOAN);
					break;
				}

				case RBM_INSULT:
				{
#ifdef JP
					abbreviate = -1;
#endif
					act = desc_insult[randint0(m_ptr->r_idx == MON_DEBBY ? 10 : 8)];
					sound(SOUND_MOAN);
					break;
				}

				case RBM_MOAN:
				{
#ifdef JP
					abbreviate = -1;
#endif
					act = desc_moan[randint0(4)];
					sound(SOUND_MOAN);
					break;
				}

				case RBM_SHOW:
				{
#ifdef JP
					abbreviate = -1;
#endif
					if (m_ptr->r_idx == MON_JAIAN)
					{
#ifdef JP
						switch(randint1(15))
						{
						  case 1:
						  case 6:
						  case 11:
							act = "�֢�������ϥ��㥤��������������������礦����";
							break;
						  case 2:
							act = "�֢��ơ��󤫤���Ƥ��Τ����Ȥ�����������";
							break;
						  case 3:
							act = "�֢��Ρ����������פϥᤸ��ʤ��������";
							break;
						  case 4:
							act = "�֢������󤫥��ݡ��ġ��ɤ�Ȥ�������";
							break;
						  case 5:
							act = "�֢�����������������ޤ������ޤ����Ȥ�����";
							break;
						  case 7:
							act = "�֢��ޡ��������С���Τˡ��󤭤�Ρ�����";
							break;
						  case 8:
							act = "�֢��٤󤭤礦���夯�����ᤸ��ʤ��������";
							break;
						  case 9:
							act = "�֢����Ϥ䤵�������ơ���������������";
							break;
						  case 10:
							act = "�֢�����������������������Хĥ��󤵡���";
							break;
						  case 12:
							act = "�֢����ä����������Ρ������Ф��ܤ�������";
							break;
						  case 13:
							act = "�֢��ɡ����ɥ�ߤ�ᤸ��ʤ��������";
							break;
						  case 14:
							act = "�֢��褸����ݤ��äȡ��ʡ������äơ���";
							break;
						  case 15:
							act = "�֢��������Ρ����ʤ������ϡ��ޤ��ʤ�������";
							break;
						}
#else
						act = "horribly sings 'I AM GIAAAAAN. THE BOOOSS OF THE KIIIIDS.'";
#endif
					}
					else
					{
						if (one_in_(3))
#ifdef JP
							act = "�Ϣ��ͤ�ϳڤ�����²���ȲΤäƤ��롣";
						else
							act = "�Ϣ����� ��� �桼���桼 ��� �ߡ����ȲΤäƤ��롣";
#else
							act = "sings 'We are a happy family.'";
						else
							act = "sings 'I love you, you love me.'";
#endif
					}

					sound(SOUND_SHOW);
					break;
				}
			}

			/* Message */
			if (act)
			{
				if (do_silly_attack)
				{
#ifdef JP
					abbreviate = -1;
#endif
					act = silly_attacks[randint0(MAX_SILLY_ATTACK)];
				}
#ifdef JP
				if (abbreviate == 0)
				    msg_format("%^s��%s", m_name, act);
				else if (abbreviate == 1)
				    msg_format("%s", act);
				else /* if (abbreviate == -1) */
				    msg_format("%^s%s", m_name, act);
				abbreviate = 1;/*�����ܰʹߤϾ�ά */
#else
				msg_format("%^s %s%s", m_name, act, do_silly_attack ? " you." : "");
#endif
			}

			/* Hack -- assume all attacks are obvious */
			obvious = TRUE;

			/* Roll out the damage */
			damage = damroll(d_dice, d_side);

			if ( p_ptr->pclass == CLASS_DUELIST
			  && p_ptr->duelist_target_idx == m_idx )
			{
				damage -= damage/3;
			}

			/*
			 * Skip the effect when exploding, since the explosion
			 * already causes the effect.
			 */
			if (explode)
				damage = 0;
			/* Apply appropriate damage */
			switch (effect)
			{
				case 0:
				{
					/* Hack -- Assume obvious */
					obvious = TRUE;

					/* Hack -- No damage */
					damage = 0;

					break;
				}
				case RBE_SUPERHURT:
				{
					if (((randint1(rlev*2+300) > (ac+200)) || one_in_(13)) && !CHECK_MULTISHADOW())
					{
						int tmp_damage = damage - (damage * ((ac < 200) ? ac : 200) / 333);

#ifdef JP
						msg_print("�˺��ΰ�⡪");
#else
						msg_print("It was a critical hit!");
#endif

						tmp_damage = MAX(damage, tmp_damage*2);
						tmp_damage = reduce_melee_dam_p(tmp_damage);

						/* Take damage */
						get_damage += take_hit(DAMAGE_ATTACK, tmp_damage, ddesc, -1);
						break;
					}
				}
				case RBE_HURT:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Player armor reduces total damage */
					damage -= (damage * ((ac < 200) ? ac : 200) / 333);
					damage = reduce_melee_dam_p(damage);

					/* Take damage */
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					break;
				}

				case RBE_POISON:
				{
					if (explode) break;

					/* Take "poison" effect */
					if (p_ptr->resist_pois <= 0 && !CHECK_MULTISHADOW())
					{
						if (set_poisoned(p_ptr->poisoned + randint1(rlev) + 5, FALSE))
						{
							obvious = TRUE;
						}
					}

					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_POIS);

					break;
				}

				case RBE_UN_BONUS:
				{
					if (explode) break;

					if (one_in_(3))
					{
						if ((p_ptr->resist_disen || CHECK_MULTISHADOW()) && one_in_(2))
						{
						}
						else if (disenchant_player())
						{
							obvious = TRUE;
						}
					}
					else 
					{
						if (!p_ptr->resist_disen && !CHECK_MULTISHADOW())
						{
							/* Apply disenchantment */
							if (apply_disenchant(0))
							{
								/* Hack -- Update AC */
								update_stuff();
								obvious = TRUE;
							}
						}
					}

					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_DISEN);

					break;
				}

				case RBE_UN_POWER:
				{
					bool drained = FALSE;

					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Drain charged wands/staffs */
						if (((o_ptr->tval == TV_STAFF) ||
						     (o_ptr->tval == TV_WAND)) &&
						    (o_ptr->pval))
						{
							/* Calculate healed hitpoints */
							int heal=rlev * o_ptr->pval;
							if( o_ptr->tval == TV_STAFF)
							    heal *=  o_ptr->number;

							/* Don't heal more than max hp */
							heal = MIN(heal, m_ptr->maxhp - m_ptr->hp);

							obvious = TRUE;

							/* Hack: Resist Charge Drain 
							   We could make this a general ability, but for now,
							   high level Demon Warlocks get this at CL40
							*/
							if ( p_ptr->pclass == CLASS_WARLOCK
							  && p_ptr->psubclass == PACT_DEMON
							  && p_ptr->lev > 39 
							  /*&& saving_throw(p_ptr->skills.sav - r_ptr->level/2)*/ )
							{
								break;
							}

							if (mut_present(MUT_DEMONIC_GRASP))
							{
								break;
							}

							msg_print(T("Energy drains from your pack!", "���å����饨�ͥ륮�����ۤ����줿��"));
							drained = TRUE;

							/* Heal the monster */
							m_ptr->hp += heal;

							/* Redraw (later) if needed */
							if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
							if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

							/* Uncharge */
							o_ptr->pval = 0;

							/* Combine / Reorder the pack */
							p_ptr->notice |= (PN_COMBINE | PN_REORDER);

							/* Window stuff */
							p_ptr->window |= (PW_INVEN);

							/* Done */
							break;
						}
					}

					if ( !drained 
					  && !(prace_is_(RACE_BALROG) || prace_is_(RACE_SKELETON) || prace_is_(MIMIC_DEMON) || prace_is_(MIMIC_DEMON_LORD)) )
					{
						msg_print("Food drains from your belly!");
						set_food(MAX(0, MIN(p_ptr->food - 1000, p_ptr->food/2)));
					}

					break;
				}

				case RBE_EAT_GOLD:
				{
					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (MON_CONFUSED(m_ptr)) break;

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Obvious */
					obvious = TRUE;

					if ((r_ptr->flags2 & RF2_THIEF) && is_original_ap_and_seen(m_ptr))
						r_ptr->r_flags2 |= RF2_THIEF;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
							      p_ptr->lev)))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("���������᤯���ۤ��ä���");
#else
						msg_print("You quickly protect your money pouch!");
#endif


						/* Occasional blink anyway */
						if (randint0(3)) blinked = TRUE;
					}

					/* Eat gold */
					else
					{
						gold = (p_ptr->au / 10) + randint1(25);
						if (gold < 2) gold = 2;
						if (gold > 5000) gold = (p_ptr->au / 20) + randint1(3000);
						if (gold > p_ptr->au) gold = p_ptr->au;
						p_ptr->au -= gold;
						if (gold <= 0)
						{
#ifdef JP
							msg_print("������������ޤ�ʤ��ä���");
#else
							msg_print("Nothing was stolen.");
#endif

						}
						else if (p_ptr->au)
						{
#ifdef JP
							msg_print("���ۤ��ڤ��ʤä��������롣");
							msg_format("$%ld �Τ��⤬��ޤ줿��", (long)gold);
#else
							msg_print("Your purse feels lighter.");
							msg_format("%ld coins were stolen!", (long)gold);
#endif
							chg_virtue(V_SACRIFICE, 1);
						}
						else
						{
#ifdef JP
							msg_print("���ۤ��ڤ��ʤä��������롣");
							msg_print("���⤬������ޤ줿��");
#else
							msg_print("Your purse feels lighter.");
							msg_print("All of your coins were stolen!");
#endif

							chg_virtue(V_SACRIFICE, 2);
						}

						/* Redraw gold */
						p_ptr->redraw |= (PR_GOLD);

						/* Window stuff */
						p_ptr->window |= (PW_PLAYER);

						/* Blink away */
						blinked = TRUE;
					}

					break;
				}

				case RBE_EAT_ITEM:
				{
					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Confused monsters cannot steal successfully. -LM-*/
					if (MON_CONFUSED(m_ptr)) break;

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					if ((r_ptr->flags2 & RF2_THIEF) && is_original_ap_and_seen(m_ptr))
						r_ptr->r_flags2 |= RF2_THIEF;

					/* Saving throw (unless paralyzed) based on dex and level */
					if (!p_ptr->paralyzed &&
					    (randint0(100) < (adj_dex_safe[p_ptr->stat_ind[A_DEX]] +
							      p_ptr->lev)))
					{
						/* Saving throw message */
#ifdef JP
						msg_print("����������Ƥƥ��å������֤�����");
#else
						msg_print("You grab hold of your backpack!");
#endif


						/* Occasional "blink" anyway */
						blinked = TRUE;

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					/* Find an item */
					for (k = 0; k < 10; k++)
					{
						s16b o_idx;

						/* Pick an item */
						i = randint0(INVEN_PACK);

						/* Obtain the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip artifacts */
						if (object_is_artifact(o_ptr)) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, OD_OMIT_PREFIX);

						/* Message */
#ifdef JP
						msg_format("%s(%c)��%s��ޤ줿��",
							   o_name, index_to_label(i),
							   ((o_ptr->number > 1) ? "���" : ""));
#else
						msg_format("%sour %s (%c) was stolen!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(i));
#endif

						chg_virtue(V_SACRIFICE, 1);


						/* Make an object */
						o_idx = o_pop();

						/* Success */
						if (o_idx)
						{
							object_type *j_ptr;

							/* Get new object */
							j_ptr = &o_list[o_idx];

							/* Copy object */
							object_copy(j_ptr, o_ptr);

							/* Modify number */
							j_ptr->number = 1;

							/* Hack -- If a rod or wand, allocate total
							 * maximum timeouts or charges between those
							 * stolen and those missed. -LM-
							 */
							if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
							{
								j_ptr->pval = o_ptr->pval / o_ptr->number;
								o_ptr->pval -= j_ptr->pval;
							}

							/* Forget mark */
							j_ptr->marked = OM_TOUCHED;

							/* Memorize monster */
							j_ptr->held_m_idx = m_idx;

							/* Build stack */
							j_ptr->next_o_idx = m_ptr->hold_o_idx;

							/* Build stack */
							m_ptr->hold_o_idx = o_idx;
						}

						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Blink away */
						blinked = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_FOOD:
				{
					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					if ((r_ptr->flags2 & RF2_THIEF) && is_original_ap_and_seen(m_ptr))
						r_ptr->r_flags2 |= RF2_THIEF;

					/* Steal some food */
					for (k = 0; k < 10; k++)
					{
						/* Pick an item from the pack */
						i = randint0(INVEN_PACK);

						/* Get the item */
						o_ptr = &inventory[i];

						/* Skip non-objects */
						if (!o_ptr->k_idx) continue;

						/* Skip non-food objects */
						if ((o_ptr->tval != TV_FOOD) && !((o_ptr->tval == TV_CORPSE) && (o_ptr->sval))) continue;
						if (object_is_artifact(o_ptr)) continue;

						/* Get a description */
						object_desc(o_name, o_ptr, (OD_OMIT_PREFIX | OD_NAME_ONLY));

						/* Message */
#ifdef JP
						msg_format("%s(%c)��%s���٤��Ƥ��ޤä���",
							  o_name, index_to_label(i),
							  ((o_ptr->number > 1) ? "���" : ""));
#else
						msg_format("%sour %s (%c) was eaten!",
							   ((o_ptr->number > 1) ? "One of y" : "Y"),
							   o_name, index_to_label(i));
#endif


						/* Steal the items */
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						/* Obvious */
						obvious = TRUE;

						/* Done */
						break;
					}

					break;
				}

				case RBE_EAT_LITE:
				{
					/* Access the lite */
					o_ptr = &inventory[INVEN_LITE];

					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					if ((r_ptr->flags2 & RF2_THIEF) && is_original_ap_and_seen(m_ptr))
						r_ptr->r_flags2 |= RF2_THIEF;

					/* Drain fuel */
					if ((o_ptr->xtra4 > 0) && (!object_is_fixed_artifact(o_ptr)))
					{
						/* Reduce fuel */
						o_ptr->xtra4 -= (250 + randint1(250));
						if (o_ptr->xtra4 < 1) o_ptr->xtra4 = 1;

						/* Notice */
						if (!p_ptr->blind)
						{
#ifdef JP
							msg_print("�����꤬�Ť��ʤäƤ��ޤä���");
#else
							msg_print("Your light dims.");
#endif

							obvious = TRUE;
						}

						/* Window stuff */
						p_ptr->window |= (PW_EQUIP);
					}

					break;
				}

				case RBE_ACID:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("������Ӥ���줿��");
#else
					msg_print("You are covered in acid!");
#endif

					/* Special damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += acid_dam(damage, ddesc, -1);

					/* Hack -- Update AC */
					update_stuff();

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_ACID);

					break;
				}

				case RBE_ELEC:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("�ŷ����Ӥ���줿��");
#else
					msg_print("You are struck by electricity!");
#endif
					/* Special damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += elec_dam(damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_ELEC);

					break;
				}

				case RBE_FIRE:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("���Ȥ������ޤ줿��");
#else
					msg_print("You are enveloped in flames!");
#endif

					/* Special damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += fire_dam(damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FIRE);

					break;
				}

				case RBE_COLD:
				{
					if (explode) break;
					/* Obvious */
					obvious = TRUE;

					/* Message */
#ifdef JP
					msg_print("���Ȥ��䵤��ʤ��줿��");
#else
					msg_print("You are covered with frost!");
#endif

					/* Special damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += cold_dam(damage, ddesc, -1);

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_COLD);

					break;
				}

				case RBE_BLIND:
				{
					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "blind" */
					if (!p_ptr->resist_blind && !CHECK_MULTISHADOW())
					{
						if (set_blind(p_ptr->blind + 10 + randint1(rlev), FALSE))
						{
#ifdef JP
							if (m_ptr->r_idx == MON_DIO) msg_print("�ɤ����á����η�����٤��ϥá�");
#else
							/* nanka */
#endif
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_BLIND);

					break;
				}

				case RBE_CONFUSE:
				{
					if (explode) break;
					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "confused" */
					if (!p_ptr->resist_conf && !CHECK_MULTISHADOW())
					{
						if (set_confused(p_ptr->confused + 3 + randint1(rlev), FALSE))
						{
							obvious = TRUE;
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_CONF);

					break;
				}

				case RBE_TERRIFY:
				{
					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "afraid" */
					if (CHECK_MULTISHADOW())
					{
						/* Do nothing */
					}
					else
					{
						fear_terrify_p(m_ptr);
						obvious = TRUE;
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FEAR);

					break;
				}

				case RBE_PARALYZE:
				{
					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead) break;

					/* Increase "paralyzed" */
					if (CHECK_MULTISHADOW())
					{
						/* Do nothing */
					}
					else if (p_ptr->free_act)
					{
#ifdef JP
						msg_print("���������̤��ʤ��ä���");
#else
						msg_print("You are unaffected!");
#endif

						obvious = TRUE;
					}
					else if (randint0(100 + r_ptr->level/2) < p_ptr->skills.sav)
					{
#ifdef JP
						msg_print("���������Ϥ�ķ���֤�����");
#else
						msg_print("You resist the effects!");
#endif

						obvious = TRUE;
					}
					else
					{
						if (!p_ptr->paralyzed)
						{
							if (set_paralyzed(3 + randint1(rlev), FALSE))
							{
								obvious = TRUE;
							}
						}
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_FREE);

					break;
				}

				case RBE_LOSE_STR:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_STR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_INT:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_INT)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_WIS:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_WIS)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_DEX:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_DEX)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CON:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_CON)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_CHR:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stat) */
					if (do_dec_stat(A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_LOSE_ALL:
				{
					/* Damage (physical) */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Damage (stats) */
					if (do_dec_stat(A_STR)) obvious = TRUE;
					if (do_dec_stat(A_DEX)) obvious = TRUE;
					if (do_dec_stat(A_CON)) obvious = TRUE;
					if (do_dec_stat(A_INT)) obvious = TRUE;
					if (do_dec_stat(A_WIS)) obvious = TRUE;
					if (do_dec_stat(A_CHR)) obvious = TRUE;

					break;
				}

				case RBE_SHATTER:
				{
					/* Obvious */
					obvious = TRUE;

					/* Hack -- Reduce damage based on the player armor class */
					damage -= (damage * ((ac < 200) ? ac : 200) / 333);

					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					/* Radius 8 earthquake centered at the monster */
					if (damage > 23 || explode)
					{
						earthquake_aux(m_ptr->fy, m_ptr->fx, 8, m_idx);
					}

					break;
				}

				case RBE_EXP_10:
				{
					s32b d = damroll(10, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 95);
					break;
				}

				case RBE_EXP_20:
				{
					s32b d = damroll(20, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 90);
					break;
				}

				case RBE_EXP_40:
				{
					s32b d = damroll(40, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 75);
					break;
				}

				case RBE_EXP_80:
				{
					s32b d = damroll(80, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					(void)drain_exp(d, d / 10, 50);
					break;
				}

				case RBE_DISEASE:
				{
					/* Take some damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					/* Take "poison" effect */
					if (p_ptr->resist_pois <= 0)
					{
						if (set_poisoned(p_ptr->poisoned + randint1(rlev) + 5, FALSE))
						{
							obvious = TRUE;
						}
					}

					/* Damage CON (10% chance)*/
					if ((randint1(100) < 11) && (p_ptr->prace != RACE_ANDROID))
					{
						/* 1% chance for perm. damage */
						bool perm = one_in_(10);
						if (dec_stat(A_CON, randint1(10), perm))
						{
#ifdef JP
							msg_print("�¤����ʤ��򿪤�Ǥ��뵤�����롣");
#else
							msg_print("You feel strange sickness.");
#endif

							obvious = TRUE;
						}
					}

					break;
				}
				case RBE_TIME:
				{
					if (explode) break;
					if (!p_ptr->resist_time && !CHECK_MULTISHADOW())
					{
						switch (randint1(10))
						{
							case 1: case 2: case 3: case 4: case 5:
							{
								if (p_ptr->prace == RACE_ANDROID) break;
#ifdef JP
								msg_print("����������ꤷ���������롣");
#else
								msg_print("You feel life has clocked back.");
#endif

								lose_exp(100 + (p_ptr->exp / 100) * MON_DRAIN_LIFE);
								break;
							}

							case 6: case 7: case 8: case 9:
							{
								int stat = randint0(6);

								switch (stat)
								{
#ifdef JP
									case A_STR: act = "����"; break;
									case A_INT: act = "������"; break;
									case A_WIS: act = "������"; break;
									case A_DEX: act = "���Ѥ�"; break;
									case A_CON: act = "�򹯤�"; break;
									case A_CHR: act = "������"; break;
#else
									case A_STR: act = "strong"; break;
									case A_INT: act = "bright"; break;
									case A_WIS: act = "wise"; break;
									case A_DEX: act = "agile"; break;
									case A_CON: act = "hale"; break;
									case A_CHR: act = "confident"; break;
#endif

								}

#ifdef JP
								msg_format("���ʤ��ϰ����ۤ�%s�ʤ��ʤäƤ��ޤä�...��", act);
#else
								msg_format("You're not as %s as you used to be...", act);
#endif


								p_ptr->stat_cur[stat] = (p_ptr->stat_cur[stat] * 3) / 4;
								if (p_ptr->stat_cur[stat] < 3) p_ptr->stat_cur[stat] = 3;
								p_ptr->update |= (PU_BONUS);
								break;
							}

							case 10:
							{
#ifdef JP
						msg_print("���ʤ��ϰ����ۤ��϶����ʤ��ʤäƤ��ޤä�...��");
#else
								msg_print("You're not as powerful as you used to be...");
#endif


								for (k = 0; k < 6; k++)
								{
									p_ptr->stat_cur[k] = (p_ptr->stat_cur[k] * 7) / 8;
									if (p_ptr->stat_cur[k] < 3) p_ptr->stat_cur[k] = 3;
								}
								p_ptr->update |= (PU_BONUS);
								break;
							}
						}
					}
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					break;
				}
				case RBE_EXP_VAMP:
				{
					s32b d = damroll(60, 6) + (p_ptr->exp / 100) * MON_DRAIN_LIFE;
					bool resist_drain;

					/* Obvious */
					obvious = TRUE;

					/* Take damage */
					damage = reduce_melee_dam_p(damage);
					get_damage += take_hit(DAMAGE_ATTACK, damage, ddesc, -1);

					if (p_ptr->is_dead || CHECK_MULTISHADOW()) break;

					resist_drain = !drain_exp(d, d / 10, 50);

					/* Heal the attacker? */
					if (get_race_t()->flags & RACE_IS_NONLIVING)
						resist_drain = TRUE;

					if ((damage > 5) && !resist_drain)
					{
						bool did_heal = FALSE;

						if (m_ptr->hp < m_ptr->maxhp) did_heal = TRUE;

						/* Heal */
						m_ptr->hp += damroll(4, damage / 6);
						if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

						/* Redraw (later) if needed */
						if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);
						if (p_ptr->riding == m_idx) p_ptr->redraw |= (PR_UHEALTH);

						/* Special message */
						if (m_ptr->ml && did_heal)
						{
#ifdef JP
msg_format("%s�����Ϥ���������褦����", m_name);
#else
							msg_format("%^s appears healthier.", m_name);
#endif

						}
					}

					break;
				}
				case RBE_DR_MANA:
				{
					/* Obvious */
					obvious = TRUE;

					if (CHECK_MULTISHADOW())
					{
#ifdef JP
						msg_print("����ϸ��Ƥ�̿�椷�����ʤ��ˤ��Ϥ��ʤ��ä���");
#else
						msg_print("The attack hits Shadow, you are unharmed!");
#endif
					}
					else if (psion_mental_fortress())
						msg_print("Your mental fortress is impenetrable!");
					else if ( prace_is_(RACE_DEMIGOD)
						   && p_ptr->psubrace == DEMIGOD_HERA
						   && randint1(100) > r_ptr->level - 2*(p_ptr->stat_ind[A_WIS] + 3))
					{
						msg_print("You keep your wits about you!");
					}
					else
					{
						do_cut = 0;
						sp_player(-damage);
					}

					/* Learn about the player */
					update_smart_learn(m_idx, DRS_MANA);

					break;
				}
			}

			/* Hack -- only one of cut or stun */
			if (do_cut && do_stun)
			{
				/* Cancel cut */
				if (randint0(100) < 50)
				{
					do_cut = 0;
				}

				/* Cancel stun */
				else
				{
					do_stun = 0;
				}
			}

			/* Handle cut */
			if (do_cut)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint1(5); break;
					case 2: k = randint1(5) + 5; break;
					case 3: k = randint1(20) + 20; break;
					case 4: k = randint1(50) + 50; break;
					case 5: k = randint1(100) + 100; break;
					case 6: k = 300; break;
					default: k = 500; break;
				}

				/* Apply the cut */
				if (k) (void)set_cut(p_ptr->cut + k, FALSE);
			}

			/* Handle stun */
			if (do_stun)
			{
				int k = 0;

				/* Critical hit (zero if non-critical) */
				tmp = monster_critical(d_dice, d_side, damage);

				/* Roll for damage */
				switch (tmp)
				{
					case 0: k = 0; break;
					case 1: k = randint1(5); break;
					case 2: k = randint1(5) + 10; break;
					case 3: k = randint1(10) + 20; break;
					case 4: k = randint1(15) + 30; break;
					case 5: k = randint1(20) + 40; break;
					case 6: k = 80; break;
					default: k = 150; break;
				}

				/* Apply the stun */
				if (k) (void)set_stun(p_ptr->stun + k, FALSE);
			}

			if (explode)
			{
				sound(SOUND_EXPLODE);

				if (mon_take_hit(m_idx, m_ptr->hp + 1, &fear, NULL))
				{
					blinked = FALSE;
					alive = FALSE;
				}
			}

			if (touched)
			{
				bool do_retaliate = FALSE;

				if (weaponmaster_get_toggle() == TOGGLE_TRADE_BLOWS)
				{
					if (m_ptr->ml && !p_ptr->confused && !p_ptr->stun && !p_ptr->blind && !p_ptr->paralyzed)
						do_retaliate = TRUE;
				}
				else if (p_ptr->pclass == CLASS_MONK && (empty_hands(TRUE) & EMPTY_HAND_RARM))
				{
					if (m_ptr->ml && !p_ptr->confused && !p_ptr->stun && !p_ptr->blind && !p_ptr->paralyzed && !mon_save_p(m_ptr->r_idx, A_DEX))
						do_retaliate = TRUE;
				}
				else if (p_ptr->pclass == CLASS_FORCETRAINER && (empty_hands(TRUE) & EMPTY_HAND_RARM))
				{
					if (m_ptr->ml && !p_ptr->confused && !p_ptr->stun && !p_ptr->blind && !p_ptr->paralyzed && !mon_save_p(m_ptr->r_idx, A_DEX) && one_in_(2))
						do_retaliate = TRUE;
				}

				if ( do_retaliate
				  && alive
				  && !retaliation_hack /* Otherwise, we get a retaliatory cycle!!! */
				  && !p_ptr->is_dead )
				{
					if (weaponmaster_get_toggle() == TOGGLE_TRADE_BLOWS)
						msg_format("You trade blows with %^s.", m_name);
					else
						msg_print("You retaliate.");

					py_attack(m_ptr->fy, m_ptr->fx, WEAPONMASTER_RETALIATION);
				}

				if (p_ptr->tim_blood_revenge && alive && !p_ptr->is_dead && monster_living(r_ptr))
				{   /* Scale the damage based on cuts and monster deadliness */
					int dam = damage * p_ptr->cut / CUT_SEVERE;

					/* Balance out a weak melee attack */
					if (dam < p_ptr->cut / 10)
						dam = p_ptr->cut / 10;

					/* Not too powerful */
					if (dam > 50)
						dam = 50;

					dam = mon_damage_mod(m_ptr, dam, FALSE);
					if (dam > 0)
					{
						msg_format("%^s feels your bloody revenge!", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
							" turns into a pool of blood."))
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
				}

				if (p_ptr->sh_fire && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK))
					{
						int dam = _aura_dam_p();

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^s������Ǯ���ʤä���", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "�ϳ��λ��ˤʤä���"))
#else
						msg_format("%^s is suddenly very hot!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " turns into a pile of ash."))
#endif

						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_FIRE_MASK);
					}
				}

				if (p_ptr->sh_elec && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK))
					{
						int dam = _aura_dam_p();

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^s���ŷ�򤯤�ä���", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "��ǳ���̤λ��ˤʤä���"))
#else
						msg_format("%^s gets zapped!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " turns into a pile of cinder."))
#endif

						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_ELEC_MASK);
					}
				}

				if (p_ptr->sh_cold && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_IM_COLD_MASK))
					{
						int dam = _aura_dam_p();

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^s���䵤�򤯤�ä���", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "�����Ĥ�����"))
#else
						msg_format("%^s is very cold!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " was frozen."))
#endif

						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_IM_COLD_MASK);
					}
				}

				/* by henkma */
				if (p_ptr->dustrobe && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_EFF_RES_SHAR_MASK))
					{
						int dam = _aura_dam_p();

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^s�϶������Ҥ򤯤�ä���", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "�ϥ��������ˤʤä���"))
#else
						msg_format("%^s gets zapped!", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " had torn to pieces."))
#endif
						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (r_ptr->flagsr & RFR_EFF_RES_SHAR_MASK);
					}

					if (is_mirror_grid(&cave[py][px]))
					{
						teleport_player(10, 0L);
					}
				}

				if (p_ptr->tim_sh_holy && alive && !p_ptr->is_dead)
				{
					if (r_ptr->flags3 & RF3_EVIL)
					{
						if (!(r_ptr->flagsr & RFR_RES_ALL))
						{
							int dam = _aura_dam_p();

							/* Modify the damage */
							dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
							msg_format("%^s�����ʤ륪����ǽ��Ĥ�����", m_name);
							if (mon_take_hit(m_idx, dam, &fear,
							    "���ݤ줿��"))
#else
							msg_format("%^s is injured by holy power!", m_name);

							if (mon_take_hit(m_idx, dam, &fear,
							    " is destroyed."))
#endif
							{
								blinked = FALSE;
								alive = FALSE;
							}
							if (is_original_ap_and_seen(m_ptr))
								r_ptr->r_flags3 |= RF3_EVIL;
						}
						else
						{
							if (is_original_ap_and_seen(m_ptr))
								r_ptr->r_flagsr |= RFR_RES_ALL;
						}
					}
				}

				if (p_ptr->tim_sh_touki && alive && !p_ptr->is_dead)
				{
					if (!(r_ptr->flagsr & RFR_RES_ALL))
					{
						int dam = _aura_dam_p();

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("%^s���Ԥ�Ʈ���Υ�����ǽ��Ĥ�����", m_name);
						if (mon_take_hit(m_idx, dam, &fear,
						    "���ݤ줿��"))
#else
						msg_format("%^s is injured by the Force", m_name);

						if (mon_take_hit(m_idx, dam, &fear,
						    " is destroyed."))
#endif

						{
							blinked = FALSE;
							alive = FALSE;
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= RFR_RES_ALL;
					}
				}

				if (hex_spelling(HEX_SHADOW_CLOAK) && alive && !p_ptr->is_dead)
				{
					int dam = 1;
					object_type *o_ptr = &inventory[INVEN_RARM];

					if (!(r_ptr->flagsr & RFR_RES_ALL || r_ptr->flagsr & RFR_RES_DARK))
					{
						if (o_ptr->k_idx)
						{
							int basedam = ((o_ptr->dd + p_ptr->weapon_info[0].to_dd) * (o_ptr->ds + p_ptr->weapon_info[0].to_ds + 1));
							dam = basedam / 2 + o_ptr->to_d + p_ptr->weapon_info[0].to_d;
						}

						/* Cursed armor makes damages doubled */
						o_ptr = &inventory[INVEN_BODY];
						if ((o_ptr->k_idx) && object_is_cursed(o_ptr)) dam *= 2;

						/* Modify the damage */
						dam = mon_damage_mod(m_ptr, dam, FALSE);

#ifdef JP
						msg_format("�ƤΥ����餬%^s��ȿ�⤷����", m_name);
						if (mon_take_hit(m_idx, dam, &fear, "���ݤ줿��"))
#else
						msg_format("Enveloped shadows attack %^s.", m_name);

						if (mon_take_hit(m_idx, dam, &fear, " is destroyed."))
#endif
						{
							blinked = FALSE;
							alive = FALSE;
						}
						else /* monster does not dead */
						{
							int j;
							int flg = PROJECT_STOP | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL;
							int typ[4][2] = {
								{ INVEN_HEAD, GF_OLD_CONF },
								{ INVEN_LARM,  GF_OLD_SLEEP },
								{ INVEN_HANDS, GF_TURN_ALL },
								{ INVEN_FEET, GF_OLD_SLOW }
							};

							/* Some cursed armours gives an extra effect */
							for (j = 0; j < 4; j++)
							{
								o_ptr = &inventory[typ[j][0]];
								if ((o_ptr->k_idx) && object_is_cursed(o_ptr) && object_is_armour(o_ptr))
									project(0, 0, m_ptr->fy, m_ptr->fx, (p_ptr->lev * 2), typ[j][1], flg, -1);
							}
						}
					}
					else
					{
						if (is_original_ap_and_seen(m_ptr))
							r_ptr->r_flagsr |= (RFR_RES_ALL | RFR_RES_DARK);
					}
				}
			}
		}

		/* Monster missed player */
		else
		{
			/* Analyze failed attacks */
			switch (method)
			{
				case RBM_HIT:
				case RBM_TOUCH:
				case RBM_PUNCH:
				case RBM_KICK:
				case RBM_CLAW:
				case RBM_BITE:
				case RBM_STING:
				case RBM_SLASH:
				case RBM_BUTT:
				case RBM_CRUSH:
				case RBM_ENGULF:
				case RBM_CHARGE:

				/* Visible monsters */
				if (m_ptr->ml)
				{
					/* Disturbing */
					disturb(1, 0);

					/* Message */
#ifdef JP
					if (abbreviate)
					    msg_format("%s���路����", (p_ptr->special_attack & ATTACK_SUIKEN) ? "��̯��ư����" : "");
					else
					    msg_format("%s%^s�ι���򤫤路����", (p_ptr->special_attack & ATTACK_SUIKEN) ? "��̯��ư����" : "", m_name);
					abbreviate = 1;/*�����ܰʹߤϾ�ά */
#else
					msg_format("%^s misses you.", m_name);
#endif

				}
				damage = 0;

				break;
			}
		}


		/* Analyze "visible" monsters only */
		if (is_original_ap_and_seen(m_ptr) && !do_silly_attack)
		{
			/* Count "obvious" attacks (and ones that cause damage) */
			if (obvious || damage || (r_ptr->r_blows[ap_cnt] > 10))
			{
				/* Count attacks of this type */
				if (r_ptr->r_blows[ap_cnt] < MAX_UCHAR)
				{
					r_ptr->r_blows[ap_cnt]++;
				}
			}
		}

		if (p_ptr->riding && damage)
		{
			char m_name[80];
			monster_desc(m_name, &m_list[p_ptr->riding], 0);
			if (rakuba((damage > 200) ? 200 : damage, FALSE))
			{
#ifdef JP
msg_format("%^s��������Ƥ��ޤä���", m_name);
#else
				msg_format("You have fallen from %s.", m_name);
#endif
			}
		}

		if (p_ptr->special_defense & NINJA_KAWARIMI)
		{
			if (kawarimi(FALSE)) return TRUE;
		}
	
		if (retaliation_hack)
			break;
	}

	/* Hex - revenge damage stored */
	revenge_store(get_damage);

	if (IS_REVENGE()
		&& get_damage > 0 && !p_ptr->is_dead)
	{
		char m_name_self[80];

		/* hisself */
		monster_desc(m_name_self, m_ptr, MD_PRON_VISIBLE | MD_POSSESSIVE | MD_OBJECTIVE);

		msg_format("The attack of %s has wounded %s!", m_name, m_name_self);
		project(0, 0, m_ptr->fy, m_ptr->fx, psion_backlash_dam(get_damage), GF_MISSILE, PROJECT_KILL, -1);
		if (p_ptr->tim_eyeeye) set_tim_eyeeye(p_ptr->tim_eyeeye-5, TRUE);
	}

	if ((p_ptr->counter || (p_ptr->special_defense & KATA_MUSOU)) && alive && !p_ptr->is_dead && m_ptr->ml && (p_ptr->csp > 7))
	{
		char m_name[80];
		monster_desc(m_name, m_ptr, 0);

		p_ptr->csp -= 7;
#ifdef JP
		msg_format("%^s��ȿ�⤷����", m_name);
#else
		msg_format("Your counterattack to %s!", m_name);
#endif
		py_attack(m_ptr->fy, m_ptr->fx, HISSATSU_COUNTER);
		fear = FALSE;

		/* Redraw mana */
		p_ptr->redraw |= (PR_MANA);
	}

	if (ht_cnt == 0 && !p_ptr->is_dead && allow_ticked_off(r_ptr) && one_in_(2))
	{
		if (!(m_ptr->smart & SM_TICKED_OFF))
		{
			char m_name[80];
			monster_desc(m_name, m_ptr, 0);
			msg_format("%^s is ticked off!", m_name);
			m_ptr->smart |= SM_TICKED_OFF;
		}
	}

	/* Blink away */
	if (blinked && alive && !p_ptr->is_dead)
	{
		if (teleport_barrier(m_idx))
		{
#ifdef JP
			msg_print("ť���ϾФä�ƨ��...�褦�Ȥ������Хꥢ���ɤ��줿��");
#else
			msg_print("The thief flees laughing...? But magic barrier obstructs it.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("ť���ϾФä�ƨ������");
#else
			msg_print("The thief flees laughing!");
#endif
			teleport_away(m_idx, MAX_SIGHT * 2 + 5, 0L);
		}
	}


	/* Always notice cause of death */
	if (p_ptr->is_dead && (r_ptr->r_deaths < MAX_SHORT) && !p_ptr->inside_arena)
	{
		r_ptr->r_deaths++;
	}

	if (m_ptr->ml && fear && alive && !p_ptr->is_dead)
	{
		sound(SOUND_FLEE);
#ifdef JP
		msg_format("%^s�϶��ݤ�ƨ���Ф�����", m_name);
#else
		msg_format("%^s flees in terror!", m_name);
#endif

	}

	if (p_ptr->special_defense & KATA_IAI)
	{
		set_action(ACTION_NONE);
	}

	/* Assume we attacked */
	return (TRUE);
}
