#ifndef _HOMUNDB_SQL_H_
#define _HOMUMDB_SQL_H_

#include "mmo.h"
#include "utils.h"

// プロトタイプ宣言
bool homundb_sql_init(void);
int homundb_sql_sync(void);
bool homundb_sql_delete(int homun_id);
const struct mmo_homunstatus* homundb_sql_load(int homun_id);
bool homundb_sql_save(struct mmo_homunstatus* p2);
bool homundb_sql_new(struct mmo_homunstatus *p2);
void homundb_sql_final(void);
int homundb_sql_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define homundb_init   homundb_sql_init
#define homundb_sync   homundb_sql_sync
#define homundb_delete homundb_sql_delete
#define homundb_load   homundb_sql_load
#define homundb_save   homundb_sql_save
#define homundb_new    homundb_sql_new
#define homundb_final  homundb_sql_final
#define homundb_config_read_sub homundb_sql_config_read_sub

#endif /* _HOMUMDB_SQL_H_ */
