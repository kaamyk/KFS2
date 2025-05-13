#include "kernel.h"

void	reset_cmd( void )
{
	memset(cmd_buf, 0, 256);
	cmd_buf_index = 0;
	cmd_complete = 0;
}

void shell_halt( void )
{
    asm volatile("cli; hlt");
}

void shell_reboot( void )
{
    unsigned char good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);
}

void shell_clear( void )
{
    terminal_initialize();
    update_cursor(0, 0);
}

void	stack( void )
{
	print_kernel_stack();
}

void shell_help() {
    write("Available commands:\n");
    write(" - halt: Stop CPU\n");
    write(" - reboot: Reboot PC\n");
    write(" - stack: Print kernel stack\n");
    write(" - clear: Clear screen\n");
    write(" - gdt: Print GDT kernel offsets\n");
    write(" - help: Show this message\n");
}

void	print_gdt()
{
	write("Kernel Code offset: 0x");
	uint16_t cs = 0;
	asm volatile("mov %%cs, %0" : "=r"(cs));
	write_btohex(cs >> 8);
	write_btohex(cs & 0xFF);
	terminal_putchar('\n');
	
	write("Kernel Data offset: 0x");
	uint16_t ds = 0;
	asm volatile("mov %%ds, %0" : "=r"(ds));
	write_btohex(ds >> 8);
	write_btohex(ds & 0xFF);
	terminal_putchar('\n');

	write("Kernel Stack offset: 0x");
	uint16_t ss = 0;
	asm volatile ("mov %%ss, %0" : "=r"(ss));
	write_btohex(ss >> 8);
	write_btohex(ss & 0xFF);
	terminal_putchar('\n');
}

void	cmd( void )
{
	if (cmd_buf_index == 0)
	{
		cmd_complete = 0;
		return;
	}
	switch(cmd_buf[0])
	{
		case 'h':
			if (strncmp(cmd_buf, "halt\0", 5) == 0)
				shell_halt();
			else if (strncmp(cmd_buf, "help\0", 5) == 0)
				shell_help();
			else
				write("Command not found\n");
			break ;
		case 'r':
			if (strncmp(cmd_buf, "reboot\0", 7) == 0)
				shell_reboot();
			else
				write("Command not found\n");
			break ;
		case 's':
			if (strncmp(cmd_buf, "stack\0", 6) == 0)
				stack();
			else
				write("Command not found\n");
			break ;
		case 'c':
			if (strncmp(cmd_buf, "clear\0", 6) == 0)
				shell_clear();
			else
				write("Command not found\n");
			break ;
		case 'g':
			if (strncmp(cmd_buf, "gdt\0", 4) == 0)
				print_gdt();
			else
				write("Command not found\n");
			break ;
		default:
			write("Command not found\n");
	}
	reset_cmd();
}
