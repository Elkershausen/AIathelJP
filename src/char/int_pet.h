#ifndef _INT_PET_H_
#define _INT_PET_H_

int inter_pet_parse_frommap(int fd);

#ifdef TXT_ONLY
	#include "txt/petdb_txt.h"
#else
	#include "sql/petdb_sql.h"
#endif

#endif
