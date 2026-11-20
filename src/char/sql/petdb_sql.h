#ifndef _PETDB_SQL_H_
#define _PETDB_SQL_H_

#include "utils.h"

// プロトタイプ宣言
bool petdb_sql_init(void);
int petdb_sql_sync(void);
bool petdb_sql_delete(int pet_id);
const struct s_pet* petdb_sql_load(int pet_id);
bool petdb_sql_save(struct s_pet* p2);
bool petdb_sql_new(struct s_pet *p);
void petdb_sql_final(void);
int petdb_sql_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define petdb_init   petdb_sql_init
#define petdb_sync   petdb_sql_sync
#define petdb_delete petdb_sql_delete
#define petdb_load   petdb_sql_load
#define petdb_save   petdb_sql_save
#define petdb_new    petdb_sql_new
#define petdb_final  petdb_sql_final
#define petdb_config_read_sub petdb_sql_config_read_sub

#endif /* _PETDB_SQL_H_ */
