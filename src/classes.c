/*
 *  Hooks and Callbacks for various classes
 */

#include "angband.h"

int get_class_idx(cptr name)
{
	/* For now, read the global table. Not all classes have been ported to the new code system */
	int i;
	for (i = 0; i < MAX_CLASS; i++)
	{
		if (strcmpi(name, class_info[i].title) == 0)
			return i;
	}
	return -1;
}

/* Goal: This should be the one and only switch off of p_ptr->pclass in the
   entire system! */
class_t *get_class_t_aux(int pclass, int psubclass)
{
class_t *result = NULL;

	switch (pclass)
	{
	case CLASS_ARCHAEOLOGIST:
		result = archaeologist_get_class_t();
		break;
	case CLASS_BLOOD_KNIGHT:
		result = blood_knight_get_class_t();
		break;
	case CLASS_BLOOD_MAGE:
		result = blood_mage_get_class_t();
		break;
	case CLASS_DUELIST:
		result = duelist_get_class_t();
		break;
	case CLASS_FORCETRAINER:
		result = force_trainer_get_class_t();
		break;
	case CLASS_MAULER:
		result = mauler_get_class_t();
		break;
	case CLASS_MINDCRAFTER:
		result = mindcrafter_get_class_t();
		break;
	case CLASS_NECROMANCER:
		result = necromancer_get_class_t();
		break;
	case CLASS_PSION:
		result = psion_get_class_t();
		break;
	case CLASS_RAGE_MAGE:
		result = rage_mage_get_class_t();
		break;
	case CLASS_RUNE_KNIGHT:
		result = rune_knight_get_class_t();
		break;
	case CLASS_SCOUT:
		result = scout_get_class_t();
		break;
	case CLASS_TIME_LORD:
		result = time_lord_get_class_t();
		break;
	case CLASS_WARLOCK:
		result = warlock_get_class_t(psubclass);
		break;
	case CLASS_WEAPONMASTER:
		result = weaponmaster_get_class_t();
		break;
	case CLASS_WILD_TALENT:
		result = wild_talent_get_class_t();
		break;
	}

	return result;
}

class_t *get_class_t(void)
{
	return get_class_t_aux(p_ptr->pclass, p_ptr->psubclass);
}

caster_info *get_caster_info(void)
{
	caster_info *result = NULL;
	class_t *class_ptr = get_class_t();
	if (class_ptr && class_ptr->caster_info)
		result = (class_ptr->caster_info)();
	return result;
}

/* HACK: This should be handled by the class_t entry point ...
   This is just here while I am refactoring code!!! */
