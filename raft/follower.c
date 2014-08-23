#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"

int follower()
{

	printf("[D] **** Start FOLLOWER\n");
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
	ssize_t	ret;
	struct sockaddr_in sa;
	socklen_t	al;
	int n;
	int dst;
	struct msg	m;
	struct timeval	tv;

	FD_ZERO(&readfds);
	FD_SET(sd, &readfds);
	sa.sin_family	= AF_INET;

	while(1){
		tv.tv_sec	= TIMEOUT;
		tv.tv_usec	= 0;
		memcpy(&fds, &readfds, sizeof(fd_set));
		n = select(sd+1, &fds, NULL, NULL, &tv);
		if (n == 0){
			/* time out */
			printf("[W][%s:%d] FOLLOWER timeout\n", __FILE__, __LINE__);
			return 0;
		}
		if(FD_ISSET(sd, &fds)){
			m.node = m.content = -1;
			/* need to add recieving/replying HB */
			recvfrom(sd, &m, sizeof(m), 0, (struct sockaddr*)&sa, &al);
			//printf("[D][%s:%d] FOLLOWER received [%d] from node [%d]\n", __FILE__, __LINE__, m.content, m.node);
			dst = m.node;
			if(m.content == MSG_VOTE_REQUEST){
				/* Replying vote */
				m.node = ID;
				m.content = MSG_VOTE_REPLY;
				sa.sin_addr.s_addr = inet_addr(destaddr[dst]);
				sa.sin_port = htons(destport[dst]);
				ret = sendto(sd, &m, sizeof(m),0, (struct sockaddr*)&sa, sizeof(sa));
				if (ret == -1){
					perror("[E] FOLLOWER sendto 1");
				}
				printf("[I] FOLLOWER reply VOTE REPLY to node[%d][%d]\n", m.node, m.content);
				continue;
			}
			else if(m.content == MSG_HB_REQUEST){
				/* Replying heartbeat */
				m.node = ID;
				m.content = MSG_HB_REPLY;
				sa.sin_addr.s_addr = inet_addr(destaddr[dst]);
				sa.sin_port = htons(destport[dst]);
				ret = sendto(sd, &m, sizeof(m), 0, (struct sockaddr*)&sa, al);
				if (ret == -1){
					perror("[E] FOLLOWER sendto 2");
				}
				printf("[D] FOLLOWER responsed for HB.\n");
				continue;
			}
			else {
				printf("[E] FOLLOWER received unknown.\n");
				continue;
			}
			return 1;
		}
	}
}

