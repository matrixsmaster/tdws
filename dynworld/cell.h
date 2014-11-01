/*
 * cell.h
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#ifndef CELL_H_
#define CELL_H_

#include "CPoint2D.h"
#include "world_const.h"
#include "npc.h"


class CWCell {
public:
	CWCell(MSMRLCG* gen, CellType typ, CPoint2D coord);
	~CWCell(void);
	CellType GetType(void) { return cell_type; }
	bool ChangeTo(CellType typ, CNPC* ref);
	bool AddNPC(CNPC* ref);
	bool SpawnNPC(void);
	void RemNPC(void) { npconcell = NULL; }
	CNPC* GetNPC(void) { return npconcell; }
	char Print(bool raw);
	bool PrintInfo(char* str, int m);
	bool ViewThru(void);
	bool Routable(void);
	void Quantum(void);

private:
	MSMRLCG* rnd;
	CellType cell_type;
	CPoint2D crd;
	CNPC* npconcell;
	npcsign_t owner;
};


#endif /* CELL_H_ */
