#include <stdio.h>
#include <stdlib.h>

#include "version.h"


/*==========================================
 * バージョンの取得
 *------------------------------------------
 */
int get_current_version(void)
{
	static int auriga_version = 0;

	if(auriga_version == 0) {
		FILE *fp;
		char filename[256] = "CHANGELOG.md";

		fp = fopen(filename, "r");
		if(fp) {
			char line[1024];
			int ver;

			while(fgets(line, sizeof(line), fp)) {
				if(sscanf(line, "//%d [%*d/%*d/%*d] by", &ver) == 1) {
					auriga_version = ver;
					break;
				}
			}
			fclose(fp);
		}
		if(auriga_version == 0)
			auriga_version = AURIGA_REVISION;
	}

	return auriga_version;
}
