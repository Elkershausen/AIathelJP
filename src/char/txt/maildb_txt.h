#ifndef _MAILDB_TXT_H_
#define _MAILDB_TXT_H_

#include "utils.h"

// プロトタイプ宣言
bool maildb_txt_store_mail(int char_id,struct mail_data *md);
bool maildb_txt_save_mail(int char_id,int i,int store,struct mail_data md[MAIL_STORE_MAX]);
bool maildb_txt_read_mail(int char_id,const struct mail *m,struct mail_data md[MAIL_STORE_MAX]);
bool maildb_txt_deletemail(int char_id,unsigned int mail_num,const struct mail *m);
bool maildb_txt_init(void);
int maildb_txt_sync(void);
bool maildb_txt_delete(int char_id);
const struct mail* maildb_txt_load(int char_id);
bool maildb_txt_save(struct mail* m2);
bool maildb_txt_new(int account_id,int char_id);
void maildb_txt_final(void);
int maildb_txt_config_read_sub(const char *w1,const char *w2);

// エイリアス
#define maildb_store_mail      maildb_txt_store_mail
#define maildb_save_mail       maildb_txt_save_mail
#define maildb_read_mail       maildb_txt_read_mail
#define maildb_deletemail      maildb_txt_deletemail
#define maildb_init            maildb_txt_init
#define maildb_sync            maildb_txt_sync
#define maildb_delete          maildb_txt_delete
#define maildb_load            maildb_txt_load
#define maildb_save            maildb_txt_save
#define maildb_new             maildb_txt_new
#define maildb_final           maildb_txt_final
#define maildb_config_read_sub maildb_txt_config_read_sub

#endif /* _MAILDB_TXT_H_ */
