/* File: store.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Store commands */

#include "angband.h"

#define MIN_STOCK 12

static int cur_store_num = 0;
static int store_top = 0;
static int store_bottom = 0;
static int xtra_stock = 0;
static store_type *st_ptr = NULL;
static owner_type *ot_ptr = NULL;
static s16b old_town_num = 0;
static s16b inner_town_num = 0;
#define RUMOR_CHANCE 8

#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] =
{
#ifdef JP
	"オーケーだ。",
	"結構だ。",
	"そうしよう！",
	"賛成だ！",
	"よし！",
	"わかった！"
#else
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
#endif

};

#ifdef JP
/* ブラックマーケット追加メッセージ（承諾） */
static cptr comment_1_B[MAX_COMMENT_1] = {
	"まあ、それでいいや。",
	"今日はそれで勘弁してやる。",
	"分かったよ。",
	"しょうがない。",
	"それで我慢するよ。",
	"こんなもんだろう。"
};
#endif
#define MAX_COMMENT_2A	2

static cptr comment_2a[MAX_COMMENT_2A] =
{
#ifdef JP
	"私の忍耐力を試しているのかい？ $%s が最後だ。",
	"我慢にも限度があるぞ。 $%s が最後だ。"
#else
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
#endif

};

#define MAX_COMMENT_2B	12

static cptr comment_2b[MAX_COMMENT_2B] =
{
#ifdef JP
	" $%s ぐらいは出さなきゃダメだよ。",
	" $%s なら受け取ってもいいが。",
	"ハ！ $%s 以下はないね。",
	"何て奴だ！ $%s 以下はあり得ないぞ。",
	"それじゃ少なすぎる！ $%s は欲しいところだ。",
	"バカにしている！ $%s はもらわないと。",
	"嘘だろう！ $%s でどうだい？",
	"おいおい！ $%s を考えてくれないか？",
	"1000匹のオークのノミに苦しめられるがいい！ $%s だ。",
	"お前の大切なものに災いあれ！ $%s でどうだ。",
	"モルゴスに賞味されるがいい！本当は $%s なんだろう？",
	"お前の母親はオーガか！ $%s は出すつもりなんだろ？"
#else
	"I can take no less than %s gold pieces.",
	"I will accept no less than %s gold pieces.",
	"Ha!  No less than %s gold pieces.",
	"You knave!  No less than %s gold pieces.",
	"That's a pittance!  I want %s gold pieces.",
	"That's an insult!  I want %s gold pieces.",
	"As if!  How about %s gold pieces?",
	"My arse!  How about %s gold pieces?",
	"May the fleas of 1000 orcs molest you!  Try %s gold pieces.",
	"May your most favourite parts go moldy!  Try %s gold pieces.",
	"May Morgoth find you tasty!  Perhaps %s gold pieces?",
	"Your mother was an Ogre!  Perhaps %s gold pieces?"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（売るとき） */
static cptr comment_2b_B[MAX_COMMENT_2B] = {
	"いくら俺様がお人好しとはいえ $%s が限界だね。嫌なら帰りな。",
	"金がないのかい、あんた？まずは家に帰って $%s 揃えてきな。",
	"物の価値が分からん奴だな。これは $%s が普通なんだよ。",
	"俺の付けた値段に文句があるのか？ $%s が限界だ。",
	"ひょっとして新手の冗談かい？ $%s 持ってないなら帰りな。",
	"うちは他の店とは違うんだよ。$%s ぐらいは出しな。",
	"買う気がないなら帰りな。 $%s だと言っているんだ。",
	"話にならないね。 $%s くらい持っているんだろ？",
	"は？なんだそりゃ？ $%s の間違いか、ひょっとして？",
	"出口はあっちだよ。それとも $%s 出せるのかい、あんたに。",
	"命知らずな奴だな。 $%s 出せば今日の所は勘弁してやるよ。",
	"うちの店は貧乏人お断りだ。 $%s ぐらい出せないのかい？"
};
#endif
#define MAX_COMMENT_3A	2

static cptr comment_3a[MAX_COMMENT_3A] =
{
#ifdef JP
	"私の忍耐力を試しているのかい？ $%s が最後だ。",
	"我慢にも限度があるぞ。 $%s が最後だ。"
#else
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
#endif

};


#define MAX_COMMENT_3B	12

static cptr comment_3b[MAX_COMMENT_3B] =
{
#ifdef JP
	"本音を言うと $%s でいいんだろ？",
	" $%s でどうだい？",
	" $%s ぐらいなら出してもいいが。",
	" $%s 以上払うなんて考えられないね。",
	"まあ落ちついて。 $%s でどうだい？",
	"そのガラクタなら $%s で引き取るよ。",
	"それじゃ高すぎる！ $%s がいいとこだろ。",
	"どうせいらないんだろ！ $%s でいいだろ？",
	"だめだめ！ $%s がずっとお似合いだよ。",
	"バカにしている！ $%s がせいぜいだ。",
	" $%s なら嬉しいところだがなあ。",
	" $%s 、それ以上はビタ一文出さないよ！"
#else
	"Perhaps %s gold pieces?",
	"How about %s gold pieces?",
	"I will pay no more than %s gold pieces.",
	"I can afford no more than %s gold pieces.",
	"Be reasonable.  How about %s gold pieces?",
	"I'll buy it as scrap for %s gold pieces.",
	"That is too much!  How about %s gold pieces?",
	"That looks war surplus!  Say %s gold pieces?",
	"Never!  %s is more like it.",
	"That's an insult!  %s is more like it.",
	"%s gold pieces and be thankful for it!",
	"%s gold pieces and not a copper more!"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（買い取り） */
static cptr comment_3b_B[MAX_COMMENT_3B] = {
	" $%s ってところだね。そのどうしようもないガラクタは。",
	"この俺が $%s って言っているんだから、その通りにした方が身のためだぞ。",
	"俺の優しさに甘えるのもいい加減にしておけ。 $%s だ。",
	"その品なら $%s で売ってくれているがね、常識ある紳士はみんな。",
	"こりゃまた、がめつい奴だな。いくら俺が温厚とはいえ $%s が限界だ。",
	" $%s だ。別に俺はそんなガラクタ欲しくはないんだから。",
	"俺の鑑定額が気に入らないのか？ $%s 、嫌なら帰りな。",
	" $%s で引き取ってやるよ。喜んで受け取りな、貧乏人。",
	"物の価値が分からん奴は始末におえんな。それは $%s なんだよ。",
	"そんなに金が欲しいのか、あんた？ $%s で満足できんのか？",
	"入る店間違えてんじゃないのか？ $%s で嫌なら他をあたってくれ。",
	"俺の言い値にケチをつける奴がいるとは！ その度胸に免じて $%s だ。"
};
#endif
#define MAX_COMMENT_4A	4

static cptr comment_4a[MAX_COMMENT_4A] =
{
#ifdef JP
	"もうたくさんだ！何度も私をわずらわせないでくれ！",
	"うがー！一日の我慢の限度を超えている！",
	"もういい！時間の無駄以外のなにものでもない！",
	"もうやってられないよ！顔も見たくない！"
#else
	"Enough!  You have abused me once too often!",
	"Arghhh!  I have had enough abuse for one day!",
	"That does it!  You shall waste my time no more!",
	"This is getting nowhere!  I'm going to Londis!"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（怒りの頂点） */
static cptr comment_4a_B[MAX_COMMENT_4A] = {
	"なめやがって！温厚な俺様でも限界があるってことを知れ！",
	"俺をここまで怒らせて...命があるだけでもありがたいと思え！",
	"ふざけてるのか！冷やかしなら相手を見てからにしろ！",
	"いいかげんにしろ！今度こんなまねしたらただじゃおかねえぞ！"
};
#endif
#define MAX_COMMENT_4B	4

static cptr comment_4b[MAX_COMMENT_4B] =
{
#ifdef JP
	"店から出て行け！",
	"俺の前から消え失せろ！",
	"どっかに行っちまえ！",
	"出ろ、出ろ、出て行け！"
#else
	"Leave my store!",
	"Get out of my sight!",
	"Begone, you scoundrel!",
	"Out, out, out!"
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（追い出し） */
static cptr comment_4b_B[MAX_COMMENT_4B] = {
	"二度とうちに来るんじゃねえ！！",
	"とっとと、どっかへ失せろ！！",
	"今すぐ消え失せろ！！",
	"出ていけ！出ていけ！！"
};
#endif
#define MAX_COMMENT_5	8

static cptr comment_5[MAX_COMMENT_5] =
{
#ifdef JP
	"考え直してくれ。",
	"そりゃおかしい！",
	"もっと真面目に言ってくれ！",
	"交渉する気があるのかい？",
	"冷やかしに来たのか！",
	"悪い冗談だ！",
	"我慢くらべかい。",
	"ふーむ、良い天気だ。"
#else
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
#endif

};

#ifdef JP
/* ブラックマーケット用追加メッセージ（怒り） */
static cptr comment_5_B[MAX_COMMENT_5] = {
	"時間の無駄だな、これは。",
	"厄介なお客様だな！",
	"話して分かる相手じゃなさそうだ。",
	"痛い目にあいたいらしいな！",
	"なんて強欲な奴だ！",
	"話にならん輩だ！",
	"どうしようもない貧乏人だ！",
	"喧嘩を売っているのか？"
};
#endif
#define MAX_COMMENT_6	4

static cptr comment_6[MAX_COMMENT_6] =
{
#ifdef JP
	"どうやら聞き間違えたらしい。",
	"失礼、よく聞こえなかったよ。",
	"すまない、何だって？",
	"悪い、もう一度言ってくれる？"
#else
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
#endif

};

static void _restock(store_type *st_ptr, bool all);

/*
 * Successful haggle.
 */
static void say_comment_1(void)
{
	char rumour[1024];

#ifdef JP
	/* ブラックマーケットのときは別のメッセージを出す */
	if ( cur_store_num == STORE_BLACK ) {
		msg_print(comment_1_B[randint0(MAX_COMMENT_1)]);
	}
	else{
		msg_print(comment_1[randint0(MAX_COMMENT_1)]);
	}
#else
	msg_print(comment_1[randint0(MAX_COMMENT_1)]);
#endif


	if (one_in_(RUMOR_CHANCE))
	{
#ifdef JP
msg_print("店主は耳うちした:");
#else
		msg_print("The shopkeeper whispers something into your ear:");
#endif


#ifdef JP
if (!get_rnd_line_jonly("rumors_j.txt", 0, rumour, 10))
#else
		if (!get_rnd_line("rumors.txt", 0, rumour))
#endif

			msg_print(rumour);
	}
}


/*
 * Continue haggling (player is buying)
 */
static void say_comment_2(s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_2a[randint0(MAX_COMMENT_2A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
#ifdef JP
		/* ブラックマーケットの時は別のメッセージを出す */
		if ( cur_store_num == STORE_BLACK ){
			msg_format(comment_2b_B[randint0(MAX_COMMENT_2B)], tmp_val);
		}
		else{
		msg_format(comment_2b[randint0(MAX_COMMENT_2B)], tmp_val);
	}
#else
		msg_format(comment_2b[randint0(MAX_COMMENT_2B)], tmp_val);
#endif

	}
}


/*
 * Continue haggling (player is selling)
 */
static void say_comment_3(s32b value, int annoyed)
{
	char	tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_3a[randint0(MAX_COMMENT_3A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
#ifdef JP
		/* ブラックマーケットの時は別のメッセージを出す */
		if ( cur_store_num == STORE_BLACK ){
			msg_format(comment_3b_B[randint0(MAX_COMMENT_3B)], tmp_val);
		}
		else{
		msg_format(comment_3b[randint0(MAX_COMMENT_3B)], tmp_val);
	}
#else
		msg_format(comment_3b[randint0(MAX_COMMENT_3B)], tmp_val);
#endif

	}
}


/*
 * Kick 'da bum out.					-RAK-
 */
static void say_comment_4(void)
{
#ifdef JP
	/* ブラックマーケットの時は別のメッセージを出す */
	if ( cur_store_num == STORE_BLACK ){
		msg_print(comment_4a_B[randint0(MAX_COMMENT_4A)]);
		msg_print(comment_4b_B[randint0(MAX_COMMENT_4B)]);
	}
	else{
		msg_print(comment_4a[randint0(MAX_COMMENT_4A)]);
		msg_print(comment_4b[randint0(MAX_COMMENT_4B)]);
	}
#else
	msg_print(comment_4a[randint0(MAX_COMMENT_4A)]);
	msg_print(comment_4b[randint0(MAX_COMMENT_4B)]);
#endif

}


/*
 * You are insulting me
 */
static void say_comment_5(void)
{
#ifdef JP
	/* ブラックマーケットの時は別のメッセージを出す */
	if ( cur_store_num == STORE_BLACK ){
		msg_print(comment_5_B[randint0(MAX_COMMENT_5)]);
	}
	else{
		msg_print(comment_5[randint0(MAX_COMMENT_5)]);
	}
#else
	msg_print(comment_5[randint0(MAX_COMMENT_5)]);
#endif

}


/*
 * That makes no sense.
 */
static void say_comment_6(void)
{
	msg_print(comment_6[randint0(MAX_COMMENT_6)]);
}



/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] =
{
#ifdef JP
	"うわああぁぁ！",
	"なんてこった！",
	"誰かがむせび泣く声が聞こえる...。",
	"店主が悔しげにわめいている！"
#else
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
#endif

};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] =
{
#ifdef JP
	"くそう！",
	"この悪魔め！",
	"店主が恨めしそうに見ている。",
	"店主が睨んでいる。"
#else
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
#endif

};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] =
{
#ifdef JP
	"すばらしい！",
	"君が天使に見えるよ！",
	"店主がクスクス笑っている。",
	"店主が大声で笑っている。"
#else
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
#endif

};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] =
{
#ifdef JP
	"やっほぅ！",
	"こんなおいしい思いをしたら、真面目に働けなくなるなぁ。",
	"店主は嬉しくて跳ね回っている。",
	"店主は満面に笑みをたたえている。"
#else
	"Yipee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
#endif

};


/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
static void purchase_analyze(s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	if ((value <= 0) && (price > value))
	{
		/* Comment */
		msg_print(comment_7a[randint0(MAX_COMMENT_7A)]);

		chg_virtue(V_HONOUR, -1);
		chg_virtue(V_JUSTICE, -1);

		/* Sound */
		sound(SOUND_STORE1);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{
		/* Comment */
		msg_print(comment_7b[randint0(MAX_COMMENT_7B)]);

		chg_virtue(V_JUSTICE, -1);
		if (one_in_(4))
			chg_virtue(V_HONOUR, -1);

		/* Sound */
		sound(SOUND_STORE2);
	}

	/* Item was a good bargain, and we got away with it */
	else if ((value > guess) && (value < (4 * guess)) && (price < value))
	{
		/* Comment */
		msg_print(comment_7c[randint0(MAX_COMMENT_7C)]);

		if (one_in_(4))
			chg_virtue(V_HONOUR, -1);
		else if (one_in_(4))
			chg_virtue(V_HONOUR, 1);

		/* Sound */
		sound(SOUND_STORE3);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value > guess) && (price < value))
	{
		/* Comment */
		msg_print(comment_7d[randint0(MAX_COMMENT_7D)]);

		if (one_in_(2))
			chg_virtue(V_HONOUR, -1);
		if (one_in_(4))
			chg_virtue(V_HONOUR, 1);

		if (10 * price < value)
			chg_virtue(V_SACRIFICE, 1);

		/* Sound */
		sound(SOUND_STORE4);
	}
}



/*
 * We store the current "store feat" here so everyone can access it
 */
static int cur_store_feat;


/*
 * Buying and selling adjustments for race combinations.
 * Entry[owner][player] gives the basic "cost inflation".
 */
static byte rgold_adj[MAX_RACES][MAX_RACES] =
{
	/*Hum, HfE, Elf,  Hal, Gno, Dwa, HfO, HfT, Dun, HiE, Barbarian,
	 HfOg, HGn, HTn, Cyc, Yek, Klc, Kbd, Nbl, DkE, Drc, Mind Flayer,
	 Imp,  Glm, Skl, Zombie, Vampire, Spectre, Fairy, Beastman, Ent,
	 Angel, Demon, Kutar */

	/* Human */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 105, 100 },

	/* Tonbery */
	{ 110, 100, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  120, 115, 108, 115, 110, 110, 120, 120, 115, 115, 110,
	  120, 110, 110, 110, 120, 110, 100, 125, 100,  95, 140,
	  110, 115, 110, 110, 100 },

	/* Demigod */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  120, 120, 105, 120, 110, 105, 125, 125, 110, 115, 108,
	  120, 115, 110, 110, 120, 110, 100, 125, 100,  95, 140,
	  110, 110, 105, 110, 100 },

	/* Halfling */
	{ 115, 110, 105,  95, 105, 110, 115, 130, 115, 105, 115,
	  125, 120, 120, 125, 115, 110, 120, 120, 120, 115, 115,
	  120, 110, 120, 120, 130, 110, 110, 130, 110,  95, 140,
	  115, 120, 105, 115, 100 },

	/* Gnome */
	{ 115, 115, 110, 105,  95, 110, 115, 130, 115, 110, 115,
	  120, 125, 110, 120, 110, 105, 120, 110, 110, 105, 110,
	  120, 101, 110, 110, 120, 120, 115, 130, 115,  95, 140,
	  115, 110, 110, 115, 100 },

	/* Dwarf */
	{ 115, 120, 120, 110, 110,  95, 125, 135, 115, 120, 115,
	  125, 140, 130, 130, 120, 115, 115, 115, 135, 125, 120,
	  120, 105, 115, 115, 115, 115, 120, 130, 120,  95, 140,
	  115, 110, 115, 115, 100 },

	/* Half-Orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 115, 120, 125, 115, 125,  95, 140,
	  115, 110, 115, 115, 100 },

	/* Half-Troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 120, 110, 120,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Amberite */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  120, 120, 105, 120, 115, 105, 115, 120, 110, 105, 105,
	  120, 105, 120, 120, 125, 120, 105, 135, 105,  95, 140,
	  100, 110, 110, 100, 100 },

	/* High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 125, 120, 100, 125, 100,  95, 140,
	  110, 110, 105, 110, 100 },

	/* Human / Barbarian (copied from human) */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 130, 125, 115, 120, 115,  95, 140,
	  100, 120, 110, 100, 100 },

	/* Half-Ogre: theoretical, copied from half-troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 120, 110, 120,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Half-Giant: theoretical, copied from half-troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 130, 120, 130,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Half-Titan: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Cyclops: theoretical, copied from half-troll */
	{ 110, 115, 115, 110, 110, 130, 110, 110, 110, 115, 110,
	  110, 115, 120, 110, 120, 120, 110, 110, 110, 115, 110,
	  110, 115, 112, 112, 115, 112, 130, 130, 130,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Yeek: theoretical, copied from Half-Orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115, 100 },

	/* Klackon: theoretical, copied from Gnome */
	{ 115, 115, 110, 105,  95, 110, 115, 130, 115, 110, 115,
	  120, 125, 110, 120, 110, 105, 120, 110, 110, 105, 110,
	  120, 101, 110, 110, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115, 100 },

	/* Kobold: theoretical, copied from Half-Orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115, 100 },

	/* Nibelung: theoretical, copied from Dwarf */
	{ 115, 120, 120, 110, 110,  95, 125, 135, 115, 120, 115,
	  125, 140, 130, 130, 120, 115, 115, 115, 135, 125, 120,
	  120, 105, 115, 115, 120, 120, 130, 130, 130,  95, 140,
	  115, 135, 115, 115, 100 },

	/* Dark Elf */
	{ 110, 110, 110, 115, 120, 130, 115, 115, 120, 110, 115,
	  115, 115, 116, 115, 120, 120, 115, 115, 101, 110, 110,
	  110, 110, 112, 122, 110, 110, 110, 115, 110, 120, 120,
	  110, 101, 115, 110, 100 },

	/* Draconian: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110 },

	/* Mind Flayer: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Imp: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  110, 110, 115, 110, 100 },

	/* Golem: theoretical, copied from High_Elf */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  125, 125, 101, 120, 115, 110, 115, 125, 110, 110, 110,
	  125, 115, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  110, 110, 115, 110, 100 },

	/* Skeleton: theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115, 100 },

	/* Zombie: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115, 100 },

	/* Vampire: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115, 100 },

	/* Spectre: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130, 120, 120,
	  115, 110, 125, 115, 100 },

	/* Sprite: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 105, 115, 100 },

	/* Beastman: Theoretical, copied from half-orc */
	{ 115, 120, 125, 115, 115, 130, 110, 115, 115, 125, 115,
	  110, 110, 120, 110, 120, 125, 115, 115, 110, 120, 110,
	  115, 125, 120, 120, 120, 120, 130, 130, 130,  95, 140,
	  115, 110, 115, 115, 100 },

	/* Ent */
	{ 110, 105, 100, 105, 110, 120, 125, 130, 110, 100, 110,
	  120, 120, 105, 120, 110, 105, 125, 125, 110, 115, 108,
	  120, 115, 110, 110, 120, 110, 100, 125, 100,  95, 140,
	  110, 110, 105, 110, 100 },

	/* Angel */
	{  95,  95,  95,  95,  95,  95,  95,  95,  95,  95,  95,
	   95,  95,  95,  95,  95,  95,  95,  95,  95,  95,  95,
	   95,  95,  95,  95,  95,  95,  95,  95,  95,  95, 160,
	   95,  95,  95,  95, 100 },

	/* Demon */
	{ 140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
	  140, 140, 140, 140, 140, 140, 140, 140, 140, 140, 140,
	  140, 140, 140, 140, 140, 140, 140, 140, 140, 160, 120,
	  140, 140, 140, 140, 100 },

	/* Dunadan */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 100, 100 },

	/* Shadow Fairy */
	{ 110, 110, 110, 115, 120, 130, 115, 115, 120, 110, 115,
	  115, 115, 116, 115, 120, 120, 115, 115, 101, 110, 110,
	  110, 110, 112, 122, 110, 110, 110, 115, 110, 120, 120,
	  110, 101, 115, 110, 100 },

	/* Kutar */
	{ 110, 110, 105, 105, 110, 115, 115, 115, 110, 105, 110,
	  115, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115,
	  115, 115, 125, 125, 125, 125, 105, 115, 105,  95, 140,
	  110, 115, 100, 110, 100 },

	/* Android */
	{ 105, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 100, 100 },

	/* Doppelganger (Copied Human) */
	{ 100, 105, 105, 110, 113, 115, 120, 125, 100, 105, 100,
	  124, 120, 110, 125, 115, 120, 120, 120, 120, 115, 120,
	  115, 105, 125, 125, 125, 125, 105, 120, 105,  95, 140,
	  100, 120, 110, 105, 100 },
};




