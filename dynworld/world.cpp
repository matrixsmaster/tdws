/*
 * world.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "world.h"

CWorld::CWorld(long seed)
{
	int x,y,z;

	rnd = new MSMRLCG(false);
	init_seed = seed;
	if (seed != 0) rnd->SetSeed(seed);
	else init_seed = rnd->TimeSeed();

	printlog("Creating world with seed of %ld\n",rnd->GetSeed());

	for (y = 0; y < WRLD_SIZE_Y; y++)
		for (x = 0; x < WRLD_SIZE_X; x++)
			field[x][y] = new CWCell(rnd,CT_Forest,CPoint2D(x,y));

	/* probability distribution testing
	int arr[] = {5,19,5,20,6,20,5,20};
	int res[] = {0,0,0,0,0,0,0,0};
	for (y=0; y<100000; y++)
		res[selfrom(rnd,arr,8)]++;
	float z,check = 0;
	for (x=0; x<8; x++) {
		z = ((float)res[x]/(float)(y-1));
		printf("%d)\t%d:\t%d\t(%f)\n",x,arr[x],res[x],z);
		check += z;
	}
	printf("check: %f\n",check);*/

	CreateFields(CT_Sand,(WRLD_SIZE_X*WRLD_SIZE_Y/100*WRLD_WILDR));
	CreateFields(CT_Field,(WRLD_SIZE_X*WRLD_SIZE_Y/100*WRLD_FIELD));
	CreateFields(CT_Mount,(WRLD_SIZE_X*WRLD_SIZE_Y/100*WRLD_MOUNT));
	CreateRivers(); //this must be the last call to land generator

	//people = NULL;
	//population = 0;
	z = (int)floor((float)(WRLD_SIZE_X*WRLD_SIZE_Y/100)*WRLD_INIT_POP);
	printlog("Spawning max %d NPCs\n",z);
	if (z < 1) z = 1;
	while (z--)
		if (!CreateNPC(CPoint2D(rnd->RangedNumber(WRLD_SIZE_X),rnd->RangedNumber(WRLD_SIZE_Y))))
			break;

	window = CPoint2D(0);
	can_exit = paused = false;
	setfield((CWCell**)&field[0],WRLD_SIZE_X,WRLD_SIZE_Y);
	printlog("World Created\n");
}

CWorld::~CWorld()
{
	int x,y;
	for (y = 0; y < WRLD_SIZE_Y; y++)
		for (x = 0; x < WRLD_SIZE_X; x++)
			delete field[x][y];
	//while (population--) delete people[population];
	//free(people);
	delete rnd;
	printlog("WORLD DESTROYED\n");
}

void CWorld::CreateFields(CellType typ, int maxsq)
{
	int x,y,w,h,f,s,i,j;
	x = rnd->RangedNumber(WRLD_SIZE_X-1);
	y = rnd->RangedNumber(WRLD_SIZE_Y-1);
	s = rnd->RangedNumber(maxsq) + 1;
	f = rnd->RangedNumber(8) + 1;
	w = (WRLD_SIZE_X-x < 2)? 1:(rnd->RangedNumber(s/f)+1);
	if (x+w >= WRLD_SIZE_X) w = WRLD_SIZE_X-x-1;
	h = (WRLD_SIZE_Y-y < 2)? 1:(s/w);
	if (y+h >= WRLD_SIZE_Y) h = WRLD_SIZE_Y-y-1;
	printlog("field %d:%d [%d x %d]\n",x,y,w,h);
	for (i = 0; i < w; i++)
		for (j = 0; j < h; j++)
			field[i+x][j+y]->ChangeTo(typ,NULL);
	if (rnd->RangedNumber(100) < WRLD_GEN_FORK_P)
		CreateFields(typ,maxsq);
}

void CWorld::CreateRivers()
{
	int x,y,d,i,pa[8];
	x = rnd->RangedNumber(WRLD_SIZE_X-2);
	y = rnd->RangedNumber(WRLD_SIZE_Y-2);
	d = rnd->RangedNumber(8);
	printlog("river(): %d:%d to %d\n",x,y,d);
	pa[d] = rnd->RangedNumber(50);
	for (i = 0; i < 8; i++) {
		if (i == d) continue;
		pa[i] = (100-pa[d])/7;
	}
	while ((x >= 0) && (x < WRLD_SIZE_X) &&
			(y >= 0) && (y < WRLD_SIZE_Y)) {
		field[x][y]->ChangeTo(CT_Water,NULL);
		i = selfrom(rnd,pa,8);
		x += direction_table[i*2];
		y += direction_table[i*2+1];
	}
	if (rnd->RangedNumber(100) < WRLD_GEN_FORK_P)
		CreateRivers();
}

bool CWorld::CreateNPC(CPoint2D crd)
{
	/*if (field[crd.X][crd.Y]->isNPCon()) return false;
	CNPC** ptr = (CNPC**)realloc(people,(population+1)*sizeof(CNPC*));
	if (!ptr) {
		fprintf(stderr,"Unable to create NPC\n");
		return false;
	}
	people = ptr;
	ptr += population++;
	*ptr = new CNPC(rnd);
	field[crd.X][crd.Y]->AddNPC(*ptr);*/
//	printlog("NPC @ %d:%d\n",crd.X,crd.Y);
	return field[crd.X][crd.Y]->SpawnNPC();
	//return true;
}

