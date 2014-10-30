/*
 * npc.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "npc.h"

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
	//
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
	}
	strncat(str,buf,m-1);

	snprintf(buf,m-1,"DIR: %d STA: %d\n",my_stats.direction,my_stats.stamina);
	strncat(str,buf,m-1);

	snprintf(buf,m-1,"Assumes pos %d:%d\n",my_coord.X,my_coord.Y);
	strncat(str,buf,m-1);

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
				if (memory[rp.X][rp.Y].typ == CT_Empty)
					memory[rp.X][rp.Y] = arr[lin].b;
			}
		}
	}
	my_view_ul = ul;
}

void CNPC::Quantum(void)
{
	if (dead) return;
	if (++tick >= chrom[CHR_LIFESP]) {
		dead = true;
		my_state = NPC_Idle;
		return;
	}
	switch (my_state) {
	case NPC_Idle:
		my_stats.direction = -1;
		if (my_stats.idle_count++ > rnd->RangedNumber(chrom[CHR_LAZINS]))
			my_state = NPC_Browsing;
		break;

	case NPC_Running:
		my_stats.stamina--;
		//no break
	case NPC_Walking:
		if (--my_stats.stamina <= 0)
			my_state = NPC_Sleeping;
		if (my_stats.stuck) {
			my_stats.stuck = false;
			my_state = NPC_Idle;
		}
		my_stats.idle_count = 0;
		break;

	case NPC_Sleeping:
		if (my_stats.on_bed)
			my_stats.stamina += 3;
		else
			my_stats.stamina++;
		if (my_stats.stamina >= chrom[CHR_STRGTH])
			my_state = NPC_Idle;
		my_stats.idle_count = 0;
		break;

	case NPC_Browsing:
		if (++my_stats.direction >= 8) {
			my_stats.direction = rnd->RangedNumber(8);
			my_state = NPC_Walking;
		}
		break;

	default:
		;
	}
}