/*
 * Determine the price of an item (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * item, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
static s32b price_item(object_type *o_ptr, int greed, bool flip)
{
	int 	factor;
	int 	adjust;
	s32b	price;


	/* Get the value of one of the items */
	price = object_value(o_ptr);

	/* Worthless items */
	if (price <= 0) return (0L);


	/* Compute the racial factor */
	if (prace_is_(RACE_DEMIGOD) && p_ptr->psubrace == DEMIGOD_APHRODITE && flip)
		factor = 100;
	else
		factor = rgold_adj[ot_ptr->owner_race][p_ptr->prace];

	/* Add in the charisma factor */
	factor += adj_gold[p_ptr->stat_ind[A_WIS]];


	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (cur_store_num == STORE_BLACK && p_ptr->realm1 != REALM_BURGLARY)
			price = price / 2;

		/* Compute the final price (with rounding) */
		/* Hack -- prevent underflow */
		price = (price * adjust + 50L) / 100L;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (cur_store_num == STORE_BLACK && p_ptr->realm1 != REALM_BURGLARY)
			price = price * 2;

		/* Compute the final price (with rounding) */
		/* Hack -- prevent overflow */
		price = (s32b)(((u32b)price * (u32b)adjust + 50UL) / 100UL);
	}

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Certain "cheap" objects should be created in "piles"
 * Some objects can be sold at a "discount" (in small piles)
 */
static void mass_produce(object_type *o_ptr)
{
	int size = 1;
	int discount = 0;

	s32b cost = object_value(o_ptr);


	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			if (cost <= 5L) size += damroll(3, 5);
			if (cost <= 20L) size += damroll(3, 5);
			if (cost <= 50L) size += damroll(2, 2);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			/* Hack: Let's rewrite the store stocking code ...
			   In the meantime, tone down the number of resist heat and cold */
			if (o_ptr->tval == TV_POTION && (o_ptr->sval == SV_POTION_RESIST_HEAT || o_ptr->sval == SV_POTION_RESIST_COLD))
			{
				 size += randint0(3);
			}
			else
			{
				if (cost <= 60L) size += damroll(3, 5);
				if (cost <= 240L) size += damroll(1, 5);
				if (o_ptr->sval == SV_SCROLL_STAR_IDENTIFY) size += damroll(3, 5);
				if (o_ptr->sval == SV_SCROLL_STAR_REMOVE_CURSE) size += damroll(1, 4);
			}
			break;
		}

		case TV_LIFE_BOOK:
		case TV_SORCERY_BOOK:
		case TV_NATURE_BOOK:
		case TV_CHAOS_BOOK:
		case TV_DEATH_BOOK:
		case TV_TRUMP_BOOK:
		case TV_ARCANE_BOOK:
		case TV_CRAFT_BOOK:
		case TV_DAEMON_BOOK:
		case TV_CRUSADE_BOOK:
		case TV_NECROMANCY_BOOK:
		case TV_ARMAGEDDON_BOOK:
		case TV_MUSIC_BOOK:
		case TV_HISSATSU_BOOK:
		case TV_HEX_BOOK:
		case TV_RAGE_BOOK:
		{
			if (cost <= 50L) size += damroll(2, 3);
			if (cost <= 500L) size += damroll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SHIELD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		case TV_BOW:
		{
			if (object_is_artifact(o_ptr)) break;
			if (object_is_ego(o_ptr)) break;
			if (cost <= 10L) size += damroll(3, 5);
			if (cost <= 100L) size += damroll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			if (cost <= 5L) size += damroll(5, 5);
			if (cost <= 50L) size += damroll(5, 5);
			if (cost <= 500L) size += damroll(5, 5);
			break;
		}

		case TV_FIGURINE:
		{
			if (cost <= 100L) size += damroll(2, 2);
			if (cost <= 1000L) size += damroll(2, 2);
			break;
		}

		case TV_CAPTURE:
		case TV_STATUE:
		case TV_CARD:
		{
			size = 1;
			break;
		}

		/*
		 * Because many rods (and a few wands and staffs) are useful mainly
		 * in quantity, the Black Market will occasionally have a bunch of
		 * one kind. -LM-
		 */
		case TV_ROD:
		case TV_WAND:
		case TV_STAFF:
		{
			if ((cur_store_num == STORE_BLACK) && one_in_(3))
			{
				if (cost < 1601L) size += damroll(1, 5);
				else if (cost < 3201L) size += damroll(1, 3);
			}
			break;
		}
	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (one_in_(25))
	{
		discount = 25;
	}
	else if (one_in_(150))
	{
		discount = 50;
	}
	else if (one_in_(300))
	{
		discount = 75;
	}
	else if (one_in_(500))
	{
		discount = 90;
	}


	if (o_ptr->art_name)
	{
		if (cheat_peek && discount)
		{
#ifdef JP
msg_print("ランダムアーティファクトは値引きなし。");
#else
			msg_print("No discount on random artifacts.");
#endif

		}
		discount = 0;
	}

	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);

	/* Ensure that mass-produced rods and wands get the correct pvals. */
	if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
	{
		o_ptr->pval *= o_ptr->number;
	}
}



