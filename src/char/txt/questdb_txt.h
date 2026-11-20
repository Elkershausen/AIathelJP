#ifndef _QUESTDB_TXT_H_
#define _QUESTDB_TXT_H_

#include "../int_quest.h"

// プロトタイプ宣言
bool questdb_txt_init(void);
int  questdb_txt_sync(void);
bool  questdb_txt_delete(int char_id);
const struct quest *questdb_txt_load(int char_id);
bool  questdb_txt_save(struct quest *q2);
void questdb_txt_final(void);
int questdb_txt_config_read_sub(const char *w1,const char *w2);

// エイリアス
#define questdb_init   questdb_txt_init
#define questdb_sync   questdb_txt_sync
#define questdb_delete questdb_txt_delete
#define questdb_load   questdb_txt_load
#define questdb_save   questdb_txt_save
#define questdb_final  questdb_txt_final
#define questdb_config_read_sub questdb_txt_config_read_sub

#endif /* _QUESTDB_TXT_H_ */
