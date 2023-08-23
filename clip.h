// XXX fix this shite!
// should suffice to only cull in screen space, which makes things a lot easier
float interpol(Q pp,Q pn,Q ls,Q le){float pd=pn.dot(pp),ad=ls.dot(pn),bd=le.dot(pn);return (pd-ad)/(bd-ad);}
Q intersect(Q pp,Q pn,Q ls,Q le){return ls+(le-ls)*interpol(pp,pn,ls,le);}
int clip(Q pp, Q pn, Tris tin, Tris*tout1, Tris*tout2){
	// plane point, plane normal
	pn=pn.norm();
	// remap um,vm, account for order
	// ok this will be ugly but: we have to count up to three vertices, each w/ a mapping. cant we use a tris for that?

	tout1->texture=tout2->texture=tin.texture;
	tout1->tsize=tout2->tsize=tin.tsize;
	for(int i=0;i<3;i++)tout1->m[i]=tout2->m[i]=tin.m[i];

	Tris t;
	Q inside[3],outside[3];int ninside=0,noutside=2;pn=pn.norm();

	for(int i=0;i<3;i++)
	//((tin.q[i]-pp).norm().dot(pn)>0)?
	((tin.q[i]-pp).dot(pn)>0)?
		(t.q[ninside]=tin.q[i],t.m[ninside]=tin.m[i],ninside++):
		(t.q[noutside]=tin.q[i],t.m[noutside]=tin.m[i],noutside--);

	if(ninside==0)return 0; // invalid

	if(ninside==1){ // clip to 1
		*tout1=tin;
		tout1->q[0]=t.q[0];tout1->m[0]=t.m[0];

		tout1->q[1]=intersect(pp,pn,t.q[0],t.q[2]);
		tout1->m[1]=t.m[0]+(t.m[2]-t.m[0])*interpol(pp,pn,t.q[0],t.q[2]);

		tout1->q[2]=intersect(pp,pn,t.q[0],t.q[1]);
		tout1->m[2]=t.m[0]+(t.m[1]-t.m[0])*interpol(pp,pn,t.q[0],t.q[1]);
		return 1;
	}

	if(ninside==2){ // clip to 2
		*tout1=*tout2=tin;
		tout1->q[0]=t.q[0];tout1->m[0]=t.m[0];
		tout1->q[1]=t.q[1];tout1->m[1]=t.m[1];

		tout1->q[2]=intersect(pp,pn,t.q[0],t.q[2]);
		tout1->m[2]=t.m[0]+(t.m[2]-t.m[0])*interpol(pp,pn,t.q[0],t.q[2]);

		tout2->q[0]=t.q[1];tout2->m[0]=t.m[1];
		tout2->q[1]=tout1->q[2];tout2->m[1]=tout1->m[2];
		tout2->q[2]=intersect(pp,pn,t.q[1],t.q[2]);
		tout2->m[2]=t.m[1]+(t.m[2]-t.m[1])*interpol(pp,pn,t.q[1],t.q[2]);
		return 2;
	}

	if(ninside==3){*tout1=tin;return 1;} // valid
	return 0; // now shut up
}
