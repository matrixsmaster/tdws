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

	CreateFields(CT_Sand,(WRLD_SIZE_X*WRLD_SIZE_Y/100*WRLD_WILDR));
	CreateFields(CT_Field,(WRLD_SIZE_X*WRLD_SIZE_Y/100*WRLD_FIELD));
	CreateFields(CT_Mount,(WRLD_SIZE_X*WRLD_SIZE_Y/100*WRLD_MOUNT));
	CreateRivers(); //this must be the last call to land generator

	z = (int)floor((float)(WRLD_SIZE_X*WRLD_SIZE_Y/100)*WRLD_INIT_POP);
	printlog("Spawning max %d NPCs\n",z);
	if (z < 1) z = 1;
	while (z--)
		if (!field[rnd->RangedNumber(WRLD_SIZE_X)][rnd->RangedNumber(WRLD_SIZE_Y)]->SpawnNPC())
			break;

	window = CPoint2D(0);
	can_exit = paused = false;
	step_size = 1;

	setfield((CWCell**)&field[0],WRLD_SIZE_X,WRLD_SIZE_Y);
	printlog("World Created\n");
}

CWorld::~CWorld()
{
	int x,y;
	for (y = 0; y < WRLD_SIZE_Y; y++)
		for (x = 0; x < WRLD_SIZE_X; x++)
			delete field[x][y];
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

	printlog("river %d:%d to %d\n",x,y,d);

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

void CWorld::Update(void)
{
	if (!paused) Cycle(step_size);
	gui_update();
}

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
	return res;
}

void CWorld::Cycle(int cycles)
{
	while (cycles--) Quantum();
}

void CWorld::Quantum(void)
{
	int x,y,z,u;
	statistic.population = 0;
	for (x = 0; x < WRLD_SIZE_X; x++)
		for (y = 0; y < WRLD_SIZE_Y; y++) {
			field[x][y]->Quantum();
			if (field[x][y]->GetNPC()) statistic.population++;
		}

	CNPC** npcarr = (CNPC**)malloc(statistic.population*sizeof(CNPC*));
	z = u = 0;
	for (x = 0; x < WRLD_SIZE_X; x++)
		for (y = 0; y < WRLD_SIZE_Y; y++) {
			if (!field[x][y]->GetNPC()) continue;
			if ((z < statistic.population) && (npcarr)) {
				for (u = 0; u < z; u++)
					if (npcarr[u] == field[x][y]->GetNPC()) {
						u = -1;
						break;
					}
				if (u >= 0) {
					npcarr[z++] = field[x][y]->GetNPC();
					ProcessNPC(CPoint2D(x,y));
				}
			}
		}
	if (npcarr) free(npcarr);
}

void CWorld::ProcessNPC(CPoint2D pos)
{
	int s,dir;
	bool tmp;
	CPoint2D pa,pl;

	CNPC* npc = field[pos.X][pos.Y]->GetNPC();
	npc->Quantum();
	NPCStats st = npc->GetStats();

	dir = st.direction * 2; //to save the code :))
	if ((dir < 0) || (dir >= 16)) return;

	s = 0;
	switch (npc->GetState()) {
	case NPC_Sleeping:
		return;

	case NPC_Running:
		s++;
		//no break;
	case NPC_Walking:
		s++;
		pa = pos;
		while (s--) {
			pl = pa;
			pa = pa + CPoint2D(direction_table[dir],direction_table[dir+1]);
			if (!MoveNPC(pl,pa)) {
				npc->SetStuck(true);
				break; //stop moving if it's not possible
			}
		}
		break;

	case NPC_Building:
		if ((npc->GetCellChange()) && (!(npc->GetCellChange()->changed))) {
			tmp = field[pos.X][pos.Y]->ChangeTo(npc->GetCellChange()->change_to,npc);
			npc->GetCellChange()->changed = tmp;
			if (tmp) printlog("Cell changed %d:%d\n",pos.X,pos.Y);
			else printlog("Cell not changed %d:%d\n",pos.X,pos.Y);
		}
		break;

	default:
		break;
	}

	RaytraceNPCVisual(npc,dir,pos);
}

