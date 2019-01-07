
#if !defined(TELEGRAM_H)
# define TELEGRAM_H

#define TELEMSG_MAX		78

typedef struct {
	unsigned from_ch;
	char from_id[ID_MAX + 1];
	char from_handle[HANDLE_MAX + 1];
	long time_stamp;
	char message[TELEMSG_MAX + 1];
} telegram_t;


#endif	/* TELEGRAM_H */

