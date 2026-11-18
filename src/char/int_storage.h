#ifndef _INT_STORAGE_H_
#define _INT_STORAGE_H_

extern struct dbt *gstorage_db;

int inter_storage_parse_frommap(int fd);
int inter_storage_init(void);

#ifdef TXT_ONLY
	#include "txt/storagedb_txt.h"
#else
	#include "sql/storagedb_sql.h"
#endif

#endif /* _INT_STORAGE_H_ */
