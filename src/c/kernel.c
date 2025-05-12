#include "kernel.h"

struct gdt_ptr		gp = {0};
struct gdt_entry	*gdt = (struct gdt_entry *)0x800;		// 0: Null descriptor; 1: code segment; 2: data segment

void kernel_main(void)
{
	terminal_initialize();

	write("/* **************************************************** */\n/*                                                      */\n/*                                  :::      ::::::::   */\n/*   KFS1                         :+:      :+:    :+:   */\n/*                              +:+ +:+         +:+     */\n/*   By: ");
	terminal_setcolor(vga_entry_color(VGA_COLOR_GREEN, VGA_COLOR_BLACK));
	write("eedy");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	write(" && ");
	terminal_setcolor(vga_entry_color(VGA_COLOR_BLUE, VGA_COLOR_BLACK));
	write("anvicent");
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
	write("     +#+  +:+       +#+        */\n/*                          +#+#+#+#+#+   +#+           */\n/*                               #+#    #+#             */\n/*                              ###   ########.fr       */\n/*                                                      */\n/* **************************************************** */\n");
	while(1){
		keyboard_handler();
		if (cmd_complete == 1)
		{
			cmd();
		}
	}
}
