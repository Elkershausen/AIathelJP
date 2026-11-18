#ifndef _BUYINGSTORE_H_
#define _BUYINGSTORE_H_

#include "utils.h"

bool buyingstore_openstorewindow(struct map_session_data *sd, unsigned char count);
void buyingstore_openstore(struct map_session_data *sd, int limit_zeny, bool result, const char *store_name, const unsigned char *data, int count);
void buyingstore_close(struct map_session_data *sd);
void buyingstore_itemlist(struct map_session_data* sd, int account_id);
void buyingstore_sell(struct map_session_data *sd, int account_id, unsigned int buyer_id, const unsigned char *data, int count);

#endif  // _BUYINGSTORE_H_
