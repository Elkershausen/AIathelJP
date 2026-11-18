#ifndef _PATH_H_
#define _PATH_H_

extern const int dirx[], diry[];

int path_search_real(struct walkpath_data *wpd,int m,int x0,int y0,int x1,int y1,int easy,cell_t flag);
#define path_search(wpd,m,x0,y0,x1,y1,easy)  path_search_real(wpd,m,x0,y0,x1,y1,easy,CELL_CHKNOPASS)
#define path_search2(wpd,m,x0,y0,x1,y1,easy) path_search_real(wpd,m,x0,y0,x1,y1,easy,CELL_CHKNONE)
#define path_search3(wpd,m,x0,y0,x1,y1,easy) path_search_real(wpd,m,x0,y0,x1,y1,easy,CELL_CHKWALL)

int path_search_long_real(struct shootpath_data *spd,int m,int x0,int y0,int x1,int y1,cell_t flag);
#define path_search_long(spd,m,x0,y0,x1,y1) path_search_long_real(spd,m,x0,y0,x1,y1,CELL_CHKWALL)

int path_blownpos(int m,int x0,int y0,int dx,int dy,int count,int flag);

int path_check_dir(int s_dir,int t_dir);
int path_calc_dir(struct block_list *src,int x,int y);
int path_distance(int x0,int y0,int x1,int y1);

#endif
