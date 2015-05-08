#ifndef TDSC_H
#define TDSC_H

typedef struct {
	int x, y;
} position;

typedef enum {PLAYER, AI, CRATE} entity;

extern position player_moving_to;
int can_move_to(int x, int y, int fx, int fy, entity type);

#endif