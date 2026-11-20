#ifndef _ACCREGDB_SQL_H_
#define _ACCREGDB_SQL_H_

#include "../inter.h"

// プロトタイプ宣言
const struct accreg* accregdb_sql_load(int account_id);
bool accregdb_sql_save(struct accreg *reg);
bool accregdb_sql_init(void);
int accregdb_sql_sync(void);
void accregdb_sql_final(void);
int accregdb_sql_config_read_sub(const char *w1,const char *w2);

// エイリアス
#define accregdb_load  accregdb_sql_load
#define accregdb_save  accregdb_sql_save
#define accregdb_init  accregdb_sql_init
#define accregdb_sync  accregdb_sql_sync
#define accregdb_final accregdb_sql_final
#define accregdb_config_read_sub accregdb_sql_config_read_sub

#endif /* _ACCREGDB_SQL_H_ */
