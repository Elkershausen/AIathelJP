#ifndef _CHARLOG_SQL_H_
#define _CHARLOG_SQL_H_

// プロトタイプ宣言
int charlog_log_sql(const char *fmt, ...);
int charlog_config_read_sql(const char *str, const char *str2);

// エイリアス
#define charlog_log charlog_log_sql
#define charlog_config_read charlog_config_read_sql

#endif /* _CHARLOG_SQL_H_ */
