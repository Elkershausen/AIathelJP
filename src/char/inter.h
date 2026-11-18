#ifndef _INTER_H_
#define _INTER_H_

struct accreg {
	int account_id,reg_num;
	struct global_reg reg[ACCOUNT_REG_NUM];
};

int inter_init(const char *file);
int inter_sync(void);
int inter_parse_frommap(int fd);
int inter_mapif_init(int fd);

void do_final_inter(void);

#ifdef TXT_ONLY
	#include "txt/accregdb_txt.h"
	#include "txt/interlog_txt.h"
#else
	#include "sql/accregdb_sql.h"
	#include "sql/interlog_sql.h"
#endif

#endif /* _INTER_H_ */