/*
 * Determine if a store item can "absorb" another item
 *
 * See "object_similar()" for the same function for the "player"
 */
static bool store_object_similar(object_type *o_ptr, object_type *j_ptr)
{
	int i;

	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);

	/* Different charges (etc) cannot be stacked, unless wands or rods. */
	if ((o_ptr->pval != j_ptr->pval) && (o_ptr->tval != TV_WAND) && (o_ptr->tval != TV_ROD)) return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (0);
	if (o_ptr->to_d != j_ptr->to_d) return (0);
	if (o_ptr->to_a != j_ptr->to_a) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2) return (0);

	/* Artifacts don't stack! */
	if (object_is_artifact(o_ptr) || object_is_artifact(j_ptr)) return (0);

	/* Hack -- Identical art_flags! */
	for (i = 0; i < TR_FLAG_SIZE; i++)
		if (o_ptr->art_flags[i] != j_ptr->art_flags[i]) return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	/* Hack -- Never stack recharging items */
	if (o_ptr->timeout || j_ptr->timeout) return (0);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac)   return (0);
	if (o_ptr->dd != j_ptr->dd)   return (0);
	if (o_ptr->ds != j_ptr->ds)   return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);
	if (o_ptr->tval == TV_STATUE) return (0);
	if (o_ptr->tval == TV_CAPTURE) return (0);

	/* Require matching discounts */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store item to absorb another item
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int max_num = (o_ptr->tval == TV_ROD) ?
		MIN(99, MAX_SHORT / k_info[o_ptr->k_idx].pval) : 99;
	int total = o_ptr->number + j_ptr->number;
	int diff = (total > max_num) ? total - max_num : 0;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > max_num) ? max_num : total;

	/* Hack -- if rods are stacking, add the pvals (maximum timeouts) together. -LM- */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
	}

	/* Hack -- if wands are stacking, combine the charges. -LM- */
	if (o_ptr->tval == TV_WAND)
	{
		o_ptr->pval += j_ptr->pval * (j_ptr->number - diff) / j_ptr->number;
	}
}


/*
 * Check to see if the shop will be carrying too many objects	-RAK-
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.	Before, one could "nuke" potions this way.
 *
 * Return value is now int:
 *  0 : No space
 * -1 : Can be combined to existing slot.
 *  1 : Cannot be combined but there are empty spaces.
 */
static int store_check_num(object_type *o_ptr)
{
	int 	   i;
	object_type *j_ptr;

	/* The "home" acts like the player */
	if ((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM))
	{
		bool old_stack_force_notes = stack_force_notes;
		bool old_stack_force_costs = stack_force_costs;

		if (cur_store_num != STORE_HOME)
		{
			stack_force_notes = FALSE;
			stack_force_costs = FALSE;
		}

		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr))
			{
				if (cur_store_num != STORE_HOME)
				{
					stack_force_notes = old_stack_force_notes;
					stack_force_costs = old_stack_force_costs;
				}

				return -1;
			}
		}

		if (cur_store_num != STORE_HOME)
		{
			stack_force_notes = old_stack_force_notes;
			stack_force_costs = old_stack_force_costs;
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the items */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing item */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return -1;
		}
	}

	/* Free space is always usable */
	/*
	 * オプション powerup_home が設定されていると
	 * 我が家が 20 ページまで使える
	 */
	if ((cur_store_num == STORE_HOME) && ( powerup_home == FALSE )) {
		if (st_ptr->stock_num < ((st_ptr->stock_size) / 10)) {
			return 1;
		}
	}
	else{
		if (st_ptr->stock_num < st_ptr->stock_size) {
			return 1;
		}
	}

	/* But there was no room at the inn... */
	return 0;
}


static bool is_blessed(object_type *o_ptr)
{
	u32b flgs[TR_FLAG_SIZE];
	object_flags(o_ptr, flgs);
	if (have_flag(flgs, TR_BLESSED)) return (TRUE);
	else return (FALSE);
}



/*
 * Determine if the current store will purchase the given item
 *
 * Note that a shop-keeper must refuse to buy "worthless" items
 */
static bool store_will_buy(object_type *o_ptr)
{
	/* Hack -- The Home is simple */
	if ((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM)) return (TRUE);

	/* Switch on the store */
	switch (cur_store_num)
	{
		/* General Store */
		case STORE_GENERAL:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_POTION:
					if (o_ptr->sval != SV_POTION_WATER) return FALSE;

				case TV_WHISTLE:
				case TV_FOOD:
				case TV_LITE:
				case TV_FLASK:
				case TV_SPIKE:
				case TV_SHOT:
				case TV_ARROW:
				case TV_BOLT:
				case TV_DIGGING:
				case TV_CLOAK:
				case TV_BOTTLE: /* 'Green', recycling Angband */
				case TV_FIGURINE:
				case TV_STATUE:
				case TV_CAPTURE:
				case TV_CARD:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Armoury */
		case STORE_ARMOURY:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_BOOTS:
				case TV_GLOVES:
				case TV_CROWN:
				case TV_HELM:
				case TV_SHIELD:
				case TV_CLOAK:
				case TV_SOFT_ARMOR:
				case TV_HARD_ARMOR:
				case TV_DRAG_ARMOR:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Weapon Shop */
		case STORE_WEAPON:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SHOT:
				case TV_BOLT:
				case TV_ARROW:
				case TV_BOW:
				case TV_DIGGING:
				case TV_POLEARM:
				case TV_SWORD:
				case TV_HISSATSU_BOOK:
				case TV_RAGE_BOOK:
				break;
				case TV_HAFTED:
				{
					if(o_ptr->sval == SV_WIZSTAFF) return (FALSE);
				}
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Temple */
		case STORE_TEMPLE:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_LIFE_BOOK:
				case TV_CRUSADE_BOOK:
				case TV_SCROLL:
				case TV_POTION:
				case TV_HAFTED:
				{
					break;
				}
				case TV_FIGURINE:
				case TV_STATUE:
				{
					monster_race *r_ptr = &r_info[o_ptr->pval];

					/* Decline evil */
					if (!(r_ptr->flags3 & RF3_EVIL))
					{
						/* Accept good */
						if (r_ptr->flags3 & RF3_GOOD) break;

						/* Accept animals */
						if (r_ptr->flags3 & RF3_ANIMAL) break;

						/* Accept mimics */
						if (my_strchr("?!", r_ptr->d_char)) break;
					}
				}
				case TV_POLEARM:
				case TV_SWORD:
				{
					if (is_blessed(o_ptr)) break;
				}
				default:
				return (FALSE);
			}
			break;
		}

		/* Alchemist */
		case STORE_ALCHEMIST:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SCROLL:
				case TV_POTION:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Magic Shop */
		case STORE_MAGIC:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SORCERY_BOOK:
				case TV_NATURE_BOOK:
				case TV_CHAOS_BOOK:
				case TV_ARMAGEDDON_BOOK:
				case TV_DEATH_BOOK:
				case TV_TRUMP_BOOK:
				case TV_ARCANE_BOOK:
				case TV_CRAFT_BOOK:
				case TV_DAEMON_BOOK:
				case TV_MUSIC_BOOK:
				case TV_HEX_BOOK:
				case TV_AMULET:
				case TV_RING:
				case TV_STAFF:
				case TV_WAND:
				case TV_ROD:
				case TV_SCROLL:
				case TV_POTION:
				case TV_FIGURINE:
				break;
				case TV_HAFTED:
				{
					if(o_ptr->sval == SV_WIZSTAFF) break;
					else return (FALSE);
				}
				default:
				return (FALSE);
			}
			break;
		}
		/* Bookstore Shop */
		case STORE_BOOK:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SORCERY_BOOK:
				case TV_NATURE_BOOK:
				case TV_CHAOS_BOOK:
				case TV_DEATH_BOOK:
				case TV_LIFE_BOOK:
				case TV_TRUMP_BOOK:
				case TV_ARCANE_BOOK:
				case TV_CRAFT_BOOK:
				case TV_DAEMON_BOOK:
				case TV_CRUSADE_BOOK:
				case TV_NECROMANCY_BOOK:
				case TV_ARMAGEDDON_BOOK:
				case TV_MUSIC_BOOK:
				case TV_HEX_BOOK:
					break;
				default:
					return (FALSE);
			}
			break;
		}
	}

	/* XXX XXX XXX Ignore "worthless" items */
	if (object_value(o_ptr) <= 0) return (FALSE);

	/* Assume okay */
	return (TRUE);
}


/*
 * Combine and reorder items in the home
 */
bool combine_and_reorder_home(int store_num)
{
	int         i, j, k;
	s32b        o_value;
	object_type forge, *o_ptr, *j_ptr;
	bool        flag = FALSE, combined;
	store_type  *old_st_ptr = st_ptr;
	bool        old_stack_force_notes = stack_force_notes;
	bool        old_stack_force_costs = stack_force_costs;

	st_ptr = &town[1].store[store_num];
	if (store_num != STORE_HOME)
	{
		stack_force_notes = FALSE;
		stack_force_costs = FALSE;
	}

	do
	{
		combined = FALSE;

		/* Combine the items in the home (backwards) */
		for (i = st_ptr->stock_num - 1; i > 0; i--)
		{
			/* Get the item */
			o_ptr = &st_ptr->stock[i];

			/* Skip empty items */
			if (!o_ptr->k_idx) continue;

			/* Scan the items above that item */
			for (j = 0; j < i; j++)
			{
				int max_num;

				/* Get the item */
				j_ptr = &st_ptr->stock[j];

				/* Skip empty items */
				if (!j_ptr->k_idx) continue;

				/*
				 * Get maximum number of the stack if these
				 * are similar, get zero otherwise.
				 */
				max_num = object_similar_part(j_ptr, o_ptr);

				/* Can we (partialy) drop "o_ptr" onto "j_ptr"? */
				if (max_num && j_ptr->number < max_num)
				{
					if (o_ptr->number + j_ptr->number <= max_num)
					{
						/* Add together the item counts */
						object_absorb(j_ptr, o_ptr);

						/* One object is gone */
						st_ptr->stock_num--;

						/* Slide everything down */
						for (k = i; k < st_ptr->stock_num; k++)
						{
							/* Structure copy */
							st_ptr->stock[k] = st_ptr->stock[k + 1];
						}

						/* Erase the "final" slot */
						object_wipe(&st_ptr->stock[k]);
					}
					else
					{
						int old_num = o_ptr->number;
						int remain = j_ptr->number + o_ptr->number - max_num;

						/* Add together the item counts */
						object_absorb(j_ptr, o_ptr);

						o_ptr->number = remain;

						/* Hack -- if rods are stacking, add the pvals (maximum timeouts) and current timeouts together. -LM- */
						if (o_ptr->tval == TV_ROD)
						{
							o_ptr->pval =  o_ptr->pval * remain / old_num;
							o_ptr->timeout = o_ptr->timeout * remain / old_num;
						}

						/* Hack -- if wands are stacking, combine the charges. -LM- */
						else if (o_ptr->tval == TV_WAND)
						{
							o_ptr->pval = o_ptr->pval * remain / old_num;
						}
					}

					/* Take note */
					combined = TRUE;

					/* Done */
					break;
				}
			}
		}

		flag |= combined;
	}
	while (combined);

	/* Re-order the items in the home (forwards) */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		/* Get the item */
		o_ptr = &st_ptr->stock[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < st_ptr->stock_num; j++)
		{
			if (object_sort_comp(o_ptr, o_value, &st_ptr->stock[j])) break;
		}

		/* Never move down */
		if (j >= i) continue;

		/* Take note */
		flag = TRUE;

		/* Get local object */
		j_ptr = &forge;

		/* Save a copy of the moving item */
		object_copy(j_ptr, &st_ptr->stock[i]);

		/* Slide the objects */
		for (k = i; k > j; k--)
		{
			/* Slide the item */
			object_copy(&st_ptr->stock[k], &st_ptr->stock[k - 1]);
		}

		/* Insert the moving item */
		object_copy(&st_ptr->stock[j], j_ptr);
	}

	st_ptr = old_st_ptr;
	if (store_num != STORE_HOME)
	{
		stack_force_notes = old_stack_force_notes;
		stack_force_costs = old_stack_force_costs;
	}

	return flag;
}


/*
 * Add the item "o_ptr" to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static int home_carry(object_type *o_ptr)
{
	int 				slot;
	s32b			   value;
	int 	i;
	object_type *j_ptr;
	bool old_stack_force_notes = stack_force_notes;
	bool old_stack_force_costs = stack_force_costs;

	if (cur_store_num != STORE_HOME)
	{
		stack_force_notes = FALSE;
		stack_force_costs = FALSE;
	}

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			if (cur_store_num != STORE_HOME)
			{
				stack_force_notes = old_stack_force_notes;
				stack_force_costs = old_stack_force_costs;
			}

			/* All done */
			return (slot);
		}
	}

	if (cur_store_num != STORE_HOME)
	{
		stack_force_notes = old_stack_force_notes;
		stack_force_costs = old_stack_force_costs;
	}

	/* No space? */
	/*
	 * 隠し機能: オプション powerup_home が設定されていると
	 *           我が家が 20 ページまで使える
	 */
	/* No space? */
	if ((cur_store_num != STORE_HOME) || (powerup_home == TRUE)) {
		if (st_ptr->stock_num >= st_ptr->stock_size) {
			return (-1);
		}
	}
	else{
		if (st_ptr->stock_num >= ((st_ptr->stock_size) / 10)) {
			return (-1);
		}
	}


	/* Determine the "value" of the item */
	value = object_value(o_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		if (object_sort_comp(o_ptr, value, &st_ptr->stock[slot])) break;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i-1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	if (cur_store_num == STORE_MUSEUM)
		chg_virtue(V_SACRIFICE, 1);

	(void)combine_and_reorder_home(cur_store_num);

	/* Return the location */
	return (slot);
}