int get_class_powers(spell_info* spells, int max)
{
	int ct = 0;

	switch (p_ptr->pclass)
	{
		case CLASS_WARRIOR:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 40;
			spell->cost = 75;
			spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_DEX]);
			spell->fn = sword_dance_spell;
			break;
		}
		case CLASS_HIGH_MAGE:
			if (p_ptr->realm1 == REALM_HEX)
			{
				spell_info* spell = &spells[ct++];
				spell->level = 1;
				spell->cost = 0;
				spell->fail = 0;
				spell->fn = hex_stop_spelling_spell;
				break;
			}
			/* vvvvvv FALL THRU ON PURPOSE vvvvvvv */
		case CLASS_MAGE:
		case CLASS_SORCERER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 25;
			spell->cost = 1;
			spell->fail = calculate_fail_rate(spell->level, 90, p_ptr->stat_ind[A_INT]);
			spell->fn = eat_magic_spell;
			break;
		}
		case CLASS_PRIEST:
			if (is_good_realm(p_ptr->realm1))
			{
				spell_info* spell = &spells[ct++];
				spell->level = 35;
				spell->cost = 70;
				spell->fail = calculate_fail_rate(spell->level, 90, p_ptr->stat_ind[A_WIS]);
				spell->fn = bless_weapon_spell;
			}
			else
			{
				spell_info* spell = &spells[ct++];
				spell->level = 42;
				spell->cost = 40;
				spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_WIS]);
				spell->fn = evocation_spell;
			}
			break;

		case CLASS_RANGER:
		case CLASS_SNIPER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 15;
			spell->cost = 20;
			spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_INT]);
			spell->fn = probing_spell;
			break;
		}
		case CLASS_PALADIN:
			if (is_good_realm(p_ptr->realm1))
			{
				spell_info* spell = &spells[ct++];
				spell->level = 30;
				spell->cost = 30;
				spell->fail = calculate_fail_rate(spell->level, 70, p_ptr->stat_ind[A_WIS]);
				spell->fn = holy_lance_spell;
			}
			else
			{
				spell_info* spell = &spells[ct++];
				spell->level = 30;
				spell->cost = 30;
				spell->fail = calculate_fail_rate(spell->level, 70, p_ptr->stat_ind[A_WIS]);
				spell->fn = hell_lance_spell;
			}
			break;

		case CLASS_WARRIOR_MAGE:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 25;
			spell->cost = 0;
			spell->fail = calculate_fail_rate(spell->level, 50, p_ptr->stat_ind[A_INT]);
			spell->fn = hp_to_sp_spell;

			spell = &spells[ct++];
			spell->level = 25;
			spell->cost = 0;
			spell->fail = calculate_fail_rate(spell->level, 50, p_ptr->stat_ind[A_INT]);
			spell->fn = sp_to_hp_spell;
			break;
		}
		case CLASS_CHAOS_WARRIOR:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 40;
			spell->cost = 50;
			spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_INT]);
			spell->fn = confusing_lights_spell;
			break;
		}
		case CLASS_MONK:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 25;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = monk_posture_spell;

			spell = &spells[ct++];
			spell->level = 30;
			spell->cost = 30;
			spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_STR]);
			spell->fn = monk_double_attack_spell;
			break;
		}
		case CLASS_TOURIST:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = take_photo_spell;

			spell = &spells[ct++];
			spell->level = 25;
			spell->cost = 20;
			spell->fail = calculate_fail_rate(spell->level, 30, p_ptr->stat_ind[A_INT]);
			spell->fn = identify_fully_spell;
			break;
		}
		case CLASS_IMITATOR:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 30;
			spell->cost = 100;
			spell->fail = calculate_fail_rate(spell->level, 90, p_ptr->stat_ind[A_DEX]);
			spell->fn = double_revenge_spell;
			break;
		}
		case CLASS_BEASTMASTER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = calculate_fail_rate(spell->level, 70, p_ptr->stat_ind[A_CHR]);
			spell->fn = dominate_living_I_spell;

			spell = &spells[ct++];
			spell->level = 30;
			spell->cost = 0;
			spell->fail = calculate_fail_rate(spell->level, 70, p_ptr->stat_ind[A_CHR]);
			spell->fn = dominate_living_II_spell;
			break;
		}
		case CLASS_ARCHER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = create_ammo_spell;
			break;
		}
		case CLASS_MAGIC_EATER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = absorb_magic_spell;
			break;
		}
		case CLASS_BARD:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = bard_stop_singing_spell;
			break;
		}
		case CLASS_RED_MAGE:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 48;
			spell->cost = 20;
			spell->fail = 0;
			spell->fn = double_magic_spell;
			break;
		}
		case CLASS_SAMURAI:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = samurai_concentration_spell;

			spell = &spells[ct++];
			spell->level = 25;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = samurai_posture_spell;
			break;
		}
		case CLASS_BLUE_MAGE:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = blue_learning_spell;
			break;
		}
		case CLASS_CAVALRY:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 10;
			spell->cost = 0;
			spell->fail = calculate_fail_rate(spell->level, 50, p_ptr->stat_ind[A_STR]);
			spell->fn = rodeo_spell;
			break;
		}
		case CLASS_BERSERKER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 10;
			spell->cost = 10;
			spell->fail = calculate_fail_rate(spell->level, 70, p_ptr->stat_ind[A_DEX]);
			spell->fn = recall_spell;
			break;
		}
		case CLASS_MIRROR_MASTER:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 1;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = break_mirrors_spell;

			spell = &spells[ct++];
			spell->level = 30;
			spell->cost = 0;
			spell->fail = calculate_fail_rate(spell->level, 50, p_ptr->stat_ind[A_INT]);
			spell->fn = mirror_concentration_spell;
			break;
		}
		case CLASS_SMITH:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 5;
			spell->cost = 15;
			spell->fail = calculate_fail_rate(spell->level, 80, p_ptr->stat_ind[A_INT]);
			spell->fn = smith_judgment_spell;
			break;
		}
		case CLASS_NINJA:
		{
			spell_info* spell = &spells[ct++];
			spell->level = 20;
			spell->cost = 0;
			spell->fail = 0;
			spell->fn = quick_walk_spell;
			break;
		}
	}

	return ct;
}

