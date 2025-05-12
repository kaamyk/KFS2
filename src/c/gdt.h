#ifndef GDT_H
#define GDT_H

#include <stdint.h>

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

struct gdt_entry	gdt[3];		// 0: Null descriptor; 1: code segment; 2: data segment
struct gdt_ptr		gp;

extern void 	_gdt_refresh;



#endif
