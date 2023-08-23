#ifndef PERLIN
#define PERLIN

namespace crand{
	unsigned char rand[256]={57, 197, 122, 71, 115, 16, 215, 177, 76, 159, 24, 250, 13, 226, 85, 229, 196, 48, 157, 89, 183, 62, 26, 22, 135, 1, 33, 107, 187, 52, 218, 230, 140, 232, 64, 133, 151, 233, 27, 224, 248, 35, 149, 195, 20, 158, 243, 5, 11, 171, 111, 227, 145, 170, 213, 198, 130, 235, 201, 245, 160, 113, 138, 143, 247, 179, 112, 176, 220, 132, 193, 79, 66, 127, 60, 163, 162, 38, 56, 58, 199, 108, 29, 95, 106, 7, 168, 49, 208, 42, 169, 214, 131, 23, 17, 109, 43, 137, 123, 219, 65, 120, 94, 0, 246, 15, 228, 255, 61, 40, 254, 185, 77, 12, 156, 252, 84, 175, 9, 18, 93, 153, 86, 10, 209, 188, 54, 105, 116, 44, 88, 221, 240, 178, 34, 63, 67, 154, 118, 211, 80, 190, 186, 251, 128, 37, 205, 172, 69, 31, 96, 59, 90, 8, 148, 146, 217, 210, 216, 36, 238, 134, 125, 45, 181, 166, 68, 14, 100, 206, 167, 139, 32, 192, 244, 73, 236, 241, 152, 99, 87, 19, 47, 200, 231, 234, 147, 102, 144, 41, 204, 72, 223, 242, 114, 119, 25, 91, 6, 104, 3, 124, 174, 50, 212, 164, 207, 28, 203, 21, 39, 81, 92, 53, 155, 30, 165, 239, 194, 98, 4, 2, 180, 74, 191, 75, 161, 101, 82, 55, 110, 182, 51, 46, 222, 249, 136, 83, 189, 253, 142, 141, 237, 184, 78, 117, 97, 225, 103, 70, 126, 129, 150, 202, 173, 121};

	unsigned char lfsr(unsigned char t){return(t>>1)|(((t>>0)^(t>>3)^(t>>5)^(t>>6)^(t>>8))&1?128:0);}

	// fuck stlib
	unsigned int r=0,a=1103515245,c=12345; // no mod bc it loops anyway
	unsigned int randint(){return(r=a*r+c);}
};

namespace perlin{
	int mod(int a,int b){a%=b;return(a>=0?a:b+a);} // ISTG
	//int mdiv(int a,int b){return (a+(a<0))/b-(a<0);}

	// interleave high and low bits, then multiply by golden ratio for good spread
	// XXX this only really makes a difference for chars, not ints...
	unsigned char hash(int x){
		return 159*(x^(x>>4));
	}

	unsigned char hash(int x,int y){
		return hash(hash(x)^y);
	}

	unsigned char hash(int x,int y,int z){
		return hash(hash(x,y),z);
	}

	unsigned char _perl(int x,int s){
		if(s==0)return 0;
		return(
			hash(x/s)*(s-mod(x,s)) +
			hash(x/s+1)* mod(x,s)
		)/s;
	}

	unsigned char _perl(int x,int y,int s){
		x+=5;
		if(s==0)return 0;
		return(
			hash(x/s,y/s)*(s-mod(x,s))*(s-mod(y,s)) +
			hash(x/s+1,y/s)*(mod(x,s))*(s-mod(y,s)) +
			hash(x/s,y/s+1)*(s-mod(x,s))*(mod(y,s)) +
			hash(x/s+1,y/s+1)*(mod(x,s))*(mod(y,s))
		)/(s*s);
	}

	unsigned char _perl(int x,int y,int z,int s){
		x+=5;y+=25;
		if(s==0)return 0;
		return(
			hash(x/s,y/s,z/s)*(s-mod(x,s))*(s-mod(y,s))*(s-mod(z,s)) +
			hash(x/s+1,y/s,z/s)*(mod(x,s))*(s-mod(y,s))*(s-mod(z,s)) +
			hash(x/s,y/s+1,z/s)*(s-mod(x,s))*(mod(y,s))*(s-mod(z,s)) +
			hash(x/s+1,y/s+1,z/s)*(mod(x,s))*(mod(y,s))*(s-mod(z,s)) +
			hash(x/s,y/s,z/s+1)*(s-mod(x,s))*(s-mod(y,s))*(mod(z,s)) +
			hash(x/s+1,y/s,z/s+1)*(mod(x,s))*(s-mod(y,s))*(mod(z,s)) +
			hash(x/s,y/s+1,z/s+1)*(s-mod(x,s))*(mod(y,s))*(mod(z,s)) +
			hash(x/s+1,y/s+1,z/s+1)*(mod(x,s))*(mod(y,s))*(mod(z,s))
		)/(s*s*s);
	}

	unsigned char perl(int x,int o){
		unsigned char ret=0;
		for(int i=1;i<o;i++)
			ret+=_perl(x,i)>>(o-i);
		return ret;
	}

	unsigned char perl(int x,int y,int o){
		unsigned char ret=0;
		for(int i=1;i<o;i++)
			ret+=_perl(x,y,i)>>(o-i);
		return ret;
	}

	unsigned char perl(int x,int y,int z,int o){
		unsigned char ret=0;
		for(int i=1;i<o;i++)
			ret+=_perl(x,y,z,i)>>(o-i);
		return ret;
	}
};

/*
its not as uniform as it should be . . .

normalizing?
ret*255/(1<<o)

cache inverses for speed
*/
#endif
