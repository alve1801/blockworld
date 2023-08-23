float*dbuf,dist;

void px(int x,int y,img*screen,short col){
	if(x>=0 && y>=0 && x<screen->sx && y<screen->sy && dist<dbuf[y*screen->sx+x])
	screen->data[y*screen->sx+x]=col,dbuf[y*screen->sx+x]=dist;
}

//#include "clip.h"

void triangle(int p[6],img*screen,short col){
	int t,x1,x2,ax=p[0],ay=p[1],bx=p[2],by=p[3],cx=p[4],cy=p[5];

	if(ay>cy){t=ax;ax=cx;cx=t;t=ay;ay=cy;cy=t;}
	if(ay>by){t=ax;ax=bx;bx=t;t=ay;ay=by;by=t;}
	if(by>cy){t=bx;bx=cx;cx=t;t=by;by=cy;cy=t;}

	if(ay==cy){ // or just ignore it?
		if(ax>cx){t=ax;ax=cx;cx=t;}
		if(ax>bx){t=ax;ax=bx;bx=t;}
		if(bx>cx){t=bx;bx=cx;cx=t;}
		for(int x=ax;x<cx;x++)px(x,ay,screen,col);
		return;
	}

	if(ay!=by)
		for(int y=ay;y<by;y++){
			x1 = ax+(y-ay)*(ax-bx)/(ay-by);
			x2 = ax+(y-ay)*(ax-cx)/(ay-cy);
			if(x1>x2){t=x1;x1=x2;x2=t;}
			for(int x=x1;x<x2;x++)px(x,y,screen,col);
		}

	if(by!=cy)
		for(int y=by;y<cy;y++){
			x1 = ax+(y-ay)*(ax-cx)/(ay-cy);
			x2 = bx+(y-by)*(bx-cx)/(by-cy);
			if(x1>x2){t=x1;x1=x2;x2=t;}
			for(int x=x1;x<x2;x++)px(x,y,screen,col);
		}
}

