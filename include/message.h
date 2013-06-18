#include "types.h"

#ifndef __MESSAGE_H__
#define __MESSAGE_H__

#define ANY -1
#define MSG_HWINTR -2
#define MSG_SZ 128

struct Message {
	int type;
	pid_t src, dest;
	char payload[MSG_SZ];
};
typedef struct Message Message;

void send(pid_t dst, Message *m);
void receive(pid_t src, Message *m);

#endif
