/*
 * world_const.h
 *
 *  Created on: Oct 28, 2014
 *      Author: matrixsmaster
 */

#ifndef WORLD_CONST_H_
#define WORLD_CONST_H_


#define WRLD_SIZE_X 200
#define WRLD_SIZE_Y 100

#define WRLD_GEN_FORK_P 75

#define WRLD_FIELD 30
#define WRLD_WILDR 10
#define WRLD_MOUNT 7

enum CellType {
	CT_Empty = 0,
	CT_Water,
	CT_Sand,
	CT_Field,
	CT_Forest,
	CT_Mount,
	CT_HH_Wall,
	CT_HH_Door,
	CT_HH_Bed,
	CT_HH_Bowl,
	CT_HH_Empty
};

#define WRLD_INIT_POP 0.5

#define CHR_MAXLIFESPAN 20123
#define CHR_NUMTRAITS 8
#define CHR_TALKDISTMUL 10
#define CHR_TALKMAXAGE 4

#define CHR_GENDER 0
#define CHR_STRGTH 1
#define CHR_ATTRCT 2
#define CHR_LIFESP 3
#define CHR_TRAITA 4
#define CHR_TRAITB 5
#define CHR_TRAITC 6
#define CHR_TRAITD 7
#define CHR_LAZINS 8
#define CHR_DISPML 9

#define CHROM_LENGTH 10

static const int chrom_max[CHROM_LENGTH] = {
		4,300,100,CHR_MAXLIFESPAN,
		CHR_NUMTRAITS,CHR_NUMTRAITS,
		CHR_NUMTRAITS,CHR_NUMTRAITS,
		50,50
};

#define CHAR_SIGN_CRC_Q 3

typedef long npcsign_t;

static const int direction_table[16] = {
		0,-1, 1,-1, 1,0, 1,1, 0,1, -1,1, -1,0, -1,-1
};

//view 'distance' must be an odd value
#define WRLD_CHR_VIEW 9
//to shorten the cone table
#define WRLD_CHR_VIEW_HALF (WRLD_CHR_VIEW/2)

static const int view_cone_table[16] = {
		WRLD_CHR_VIEW_HALF,WRLD_CHR_VIEW,
		-1,WRLD_CHR_VIEW,
		-1,WRLD_CHR_VIEW_HALF,
		-1,-1,
		WRLD_CHR_VIEW_HALF,-1,
		WRLD_CHR_VIEW,-1,
		WRLD_CHR_VIEW,WRLD_CHR_VIEW_HALF,
		WRLD_CHR_VIEW,WRLD_CHR_VIEW
};

#endif /* WORLD_CONST_H_ */
