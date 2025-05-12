#include "kernel.h"

void	gdt_set_entry( int entry_index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran )
{
	gdt[entry_index].base_low = base & 0xFFFF;
	gdt[entry_index].base_middle = ((base >> 16) & 0xFF);
	gdt[entry_index].base_high = ((base >> 24) & 0xFF);

	gdt[entry_index].limit_low = limit & 0xFFFF;
	// gdt[entry_index].limit_high = ((limit >> 16) & 0xFF);
	gdt[entry_index].limit_high_flags = ((limit >> 16) & 0x0F);
	
	gdt[entry_index].access = access;

	gdt[entry_index].limit_high_flags |= gran & 0xF0;	
	// In GDT flag field is only 4 bits large.
	// With a binary operator and a flag we only keep the 4 high bits of flag without modifying the other fields.
}

void	gdt_install( void )
{
	gp.limit = ((sizeof(struct gdt_entry) * 7) - 1);
	gp.base = (uint32_t)gdt;

	// Set the Null descriptor
	gdt_set_entry(0, 0, 0, 0, 0);

	// Access byte
	// 0x9A = 0b10011010 -> Kernel code
	// 0x92 = 0b10010010 -> Kernel data / Kernel Stack
	// Granularity
	// 0xCF = 0b11001111
	gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
	gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_entry(3, 0, 0xFFFFFFFF, 0x92, 0xCF);
	gdt_set_entry(4, 0, 0xFFFFFFFF, 0xFA, 0xCF);
	gdt_set_entry(5, 0, 0xFFFFFFFF, 0xF2, 0xCF);
	gdt_set_entry(6, 0, 0xFFFFFFFF, 0xF2, 0xCF);

	// _gdt_refresh();
}

