/*
 * world.h
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#ifndef WORLD_H_
#define WORLD_H_

#include "npc.h"
#include "cell.h"
#include "MSMRLCG.h"
#include "crazymath.h"
#include "CPoint2D.h"
#include "gui.h"
#include "world_const.h"

typedef struct SWorldStats {
	int population;
} WorldStats;

class CWorld {
public:
	CWorld(long seed);
	~CWorld(void);
//	void PrintWorld(void);
	void Update(void);
	bool ProcKey(int ch);
	bool isReadyToQuit(void) { return can_exit; }
	long getInitSeed(void)	 { return init_seed; }

private:
	void CreateFields(CellType typ, int maxsq);
	void CreateRivers(void);
//	bool CreateNPC(CPoint2D crd);
	void Cycle(int cycles);
	void Quantum(void);
	void ProcessNPC(CPoint2D pos);
	void RaytraceNPCVisual(CNPC* npc, int dir, CPoint2D pos);
	bool MoveNPC(CPoint2D from, CPoint2D to);
	MSMRLCG* rnd;
	long init_seed;
	CWCell* field[WRLD_SIZE_X][WRLD_SIZE_Y];
	CPoint2D window;
	bool can_exit,paused;
	int step_size;
	WorldStats statistic;
};


#endif /* WORLD_H_ */
