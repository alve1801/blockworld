// XXX this still has values for world height/width, do away with that
// looking at it, it only seems to reference world height for planes

unsigned char map(unsigned char x){
	float tmp=x/128.0-1;
	return (tmp/(1+fabs(tmp))+.5)*256;
}
unsigned char lookup[256];
void generate_lookup(){
	for(int i=0;i<256;i++)lookup[i]=map(i);
	return;
	for(int i=0;i<256;i+=4){
	for(int j=0;j<256;j+=4)
	putchar(j<lookup[i]?'#':'.');
	putchar(10);
	}
}

int clip(int x){if(x<0)return 0;if(x>256)return 256;return x;}

bool generate_block(int x,int y,int z){
	int funk_falloff = 2;
	int funk = perlin::perl(x,y,z,6)-z*funk_falloff-128;

	int plane_scale = 2;
	int plane_base = perlin::perl(x,y,8)*plane_scale;
	plane_base>>=8;
	int plane = plane_base - z;

	//return plane<0;

	int biome_base = perlin::perl(x,y,16);

	//biome_base*=.7; // ~77
	biome_base-=64;
	if(biome_base<0)biome_base=0;

	for(int i=0;i<8;i++)biome_base=lookup[biome_base];

	biome_base = 256-biome_base;

	//return biome_base<64+z*16;

	int biome_bias_val = 256;
	int biome_bias_scale = 0;

	int biome = biome_base*(256-biome_bias_scale) + biome_bias_val*biome_bias_scale;
	biome>>=8;

	int val = plane*biome + funk*(256-biome);
	val>>=8;
	return val<0;
}

/*
void generate_terrain(){
	int z=0;
	// this is just an iterator
	for(int x=0;x<sh;x++){
	printf("%i/%i\n",x,sh);
	for(int y=0;y<sh;y++)
	for(int z=0;z<sv;z++)
	//data[(x*sh+y)*sv+z]=generate_block(x,y,z);
	at(x,y,z)=generate_block(x,y,z);
	}
}
*/
