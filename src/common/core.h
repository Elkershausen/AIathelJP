#ifndef	_CORE_H_
#define	_CORE_H_

int do_init(int,char**);
void do_final(void);
void do_pre_final(void);
void do_stop(void);
double uptime(void);

int main_sub(int argc, char **argv);

#endif	// _CORE_H_
