//#include "basetypes.h"

void Chunk::generate(){
	for(int bx=0;bx<sidelen;bx++)
	for(int by=0;by<sidelen;by++)
	for(int bz=0;bz<sidelen;bz++)
	data[v3(bx,by,bz).index()]=block(generate_block(loc.x*sidelen+bx,loc.y*sidelen+by,loc.z*sidelen+bz));
}

void World::generate(){
	for(int x=-2;x<3;x++)
	for(int y=-2;y<3;y++)
	for(int z=-2;z<1;z++)
	data.push_back(Chunk(v3(x,y,z)));
}

Chunk*World::getchunk(v3 loc){
	for(Chunk&c:data)
		if(c.loc==loc)
			return&c;
	//return 0;
	data.push_back(Chunk(loc));
	return&data.back();
}

block World::getblock(v3 loc){
	printf("getblock ");loc.p();
	printf("   chunk ");(loc>>sidebits).p();
	Chunk*c=getchunk(loc>>sidebits);
	if(!c)return block();
	loc = v3(mod(loc.x,sidelen),mod(loc.y,sidelen),mod(loc.z,sidelen));
	printf("   block ");loc.p();
	return c->data[loc.index()];
}


bool World::setblock(block b,v3 loc){
	Chunk*c=getchunk(loc>>sidebits);
	if(!c)return 0;
	loc = v3(mod(loc.x,sidelen),mod(loc.y,sidelen),mod(loc.z,sidelen));
	c->data[loc.index()]=b;
	return 1;
}

