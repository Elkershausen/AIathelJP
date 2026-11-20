#ifndef _ACCREGDB_TXT_H_
#define _ACCREGDB_TXT_H_

#include "../inter.h"

// プロトタイプ宣言
const struct accreg* accregdb_txt_load(int account_id);
bool accregdb_txt_save(struct accreg* reg2);
bool accregdb_txt_init(void);
int accregdb_txt_sync(void);
void accregdb_txt_final(void);
int accregdb_txt_config_read_sub(const char *w1,const char *w2);

// エイリアス
#define accregdb_load  accregdb_txt_load
#define accregdb_save  accregdb_txt_save
#define accregdb_init  accregdb_txt_init
#define accregdb_sync  accregdb_txt_sync
#define accregdb_final accregdb_txt_final
#define accregdb_config_read_sub accregdb_txt_config_read_sub

#endif /* _ACCREGDB_TXT_H_ */
