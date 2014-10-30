/*
 * npc.h
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#ifndef NPC_H_
#define NPC_H_

#include "MSMRLCG.h"
#include "crazymath.h"
#include "world_const.h"
#include "CPoint2D.h"

class CNPC;

enum NPCState {
	NPC_Idle,
	NPC_Walking,
	NPC_Running,
	NPC_Sleeping,
	NPC_Browsing,
	NPC_Talking
};

typedef struct SNPCStats {
	int direction;
	int stamina;
	int idle_count;
	bool on_bed;
	bool stuck;
	CNPC* talk_to;
} NPCStats;

typedef struct SNPCVMemCell {
	CellType typ;
	char symbol;
	npcsign_t owned;
} NPCVMemCell;

typedef struct SNPCVisualIn {
	NPCVMemCell b; //basic information to be stored
	CNPC* npc;
} NPCVisualIn;

class CNPC {
public:
	CNPC(MSMRLCG* gen); 					//create random character
	CNPC(MSMRLCG* gen, CNPC* clone); 		//create a clone
	CNPC(MSMRLCG* gen, CNPC* a, CNPC* b); 	//create a child
	virtual ~CNPC(void); 					//death
	void Reset(void); 						//reset internal state
	char GetSymbol(void); 					//get symbolic representation (a 'view')
	bool PrintInfo(char* str, int m);

	int* GetGenome(void)	{ return chrom; }
	int GetAge(void)		{ return tick; }
	NPCState GetState(void) { return my_state; }
	NPCStats GetStats(void) { return my_stats; }
	bool GetIsDead(void)	{ return dead; }
	CPoint2D GetCrd(void)	{ return my_coord; }
	void SetOnBed(bool b)	{ my_stats.on_bed = b; }
	void SetCrd(CPoint2D c) { if (!(my_coord==CPoint2D(0))) { if (distance(c,my_coord)>2.0) abort(); } my_coord = c; }
	npcsign_t GetSign(void) { return signature; }
	void SetStuck(bool s)	{ my_stats.stuck = s; }

	void PutVision(CPoint2D ul, NPCVisualIn* arr);
	NPCVMemCell* GetVMemory(void) { return (NPCVMemCell*)&memory[0]; }

	void Quantum(void);

private:
	int chrom[CHROM_LENGTH];
	MSMRLCG* rnd;
	int tick;
	NPCState my_state;
	NPCStats my_stats;
	CPoint2D my_coord;
	bool dead;
	NPCVisualIn view[WRLD_CHR_VIEW][WRLD_CHR_VIEW];
	NPCVMemCell memory[WRLD_SIZE_X][WRLD_SIZE_Y];
	npcsign_t signature;
};

#endif /* NPC_H_ */
