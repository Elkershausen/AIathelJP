#ifndef _MAILDB_SQL_H_
#define _MAILDB_SQL_H_

#include "utils.h"

// プロトタイプ宣言
bool maildb_sql_store_mail(int char_id,struct mail_data *md);
bool maildb_sql_save_mail(int char_id,int i,int store,struct mail_data md[MAIL_STORE_MAX]);
bool maildb_sql_read_mail(int char_id,const struct mail *m,struct mail_data md[MAIL_STORE_MAX]);
bool maildb_sql_deletemail(int char_id,unsigned int mail_num,const struct mail *m);
bool maildb_sql_init(void);
int maildb_sql_sync(void);
bool maildb_sql_delete(int char_id);
const struct mail* maildb_sql_load(int char_id);
bool maildb_sql_save(struct mail* m2);
bool maildb_sql_new(int account_id,int char_id);
void maildb_sql_final(void);
int maildb_sql_config_read_sub(const char *w1,const char *w2);

// エイリアス
#define maildb_store_mail      maildb_sql_store_mail
#define maildb_save_mail       maildb_sql_save_mail
#define maildb_read_mail       maildb_sql_read_mail
#define maildb_deletemail      maildb_sql_deletemail
#define maildb_init            maildb_sql_init
#define maildb_sync            maildb_sql_sync
#define maildb_delete          maildb_sql_delete
#define maildb_load            maildb_sql_load
#define maildb_save            maildb_sql_save
#define maildb_new             maildb_sql_new
#define maildb_final           maildb_sql_final
#define maildb_config_read_sub maildb_sql_config_read_sub

#endif /* _MAILDB_SQL_H_ */
