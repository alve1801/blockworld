	m.generate();
	Framework w(640,480,2);
	w.userfunc=[](Framework*w){
		if(w->keyp(SDL_SCANCODE_ESCAPE))return false;

		static Player p; // idk man

		float inc=.25;
		if(w->keyh(SDL_SCANCODE_LSHIFT))inc=2;

		p.dir.x=p.dir.y=0;
		float front=0,right=0;
		if(w->keyh(SDL_SCANCODE_W))front=-inc;
		if(w->keyh(SDL_SCANCODE_S))front=inc;
		if(w->keyh(SDL_SCANCODE_D))right=inc;
		if(w->keyh(SDL_SCANCODE_A))right=-inc;
		float dx=-sinf(p.yaw/40.74);
		float dy=cosf(p.yaw/40.74);
		p.dir.x+=dx*front+dy*right;
		p.dir.y+=dy*front-dx*right;

		if(gravity){
		if(w->keyh(SDL_SCANCODE_SPACE))p.jmp=1;
		}else{
		if(w->keyh(SDL_SCANCODE_SPACE))p.loc.z-=inc;
		if(w->keyh(SDL_SCANCODE_LCTRL))p.loc.z+=inc;
		}

		inc*=8;
		if(w->keyh(SDL_SCANCODE_J))p.pitch+=inc;
		if(w->keyh(SDL_SCANCODE_K))p.pitch-=inc;
		if(p.pitch<0)p.pitch=0;
		if(p.pitch>256)p.pitch=256;
		inc*=.5;
		if(w->keyh(SDL_SCANCODE_H))p.yaw-=inc;
		if(w->keyh(SDL_SCANCODE_L))p.yaw+=inc;
		p.yaw=mod(p.yaw,256);

		if(w->keyp(SDL_SCANCODE_U))m.raycast(p.loc,p.pitch,p.yaw,0);
		if(w->keyp(SDL_SCANCODE_I))m.raycast(p.loc,p.pitch,p.yaw,1);
		if(w->keyp(SDL_SCANCODE_G))gravity^=1;

		img screen(640,480);
		screen.clear();
		render(&m,&p,&screen);
		w->pi(screen,0,0);

		//w->p(0xfff,w->_sx/2,w->_sy/2);
		for(int x=-2;x<3;x++)
		for(int y=-2;y<3;y++)
		if(x==0 || y==0)
		w->newmap[(w->_sy/2+y)*w->_sx+w->_sx/2+x]^=0xfff;

		p.update(m);

		if(w->keyp(SDL_SCANCODE_O))printf("%.2f %.2f %.2f %i %i\n",
p.loc.x,
p.loc.y,
p.loc.z,
p.pitch,
p.yaw
);
		if(w->keyp(SDL_SCANCODE_P))w->snap();
		return true;
	};
	w.run();