//void CWorld::PrintWorld()
//{
//	int x,y;
//	printf("\n\n%04d-->\n",window.X);
//	for (y = 0; y < WRLD_PRINT_H; y++) {
//		if (y + window.Y >= WRLD_SIZE_Y) break;
//		printf("%04d  ",window.Y+y);
//		for (x = 0; x < WRLD_PRINT_W; x++) {
//			if (x + window.X >= WRLD_SIZE_X) break;
//			field[x+window.X][y+window.Y]->Print();
//		}
//		printf("\n");
//	}
//}


bool CWorld::ProcKey(int ch)
{
	bool res = true;
	switch (ch) {
	case 'q': can_exit = true; break;
	case ' ':
		paused = !paused;
		if (paused) printlog("Paused\n");
		else printlog("Resumed\n");
		break;
	case 't':
		gui_settarget(field[gui_getcursor().X][gui_getcursor().Y]->GetNPC());
		break;
	default: res = gui_prockey(ch);
	}
	if (!paused) Cycle(1);
	gui_update();
	return res;
}

void CWorld::Cycle(int cycles)
{
	while (cycles--) Quantum();
}

void CWorld::Quantum(void)
{
	int x,y;
	for (x = 0; x < WRLD_SIZE_X; x++)
		for (y = 0; y < WRLD_SIZE_Y; y++) {
			field[x][y]->Quantum();
			if (field[x][y]->GetNPC())
				ProcessNPC(CPoint2D(x,y));
			field[x][y]->Quantum();
		}
}

void CWorld::ProcessNPC(CPoint2D pos)
{
	int s,dir,lin;
	NPCVisualIn vbuf[WRLD_CHR_VIEW*WRLD_CHR_VIEW];
	CPoint2D ul,br,cr,rl;

	CNPC* npc = field[pos.X][pos.Y]->GetNPC();
	npc->Quantum();
	NPCStats st = npc->GetStats();

	dir = st.direction * 2; //to save the code :))
	if ((dir < 0) || (dir >= 16)) return;

//	if ((npc->GetCrd() != pos) && (st.direction < 8)) {
		memset(vbuf,0,sizeof(vbuf));
		//put visual feedback
		ul = pos - CPoint2D(view_cone_table[dir],view_cone_table[dir+1]);
		br = ul + CPoint2D(WRLD_CHR_VIEW);
		//FIXME: cover all 'viewing lines' and do some raytracing
		normpoint(&ul,0,0,WRLD_SIZE_X,WRLD_SIZE_Y);
		normpoint(&br,0,0,WRLD_SIZE_X,WRLD_SIZE_Y);
		for (s = 0;;s++) {
			cr = pointonline(ul,br,s);
			if ((cr.X < ul.X) || (cr.X >= br.X) ||
					(cr.Y < ul.Y) || (cr.Y >= br.Y))
				break;
			rl = cr - ul;
			lin = rl.X*WRLD_CHR_VIEW+rl.Y;
			if (vbuf[lin].b.typ == CT_Empty) {
				vbuf[lin].b.typ = field[cr.X][cr.Y]->GetType();
				vbuf[lin].b.symbol = field[cr.X][cr.Y]->Print(true);
				vbuf[rl.X*WRLD_CHR_VIEW+rl.Y].npc = field[cr.X][cr.Y]->GetNPC();
			}
		}
		npc->PutVision(ul,vbuf);
//	}

	s = 0;
	switch (npc->GetState()) {
	case NPC_Running:
		s++;
		//no break;
	case NPC_Walking:
		s++;
		ul = pos;
		while (s--) {
			br = ul;
			//ul += CPoint2D(direction_table[dir],direction_table[dir+1]);
			ul = ul + CPoint2D(direction_table[dir],direction_table[dir+1]);
			if (!MoveNPC(br,ul)) {
				npc->SetStuck(true);
				break; //stop moving if it's not possible
			}
		}
		break;
	default:
		;
	}
}

bool CWorld::MoveNPC(CPoint2D from, CPoint2D to)
{
	if ((to.X < 0) || (to.Y < 0) || (to.X >= WRLD_SIZE_X) || (to.Y >= WRLD_SIZE_Y))
		return false;
	if (!field[from.X][from.Y]->GetNPC()) return false;
	if (field[to.X][to.Y]->GetNPC()) return false;
	//check movement ability to
	switch (field[to.X][to.Y]->GetType()) {
	case CT_Empty:
	case CT_Sand:
	case CT_Field:
	case CT_Forest:
		field[to.X][to.Y]->AddNPC(field[from.X][from.Y]->GetNPC());
		field[to.X][to.Y]->GetNPC()->SetCrd(to);
		field[from.X][from.Y]->RemNPC();
		return true;
	default:
		return false;
	}
}
