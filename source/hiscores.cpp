#include <nds.h>
#include <stdio.h>
#include <string.h>
#include "hiscores.h"

HighscoreEntry high_scores[MAX_HIGH_SCORES];
u8 num_high_scores;

bool is_high_score(u32 score) {
	load_high_scores();

	if (num_high_scores != MAX_HIGH_SCORES) {
		return true;
	}

	for (u8 i = 0; i < num_high_scores; i++) {
		if (score > high_scores[i].score) {
			return true;
		}
	}

	return false;
}

bool add_high_score(u32 score, Font *font, Font *keyboard_font) {
	if (!is_high_score(score)) {
		return false;
	}

	Keyboard keyboard(options.full_path("keyboards/default.kb"), &screen_bottom, keyboard_font, 17, 100);
	Textbox textbox(&screen_top, font);

	textbox.set_x(5);
	textbox.set_y(60);
	textbox.set_dimensions(240, 22);
	textbox.set_value("");

	screen_top.clear(MENU_BACKGROUND_COLOR);
	screen_bottom.clear(MENU_BACKGROUND_COLOR);

	keyboard.draw();
	textbox.draw(true);

	font->print_string("You've got a top-10 high", 10, 10, &screen_top, RGB(0, 0, 0));
	font->print_string("score! Please enter your", 10, 25, &screen_top, RGB(0, 0, 0));
	font->print_string("name.", 10, 40, &screen_top, RGB(0, 0, 0));

	char name[256] = "";
	touchPosition stylus;
	char key;
	u8 name_index = 0;

	while (1) {
		scanKeys();
		stylus = touchReadXY();

		if ((key = keyboard.key_pressed(stylus))) {
			while (keysHeld() & KEY_TOUCH) { scanKeys(); swiWaitForVBlank(); }

			if (key == '\n') {
				break;
			} else if (key == '\b') {
				if (name_index) {
					name_index--;
					name[name_index] = '\0';
					textbox.set_value(name);
				}
			} else {
				name[name_index] = key;
				name_index++;
				textbox.set_value(name);
			}
		}

		textbox.draw(true);

		swiWaitForVBlank();
	}

	load_high_scores();

	for (u8 i = 0; i <= num_high_scores; i++) {
		if (i == num_high_scores || score > high_scores[i].score) {
			for (u8 j = num_high_scores; j > i; j--) {
				if (j < MAX_HIGH_SCORES) {
					strcpy(high_scores[j].name, high_scores[j - 1].name);
					high_scores[j].score = high_scores[j - 1].score;
				}
			}

			strcpy(high_scores[i].name, name);
			high_scores[i].score = score;

			num_high_scores++;

			break;
		}
	}

	save_high_scores();

	return true;
}

void display_high_scores(Font *font) {
	load_high_scores();

	screen_top.clear(MENU_BACKGROUND_COLOR);

	font->print_string_center("High Scores", 10, &screen_top, RGB(0, 0, 0));

	char str[256];
	for (u8 i = 0; i < MAX_HIGH_SCORES; i++) {
		Canvas *which_screen;

		if (i < 5) {
			which_screen = &screen_top;
		} else {
			which_screen = &screen_bottom;
		}

		strcpy(str, strval(i + 1));
		strcat(str, ". ");

		if (i < num_high_scores) {
			strcat(str, high_scores[i].name);
		}

		font->print_string(str, 10, 50 + (i % 5) * 20, which_screen, RGB(31, 15, 0));

		if (i < num_high_scores) {
			font->print_number(high_scores[i].score, 200, 50 + (i % 5) * 20, which_screen, RGB(31, 15, 0));
		}
	}
}

void reset_high_scores() {
	num_high_scores = 0;
	save_high_scores();
}

void load_high_scores() {
	FILE *hiscores_file = fopen(options.full_path("hiscores"), "r");

	if (!hiscores_file) {
		return;
	}

	num_high_scores = 0;
	char line[256];
	while (!feof(hiscores_file)) {
		// Get name
		fgets(line, 256, hiscores_file);
		chop(line);
		strcpy(high_scores[num_high_scores].name, line);

		// Get score
		fgets(line, 256, hiscores_file);
		chop(line);

		s32 the_score;
		intval(line, &the_score);
		
		high_scores[num_high_scores].score = the_score;

		if (!feof(hiscores_file)) {
			num_high_scores++;
		}
	}

	fclose(hiscores_file);
}

void save_high_scores() {
	FILE *hiscores_file = fopen(options.full_path("hiscores"), "w");

	for (u8 i = 0; i < num_high_scores; i++) {
		// Write name
		fputs(high_scores[i].name, hiscores_file);
		fputc('\n', hiscores_file);

		// Write score
		fputs(strval(high_scores[i].score), hiscores_file);
		fputc('\n', hiscores_file);
	}

	fclose(hiscores_file);
}
