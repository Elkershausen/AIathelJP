#ifndef _INT_QUEST_H_
#define _INT_QUEST_H_

struct quest {
	int account_id;
	int char_id;
	short count;
	struct {
		int nameid;
		char state;
		unsigned int limit;
		struct {
			short id;
			short max;
			short cnt;
		} mob[3];
	} data[MAX_QUESTLIST];
};

int inter_quest_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/questdb_txt.h"
#else
	#include "sql/questdb_sql.h"
#endif

#endif /* _INT_QUEST_H_ */
