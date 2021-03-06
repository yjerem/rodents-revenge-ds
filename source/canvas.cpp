#include <nds.h>
#include <string.h>
#include "canvas.h"

Canvas screen_top(SCREEN_WIDTH, SCREEN_HEIGHT, (Color *)BG_BMP_RAM(0));
Canvas screen_bottom(SCREEN_WIDTH, SCREEN_HEIGHT, (Color *)BG_BMP_RAM_SUB(0));

Canvas::Canvas(u32 the_width, u32 the_height) {
	width = the_width;
	height = the_height;
	data = new Color [width * height];
	memory_allocated = true;
}

Canvas::Canvas(u32 the_width, u32 the_height, Color *the_data) {
	width = the_width;
	height = the_height;
	data = the_data;
	memory_allocated = false;
}

Canvas::~Canvas() {
	if (memory_allocated) {
		delete [] data;
		data = 0;
	}
}

void Canvas::rect(u32 upper_left_x, u32 upper_left_y, u32 lower_right_x, u32 lower_right_y, Color color, RectType type) {
	// Don't try to print the box if it's completely out of range of the canvas.
	if (upper_left_x >= width || upper_left_y >= height) {
		return;
	}

	// Make box smaller if the lower-right corner is out of range of the canvas.
	if (lower_right_x >= width) {
		lower_right_x = width - 1;
	}
	if (lower_right_y >= height) {
		lower_right_y = height - 1;
	}

	switch (type) {
		case RECT_FILLED:
			// Draw a filled-in rectangle.
			for (u32 put_x = upper_left_x; put_x <= lower_right_x; put_x++) {
				for (u32 put_y = upper_left_y; put_y <= lower_right_y; put_y++) {
					plot(put_x, put_y, color);
				}
			}
			break;

		case RECT_OUTLINE:
			// Only draw an outline of the rectangle.
			for (u32 put_x = upper_left_x; put_x <= lower_right_x; put_x++) {
				plot(put_x, upper_left_y, color);
				plot(put_x, lower_right_y, color);
			}

			for (u32 put_y = upper_left_y; put_y <= lower_right_y; put_y++) {
				plot(upper_left_x, put_y, color);
				plot(lower_right_x, put_y, color);
			}
			break;
	}
}

void Canvas::copy(Canvas *destination, u32 x, u32 y) {
	if (x >= destination->get_width() || y >= destination->get_height()) {
		return;
	}

	// Don't write into other areas of memory if this canvas won't fit on the
	// destination canvas.
	u32 right_limit = x + width;
	u32 bottom_limit = y + height;
	if (x + width >= destination->get_width()) {
		right_limit = destination->get_width();
	}
	if (y + height >= destination->get_height()) {
		bottom_limit = destination->get_height();
	}

	Color *dest_data = destination->get_data();
	u16 dest_width = destination->get_width();

	u32 get_x;
	u32 put_x;
	u32 get_y;
	u32 put_y;

	get_x = 0;
	for (put_x = x; put_x < right_limit; put_x++) {
		get_y = 0;
		for (put_y = y; put_y < bottom_limit; put_y++) {
			dest_data[put_x + put_y * dest_width] = data[get_x + get_y * width];
			get_y++;
		}
		get_x++;
	}
}

void Canvas::copy(Canvas *destination) {
	DC_FlushAll();
	dmaCopy(data, destination->get_data(), width * height * sizeof(data[0]));
}

void Canvas::clear(Color color) {
	for (u32 i = 0; i < width * height; i++) {
		data[i] = color;
	}
}

void Canvas::line(u32 x1, u32 y1, u32 x2, u32 y2, Color color) {	
	s32 dx = x1 - x2;
	s32 dy = y1 - y2;
	s32 dxabs = dx;
	s32 sdx = 1;
	
	if (dx < 0) {
		dxabs = -dx;
		sdx = -1;
	}

	s32 dyabs = dy;
	s32 sdy = 1;
	
	if (dy < 0) {
		dyabs = -dy;
		sdy = -1;
	}

	s32 x = dyabs / 2;
	s32 y = dxabs / 2;
	s32 px = x2;
	s32 py = y2;

	plot(px, py, color);

	if (dxabs >= dyabs) {
		for (s32 i = 0; i < dxabs; i++) {
			y += dyabs;
			
			if (y >= dxabs) {
				y -= dxabs;
				py += sdy;
			}
			
			px += sdx;
			plot(px, py, color);
		}
	} else {
		for (s32 i = 0; i < dyabs; i++) {
			x += dxabs;
			
			if (x >= dyabs) {
				x -= dyabs;
				px += sdx;
			}

			py += sdy;
			plot(px, py, color);
		}
	}
}

void Canvas::darken() const {
	u16 i;
	Color pixel;
	
	for (i = 0; i < width * height; i++) {
		pixel = data[i];

		data[i] = ((pixel & 0x1F) >> 1) |
		          ((((pixel >> 5) & 0x1F) >> 1) << 5) |
		          ((((pixel >> 10) & 0x1F) >> 1) << 10) |
		          (1 << 15);
	}
}

