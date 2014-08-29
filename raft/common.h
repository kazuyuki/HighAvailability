#include <unistd.h>
#include <time.h>
#include <sys/select.h>

enum {
	MSG_VOTE_REQUEST,
	MSG_VOTE_REPLY,
	MSG_HB_REQUEST,
	MSG_HB_REPLY
};

enum { FOLLOWER, CANDIDATE, LEADER };

struct msg {
	int	node;
	int	content;
};

int role;

int	sd;
struct	sockaddr_in	addr;
fd_set	fds, readfds;
int	maxfd;
int	currentTerm;

#ifndef EXTERN
extern int	ID;
extern int	NUMNODE;
extern char*	destaddr[3];
extern int	destport[]; 
extern int	TIMEOUT;
/*
extern int	sd;
extern struct	sockaddr_in	addr;
extern fd_set	fds, readfds;
extern int	maxfd;
*/
#else

int	ID;
int	NUMNODE;
char*	destaddr[] = {
	"127.0.0.1",
	"127.0.0.1",
	"127.0.0.1"
};

int	destport[] = {
	11111,
	11112,
	11113
};

int	TIMEOUT	= 6;

#endif

// main.c
int init_raft(char*);

// follower.c
int follower(void);
int follower_recv(void);

// candidate.c
int candidate(void);
int request_vote(void);
int candidate_recv(void);

// leader.c
int leader(void);
