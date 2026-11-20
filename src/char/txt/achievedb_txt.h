#ifndef _ACHIEVEDB_TXT_H_
#define _ACHIEVEDB_TXT_H_

#include "../int_achieve.h"

// プロトタイプ宣言
bool achievedb_txt_init(void);
int  achievedb_txt_sync(void);
bool  achievedb_txt_delete(int char_id);
const struct achieve *achievedb_txt_load(int char_id);
bool  achievedb_txt_save(struct achieve *a2);
void achievedb_txt_final(void);
int achievedb_txt_config_read_sub(const char *w1,const char *w2);

// エイリアス
#define achievedb_init   achievedb_txt_init
#define achievedb_sync   achievedb_txt_sync
#define achievedb_delete achievedb_txt_delete
#define achievedb_load   achievedb_txt_load
#define achievedb_save   achievedb_txt_save
#define achievedb_final  achievedb_txt_final
#define achievedb_config_read_sub achievedb_txt_config_read_sub

#endif /* _ACHIEVEDB_TXT_H_ */
