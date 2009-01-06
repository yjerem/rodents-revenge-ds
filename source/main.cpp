#include <nds.h>
#include <fat.h>
#include "canvas.h"
#include "font.h"
#include "menuset.h"
#include "game.h"
#include "options.h"
#include "radiomenu.h"
#include "level.h"
#include "levelselector.h"
#include "help.h"
#include "image.h"

void init_screens(void) {
	// Main screen turn on
	videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D);
	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	BACKGROUND.control[3] = BG_BMP16_256x256 | BG_BMP_BASE(0);
	BACKGROUND.bg3_rotation.xdy = 0;
	BACKGROUND.bg3_rotation.xdx = 1 << 8;
	BACKGROUND.bg3_rotation.ydx = 0;
	BACKGROUND.bg3_rotation.ydy = 1 << 8;
	
	// Sub screen
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	vramSetBankC(VRAM_C_SUB_BG_0x06200000);
	BACKGROUND_SUB.control[3] = BG_BMP16_256x256 | BG_BMP_BASE(0);
	BACKGROUND_SUB.bg3_rotation.xdy = 0;
	BACKGROUND_SUB.bg3_rotation.xdx = 1 << 8;
	BACKGROUND_SUB.bg3_rotation.ydx = 0;
	BACKGROUND_SUB.bg3_rotation.ydy = 1 << 8;
}

void blah(void *data) {
}

void play_game(void *fonts) {
	Game game(&screen_top, ((FontSet *)fonts)->large_font);
	game.begin();

	// Load title screen
	Image image_title(&screen_top, options.full_path("images/title.img"));
	image_title.draw(0, 0);
}

void change_game_speed(void *fonts) {
	RadioMenu speed_menu(&screen_bottom, ((FontSet *)fonts)->large_font, MENU_BACKGROUND_COLOR);

	speed_menu.set_button_widths(RADIOMENU_BUTTON_WIDTHS);
	speed_menu.set_button_heights(RADIOMENU_BUTTON_HEIGHTS);
	speed_menu.set_button_colors(BUTTON_COLORS, BUTTON_PRESSED_COLORS);

	speed_menu.add_back_button("Back");

	speed_menu.add_radio_button("Snail", SPEED_SNAIL);
	speed_menu.add_radio_button("Slow", SPEED_SLOW);
	speed_menu.add_radio_button("Medium", SPEED_MEDIUM);
	speed_menu.add_radio_button("Fast", SPEED_FAST);
	speed_menu.add_radio_button("Blazing", SPEED_BLAZING);

	speed_menu.set_selected_value(options.get_speed());

	options.set_speed((GameSpeed)speed_menu.select());
}

void change_starting_level(void *fonts) {
	LevelSelector level_selector(((FontSet *)fonts)->large_font, ((FontSet *)fonts)->small_font, MENU_BACKGROUND_COLOR);

	level_selector.set_selected_level(options.get_start_level());
	options.set_start_level(level_selector.select_level());

	// Load title screen
	Image image_title(&screen_top, options.full_path("images/title.img"));
	image_title.draw(0, 0);
}

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);

	keysSetRepeat(10, 2);

	init_screens();

	screen_top.clear(RGB(31, 31, 31));
	screen_bottom.clear(RGB(31, 31, 31));

	// Init FAT (Displays green screens if it fails)
	if (!fatInitDefault()) {
		screen_top.clear(RGB(0, 31, 0));
		screen_bottom.clear(RGB(0, 31, 0));
		while (1);
	}

	// Init fonts
	Font font_sans(options.full_path("fonts/sans.font"));
	Font font_metroid(options.full_path("fonts/keyboard.font"));

	FontSet fonts;
	fonts.large_font = &font_sans;
	fonts.small_font = &font_metroid;

	// Create menus
	MenuSet menu(&screen_bottom, &font_sans, MENU_BACKGROUND_COLOR);
	menu.set_button_widths(MENU_BUTTON_WIDTHS);
	menu.set_button_heights(MENU_BUTTON_HEIGHTS);
	menu.set_button_colors(BUTTON_COLORS, BUTTON_PRESSED_COLORS);

	MenuId MAIN_MENU = menu.add_menu();
	MenuId OPTIONS_MENU = menu.add_menu();
	MenuId HIGH_SCORES_MENU = menu.add_menu();
	MenuId RESET_HIGH_SCORES_MENU = menu.add_menu();

	menu.add_button(MAIN_MENU, "Play Game", &play_game, (void *)&fonts);
	menu.add_button(MAIN_MENU, "High Scores", HIGH_SCORES_MENU);
	menu.add_button(MAIN_MENU, "Options", OPTIONS_MENU);
	menu.add_button(MAIN_MENU, "Help", &help, (void *)&fonts);

	menu.add_button(OPTIONS_MENU, "Back", MAIN_MENU);
	menu.add_button(OPTIONS_MENU, "Change Starting Level", &change_starting_level, (void *)&fonts);
	menu.add_button(OPTIONS_MENU, "Change Game Speed", &change_game_speed, (void *)&fonts);

	menu.add_button(HIGH_SCORES_MENU, "Back", MAIN_MENU);
	menu.add_button(HIGH_SCORES_MENU, "View High Scores", &blah, NULL);
	menu.add_button(HIGH_SCORES_MENU, "Reset High Scores", RESET_HIGH_SCORES_MENU);

	menu.add_button(RESET_HIGH_SCORES_MENU, "Back", HIGH_SCORES_MENU);
	menu.add_button(RESET_HIGH_SCORES_MENU, "Confirm Reset", &blah, NULL);

	// Load title screen
	Image image_title(&screen_top, options.full_path("images/title.img"));
	image_title.draw(0, 0);

	menu.begin(MAIN_MENU);

	return 0;
}

