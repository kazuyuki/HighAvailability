#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define EXTERN
#include "common.h"

int leader(){
	return 0;
}

int main (int argc, char** argv)
{
	init_raft(argv[1]);

	while(1){
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

int init_raft (char* id)
{
	sd = socket (AF_INET, SOCK_DGRAM, 0);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr	= inet_addr(MYADDR);
	//addr.sin_port	= htons(PORT);
	addr.sin_port	= htons(destport[atoi(id)]);
	bind(sd, (struct sockaddr *)&addr, sizeof(addr));

	NUMNODE = sizeof(destaddr)/sizeof(destaddr[0]);
	return 0;
}
