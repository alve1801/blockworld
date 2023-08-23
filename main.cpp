#include <stdio.h>
#include <vector>
using namespace std;
#include "libv2.h"
#include "perlin.h"

bool gravity=0,drawwisps=0;

#include "worldgen.h"
#include "basetypes.h"
#include "graphics.h"
#include "multiplayer.h"

#include "basetypes.cpp"
#include "multiplayer.cpp"

int main(int argc,char**argv){
	generate_lookup();
	if(argc>1){
	if(argv[1][0]=='s'){
		printf("initialising server instance\n");
		serv.init();
		printf("server: listening\n");
		for(;;)
			serv.update();
		// thisll have to be manually terminated, so no return statement
	}else if(argv[1][0]=='c'){
		printf("initialising client instance\n");
		client c;
		cli.init();
		printf("client id %i\n",cli.p.id);
		// game window from generic, but with c.update() added to the logic

		Framework w(640,480,2);
		w.userfunc=[](Framework*w){
			if(w->keyp(SDL_SCANCODE_ESCAPE))return false;

			float inc=.25;
			if(w->keyh(SDL_SCANCODE_LSHIFT))inc=2;

			cli.p.dir.x=cli.p.dir.y=0;
			float front=0,right=0;
			if(w->keyh(SDL_SCANCODE_W))front=-inc;
			if(w->keyh(SDL_SCANCODE_S))front=inc;
			if(w->keyh(SDL_SCANCODE_D))right=inc;
			if(w->keyh(SDL_SCANCODE_A))right=-inc;
			float dx=-sinf(cli.p.yaw/40.74);
			float dy=cosf(cli.p.yaw/40.74);
			cli.p.dir.x+=dx*front+dy*right;
			cli.p.dir.y+=dy*front-dx*right;

			if(gravity){
			if(w->keyh(SDL_SCANCODE_SPACE))cli.p.jmp=1;
			}else{
			if(w->keyh(SDL_SCANCODE_SPACE))cli.p.loc.z-=inc;
			if(w->keyh(SDL_SCANCODE_LCTRL))cli.p.loc.z+=inc;
			}

			inc*=8;
			if(w->keyh(SDL_SCANCODE_J))cli.p.pitch+=inc;
			if(w->keyh(SDL_SCANCODE_K))cli.p.pitch-=inc;
			if(cli.p.pitch<0)cli.p.pitch=0;
			if(cli.p.pitch>256)cli.p.pitch=256;
			inc*=.5;
			if(w->keyh(SDL_SCANCODE_H))cli.p.yaw-=inc;
			if(w->keyh(SDL_SCANCODE_L))cli.p.yaw+=inc;
			cli.p.yaw=mod(cli.p.yaw,256);

			if(w->keyp(SDL_SCANCODE_U))m.raycast(cli.p.loc,cli.p.pitch,cli.p.yaw,0);
			if(w->keyp(SDL_SCANCODE_I))m.raycast(cli.p.loc,cli.p.pitch,cli.p.yaw,1);
			if(w->keyp(SDL_SCANCODE_G))gravity^=1;

			for(int x=-2;x<3;x++)for(int y=-2;y<3;y++)
			//if(x==0||y==0)
			if(x*y==0)
			w->newmap[(w->_sy/2+y)*w->_sx+w->_sx/2+x]^=0xfff;

			if(w->keyp(SDL_SCANCODE_O))printf("%.2f %.2f %.2f %i %i\n",
				cli.p.loc.x,
				cli.p.loc.y,
				cli.p.loc.z,
				cli.p.pitch,
				cli.p.yaw
			);

			img screen(640,480);
			screen.clear();
			render(&m,&cli.p,&screen);
			w->pi(screen,0,0);

			cli.update(); // also updates player

			if(w->keyp(SDL_SCANCODE_P))w->snap();
			return true;
		};

		w.run();

		return 0;
	}}

	printf("no mode specified, assuming generic demo\n");
	#include "generic.cpp"
	return 0;
}
