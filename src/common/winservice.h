#ifndef _WINSERVICE_H_
#define _WINSERVICE_H_

#ifdef WINDOWS

#include "utils.h"

bool winservice_change_current_dir(void);
bool winservice_init(int argc, char **argv);
bool winservice_notify_start(void);
bool winservice_notify_ready(void);
bool winservice_notify_stop(void);
bool winservice_notify_finish(void);

#else

#define winservice_change_current_dir() (1 == 1)
#define winservice_init(c,v)            (1 == 0)
#define winservice_notify_start()       (1 == 1)
#define winservice_notify_ready()       (1 == 1)
#define winservice_notify_stop()        (1 == 1)
#define winservice_notify_finish()      (1 == 1)

#endif

#endif /* _WINSERVICE_H_ */