/* 
Helper for getting powers (cf. spoil_powers_aux)

Sample usage:
	static power_info _powers[] =
	{
		{ A_WIS, {15, 0, 30, clear_mind_spell}}, 
		{ -1, {-1, -1, -1, NULL}}
	};
	...
	static int _get_powers(spell_info* spells, int max)
	{
		return get_powers_aux(spells, max, _powers);
	}
	...
	class_t *mindcrafter_get_class_t(void)
	{
	...
		me.get_powers = _get_powers;
	...
	}
*/
int get_powers_aux(spell_info* spells, int max, power_info* table)
{
	int i;
	int ct = 0;
	
	for (i = 0; ; i++)
	{
		power_info *base = &table[i];
		
		if (ct >= max) break;
		if (!base->spell.fn) break;

		if (base->spell.level <= p_ptr->lev)
		{
			spell_info* current = &spells[ct];
			current->fn = base->spell.fn;
			current->level = base->spell.level;
			current->cost = base->spell.cost;

			current->fail = calculate_fail_rate(
				base->spell.level, 
				base->spell.fail, 
				p_ptr->stat_ind[base->stat]
			);			
			ct++;
		}
	}
	return ct;
}

void spoil_powers_aux(FILE *fff, power_info *table)
{
	int i;
	variant vn, vd;
	var_init(&vn);
	var_init(&vd);

	fprintf(fff, "\n== Powers ==\n");
	fprintf(fff, "||  || *Stat* || *Lvl* || *Cost* || *Fail* || *Description* ||\n");
	for (i = 0; ; i++)
	{
		power_info *base = &table[i];		
		if (!base->spell.fn) break;

		base->spell.fn(SPELL_SPOIL_NAME, &vn);
		if (var_is_null(&vn)) base->spell.fn(SPELL_NAME, &vn);
		
		base->spell.fn(SPELL_SPOIL_DESC, &vd);
		if (var_is_null(&vd)) base->spell.fn(SPELL_DESC, &vd);

		fprintf(fff, "||%s||%s||%d||%d||%d||`%s`||\n", 
			var_get_string(&vn), 
			stat_abbrev_true[base->stat],
			base->spell.level, base->spell.cost, base->spell.fail, 
			var_get_string(&vd)
		);
	}

	var_clear(&vn);
	var_clear(&vd);
}


/* 
Helper for getting spells (cf. spoil_spells_aux)

Sample usage:
	static spell_info _spells[] = 
	{
		{ 1,   1,  15, _neural_blast_spell},
		{ 2,   1,  20, _precognition_spell},
		{ 3,   2,  25, _minor_displacement_spell},
		...
		{ -1, -1,  -1, NULL}
	};
	...
	static int _get_spells(spell_info* spells, int max)
	{
		return get_spells_aux(spells, max, _spells, p_ptr->stat_ind[A_WIS]);
	}
	...
	class_t *mindcrafter_get_class_t(void)
	{
	...
		me.get_spells = _get_spells;
	...
	}
*/
int get_spells_aux(spell_info* spells, int max, spell_info* table, int stat_idx)
{
	int i;
	int ct = 0;

	for (i = 0; ; i++)
	{
		spell_info *base = &table[i];
		if (ct >= max) break;
		if (!base->fn) break;

		if (base->level <= p_ptr->lev)
		{
			spell_info* current = &spells[ct];
			current->fn = base->fn;
			current->level = base->level;
			current->cost = base->cost;

			current->fail = calculate_fail_rate(base->level, base->fail, stat_idx);			
			ct++;
		}
	}
	return ct;
}

void spoil_spells_aux(FILE *fff, spell_info *table)
{
	int i;
	variant vn, vd;
	var_init(&vn);
	var_init(&vd);

	fprintf(fff, "\n== Spells ==\n");
	fprintf(fff, "||  || *Lvl* || *Cost* || *Fail* || *Description* ||\n");
	for (i = 0; ; i++)
	{
		spell_info *base = &table[i];
		if (!base->fn) break;

		base->fn(SPELL_SPOIL_NAME, &vn);
		if (var_is_null(&vn)) base->fn(SPELL_NAME, &vn);
		
		base->fn(SPELL_SPOIL_DESC, &vd);
		if (var_is_null(&vd)) base->fn(SPELL_DESC, &vd);

		fprintf(fff, "||%s||%d||%d||%d||`%s`||\n", 
			var_get_string(&vn), base->level, base->cost, base->fail, var_get_string(&vd));
	}

	var_clear(&vn);
	var_clear(&vd);
}