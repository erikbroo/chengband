#include "angband.h"

void satisfy_hunger_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Satisfy Hunger");
		break;
	case SPELL_DESC:
		var_set_string(res, "Fills your belly with pure yuminess.");
		break;
	case SPELL_CAST:
		set_food(PY_FOOD_MAX - 1);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_satisfy_hunger(void) { return cast_spell(satisfy_hunger_spell); }

void scare_monster_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Scare Monster", "��󥹥�������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		stop_mouth();
		/*
		msg_print(T("You make a horrible scream!", "�Ȥ��Ӥ����Ķ�������夲����");
		msg_print(T("You emit an eldritch howl!", "���ʤ��Ϥ��ɤ��ɤ����������򤢤�����"));
		*/
		fear_monster(dir, p_ptr->lev);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void sense_surroundings_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Sense Surroundings");
		break;
	default:
		magic_mapping_spell(cmd, res);
		break;
	}
}

void shadow_shifting_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Shadow Shifting", "����ɥ������ե�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Recreates the current dungeon level after a short delay.");
		break;
	case SPELL_CAST:
		msg_print(T("You start walking around.", "���ʤ����⤭����Ϥ᤿��"));
		alter_reality();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void shoot_arrow_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Shoot Arrow", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires an arrow.", ""));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(2, spell_power(7), 0));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You fire an arrow.", "������ä���"));
		fire_bolt(GF_ARROW, dir, spell_power(damroll(2, 7)));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void shriek_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Shriek", "����"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Generates a large sound ball centered on you.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev)));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("Your vocal cords get much tougher.", "���ʤ����������������ʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("Your vocal cords get much weaker.", "���ʤ��������ϼ夯�ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can emit a horrible shriek.", "���ʤ��ϿȤ��Ӥ����Ķ�������ȯ���뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		stop_mouth();
		fire_ball(GF_SOUND, 0, spell_power(2 * p_ptr->lev), 8);
		aggravate_monsters(0);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_shriek(void) { return cast_spell(shriek_spell); }

void sleeping_dust_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Sleeping Dust", "̲��ʴ"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("", ""));
		break;
	case SPELL_CAST:
		msg_print(T("You throw some magic dust...", "���ʤ�����ˡ��ʴ���ꤲ�Ĥ���..."));
		if (p_ptr->lev < 25) sleep_monsters_touch();
		else sleep_monsters(p_ptr->lev);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void smell_metal_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Smell Metal", "��°�̳�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Smells nearby metallic odors.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You smell a metallic odor.", "��°���������̤�ʬ������褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer smell a metallic odor.", "��°�ν������̤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can smell nearby precious metal.", "���ʤ��϶᤯�ˤ��뵮��°�򤫤�ʬ���뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		stop_mouth();
		detect_treasure(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void smell_monsters_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Smell Monsters", "Ũ���̳�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Detects nearby monsters.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You smell filthy monsters.", "��󥹥����ν������̤�ʬ������褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer smell filthy monsters.", "�Է�ʥ�󥹥����ν������̤��ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can smell nearby monsters.", "���ʤ��϶᤯�Υ�󥹥�����¸�ߤ򤫤�ʬ���뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		stop_mouth();
		detect_monsters_normal(DETECT_RAD_DEFAULT);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void sp_to_hp_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Convert SP to HP", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Converts SP into HP", ""));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You are subject to fits of magical healing.", "��ˡ�μ�����ȯ��򵯤����褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer subject to fits of magical healing.", "��ˡ�μ�����ȯ��˽����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("Your blood sometimes rushes to your muscles.", "���ʤ��ϻ����줬�����ˤɤä�ή��롣"));
		break;
	case SPELL_PROCESS:
		if (one_in_(2000))
		{
			int wounds = p_ptr->mhp - p_ptr->chp;

			if (wounds > 0)
			{
				int healing = p_ptr->csp;

				if (healing > wounds)
					healing = wounds;

				hp_player(healing);
				p_ptr->csp -= healing;

				p_ptr->redraw |= (PR_MANA);
			}
		}
		break;
	case SPELL_CAST:
		if (p_ptr->csp >= p_ptr->lev / 5)
		{
			p_ptr->csp -= p_ptr->lev / 5;
			p_ptr->redraw |= PR_MANA;
			hp_player(p_ptr->lev);
		}
		else
			msg_print(T("You failed to convert.", "�Ѵ��˼��Ԥ�����"));

		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void spit_acid_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Spit Acid", "������"));
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 25)
			var_set_string(res, "Spits a bolt of acid.");
		else
			var_set_string(res, "Spits a ball of acid.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to spit acid.", "�����Ǥ�ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to spit acid.", "����᤭������ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can spit acid (dam lvl*2).", "���ʤ��ϻ���᤭�����뤳�Ȥ��Ǥ��롣(���᡼�� ��٥�X1)"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev * 2)));
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev/5);
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			stop_mouth();
			msg_print(T("You spit acid...", "�����Ǥ�������..."));
			if (p_ptr->lev < 25) fire_bolt(GF_ACID, dir, spell_power(p_ptr->lev * 2));
			else fire_ball(GF_ACID, dir, spell_power(p_ptr->lev * 2), 2);
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_spit_acid(void) { return cast_spell(spit_acid_spell); }

