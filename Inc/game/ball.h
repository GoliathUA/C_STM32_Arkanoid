#include "stm32f4xx_hal.h"
#include "lcd.h"

#define GAME_STATUS_PLAYING 1
#define GAME_STATUS_WIN 2
#define GAME_STATUS_LOSE 3

typedef struct {
	int16_t x;
	int16_t y;
	int16_t radius;
	uint16_t color;

	int16_t velocity_x;
	int16_t velocity_y;

	int16_t direction_x;
	int16_t direction_y;

	int16_t previous_x;
	int16_t previous_y;
} Ball;

typedef struct {
	int16_t x;
	int16_t y;
	uint16_t color;

	int16_t velocity_x;
	int16_t velocity_y;

	int16_t previous_x;
	int16_t previous_y;

	uint16_t width;
	uint16_t height;

	int16_t score;
} Player;

typedef struct {
	int16_t x;
	int16_t y;
	uint16_t color;

	uint16_t width;
	uint16_t height;

	uint8_t is_destroying;
	uint8_t is_alive;
	uint16_t score;
	uint8_t index;
	uint8_t quad_tree_index;
} Block;

typedef struct {

	uint16_t width;
	uint16_t height;

	uint16_t background_color;

	uint8_t blocks_columns;
	uint8_t blocks_rows;
	uint8_t blocks_size;

	uint8_t status;
	uint8_t is_clean_ball;
	uint8_t is_clean_player;

} SceneState;

volatile SceneState scene;

volatile Ball ball;

volatile Player player;

volatile Block* blocks;


void game_init();

///////////////////
/// DRAW
///////////////////

void game_draw_ball();
void game_draw_player();
void game_draw_blocks();

///////////////////
/// Physics
///////////////////

void game_world_update();
void game_physic_colission_detection();
uint8_t game_physic_has_collision(int16_t x, int16_t y, int16_t bound_x,
		int16_t bound_y, int16_t target_x, int16_t target_y,
		int16_t target_bound_x, int16_t target_bound_y);
void game_physic_move_ball();
void game_physic_move_player_left();
void game_physic_move_player_right();

