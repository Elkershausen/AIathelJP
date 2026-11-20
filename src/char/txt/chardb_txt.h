#ifndef _CHARDB_TXT_H_
#define _CHARDB_TXT_H_

#include "mmo.h"
#include "utils.h"
#include "../char.h"

// プロトタイプ宣言
const struct mmo_chardata *chardb_txt_make(int account_id, const unsigned char *name, short str, short agi, short vit, short int_, short dex, short luk, short hair_color, short hair, short job, char sex, unsigned char slot, int *flag);
bool chardb_txt_init(void);
void chardb_txt_sync(void);
const struct mmo_chardata* chardb_txt_load(int char_id);
bool chardb_txt_save_reg(int account_id,int char_id,int num,struct global_reg *reg);
bool chardb_txt_save(struct mmo_charstatus *st);
void chardb_txt_final(void);
int chardb_txt_load_all(struct char_session_data* sd,int account_id);
bool chardb_txt_delete_sub(int char_id);
const struct mmo_chardata* chardb_txt_nick2chardata(const char *char_name);
bool chardb_txt_build_ranking(void);
int chardb_txt_config_read_sub(const char* w1,const char* w2);
bool chardb_txt_set_online(int char_id, bool is_online);

// エイリアス
#define chardb_make            chardb_txt_make
#define chardb_init            chardb_txt_init
#define chardb_sync            chardb_txt_sync
#define chardb_load            chardb_txt_load
#define chardb_save_reg        chardb_txt_save_reg
#define chardb_save            chardb_txt_save
#define chardb_final           chardb_txt_final
#define chardb_load_all        chardb_txt_load_all
#define chardb_delete_sub      chardb_txt_delete_sub
#define chardb_nick2chardata   chardb_txt_nick2chardata
#define chardb_build_ranking   chardb_txt_build_ranking
#define chardb_config_read_sub chardb_txt_config_read_sub
#define chardb_set_online(id)  chardb_txt_set_online((id),true)
#define chardb_set_offline(id) chardb_txt_set_online((id),false)
#define chardb_build_ranking   chardb_txt_build_ranking

#endif /* _CHARDB_TXT_H_ */
