TARGET = krc

.c.o:
	cc -c $<

SRC = main.c follower.c candidate.c leader.c
OBJ = $(SRC:%.c=%.o)
krc: $(OBJ)
	cc -o $@ $^

ALL: krc

#krc:	main.c follower.c candidate.c leader.c
#	cc -o $@ $<

client: client.c

	cc -o $@ $<

clean:
	rm -rf $(TARGET) client *.o *~ 
