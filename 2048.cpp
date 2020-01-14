
//  https://github.com/cuadue/2048_game/blob/master/2048_game.c

#include <string>
#include <string.h>
#include <memory>
#include <cstdlib>
#include <array>
#include <assert.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>

#include "2048.hpp"

#define max(a, b) ((a) > (b) ? (a) : (b))

#define NROWS 4
#define NCOLS NROWS

typedef int tile;
int last_turn;

int opt;
int chgt;
uint16_t last_buttons = 0;

	
struct game {
	int turns, score;
	tile board[NROWS][NCOLS];
};

struct game games;



static int delay_ms = 250;
static int batch_mode;


using namespace blit;

const uint16_t screen_width = 320;
const uint16_t screen_height = 240;

spritesheet *ss_tile = spritesheet::load(packed_data);


// place_tile() returns 0 if it did place a tile and -1 if there is no open
// space.
int place_tile(struct game *game)
{
	// lboard is the "linear board" -- no need to distinguish rows/cols
	tile *lboard = (tile *)game->board;
	int i, num_zeros = 0;

	// Walk the board and count the number of empty tiles
	for (i = 0; i < NROWS * NCOLS; i++) {
		num_zeros += lboard[i] ? 0 : 1;
	}

	if (!num_zeros) {
		return -1;
	}

	// Choose the insertion point
	int loc = rand() % num_zeros;

	// Find the insertion point and place the new tile
	for (i = 0; i < NROWS * NCOLS; i++) {
		if (!lboard[i] && !(loc--)) {
			lboard[i] = rand() % 10 ? 1 : 2;
			return 0;
		}
	}
	assert(0);
}

void print_tile(int tile,int crow,int ccol)
{
	point position(crow*20 + 2, ccol*20 + 2);
	
	if (tile) 
	{
		if (1 << tile == 2) fb.blit(ss_tile, rect(0, 0, 20, 20), position);
		if (1 << tile == 4) fb.blit(ss_tile, rect(0, 20, 20, 20), position);
		if (1 << tile == 8) fb.blit(ss_tile, rect(0, 40, 20, 20), position);
		if (1 << tile == 16) fb.blit(ss_tile, rect(0, 60, 20, 20), position);
		if (1 << tile == 32) fb.blit(ss_tile, rect(0, 80, 20, 20), position);
		if (1 << tile == 64) fb.blit(ss_tile, rect(20, 0, 20, 20), position);
		if (1 << tile == 128) fb.blit(ss_tile, rect(20, 20, 20, 20), position);
		if (1 << tile == 256) fb.blit(ss_tile, rect(20, 40, 20, 20), position);
		
	}
	else {
		  fb.blit(ss_tile, rect(0, 100, 20, 20), position);
	}
		 
}

void print_game(const struct game *game)
{
	int r, c;
	//move(0, 0);
	//printw("Score: %6d  Turns: %4d", game->score, game->turns);
	printf("jeu");
	for (r = 0; r < NROWS; r++) {
		for (c = 0; c < NCOLS; c++) {
			//move(r + 2, 5 * c);  // raf gestion position tuile
			
			print_tile(game->board[r][c],r,c);
		}
	}
	chgt = 0;
	
}


int combine_left(struct game *game, tile row[NCOLS])
{
	int c, did_combine = 0;
	for (c = 1; c < NCOLS; c++) {
		if (row[c] && row[c-1] == row[c]) {
			row[c-1]++;
			row[c] = 0;
			game->score += 1 << (row[c-1] - 1);
			did_combine = 1;
		}
	}
	return did_combine;
}

// deflate_left() returns nonzero if it did deflate, and 0 otherwise
int deflate_left(tile row[NCOLS])
{
	tile buf[NCOLS] = {0};
	tile *out = buf;
	int did_deflate = 0;
	int in;

	for (in = 0; in < NCOLS; in++) {
		if (row[in] != 0) {
			*out++ = row[in];
			did_deflate |= buf[in] != row[in];
		}
	}

	memcpy(row, buf, sizeof(buf));
	return did_deflate;
}

void rotate_clockwise(struct game *game)
{
	tile buf[NROWS][NCOLS];
	memcpy(buf, game->board, sizeof(game->board));

	int r, c;
	for (r = 0; r < NROWS; r++) {
		for (c = 0; c < NCOLS; c++) {
			game->board[r][c] = buf[NCOLS - c - 1][r];
		}
	}
}

void move_left(struct game *game)
{

	int r, ret = 0;
	for (r = 0; r < NROWS; r++) {
		tile *row = &game->board[r][0];
		ret |= deflate_left(row);
		ret |= combine_left(game, row);
		ret |= deflate_left(row);
	}

	game->turns += ret;
}

void move_right(struct game *game)
{
	rotate_clockwise(game);
	rotate_clockwise(game);
	move_left(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
}

void move_up(struct game *game)
{
	rotate_clockwise(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
	move_left(game);
	rotate_clockwise(game);
}

void move_down(struct game *game)
{
	rotate_clockwise(game);
	move_left(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
	rotate_clockwise(game);
}

// Pass by value because this function mutates the game
int lose_game(struct game test_game)
{
	int start_turns = test_game.turns;
	move_left(&test_game);
	move_up(&test_game);
	move_down(&test_game);
	move_right(&test_game);
	return test_game.turns == start_turns;
}


int max_tile(const tile *lboard)
{
	int i, ret = 0;
	for (i = 0; i < NROWS * NCOLS; i++) {
		ret = max(ret, lboard[i]);
	}
	return ret;
}

void init_curses()
{
	int bg = 0;
			
}






/* setup */
void init() {
	
	
	// load and set as the current spritesheet on our framebuffer
	//last_turn = games.turns;
	
	last_turn = games.turns;
	//init game
	place_tile(&games);
	place_tile(&games);	
	print_game(&games);
	chgt = 0;
	games = {0};
}


void render(uint32_t time) {

   

    
}

void update(uint32_t time) {

	 uint16_t changed = blit::buttons ^ last_buttons;
    uint16_t pressed = changed & blit::buttons;
    uint16_t released = changed & ~blit::buttons;


 fb.pen(rgba(255, 255, 255, 100));
     fb.rectangle(rect(1, 120 - 10, 12, 9));  
	 
	 // define position

	last_turn = games.turns;
	

	
   if(released & blit::button::DPAD_LEFT) {  
									chgt = 1;
									 move_up(&games); 
									 place_tile(&games);
									 print_game(&games);
									 if (lose_game(games)) {printf("loos");}
}
 
 if (released & blit::button::DPAD_RIGHT)  {
									chgt = 1;
									move_down(&games); 
									 place_tile(&games);	
									 print_game(&games);
									 if (lose_game(games)) {printf("loos");}
									 }
  if (released & blit::button::DPAD_UP )    { 
									chgt = 1;
									move_left(&games);  
									place_tile(&games);
									 print_game(&games);
									 if (lose_game(games)) {printf("loos");}
									}
  if (released & blit::button::DPAD_DOWN)  { 
									chgt = 1;
									move_right(&games);
									 place_tile(&games);	
									 print_game(&games);
									 if (lose_game(games)) {printf("loos");}
									}
	last_buttons = blit::buttons;
 
	
		
	 
}