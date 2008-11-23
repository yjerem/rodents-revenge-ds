#ifndef _menu_h
#define _menu_h

#include "canvas.h"
#include "button.h"

const u8 MAX_BUTTONS = 10;

class Menu {
	public:
		// Constructors
		Menu();
		Menu(Canvas *the_canvas, Font *the_font, Color background);
		~Menu();

		// Accessors
		void set_background_color(Color new_bgcolor) { background_color = new_bgcolor; }
		void set_button_widths(u16 new_button_widths) { button_widths = new_button_widths; }
		u16 get_button_widths() const { return button_widths; }
		void set_button_heights(u16 new_button_heights) { button_heights = new_button_heights; }
		u16 get_button_heights() const { return button_heights; }
		void set_button_colors(ButtonColors colors, ButtonColors pressed_colors) { button_colors = colors; button_pressed_colors = pressed_colors; }

		// Methods
		void init(Canvas *the_canvas, Font *the_font, Color background);
		Menu *select();
		void draw();
		void add_button(const char *button_text, Menu *submenu);
		void add_button(const char *button_text, void (*action)());

	private:
		Canvas *canvas;
		Font *font;
		Color background_color;
		u16 button_widths;
		u16 button_heights;
		ButtonColors button_colors;
		ButtonColors button_pressed_colors;

		Button *buttons[MAX_BUTTONS];
		Menu *submenus[MAX_BUTTONS];
		void (*actions[MAX_BUTTONS])();

		void add_button(const char *button_text, Menu *submenu, void (*action)());
		void arrange_buttons();
		u8 button_count() const;
};

#endif
