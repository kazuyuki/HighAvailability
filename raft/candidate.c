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
	if(candidate_recv()){
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
	struct msg m;

	m.node = ID;
	m.content = MSG_VOTE_REQUEST;
	for(i=0; i<NUMNODE; i++){ 
		addr.sin_addr.s_addr = inet_addr(destaddr[i]);
		addr.sin_port = htons(destport[i]); 
		sendto(sd, &m, sizeof(m), 0, (struct sockaddr *)&addr, sizeof(addr));
		printf("[D][%s:%d] CANDIDATE sent REQUEST VOTE to node[%d][%d]\n", __FILE__, __LINE__, i, MSG_VOTE_REQUEST);
	}
	return 0;
}

int candidate_recv()
{
	struct timeval	tv;

	FD_ZERO(&readfds);
	FD_SET(sd, &readfds);
	tv.tv_sec	= TIMEOUT;
	tv.tv_usec	= 0;
	int n;
	int votecnt	= 1;	// self vote
	struct msg	m;

	while(1){
		memcpy(&fds, &readfds, sizeof(fd_set));
		n = select(sd+1, &fds, NULL, NULL, &tv);
		if (n == 0){
			/* time out */
			printf("[D][%s:%d] CANDIDATE timeout\n", __FILE__, __LINE__);
			if (votecnt > NUMNODE/2){
				return 1;
			} else {
				return 0;
			}
		}

		/* Should I reply to heartbeat? */
		if(FD_ISSET(sd, &fds)){
			m.node = -1;
			m.content = -1;
			recv(sd, &m, sizeof(m), 0);
			printf("[D][%s:%d] CANDIDATE received [%d] from node [%d]\n", __FILE__, __LINE__, m.content, m.node);
			if(m.content == MSG_VOTE_REPLY){
				votecnt++;
				printf("[I] Candidate received vote.	count = [%d]\n", votecnt);
				if(votecnt > NUMNODE/2){
					printf("[I] Candidate got quorum.	count = [%d]\n", votecnt);
					return 1;
				}
			}
			else if(m.content == MSG_VOTE_REQUEST){
				printf("[D] CANDIDATE received and ignored vote request.\n");
			}
			else {
				printf("[E] CANDIDATE received unknown message.\n");
			}
		}
	}
	return 0;
}
