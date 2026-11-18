#ifndef	_VENDING_H_
#define	_VENDING_H_

#include "map.h"
#include "utils.h"

void vending_closevending(struct map_session_data *sd);
void vending_openvending(struct map_session_data *sd, short count, const char *shop_title, bool is_open, const unsigned char *data);
void vending_vendinglistreq(struct map_session_data *sd,int id);
void vending_purchasereq(struct map_session_data *sd, short count, int account_id, unsigned int vender_id, const unsigned char *data);

#endif	// _VENDING_H_
