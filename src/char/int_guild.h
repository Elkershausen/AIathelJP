#ifndef _INT_GUILD_H_
#define _INT_GUILD_H_

#include "mmo.h"

extern int guild_exp[MAX_GUILDLEVEL];
extern struct dbt *guild_db;
extern struct guild_castle castle_db[MAX_GUILDCASTLE];

char* guild_decode_emblem(char *dst, const char *src, int len);

int guild_readdb(void);
int guild_config_read(const char *w1,const char* w2);
void guild_calc_skilltree(struct guild *g);
int guild_calcinfo(struct guild *g);

int mapif_guild_broken(int guild_id,int flag);
int inter_guild_parse_frommap(int fd);
int inter_guild_mapif_init(int fd);
int inter_guild_leave(int guild_id,int account_id,int char_id);

#ifdef TXT_ONLY
	#include "txt/guilddb_txt.h"
#else
	#include "sql/guilddb_sql.h"
#endif

#endif /* _INT_GUILD_H_ */
