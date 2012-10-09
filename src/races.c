#include "angband.h"


/****************************************************************
 * Public Entrypoints
 ****************************************************************/
int get_race_idx(cptr name)
{
	int i;
	for (i = 0; i < MAX_RACES; i++)
	{
		race_t *race_ptr = get_race_t_aux(i, 0);
		if (strcmpi(name, race_ptr->name) == 0)
			return i;
	}
	return -1;
}

race_t *get_race_t_aux(int prace, int psubrace)
{
	race_t *result = NULL;

	switch (prace)
	{
	/* Player Races */
	case RACE_AMBERITE:
		result = amberite_get_race_t();
		break;
	case RACE_ANDROID:
		result = android_get_race_t();
		break;
	case RACE_ARCHON:
		result = archon_get_race_t();
		break;
	case RACE_BARBARIAN:
		result = barbarian_get_race_t();
		break;
	case RACE_BEASTMAN:
		result = beastman_get_race_t();
		break;
	case RACE_CYCLOPS:
		result = cyclops_get_race_t();
		break;
	case RACE_DARK_ELF:
		result = dark_elf_get_race_t();
		break;
	case RACE_DEMIGOD:
		result = demigod_get_race_t(psubrace);
		break;
	case RACE_BALROG:
		result = balrog_get_race_t();
		break;
	case RACE_DOPPELGANGER:
		result = doppelganger_get_race_t();
		break;
	case RACE_DUNADAN:
		result = dunadan_get_race_t();
		break;
	case RACE_DRACONIAN:
		result = draconian_get_race_t();
		break;
	case RACE_DWARF:
		result = dwarf_get_race_t();
		break;
	case RACE_ENT:
		result = ent_get_race_t();
		break;
	case RACE_GNOME:
		result = gnome_get_race_t();
		break;
	case RACE_GOLEM:
		result = golem_get_race_t();
		break;
	case RACE_HALF_GIANT:
		result = half_giant_get_race_t();
		break;
	case RACE_HALF_OGRE:
		result = half_ogre_get_race_t();
		break;
	case RACE_HALF_TITAN:
		result = half_titan_get_race_t();
		break;
	case RACE_HALF_TROLL:
		result = half_troll_get_race_t();
		break;
	case RACE_HIGH_ELF:
		result = high_elf_get_race_t();
		break;
	case RACE_HOBBIT:
		result = hobbit_get_race_t();
		break;
	case RACE_HUMAN:
		result = human_get_race_t();
		break;
	case RACE_IMP:
		result = imp_get_race_t();
		break;
	case RACE_KLACKON:
		result = klackon_get_race_t();
		break;
	case RACE_KOBOLD:
		result = kobold_get_race_t();
		break;
	case RACE_KUTAR:
		result = kutar_get_race_t();
		break;
	case RACE_MIND_FLAYER:
		result = mindflayer_get_race_t();
		break;
	case RACE_NIBELUNG:
		result = nibelung_get_race_t();
		break;
	case RACE_SHADOW_FAIRY:
		result = shadow_fairy_get_race_t();
		break;
	case RACE_SKELETON:
		result = skeleton_get_race_t();
		break;
	case RACE_SNOTLING:
		result = snotling_get_race_t();
		break;
	case RACE_SPECTRE:
		result = spectre_get_race_t();
		break;
	case RACE_SPRITE:
		result = sprite_get_race_t();
		break;
	case RACE_TONBERRY:
		result = tonberry_get_race_t();
		break;
	case RACE_VAMPIRE:
		result = vampire_get_race_t();
		break;
	case RACE_YEEK:
		result = yeek_get_race_t();
		break;
	case RACE_ZOMBIE:
		result = zombie_get_race_t();
		break;
	/* Mimic Races */
	case MIMIC_CLAY_GOLEM:
		result = clay_golem_get_race_t();
		break;
	case MIMIC_COLOSSUS:
		result = colossus_get_race_t();
		break;
	case MIMIC_DEMON:
		result = demon_get_race_t();
		break;
	case MIMIC_DEMON_LORD:
		result = demon_lord_get_race_t();
		break;
	case MIMIC_IRON_GOLEM:
		result = iron_golem_get_race_t();
		break;
	case MIMIC_MITHRIL_GOLEM:
		result = mithril_golem_get_race_t();
		break;
	case MIMIC_VAMPIRE:
		result = vampire_lord_get_race_t();
		break;
	}

	return result;
}

race_t *get_true_race_t(void)
{
	return get_race_t_aux(p_ptr->prace, p_ptr->psubrace);
}

race_t *get_race_t(void)
{
	race_t *result;
	if (p_ptr->mimic_form != MIMIC_NONE)
	{
		result = get_race_t_aux(p_ptr->mimic_form, 0);
		result->mimic = TRUE;
	}
	else
	{
		result = get_race_t_aux(p_ptr->prace, p_ptr->psubrace);
		result->mimic = FALSE;
	}
	return result;
}

