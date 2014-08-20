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
		snprintf(buf, sizeof(buf), MSG_VOTE_REQUEST);
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
	int votecnt	= 0;

	while(1){
		memcpy(&fds, &readfds, sizeof(fd_set));
		n = select(sd+1, &fds, NULL, NULL, &tv);
		if (n == 0){
			/* time out */
			printf("[D][%s:%d] timeout\n", __FILE__, __LINE__);
			if (votecnt > NUMNODE/2){
				return 1;
			} else {
				return 0;
			}
		}

		/* Should I reply to heartbeat? */
		if(FD_ISSET(sd, &fds)){
			memset(buf, 0, sizeof(buf));
			recv(sd, buf, sizeof(buf), 0);
			printf("[D][%s:%d] %s\n", __FILE__, __LINE__, buf);
			if(0 == strcmp(buf, MSG_VOTE_REPLY)){
				votecnt++;
				printf("[I] Candidate received vote.	count = [%d]\n", votecnt);
				if(votecnt > NUMNODE/2){
					printf("[I] Candidate got quorum.	count = [%d]\n", votecnt);
					return 1;
				}
			}
			else if(0 == strcmp(buf, MSG_VOTE_REQUEST)){
				printf("[D] Candidate received and ignore vote request.\n");
			}
		}
	}
	return 0;
}
