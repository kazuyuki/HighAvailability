#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"

int leader()
{
	int	i, n;
	struct msg m;

	while(1){
		m.node = ID;
		m.term = currentTerm;
		m.content = MSG_HB_REQUEST;
		for(i=0; i<NUMNODE; i++){
			addr.sin_addr.s_addr = inet_addr(destaddr[i]);
			addr.sin_port = htons(destport[i]);
			sendto(sd, &m, sizeof(m), 0, (struct sockaddr*)&addr, sizeof(addr));
			printf("[D] LEADER term [%d] sent [%d] to node [%d]\n", m.term, m.content, i);
		}
		while(1){
			struct timeval tv;

			tv.tv_sec	= 3/2;
			tv.tv_usec	= (TO_MIN + (rand() * (TO_MAX - TO_MIN) / RAND_MAX))/2;

			memcpy(&fds, &readfds, sizeof(fd_set));
			n = select(sd+1, &fds, NULL, NULL, &tv);
			if(n == 0){
				/* time out */
				printf("[D] LEADER nothing to recv\n");
				break;
			}
			else if(FD_ISSET(sd, &fds)){
				m.node = m.content = -1;
				recv(sd, &m, sizeof(m), 0);
				printf("[D] LEADER received [%d] from node [%d]\n", m.content, m.node);
			}
		}
		//sleep(TIMEOUT/2);
	}
	return 0;
}
