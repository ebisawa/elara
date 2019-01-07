
#include "klib.h"
#include "mlib.h"
#include "config.h"
#include "cfgdat.h"
#include "bbsdat.h"
#include "usrdat.h"
#include "logdat.h"
#include "ulib.h"
#include "telegram.h"

uid_struct far *Head;		/* ‚±‚ê‚Í‚Ç‚Ìƒ`ƒƒƒ“ƒlƒ‹‚Å‚à‹¤’Ê‚¾‚©‚ç‹¤—L‰Â */
void disp_rooms(void);
void disp_oneroom(int room);
int get_roomusers(int room);

void main(void)
{
	char far *argv;
	uid_struct far *uid;

	if ((Head = getuid_n("pwatch")) == 0)
		return;
	if ((uid = getuid(getpid())) == 0)
		return;

	argv = getargv();
	disp_rooms();
}

void disp_rooms(void)
{
	int i;

	for (i = 1; i <= Head->us.p.config->chatrooms; i++)
		disp_oneroom(i);
}

void disp_oneroom(int room)
{
	int i;

	if (get_roomusers(room) == 0)
		memset(&Head->us.p.chatroom[room - 1], 0, sizeof(chatroom_t));

	printf("Room [%d] : ", room);
	room--;
	if (i, Head->us.p.chatroom[room].status & CHAT_CLOSED)
		print("Closed");
	else
		print("Opened");

	printf(" ; Title \"%s\"\n", FAR(Head->us.p.chatroom[room].title));

	for (i = 0; i <= Head->us.p.maxusers; i++) {
		if (Head[i].chstat == CS_USING && Head[i].us.u.chat == room + 1)
			printf(" %-8s", FAR(Head[i].us.u.udat.id));
	}
	putchar('\n');
}

int get_roomusers(int room)
{
	int i, users = 0;

	for (i = 0; i <= Head->us.p.maxusers; i++) {
		if (Head[i].chstat == CS_USING && Head[i].us.u.chat == room)
			users++;
	}
	return users;
}



/*
Room [1] : Opened ; Title ""

Room [2] : Opened ; Title ""

enter room(1-2)/[q]uit : 1
room title : 
-- entering room 1 --
/e
-- echoback off --
-- hjn00001 comes in --
*/

