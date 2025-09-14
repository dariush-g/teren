#include "teren.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

void teren_enable_raw_mode(void) {
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void teren_disable_raw_mode(void) {
	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void teren_set_non_blocking(void) {
	int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

void teren_term_init(void) {
	teren_enable_raw_mode();
	teren_set_non_blocking();

	terminal_width = get_buffer_width();
	terminal_height = get_buffer_height();

	// allocate back buffer
	back_buf = malloc(terminal_height * sizeof(char *));
	// allocate front buffer
	front_buf = malloc(terminal_height * sizeof(char *));

	for (int i = 0; i < terminal_height; i++) {
		back_buf[i] = malloc(terminal_width * sizeof(char));
		front_buf[i] = malloc(terminal_width * sizeof(char));
	}

	for (int y = 0; y < terminal_height; y++) {
		for (int x = 0; x < terminal_width; x++) {
			front_buf[y][x] = ' ';
		}
	}
}

void teren_term_cleanup(void) {
	for (int i = 0; i < terminal_height; i++) {
		free(back_buf[i]);
		free(front_buf[i]);
	}
	free(back_buf);
	free(front_buf);

	teren_disable_raw_mode();

	// Reset terminal colors and cursor
	printf("\x1b[0m");   // reset colors
	printf("\x1b[?25h"); // show cursor again (if you hide it later)
	fflush(stdout);
}

// void teren_term_cleanup(void) { teren_disable_raw_mode(); }

void teren_set_char(int x, int y, char c) { back_buf[y][x] = c; }

void teren_clear(void) {
	for (int y = 0; y < terminal_height; y++)
		for (int x = 0; x < terminal_width; x++)
			back_buf[y][x] = ' ';
}

void teren_fill_buffer_char(char c) {
	for (int i = 0; i < terminal_height; i++) {
		for (int j = 0; j < terminal_width; j++) {
			back_buf[i][j] = c;
		}
	}
}

void teren_present(void) {
	int cur_fg = -1, cur_bg = -1; // cached last printed ANSI code values
	for (int y = 0; y < terminal_height; y++) {
		for (int x = 0; x < terminal_width; x++) {
			char back = back_buf[y][x];
			char front = front_buf[y][x];

			if (back != front) {
				// Move cursor
				printf("\x1b[%d;%dH", y + 1, x + 1);

				putchar(back);

				// update front buffer cell
				front_buf[y][x] = back;
			}
		}
	}
	// Optionally reset colors at end of frame:
	printf("\x1b[0m");
	fflush(stdout);
}

int teren_poll_key(void) {
	char c;
	int n = read(STDIN_FILENO, &c, 1);
	if (n == 1)
		return c;
	return -1;
}

int teren_read_key(void) {
	char c;
	while (read(STDIN_FILENO, &c, 1) != 1) {
		;
	}
	return c;
}
