/*
 * gui.h
 *
 *  Created on: Oct 27, 2014
 *      Author: matrixsmaster
 *
 *  Mixed out of some curses' C code, so not purely perfect C++ :-/
 */

#ifndef GUI_H_
#define GUI_H_

#include <ncurses.h>
#include "cell.h"
#include "CPoint2D.h"

#define LOGSTRBUF 1024
#define VIEWSTRBUF 2048
#define COLPAIRS 8
//#define DOOWNCOLORS

struct SGUIWCol {
	char sym;
	short f,b;
};

struct SGUITCol {
	short cnam;
	int r,g,b;
};

struct SGUISet {
	int w,h;
	int fw,fh;
	int vw,vh;
	int lw,lh;
	char** log;
	CNPC* target;
	int zoom;
	bool showvismem;
};


static const struct SGUIWCol gui_coltable[COLPAIRS] = {
		{' ',COLOR_WHITE,COLOR_BLACK}, //the first one is default
		{'~',COLOR_WHITE,COLOR_BLUE},
		{'.',COLOR_YELLOW,COLOR_BLACK},
		{'_',COLOR_BLACK,COLOR_YELLOW},
		{'!',COLOR_YELLOW,COLOR_GREEN},
		{'F',COLOR_BLACK,COLOR_MAGENTA},
		{'M',COLOR_YELLOW,COLOR_BLUE},
		{'G',COLOR_MAGENTA,COLOR_BLACK}
};

#ifdef DOOWNCOLORS
static const struct SGUITCol gui_termcol[] = {
		{COLOR_BLACK,0,0,0},
		{COLOR_WHITE,1000,1000,1000},
		{COLOR_RED,1000,0,0},
		{COLOR_GREEN,0,1000,0},
		{COLOR_BLUE,0,0,1000},
		{COLOR_YELLOW,1000,1000,0},
		{COLOR_MAGENTA,1000,0,1000},
		{0,-1,-1,-1}
};
#endif


int gui_init();
void gui_kill();
void gui_update();
bool gui_checksize();
void gui_softreset();
bool gui_prockey(int key);
CNPC* gui_gettarget();
void gui_settarget(CNPC* trg);
CPoint2D gui_getcursor();
void putlogstr(const char* str);
void printlog(char const* format,...);
void setfield(CWCell** buf, const int w, const int h);
void drawfield();
void drawinfo();

#endif /* GUI_H_ */
