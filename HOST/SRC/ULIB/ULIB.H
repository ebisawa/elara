
#if !defined(ULIB_H)
#define ULIB_H

int dopen(char far *dir, char far *filename, unsigned mode);
int dopen2(char far *dir1, char far *dir2, char far *name, unsigned mode);
int typetext(char far *dir, char far *filename);
void disconnect(void);
void disconnect_device(char far *device);
uid_struct far *getuid_n(char far *name);
int save_usrdat(userdat far *usrdat, char far *id, config_t far *conf);
void kill_lock(int sw);
int nelems(char far *p);
char far *nextelem(char far *p);
int search_usrdat(userdat far *usrdat, char far *id, config_t far *conf);
uid_struct far *getuid_d(char far *device_name, uid_struct far *head);
void save_log_nouser(uid_struct far *uid, uid_struct far *head, long connect_time, long disconnect_time, int reason);
void last_access(accesslog far *log, config_t far *conf);
void savelog_file(accesslog far *log, config_t far *conf);
int getchnum(char far *device_name, uid_struct far *head);
void save_log_preset(uid_struct far *uid, uid_struct far *head, int count);
void save_log_off(uid_struct far *uid, uid_struct far *head, long disconnect_time, int reason);
void alldtr_off(uid_struct far *head);
void far *str2farp(char far *str);
void waitsec(int sec, uid_struct far *head);
unsigned waitmsg(unsigned timeout, uid_struct far *head);
void lineon(char far *device_name);
void lineoff(char far *device_name);
int checkopt(char far *s, long flag, uid_struct far *head);
int search_board(boardlist far *blist, char far *bname, config_t far *conf);
int save_boardlist(boardlist far *blist, char far *bname, config_t far *conf);
void cputs(char far *s, char far *color, uid_struct far *uid);
void filecopy_h(int src, int dst);
void filecopy_n(char far *src, char far *dst);
void dispdate(long t);
void disptime(long t);
void lockbbs(uid_struct far *uid, uid_struct far *head);
void unlockbbs(uid_struct far *uid, uid_struct far *head);
int cquery(char far *msg, int def, uid_struct far *uid);

#endif	/* ULIB_H */

