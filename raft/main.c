#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define EXTERN
#include "common.h"

int main(int argc, char **argv)
{
	init_raft(argv[1]);

	while (1) {
		switch (role) {
		case FOLLOWER:
			follower();
			break;
		case CANDIDATE:
			candidate();
			break;
		case LEADER:
			leader();
			break;
		default:
			break;
		}
	}
	return 0;
}

int init_raft(char *id)
{
	ID = atoi(id);
	NUMNODE = sizeof(destaddr) / sizeof(destaddr[0]);

	role = FOLLOWER;
	currentTerm = 0;

	sd = socket(AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(destaddr[ID]);
	addr.sin_port = htons(destport[ID]);
	if(0 != bind(sd, (struct sockaddr *) &addr, sizeof(addr))){
		return -1;
	}
	return 0;
}
