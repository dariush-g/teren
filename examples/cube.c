
#include "../teren.h"
#include <math.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct {
	float x, y, z;
} Vec3;

Vec3 cube[8] = {{-1, -1, -1}, {1, -1, -1}, {1, 1, -1}, {-1, 1, -1},
		{-1, -1, 1},  {1, -1, 1},  {1, 1, 1},  {-1, 1, 1}};

int edges[12][2] = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {4, 5}, {5, 6},
		    {6, 7}, {7, 4}, {0, 4}, {1, 5}, {2, 6}, {3, 7}};

Vec3 rotate_y(Vec3 v, float angle) {
	float c = cosf(angle), s = sinf(angle);
	return (Vec3){v.x * c + v.z * s, v.y, -v.x * s + v.z * c};
}

Vec3 rotate_x(Vec3 v, float angle) {
	float c = cosf(angle), s = sinf(angle);
	return (Vec3){v.x, v.y * c - v.z * s, v.y * s + v.z * c};
}

int screen_x(float x, float z, int width) {
	float aspect = 0.6f; // horizontal squish
	return (int)(width / 2.f + x * aspect * (width / 4.f) / (z + 3));
}

int screen_y(float y, float z, int height) {
	return (int)(height / 2.f - y * (height / 4.f) / (z + 3));
}

void draw_line(int x0, int y0, int x1, int y1, char c) {
	int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy;
	while (1) {
		if (x0 >= 0 && x0 < get_buffer_width() && y0 >= 0 &&
		    y0 < get_buffer_height())
			teren_set_char(x0, y0, c);
		if (x0 == x1 && y0 == y1)
			break;
		int e2 = 2 * err;
		if (e2 >= dy) {
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx) {
			err += dx;
			y0 += sy;
		}
	}
}

int main() {
	teren_term_init();

	float angle_step = 0.05f;

	while (1) {
		// Fill background
		teren_fill_buffer_char('.');

		// Rotate cube based on key press
		char key = teren_poll_key();
		if (key > 0) {
			for (int i = 0; i < 8; i++) {
				if (key == 'w')
					cube[i] = rotate_x(cube[i], angle_step);
				if (key == 's')
					cube[i] =
					    rotate_x(cube[i], -angle_step);
				if (key == 'a')
					cube[i] = rotate_y(cube[i], angle_step);
				if (key == 'd')
					cube[i] =
					    rotate_y(cube[i], -angle_step);
			}
		}

		if (key == 27) { // Escape
			char seq[2];
			if (read(STDIN_FILENO, &seq, 2) == 2) {
				if (seq[0] == '[') {
					for (int i = 0; i < 8; i++) {
						switch (seq[1]) {
						case 'A':
							cube[i] = rotate_x(
							    cube[i],
							    angle_step);
							break;
						case 'B':
							cube[i] = rotate_x(
							    cube[i],
							    -angle_step);
							break;
						case 'C':
							cube[i] = rotate_y(
							    cube[i],
							    -angle_step);
							break;
						case 'D':
							cube[i] = rotate_y(
							    cube[i],
							    angle_step);
							break;
						}
					}
				}
			}
		}

		// Draw edges
		char c = 'a';
		for (int i = 0; i < 12; i++) {
			int a = edges[i][0], b = edges[i][1];
			int x0 =
			    screen_x(cube[a].x, cube[a].z, get_buffer_width());
			int y0 =
			    screen_y(cube[a].y, cube[a].z, get_buffer_height());
			int x1 =
			    screen_x(cube[b].x, cube[b].z, get_buffer_width());
			int y1 =
			    screen_y(cube[b].y, cube[b].z, get_buffer_height());
			draw_line(x0, y0, x1, y1, c++);
			if (c > 'z')
				c = 'a';
		}

		teren_present();
		usleep(30000); // ~33 FPS
	}

	teren_term_cleanup();
	return 0;
}
