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
	NPC_Talking,
	NPC_Building
};

typedef struct SNPCStats {
	int direction;
	int stamina;
	int idle_count;
	bool on_bed;
	bool stuck;
	bool building;
	bool own_home;
	bool aimed;
	float last_dist;
	CPoint2D aim;
	CNPC* talk_to;
} NPCStats;

typedef struct SNPCVMemCell {
	CellType typ;
	char symbol;
	bool routable;
	npcsign_t owned;
} NPCVMemCell;

typedef struct SNPCVisualIn {
	NPCVMemCell b; //basic information to be stored
	CNPC* npc;
} NPCVisualIn;

typedef struct SNPCBuildPlan {
	CPoint2D ul; //upper-left corner
	CPoint2D sz; //size
	CellType map[WRLD_CHR_VIEW][WRLD_CHR_VIEW];
	int done;
	CPoint2D cur; //current point holder
} NPCBuildPlan;

typedef struct SNPCDisposition {
	npcsign_t who;
	int disp;
} NPCDisposition;

typedef struct SNPCCellChangeP {
//	CPoint2D crd;
	bool want_change;
	CellType change_to;
	bool changed;
} NPCCellChangeP;


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
	void SetCrd(CPoint2D c) { my_coord = c; }
	npcsign_t GetSign(void) { return signature; }
	void SetStuck(bool s)	{ my_stats.stuck = s; }

	void PutVision(CPoint2D ul, NPCVisualIn* arr);
	NPCVisualIn* GetVision(void)	{ return (NPCVisualIn*)&view[0]; }
	NPCVMemCell* GetVMemory(void)	{ return (NPCVMemCell*)&memory[0]; }
	CPoint2D GetVisionUL(void)		{ return my_view_ul; }
	NPCCellChangeP* GetCellChange(void);

//	void SetDirectionTo(CPoint2D aim);
//	void SetDirectionTo()			{ SetDirectionTo(my_stats.aim); }
	int GetWantTalkTo(CNPC* one);
	bool StartTalkTo(CNPC* caller);
	void StopTalkTo(CNPC* caller);
	int TalkAbout(int trait);
	void SetDisposition(npcsign_t to, int x);
	int GetDisposition(npcsign_t to);

	void Quantum(void);

private:
	bool PlanBuilding(void);
	bool PlanTalking(void);
	bool StartTalking(CNPC* npc);
	NPCDisposition* FindNPCDisp(npcsign_t to, int* n);
	void PrintDispositions(char* str, int m);
	bool AimTo(CPoint2D aim);
	bool AimTo(void)				{ return AimTo(my_stats.aim); }

	int chrom[CHROM_LENGTH];
	MSMRLCG* rnd;
	int tick;
	NPCState my_state;
	NPCStats my_stats;
	CPoint2D my_coord;
	bool dead;
	NPCVisualIn view[WRLD_CHR_VIEW][WRLD_CHR_VIEW];
	NPCVMemCell memory[WRLD_SIZE_X][WRLD_SIZE_Y];
	CPoint2D my_view_ul; 					//upper-left corner of current view
	npcsign_t signature;
	NPCBuildPlan my_plan;
	NPCDisposition* disposits;
	NPCCellChangeP cur_change;
};

#endif /* NPC_H_ */