#define PCNPC_VIEWMACRO if (!ispointin(&cr,ul.X,ul.Y,br.X,br.Y)) break;\
		rl = cr - ul;\
		lin = rl.X*WRLD_CHR_VIEW+rl.Y;\
		if (vbuf[lin].b.typ == CT_Empty) {\
			vbuf[lin].b.typ = field[cr.X][cr.Y]->GetType();\
			vbuf[lin].b.symbol = field[cr.X][cr.Y]->Print(true);\
			vbuf[lin].b.routable = field[cr.X][cr.Y]->Routable();\
			if ((field[cr.X][cr.Y]->GetNPC()) && \
					(field[cr.X][cr.Y]->GetNPC()->GetAge() + 2 < \
							field[cr.X][cr.Y]->GetNPC()->GetGenome()[CHR_LIFESP]))\
							vbuf[rl.X*WRLD_CHR_VIEW+rl.Y].npc = field[cr.X][cr.Y]->GetNPC();\
		}\
		if (!field[cr.X][cr.Y]->ViewThru()) break;

//I'd removed this function from main NPC Processing to simplify things :)
//more params passed to avoid excessive duplications...
//and just because i'm too laaazyyy
void CWorld::RaytraceNPCVisual(CNPC* npc, int dir, CPoint2D pos)
{
	int l,s,lin;

	CPoint2D ul,br,cr,rl;
	NPCVisualIn vbuf[WRLD_CHR_VIEW*WRLD_CHR_VIEW];

	//gather visual feedback
	memset(vbuf,0,sizeof(vbuf));
	ul = pos - CPoint2D(view_cone_table[dir],view_cone_table[dir+1]);
	br = ul + CPoint2D(WRLD_CHR_VIEW);
	normpoint(&ul,0,0,WRLD_SIZE_X,WRLD_SIZE_Y);
	normpoint(&br,0,0,WRLD_SIZE_X,WRLD_SIZE_Y);
	switch (dir/2) {
	case 2: case 6:
		//scroll by Y
		for (l = ul.Y; l < br.Y; l++) {
			cr = pos;
			for (s = 0; s < WRLD_CHR_VIEW*2; s++) {
				switch (dir/2) {
				case 2:
					//look to right side
					cr = getnextpoint(cr,CPoint2D(br.X,l),NULL);
					break;
				default:
					//look to left side
					cr = getnextpoint(cr,CPoint2D(ul.X,l),NULL);
					break;
				}
				PCNPC_VIEWMACRO
			}
		}
		break;
	default:
		//scroll by X
		for (l = ul.X; l < br.X; l++) {
			cr = pos;
			for (s = 0; s < WRLD_CHR_VIEW*2; s++) {
				switch (dir/2) {
				case 0: case 1: case 7:
					//look to upper side
					cr = getnextpoint(cr,CPoint2D(l,ul.Y),NULL);
					break;
				default:
					//look by lower side
					cr = getnextpoint(cr,CPoint2D(l,br.Y),NULL);
					break;
				}
				PCNPC_VIEWMACRO
			}
		}
		break;
	}
	npc->PutVision(ul,vbuf);
}

bool CWorld::MoveNPC(CPoint2D from, CPoint2D to)
{
	//checking route
	if (!ispointin(&to,0,0,WRLD_SIZE_X,WRLD_SIZE_Y)) return false;
	if (!field[from.X][from.Y]->GetNPC()) return false;
	if (field[to.X][to.Y]->GetNPC()) return false;
	if (!field[to.X][to.Y]->Routable()) return false;
	//movement
	field[to.X][to.Y]->AddNPC(field[from.X][from.Y]->GetNPC());
	field[from.X][from.Y]->RemNPC();
	field[to.X][to.Y]->GetNPC()->SetCrd(to);
	return true;
}
