/* COMP2215 Task 6  Model Answer */

#include "os.h"
#include "maps.h"
#include "tdsc.h"

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
position ais[10];
position blocks[100];
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
					num_ais++;
					break;
				case 5:
					col = BLUE; 
					blocks[num_blocks].x = x;
					blocks[num_blocks].y = y;
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
	if(level < 10) {
		level++;
		load_level();
	}
}

int is_free(int x, int y) {
	int i;

	if(x < 0 || y < 0 || x > 20 || y > 20)
		return 0;

	if(levels[level][y][x] == 1)
		return 0;

	for(i = 0; i < num_ais; i++) {
		if(ais[i].x == x && ais[i].y == y) {
			return 0;
		}
	}

	for(i = 0; i < num_blocks; i++) {
		if(blocks[i].x == x && blocks[i].y == y) {
			return 0;
		}
	}

	return 1;
}

void move_player(int dx, int dy) {
	int nx = player.x + dx;
	int ny = player.y + dy;
	
	if(is_free(nx, ny)) {
		if(levels[level][ny][nx] == 3) {
			next_level();
		}
		else {
			fill_rectangle(get_rect(player.x, player.y), WHITE);
			fill_rectangle(get_rect(nx, ny), GREEN);
			player.x = nx;
			player.y = ny;
		}
	}
	else {
		int i;
		for(i = 0; i < num_blocks; i++) {
			if(blocks[i].x == nx && blocks[i].y == ny) {
				int bx = nx + dx;
				int by = ny + dy;

				if(is_free(bx, by) && levels[level][by][bx] != 7) {
					if(levels[level][ny][nx] == 3) {
						next_level();
						break;
					}
					else {
						fill_rectangle(get_rect(player.x, player.y), WHITE);
						fill_rectangle(get_rect(nx, ny), GREEN);
						fill_rectangle(get_rect(bx, by), BLUE);
						blocks[i].x = bx;
						blocks[i].y = by;
						player.x = nx;
						player.y = ny;
						break;
					}
				}
			}
		}
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