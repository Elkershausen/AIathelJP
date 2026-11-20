#ifndef _PARTYDB_SQL_H_
#define _PARTYDB_SQL_H_

#include "mmo.h"
#include "utils.h"

// プロトタイプ宣言
bool partydb_sql_init(void);
int partydb_sql_sync(void);
const struct party* partydb_sql_load_str(const char *str);
const struct party* partydb_sql_load_num(int party_id);
bool partydb_sql_save(struct party* p2);
bool partydb_sql_delete(int party_id);
bool partydb_sql_new(struct party *p);
int partydb_sql_config_read_sub(const char *w1,const char *w2);
void partydb_sql_final(void);

// エイリアス
#define partydb_init     partydb_sql_init
#define partydb_sync     partydb_sql_sync
#define partydb_save     partydb_sql_save
#define partydb_final    partydb_sql_final
#define partydb_delete   partydb_sql_delete
#define partydb_load_str partydb_sql_load_str
#define partydb_load_num partydb_sql_load_num
#define partydb_new      partydb_sql_new
#define partydb_config_read_sub partydb_sql_config_read_sub

#endif /* _PARTYDB_SQL_H_ */
