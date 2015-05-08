#ifndef PF_H
#define PF_H

#include "tdsc.h"

typedef struct {
	position start;
	position end;
	int g_score[21][21];
	int f_score[21][21];
	int h_score[21][21];
	position came_from[21][21];
	position found[4];
	entity type;

	position move;
} pathfinder;

void find_move(pathfinder pf);

#endif