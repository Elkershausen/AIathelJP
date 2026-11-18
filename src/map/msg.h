#ifndef _MSG_H_
#define _MSG_H_

#include "map.h"

const char * msg_txt(int msg_number);
void msg_output(const int fd, const char *format, ...);

void msg_send_motd(struct map_session_data *sd);
int msg_read_motd(void);
int msg_config_read(const char *cfgName);

int do_init_msg(void);
void do_final_msg(void);

#endif
