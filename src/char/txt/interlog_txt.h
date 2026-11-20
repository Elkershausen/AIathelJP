#ifndef _INTERLOG_TXT_H_
#define _INTERLOG_TXT_H_

// プロトタイプ宣言
int interlog_log_txt(const char *fmt, ...);
int interlog_config_read_txt(const char *str, const char *str2);

// エイリアス
#define interlog_log interlog_log_txt
#define interlog_config_read interlog_config_read_txt

#endif /* _INTERLOG_TXT_H_ */
