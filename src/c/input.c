#include "kernel.h"

uint8_t	shift = 0;
char	cmd_buf[256] = {0};
uint8_t	cmd_buf_index = 0;
uint8_t cmd_complete = 0;

char scancode_to_ascii[128] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t', 'q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
    'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    0,0,0,0,0,0,0,0,0,0,0, 0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};


char scancode_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,
    0, 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\'', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0
};

unsigned char inb(unsigned short port)
{
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

uint8_t read_byte()
{
    while(!(inb(0x64) & 1)){};
	return inb(0x60);
}

void keyboard_handler()
{
    uint8_t scancode = read_byte();

    if (cmd_buf_index < 254 &&  scancode <= 0x3A)
    {
        if ((scancode == 0x2A) || scancode == 0x36 )
            shift = 1;
        else if ( scancode == 0x0E)
        {
        	if (cmd_buf_index == 0)
         		return ;
         
         	delete_char();
	        --cmd_buf_index;
			cmd_buf[cmd_buf_index] = 0;
        }
        else if (shift == 1)
		{
            terminal_putchar(scancode_shift[scancode]);
			cmd_buf[cmd_buf_index] = scancode_shift[scancode];
			cmd_buf_index ++;
		}
        else
		{
            terminal_putchar(scancode_to_ascii[scancode]);
			if (scancode_to_ascii[scancode] == '\n')
			{
				cmd_buf[cmd_buf_index] = 0;
				cmd_complete = 1;
			}
			else
				cmd_buf[cmd_buf_index++] = scancode_to_ascii[scancode];
		}
    }
    // F1-F10
    else if (scancode >= 0x3B && scancode <=0x44)
    {
        if (scancode >= 0x3B && scancode <= 0x3F)
            switch_screen(scancode - 0x3B);
    }
    // SHIFT
    else if(scancode <= 0x39 + 0x80)
    {
		if ((scancode - 0x80 == 0x2A) || (scancode - 0x80 == 0x36))
			shift = 0;
    }
}
