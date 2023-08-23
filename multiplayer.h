#include <sys/types.h>
#include <sys/socket.h>
#include<unistd.h>

// for resolving address
#include<netinet/in.h>
#include<arpa/inet.h>

const int bufsize = 1024+svol*sizeof(int);

struct server{
	Player*chat[256];
	int max_player=0;
	int fd;

	void init();
	void update();
};

server serv;

struct client{
	Player p;
	struct sockaddr_in addr;
	char buf[1024+svol*sizeof(int)];

	void init();
	int mesg(int len);
	void update();
};

client cli;