/*
 * Add the item "o_ptr" to a real stores inventory.
 *
 * If the item is "worthless", it is thrown away (except in the home).
 *
 * If the item cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.	Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an item to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(object_type *o_ptr)
{
	int 	i, slot;
	s32b	value, j_value;
	object_type *j_ptr;


	/* Evaluate the object */
	value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	if (value <= 0) return (-1);

	/* All store items are fully *identified* */
	o_ptr->ident |= IDENT_MENTAL;

	/* Erase the inscription */
	o_ptr->inscription = 0;

	/* Erase the "feeling" */
	o_ptr->feeling = FEEL_NONE;

	/* Check each existing item (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing item */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Hack -- extra items disappear */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that item */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/*
		 * Hack:  otherwise identical rods sort by
		 * increasing recharge time --dsb
		 */
		if (o_ptr->tval == TV_ROD)
		{
			if (o_ptr->pval < j_ptr->pval) break;
			if (o_ptr->pval > j_ptr->pval) continue;
		}

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		st_ptr->stock[i] = st_ptr->stock[i-1];
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Insert the new item */
	st_ptr->stock[slot] = *o_ptr;

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.	This can result in zero items.
 */
static void store_item_increase(int item, int num)
{
	int 		cnt;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255) cnt = 255;
	else if (cnt < 0) cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int item)
{
	int 		j;
	object_type *o_ptr;

	/* Get the item */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx) return;

	/* Must have no items */
	if (o_ptr->number) return;

	/* One less item */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any item that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(object_type *o_ptr)
{
	int 	i, j;

	/* Ego items are never crap */
	if (object_is_ego(o_ptr)) return (FALSE);
	if (o_ptr->marked & OM_RESERVED) return FALSE;

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check all stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		if (i == STORE_HOME) continue;
		if (i == STORE_MUSEUM) continue;

		/* Check every item in the store */
		for (j = 0; j < town[p_ptr->town_num].store[i].stock_num; j++)
		{
			object_type *j_ptr = &town[p_ptr->town_num].store[i].stock[j];

			/* Duplicate item "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx) return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random item from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(void)
{
	int what, num;

	/* Pick a random slot */
	what = randint0(st_ptr->stock_num);
	if (st_ptr->stock[what].marked & OM_RESERVED) return;

	/* Determine how many items are here */
	num = st_ptr->stock[what].number;

	/* Hack -- sometimes, only destroy half the items */
	if (randint0(100) < 50) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single item */
	if (randint0(100) < 50) num = 1;

	/* Hack -- decrement the maximum timeouts and total charges of rods and wands. -LM- */
	if ((st_ptr->stock[what].tval == TV_ROD) || (st_ptr->stock[what].tval == TV_WAND))
	{
		st_ptr->stock[what].pval -= num * st_ptr->stock[what].pval / st_ptr->stock[what].number;
	}

	/* Actually destroy (part of) the item */
	store_item_increase(what, -num);
	store_item_optimize(what);
}


/*
 * Creates a random item and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 * Currently, "normal" stores use a pre-built array.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given item?
 */
static void store_create(void)
{
	int i, tries, level;

	object_type forge;
	object_type *q_ptr;


	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= st_ptr->stock_size) return;


	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		/* Black Market */
		if (cur_store_num == STORE_BLACK)
		{
			if (one_in_(6))
			{
				i = lookup_kind(TV_BURGLARY_BOOK, randint0(2));
				level = rand_range(1, STORE_OBJ_LEVEL);
			}
			else
			{
				/* Pick a level for object/magic */
				level = 25 + randint0(25);

				/* Random item (usually of given level) */
				i = get_obj_num(level);

				/* Handle failure */
				if (!i) continue;
			}
		}

		/* Normal Store */
		else
		{
			/* Hack -- Pick an item to sell */
			i = st_ptr->table[randint0(st_ptr->table_num)];

			/* Hack -- fake level for apply_magic() */
			level = rand_range(1, STORE_OBJ_LEVEL);
		}


		/* Get local object */
		q_ptr = &forge;

		/* Create a new object of the chosen kind */
		object_prep(q_ptr, i);

		/* Apply some "low-level" magic (no artifacts) */
		apply_magic(q_ptr, level, AM_NO_FIXED_ART);

		/* Require valid object */
		if (!store_will_buy(q_ptr)) continue;

		/* Hack -- Charge lite's */
		if (q_ptr->tval == TV_LITE)
		{
			if (q_ptr->sval == SV_LITE_TORCH) q_ptr->xtra4 = FUEL_TORCH / 2;
			if (q_ptr->sval == SV_LITE_LANTERN) q_ptr->xtra4 = FUEL_LAMP / 2;
		}


		/* The item is "known" */
		object_known(q_ptr);

		/* Mark it storebought */
		q_ptr->ident |= IDENT_STORE;

		/* Mega-Hack -- no chests in stores */
		if (q_ptr->tval == TV_CHEST) continue;

		/* Prune the black market */
		if (cur_store_num == STORE_BLACK)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(q_ptr)) continue;

			/* Hack -- No "cheap" items */
			if (object_value(q_ptr) < 10) continue;

			/* No "worthless" items */
			/* if (object_value(q_ptr) <= 0) continue; */
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(q_ptr) <= 0) continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(q_ptr);

		/* Attempt to carry the (known) item */
		(void)store_carry(q_ptr);

		/* Definitely done */
		break;
	}
}



/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(s32b minprice)
{
	s32b good = st_ptr->good_buy;
	s32b bad = st_ptr->bad_buy;

	/* Cheap items are "boring" */
	if (minprice < 10L) return (TRUE);

	/* Perfect haggling */
	if (good == MAX_SHORT) return (TRUE);

	/* Reward good haggles, punish bad haggles, notice price */
	if (good > ((3 * bad) + (5 + (minprice/50)))) return (TRUE);

	/* Return the flag */
	return (FALSE);
}


/*
 * Update the bargain info
 */
static void updatebargain(s32b price, s32b minprice, int num)
{
	/* Hack -- auto-haggle */
	if (!manual_haggle) return;

	/* Cheap items are "boring" */
	if ((minprice/num) < 10L) return;

	/* Count the successful haggles */
	if (price == minprice)
	{
		/* Just count the good haggles */
		if (st_ptr->good_buy < MAX_SHORT)
		{
			st_ptr->good_buy++;
		}
	}

	/* Count the failed haggles */
	else
	{
		/* Just count the bad haggles */
		if (st_ptr->bad_buy < MAX_SHORT)
		{
			st_ptr->bad_buy++;
		}
	}
}



/*
 * Re-displays a single store entry
 */
static void display_entry(int pos)
{
	int 		i, cur_col;
	object_type 	*o_ptr;
	s32b		x;

	char		o_name[MAX_NLEN];
	char		out_val[160];


	int maxwid = 75;

	/* Get the item */
	o_ptr = &st_ptr->stock[pos];

	/* Get the "offset" */
	i = (pos % store_bottom);

	/* Label it, clear the line --(-- */
	(void)sprintf(out_val, "%c) ", ((i > 25) ? toupper(I2A(i - 26)) : I2A(i)));
	prt(out_val, i+6, 0);

	cur_col = 3;
	if (show_item_graph)
	{
		byte a = object_attr(o_ptr);
		char c = object_char(o_ptr);

#ifdef AMIGA
		if (a & 0x80)
			a |= 0x40;
#endif

		Term_queue_bigchar(cur_col, i + 6, a, c, 0, 0);
		if (use_bigtile) cur_col++;

		cur_col += 2;
	}

	/* Describe an item in the home */
	if ((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM))
	{
		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, o_ptr, 0);
		o_name[maxwid] = '\0';
		c_put_str(tval_to_attr[o_ptr->tval], o_name, i+6, cur_col);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%1d kg", lbtokg1(wgt) , lbtokg2(wgt) );
			put_str(out_val, i+6, 67);
#else
			(void)sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, i+6, 68);
#endif

		}
		if (p_ptr->wizard)
		{
			int score = new_object_cost(o_ptr);
			(void)sprintf(out_val, "%9d ", score);
			put_str(out_val, i+6, 76);
		}
	}

	/* Describe an item (fully) in a store */
	else
	{
		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc(o_name, o_ptr, 0);
		o_name[maxwid] = '\0';
		c_put_str(tval_to_attr[o_ptr->tval], o_name, i+6, cur_col);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of an individual item */
			int wgt = o_ptr->weight;
#ifdef JP
			sprintf(out_val, "%3d.%1d", lbtokg1(wgt) , lbtokg2(wgt) );
			put_str(out_val, i+6, 60);
#else
			(void)sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, i+6, 61);
#endif

		}

		/* Display a "fixed" cost */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Actually draw the price (not fixed) */
#ifdef JP
(void)sprintf(out_val, "%9ld固", (long)x);
#else
			(void)sprintf(out_val, "%9ld F", (long)x);
#endif

			put_str(out_val, i+6, 68);
		}

		/* Display a "taxed" cost */
		else if (!manual_haggle)
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Hack -- Apply Sales Tax if needed */
			if (!noneedtobargain(x)) x += x / 10;

			/* Actually draw the price (with tax) */
			if (p_ptr->wizard)
			{
				 int score = new_object_cost(o_ptr);
				 (void)sprintf(out_val, "%9ld %9d ", (long)x, score);
			}
			else
				(void)sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, i+6, 68);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			(void)sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, i+6, 68);
		}
	}
}


/*
 * Displays a store's inventory 		-RAK-
 * All prices are listed as "per individual object".  -BEN-
 */
static void display_inventory(void)
{
	int i, k;

	/* Display the next 12 items */
	for (k = 0; k < store_bottom; k++)
	{
		/* Do not display "dead" items */
		if (store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < store_bottom + 1; i++) prt("", i + 6, 0);

	/* Assume "no current page" */
#ifdef JP
	put_str("          ", 5, 20);
#else
	put_str("        ", 5, 20);
#endif


	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > store_bottom)
	{
		/* Show "more" reminder (after the last item) */
#ifdef JP
		prt("-続く-", k + 6, 3);
#else
		prt("-more-", k + 6, 3);
#endif


		/* Indicate the "current page" */
		/* Trailing spaces are to display (Page xx) and (Page x) */
#ifdef JP
		put_str(format("(%dページ)  ", store_top/store_bottom + 1), 5, 20);
#else
		put_str(format("(Page %d)  ", store_top/store_bottom + 1), 5, 20);
#endif

	}

	if (cur_store_num == STORE_HOME || cur_store_num == STORE_MUSEUM)
	{
		k = st_ptr->stock_size;

		if (cur_store_num == STORE_HOME && !powerup_home) k /= 10;
#ifdef JP
		put_str(format("アイテム数:  %4d/%4d", st_ptr->stock_num, k), 19 + xtra_stock, 27);
#else
		put_str(format("Objects:  %4d/%4d", st_ptr->stock_num, k), 19 + xtra_stock, 30);
#endif
	}
}


/*
 * Displays players gold					-RAK-
 */
static void store_prt_gold(void)
{
	char out_val[64];

#ifdef JP
	prt("手持ちのお金: ", 19 + xtra_stock, 53);
#else
	prt("Gold Remaining: ", 19 + xtra_stock, 53);
#endif


	sprintf(out_val, "%9ld", (long)p_ptr->au);
	prt(out_val, 19 + xtra_stock, 68);
}


/*
 * Displays store (after clearing screen)		-RAK-
 */
static void display_store(void)
{
	char buf[80];


	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (cur_store_num == STORE_HOME)
	{
		/* Put the owner name */
#ifdef JP
		put_str("我が家", 3, 31);
#else
		put_str("Your Home", 3, 30);
#endif


		/* Label the item descriptions */
#ifdef JP
		put_str("アイテムの一覧", 5, 4);
#else
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("重さ", 5, 72);
#else
			put_str("Weight", 5, 70);
#endif

		}

		if (p_ptr->wizard)
			put_str("Score", 5, 80);
	}

	/* The "Home" is special */
	else if (cur_store_num == STORE_MUSEUM)
	{
		/* Put the owner name */
#ifdef JP
		put_str("博物館", 3, 31);
#else
		put_str("Museum", 3, 30);
#endif


		/* Label the item descriptions */
#ifdef JP
		put_str("アイテムの一覧", 5, 4);
#else
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("重さ", 5, 72);
#else
			put_str("Weight", 5, 70);
#endif

		}
		if (p_ptr->wizard)
			put_str("Score", 5, 80);
	}

	/* Normal stores */
	else
	{
		cptr store_name = (f_name + f_info[cur_store_feat].name);
		cptr owner_name = (ot_ptr->owner_name);
		cptr race_name = get_race_t_aux(ot_ptr->owner_race, 0)->name;

		/* Put the owner name and race */
		sprintf(buf, "%s (%s)", owner_name, race_name);
		put_str(buf, 3, 10);

		/* Show the max price in the store (above prices) */
		sprintf(buf, "%s (%ld)", store_name, (long)(ot_ptr->max_cost));
		prt(buf, 3, 50);

		/* Label the item descriptions */
#ifdef JP
		put_str("商品の一覧", 5, 7);
#else
		put_str("Item Description", 5, 3);
#endif


		/* If showing weights, show label */
		if (show_weights)
		{
#ifdef JP
			put_str("重さ", 5, 62);
#else
			put_str("Weight", 5, 60);
#endif

		}

		/* Label the asking price (in stores) */
#ifdef JP
		put_str("価格", 5, 73);
#else
		put_str("Price", 5, 72);
#endif

		if (p_ptr->wizard)
			put_str("Score", 5, 82);

	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();
}