void starburst_I_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Star Burst", "�������С�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a huge ball of powerful light.", "����������ε�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(100 + p_ptr->lev * 2)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You invoke a starburst.", "�������С����Ȥμ�ʸ��ǰ������"));
		fire_ball(GF_LITE, dir, spell_power(100 + p_ptr->lev * 2), spell_power(4));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void starburst_II_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Star Burst", "�������С�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires a huge ball of powerful light.", "����������ε�����ġ�"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(10, spell_power(10), spell_power(50 + p_ptr->lev * 8)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You invoke a starburst.", "�������С����Ȥμ�ʸ��ǰ������"));
		fire_ball(GF_LITE, dir, 
			spell_power(50 + p_ptr->lev * 8 + damroll(10, 10)), 
			spell_power(4));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void sterility_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Sterility", "�����˻�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Stops breeding monsters from ... umm ... doing the nasty.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You can give everything around you a headache.", "��������ƤμԤ�Ƭ�ˤ򵯤������Ȥ��Ǥ��롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You hear a massed sigh of relief.", "��������ΰ��Ȥ���©��ʹ��������"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can cause mass impotence.", "���ʤ��Ͻ���Ū������ǽ�򵯤������Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		msg_print(T("You suddenly have a headache!", "����Ƭ���ˤ��ʤä���"));
		take_hit(DAMAGE_LOSELIFE, randint1(17) + 17, T("the strain of forcing abstinence", "���ߤ򶯤�����ϫ"), -1);

		/* Fake a population explosion. */
		num_repro += MAX_REPRO;
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_sterility(void) { return cast_spell(sterility_spell); }

