#ifndef _PETDB_TXT_H_
#define _PETDB_TXT_H_

#include "utils.h"

// プロトタイプ宣言
bool petdb_txt_init(void);
int petdb_txt_sync(void);
bool petdb_txt_delete(int pet_id);
const struct s_pet* petdb_txt_load(int pet_id);
bool petdb_txt_save(struct s_pet* p2);
bool petdb_txt_new(struct s_pet *p);
void petdb_txt_final(void);
int petdb_txt_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define petdb_init   petdb_txt_init
#define petdb_sync   petdb_txt_sync
#define petdb_delete petdb_txt_delete
#define petdb_load   petdb_txt_load
#define petdb_save   petdb_txt_save
#define petdb_new    petdb_txt_new
#define petdb_final  petdb_txt_final
#define petdb_config_read_sub petdb_txt_config_read_sub

#endif /* _PETDB_TXT_H_ */
