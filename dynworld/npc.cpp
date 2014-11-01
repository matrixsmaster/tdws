/*
 * npc.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "npc.h"
#include "gui.h"

CNPC::CNPC(MSMRLCG* gen)
{
	int i;
	rnd = gen;
	memset(chrom,0,sizeof(chrom));
	for (i = 0; i < CHROM_LENGTH; i++)
		chrom[i] = rnd->RangedNumber(chrom_max[i]) + 1;
	Reset();
}

CNPC::CNPC(MSMRLCG* gen, CNPC* clone)
{
	rnd = gen;
	memcpy(this->chrom,clone->chrom,sizeof(chrom));
	Reset();
}

CNPC::CNPC(MSMRLCG* gen, CNPC* a, CNPC* b)
{
	rnd = gen;
	//TODO: crossover and mutation
	Reset();
}

CNPC::~CNPC(void)
{
	if (disposits) free(disposits);
}

void CNPC::Reset()
{
	int i;
	my_state = NPC_Idle;
	tick = 0;
	memset(&my_stats,0,sizeof(my_stats));
	my_stats.stamina = chrom[CHR_STRGTH];
	dead = false;
	memset(view,0,sizeof(view));
	memset(memory,0,sizeof(memory));
	signature = 0;
	for (i = 0; i < CHAR_SIGN_CRC_Q; i++)
		signature += rnd->NextNumber();
	i = chrom[CHR_DISPML]*sizeof(NPCDisposition);
	disposits = (NPCDisposition*)malloc(i);
	if (!disposits) abort();
	memset(disposits,0,i);
	memset(&cur_change,0,sizeof(cur_change));
}

char CNPC::GetSymbol()
{
	switch (chrom[CHR_GENDER]) {
	case 1: return 'F'; break; //female
	case 2: return 'M'; break; //male
	case 3: return 'G'; break; //gynoid
	case 4: return 'A'; break; //android
	}
	return '?';
}

bool CNPC::PrintInfo(char* str, int m)
{
	if ((!str) || (m < 2)) return false;
	memset(str,0,m);

	char* buf = (char*)malloc(m);
	if (!buf) return false;

	switch (chrom[CHR_GENDER]) {
	case 1: snprintf(str,m-1,"NPC: woman\n"); break;
	case 2: snprintf(str,m-1,"NPC: man\n"); break;
	case 3: snprintf(str,m-1,"NPC: gynoid\n"); break;
	case 4: snprintf(str,m-1,"NPC: android\n"); break;
	}

	snprintf(buf,m-1,"Signature: %ld\n",signature);
	strncat(str,buf,m-1);
	snprintf(buf,m-1,"Age: %d / %d\n",tick,chrom[CHR_LIFESP]);
	strncat(str,buf,m-1);
	snprintf(buf,m-1,"Strength %d\nAttract. %d\n",chrom[CHR_STRGTH],chrom[CHR_ATTRCT]);
	strncat(str,buf,m-1);
	snprintf(buf,m-1,"Laziness %d\n",chrom[CHR_LAZINS]);
	strncat(str,buf,m-1);
	snprintf(buf,m-1,"Traits: %d,%d,%d,%d\n",
			chrom[CHR_TRAITA],chrom[CHR_TRAITB],chrom[CHR_TRAITC],chrom[CHR_TRAITD]);
	strncat(str,buf,m-1);

	memset(buf,0,m);
	switch (my_state) {
	case NPC_Idle:
		snprintf(buf,m-1,"Idling (%d)\n",my_stats.idle_count);
		break;
	case NPC_Walking:
		snprintf(buf,m-1,"Walking\n");
		break;
	case NPC_Running:
		snprintf(buf,m-1,"Running\n");
		break;
	case NPC_Sleeping:
		snprintf(buf,m-1,"Sleeping\n");
		break;
	case NPC_Browsing:
		snprintf(buf,m-1,"Browsing\n");
		break;
	case NPC_Talking:
		snprintf(buf,m-1,"Talking to %ld\n",my_stats.talk_to->GetSign());
		break;
	case NPC_Building:
		snprintf(buf,m-1,"Building\n");
		break;
	default:
		snprintf(buf,m-1,"Unknown state!\n");
	}
	strncat(str,buf,m-1);

	snprintf(buf,m-1,"DIR: %d STA: %d\n",my_stats.direction,my_stats.stamina);
	strncat(str,buf,m-1);
	if (my_stats.aimed) {
		snprintf(buf,m-1,"Aimed: %d:%d\n",my_stats.aim.X,my_stats.aim.Y);
		strncat(str,buf,m-1);
	}
	if (my_stats.building) strncat(str,"Wants to build a home\n",m-1);

	free(buf);
	return true;
}

void CNPC::PutVision(CPoint2D ul, NPCVisualIn* arr)
{
	CPoint2D rp;
	int x,y,lin;
	for (x = 0, lin = 0; x < WRLD_CHR_VIEW; x++) {
		for (y = 0; y < WRLD_CHR_VIEW; y++, lin++) {
			view[x][y] = arr[lin];
			rp = CPoint2D(x,y) + ul;
			if (ispointin(&rp,0,0,WRLD_SIZE_X,WRLD_SIZE_Y)) {
				if (arr[lin].b.typ != CT_Empty)
					memory[rp.X][rp.Y] = arr[lin].b;
			}
		}
	}
	my_view_ul = ul;
}

void CNPC::SetDirectionTo(CPoint2D aim)
{
	getnextpoint(my_coord,aim,&(my_stats.direction));
	if (gui_gettarget() == this) {
		printlog("selects direction %d to %d:%d\n",my_stats.direction,aim.X,aim.Y);
	}
}

void CNPC::Quantum(void)
{
	if (dead) return;

	if (++tick >= chrom[CHR_LIFESP]) {
		dead = true;
		//stop talking if you're dead :)
		if (my_state == NPC_Talking)
			if (my_stats.talk_to) my_stats.talk_to->StopTalkTo(this);
		my_state = NPC_Idle;
		return;
	}

	if (my_stats.aimed) {
		if (my_stats.last_dist < distance(my_stats.aim,my_coord))
			SetDirectionTo();
		my_stats.last_dist = distance(my_stats.aim,my_coord);
	} else
		my_stats.last_dist = 0;

	switch (my_state) {
	case NPC_Idle:
		my_stats.direction = -1;
		if (my_stats.idle_count++ > rnd->RangedNumber(chrom[CHR_LAZINS])) {
			my_stats.idle_count = 0;
			my_state = (my_stats.building)? NPC_Building:NPC_Browsing;
		}
		break;

	case NPC_Running:
		my_stats.stamina--;
		//no break
	case NPC_Walking:
		my_stats.idle_count = 0;
		if (--my_stats.stamina <= 0) {
			my_state = NPC_Sleeping;
			return;
		}
		if (my_coord == my_stats.aim) {
			my_state = (my_stats.building)? NPC_Building:NPC_Browsing;
			return;
		}
		if (my_stats.stuck) {
			my_stats.stuck = false;
			if (my_stats.aimed) {
				if (++my_stats.direction > 7) my_stats.direction = 0;
			} else
				my_state = NPC_Idle;
		}
		break;

	case NPC_Sleeping:
		my_stats.aimed = false;
		my_stats.talk_to = NULL;
		if (my_stats.on_bed)
			my_stats.stamina += 3;
		else
			my_stats.stamina++;
		if (my_stats.stamina >= chrom[CHR_STRGTH])
			my_state = NPC_Idle;
		my_stats.idle_count = 0;
		break;

	case NPC_Browsing:
		if (my_stats.direction >= 0) { //already have some visual information
			if (PlanTalking()) return;
		}
		if (++my_stats.direction >= 8) {//it's time to make a decision
			my_stats.direction = rnd->RangedNumber(8);
			if ((!my_stats.own_home) && (!my_stats.building) && (PlanBuilding()))
				my_state = NPC_Building;
			else
				my_state = NPC_Walking;
		}
		break;

	case NPC_Talking:
		my_stats.aimed = false;
		if (my_stats.idle_count++ > my_stats.stamina) {
			if (my_stats.talk_to) my_stats.talk_to->StopTalkTo(this);
			my_state = NPC_Idle;
		}
		break;

	case NPC_Building:
		my_plan.cur = CPoint2D(my_plan.done/(my_plan.sz.X-1),my_plan.done%(my_plan.sz.X-1));
		my_stats.aim = my_plan.cur + my_plan.ul;
		my_stats.aimed = true;

		if (!my_stats.building) //begin
			my_stats.building = true;

		if (my_coord != my_stats.aim) {
			SetDirectionTo();
			my_state = NPC_Walking;
			return;
		}
		if (cur_change.changed) {
			//continue
			cur_change.want_change = cur_change.changed = false;
			if (++my_plan.done >= my_plan.sz.X*my_plan.sz.Y) {
				//done
				my_stats.building = false;
				my_stats.aimed = false;
				my_stats.direction = 4;
				my_stats.own_home = true;
				my_stats.stamina = 1;
				my_state = NPC_Walking;
				return;
			}
		} else if (!cur_change.want_change) {
			cur_change.want_change = true;
			cur_change.change_to = my_plan.map[my_plan.cur.X][my_plan.cur.Y];
			if (cur_change.change_to == CT_Empty) abort();
		} else {
			if (++my_stats.idle_count > chrom[CHR_STRGTH]) {
				//abandon all hope
				my_stats.building = false;
				my_state = NPC_Idle;
			}
		}
		break;

	default:
		;
	}
}

bool CNPC::PlanBuilding(void)
{
	int i,j,w,h;
	CPoint2D cur;
	memset(&my_plan,0,sizeof(my_plan));
	memset(&cur_change,0,sizeof(cur_change));

	//FIXME: single char only, ugly hardcoded shack, for testing only!
	w = 5;
	h = 4;
	for (i = 0; i < w; i++)
		for (j = 0; j < h; j++) {
			if ((i==0) || (i==4)) {
				my_plan.map[i][j] = CT_HH_Wall;
				continue;
			}
			switch (j) {
			case 0:
				my_plan.map[i][j] = CT_HH_Wall;
				break;
			case 3:
				my_plan.map[i][j] = CT_HH_Door;
				break;
			default:
				my_plan.map[i][j] = CT_HH_Empty;
			}
		}
	my_plan.map[1][1] = CT_HH_Bed;
	my_plan.map[2][1] = CT_HH_Bowl;

	//try to find a place for that
	my_plan.ul = my_coord;
	for (i = -1; i <= w; i++) {
		for (j = -1; j <= h; j++) {
			cur = my_plan.ul + CPoint2D(i,j);
			if (!ispointin(&cur,0,0,WRLD_SIZE_X,WRLD_SIZE_Y))
				return false;
			if ((memory[cur.X][cur.Y].owned) || (!memory[cur.X][cur.Y].routable))
				return false;
		}
	}
	my_plan.sz = CPoint2D(w,h);
	return true;
}

bool CNPC::PlanTalking(void)
{
	int i,j,x,wants = 0;
	CNPC* npc = NULL;
	//gather all NPCs in current view
	for (i = 0; i < WRLD_CHR_VIEW; i++)
		for (j = 0; j < WRLD_CHR_VIEW; j++) {
			if (view[i][j].npc) {
				x = GetWantTalkTo(view[i][j].npc);
				if (x > wants) {
					npc = view[i][j].npc;
					wants = x;
				}
			}
		}
	if ((!npc) || (wants < 1)) return false;
	//if NPC is just right here, start talking
	if (distance(my_coord,npc->GetCrd()) < 1.5f)
		return StartTalking(npc);
	else {
		my_stats.aim = npc->GetCrd();
		my_stats.aimed = true;
		my_state = NPC_Running;
	}
	return true;
}

int CNPC::GetWantTalkTo(CNPC* one)
{
	int i,val;
	int aa = chrom[CHR_ATTRCT];
	int ba = one->GetGenome()[CHR_ATTRCT];

	if (one->GetGenome()[CHR_LIFESP] - one->GetAge() < CHR_TALKMAXAGE)
		return 0;
	if (ba >= aa - rnd->RangedNumber(aa/2)) val = 2;
	else return 0;
	val += ba - aa;
	val += (int)ceil(2.f / distance(one->GetCrd(),my_coord) * CHR_TALKDISTMUL);
	aa = chrom[CHR_GENDER];
	ba = one->GetGenome()[CHR_GENDER];
	if (aa != ba) val *= 2;
	if ((aa % 2) != (ba % 2)) val *= 2;

	for (i = 0; i < chrom[CHR_DISPML]; i++) {
		if (!disposits[i].who) break;
		else if (disposits[i].who == one->GetSign()) {
			val += disposits[i].disp;
			break;
		}
	}

	if (gui_gettarget() == this) {
		printlog("wants to talk to %ld as much as %d\n",one->GetSign(),val);
		if ((i < chrom[CHR_DISPML]) && (disposits[i].who == one->GetSign()))
			printlog("disposition %d\n",disposits[i].disp);
	}

	return val;
}

bool CNPC::StartTalking(CNPC* npc)
{
	if (!npc->StartTalkTo(this)) return false;
	my_stats.talk_to = npc;
	my_state = NPC_Talking;
	my_stats.idle_count = 0;
	return true;
}

bool CNPC::StartTalkTo(CNPC* caller)
{
	switch (my_state) {
	case NPC_Sleeping:
	case NPC_Talking:
	case NPC_Building:
		return false;
	default:
		;
	}
	if (GetWantTalkTo(caller) < 1) return false;
	my_stats.talk_to = caller;
	my_state = NPC_Talking;
	return true;
}

void CNPC::StopTalkTo(CNPC* caller)
{
	if ((my_stats.talk_to == caller) && (my_state == NPC_Talking)) {
		my_state = NPC_Idle;
		my_stats.talk_to = NULL;
	}
}

NPCCellChangeP* CNPC::GetCellChange(void)
{
	if (!cur_change.want_change) return NULL;
	return &cur_change;
}