void stone_skin_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Stone Skin");
		break;
	case SPELL_DESC:
		var_set_string(res, "");
		break;
	case SPELL_CAST:
		set_shield(randint1(30) + 20, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_stone_skin(void) { return cast_spell(stone_skin_spell); }

void stone_to_mud_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Stone to Mud", "�����ϲ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Turns one rock square to mud.", "�ɤ��Ϥ����ƾ��ˤ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		wall_to_mud(dir);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_stone_to_mud(void) { return cast_spell(stone_to_mud_spell); }

void summon_manes_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Summon Manes", "����λ����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to summon some demonic friends.", "����λ���򾤴����롣"));
		break;
	case SPELL_CAST:
		if (!summon_specific(-1, py, px, (p_ptr->lev * 3) / 2, SUMMON_MANES, (PM_ALLOW_GROUP | PM_FORCE_PET)))
			msg_print(T("No Manes arrive.", "����λ���ϸ���ʤ��ä���"));

		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void summon_tree_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		if (p_ptr->lev >= 45)
			var_set_string(res, "Summon Trees");
		else
			var_set_string(res, "Summon Tree");
		break;
	case SPELL_SPOIL_NAME:
		var_set_string(res, "Summon Tree");
		break;
	case SPELL_DESC:
		if (p_ptr->lev >= 45)
			var_set_string(res, "Attempts to summon many trees");
		else
			var_set_string(res, "Attempts to summon a tree.");
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Attempts to summon a tree. At L45, attempts to surround the player with trees.");
		break;
	case SPELL_CAST:
		if (p_ptr->lev >= 45)
		{
			tree_creation();
			var_set_bool(res, TRUE);
		}	
		else
		{
			int attempts = 0;
			int x, y, dir;

			var_set_bool(res, TRUE);
			for (;;)
			{
				if (attempts > 4)
				{
					msg_print("No trees arrive.");
					break;
				}

				dir = randint0(9);
				if (dir == 5) continue;

				attempts++;
				y = py + ddy[dir];
				x = px + ddx[dir];

				if (!in_bounds(y, x)) continue;
				if (!cave_naked_bold(y, x)) continue;
				if (player_bold(y, x)) continue;

				cave_set_feat(y, x, feat_tree);
				break;
			}
		}
		break;
	case SPELL_COST_EXTRA:
	{
		int n = 0;
		if (p_ptr->lev >= 45)
			n += 30;

		var_set_int(res, n);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_summon_tree(void) { return cast_spell(summon_tree_spell); }

void super_stealth_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Hide in Darkness", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Grants the stealth of the Ninja!  You may hide in shadows and see in the dark.  Your light radius is decreased by 3.", ""));
		break;
	case SPELL_CAST:
		if (p_ptr->tim_superstealth)
		{
			msg_print("You are already moving in the shadows.");
			var_set_bool(res, FALSE);
		}
		else
		{
			set_tim_superstealth(spell_power(randint1(p_ptr->lev/2) + p_ptr->lev/2), FALSE);
			var_set_bool(res, TRUE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void swap_pos_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Swap Position", "���ָ�"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Swap locations with a given monster.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel like walking a mile in someone else's shoes.", "¾�ͤη��ǰ�ޥ����⤯�褦�ʵ�ʬ�����롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You feel like staying in your own shoes.", "���ʤ��ϼ�ʬ�η���α�ޤ봶�������롣"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can switch locations with another being.", "���ʤ���¾�μԤȾ��������ؤ�뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);

		project_length = -1;
		if (get_aim_dir(&dir))
		{
			teleport_swap(dir);
			var_set_bool(res, TRUE);
		}
		project_length = 0;
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_swap_pos(void) { return cast_spell(swap_pos_spell); }

void sword_dance_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Sword Dancing", "������"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attacks adjacent monsters randomly.");
		break;
	case SPELL_CAST:
	{
		int y = 0, x = 0, i, dir = 0;
		cave_type *c_ptr;

		for (i = 0; i < 6; i++)
		{
			dir = randint0(8);
			y = py + ddy_ddd[dir];
			x = px + ddx_ddd[dir];
			c_ptr = &cave[y][x];

			if (c_ptr->m_idx)
				py_attack(y, x, 0);
			else
				msg_print(T("You attack the empty air.", "���⤬���򤭤ä���"));
		}
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void take_photo_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Take Photograph", "�̿�����"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Creates something to show the kids back home!", ""));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, TRUE);
		if (!get_aim_dir(&dir)) return;
		project_length = 1;
		fire_beam(GF_PHOTO, dir, 1);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void telekinesis_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Telekinesis", "ǰư��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Attempts to fetch a distant object.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the ability to move objects telekinetically.", "ʪ�Τ�ǰư�Ϥ�ư����ǽ�Ϥ�������"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the ability to move objects telekinetically.", "ǰư�Ϥ�ʪ��ư����ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You are telekinetic.", "���ʤ���ǰư�Ϥ��äƤ��롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (get_aim_dir(&dir))
		{
			fetch(dir, p_ptr->lev * 10, TRUE);
			var_set_bool(res, TRUE);
		}
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_telekinesis(void) { return cast_spell(telekinesis_spell); }

