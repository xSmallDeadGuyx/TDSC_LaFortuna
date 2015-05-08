#include "stdio.h"
#include "os.h"
#include "maps.h"
#include "tdsc.h"
#include "pathfinder.h"

int check_switches(int);
void init_game();

/* Lines have to be shorter than this. Note: buffer is on stack. */
#define LINE_BUFF_LEN 55

#define WIDTH  320
#define HEIGHT 240

void main(void) {
    os_init();

    os_add_task(check_switches,  40, 1);
    
    init_game();

    sei();
    for(;;){}
    
}

void display_string_xy_centered(char *str, uint16_t x, uint16_t y) {
	int c;
	for(c = 0; str[c]; c++);
	display_string_xy(str, x - (c * 6) / 2, y - 4);
}


int menu = 1;
int level = 0;

void init_game() {
	display_color(BLACK, WHITE);
	clear_screen();

	display_string_xy_centered("They Don't Stop Coming", WIDTH/2, HEIGHT/2);
}

position player;
position player_moving_to;
position ais[10];
position ai_moving_to[10];
position blocks[100];
position blocks_moving_to[100];
int num_ais;
int num_blocks;

rectangle get_rect(int x, int y) {
	rectangle r;
	r.left = WIDTH/2 + (x - 10) * 10 - 5;
	r.right = r.left + 9;
	r.top = y * 10;
	r.bottom = r.top + 9;
	return r;
}

void load_level() {
	int x;
	int y;

	num_ais = 0;
	num_blocks = 0;

	clear_screen();
	for(y = 0; y < 21; y++) {
		for(x = 0; x < 21; x++) {
			uint16_t col;
			switch(levels[level][y][x]) {
				case 1: 
					col = BLACK;
					break;
				case 2: 
					col = GREEN; 
					player.x = x;
					player.y = y;
					break;
				case 3:  
					col = YELLOW;
					break;
				case 4:  
					col = RED;
					ais[num_ais].x = x;
					ais[num_ais].y = y;
					ai_moving_to[num_ais].x = x;
					ai_moving_to[num_ais].y = y;
					num_ais++;
					break;
				case 5:
					col = BLUE; 
					blocks[num_blocks].x = x;
					blocks[num_blocks].y = y;
					blocks_moving_to[num_blocks].x = x;
					blocks_moving_to[num_blocks].y = y;
					num_blocks++;
					break;
				case 6:
					col = GRAY;
					break;
				case 7:
					col = LIGHT_CYAN;
					break;
				default:
					col = WHITE;
			}

			fill_rectangle(get_rect(x, y), col);
		}
	}
	display_string_xy(level_text[level], 40, 215);
}

void next_level() {
	if(level < 4) {
		level++;
		load_level();
	}
}

int can_move_to(int x, int y, int fx, int fy, entity type) {
	int i;

	if(fx < 0 || fy < 0 || fx > 20 || fy > 20)
		return 0;

	if(levels[level][fy][fx] == 1)
		return 0;

	if(type == AI && levels[level][fy][fx] == 6)
		return 0;

	if(type == CRATE && levels[level][fy][fx] == 7)
		return 0;

	if(type != PLAYER && fx == player_moving_to.x && fy == player_moving_to.y)
		return 0;

	for(i = 0; i < num_ais; i++)
		if(type != AI && ai_moving_to[i].x == fx && ai_moving_to[i].y == fy)
			return 0;

	for(i = 0; i < num_blocks; i++) {
		if(type != CRATE && blocks[i].x == fx && blocks[i].y == fy)
			return can_move_to(fx, fy, fx + (fx - x), fy + (fy - y), CRATE);
		else if(type != CRATE && blocks_moving_to[i].x == fx && blocks_moving_to[i].y == fy)
			return 0;
	}

	return 1;
}

void move_player(int dx, int dy) {
	int nx = player.x + dx;
	int ny = player.y + dy;
	
	if(can_move_to(player.x, player.y, nx, ny, PLAYER)) {
		int i;
		player_moving_to.x = nx;
		player_moving_to.y = ny;

		if(levels[level][player_moving_to.y][player_moving_to.x] == 3) {
			next_level();
			return;
		}

		for(i = 0; i < num_blocks; i++) {
			if(blocks[i].x == nx && blocks[i].y == ny) {
				blocks_moving_to[i].x = nx + dx;
				blocks_moving_to[i].y = ny + dy;
			}
		}

		for(i = 0; i < num_ais; i++) {
			pathfinder pf;
			pf.start = ais[i];
			pf.end = player;
			pf.type = AI;

			display_string_xy("gets here", 0, 0);

			ai_moving_to[i] = ais[i];
			find_move(pf);
			if(pf.move.x != -1 && pf.move.y != -1) {
				if(can_move_to(ais[i].x, ais[i].y, pf.move.x, pf.move.y, AI)) {
					int j;
					ai_moving_to[i] = pf.move;

					for(j = 0; j < num_blocks; j++) {
						if(blocks[j].x == pf.move.x && blocks[j].y == pf.move.y) {
							blocks_moving_to[j].x = blocks[j].x + pf.move.x - ais[i].x;
							blocks_moving_to[j].y = blocks[j].y + pf.move.y - ais[i].y;
						}
					}
				}
			}
		}

		
		for(i = 0; i < num_blocks; i++) {
			fill_rectangle(get_rect(blocks_moving_to[i].x, blocks_moving_to[i].y), BLUE);
			blocks[i].x = blocks_moving_to[i].x;
			blocks[i].y = blocks_moving_to[i].y;
		}

		for(i = 0; i < num_ais; i++) {
			fill_rectangle(get_rect(ais[i].x, ais[i].y), WHITE);
			fill_rectangle(get_rect(ai_moving_to[i].x, ai_moving_to[i].y), RED);
			ais[i].x = ai_moving_to[i].x;
			ais[i].y = ai_moving_to[i].y;
		}

		fill_rectangle(get_rect(player.x, player.y), WHITE);
		fill_rectangle(get_rect(player_moving_to.x, player_moving_to.y), GREEN);
		player.x = player_moving_to.x;
		player.y = player_moving_to.y;
	}
}

int check_switches(int state) {
	if (get_switch_press(_BV(SWN))) {
		if(!menu) {
			move_player(0, -1);
		}
	}
		
	if (get_switch_press(_BV(SWE))) {
		if(!menu) {
			move_player(1, 0);
		}
	}
		
	if (get_switch_press(_BV(SWS))) {
		if(!menu) {
			move_player(0, 1);
		}
    }
		
	if (get_switch_press(_BV(SWW))) {
		if(!menu) {
			move_player(-1, 0);
		}
	}
		
	if (get_switch_press(_BV(SWC))) {
		if(menu) {
			menu = 0;
			level = 0;
			load_level();
		}
		else {
			load_level();
		}
	}

	return state;
}