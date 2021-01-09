#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <stdlib.h>

enum {
	MSG_VOTE_REQUEST,
	MSG_VOTE_REPLY,
	MSG_HB_REQUEST,
	MSG_HB_REPLY
};

enum { FOLLOWER, CANDIDATE, LEADER };

struct msg {
	int	node;
	int	term;
	int	content;
};

int role;

int	sd;
struct	sockaddr_in	addr;
fd_set	fds, readfds;
int	maxfd;

// Persistent state on all servers
int	currentTerm;
int	votedFor;
int	cmtlog[10];

// Volatile state on all servers
int	commitIndex;
int	lastApplied;

// Volatile state on leaders
int	nextIndex;
int	matchIndex;

#ifndef EXTERN
extern int	ID;
extern int	NUMNODE;
extern char*	destaddr[3];
extern int	destport[]; 
extern int	TO_MIN;
extern int	TO_MAX;

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

int	TO_MIN = 100000;
int	TO_MAX = 900000;

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