v3 World::raycast(v3f loc,int pitch,int yaw,bool offset){
	/*
	if nblock==0, destroys the block were looking at
	else places a block of type nblock on the face were looking at
	p.pitch and p.yaw define a direction - well have to use trigs to convert that into a direction vector
	using that, we can raycast along each axis and find the nearest block, then take the min of that
		or fail - well need a maxrange
	and once we have that, we switch depending on the input

	XXX should be possible to simplify this
	also better interface bc rn we cant place blocks

	ok wow fuck it mostly works now, that took me a whole fucking day
	its still off by a bit, but idk it usually points at the right block now (was an error with how c casts negative values), and i aint arsed to figure out what the remaining error is
	*/

	// dx dy dz specify the delta vector, dt is needed for the trigonometrics
	float dx,dy,dz,dt;
	sincosf(pitch/81.49,&dt,&dz);
	sincosf((128-yaw)/40.74,&dx,&dy);
	dx*=dt,dy*=dt;

	// norm_dir is the delta vector normalised for each axis
	//  norm_dir = dir * pos / dD
	// acc is the accumulated vector, aka the ray being cast
	//  starts at face nearest input position, gets incremented by norm_dir
	// pos (short for positive) tells us whether an axis needs to be inverted
	// tinc is the factor of the initial increment
	v3f norm_dir,acc;float pos,tinc;

	// nD is the index of the block being tested (acc is v3f, we need v3)
	v3 nx,ny,nz;

	// collD tells us whether we have collided XXX its inverted
	bool collx=1,colly=1,collz=1;

	// Ddist stores the accumulated distance (squared)
	float xdist,ydist,zdist;

	// maximal distance we can affect blocks at
	const float range=49; // 7^2

	{
	printf("\n\nraycast %.2f %.2f %.2f\n",dx,dy,dz);
	wisps.clear();
	wisps.push_back((Wisp){loc,0xfff});

	v3 dloc=f3(loc);
	//dloc.z-=1;
	wisps.push_back((Wisp){v3f(dloc.x,dloc.y,dloc.z),0x888});

	wisps.push_back((Wisp){v3f(dloc.x,dloc.y,dloc.z+1),0x888});
	wisps.push_back((Wisp){v3f(dloc.x,dloc.y+1,dloc.z),0x888});
	wisps.push_back((Wisp){v3f(dloc.x+1,dloc.y,dloc.z),0x888});

	wisps.push_back((Wisp){v3f(dloc.x+1,dloc.y+1,dloc.z),0x888});
	wisps.push_back((Wisp){v3f(dloc.x+1,dloc.y,dloc.z+1),0x888});
	wisps.push_back((Wisp){v3f(dloc.x,dloc.y+1,dloc.z+1),0x888});

	wisps.push_back((Wisp){v3f(dloc.x+1,dloc.y+1,dloc.z+1),0x888});
	}

	// X
	printf("x:\n");
	pos=dx>0?1:-1;
	norm_dir=v3f(pos,pos*dy/dx,pos*dz/dx);
	tinc = 1-modf(loc.x);if(dx<0)tinc=tinc-1; // can prolly be simplified
	acc = v3f(loc.x+tinc,loc.y+tinc*dy/dx,loc.z+tinc*dz/dx);
	xdist = (dy*dy+dz*dz)*(tinc*tinc)/(dx*dx);
	while(collx && xdist<range){
		wisps.push_back((Wisp){acc,0xf00});
		nx=f3(acc); // XXX
		wisps.push_back((Wisp){nx,0xf44});
		printf("rounded:\n%.2f %.2f %.2f\n",acc.x,acc.y,acc.z);nx.p();

		if(getblock(nx))collx=0;
		else{
			acc=acc+norm_dir;
			xdist+=1+norm_dir.y*norm_dir.y+norm_dir.z*norm_dir.z;
		}
	}
	if(offset)nx.x-=pos;
	printf("nx:");nx.p();

	// Y
	printf("\n\ny:\n");
	pos=dy>0?1:-1;
	norm_dir=v3f(pos*dx/dy,pos,pos*dz/dy);
	tinc = 1-modf(loc.y);if(dy<0)tinc=tinc-1;
	acc = v3f(loc.x+tinc*dx/dy,loc.y+tinc,loc.z+tinc*dz/dy);
	ydist = (dx*dx+dz*dz)*(tinc*tinc)/(dy*dy);
	while(colly && ydist<range){
		wisps.push_back((Wisp){acc,0x0f0});
		ny=f3(acc);
		wisps.push_back((Wisp){ny,0x4f4});
		if(getblock(ny))colly=0;
		else{
			acc=acc+norm_dir;
			ydist+=norm_dir.x*norm_dir.x+1+norm_dir.z*norm_dir.z;
		}
	}
	if(offset)ny.y-=pos;
	printf("ny:");ny.p();

	// Z
	printf("\n\nz:\n");
	pos=dz>0?1:-1;
	norm_dir=v3f(pos*dx/dz,pos*dy/dz,pos);
	tinc = 1-modf(loc.z);if(dz<0)tinc=tinc-1;
	acc = v3f(loc.x+tinc*dx/dz,loc.y+tinc*dy/dz,loc.z+tinc);
	zdist = (dx*dx+dy*dy)*(tinc*tinc)/(dz*dz);
	while(collz && zdist<range){
		wisps.push_back((Wisp){acc,0x00f});
		nz=f3(acc);
		wisps.push_back((Wisp){nz,0x44f});
		if(getblock(nz))collz=0;
		else{
			acc=acc+norm_dir;
			zdist+=norm_dir.x*norm_dir.x+norm_dir.y*norm_dir.y+1;
		}
	}
	if(offset)nz.z-=pos;
	printf("\nnz:");nz.p();

	printf("distances: x%.2f y%.2f z%.2f\n",xdist,ydist,zdist);

	v3 res;
	float mdist=range+1;
	if(!collx && xdist<mdist)mdist=xdist,res=nx,printf("x\n");
	if(!colly && ydist<mdist)mdist=ydist,res=ny,printf("y\n");
	if(!collz && zdist<mdist)mdist=zdist,res=nz,printf("z\n");
	if(mdist>range)return 0;

	printf("block ");res.p();
	wisps.push_back((Wisp){res,0xfff});

#if 0
does not extrude downwards - iow +z
-z works


	char*worlddata=(char*)malloc(rend*rend*rend*8);
	worlddata=getrange(worlddata,qvi(p.loc),rend);
	Q delta,current,corr=corr=p.loc.round()-Q(rend,rend,rend);
	Q rot=Q(0,1,0).rot(p.rot.inv()) *-1;
	Q depth=Q(1000,1000,1000);
	vi t; // struct{int[3]}
	// qvi uses floorf

	// X
	delta=rot*((rot.x>0?1:-1)/rot.x);
	current=p.loc+delta*modf(p.loc.x,1)*(delta.x>0?-1:1);
	while((p.loc-current).sqabs()<rend*rend){
	t=qvi(current-corr);
	if(delta.x<0)t.x--;
	if(ind(worlddata,rend,t)){
	depth.x=(p.loc-current).abs();
	break;
	}
	current+=delta;
	}
#endif

	// XXX debug, keep this separate
	if(offset)setblock(block(2),res);
	else setblock(block(),res);

	return res;
}

void Player::update(World&w){
	if(!gravity){
		dir.z=0;
		loc=loc+dir;
		return;
	}

	v3 ploc(loc.x,loc.y,loc.z+1.5);
	if(!w.getblock(ploc))dir.z+=.1; // gravity
	else if(jmp)dir.z=-1; // jumping force
	else dir.z=0;
	jmp=0;

	/* if were in a block (XXX tested how?), move towards the nearest (horisontally) empty block
	ploc=v3(loc.x+.5,loc.y,loc.z);
	if(getblock(ploc))dir.x-=.5; // etc */

	// *2 so we dont go into the block
	// ideally, replace that w/ +-.5 for block thickness, but that makes the formula directional and thus more complicated
	ploc=v3(loc.x+dir.x*2,loc.y,loc.z+1);
	if(w.getblock(ploc))dir.x=0;// *=-1;

	ploc=v3(loc.x,loc.y+dir.y*2,loc.z+1);
	if(w.getblock(ploc))dir.y=0;// *=-1;

	loc=loc+dir;
}

