#include <stdio.h>
#include <string.h>

#include "lock.h"
#include "utils.h"

#include "charlog_txt.h"

static char char_log_filename[1024] = "log/char.log";

/*==========================================
 * ログ記録
 *------------------------------------------
 */
int charlog_log_txt(const char *fmt, ...)
{
	FILE *logfp;
	va_list ap;

	va_start(ap, fmt);

	logfp = fopen(char_log_filename, "a");
	if(logfp) {
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
int charlog_config_read_txt(const char *w1, const char *w2)
{
	if( strcmpi(w1, "char_log_filename") == 0 ) {
		auriga_strlcpy(char_log_filename, w2, sizeof(char_log_filename));
	} else {
		return 0;
	}

	return 1;
}
