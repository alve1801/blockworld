#define sidebits 4
#define sidelen 16
#define svol sidelen*sidelen*sidelen

// fuck stdlib
float modf(float a,float b=1){a=fmod(a,b);return a>=0?a:b+a;}

struct v3{
	int x,y,z;
	v3(int x=0,int y=0,int z=0):x(x),y(y),z(z){}
	// having some kind of generalised map operator sure would be fucking neat huh...
	v3 operator*(int a){return v3(x*a,y*a,z*a);}
	v3 operator+(v3 a){return v3(x+a.x,y+a.y,z+a.z);}
	v3 operator>>(int a){return v3(x>>a,y>>a,z>>a);}
	bool operator==(v3 a){return x==a.x && y==a.y && z==a.z;}
	int index(){return (x*sidelen+y)*sidelen+z;}
	void p(){printf("%i %i %i\n",x,y,z);}
};

// XXX only used for player position - replace w/ fixed point?
struct v3f{
	float x,y,z;
	v3f(float x=0,float y=0,float z=0):x(x),y(y),z(z){}
	v3f(v3 a):x(a.x),y(a.y),z(a.z){}
	v3f operator+(v3f a){return v3f(x+a.x,y+a.y,z+a.z);}
};

v3 f3(v3f a){return v3(floorf(a.x),floorf(a.y),floorf(a.z));}

struct Wisp{v3f loc;short col;};vector<Wisp>wisps; // for visual debugging

struct block{
	int type;
	bool solid=1;
	block(int t=0):type(t){}
	operator bool(){return type!=0;}
};

short block_cols[]={
	0,0,
	0x4f8,0x863,
	0x666,0x444
};

struct Chunk{
	v3 loc;
	block data[svol];
	Chunk(v3 l):loc(l){generate();}

	void generate();
};

struct World{
	vector<Chunk>data;
	char*savename;

	World(){}

	// XXX should be able to generate new chunks as the player wanders around, or optionally request/receive them from somewhere else

	void generate();
	Chunk*getchunk(v3 loc);
	block getblock(v3 loc);
	bool setblock(block b,v3 loc);
	v3 raycast(v3f loc,int pitch,int yaw,bool offset=0);
};

World m; // since all versions inevitably use this, we might as well put it here

struct Player{
	char id; // for server connections
	v3f loc; // XXX consider making this fixed-point
	int pitch,yaw;
	v3f dir;
	bool jmp=0;
	Player(int t=0):id(t),loc(v3f(.2,.4,-5.75)),pitch(60),yaw(50){}

	void update(World&w);
};
