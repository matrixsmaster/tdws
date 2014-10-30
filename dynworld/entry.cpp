/*
 * entry.cpp
 *
 *  Created on: Oct 26, 2014
 *      Author: matrixsmaster
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include "world.h"
#include "gui.h"

#define DEFAULT_SEED 3968230L

static CWorld* world = NULL;

int main(int argc, char* argv[])
{
	int key,ret;
	long seed = 0;

	ret = gui_init();
	if (ret) {
		fprintf(stderr,"Error init ncurses (err %d)\n",ret);
		exit(EXIT_FAILURE);
	}

	if (argc > 1)
		seed = atol(argv[1]);
	if (seed == 0) seed = DEFAULT_SEED;
	else printlog("Using seed from command-line argument\n");

	world = new CWorld(seed);

	do {
		ret = halfdelay(1);
		if (ret != ERR) key = getch();
		else key = 0;

		switch (key) {
		case 'R':
			printlog("REGENERATING\n");
			delete world;
			world = new CWorld(0); //use time seed
			break;
		case 'S':
			printlog("Init seed for this world was %ld\n",world->getInitSeed());
			break;
		default:
			world->ProcKey(key);
		}

		if (!gui_checksize()) {
			fprintf(stderr,"Resetting GUI...\n");
			sleep(1);
			gui_softreset();
		}
	} while (!world->isReadyToQuit());

	delete world;

	gui_kill();
	printf("\nexit()\n");
	return 0;
}
