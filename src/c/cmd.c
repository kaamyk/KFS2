#include "kernel.h"

void	reset_cmd( void )
{
	memset(cmd_buf, 0, cmd_buf_index);
	cmd_buf_index = 0;
	cmd_complete = 0;
}

uint32_t	get_gdt_limit( const struct gdt_entry *entry )
{
	return ( (entry->limit_low | (entry->limit_high << 16)) & 0x0FFFFFFF );
}

uintptr_t get_gdt_base( const struct gdt_entry *entry )
{
	return ( entry->base_low | (entry->base_middle << 16) | (entry->base_high << 24) );
}

void	printk( void )
{
	const uint32_t		limit = get_gdt_limit(gdt + GDT_KSTACK);
	uint32_t	*k_stack = (void *) get_gdt_base(gdt + GDT_KSTACK);

	write_itohex((uint32_t)gdt + GDT_KSTACK);
	terminal_putchar('\n');
	write_itohex(limit);
	terminal_putchar('\n');
	
	for (uint32_t i = 0; i < limit; i += 4)
		write_itohex(*(k_stack + i));
	
	return ;
}

void	cmd( void )
{
	write(cmd_buf);
	if (cmd_buf_index == 0)
		return;
	switch(cmd_buf[0])
	{
		case 'p':
			write("Command complete\n");
			if (strncmp(cmd_buf, "printk", 5) == 0)
				printk();
			break ;
		default:
			write("Command not found\n");
	}
	reset_cmd();
}
