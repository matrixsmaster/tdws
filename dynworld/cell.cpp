/*
 * cell.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cell.h"
#include "gui.h"

CWCell::CWCell(MSMRLCG* gen, CellType typ, CPoint2D coord)
{
	rnd = gen;
	cell_type = typ;
	npconcell = NULL;
	crd = coord;
	owner = 0;
}

CWCell::~CWCell()
{
	if (npconcell) delete npconcell;
}

bool CWCell::ChangeTo(CellType typ, void* ref)
{
	cell_type = typ;
	return true;
}

bool CWCell::AddNPC(CNPC* ref)
{
	if (!npconcell) {
		npconcell = ref;
		return true;
	}
	return false;
}

bool CWCell::SpawnNPC(void)
{
	if (npconcell)
		return false;
	npconcell = new CNPC(rnd);
	npconcell->SetCrd(crd);
	printlog("NPC spawned @ %d:%d\n",crd.X,crd.Y);
	return true;
}

char CWCell::Print(bool raw)
{
	if (npconcell && (!raw))
		return(npconcell->GetSymbol());
	switch (cell_type) {
	case CT_Water:		return('~'); break;
	case CT_Sand:		return('.'); break;
	case CT_Field: 		return('_'); break;
	case CT_Forest: 	return('!'); break;
	case CT_Mount:	 	return('^'); break;
	case CT_HH_Wall: 	return('*'); break;
	case CT_HH_Door: 	return('D'); break;
	default:		//to make compiler happy
		return(' ');
	}
}

bool CWCell::PrintInfo(char* str, int m)
{
	if ((!str) || (m < 2)) return false;
	memset(str,0,m);
	if (!npconcell) return false;
	return npconcell->PrintInfo(str,m);
}

void CWCell::Quantum(void)
{
	if (npconcell) {
		npconcell->SetOnBed(cell_type == CT_HH_Bed);
		//npconcell->Quantum();
		if (npconcell->GetIsDead()) {
			if (gui_gettarget() == npconcell)
				gui_settarget(NULL);
			delete npconcell;
			npconcell = NULL;
			printlog("NPC dead @ %d:%d\n",crd.X,crd.Y);
		}
	}
}
