#ifndef _LOGINLOG_TXT_H_
#define _LOGINLOG_TXT_H_

// プロトタイプ宣言
int loginlog_log_txt(const char *fmt, ...);
int loginlog_config_read_txt(const char *str, const char *str2);

// エイリアス
#define loginlog_log loginlog_log_txt
#define loginlog_config_read loginlog_config_read_txt

#endif /* _LOGINLOG_TXT_H_ */
