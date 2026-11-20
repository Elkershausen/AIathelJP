#ifndef _GUILDDB_SQL_H_
#define _GUILDDB_SQL_H_

#include "mmo.h"
#include "utils.h"

// プロトタイプ宣言
bool guilddb_sql_new(struct guild *g);
bool guilddb_sql_init(void);
bool guilddb_sql_save(struct guild* g2);
int guilddb_sql_sync(void);
void guilddb_sql_final(void);
const struct guild *guilddb_sql_load_num(int guild_id);
const struct guild* guilddb_sql_load_str(const char *str);
bool guilddb_sql_delete(int guild_id);
int guilddb_sql_config_read_sub(const char* w1,const char *w2);

// エイリアス
#define guilddb_new      guilddb_sql_new
#define guilddb_init     guilddb_sql_init
#define guilddb_save     guilddb_sql_save
#define guilddb_sync     guilddb_sql_sync
#define guilddb_final    guilddb_sql_final
#define guilddb_load_num guilddb_sql_load_num
#define guilddb_load_str guilddb_sql_load_str
#define guilddb_delete   guilddb_sql_delete
#define guilddb_config_read_sub guilddb_sql_config_read_sub

#endif /* _GUILDDB_SQL_H_ */
