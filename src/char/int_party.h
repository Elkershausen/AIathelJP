#ifndef _INT_PARTY_H_
#define _INT_PARTY_H_

int party_check_empty(const struct party *p);
int inter_party_parse_frommap(int fd);
void inter_party_leave(int party_id, int account_id, int char_id);
int party_config_read(const char *w1,const char* w2);

#ifdef TXT_ONLY
	#include "txt/partydb_txt.h"
#else
	#include "sql/partydb_sql.h"
#endif

#endif /* _INT_PARTY_H_ */
