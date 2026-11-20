#ifndef _STATUSDB_TXT_H_
#define _STATUSDB_TXT_H_

#ifndef NO_SCDATA_SAVING

#include "utils.h"
#include "../int_status.h"

// プロトタイプ宣言
bool statusdb_txt_init(void);
int statusdb_txt_sync(void);
bool statusdb_txt_delete(int char_id);
struct scdata *statusdb_txt_load(int char_id);
bool statusdb_txt_save(struct scdata *sc2);
void statusdb_txt_final(void);
int statusdb_txt_config_read_sub(const char *w1, const char *w2);

// エイリアス
#define statusdb_init   statusdb_txt_init
#define statusdb_sync   statusdb_txt_sync
#define statusdb_delete statusdb_txt_delete
#define statusdb_load   statusdb_txt_load
#define statusdb_save   statusdb_txt_save
#define statusdb_final  statusdb_txt_final
#define statusdb_config_read_sub statusdb_txt_config_read_sub

#endif

#endif /* _STATUSDB_TXT_H_ */
