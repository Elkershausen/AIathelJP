#ifndef _INT_MAIL_H_
#define _INT_MAIL_H_

int mapif_mail_res(const int fd,int char_id,int flag);
int inter_mail_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/maildb_txt.h"
#else
	#include "sql/maildb_sql.h"
#endif

#endif /* _INT_MAIL_H_ */
