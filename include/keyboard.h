#ifndef _keyboard_h
#define _keyboard_h

#include "color.h"
#include "canvas.h"
#include "font.h"

enum KeyboardStatus { KEYBOARD_OK, KEYBOARD_FILE_NOT_FOUND };

struct KeyboardColors {
	Color line;
	Color key_background;
	Color text;
	Color background;
	Color highlight_key_background;
	Color highlight_text;
};

// This control character represents a new row of keys on the keyboard.
const char KEYBOARD_NEW_ROW = 5;

// This gets passed to the print_key() function.
const bool KEY_HIGHLIGHTED = true;

class Keyboard {
	public:
		// Constructors
		Keyboard(const char *filename, Canvas *the_canvas, Font *the_font, u8 the_key_width, u8 space_bar_width);
		~Keyboard();

		// Returns key that is currently pressed, and highlights/unhighlights it
		char key_pressed(bool shift, u16 stylus_x, u16 stylus_y);

		// Load keyboard layout from file
		KeyboardStatus load_keyboard(const char *filename);

		// Screen output
		void print_key(char key, u16 x, u16 y, bool highlighted=false) const;
		void draw(bool shift) const;

		// Accessors
		void set_colors(KeyboardColors new_colors) { colors = new_colors; }
		u8 get_status() const { return status; }
		u8 get_x() const { return keyboard_x; }
		u8 get_y() const { return keyboard_y; }
		void set_xy(u8 x, u8 y) { keyboard_x = x; keyboard_y = y; }
		void set_canvas(Canvas *new_canvas) { canvas = new_canvas; }

	private:
		Canvas *canvas;
		Font *font;
		char keyboard[2][255];
		u8 key_width;
		KeyboardStatus status;
		u8 keyboard_x;
		u8 keyboard_y;
		u8 width_of_space_bar;
		char highlighted_key;
		KeyboardColors colors;
};

#endif
