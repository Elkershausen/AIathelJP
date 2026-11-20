#ifndef _MERCDB_SQL_H_
#define _MERCDB_SQL_H_

#include "utils.h"

// プロトタイプ宣言
bool mercdb_sql_init(void);
int mercdb_sql_sync(void);
bool mercdb_sql_delete(int merc_id);
const struct mmo_mercstatus* mercdb_sql_load(int merc_id);
bool mercdb_sql_save(struct mmo_mercstatus* p2);
bool mercdb_sql_new(struct mmo_mercstatus *p2);
void mercdb_sql_final(void);
int mercdb_sql_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define mercdb_init   mercdb_sql_init
#define mercdb_sync   mercdb_sql_sync
#define mercdb_delete mercdb_sql_delete
#define mercdb_load   mercdb_sql_load
#define mercdb_save   mercdb_sql_save
#define mercdb_new    mercdb_sql_new
#define mercdb_final  mercdb_sql_final
#define mercdb_config_read_sub mercdb_sql_config_read_sub

#endif /* _MERCDB_SQL_H_ */
