#ifndef _INTERLOG_SQL_H_
#define _INTERLOG_SQL_H_

// プロトタイプ宣言
int interlog_log_sql(const char *fmt, ...);
int interlog_config_read_sql(const char *str, const char *str2);

// エイリアス
#define interlog_log interlog_log_sql
#define interlog_config_read interlog_config_read_sql

#endif /* _INTERLOG_SQL_H_ */
