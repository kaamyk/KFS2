#ifndef KFS
#define KFS

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 
#define VGA_SIZE 	80 * 25

enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

#define GDT_ADDR	0x800
#define GDT_KCODE	0x8
#define GDT_KDATA	0x10
#define GDT_KSTACK	0x18
#define GDT_UCODE	0x20
#define GDT_UDATA	0x28
#define GDT_USTACK	0x30

struct gdt_entry
{
	uint16_t	limit_low;
	uint16_t	base_low;
	uint8_t		base_middle;
	uint8_t		access;
	uint8_t		limit_high;
	uint8_t		flags;
	uint8_t		base_high;
}	__attribute__((packed));	// Tells the compiler not to add padding between fields

struct	gdt_ptr
{
	uint16_t	limit;
	uint32_t	base;
}	__attribute__((packed));

extern char 	cmd_buf[256];
extern uint8_t	cmd_complete;

// gdt.c
extern struct gdt_entry	*gdt;		// 0: Null descriptor; 1: code segment; 2: data segment
extern struct gdt_ptr	gp;

extern void 	_setGdt( void );
void	gdt_install( struct gdt_ptr *gp, struct gdt_entry *gdt );

#define HEX_BASE "0123456789ABCDEF"

// utils.c
int	strncmp(const char *s1, const char *s2, size_t n);
int	atoi(const char *str);
void	write_itohex(const uint32_t b);
void	write_btohex(const uint8_t b);
char *strcpy(char *dest, char *src);
size_t strlen(const char* str);
void *memcpy(void *dst, const void *src, size_t n);
void *memset(void *b, int c, size_t len);

extern char cmd_buf[256];
extern uint8_t cmd_buf_index;
extern uint8_t cmd_complete;
void cmd();

// cursor.c
void update_cursor(int x, int y);
void outb(uint16_t port, uint8_t value);

// output.c
void terminal_putchar(char c);
void delete_char();
void terminal_initialize(void);
void terminal_setcolor(uint8_t color);
void write(const char* data);
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);
void switch_screen(uint8_t screen);

//input.c
void keyboard_handler();

#endif