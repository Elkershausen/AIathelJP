#ifndef _MAIL_H_
#define _MAIL_H_

void mail_setitem(struct map_session_data *sd, int index, int amount);
int mail_removeitem(struct map_session_data *sd, int flag);
int mail_checkmail(struct map_session_data *sd, const char *name, const char *title,const char *body, int len);
int mail_sendmail(struct map_session_data *sd, struct mail_data *md);
void mail_getappend(int account_id, int zeny, int mail_num, struct item *item);

#endif
