/*
 * gui.cpp
 *
 *  Created on: Oct 27, 2014
 *      Author: matrixsmaster
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "CPoint2D.h"
#include "gui.h"
#include "gui_help.h"
#include "crazymath.h" //for debug only

static WINDOW *scrn, *field, *view, *log;
static struct SGUISet mygui;
static CWCell** myfield;
static CPoint2D wrldsize,base,center;
CPoint2D oldpnt;

static void remwindows()
{
	if (log) delwin(log);
    if (view) delwin(view);
    if (field) delwin(field);
    
    if (mygui.log) {
		while (mygui.lh--) free(mygui.log[mygui.lh]);
		free(mygui.log);
	}

    memset(&mygui,0,sizeof(mygui));
    field = view = log = NULL;
}

static void newgui()
{
	int w,h;
	remwindows();
	getmaxyx(scrn,h,w);
	
	int fw = w*2/3;
    int fh = h*2/3;
    field = subwin(scrn,fh,fw,0,0);
    
    int vw = w - fw;
    int vh = fh;
    view = subwin(scrn,vh,vw,0,fw);
    
    int lw = w;
    int lh = h - fh;
    log = subwin(scrn,lh,lw,fh,0);
	
	mygui.w = w;
	mygui.h = h;
	mygui.fw = fw;
	mygui.fh = fh;
	mygui.vw = vw;
	mygui.vh = vh;
	mygui.lw = lw;
	mygui.lh = lh;
	
	mygui.log = (char**)malloc(lh*sizeof(char*));
	if (!mygui.log) abort();
	while (lh--) {
		mygui.log[lh] = (char*)malloc(lw);
		if (!mygui.log[lh]) abort();
		memset(mygui.log[lh],0,lw);
	}

	center = CPoint2D(fw/2,fh/2);
	mygui.zoom = 1;
}

static inline void m_wupdate(WINDOW* wnd)
{
	box(wnd,0,0);
	wrefresh(wnd);
}

static inline void m_wupdate_all()
{
	m_wupdate(field);
	m_wupdate(view);
	m_wupdate(log);
	refresh();
}

static void drawlog()
{
	int i;
	werase(log);
	for (i = 0; i < mygui.lh - 2; i++)
		mvwaddnstr(log,i+1,1,mygui.log[i],mygui.lw-2);
	m_wupdate(log);
}

static void shiftlog()
{
	int i;
	for (i = 0; i < mygui.lh - 3; i++)
		memcpy(mygui.log[i],mygui.log[i+1],mygui.lw);
	memset(mygui.log[mygui.lh-3],0,mygui.lw);
}

static void shorthelp()
{
	int i = 0;
	while (strlen(gui_help_table[i]) > 0)
		putlogstr(gui_help_table[i++]);
}

void putlogstr(const char* str)
{
	int i;
	int l = strlen(str);
	if (l < 1) return;
	if (str[l-1] == '\n') l--; //remove trailing LF
	//FIXME: faster algorithm is easily possible, but i'm too lazy now
	char* cs = (char*)str;
	char* es = mygui.log[mygui.lh-3];
	//while (p--) {
	do {
		shiftlog();
		for (i = 0; (i < l) && (i < mygui.lw-2); i++)
			if (cs[i] != '\n')
				es[i] = cs[i];
			else {
				i++;
				break;
			}
		cs += i;
		l -= i;
	} while (l > 0);
	drawlog();
}

void printlog(char const* format,...)
{
	char buf[LOGSTRBUF];
	va_list msg;

	va_start(msg,format);
	vsnprintf(buf,LOGSTRBUF-1,format,msg);
	va_end(msg);

	putlogstr(buf);
}

void setfield(CWCell** buf, const int w, const int h)
{
	myfield = buf;
	wrldsize = CPoint2D(w,h);
	base = CPoint2D(0);
	mygui.target = NULL;
	mygui.showvismem = false;
	mygui.zoom = 1;
}

void drawfield()
{
	char* lbuf,prev;
	int i,j,k,x,y,l;

	if (!myfield) return;

	lbuf = (char*)malloc(mygui.fw);
	if (!lbuf) return;

	werase(field);
	for (i = 0; i < mygui.fh-2; i++) {
		y = (i + base.Y) * mygui.zoom;
		l = 0;
		prev = 0;

		if ((y < 0) || (y >= wrldsize.Y)) {
			wcolor_set(field,1,NULL);
			memset(lbuf,' ',mygui.fw);
		} else {

			for (j = 0; j < mygui.fw-2; j++) {
				x = (j + base.X) * mygui.zoom;

				if (mygui.target && mygui.showvismem) {
					lbuf[j] = ((x < wrldsize.X) && (x >= 0))?
							mygui.target->GetVMemory()[x*wrldsize.Y+y].symbol:' ';
//					lbuf[j] = ((x < wrldsize.X) && (x >= 0))?
//							vismemory[x*wrldsize.Y+y]:' ';
					if (!lbuf[j]) lbuf[j] = '`'; //do not append zero bytes
				} else {
					lbuf[j] = ((x < wrldsize.X) && (x >= 0))?
							myfield[x*wrldsize.Y+y]->Print(false):' ';
				}

				if (prev != lbuf[j]) {
					if (prev) //put out accumulated part
						mvwaddnstr(field,i+1,l+1,lbuf+l,j-l);

					wcolor_set(field,1,NULL); //reset color
					//and seek for new one
					for (k = 0; k < COLPAIRS; k++)
						if (lbuf[j] == gui_coltable[k].sym) {
							wcolor_set(field,k+1,NULL);
							break;
						}
					prev = lbuf[j];
					l = j;
				}
			}
		}

		mvwaddnstr(field,i+1,l+1,lbuf+l,mygui.fw-2-l); //remainder
	}

	wcolor_set(field,1,NULL);
	m_wupdate(field);

	free(lbuf);
}

void drawinfo()
{
	int i,l,c;
	char str[VIEWSTRBUF];
	char* cs;
	CPoint2D pnt = (center + base) * mygui.zoom;
	CWCell* ptr;

	werase(view);
	snprintf(str,VIEWSTRBUF-1,"X %d  Y %d",pnt.X,pnt.Y);
	mvwaddnstr(view,1,1,str,mygui.vw-2);

	if ((pnt.X >= 0) && (pnt.X < wrldsize.X) &&
			(pnt.Y >= 0) && (pnt.Y < wrldsize.Y)) {

		ptr = myfield[pnt.X*wrldsize.Y+pnt.Y];

		if (ptr->PrintInfo(str,VIEWSTRBUF)) {
			l = 3;
			i = c = 0;
			cs = str;
			while (i < (int)strlen(str)) {
				if ((c >= mygui.vw-3) || (str[i] == '\n')) {
					mvwaddnstr(view,l++,1,cs,c);
					c = 0;
					cs = str + i + 1;
				}
				c++; i++;
			}
		}
	}

	m_wupdate(view);
}

bool gui_prockey(int key)
{
	bool res = true;
	switch (key) {
	case KEY_UP:	base.Y--; break;
	case KEY_DOWN:	base.Y++; break;
	case KEY_LEFT:	base.X--; break;
	case KEY_RIGHT:	base.X++; break;
	case KEY_HOME:	base = CPoint2D(0); break;
	case KEY_END:	base = wrldsize - CPoint2D(mygui.fw-2,mygui.fh-2); break;
	case '=':		mygui.zoom = (mygui.zoom > 1)? mygui.zoom-1:1; break;
	case '-':		mygui.zoom++; break;
	case '0':		mygui.zoom = 1; break;
	case 'T':		mygui.target = NULL; break;
	case KEY_F(10):	shorthelp(); break; //was F1, but xfce terminal binds it :(
	case KEY_F(2):	mygui.showvismem = !mygui.showvismem; break;
	default:
		res = false;
	}
	//if (res) updategui();
	return res;
}

CNPC* gui_gettarget()
{
	return mygui.target;
}

void gui_settarget(CNPC* trg)
{
	mygui.target = trg;
	if (!trg) mygui.showvismem = false;
	else oldpnt = trg->GetCrd();
}

CPoint2D gui_getcursor()
{
	CPoint2D r = (base + center) * mygui.zoom;
	if (r.X < 0) r.X = 0;
	if (r.X >= WRLD_SIZE_X) r.X = WRLD_SIZE_X - 1;
	if (r.Y < 0) r.Y = 0;
	if (r.Y >= WRLD_SIZE_Y) r.Y = WRLD_SIZE_Y - 1;
	return r;
}

void gui_update()
{
	erase();
	gui_cnt++;
	if (mygui.target) {
		float d = distance(mygui.target->GetCrd(),oldpnt);
		if (d > 2.0)
			abort();
		base = mygui.target->GetCrd() - center;
		oldpnt = mygui.target->GetCrd();
	}
	drawfield();
	drawinfo();
	drawlog();
	mvchgat(center.Y+1,center.X+1,1,A_REVERSE | A_BOLD,0,NULL);
	move(0,0);
	m_wupdate_all();
}

bool gui_checksize()
{
	int w,h;
	if (!scrn) return false;
	getmaxyx(scrn,h,w);
	return (!((mygui.w != w) || (mygui.h != h)));
}

int gui_init()
{
	short i;
	field = view = log = NULL;
	myfield = NULL;
	memset(&mygui,0,sizeof(mygui));

	scrn = initscr();
	if (!scrn) return 1;
	if (has_colors() == FALSE) return 2;
    
    noecho();
    cbreak();
    keypad(scrn,TRUE);
    start_color();
    
    if (COLOR_PAIRS < COLPAIRS) {
    	gui_kill();
    	return 3;
    }

#ifdef DOOWNCOLORS
    if (can_change_color()) {
    	i = 0;
    	while (gui_termcol[i].r >= 0) {
    		init_color(gui_termcol[i].cnam,
    				gui_termcol[i].r,gui_termcol[i].g,gui_termcol[i].b);
    		i++;
    	}
    }
#endif

    for (i = 0; i < COLPAIRS; i++)
    	init_pair(i+1,gui_coltable[i].f,gui_coltable[i].b);

    newgui();
    m_wupdate_all();
    refresh();
    
    return 0;
}

void gui_kill()
{
	remwindows();
	if (scrn) delwin(scrn);
	endwin();
	refresh();
}

void gui_softreset()
{
	wrefresh(scrn);
	remwindows();
	newgui();
	wrefresh(scrn);
	clear();
	refresh();
	gui_update();
}
