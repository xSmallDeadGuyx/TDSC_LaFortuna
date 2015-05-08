#include "stdlib.h"
#include "math.h"
#include "pathfinder.h"

int calc_heuristic(pathfinder pf, int sx, int sy) {
	return 10 * (abs(sx - pf.end.x) + abs(sy - pf.end.y));
}

int get_distance(int sx, int sy, int ex, int ey) {
	int dx = sx - ex;
	int dy = sy - ey;
	return (int) round(10 * sqrt(dx * dx + dy * dy));
}

position get_lowest(pathfinder pf, position *positions, int num) {
	int lowest = -1;
	int i;
	position found;
	found.x = -1;
	found.y = -1;
	for(i = 0; i < num; i++) {
		position p = positions[i];
		position c = pf.came_from[p.x][p.y];
		int d = (c.x == -1 || c.y == -1 ? 0 : pf.g_score[c.x][c.y] + get_distance(p.x, p.y, c.x, c.y) + calc_heuristic(pf, p.x, p.y));
		if(d <= lowest || lowest == -1) {
			lowest = d;
			found = p;
		}
	}
	return found;
}

int pf_can_move_to(pathfinder pf, int x, int y, int fx, int fy) {
	if(pf.end.x == x && pf.end.y == y)
		return 1;
	if(player_moving_to.x == x && player_moving_to.y == y)
		return 1;
	return can_move_to(x, y, fx, fy, pf.type);
}

void get_neighbours(pathfinder pf, int x, int y) {
	int i = 0;

	if(pf_can_move_to(pf, x, y, x + 1, y)) {
		position p;
		p.x = x + 1;
		p.y = y;
		pf.found[i] = p;
		i++;
	}
	if(pf_can_move_to(pf, x, y, x - 1, y)) {
		position p;
		p.x = x - 1;
		p.y = y;
		pf.found[i] = p;
		i++;
	}
	if(pf_can_move_to(pf, x, y, x, y + 1)) {
		position p;
		p.x = x;
		p.y = y + 1;
		pf.found[i] = p;
		i++;
	}
	if(pf_can_move_to(pf, x, y, x, y - 1)) {
		position p;
		p.x = x;
		p.y = y - 1;
		pf.found[i] = p;
		i++;
	}

	for(; i < 4; i++) {
		position p;
		p.x = -1;
		p.y = -1;
		pf.found[i] = p;
	}
}

find_move(pathfinder pf) {
	position open[200];
	position closed[200];
	int num_open = 1;
	int num_closed = 0;

	open[0] = pf.start;
	pf.g_score[pf.start.x][pf.start.y] = 0;
	pf.h_score[pf.start.x][pf.start.y] = calc_heuristic(pf, pf.start.x, pf.start.y);
	pf.f_score[pf.start.x][pf.start.y] = pf.h_score[pf.start.x][pf.start.y];

	while(num_open > 0) {
		int i;
		int j;
		position point = get_lowest(pf, open, num_open);
		pf.move.x = -1;
		pf.move.y = -1;

		if(point.x == pf.end.x && point.y == pf.end.y) {
			position move;
			position from;
			move.x = point.x;
			move.y = point.y;
			from = pf.came_from[move.x][move.y];
			while(from.x != pf.start.x || from.y != pf.start.y) {
				move.x = from.x;
				move.y = from.y;
				from = pf.came_from[move.x][move.y];
			}
			pf.move = move;
			return;
		}

		num_open--;
		closed[num_closed] = point;
		num_closed++;

		get_neighbours(pf, point.x, point.y);
		for(i = 0; i < 4 && pf.found[i].x != -1 && pf.found[i].y != -1; i++) {
			int in_closed = 0;
			int in_open = 0;
			int g_possible;
			for(j = 0; j < num_closed; j++)
				if(closed[j].x == pf.found[i].x && closed[j].y == pf.found[j].y) {
					in_closed = 1;
					break;
				}
			if(in_closed)
				continue;

			g_possible = pf.g_score[point.x][point.y] + get_distance(point.x, point.y, pf.found[i].x, pf.found[i].y);
			for(j = 0; j < num_open; j++)
				if(open[j].x == pf.found[i].x && open[j].y == pf.found[j].y) {
					in_open = 1;
					break;
				}
			if(!in_open || (in_open && g_possible < pf.g_score[pf.found[i].x][pf.found[i].y])) {
				if(!in_open) {
					open[num_open] = pf.found[i];
					num_open++;
				}
				pf.came_from[pf.found[i].x][pf.found[i].y] = point;
				pf.g_score[pf.found[i].x][pf.found[i].y] = g_possible;
				pf.h_score[pf.found[i].x][pf.found[i].y] = calc_heuristic(pf, pf.found[i].x, pf.found[i].y);
				pf.f_score[pf.found[i].x][pf.found[i].y] = g_possible + pf.h_score[pf.found[i].x][pf.found[i].y];
			}
		}
	}
}