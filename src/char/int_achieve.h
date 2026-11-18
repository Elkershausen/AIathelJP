#ifndef _INT_ACHIEVE_H_
#define _INT_ACHIEVE_H_

struct achieve {
	int account_id;
	int char_id;
	short count;
	struct {
		int nameid;
		int count[MAX_ACHIEVE_CONTENT];
		unsigned int comp_date;
		char reward;
	} data[MAX_ACHIEVELIST];
};

int inter_achieve_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/achievedb_txt.h"
#else
	#include "sql/achievedb_sql.h"
#endif

#endif /* _INT_ACHIEVE_H_ */
