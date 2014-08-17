#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "common.h"

int candidate()
{
	request_vote();

	/* need to add proccessing response for vote */
	if(check_vote()){
		role = LEADER;
		printf("[I] became LEADER\n");
	} else {
		role = FOLLOWER;
		printf("[I] became FOLLOWER\n");
	}
	return 0;
}

int request_vote()
{
	int	i;
	struct sockaddr_in	dest;
	
	dest.sin_family = AF_INET;
	for(i=0; i<sizeof(destaddr)/sizeof(destaddr[0]); i++){ 
		dest.sin_addr.s_addr = inet_addr(destaddr[i]);
		dest.sin_port = htons(destport[i]); 
		memset(buf, 0, sizeof(buf));  
		snprintf(buf, sizeof(buf), "request vote");
		sendto(sd, buf, strlen(buf), 0, (struct sockaddr *)&dest, sizeof(dest));
		printf("[D][%s:%d] sent REQUEST VOTE to [%s:%d]\n", __FILE__, __LINE__, destaddr[i], destport[i]);
	}
	return 0;
}

int check_vote()
{
	FD_ZERO(&readfds);
	FD_SET(sd, &readfds);
	tv.tv_sec	= TIMEOUT;
	int n;
	int votecnt	= 1;	/* self vote */

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
			if(strcmp(buf, MSG_VOTE_OK)){
				votecnt++;	
				printf("[I] vote count = [%d]\n", votecnt);
			}
			if(votecnt > NUMNODE/2){
				/*******************
				need to do something
				********************/
				return 1;
			}
		}
	}
	return 0;
}
