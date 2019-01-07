
#if !defined(CHAT_T)
# define CHAT_T

#define CHAT_TITLE_MAX	40

#define CHAT_CLOSED		0x0001

typedef struct {
	unsigned status;
	char title[CHAT_TITLE_MAX + 1];
} chatroom_t;

#endif	/* CHAT_T */

