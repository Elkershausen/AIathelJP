#ifndef _HOMUNDB_TXT_H_
#define _HOMUNDB_TXT_H_

#include "mmo.h"
#include "utils.h"

// プロトタイプ宣言
bool homundb_txt_init(void);
int homundb_txt_sync(void);
bool homundb_txt_delete(int homun_id);
const struct mmo_homunstatus* homundb_txt_load(int homun_id);
bool homundb_txt_save(struct mmo_homunstatus* p2);
bool homundb_txt_new(struct mmo_homunstatus *p);
void homundb_txt_final(void);
int homundb_txt_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define homundb_init   homundb_txt_init
#define homundb_sync   homundb_txt_sync
#define homundb_delete homundb_txt_delete
#define homundb_load   homundb_txt_load
#define homundb_save   homundb_txt_save
#define homundb_new    homundb_txt_new
#define homundb_final  homundb_txt_final
#define homundb_config_read_sub homundb_txt_config_read_sub

#endif /* _HOMUNDB_TXT_H_ */
