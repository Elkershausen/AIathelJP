#ifndef _INT_MERC_H_
#define _INT_MERC_H_

int inter_merc_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/mercdb_txt.h"
#else
	#include "sql/mercdb_sql.h"
#endif

#endif /* _INT_MERC_H_ */
