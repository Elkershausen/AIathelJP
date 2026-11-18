#ifndef _INT_HOMUN_H_
#define _INT_HOMUN_H_

int inter_hom_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/homundb_txt.h"
#else
	#include "sql/homundb_sql.h"
#endif

#endif /* _INT_HOMUN_H_ */
