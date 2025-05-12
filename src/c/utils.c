#include "kernel.h"

/* Basic function */

int	strncmp(const char *s1, const char *s2, size_t n)
{
	size_t			i;	
	unsigned char	*str1;
	unsigned char	*str2;

	str1 = (unsigned char *)s1;
	str2 = (unsigned char *)s2;
	if (!str1 || !str2)
		return (0);
	i = 0;
	while (n)
	{
		if (str1[i] != str2[i])
			return (str1[i] - str2[i]);
		i ++;
		n --;
	}
	return (0);
}

int	atoi(const char *str)
{
	int	i;
	int	neg;
	int	nbr;

	i = 0;
	nbr = 0;
	neg = 1;
	while (str[i] == ' ' || (str[i] >= 9 && str[i] <= 13))
		i ++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i] == '-')
			neg *= -1;
		i ++;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		nbr = nbr * 10 + (str[i] - '0');
		i ++;
	}
	nbr *= neg;
	return (nbr);
}

void	write_itohex(const uint32_t n)
{
	terminal_putchar(HEX_BASE[((n >> 28) & 0x0F)]);
	terminal_putchar(HEX_BASE[((n >> 24) & 0x0F)]);
	terminal_putchar(' ');
	terminal_putchar(HEX_BASE[((n >> 20) & 0x0F)]);
	terminal_putchar(HEX_BASE[((n >> 16) & 0x0F)]);
	terminal_putchar(' ');
	terminal_putchar(HEX_BASE[((n >> 12) & 0x0F)]);
	terminal_putchar(HEX_BASE[((n >> 8) & 0x0F)]);
	terminal_putchar(' ');
	terminal_putchar(HEX_BASE[((n >> 4) & 0x0F)]);
	terminal_putchar(HEX_BASE[(n & 0x0F)]);
}

void	write_btohex(const uint8_t b)
{
	terminal_putchar(HEX_BASE[(b >> 4) & 0x0F]);
	terminal_putchar(HEX_BASE[b & 0x0F]);
}

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

char *strcpy(char *dest, char *src)
{
	int i;

	i = 0;
	while (src[i] != '\0')
	{
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}

void *memset(void *b, int c, size_t len)
{
	char *str;

	str = b;
	while (len)
	{
		*str = (unsigned char)c;
		str++;
		len--;
	}
	return (b);
}

void *memcpy(void *dst, const void *src, size_t n)
{
	size_t	i;
	char	*s1;

	s1 = (char *)dst;
	i = 0;
	while (i < n)
	{
		*(char *)s1 = *(char *)src;
		s1++;
		src++;
		i++;
	}
	return (dst);
}

/* End of basic functions */