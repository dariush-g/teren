#ifndef TEREN_H
#define TEREN_H

#include <asm-generic/ioctls.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

static uint_fast8_t terminal_width;
static uint_fast8_t terminal_height;

static inline int get_buffer_width() {
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	return w.ws_col;
}

static inline int get_buffer_height() {
	struct winsize w;
	ioctl(0, TIOCGWINSZ, &w);
	return w.ws_row;
}

static char **front_buf;
static char **back_buf;

void teren_swap_buffers(void);

void teren_enable_raw_mode(void);

void teren_disable_raw_mode(void);

void teren_set_non_blocking(void);

// enables terminal modes
void teren_term_init(void);

// resets terminal modes
void teren_term_cleanup(void);

// prints the buffer to the screen and swaps front and back buffers
void teren_present(void);

// sets buffer[y][x] to 'c'
void teren_set_char(int x, int y, char c);

// fills buffer with char 'c'
void teren_fill_buffer_char(char c);

// clears buffer
void teren_clear(void);

// non-blocking key check
int teren_poll_key(void);

// blocking key check
int teren_read_key(void);

#endif
