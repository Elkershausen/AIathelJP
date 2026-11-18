#ifndef _PET_H_
#define _PET_H_

#define MAX_PET_DB	32767

struct pet_db {
	short class_;
	char name[24],jname[24];
	int itemID;
	int EggID;
	int AcceID;
	int FoodID;
	int fullness;
	int hungry_delay;
	int r_hungry;
	int r_full;
	int die;
	int capture;
	int speed;
	short intimate;
	char s_perfor;
	int talk_convert_class;
	int attack_rate;
	int defence_attack_rate;
	int change_target_rate;
	struct script_code *script;
};

enum {
	PET_CLASS,
	PET_CATCH,
	PET_EGG,
	PET_EQUIP,
	PET_FOOD,
};

struct pet_db* pet_search_data(int key, int type);
int pet_hungry_val(struct map_session_data *sd);
int pet_target_check(struct map_session_data *sd,struct block_list *bl,int type);
int pet_hungry_timer_delete(struct pet_data *pd);
int pet_recv_petdata(int account_id,struct s_pet *p,int flag);
int pet_select_egg(struct map_session_data *sd,short egg_index);
int pet_catch_process1(struct map_session_data *sd,int target_class);
int pet_catch_process2(struct map_session_data *sd,int target_id);
int pet_get_egg(int account_id,int pet_id,int flag);
int pet_return_egg(struct map_session_data *sd);
int pet_menu(struct map_session_data *sd,int menunum);
int pet_change_name(struct map_session_data *sd,const char *name);
void pet_equipitem(struct map_session_data *sd, int idx);
int pet_lootitem_drop(struct pet_data *pd,struct map_session_data *sd);
int pet_lootitem_free(struct pet_data *pd);
int pet_skill_support_timer(int tid,unsigned int tick,int id,void *data);
int do_init_pet(void);
int do_final_pet(void);

int read_petdb(void);

#endif
