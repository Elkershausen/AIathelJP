#ifndef _BOOKING_H_
#define _BOOKING_H_

#include "map.h"

// 旧仕様のパーティーブッキング
void booking_searchcond(struct map_session_data *sd, int lv, int map, int job, unsigned int last_index, int result_count);
void booking_register(struct map_session_data *sd, int lv, int map, int *job);
void booking_update(struct map_session_data *sd, int *job);

// 新仕様のパーティーブッキング
void booking_searchcond2(struct map_session_data *sd, int lv);
void booking_register2(struct map_session_data *sd, int lv, const char *memo);
void booking_update2(struct map_session_data *sd, const char *memo);

// 新旧仕様の共通関数
void booking_delete(struct map_session_data *sd);

// パーティーブッキングデータの処理用
void do_final_booking(void);
void do_init_booking(void);

#endif
