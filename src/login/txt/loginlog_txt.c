#include <stdio.h>
#include <string.h>

#include "lock.h"
#include "utils.h"

#include "loginlog_txt.h"

static char loginlog_filename[1024] = "log/login.log";

/*==========================================
 * ログ記録
 *------------------------------------------
 */
int loginlog_log_txt(const char *fmt, ...)
{
	FILE *logfp;
	va_list ap;

	va_start(ap, fmt);
	logfp = fopen(loginlog_filename, "a");

	if(logfp)
	{
		vfprintf(logfp, fmt, ap);
		fprintf(logfp, NEWLINE);
		fclose(logfp);
	}

	va_end(ap);

	return 0;
}

/*==========================================
 * 設定ファイル読込
 *------------------------------------------
 */
int loginlog_config_read_txt(const char *w1, const char *w2)
{
	if( strcmpi(w1, "login_log_filename") == 0 ) {
		strncpy(loginlog_filename, w2, sizeof(loginlog_filename) - 1);
		loginlog_filename[sizeof(loginlog_filename) - 1] = '\0';
	} else {
		return 0;
	}

	return 1;
}