void telepathy_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Telepathy");
		break;
	case SPELL_DESC:
		var_set_string(res, "Gives telepathy for a while.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(25, 30));
		break;
	case SPELL_CAST:
		set_tim_esp(randint1(25) + 30, FALSE);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void teleport_other_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Teleport Other", "�ƥ�ݡ��ȡ���������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Teleports all monsters on the line away unless resisted.", "��󥹥�����ƥ�ݡ��Ȥ�����ӡ�������ġ��񹳤�����̵����"));
		break;
	case SPELL_CAST:
	{
		int dir;
		int power = spell_power(p_ptr->lev);

		var_set_bool(res, FALSE);

		if (!get_aim_dir(&dir)) return;
		fire_beam(GF_AWAY_ALL, dir, power);
			
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void teleport_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Teleport", "�ƥ�ݡ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Escape to a distant location.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You gain the power of teleportation at will.", "��ʬ�ΰջפǥƥ�ݡ��Ȥ���ǽ�Ϥ�������"));
		mut_lose(MUT_TELEPORT_RND);
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You lose the power of teleportation at will.", "��ʬ�ΰջפǥƥ�ݡ��Ȥ���ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can teleport at will.", "���ʤ��ϼ�ʬ�ΰջפǥƥ�ݡ��ȤǤ��롣"));
		break;
	case SPELL_CAST:
		teleport_player(10 + 4 * p_ptr->lev, 0);
		var_set_bool(res, TRUE);
		break;
	case SPELL_ENERGY:
		if (mut_present(MUT_ASTRAL_GUIDE))
			var_set_int(res, 30);
		else
			default_spell(cmd, res);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_teleport(void) { return cast_spell(teleport_spell); }

void teleport_level_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Teleport Level", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, "Escape to another level.");
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (!get_check("Are you sure? (Teleport Level)")) return;
		teleport_level(0);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_teleport_level(void) { return cast_spell(teleport_level_spell); }

void teleport_to_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Teleport To", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, "Teleport a visible monster next to you.");
		break;
	case SPELL_CAST:
	{
		monster_type *m_ptr;
		monster_race *r_ptr;
		char m_name[80];

		if (!target_set(TARGET_KILL)) break;
		if (!cave[target_row][target_col].m_idx) break;
		if (!player_has_los_bold(target_row, target_col)) break;
		if (!projectable(py, px, target_row, target_col)) break;

		var_set_bool(res, TRUE);

		m_ptr = &m_list[cave[target_row][target_col].m_idx];
		r_ptr = &r_info[m_ptr->r_idx];
		monster_desc(m_name, m_ptr, 0);
		if (r_ptr->flagsr & RFR_RES_TELE)
		{
			if ((r_ptr->flags1 & (RF1_UNIQUE)) || (r_ptr->flagsr & RFR_RES_ALL))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
				msg_format("%s is unaffected!", m_name);
				break;
			}
			else if (r_ptr->level > randint1(100))
			{
				if (is_original_ap_and_seen(m_ptr)) r_ptr->r_flagsr |= RFR_RES_TELE;
				msg_format("%s resists!", m_name);
				break;
			}
		}
		msg_format("You command %s to return.", m_name);
		teleport_monster_to(cave[target_row][target_col].m_idx, py, px, 100, TELEPORT_PASSIVE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void throw_boulder_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Throw Boulder", "�����ꤲ�ʥ��᡼��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Hurls a huge boulder at chosen target.");
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Hurls a huge boulder at chosen target for 3L damage.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(3 * p_ptr->lev)));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		msg_print(T("You throw a huge boulder.", "����ʴ���ꤲ����"));
		fire_bolt(GF_MISSILE, dir, spell_power(3 * p_ptr->lev));
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void touch_of_confusion_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Touch of Confusion", ""));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to confuse the next monster that you hit.", ""));
		break;
	case SPELL_CAST:
		if (!(p_ptr->special_attack & ATTACK_CONFUSE))
		{
			msg_print("Your hands start glowing.");
			p_ptr->special_attack |= ATTACK_CONFUSE;
			p_ptr->redraw |= (PR_STATUS);
		}
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void turn_undead_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Turn Undead", "�ѥ˥å�������ǥå�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Attempts to scare undead monsters in sight.", "�볦��Υ���ǥåɤ��ݤ����롣�񹳤�����̵����"));
		break;
	case SPELL_CAST:
		if (project_hack(GF_TURN_UNDEAD, spell_power(p_ptr->lev)))
			chg_virtue(V_UNLIFE, -1);
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