/*
 * Get the ID of a store item and return its value	-RAK-
 */
static int get_stock(int *com_val, cptr pmt, int i, int j)
{
	char	command;
	char	out_val[160];
	char	lo, hi;

#ifdef ALLOW_REPEAT /* TNB */

	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= i) && (*com_val <= j))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT -- TNB */

	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	lo = I2A(i);
	hi = (j > 25) ? toupper(I2A(j - 26)) : I2A(j);
#ifdef JP
	(void)sprintf(out_val, "(%s:%c-%c, ESCで中断) %s",
		(((cur_store_num == STORE_HOME) || (cur_store_num == STORE_MUSEUM)) ? "アイテム" : "商品"), 
				  lo, hi, pmt);
#else
	(void)sprintf(out_val, "(Items %c-%c, ESC to exit) %s",
				  lo, hi, pmt);
#endif


	/* Ask until done */
	while (TRUE)
	{
		int k;

		/* Escape */
		if (!get_com(out_val, &command, FALSE)) break;

		/* Convert */
		if (islower(command))
			k = A2I(command);
		else if (isupper(command))
			k = A2I(tolower(command)) + 26;
		else
			k = -1;

		/* Legal responses */
		if ((k >= i) && (k <= j))
		{
			*com_val = k;
			break;
		}

		/* Oops */
		bell();
	}

	/* Clear the prompt */
	prt("", 0, 0);

	/* Cancel */
	if (command == ESCAPE) return (FALSE);

#ifdef ALLOW_REPEAT /* TNB */

	repeat_push(*com_val);

#endif /* ALLOW_REPEAT -- TNB */

	/* Success */
	return (TRUE);
}


/*
 * Increase the insult counter and get angry if too many -RAK-
 */
static int increase_insults(void)
{
	/* Increase insults */
	st_ptr->insult_cur++;

	/* Become insulted */
	if (st_ptr->insult_cur > ot_ptr->insult_max)
	{
		/* Complain */
		say_comment_4();

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Open tomorrow */
		st_ptr->store_open = turn + TURNS_PER_TICK*TOWN_DAWN/8 + randint1(TURNS_PER_TICK*TOWN_DAWN/8);

		/* Closed */
		return (TRUE);
	}

	/* Not closed */
	return (FALSE);
}


/*
 * Decrease insults 				-RAK-
 */
static void decrease_insults(void)
{
	/* Decrease insults */
	if (st_ptr->insult_cur) st_ptr->insult_cur--;
}


/*
 * Have insulted while haggling 			-RAK-
 */
static int haggle_insults(void)
{
	/* Increase insults */
	if (increase_insults()) return (TRUE);

	/* Display and flush insult */
	say_comment_5();

	/* Still okay */
	return (FALSE);
}


/*
 * Mega-Hack -- Enable "increments"
 */
static bool allow_inc = FALSE;

/*
 * Mega-Hack -- Last "increment" during haggling
 */
