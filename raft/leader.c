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
	struct sockaddr_in	dest;
	struct msg m;

	dest.sin_family = AF_INET;
	while(1){
		m.node	= ID;
		m.content	= MSG_HB_REQUEST;
		for(i=0; i<NUMNODE; i++){
			dest.sin_addr.s_addr = inet_addr(destaddr[i]);
			dest.sin_port = htons(destport[i]);
			sendto(sd, &m, sizeof(m), 0, (struct sockaddr*)&dest, sizeof(dest));
			printf("[D] LEADER sent [%d] to node [%d]\n", m.content, i);
		}
		while(1){
			struct timeval tv;

			tv.tv_sec	= TIMEOUT/2;
			tv.tv_usec	= 0;

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
