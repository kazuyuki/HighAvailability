#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"

int follower()
{
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
	struct sockaddr_in sa;
	socklen_t	al;
	int n;

	FD_ZERO(&readfds);
	FD_SET(sd, &readfds);
	tv.tv_sec	= TIMEOUT;

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
			/* need to add recieving/replying HB */
			recvfrom(sd, buf, sizeof(buf), 0, (struct sockaddr*)&sa, &al);
			printf("[D][%s:%d] FOLLOWER received from %s:%d\n", __FILE__, __LINE__, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
			if(!strcmp(buf, MSG_VOTE_REQUEST)){
				/* Replying vote */
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), MSG_VOTE_REPLY);
				printf("[D][%s:%d] reply to %s:%d %s\n", __FILE__, __LINE__, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port), buf);
				sendto(sd, buf, strlen(buf),0, (struct sockaddr*)&sa, al);
				printf("[I] responsed for vote.\n");
				continue;
			}
			else if(!strcmp(buf, MSG_HB_REQIEST)){
				/* Replying heartbeat */
				memset(buf, 0, sizeof(buf));
				snprintf(buf, sizeof(buf), MSG_HB_REPLY);
				sendto(sd, buf, strlen(buf),0, (struct sockaddr*)&sa, al);
				printf("[D] responsed for heartbeat.\n");
				continue;
			}
			return 1;
		}
	}
}