static s32b last_inc = 0L;


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer, s32b price, int final)
{
	s32b		i;

	cptr		p;

	char				buf[128];
	char		out_val[160];


	/* Clear old increment if necessary */
	if (!allow_inc) last_inc = 0L;


	/* Final offer */
	if (final)
	{
#ifdef JP
		sprintf(buf, "%s [承諾] ", pmt);
#else
		sprintf(buf, "%s [accept] ", pmt);
#endif

	}

	/* Old (negative) increment, and not final */
	else if (last_inc < 0)
	{
#ifdef JP
		sprintf(buf, "%s [-$%ld] ", pmt, (long)(ABS(last_inc)));
#else
		sprintf(buf, "%s [-%ld] ", pmt, (long)(ABS(last_inc)));
#endif

	}

	/* Old (positive) increment, and not final */
	else if (last_inc > 0)
	{
#ifdef JP
		sprintf(buf, "%s [+$%ld] ", pmt, (long)(ABS(last_inc)));
#else
		sprintf(buf, "%s [+%ld] ", pmt, (long)(ABS(last_inc)));
#endif

	}

	/* Normal haggle */
	else
	{
		sprintf(buf, "%s ", pmt);
	}


	/* Paranoia XXX XXX XXX */
	msg_print(NULL);


	/* Ask until done */
	while (TRUE)
	{
		bool res;

		/* Display prompt */
		prt(buf, 0, 0);

		/* Default */
		strcpy(out_val, "");

		/*
		 * Ask the user for a response.
		 * Don't allow to use numpad as cursor key.
		 */
		res = askfor_aux(out_val, 32, FALSE);

		/* Clear prompt */
		prt("", 0, 0);

		/* Cancelled */
		if (!res) return FALSE;

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */;

		/* Empty response */
		if (*p == '\0')
		{
			/* Accept current price */
			if (final)
			{
				*poffer = price;
				last_inc = 0L;
				break;
			}

			/* Use previous increment */
			if (allow_inc && last_inc)
			{
				*poffer += last_inc;
				break;
			}
		}

		/* Normal response */
		else
		{
			/* Extract a number */
			i = atol(p);

			/* Handle "incremental" number */
			if ((*p == '+' || *p == '-'))
			{
				/* Allow increments */
				if (allow_inc)
				{
					/* Use the given "increment" */
					*poffer += i;
					last_inc = i;
					break;
				}
			}

			/* Handle normal number */
			else
			{
				/* Use the given "number" */
				*poffer = i;
				last_inc = 0L;
				break;
			}
		}

		/* Warning */
#ifdef JP
		msg_print("値がおかしいです。");
#else
		msg_print("Invalid response.");
#endif

		msg_print(NULL);
	}

	/* Success */
	return (TRUE);
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(cptr pmt, s32b *poffer,
			  s32b last_offer, int factor,
			  s32b price, int final)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if (!get_haggle(pmt, poffer, price, final)) return (TRUE);

		/* Acceptable offer */
		if (((*poffer) * factor) >= (last_offer * factor)) break;

		/* Insult, and check for kicked out */
		if (haggle_insults()) return (TRUE);

		/* Reject offer (correctly) */
		(*poffer) = last_offer;
	}

	/* Success */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool purchase_haggle(object_type *o_ptr, s32b *price)
{
	s32b			   cur_ask, final_ask;
	s32b			   last_offer, offer;
	s32b			   x1, x2, x3;
	s32b			   min_per, max_per;
	int 			   flag, loop_flag, noneed;
	int 			   annoyed = 0, final = FALSE;

	bool		cancel = FALSE;

#ifdef JP
	cptr pmt = "提示価格";
#else
	cptr		pmt = "Asking";
#endif


	char		out_val[160];


	*price = 0;


	/* Extract the starting offer and the final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, FALSE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* No need to haggle */
	if (noneed || !manual_haggle)
	{
		/* No need to haggle */
		if (noneed)
		{
			/* Message summary */
#ifdef JP
			msg_print("結局この金額にまとまった。");
#else
			msg_print("You eventually agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
#ifdef JP
			msg_print("すんなりとこの金額にまとまった。");
#else
			msg_print("You quickly agree upon the price.");
#endif

			msg_print(NULL);

			/* Apply Sales Tax */
			final_ask += final_ask / 10;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Go to final offer */
#ifdef JP
		pmt = "最終提示価格";
#else
		pmt = "Final Offer";
#endif

		final = TRUE;
	}


	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* Haggle parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * (200 - (int)(ot_ptr->max_inflate)) / 100L;
	if (last_offer <= 0) last_offer = 1;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle until done */
	for (flag = FALSE; !flag; )
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
			(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer("提示する金額? ",
#else
			cancel = receive_offer("What do you offer? ",
#endif

					       &offer, last_offer, 1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer > cur_ask)
			{
				say_comment_6();
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if (!flag)
		{
			x1 = 100 * (offer - last_offer) / (cur_ask - last_offer);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((cur_ask - offer) * x2 / 100L) + 1;
			/* don't let the price go up */
			if (x3 < 0) x3 = 0;
			cur_ask -= x3;

			/* Too little */
			if (cur_ask < final_ask)
			{
				final = TRUE;
				cur_ask = final_ask;
#ifdef JP
				pmt = "最終提示価格";
#else
				pmt = "Final Offer";
#endif

				annoyed++;
				if (annoyed > 3)
				{
					(void)(increase_insults());
					cancel = TRUE;
					flag = TRUE;
				}
			}
			else if (offer >= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
#ifdef JP
(void)sprintf(out_val, "前回の提示金額: $%ld",
#else
				(void)sprintf(out_val, "Your last offer: %ld",
#endif

							  (long)last_offer);
				put_str(out_val, 1, 39);
				say_comment_2(cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if (cancel) return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask, o_ptr->number);

	/* Do not cancel */
	return (FALSE);
}


/*
 * Haggling routine 				-RAK-
 *
 * Return TRUE if purchase is NOT successful
 */
static bool sell_haggle(object_type *o_ptr, s32b *price)
{
	s32b    purse, cur_ask, final_ask;
	s32b    last_offer = 0, offer = 0;
	s32b    x1, x2, x3;
	s32b    min_per, max_per;
	int     flag, loop_flag, noneed;
	int     annoyed = 0, final = FALSE;
	bool    cancel = FALSE;
#ifdef JP
	cptr pmt = "提示金額";
#else
	cptr    pmt = "Offer";
#endif

	char    out_val[160];


	*price = 0;

	if (prace_is_(RACE_DEMIGOD) && p_ptr->psubrace == DEMIGOD_APHRODITE)
	{
		cur_ask = 1;
		final_ask = price_item(o_ptr, 100, TRUE);
		noneed = TRUE;
	}
	else
	{
		/* Obtain the starting offer and the final offer */
		cur_ask = price_item(o_ptr, ot_ptr->max_inflate, TRUE);
		final_ask = price_item(o_ptr, ot_ptr->min_inflate, TRUE);

		/* Determine if haggling is necessary */
		noneed = noneedtobargain(final_ask);
	}

	/* Get the owner's payout limit */
	purse = (s32b)(ot_ptr->max_cost);

	/* No need to haggle */
	if (noneed || !manual_haggle || (final_ask >= purse))
	{
		/* Apply Sales Tax (if needed) */
		if (!manual_haggle && !noneed)
		{
			final_ask -= final_ask / 10;
		}

		/* No reason to haggle */
		if (final_ask >= purse)
		{
			/* Message */
#ifdef JP
			msg_print("即座にこの金額にまとまった。");
#else
			msg_print("You instantly agree upon the price.");
#endif

			msg_print(NULL);

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message */
#ifdef JP
			msg_print("結局この金額にまとまった。");
#else
			msg_print("You eventually agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* No haggle option */
		else
		{
			/* Message summary */
#ifdef JP
			msg_print("すんなりとこの金額にまとまった。");
#else
			msg_print("You quickly agree upon the price.");
#endif

			msg_print(NULL);
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		final = TRUE;
#ifdef JP
		pmt = "最終提示金額";
#else
		pmt = "Final Offer";
#endif

	}

	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* XXX XXX XXX Display commands */

	/* Haggling parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * ot_ptr->max_inflate / 100L;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle */
	for (flag = FALSE; !flag; )
	{
		while (1)
		{
			loop_flag = TRUE;

			(void)sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
#ifdef JP
			cancel = receive_offer("提示する価格? ",
#else
			cancel = receive_offer("What price do you ask? ",
#endif

								   &offer, last_offer, -1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer < cur_ask)
			{
				say_comment_6();
				/* rejected, reset offer for incremental haggling */
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if (flag || !loop_flag) break;
		}

		if (!flag)
		{
			x1 = 100 * (last_offer - offer) / (last_offer - cur_ask);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((offer - cur_ask) * x2 / 100L) + 1;
			/* don't let the price go down */
			if (x3 < 0) x3 = 0;
			cur_ask += x3;

			if (cur_ask > final_ask)
			{
				cur_ask = final_ask;
				final = TRUE;
#ifdef JP
				pmt = "最終提示金額";
#else
				pmt = "Final Offer";
#endif

				annoyed++;
				if (annoyed > 3)
				{
					flag = TRUE;
#ifdef JP
				/* 追加 $0 で買い取られてしまうのを防止 By FIRST*/
					cancel = TRUE;
#endif
					(void)(increase_insults());
				}
			}
			else if (offer <= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				(void)sprintf(out_val,
#ifdef JP
					      "前回の提示価格 $%ld", (long)last_offer);
#else
							  "Your last bid %ld", (long)last_offer);
#endif

				put_str(out_val, 1, 39);
				say_comment_3(cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if (cancel) return (TRUE);

	/* Update bargaining info */
	updatebargain(*price, final_ask, o_ptr->number);

	/* Do not cancel */
	return (FALSE);
}

/*
 * Buy an item from a store 			-RAK-
 */
static void store_purchase(void)
{
	int i, amt, choice;
	int item, item_new;

	s32b price, best;

	object_type forge;
	object_type *j_ptr;

	object_type *o_ptr;

	char o_name[MAX_NLEN];

	char out_val[160];

	if (cur_store_num == STORE_MUSEUM)
	{
#ifdef JP
		msg_print("博物館から取り出すことはできません。");
#else
		msg_print("Museum.");
#endif
		return;
	}

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == STORE_HOME)
#ifdef JP
			msg_print("我が家には何も置いてありません。");
#else
			msg_print("Your home is empty.");
#endif

		else
#ifdef JP
			msg_print("現在商品の在庫を切らしています。");
#else
			msg_print("I am currently out of stock.");
#endif

		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > store_bottom) i = store_bottom;

	/* Prompt */
#ifdef JP
	/* ブラックマーケットの時は別のメッセージ */
	switch( cur_store_num ) {
		case 7:
			sprintf(out_val, "どのアイテムを取りますか? ");
			break;
		case 6:
			sprintf(out_val, "どれ? ");
			break;
		default:
			sprintf(out_val, "どの品物が欲しいんだい? ");
			break;
	}
#else
	if (cur_store_num == STORE_HOME)
	{
		sprintf(out_val, "Which item do you want to take? ");
	}
	else
	{
		sprintf(out_val, "Which item are you interested in? ");
	}
#endif


	/* Get the item number to be bought */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Assume the player wants just one of them */
	amt = 1;

	/* Get local object */
	j_ptr = &forge;

	/* Get a copy of the object */
	object_copy(j_ptr, o_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, o_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
#ifdef JP
msg_print("そんなにアイテムを持てない。");
#else
		msg_print("You cannot carry that many different items.");
#endif

		return;
	}

	/* Determine the "best" price (per item) */
	best = price_item(j_ptr, ot_ptr->min_inflate, FALSE);

	/* Find out how many the player wants */
	if (o_ptr->number > 1)
	{
		/* Hack -- note cost of "fixed" items */
		if ((cur_store_num != STORE_HOME) &&
		    (o_ptr->ident & IDENT_FIXED))
		{
#ifdef JP
msg_format("一つにつき $%ldです。", (long)(best));
#else
			msg_format("That costs %ld gold per item.", (long)(best));
#endif

		}

		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	j_ptr = &forge;

	/* Get desired object */
	object_copy(j_ptr, o_ptr);

	/*
	 * If a rod or wand, allocate total maximum timeouts or charges
	 * between those purchased and left on the shelf.
	 */
	reduce_charges(j_ptr, o_ptr->number - amt);

	/* Modify quantity */
	j_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(j_ptr))
	{
#ifdef JP
		msg_print("ザックにそのアイテムを入れる隙間がない。");
#else
		msg_print("You cannot carry that many items.");
#endif

		return;
	}

	/* Attempt to buy it */
	if (cur_store_num != STORE_HOME)
	{
		/* Fixed price, quick buy */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Assume accept */
			choice = 0;

			/* Go directly to the "best" deal */
			price = (best * j_ptr->number);
		}

		/* Haggle for it */
		else
		{
			/* Describe the object (fully) */
			object_desc(o_name, j_ptr, 0);

			/* Message */
#ifdef JP
msg_format("%s(%c)を購入する。", o_name, I2A(item));
#else
			msg_format("Buying %s (%c).", o_name, I2A(item));
#endif

			msg_print(NULL);

			/* Haggle for a final price */
			choice = purchase_haggle(j_ptr, &price);

			/* Hack -- Got kicked out */
			if (st_ptr->store_open >= turn) return;
		}

		/* Player wants it */
		if (choice == 0)
		{
			/* Fix the item price (if "correctly" haggled) */
			if (price == (best * j_ptr->number)) o_ptr->ident |= (IDENT_FIXED);

			/* Player can afford it */
			if (p_ptr->au >= price)
			{
				/* Say "okay" */
				say_comment_1();

				if (cur_store_num == STORE_BLACK) /* The black market is illegal! */
					chg_virtue(V_JUSTICE, -1);
				if((o_ptr->tval == TV_BOTTLE) && (cur_store_num != STORE_HOME))
					chg_virtue(V_NATURE, -1);

				/* Make a sound */
				sound(SOUND_BUY);

				/* Be happy */
				decrease_insults();

				/* Spend the money */
				p_ptr->au -= price;

				/* Update the display */
				store_prt_gold();

				/* Hack -- buying an item makes you aware of it */
				object_aware(j_ptr);

				/* Hack -- clear the "fixed" flag from the item */
				j_ptr->ident &= ~(IDENT_FIXED);

				/* Describe the transaction */
				object_desc(o_name, j_ptr, 0);

				/* Message */
#ifdef JP
msg_format("%sを $%ldで購入しました。", o_name, (long)price);
#else
				msg_format("You bought %s for %ld gold.", o_name, (long)price);
#endif

				strcpy(record_o_name, o_name);
				record_turn = turn;

				if (record_buy) do_cmd_write_nikki(NIKKI_BUY, 0, o_name);
				object_desc(o_name, o_ptr, OD_NAME_ONLY);
				if(record_rand_art && o_ptr->art_name)
					do_cmd_write_nikki(NIKKI_ART, 0, o_name);

				/* Erase the inscription */
				j_ptr->inscription = 0;

				/* Erase the "feeling" */
				j_ptr->feeling = FEEL_NONE;
				j_ptr->ident &= ~(IDENT_STORE);
				j_ptr->marked &= ~(OM_RESERVED);

				/* Give it to the player */
				item_new = inven_carry(j_ptr);

				/* Describe the final result */
				object_desc(o_name, &inventory[item_new], 0);

				/* Message */
#ifdef JP
		msg_format("%s(%c)を手に入れた。", o_name, index_to_label(item_new));
#else
				msg_format("You have %s (%c).",
						   o_name, index_to_label(item_new));
#endif

				/* Auto-inscription */
				autopick_alter_item(item_new, FALSE);

				/* Now, reduce the original stack's pval. */
				if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
				{
					o_ptr->pval -= j_ptr->pval;
				}

				/* Handle stuff */
				handle_stuff();

				/* Note how many slots the store used to have */
				i = st_ptr->stock_num;

				/* Remove the bought items from the store */
				store_item_increase(item, -amt);
				store_item_optimize(item);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
					/* Shuffle */
					if (one_in_(STORE_SHUFFLE))
					{
						char buf[80];
						/* Message */
#ifdef JP
						msg_print("店主は引退した。");
#else
						msg_print("The shopkeeper retires.");
#endif


						/* Shuffle the store */
						store_shuffle(cur_store_num);

						prt("",3,0);
						sprintf(buf, "%s (%s)",
							ot_ptr->owner_name, get_race_t_aux(ot_ptr->owner_race, 0)->name);
						put_str(buf, 3, 10);
						sprintf(buf, "%s (%ld)",
							(f_name + f_info[cur_store_feat].name), (long)(ot_ptr->max_cost));
						prt(buf, 3, 50);
					}

					/* Maintain */
					else
					{
						/* Message */
#ifdef JP
						msg_print("店主は新たな在庫を取り出した。");
#else
						msg_print("The shopkeeper brings out some new stock.");
#endif

					}

					/* New inventory */
					for (i = 0; i < 10; i++)
					{
						/* Maintain the store */
						store_maint(p_ptr->town_num, cur_store_num);
					}

					/* Start over */
					store_top = 0;

					/* Redraw everything */
					display_inventory();
				}

				/* The item is gone */
				else if (st_ptr->stock_num != i)
				{
					/* Pick the correct screen */
					if (store_top >= st_ptr->stock_num) store_top -= store_bottom;

					/* Redraw everything */
					display_inventory();
				}

				/* Item is still here */
				else
				{
					/* Redraw the item */
					display_entry(item);
				}
			}

			/* Player cannot afford it */
			else
			{
				/* Simple message (no insult) */
#ifdef JP
				msg_print("お金が足りません。");
#else
				msg_print("You do not have enough gold.");
#endif

			}
		}
	}

	/* Home is much easier */
	else
	{
		bool combined_or_reordered;

		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, j_ptr, amt);

		/* Give it to the player */
		item_new = inven_carry(j_ptr);

		/* Describe just the result */
		object_desc(o_name, &inventory[item_new], 0);

		/* Message */
#ifdef JP
				msg_format("%s(%c)を取った。",
#else
		msg_format("You have %s (%c).",
#endif
 o_name, index_to_label(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		i = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		combined_or_reordered = combine_and_reorder_home(STORE_HOME);

		/* Hack -- Item is still here */
		if (i == st_ptr->stock_num)
		{
			/* Redraw everything */
			if (combined_or_reordered) display_inventory();

			/* Redraw the item */
			else display_entry(item);
		}

		/* The item is gone */
		else
		{
			/* Nothing left */
			if (st_ptr->stock_num == 0) store_top = 0;

			/* Nothing left on that screen */
			else if (store_top >= st_ptr->stock_num) store_top -= store_bottom;

			/* Redraw everything */
			display_inventory();

			/* ??? chg_virtue(V_SACRIFICE, 1); */
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an item to the store (or home)
 */
static void store_sell(void)
{
	int choice;
	int item, item_pos;
	int amt;

	s32b price, value, dummy;

	object_type forge;
	object_type *q_ptr;

	object_type *o_ptr;

	cptr q, s;

	char o_name[MAX_NLEN];


	/* Prepare a prompt */
	if (cur_store_num == STORE_HOME)
#ifdef JP
	q = "どのアイテムを置きますか? ";
#else
		q = "Drop which item? ";
#endif

	else if (cur_store_num == STORE_MUSEUM)
#ifdef JP
	q = "どのアイテムを寄贈しますか? ";
#else
		q = "Give which item? ";
#endif

	else if (no_selling)
	{
		msg_print("Selling is prohibited!");
		return;
	}
	else
#ifdef JP
		q = "どのアイテムを売りますか? ";
#else
		q = "Sell which item? ";
#endif


	item_tester_no_ryoute = TRUE;
	/* Only allow items the store will buy */
	item_tester_hook = store_will_buy;

	/* Get an item */
	/* 我が家でおかしなメッセージが出るオリジナルのバグを修正 */
	if (cur_store_num == STORE_HOME)
	{
#ifdef JP
		s = "置けるアイテムを持っていません。";
#else
		s = "You don't have any item to drop.";
#endif
	}
	else if (cur_store_num == STORE_MUSEUM)
	{
#ifdef JP
		s = "寄贈できるアイテムを持っていません。";
#else
		s = "You don't have any item to give.";
#endif
	}
	else
	{
#ifdef JP
		s = "欲しい物がないですねえ。";
#else
		s = "You have nothing that I want.";
#endif
	}

	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Hack -- Cannot remove cursed items */
	if ((item >= INVEN_RARM) && object_is_cursed(o_ptr))
	{
		/* Oops */
#ifdef JP
		msg_print("ふーむ、どうやらそれは呪われているようだね。");
#else
		msg_print("Hmmm, it seems to be cursed.");
#endif


		/* Nope */
		return;
	}

	if (o_ptr->tval == TV_POTION && o_ptr->sval == SV_POTION_BLOOD)
	{
		msg_print("You can't do that!  Your blood will go sour!");
		return;
	}

	/* Assume one item */
	amt = 1;

	/* Find out how many the player wants (letter means "all") */
	if (o_ptr->number > 1)
	{
		/* Get a quantity */
		amt = get_quantity(NULL, o_ptr->number);

		/* Allow user abort */
		if (amt <= 0) return;
	}

	/* Get local object */
	q_ptr = &forge;

	/* Get a copy of the object */
	object_copy(q_ptr, o_ptr);

	/* Modify quantity */
	q_ptr->number = amt;

	/*
	 * Hack -- If a rod or wand, allocate total maximum
	 * timeouts or charges to those being sold. -LM-
	 */
	if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
	{
		q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, q_ptr, 0);

	/* Remove any inscription, feeling for stores */
	if ((cur_store_num != STORE_HOME) && (cur_store_num != STORE_MUSEUM))
	{
		q_ptr->inscription = 0;
		q_ptr->feeling = FEEL_NONE;
	}

	/* Is there room in the store (or the home?) */
	if (!store_check_num(q_ptr))
	{
		if (cur_store_num == STORE_HOME)
#ifdef JP
			msg_print("我が家にはもう置く場所がない。");
#else
			msg_print("Your home is full.");
#endif

		else if (cur_store_num == STORE_MUSEUM)
#ifdef JP
			msg_print("博物館はもう満杯だ。");
#else
			msg_print("Museum is full.");
#endif

		else
#ifdef JP
			msg_print("すいませんが、店にはもう置く場所がありません。");
#else
			msg_print("I have not the room in my store to keep it.");
#endif

		return;
	}


	/* Real store */
	if ((cur_store_num != STORE_HOME) && (cur_store_num != STORE_MUSEUM))
	{
		/* Describe the transaction */
#ifdef JP
		msg_format("%s(%c)を売却する。", o_name, index_to_label(item));
#else
		msg_format("Selling %s (%c).", o_name, index_to_label(item));
#endif

		msg_print(NULL);

		/* Haggle for it */
		choice = sell_haggle(q_ptr, &price);

		/* Kicked out */
		if (st_ptr->store_open >= turn) return;

		/* Sold... */
		if (choice == 0)
		{
			/* Say "okay" */
			say_comment_1();

			/* Make a sound */
			sound(SOUND_SELL);

			/* Be happy */
			if (cur_store_num == STORE_BLACK) /* The black market is illegal! */
				chg_virtue(V_JUSTICE, -1);

			if((o_ptr->tval == TV_BOTTLE) && (cur_store_num != STORE_HOME))
				chg_virtue(V_NATURE, 1);
			decrease_insults();

			/* Get some money */
			p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(q_ptr) * q_ptr->number;

			/* Identify it */
			identify_item(o_ptr);

			/* Get local object */
			q_ptr = &forge;

			/* Get a copy of the object */
			object_copy(q_ptr, o_ptr);

			/* Modify quantity */
			q_ptr->number = amt;

			/* Make it look like to be known */
			q_ptr->ident |= IDENT_STORE;

			/*
			 * Hack -- If a rod or wand, let the shopkeeper know just
			 * how many charges he really paid for. -LM-
			 */
			if ((o_ptr->tval == TV_ROD) || (o_ptr->tval == TV_WAND))
			{
				q_ptr->pval = o_ptr->pval * amt / o_ptr->number;
			}

			/* Get the "actual" value */
			value = object_value(q_ptr) * q_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, q_ptr, 0);

			/* Describe the result (in message buffer) */
#ifdef JP
msg_format("%sを $%ldで売却しました。", o_name, (long)price);
#else
			msg_format("You sold %s for %ld gold.", o_name, (long)price);
#endif

			if (record_sell) do_cmd_write_nikki(NIKKI_SELL, 0, o_name);

			if (!((o_ptr->tval == TV_FIGURINE) && (value > 0)))
			{
			 /* Analyze the prices (and comment verbally) unless a figurine*/
			purchase_analyze(price, value, dummy);
			}

			/*
			 * Hack -- Allocate charges between those wands or rods sold
			 * and retained, unless all are being sold. -LM-
			 */
			distribute_charges(o_ptr, q_ptr, amt);

			/* Reset timeouts of the sold items */
			q_ptr->timeout = 0;

			/* Take the item from the player, describe the result */
			inven_item_increase(item, -amt);
			inven_item_describe(item);

			/* If items remain, auto-inscribe before optimizing */
			if (o_ptr->number > 0)
				autopick_alter_item(item, FALSE);

			inven_item_optimize(item);

			/* Handle stuff */
			handle_stuff();

			/* The store gets that (known) item */
			item_pos = store_carry(q_ptr);

			/* Re-display if item is now in store */
			if (item_pos >= 0)
			{
				store_top = (item_pos / store_bottom) * store_bottom;
				display_inventory();
			}
		}
	}

	/* Player is at museum */
	else if (cur_store_num == STORE_MUSEUM)
	{
		char o2_name[MAX_NLEN];
		object_desc(o2_name, q_ptr, OD_NAME_ONLY);

		if (-1 == store_check_num(q_ptr))
		{
#ifdef JP
			msg_print("それと同じ品物は既に博物館にあるようです。");
#else
			msg_print("The same object as it is already in the Museum.");
#endif
		}
		else
		{
#ifdef JP
			msg_print("博物館に寄贈したものは取り出すことができません！！");
#else
			msg_print("You cannot take items which is given to the Museum back!!");
#endif
		}
#ifdef JP
		if (!get_check(format("本当に%sを寄贈しますか？", o2_name))) return;
#else
		if (!get_check(format("Really give %s to the Museum? ", o2_name))) return;
#endif

		/* Identify it */
		identify_item(q_ptr);
		q_ptr->ident |= IDENT_MENTAL;

		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, q_ptr, amt);

		/* Describe */
#ifdef JP
		msg_format("%sを置いた。(%c)", o_name, index_to_label(item));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif

		choice = 0;

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / store_bottom) * store_bottom;
			display_inventory();
		}
	}
	/* Player is at home */
	else
	{
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, q_ptr, amt);

		/* Describe */
#ifdef JP
		msg_format("%sを置いた。(%c)", o_name, index_to_label(item));
#else
		msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif

		choice = 0;

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(q_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			store_top = (item_pos / store_bottom) * store_bottom;
			display_inventory();
		}
	}

	if ((choice == 0) && (item >= INVEN_RARM))
	{
		calc_android_exp();
		kamaenaoshi(item);
	}
}


/*
 * Examine an item in a store			   -JDL-
 */
static void store_examine(void)
{
	int         i;
	int         item;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	char        out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (cur_store_num == STORE_HOME)
#ifdef JP
			msg_print("我が家には何も置いてありません。");
#else
			msg_print("Your home is empty.");
#endif

		else if (cur_store_num == STORE_MUSEUM)
#ifdef JP
			msg_print("博物館には何も置いてありません。");
#else
			msg_print("Museum is empty.");
#endif

		else
#ifdef JP
			msg_print("現在商品の在庫を切らしています。");
#else
			msg_print("I am currently out of stock.");
#endif

		return;
	}


	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > store_bottom) i = store_bottom;

	/* Prompt */
#ifdef JP
sprintf(out_val, "どれを調べますか？");
#else
	sprintf(out_val, "Which item do you want to examine? ");
#endif


	/* Get the item number to be examined */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Require full knowledge */
	if (!(o_ptr->ident & IDENT_MENTAL))
	{
		/* This can only happen in the home */
#ifdef JP
msg_print("このアイテムについて特に知っていることはない。");
#else
		msg_print("You have no special knowledge about that item.");
#endif

		return;
	}

	/* Description */
	object_desc(o_name, o_ptr, 0);

	/* Describe */
#ifdef JP
msg_format("%sを調べている...", o_name);
#else
	msg_format("Examining %s...", o_name);
#endif


	/* Describe it fully */
	if (!screen_object(o_ptr, SCROBJ_FORCE_DETAIL))
#ifdef JP
msg_print("特に変わったところはないようだ。");
#else
		msg_print("You see nothing special.");
#endif


	return;
}


/*
 * Remove an item from museum (Originally from TOband)
 */
static void museum_remove_object(void)
{
	int         i;
	int         item;
	object_type *o_ptr;
	char        o_name[MAX_NLEN];
	char        out_val[160];

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
#ifdef JP
		msg_print("博物館には何も置いてありません。");
#else
		msg_print("Museum is empty.");
#endif

		return;
	}

	/* Find the number of objects on this and following pages */
	i = st_ptr->stock_num - store_top;

	/* And then restrict it to the current page */
	if (i > store_bottom) i = store_bottom;

	/* Prompt */
#ifdef JP
	sprintf(out_val, "どのアイテムの展示をやめさせますか？");
#else
	sprintf(out_val, "Which item do you want to order to remove? ");
#endif

	/* Get the item number to be removed */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual item */
	o_ptr = &st_ptr->stock[item];

	/* Description */
	object_desc(o_name, o_ptr, 0);

#ifdef JP
	msg_print("展示をやめさせたアイテムは二度と見ることはできません！");
	if (!get_check(format("本当に%sの展示をやめさせますか？", o_name))) return;
#else
	msg_print("You cannot see items which is removed from the Museum!");
	if (!get_check(format("Really order to remove %s from the Museum? ", o_name))) return;
#endif

	/* Message */
#ifdef JP
	msg_format("%sの展示をやめさせた。", o_name);
#else
	msg_format("You ordered to remove %s.", o_name);
#endif

	/* Remove the items from the home */
	store_item_increase(item, -o_ptr->number);
	store_item_optimize(item);

	(void)combine_and_reorder_home(STORE_MUSEUM);

	/* The item is gone */

	/* Nothing left */
	if (st_ptr->stock_num == 0) store_top = 0;

	/* Nothing left on that screen */
	else if (store_top >= st_ptr->stock_num) store_top -= store_bottom;

	/* Redraw everything */
	display_inventory();

	return;
}


/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 */
static void store_process_command(void)
{
#ifdef ALLOW_REPEAT /* TNB */

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT -- TNB */

	if (rogue_like_commands && command_cmd == 'l')
	{
		command_cmd = 'x';	/* hack! */
	}

	/* Parse the command */
	switch (command_cmd)
	{
		/* Leave */
		case ESCAPE:
		{
			leave_store = TRUE;
			break;
		}

		/* 日本語版追加 */
		/* 1 ページ戻るコマンド: 我が家のページ数が多いので重宝するはず By BUG */
		case '-':
		{
			if (st_ptr->stock_num <= store_bottom) {
#ifdef JP
				msg_print("これで全部です。");
#else
				msg_print("Entire inventory is shown.");
#endif
			}
			else{
				store_top -= store_bottom;
				if ( store_top < 0 )
					store_top = ((st_ptr->stock_num - 1 )/store_bottom) * store_bottom;
				if ( (cur_store_num == STORE_HOME) && (powerup_home == FALSE) )
					if ( store_top >= store_bottom ) store_top = store_bottom;
				display_inventory();
			}
			break;
		}

		/* Browse */
		case ' ':
		{
			if (st_ptr->stock_num <= store_bottom)
			{
#ifdef JP
				msg_print("これで全部です。");
#else
				msg_print("Entire inventory is shown.");
#endif

			}
			else
			{
				store_top += store_bottom;
				/*
				 * 隠しオプション(powerup_home)がセットされていないときは
				 * 我が家では 2 ページまでしか表示しない
				 */
				if ((cur_store_num == STORE_HOME) && 
				    (powerup_home == FALSE) && 
					(st_ptr->stock_num >= STORE_INVEN_MAX))
				{
					if (store_top >= (STORE_INVEN_MAX - 1))
					{
						store_top = 0;
					}
				}
				else
				{
					if (store_top >= st_ptr->stock_num) store_top = 0;
				}

				display_inventory();
			}
			break;
		}

		/* Redraw */
		case KTRL('R'):
		{
			do_cmd_redraw();
			display_store();
			break;
		}

		/* Get (purchase) */
		case 'g':
		{
			store_purchase();
			break;
		}

		/* Drop (Sell) */
		case 'd':
		{
			store_sell();
			break;
		}

		/* Examine */
		case 'x':
		{
			store_examine();
			break;
		}

		/* Ignore return */
		case '\r':
		{
			break;
		}

		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('I'):
		{
			toggle_inven_equip();
			break;
		}



		/*** Use various objects ***/

		/* Browse a book */
		case 'b':
		{
			if ( (p_ptr->pclass == CLASS_MINDCRAFTER) ||
			     (p_ptr->pclass == CLASS_BERSERKER) ||
			     (p_ptr->pclass == CLASS_NINJA) ||
			     (p_ptr->pclass == CLASS_MIRROR_MASTER) 
			     ) do_cmd_mind_browse();
			else if (p_ptr->pclass == CLASS_SMITH)
				do_cmd_kaji(TRUE);
			else if (p_ptr->pclass == CLASS_MAGIC_EATER)
				do_cmd_magic_eater(TRUE);
			else if (p_ptr->pclass == CLASS_SNIPER)
				do_cmd_snipe_browse();
			else do_cmd_browse();
			break;
		}

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_change_name();
			p_ptr->town_num = inner_town_num;
			display_store();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_pref();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with macros */
		case '@':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_macros();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_visuals();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with colors */
		case '&':
		{
			p_ptr->town_num = old_town_num;
			do_cmd_colors();
			p_ptr->town_num = inner_town_num;
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			(void)combine_and_reorder_home(STORE_HOME);
			do_cmd_redraw();
			display_store();
			break;
		}

		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages(0);
			break;
		}

		case '|':
		{
			do_cmd_nikki();
			break;
		}

		/* Check artifacts, uniques etc. */
		case '~':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}

		/* Hack -- Unknown command */
		default:
		{
			if ((cur_store_num == STORE_MUSEUM) && (command_cmd == 'r'))
			{
				museum_remove_object();
			}
			else
			{
#ifdef JP
				msg_print("そのコマンドは店の中では使えません。");
#else
				msg_print("That command does not work in stores.");
#endif
			}

			break;
		}
	}
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void do_cmd_store(void)
{
	int         which;
	int         maintain_num;
	int         i;
	cave_type   *c_ptr;
	bool        need_redraw_store_inv = FALSE; /* To redraw missiles damage and prices in store */
	int w, h;
	bool        vanilla_zerker_hack = FALSE;
	bool        friend_hack = FALSE;


	/* Get term size */
	Term_get_size(&w, &h);

	/* Calculate stocks per 1 page */
	xtra_stock = MIN(14+26, ((h > 24) ? (h - 24) : 0));
	store_bottom = MIN_STOCK + xtra_stock;

	/* Access the player grid */
	c_ptr = &cave[py][px];

	/* Verify a store */
	if (!cave_have_flag_grid(c_ptr, FF_STORE))
	{
#ifdef JP
		msg_print("ここには店がありません。");
#else
		msg_print("You see no store here.");
#endif

		return;
	}

	/* Extract the store code */
	which = f_info[c_ptr->feat].subtype;

	old_town_num = p_ptr->town_num;
	if ((which == STORE_HOME) || (which == STORE_MUSEUM)) p_ptr->town_num = 1;
	if (dun_level) p_ptr->town_num = NO_TOWN;
	inner_town_num = p_ptr->town_num;

	/* Berserkers in Vanilla Town are forced into a tough iron man game, with no id 
	   Let's cut them some slack by giving extra options in the BM.
	*/
	if ( which == STORE_BLACK
		&& p_ptr->pclass == CLASS_BERSERKER
		&& vanilla_town )
	{
		vanilla_zerker_hack = TRUE;
		--xtra_stock;
		--store_bottom;
	}

	if ( (mut_present(MUT_MERCHANTS_FRIEND) || p_ptr->wizard)
	  && which != STORE_HOME
	  && which != STORE_MUSEUM )
	{
		friend_hack = TRUE;
		--xtra_stock;
		--store_bottom;
	}

	/* Hack -- Check the "locked doors" */
	if ((town[p_ptr->town_num].store[which].store_open >= turn) ||
	    (ironman_shops))
	{
#ifdef JP
		msg_print("ドアに鍵がかかっている。");
#else
		msg_print("The doors are locked.");
#endif

		p_ptr->town_num = old_town_num;
		return;
	}

	/* Calculate the number of store maintainances since the last visit */
	maintain_num = (turn - town[p_ptr->town_num].store[which].last_visit) / (TURNS_PER_TICK * STORE_TICKS);

	/* Maintain the store max. 10 times */
	if (maintain_num > 10) maintain_num = 10;

	if (maintain_num && which != STORE_GENERAL && which != STORE_BOOK)
	{
		int xp = town[p_ptr->town_num].store[which].last_exp;
		xp += MIN(MAX(xp / 20, 1000), 100000);
		if ( !ironman_downward
		  && p_ptr->max_plv <= town[p_ptr->town_num].store[which].last_lev
		  && p_ptr->max_exp <= xp 
		  && p_ptr->prace != RACE_ANDROID )
		{
			if (p_ptr->wizard)
				msg_format("Rejecting store restock.  Lev = %d.  XP = %d.", town[p_ptr->town_num].store[which].last_lev, xp);
			maintain_num = 0;
		}
	}

	if (maintain_num)
	{
		/* Maintain the store */
		for (i = 0; i < maintain_num; i++)
			store_maint(p_ptr->town_num, which);

		/* Save the visit */
		town[p_ptr->town_num].store[which].last_visit = turn;
		town[p_ptr->town_num].store[which].last_lev = p_ptr->max_plv;
		town[p_ptr->town_num].store[which].last_exp = p_ptr->max_exp;
	}

	/* Forget the lite */
	forget_lite();

	/* Forget the view */
	forget_view();


	/* Hack -- Character is in "icky" mode */
	character_icky = TRUE;


	/* No command argument */
	command_arg = 0;

	/* No repeated command */
	command_rep = 0;

	/* No automatic command */
	command_new = 0;

	/* Do not expand macros */
	get_com_no_macros = TRUE;

	/* Save the store number */
	cur_store_num = which;

	/* Hack -- save the store feature */
	cur_store_feat = c_ptr->feat;

	/* Save the store and owner pointers */
	st_ptr = &town[p_ptr->town_num].store[cur_store_num];
	ot_ptr = &owners[cur_store_num][st_ptr->owner];


	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Do not leave */
	leave_store = FALSE;

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Clear */
		clear_from(20 + xtra_stock);


		/* Basic commands */
#ifdef JP
		prt(" ESC) 建物から出る", 21 + xtra_stock, 0);
#else
		prt(" ESC) Exit from Building.", 21 + xtra_stock, 0);
#endif

		/* Browse if necessary */
		if (st_ptr->stock_num > store_bottom)
		{
#ifdef JP
			prt(" -)前ページ", 22 + xtra_stock, 0);
			prt(" スペース) 次ページ", 23 + xtra_stock, 0);
#else
			prt(" -) Previous page", 22 + xtra_stock, 0);
			prt(" SPACE) Next page", 23 + xtra_stock, 0);
#endif

		}

		if (vanilla_zerker_hack)
		{
			prt(" 1) Identify (100gp)", 24 + xtra_stock, 0);
			prt("   2) *Identify* (2000gp)", 24 + xtra_stock, 27);
			prt("  3) Identify All (1000gp)", 24 + xtra_stock, 56);
		}
		else if (friend_hack)
		{
			prt(" 1) Shuffle Stock (5000gp)", 24 + xtra_stock, 0);
			prt("   2) Hold Item (10000gp)", 24 + xtra_stock, 27);
		}

		/* Home commands */
		if (cur_store_num == STORE_HOME)
		{
#ifdef JP
			prt("g) アイテムを取る", 21 + xtra_stock, 27);
			prt("d) アイテムを置く", 22 + xtra_stock, 27);
			prt("x) 家のアイテムを調べる", 23 + xtra_stock, 27);
#else
			prt("g) Get an item.", 21 + xtra_stock, 27);
			prt("d) Drop an item.", 22 + xtra_stock, 27);
			prt("x) eXamine an item in the home.", 23 + xtra_stock, 27);
#endif
		}

		/* Museum commands */
		else if (cur_store_num == STORE_MUSEUM)
		{
#ifdef JP
			prt("d) アイテムを置く", 21 + xtra_stock, 27);
			prt("r) アイテムの展示をやめる", 22 + xtra_stock, 27);
			prt("x) 博物館のアイテムを調べる", 23 + xtra_stock, 27);
#else
			prt("d) Drop an item.", 21 + xtra_stock, 27);
			prt("r) order to Remove an item.", 22 + xtra_stock, 27);
			prt("x) eXamine an item in the museum.", 23 + xtra_stock, 27);
#endif
		}

		/* Shop commands XXX XXX XXX */
		else
		{
#ifdef JP
			prt("p) 商品を買う", 21 + xtra_stock, 30);
			prt("s) アイテムを売る", 22 + xtra_stock, 30);
			prt("x) 商品を調べる", 23 + xtra_stock,30);
#else
			prt("p) Purchase an item.", 21 + xtra_stock, 30);
			if (!no_selling)
				prt("s) Sell an item.", 22 + xtra_stock, 30);
			prt("x) eXamine an item in the shop", 23 + xtra_stock,30);
#endif
		}

#ifdef JP
		/* 基本的なコマンドの追加表示 */

		prt("i/e) 持ち物/装備の一覧", 21 + xtra_stock, 56);

		if (rogue_like_commands)
		{
			prt("w/T) 装備する/はずす", 22 + xtra_stock, 56);
		}
		else
		{
			prt("w/t) 装備する/はずす", 22 + xtra_stock, 56);
		}
#else
		prt("i/e) Inventry/Equipment list", 21 + xtra_stock, 56);

		if (rogue_like_commands)
		{
			prt("w/T) Wear/Take off equipment", 22 + xtra_stock, 56);
		}
		else
		{
			prt("w/t) Wear/Take off equipment", 22 + xtra_stock, 56);
		}
#endif
		/* Prompt */
#ifdef JP
		prt("コマンド:", 20 + xtra_stock, 0);
#else
		prt("You may: ", 20 + xtra_stock, 0);
#endif


		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		if (vanilla_zerker_hack)
		{
			/* Total Hackage */
			switch (command_cmd)
			{
			case '1':
				if (100 > p_ptr->au)
					msg_print("You do not have the gold!");
				else
				{
					if (ident_spell(FALSE))
					{
						p_ptr->au -= 100;
						store_prt_gold(); 
					}
				}
				break;
			case '2':
				if (2000 > p_ptr->au)
					msg_print("You do not have the gold!");
				else
				{
					if (identify_fully(FALSE))
					{
						p_ptr->au -= 2000;
						store_prt_gold();
					}
				}
				break;
			case '3':
				if (1000 > p_ptr->au)
					msg_print("You do not have the gold!");
				else
				{
					if (!get_check("Do you pay for identify all your possession? ")) break;
					identify_pack();
					p_ptr->au -= 1000;
					msg_print("Your possessions have been identified.");
					store_prt_gold();
				}
				break;
			default:
				store_process_command();
				break;
			}
		}
		else if (friend_hack)
		{
			switch (command_cmd)
			{
			case '1':
				if (5000 > p_ptr->au)
					msg_print("You do not have the gold!");
				else
				{
					_restock(st_ptr, TRUE);
					need_redraw_store_inv = TRUE;
					p_ptr->au -= 5000;
					store_prt_gold(); 
				}
				break;
			case '2':
				if (10000 > p_ptr->au)
					msg_print("You do not have the gold!");
				else
				{
					int item, i;
					object_type *o_ptr;

					i = (st_ptr->stock_num - store_top);
					if (i > store_bottom) i = store_bottom;
					if (get_stock(&item, "Which item shall I hold for you? ", 0, i - 1))
					{
						item = item + store_top;
						o_ptr = &st_ptr->stock[item];
						if (o_ptr->marked & OM_RESERVED)
						{
							msg_print("You already reserved that item!");
						}
						else
						{
							o_ptr->marked |= OM_RESERVED;
							p_ptr->au -= 10000;
							store_prt_gold();

							need_redraw_store_inv = TRUE;
							msg_print("Done! Come back later when you have more gold, OK?");
						}
					}
				}
				break;
			default:
				store_process_command();
				break;
			}
		}
		else
			store_process_command();

		/*
		 * Hack -- To redraw missiles damage and prices in store
		 * If player's charisma changes, or if player changes a bow, PU_BONUS is set
		 */
		 if (p_ptr->update & PU_BONUS)
			need_redraw_store_inv = TRUE;

		/* Hack -- Character is still in "icky" mode */
		character_icky = TRUE;

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* XXX XXX XXX Pack Overflow */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &inventory[item];

			/* Hack -- Flee from the store */
			if (cur_store_num != STORE_HOME)
			{
				/* Message */
#ifdef JP
				if (cur_store_num == STORE_MUSEUM)
					msg_print("ザックからアイテムがあふれそうなので、あわてて博物館から出た...");
				else
					msg_print("ザックからアイテムがあふれそうなので、あわてて店から出た...");
#else
				if (cur_store_num == STORE_MUSEUM)
					msg_print("Your pack is so full that you flee the Museum...");
				else
					msg_print("Your pack is so full that you flee the store...");
#endif


				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
#ifdef JP
				msg_print("ザックからアイテムがあふれそうなので、あわてて家から出た...");
#else
				msg_print("Your pack is so full that you flee your home...");
#endif


				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type forge;
				object_type *q_ptr;

				char o_name[MAX_NLEN];


				/* Give a message */
#ifdef JP
				msg_print("ザックからアイテムがあふれてしまった！");
#else
				msg_print("Your pack overflows!");
#endif


				/* Get local object */
				q_ptr = &forge;

				/* Grab a copy of the item */
				object_copy(q_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, q_ptr, 0);

				/* Message */
#ifdef JP
				msg_format("%sが落ちた。(%c)", o_name, index_to_label(item));
#else
				msg_format("You drop %s (%c).", o_name, index_to_label(item));
#endif


				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(q_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					store_top = (item_pos / store_bottom) * store_bottom;
					display_inventory();
				}
			}
		}

		/* Hack -- Redisplay store prices if charisma changes */
		/* Hack -- Redraw missiles damage if player changes bow */
		if (need_redraw_store_inv) display_inventory();

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}

	p_ptr->town_num = old_town_num;

	/* Free turn XXX XXX XXX */
	energy_use = 100;


	/* Hack -- Character is no longer in "icky" mode */
	character_icky = FALSE;


	/* Hack -- Cancel automatic command */
	command_new = 0;

	/* Hack -- Cancel "see" mode */
	command_see = FALSE;

	/* Allow expanding macros */
	get_com_no_macros = FALSE;

	/* Flush messages XXX XXX XXX */
	msg_print(NULL);


	/* Clear the screen */
	Term_clear();


	/* Update everything */
	p_ptr->update |= (PU_VIEW | PU_LITE | PU_MON_LITE);
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_EQUIPPY);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_DUNGEON);
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;


	/* Ignore home */
	if (which == STORE_HOME) return;
	if (which == STORE_MUSEUM) return;


	/* Save the store index */
	cur_store_num = which;

	/* Activate that store */
	st_ptr = &town[p_ptr->town_num].store[cur_store_num];

	j = st_ptr->owner;
	/* Pick a new owner */
	while(1)
	{
		st_ptr->owner = (byte)randint0(MAX_OWNERS);
		if (j == st_ptr->owner) continue;
		for (i = 1;i < max_towns; i++)
		{
			if (i == p_ptr->town_num) continue;
			if (st_ptr->owner == town[i].store[cur_store_num].owner) break;
		}
		if (i == max_towns) break;
	}

	/* Activate the new owner */
	ot_ptr = &owners[cur_store_num][st_ptr->owner];


	/* Reset the owner data */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;


	/* Hack -- discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the item */
		o_ptr = &st_ptr->stock[i];

		if (!object_is_artifact(o_ptr))
		{
			/* Hack -- Sell all non-artifact old items for "half price" */
			o_ptr->discount = 50;

			/* Hack -- Items are no longer "fixed price" */
			o_ptr->ident &= ~(IDENT_FIXED);

			/* Mega-Hack -- Note that the item is "on sale" */
#ifdef JP
			o_ptr->inscription = quark_add("売出中");
#else
			o_ptr->inscription = quark_add("on sale");
#endif
		}
	}
}

/*
 * Maintain the inventory at the stores.
 */
void store_maint(int town_num, int store_num)
{
	int j;

	cur_store_num = store_num;

	/* Ignore home */
	if (store_num == STORE_HOME) return;
	if (store_num == STORE_MUSEUM) return;

	/* Activate that store */
	st_ptr = &town[town_num].store[store_num];

	/* Activate the owner */
	ot_ptr = &owners[store_num][st_ptr->owner];

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;

	/* Mega-Hack -- prune the black market */
	if (store_num == STORE_BLACK)
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if (black_market_crap(o_ptr))
			{
				/* Destroy the item */
				store_item_increase(j, 0 - o_ptr->number);
				store_item_optimize(j);
			}
		}
	}

	_restock(st_ptr, FALSE);
}


/*
 * Initialize the stores
 */
void store_init(int town_num, int store_num)
{
	int 		k;

	cur_store_num = store_num;

	/* Activate that store */
	st_ptr = &town[town_num].store[store_num];


	/* Pick an owner */
	while(1)
	{
		int i;

		st_ptr->owner = (byte)randint0(MAX_OWNERS);
		for (i = 1;i < max_towns; i++)
		{
			if (i == town_num) continue;
			if (st_ptr->owner == town[i].store[store_num].owner) break;
		}
		if (i == max_towns) break;
	}

	/* Activate the new owner */
	ot_ptr = &owners[store_num][st_ptr->owner];


	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/*
	 * MEGA-HACK - Last visit to store is
	 * BEFORE player birth to enable store restocking
	 */
	st_ptr->last_visit = -10L * TURNS_PER_TICK * STORE_TICKS;
	st_ptr->last_lev = 0;
	st_ptr->last_exp = 0;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}
}


