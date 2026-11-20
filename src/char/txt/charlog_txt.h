#ifndef _CHARLOG_TXT_H_
#define _CHARLOG_TXT_H_

// プロトタイプ宣言
int charlog_log_txt(const char *fmt, ...);
int charlog_config_read_txt(const char *str, const char *str2);

// エイリアス
#define charlog_log charlog_log_txt
#define charlog_config_read charlog_config_read_txt

#endif /* _CHARLOG_TXT_H_ */
