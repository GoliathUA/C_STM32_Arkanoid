#include <game/ball.h>

void game_init() {

	// Scene
	scene.background_color = BLACK;
	scene.width = TFTWIDTH;
	scene.height = TFTHEIGHT;
	scene.status = GAME_STATUS_PLAYING;
	scene.blocks_columns = 3;
	scene.blocks_rows = 3;

	// Blocks
	scene.blocks_size = scene.blocks_columns * scene.blocks_rows;

	blocks = (Block*) calloc(scene.blocks_size, sizeof(Block));

	uint16_t current_row_width = 0;
	uint16_t current_column_index = 0;
	uint16_t current_y = 15;
	uint16_t bound_size = 10;
	for (int i = 0; i < scene.blocks_size; i++) {
		blocks[i].is_alive = 1;
		blocks[i].color = LCD_Color565(255, 255, (i + 25) * (current_column_index + 25) );
		blocks[i].height = 15;
		blocks[i].width = (scene.width / scene.blocks_columns) - bound_size;
		blocks[i].x = current_column_index * (blocks[i].width + bound_size);
		blocks[i].y = current_y;
		blocks[i].score = 10;

		current_column_index++;
		current_row_width += blocks[i].width + bound_size;
		if (current_row_width >= scene.width) {
			current_column_index = 0;
			current_row_width = 0;
			current_y += blocks[i].height + 5;
		}
	}

	//malloc(sizeof(int) * vector->capacity)

	// Ball
	ball.x = scene.width / 2;
	ball.y = scene.height / 2;
	ball.radius = 5;
	ball.color = WHITE;

	ball.velocity_x = 1;
	ball.velocity_y = 1;

	ball.direction_x = -1;
	ball.direction_y = -1;

	ball.previous_x = ball.x;
	ball.previous_y = ball.y;

	// Player
	player.width = 100;
	player.height = 10;
	player.x = (scene.width / 2) - (player.width / 2);
	player.y = scene.height - player.height;
	player.previous_x = player.x;
	player.previous_y = player.y;
	player.color = RED;
	player.score = 0;
}

void game_draw_ball() {

	if (scene.is_clean_ball) {
		scene.is_clean_ball = 0;
		LCD_FillCircle(ball.previous_x, ball.previous_y, ball.radius + 2,
				scene.background_color);
	}

	LCD_FillCircle(ball.x, ball.y, ball.radius, ball.color);
}

void game_draw_blocks() {
	for (int i = 0; i < scene.blocks_size; i++) {

		if (blocks[i].is_destroying) {
			blocks[i].is_destroying = 0;
			LCD_FillRect(blocks[i].x, blocks[i].y, blocks[i].width,
					blocks[i].height, scene.background_color);
		}

		if (!blocks[i].is_alive) {
			continue;
		}
		LCD_FillRect(blocks[i].x, blocks[i].y, blocks[i].width,
				blocks[i].height, blocks[i].color);
	}
}

void game_draw_player() {

	if (scene.is_clean_player) {
		scene.is_clean_player = 0;
		LCD_FillRect(0, player.previous_y,
				scene.width, player.height, scene.background_color);

	}

	LCD_FillRect(player.x, player.y, player.width, player.height, player.color);

}

void game_physic_move_player_left() {
	scene.is_clean_player = 1;

	player.previous_x = player.x;
	player.previous_y = player.y;

	player.x -= 10;

	if (player.x <= 0) {
		player.x = 0;
	}

}

void game_physic_move_player_right() {
	scene.is_clean_player = 1;

	player.previous_x = player.x;
	player.previous_y = player.y;

	player.x += 10;

	if ((player.x + player.width) >= scene.width) {
		player.x = scene.width - player.width;
	}

}

void game_physic_colission_detection() {
	int16_t x = ball.x;
	int16_t y = ball.y;
	int16_t bound_x = ball.radius / 2;
	int16_t bound_y = ball.radius / 2;

	int16_t target_x = player.x + (player.width / 2);
	int16_t target_y = player.y;
	int16_t target_bound_x = player.width / 2;
	int16_t target_bound_y = player.height / 2;

	if (game_physic_has_collision(x, y, bound_x, bound_y, target_x, target_y,
			target_bound_x, target_bound_y)) {
		ball.direction_y *= -1;
	} else {
		for (int i = 0; i < scene.blocks_size; i++) {
			if (!blocks[i].is_alive) {
				continue;
			}

			target_bound_x = blocks[i].width / 2;
			target_bound_y = blocks[i].height / 2;
			target_x = blocks[i].x + target_bound_x;
			target_y = blocks[i].y + target_bound_y;
			if (game_physic_has_collision(x, y, bound_x, bound_y, target_x,
					target_y, target_bound_x, target_bound_y)) {
				blocks[i].is_destroying = 1;
				blocks[i].is_alive = 0;
				ball.direction_y *= -1;
				player.score += blocks[i].score;
			}
		}
	}

}

uint8_t game_physic_has_collision(int16_t x, int16_t y, int16_t bound_x,
		int16_t bound_y, int16_t target_x, int16_t target_y,
		int16_t target_bound_x, int16_t target_bound_y) {

	return (x + bound_x > target_x - target_bound_x)
			&& (x - bound_x < target_x + target_bound_x)
			&& (y + bound_y > target_y - target_bound_y)
			&& (y - bound_y < target_y + target_bound_y);
}

void game_physic_move_ball() {

	scene.is_clean_ball = 1;

	ball.previous_x = ball.x;
	ball.previous_y = ball.y;

	if (ball.x >= scene.width || ball.x <= 0) {
		ball.direction_x *= -1;
	} else if (ball.y <= 0) {
		ball.direction_y *= -1;
	} else if (ball.y >= scene.height) {
		scene.status = GAME_STATUS_LOSE;
		//ball.direction_y *= -1;
	}

	ball.x += ball.velocity_x * ball.direction_x;
	ball.y += ball.velocity_y * ball.direction_y;

}

void game_world_update() {

	if (scene.status != GAME_STATUS_PLAYING) {
		return;
	}

	game_physic_move_ball();
	game_physic_colission_detection();

	uint8_t has_alive_blocks = 0;
	for (int i = 0; i < scene.blocks_size; i++) {
		if (blocks[i].is_alive) {
			has_alive_blocks = 1;
			break;
		}
	}

	if (!has_alive_blocks) {
		scene.status = GAME_STATUS_WIN;
	}
}
