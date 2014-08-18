#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "common.h"

int follower()
{
	int	INTERVAL	= 1;

	while(1){
		if(!hbrecv()){
			/* timeout */
			role = CANDIDATE;
			printf("[I] became CANDIDATE\n");
			break;
		}
	}
	return 0;
}

int hbrecv()
{
	FD_ZERO(&readfds);
	FD_SET(sd, &readfds);
	tv.tv_sec	= TIMEOUT;
	int n;

	while(1){
		memcpy(&fds, &readfds, sizeof(fd_set));
		n = select(sd+1, &fds, NULL, NULL, &tv);
		if (n == 0){
			/* time out */
			printf("[D][%s:%d] timeout\n", __FILE__, __LINE__);
			return 0;
		}
		if(FD_ISSET(sd, &fds)){
			memset(buf, 0, sizeof(buf));
			recv(sd, buf, sizeof(buf), 0);
			/* need to add recieving HB and VOTE */
			return 1;
		}
	}
}

