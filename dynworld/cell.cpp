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

bool CWCell::ChangeTo(CellType typ, CNPC* ref)
{
	if (ref) {
		if ((owner) && (owner != ref->GetSign())) return false;
		owner = ref->GetSign();
	}
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
	case CT_Field: 		return('W'); break;
	case CT_Forest: 	return('!'); break;
	case CT_Mount:	 	return('^'); break;
	case CT_HH_Wall: 	return('*'); break;
	case CT_HH_Door: 	return('#'); break;
	case CT_HH_Bed: 	return('_'); break;
	case CT_HH_Bowl: 	return('@'); break;
	default:		//to make compiler happy
		return(' ');
	}
}

bool CWCell::PrintInfo(char* str, int m)
{
	char* buf;
	if ((!str) || (m < 2)) return false;
	memset(str,0,m);

	if (npconcell)
		return npconcell->PrintInfo(str,m);

	switch (cell_type) {
	case CT_Empty:
		snprintf(str,m-1,"The Void\n");
		break;
	case CT_HH_Empty:
		snprintf(str,m-1,"House floor\n");
		break;
	case CT_HH_Wall:
		snprintf(str,m-1,"Wall\n");
		break;
	case CT_HH_Door:
		snprintf(str,m-1,"Doorway\n");
		break;
	case CT_HH_Bed:
		snprintf(str,m-1,"Bed\n");
		break;
	case CT_HH_Bowl:
		snprintf(str,m-1,"Bowl\n");
		break;
	default:
		return false;
	}
	if (owner) {
		buf = (char*)malloc(m);
		if (!buf) return false;
		snprintf(buf,m-1," owned by %ld\n",owner);
		strncat(str,buf,m-1);
		free(buf);
	}
	return true;
}

bool CWCell::ViewThru(void)
{
	switch (cell_type) {
	case CT_Empty:
	case CT_Water:
	case CT_Sand:
	case CT_Field:
	case CT_Forest:
	case CT_HH_Empty:
	case CT_HH_Bed:
	case CT_HH_Bowl:
		return true;
	default:
		return false;
	}
}

bool CWCell::Routable(void)
{
	switch (cell_type) {
	case CT_Empty:
	case CT_Sand:
	case CT_Field:
	case CT_Forest:
	case CT_HH_Empty:
		return true;
	default:
		return false;
	}
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
