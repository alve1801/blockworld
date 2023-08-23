//#include "multiplayer.h"

void server::init(){
	// inits world - what does that entail?
	// init server-side conns
	fd=socket(AF_INET,SOCK_STREAM,0);

	struct sockaddr_in addr;
	addr.sin_addr.s_addr = INADDR_ANY; // since we only accept anyway
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080); // converts port

	bind(fd,(struct sockaddr*)&addr,sizeof(addr));
	listen(fd,4);
}

void server::update(){
	//int connfd=accept(fd,sockaddr*cliaddr,int cliaddrlen); // XXX dont think we actually need those tbh

	char buf[bufsize];

	int connfd=accept(fd,0,0);
	int datalen=read(connfd,buf,bufsize); // i sure hope this returns prematurely
	int user_id=buf[0];

	printf("server: packet of type %i from user %i\n",buf[1],user_id);

	Chunk*c;

	switch(buf[1]){
		case 0:
			// 0 - request to join. init player in memory, respond w/ 1 + id and location
			printf("assigning player id %i\n",max_player);
			chat[max_player]=new Player(max_player);
			memset(buf,0,bufsize);
			buf[1]=1;
			buf[2]=max_player++;
			datalen=9;
			break;

		case 2:
			// 2 - location update. body contains new location of player. ignore for now.
			printf("location update (ignoring)\n");
			close(connfd);
			return;

		case 3:
			// 3 - chunk update. contains a vector w/ location and chunk data. update state and forward to all other players
			printf("chunk update for chunk ");
			c=m.getchunk(v3(buf[2]*256+buf[3],buf[4]*256+buf[5],buf[6]*256+buf[7]));
			c->loc.p();
			for(int i=0;i<svol;i++)
				c->data[i].type=buf[8+i];

			// XXX forward
			close(connfd);
			return;

		case 4:
			// 4 - chunk request. body contains vector. respond w/ 3 + chunk location + chunk data. might have to generate.
			printf("chunk request for chunk ");
			buf[1]=3;
			c=m.getchunk(v3(buf[2]*256+buf[3],buf[4]*256+buf[5],buf[6]*256+buf[7]));
			c->loc.p();
			for(int i=0;i<svol;i++)
				buf[8+i]=c->data[i].type;
			datalen=8+svol;
			break;

		default:
			printf("Request code %i unknown\n",buf[1]);
			close(connfd);
			return;
	}

	write(connfd,buf,datalen);
	close(connfd);
}

void client::init(){
	//struct sockaddr_in addr;
	inet_pton(AF_INET,"127.0.0.1", &addr.sin_addr); // converts address
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080); // converts port

	// send a 0-0 to server
	buf[0]=buf[1]=0;
	mesg(2);

	// response tells us our id and location (for init'ing player)
	p.id=buf[2];
	p.loc=v3(buf[3]*256+buf[4],buf[5]*256+buf[6],buf[7]*256+buf[8]);

	// send a couple id-4-vec to ask for chunks around spawn

}

int client::mesg(int len){
	// sends len bits of buffer to fd
	// records response back in buf (buf must be able to hold it)
	// returns response length
	int fd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(fd,(struct sockaddr*)&addr,sizeof(addr))==-1){
		printf("couldn't connect to host\n");
		// XXX hcf
	}
	write(fd,buf,len);
	int res=read(fd,buf,256); // XXX
	close(fd);
	return res;
}

void client::update(){
	// send id-3-loc to server - or not, havent implemented them anyway
	//  check if weve even moved?
	// if we place/destroy a block, send an id-3-loc-data to server
	//  checked how?
	// if we enter a new chunk, send an id-4-loc
	//  more generally, check if player surroundings need to be updated, and do so if needed?
	// separately (how?), listen to 0-3-loc-data from server, and update chunks accordingly
	//  XXX this gon be a lot harder than expected...

	p.update(m);
}

