#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main (int argc, char** argv){
	
	int ret = 0;
	int PORT = 11110;
	struct sockaddr_in	addr;
	int sd = socket(AF_INET, SOCK_DGRAM, 0);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = (PORT);
	printf("[D] sending = [%s] length = [%d]\n", argv[1], strlen(argv[1]));
	ret = sendto(sd, argv[1], strlen(argv[1]), 0, (struct sockaddr *) &addr, sizeof(addr));
	if (ret == -1){
		perror("[E] CLIENT sendto");
	} else {
		printf("[I] CLIENT sendto\n");
	}
}