void render(World*w,Player*p,img*screen){
	// player gives us viewport
	// world gives us world data
	// we render all chunks w/in some distance

	dbuf=(float*)malloc(screen->sx*screen->sy*sizeof(float));
	for(int i=0;i<screen->sx*screen->sy;i++)dbuf[i]=INFINITY;

	struct tris{v3 loc,norm;block b;};
	// well that was easy
	// not gonna cover all usecases, but good enough for a baseline

	vector<tris>faces;

	for(Chunk&c:w->data)
	for(int bx=0;bx<sidelen;bx++)
	for(int by=0;by<sidelen;by++)
	for(int bz=0;bz<sidelen;bz++){
		v3 ind(bx,by,bz);
		block b=c.data[ind.index()];
		if(b){
			v3 vloc=c.loc*sidelen+ind;

			// this can prolly be optimised

			int x=vloc.x<p->loc.x?1:-1;
			if(bx+x<0 || bx+x>=sidelen || !c.data[v3(bx+x,by,bz).index()])
				faces.push_back((tris){vloc,v3(x,0,0),b});

			int y=vloc.y<p->loc.y?1:-1;
			if(by+y<0 || by+y>=sidelen || !c.data[v3(bx,by+y,bz).index()])
				faces.push_back((tris){vloc,v3(0,y,0),b});

			int z=vloc.z<p->loc.z?1:-1;
			if(bz+z<0 || bz+z>=sidelen || !c.data[v3(bx,by,bz+z).index()])
				faces.push_back((tris){vloc,v3(0,0,z),b});
		}
	}

	for(tris&f:faces){
		v3 verts[4];short col;
		// fuck it, we hardcodin this
		// XXX make sure these are oriented properly
		if(f.norm.x==-1){
			verts[0]=v3(0,0,0);
			verts[1]=v3(0,0,1);
			verts[3]=v3(0,1,1);
			verts[2]=v3(0,1,0);
			//col=0x844;
			col=block_cols[2*f.b.type+1]|0x111;
		}else if(f.norm.x==1){
			verts[0]=v3(1,0,0);
			verts[1]=v3(1,0,1);
			verts[3]=v3(1,1,1);
			verts[2]=v3(1,1,0);
			//col=0xf88;
			col=block_cols[2*f.b.type+1]|0x111;
		}else if(f.norm.y==-1){
			verts[0]=v3(0,0,0);
			verts[1]=v3(0,0,1);
			verts[3]=v3(1,0,1);
			verts[2]=v3(1,0,0);
			//col=0x484;
			col=block_cols[2*f.b.type+1]&0xeee;
		}else if(f.norm.y==1){
			verts[0]=v3(0,1,0);
			verts[1]=v3(0,1,1);
			verts[3]=v3(1,1,1);
			verts[2]=v3(1,1,0);
			//col=0x8f8;
			col=block_cols[2*f.b.type+1]&0xeee;
		}else if(f.norm.z==-1){
			verts[0]=v3(0,0,0);
			verts[1]=v3(0,1,0);
			verts[2]=v3(1,0,0);
			verts[3]=v3(1,1,0);
			//col=0x448;
			col=block_cols[2*f.b.type];
		}else if(f.norm.z==1){
			verts[0]=v3(0,0,1);
			verts[1]=v3(0,1,1);
			verts[2]=v3(1,0,1);
			verts[3]=v3(1,1,1);
			//col=0x88f;
			col=block_cols[2*f.b.type];
		}else printf("norm %i %i %i\n",f.norm.x,f.norm.y,f.norm.z);

		// XXX

		int pts[8]={0};bool r=1;

		float t;dist=0;
		t=f.loc.x-p->loc.x;dist+=t*t;
		t=f.loc.y-p->loc.y;dist+=t*t;
		t=f.loc.z-p->loc.z;dist+=t*t;

		for(int i=0;i<4;i++){
			float
				tx = verts[i].x+f.loc.x-p->loc.x,
				ty = verts[i].y+f.loc.y-p->loc.y,
				tz = verts[i].z+f.loc.z-p->loc.z,
				sa,sb,ca,cb,tt;

			sincosf(p->pitch/81.49,&sa,&ca);
			sincosf(p->yaw/40.74,&sb,&cb);

			tt = cb*ty-sb*tx;
			tx = tx*cb+ty*sb;
			ty = tt*ca+tz*sa;
			tz = tz*ca-tt*sa;

			if(tz<.1)r=0;
			else{
				//pts[2*i  ] = tx*screen->sx/tz + screen->sx/2;
				//pts[2*i+1] = ty*screen->sy/tz + screen->sy/2;

				// and now, the shittiest view frustrum culling youve ever seen
				int u = tx*screen->sx/tz + screen->sx/2;
				int v = ty*screen->sy/tz + screen->sy/2;
				const int offset=50;
				if(u<-offset || u>screen->sx+offset || v<-offset || v>screen->sy+offset)r=0;
				else pts[2*i]=u,pts[2*i+1]=v;
			}
		}

		if(r)triangle(pts,screen,col),triangle(pts+2,screen,col);
	}

	dist=0;

	if(!drawwisps)return;
	for(Wisp&w:wisps){
		float
			tx = w.loc.x - p->loc.x,
			ty = w.loc.y - p->loc.y,
			tz = w.loc.z - p->loc.z,
			sa,sb,ca,cb,tt;

		sincosf(p->pitch/81.49,&sa,&ca);
		sincosf(p->yaw/40.74,&sb,&cb);

		tt = cb*ty-sb*tx;
		tx = tx*cb+ty*sb;
		ty = tt*ca+tz*sa;
		tz = tz*ca-tt*sa;

		if(tz>.2){
			int u = tx*screen->sx/tz + screen->sx/2;
			int v = ty*screen->sy/tz + screen->sy/2;
			for(int sx=-2;sx<3;sx++)
			for(int sy=-2;sy<3;sy++)
			px(u+sx,v+sy,screen,w.col);
		}
	}

	//printf("frame!\n");
}
