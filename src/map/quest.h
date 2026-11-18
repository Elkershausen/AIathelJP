#ifndef _QUEST_H_
#define _QUEST_H_

#define MAX_QUEST_DB 4000

// クエストデータベース
struct quest_db {
	int nameid;
	unsigned int limit;
	bool limit_type;
	struct {
		short id;
		short count;
	} mob[3];
	struct {
		short id;
		int nameid;
		int p;
	} drop[3];
};

struct quest_data *quest_get_data(struct map_session_data *sd, int quest_id);
int quest_search_mobid(int mob_id);
int quest_addlist(struct map_session_data *sd, int quest_id);
int quest_updatelist(struct map_session_data *sd, int old_id, int new_id);
int quest_dellist(struct map_session_data *sd, int quest_id);
int quest_update_status(struct map_session_data *sd, int quest_id, char state);
int quest_killcount_sub(struct block_list *tbl, va_list ap);
int quest_killcount(struct map_session_data *sd, int mob_id);

void quest_reload(void);
int do_final_quest(void);
int do_init_quest(void);

#endif	// _QUEST_H_