void vampirism_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Vampiric Drain", "�۷�ɥ쥤��"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Suck blood from an adjacent monster, gaining hp in the process.");
		break;
	case SPELL_INFO:
		var_set_string(res, info_damage(0, 0, spell_power(p_ptr->lev * 2)));
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You become vampiric.", "��̿�Ϥ�ۼ��Ǥ���褦�ˤʤä���"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You are no longer vampiric.", "�۷��ǽ�Ϥ򼺤ä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can drain life from a foe like a vampire.", "���ʤ��ϵ۷쵴�Τ褦��Ũ������̿�Ϥ�ۼ����뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (d_info[dungeon_type].flags1 & DF1_NO_MELEE)
		{
			msg_print(T("Something prevent you from attacking.", "�ʤ������⤹�뤳�Ȥ��Ǥ��ʤ���"));
			return;
		}
		else
		{
			int x, y, dummy;
			cave_type *c_ptr;
			int dir = 0;

			/* Only works on adjacent monsters */
			if (!get_rep_dir2(&dir)) break;

			var_set_bool(res, TRUE);

			y = py + ddy[dir];
			x = px + ddx[dir];
			c_ptr = &cave[y][x];

			stop_mouth();

			if (!(c_ptr->m_idx))
			{
				msg_print(T("You bite into thin air!", "����ʤ����˳��ߤĤ�����"));
				break;
			}

			msg_print(T("You grin and bare your fangs...", "���ʤ��ϥ˥��Ȥ��Ʋ��त��..."));
			dummy = spell_power(p_ptr->lev * 2);

			if (drain_life(dir, dummy))
			{
				/* No heal if we are "full" */
				if (p_ptr->food < PY_FOOD_FULL)
					hp_player(dummy);
				else
					msg_print(T("You were not hungry.", "���ʤ��϶�ʢ�ǤϤ���ޤ���"));

				/* Gain nutritional sustenance: 150/hp drained
				 * A Food ration gives 5000 food points (by contrast)
				 * Don't ever get more than "Full" this way
				 * But if we ARE Gorged,  it won't cure us 
				 */
				dummy = p_ptr->food + MIN(5000, 100 * dummy);
				if (p_ptr->food < PY_FOOD_MAX)   /* Not gorged already */
					set_food(dummy >= PY_FOOD_MAX ? PY_FOOD_MAX-1 : dummy);
			}
			else
				msg_print(T("Yechh. That tastes foul.", "�������Ҥɤ�̣����"));
		}
		break;
	case SPELL_COST_EXTRA:
		var_set_int(res, p_ptr->lev / 3);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_vampirism(void) { return cast_spell(vampirism_spell); }

void weigh_magic_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Weigh Magic", "���ϴ���"));
		break;
	case SPELL_DESC:
		var_set_string(res, "Determine the strength of magics affecting you.");
		break;
	case SPELL_GAIN_MUT:
		msg_print(T("You feel you can better understand the magic around you.", "���ʤ��ϼ��Ϥˤ�����ˡ�����ɤ�����Ǥ��뵤�����롣"));
		break;
	case SPELL_LOSE_MUT:
		msg_print(T("You no longer sense magic.", "���Ϥ򴶤����ʤ��ʤä���"));
		break;
	case SPELL_MUT_DESC:
		var_set_string(res, T("You can feel the strength of the magics affecting you.", "���ʤ��ϼ�ʬ�˱ƶ���Ϳ������ˡ���Ϥ򴶤��뤳�Ȥ��Ǥ��롣"));
		break;
	case SPELL_CAST:
		report_magics();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}
bool cast_weigh_magic(void) { return cast_spell(weigh_magic_spell); }

void wonder_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Wonder", "������"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Fires something with random effects.", "��󥹥����˥�����ʸ��̤�Ϳ���롣"));
		break;
	case SPELL_CAST:
	{
		int dir = 0;
		var_set_bool(res, FALSE);
		if (!get_aim_dir(&dir)) return;
		cast_wonder(dir);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}

void wraithform_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, T("Wraithform", "ͩ�β�"));
		break;
	case SPELL_DESC:
		var_set_string(res, T("Leave the world of the living and travel the shadows of the underwold.  You gain passwall and great resistance to damage.", "������֡��ɤ��̤�ȴ���뤳�Ȥ��Ǥ���������᡼�����ڸ������ͩ�Τξ��֤��ѿȤ��롣"));
		break;
	case SPELL_INFO:
		var_set_string(res, info_duration(spell_power(p_ptr->lev/2), spell_power(p_ptr->lev/2)));
		break;
	case SPELL_CAST:
	{
		int base = spell_power(p_ptr->lev / 2);
		set_wraith_form(randint1(base) + base, FALSE);
		var_set_bool(res, TRUE);
		break;
	}
	default:
		default_spell(cmd, res);
		break;
	}
}
