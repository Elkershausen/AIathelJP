#ifndef _ELEMDB_SQL_H_
#define _ELEMDB_SQL_H_

#include "utils.h"

// プロトタイプ宣言
bool elemdb_sql_init(void);
int elemdb_sql_sync(void);
bool elemdb_sql_delete(int elem_id);
const struct mmo_elemstatus* elemdb_sql_load(int elem_id);
bool elemdb_sql_save(struct mmo_elemstatus* p2);
bool elemdb_sql_new(struct mmo_elemstatus *p2);
void elemdb_sql_final(void);
int elemdb_sql_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define elemdb_init   elemdb_sql_init
#define elemdb_sync   elemdb_sql_sync
#define elemdb_delete elemdb_sql_delete
#define elemdb_load   elemdb_sql_load
#define elemdb_save   elemdb_sql_save
#define elemdb_new    elemdb_sql_new
#define elemdb_final  elemdb_sql_final
#define elemdb_config_read_sub elemdb_sql_config_read_sub

#endif /* _ELEMDB_SQL_H_ */
