#ifndef _INT_ELEM_H_
#define _INT_ELEM_H_

int inter_elem_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/elemdb_txt.h"
#else
	#include "sql/elemdb_sql.h"
#endif

#endif /* _INT_ELEM_H_ */
