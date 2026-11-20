#ifndef _STORAGEDB_SQL_H_
#define _STORAGEDB_SQL_H_

#include "mmo.h"
#include "utils.h"

// プロトタイプ宣言
bool storagedb_sql_init(void);
const struct storage* storagedb_sql_load(int account_id);
bool storagedb_sql_save(struct storage *s2);
int storagedb_sql_sync(void);
bool storagedb_sql_delete(int account_id);
void storagedb_sql_final(void);
int storagedb_sql_config_read_sub(const char* w1,const char* w2);
const struct guild_storage *gstoragedb_sql_load(int guild_id);
bool gstoragedb_sql_save(struct guild_storage *gs2, int easy);
int gstoragedb_sql_sync(void);
bool gstoragedb_sql_delete(int guild_id);
void gstoragedb_sql_final(void);

// エイリアス
#define storagedb_init   storagedb_sql_init
#define storagedb_load   storagedb_sql_load
#define storagedb_save   storagedb_sql_save
#define storagedb_sync   storagedb_sql_sync
#define storagedb_delete storagedb_sql_delete
#define storagedb_final  storagedb_sql_final
#define storagedb_config_read_sub storagedb_sql_config_read_sub

#define gstoragedb_load   gstoragedb_sql_load
#define gstoragedb_save   gstoragedb_sql_save
#define gstoragedb_sync   gstoragedb_sql_sync
#define gstoragedb_delete gstoragedb_sql_delete
#define gstoragedb_final  gstoragedb_sql_final

#endif /* _STORAGEDB_SQL_H_ */
