#ifndef _ACCOUNT_TXT_H_
#define _ACCOUNT_TXT_H_

#include "mmo.h"
#include "utils.h"

#include "../login.h"

// プロトタイプ宣言
bool account_txt_init(void);
void account_txt_sync(void);
const struct mmo_account* account_txt_account_load_num(int account_id);
const struct mmo_account* account_txt_account_load_str(const char *account_id);
const struct mmo_account* account_txt_account_load_idx(int idx);
bool account_txt_account_save(struct mmo_account *account);
bool account_txt_account_delete(int account_id);
bool account_txt_account_new(struct mmo_account* account,const char *tmpstr);
void account_txt_final(void);
void account_txt_set_default_configvalue(void);
int account_txt_config_read_sub(const char *w1,const char *w2);
void display_conf_warnings_txt(void);

// エイリアス
#define login_init             account_txt_init
#define login_sync             account_txt_sync
#define login_final            account_txt_final
#define login_config_set_defaultvalue_sub account_txt_set_default_configvalue
#define login_config_read_sub  account_txt_config_read_sub
#define account_new            account_txt_account_new
#define account_save           account_txt_account_save
#define account_delete         account_txt_account_delete
#define account_load_num       account_txt_account_load_num
#define account_load_str       account_txt_account_load_str
#define account_load_idx       account_txt_account_load_idx
#define display_conf_warnings2 display_conf_warnings_txt

#endif /* _ACCOUNT_TXT_H_ */