void move_to_black_market(object_type *o_ptr)
{
	/* Not in town */
	if (!p_ptr->town_num) return;

	st_ptr = &town[p_ptr->town_num].store[STORE_BLACK];

	o_ptr->ident |= IDENT_STORE;

	(void)store_carry(o_ptr);

	object_wipe(o_ptr); /* Don't leave a bogus object behind... */
}

static void _restock(store_type *st_ptr, bool all)
{
	int j, attempt;
	
	j = st_ptr->stock_num;

	if (all)
		j = 0;
	else
	{
		j = j - randint1(STORE_TURNOVER);
		if (j > STORE_MAX_KEEP) 
			j = STORE_MAX_KEEP;
		if (j < STORE_MIN_KEEP) 
			j = STORE_MIN_KEEP;
		if (j < 0) 
			j = 0;
	}

	attempt = 1;
	while (st_ptr->stock_num > j) 
	{
		store_delete(); /* Players may reserve items, so this might fail! */
		attempt++;
		if (attempt > 50)
			break;
	}

	j = st_ptr->stock_num;

	if (all)
		j = STORE_MAX_KEEP;
	else
	{
		j = j + randint1(STORE_TURNOVER);
		if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;
		if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;
	}

	if (j >= st_ptr->stock_size) 
		j = st_ptr->stock_size - 1;

	while (st_ptr->stock_num < j) 
		store_create();
}

