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
	while (1) {
		if (!follower_recv()) {
			/* timeout */
			role = CANDIDATE;
			printf("[I] became CANDIDATE\n");
			break;
		}
	}
	return 0;
}

int follower_recv()
{
	ssize_t ret;
	socklen_t al;
	int n;
	int dest;
	struct msg m;
	struct timeval tv;

	FD_ZERO(&readfds);
	FD_SET(sd, &readfds);

	while (1) {
		tv.tv_sec = TIMEOUT;
		tv.tv_usec = 0;
		memcpy(&fds, &readfds, sizeof(fd_set));
		n = select(sd + 1, &fds, NULL, NULL, &tv);
		if (n == 0) {
			/* time out */
			printf("[W][%s:%d] FOLLOWER timeout\n", __FILE__, __LINE__);
			return 0;
		}
		if (FD_ISSET(sd, &fds)) {
			m.node = m.content = -1;
			//recvfrom(sd, &m, sizeof(m), 0, (struct sockaddr*)&addr, &al);
			if (-1 == recv(sd, &m, sizeof(m), 0)) {
				perror("[E] FOLLOWER recv");
			}
			//printf("[D][%s:%d] FOLLOWER received [%d] from node [%d]\n", __FILE__, __LINE__, m.content, m.node);

			addr.sin_addr.s_addr = inet_addr(destaddr[m.node]);
			addr.sin_port = htons(destport[m.node]);
			if (m.content == MSG_VOTE_REQUEST) {
				/*
				   Replying vote 
				   Receiver implementation:
				   1. Reply false if term < currentTerm (section 5.1)
				   2. If votedFor is null or candidateId, and candidate’s log is at
				      least as up-to-date as receiver’s log, grant vote 
				*/
				if ((m.term >= currentTerm) &&
				   ((m.node == votedFor) || (votedFor == -1))){
					votedFor = dest;
					m.content = MSG_VOTE_REPLY;
				}
			} else if (m.content == MSG_HB_REQUEST) {
				/* Replying heartbeat */
				m.content = MSG_HB_REPLY;
			} else {
				printf("[E] FOLLOWER received unknown.\n");
				continue;
			}

			/*
			   If RPC request or response contains term T > currentTerm:
			   set currentTerm = T, convert to follower (section 5.1)
			*/
			if(m.term > currentTerm){
				printf("[I] FOLLOWER changed currentTerm from [%d] to [%d]\n", currentTerm, m.term);
				currentTerm = m.term;
			}

			dest = m.node;
			m.node = ID;
			ret = sendto(sd, &m, sizeof(m), 0, (struct sockaddr *) &addr, sizeof(addr));
			if (ret == -1) {
				perror("[E] FOLLOWER sendto 1");
			}
			printf("[I] FOLLOWER term [%d] send [%d] to node [%d]\n", m.term, m.content, dest);
		}
	}
}
