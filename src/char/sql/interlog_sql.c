#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "sqldbs.h"

#include "interlog_sql.h"

/*==========================================
 * ログ記録
 *------------------------------------------
 */
int interlog_log_sql(const char *fmt, ...)
{
	char msg[256], buf[512];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(msg, sizeof(msg), fmt, ap);
	va_end(ap);

	sqldbs_query(&mysql_handle, "INSERT INTO `" INTERLOG_TABLE "` (`time`,`log`) VALUES (NOW(),'%s')", strecpy(buf, msg));

	return 0;
}

/*==========================================
 * 設定ファイル読込
 *------------------------------------------
 */
int interlog_config_read_sql(const char *w1, const char *w2)
{
	return 0;
}
