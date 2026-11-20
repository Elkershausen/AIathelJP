#ifndef _LOGINLOG_SQL_H_
#define _LOGINLOG_SQL_H_

// プロトタイプ宣言
int loginlog_log_sql(const char *fmt, ...);
int loginlog_config_read_sql(const char *str, const char *str2);

// エイリアス
#define loginlog_log loginlog_log_sql
#define loginlog_config_read loginlog_config_read_sql

#endif /* _LOGINLOG_SQL_H_ */
