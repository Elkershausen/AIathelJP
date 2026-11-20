#ifndef _STATUSDB_SQL_H_
#define _STATUSDB_SQL_H_

#ifndef NO_SCDATA_SAVING

#include "utils.h"
#include "../int_status.h"

// プロトタイプ宣言
bool statusdb_sql_init(void);
int statusdb_sql_sync(void);
bool statusdb_sql_delete(int char_id);
struct scdata *statusdb_sql_load(int char_id);
bool statusdb_sql_save(struct scdata *sc2);
void statusdb_sql_final(void);
int statusdb_sql_config_read_sub(const char *w1, const char *w2);

// エイリアス
#define statusdb_init   statusdb_sql_init
#define statusdb_sync   statusdb_sql_sync
#define statusdb_delete statusdb_sql_delete
#define statusdb_load   statusdb_sql_load
#define statusdb_save   statusdb_sql_save
#define statusdb_final  statusdb_sql_final
#define statusdb_config_read_sub statusdb_sql_config_read_sub

#endif

#endif /* _STATUSDB_SQL_H_ */
