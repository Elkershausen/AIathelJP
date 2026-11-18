#ifndef _LUASCRIPT_H_
#define _LUASCRIPT_H_

#include "lua.h"

extern lua_State *L;

extern int lua_respawn_id;
int luascript_run_function(const char *name,int char_id,const char *format,...);
void luascript_addscript(const char *chunk);

int luascript_config_read(const char *cfgName);
void luascript_reload(void);

int do_init_luascript(void);
int do_final_luascript(void);

#endif
