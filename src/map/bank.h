#ifndef	_BANK_H_
#define	_BANK_H_

void bank_deposit(struct map_session_data *sd, int target_id, int zeny);
void bank_withdraw(struct map_session_data *sd, int target_id, int zeny);

#endif	// _BANK_H_
