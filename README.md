# KFS2
2nd step of building our kernel from Scratch.

## Summary
- [How it works](#how-it-works-)
- [Objectives](#objectives)
- [What is a GDT ?](#what-is-a-gdt)
	- [General](#general)
	- [GDT Composition](#gdt-composition)
		- [Flags](#flags)
		- [Access bytes](#access-byte)
- [In our case](#in-our-case-how-to-use-it-)
	- [Update the GDTR](#update-the-gdtr)
	- [Create the GDT and its entries](#create-the-gdt-and-its-entries)
- [Bonus](#bonus)
- [Sources](#sources)

## How it works ?
- Run the ```make run``` command to compile the different files and launch the *Qemu* virtual machine.
- In the machine, see [bonus](#bonus) to see the differents commands in the terminal.
- You can change screens using keys F1 -> F10.

## Objectives
> Create, fill and link a Global Descriptor Table (GDT)

- Understand how memory works in a system
- Understand how **Stack** and **RAM** works
- How to **fill** it and **link** it with the **BIOS** 

## What is a GDT?
### General
> A Global Descriptor Table is a data structure used by Intel x86-family processors [...] in order to define the characteristics of the various memory areas used during a program execution [...].

A GDT is a table of 8-byte entries. Each referring to:
- a Null descriptor,
- Task State Segment (TSS) descriptor,
- Local Descriptor Table (LDT) ,
- or call gate

**Segment descriptor:**<br>
part of the segmentation unit, used to translate logical address to linear address. It contains fields that describe its size, the protection and control bits.

**Task State Segment (TSS):**<br>
Holds information about a task. Used by kernel for task management.

**Local Descriptor Table (LDT):**<br>
Memory table used in protected mode, contains memory segment descriptors.

**Call Gate:**<br>
Call gates are not used on most modern OS

### GDT Composition:
> Note that all the following tables are reversed comparing to the real bit order for readability reasons.

The GDT is loaded using the *lgdt* assembly instruction.<br>
Actually, the *lgdt* instruction loads the LGDT's size and address in the **Global Descriptor Table Register** (GDTR).  
The GDTR is a **6 bytes** register that holds **2 fields**:
- 2 bytes **limit** (size)
- 4 bytes **base** (linear/physical address location)  
(see more in [sources](#sources))

The GDT is structured as follows:

| 64 bytes index | 0 - 15 | 16 - 31 | 32 - 39 | 40 - 47 | 48 - 51 | 52 - 55 | 56 - 63 |
|-------|--------|---------|---------|---------|---------|---------|---------|
| **fields** | Limit 0:15 | Base 0:15 | Base 16:23 | Access Byte | Limit 16:19 | Flag | Base 24:31 |

The *fields* line can be confusing. Let's describe the value 'Limit 16:19' to understand.  
- *Limit* is the name of the value we are describing.
- *16:19* is the bitwise range of the value. Meaning the bits written here correspond to the 16th to the 19th bit of the *Limit* value.


To summarize the full *Limit* value is defined on 20 bits. The 20 bits of *Limit* are defined **from the first bit to the 16th bit** of the GDT **AND** **from the 49th bit to the 52nd bit** of the GDT. Same for the *Base* value but with 32 bits.  
"**Why is it split?**", you might ask. Well, it is out of the scope of this project but for your culture the reason is both historical and efficiency related.

#### Flags:
| **Bit index** | 4 | 5 | 6 | 7 |
|----------|---|---|---|---|
| **Symbol**| 0 | 0 | Sz | Gr |

**Sz** (size):  
*if 0 -> 16 bit protected mode; else if 1 -> 32 bit protected mode*  
Defines the default operand size. For example in a code segment defines the usage of ax (16 bits) or eax (32 bits).

**Gr** (granularity):  
*if 0 -> limit is 1 Byte block ; else if 1 -> limit is 4 KB block.*  
The *Limit* value defined on 20 bits has a maximum of 0x000FFFFF.  
if Gr == 0 -> Maximum segment size is 0x000FFFFF + 1 == 1 MB;
else -> Maximum segment size is (0x000FFFFF + 1) * 4KB == 4 GB

#### Access Byte:
| **Bit index** : | 0 | 1 | 2 | 3 | 4 | 5 - 6 | 7 |
|-----------|---|---|---|---|---|---|---|
| **Symbol** : | Ac | RW | DC | Ex | S | Privl | Pr |

**Ac** (Accessed):  
*Needs to be cleared to 0; the CPU sets it to 1 when accessing.*

**RW** (Read Write):  
*if 0 -> Read access; else -> Write access.*  
When **Read** access is allowed, **Write** access is **never** allowed.  
When **Write** access is allowed, **Read** access is **always** allowed.

**DC** (Direction Bit / Conforming Bit):   
*if 0 -> segment grows up; else -> segment grows down*

**Ex** (Executable):  
*if 0 -> data; else -> exeuctable code*  
Tells the CPU if the bytes in the segments are unstruction to execute.

**S** (Descriptor type):  
*if 0 -> system segment; else -> data or code segment*

**Privl** (Privilege):  
*Defines the protection ring from 0 = highest (kernel) and 3 = lowest (user)*
| **Level** | **Use** |
|-----------|---------|
|0| Kernel mode |
|1| OS services |
|2| OS drivers |
|3| User mode |

**Pr** (Present):  
*if 0 -> The segment is not present in memory, access it will result in segfault; else -> segment present in memory and usable by CPU*

## In our case

The kernel we built in the previous part uses GRUB as bootloader. GRUB already installs a GDT. Overwrite it will result in a 'triple fault' reseting the machine. We will need to set our own GDT in a known memory address we can access.  
The subject tells us to set the GDT at address **0x00000800**.

We will need:
- a special pointer to use with the *lgdt* instruction.

- a 7 entries GDT:
1. A Null descriptor
1. A kernel code segment
1. A kernel data segment
1. A kernel stack
1. A user code segment
1. A user data segment
1. A user stack

Our *GDT pointer* will be represented by this structure:
```C
struct	gdt_ptr
{
	uint16_t	limit;
	uint32_t	base;
}	__attribute__((packed));		// Tells the compiler not to add padding between fields

```
	
	
Our *GDT entry* will be represented by this structure:  
```C
struct gdt_entry
{
	uint16_t	limit_low;
	uint16_t	base_low;
	uint8_t		base_middle;
	uint8_t		access;
	uint8_t		limit_high_flags;
	uint8_t		base_high;
}	__attribute__((packed));	// Tells the compiler not to add padding between fields
```	
> Note: ```limit_high``` (4 bits value) and ```flags``` (4 bits value) are joined in on unique 8 bits value: ```limit_high_flags```

### Update the GDTR:
- We move the value 0x38 (sizeof(gdt_entry) * nb entry) and 0x800 (base of the GDT) to the variable gp.  
- Then, load the GDT with the instruction lgdt.
```assembly
_setGdt:
	movw $0x38, gp
	movl $0x800, gp + 2
	lgdt gp
```
 
- Call our function gdt_install() that will load and create the GDT table in memory using global variable ```gp``` and ```gdt```
- Jump (far jump) to the kernel code segment (ljmp jumps to another segment):
	- ```0x8``` is the offset of the segment we are jumping to,
	- ```$_reload_cs``` will be the offset inside the segment we are jumping to.
- Updates all the register needed by the CPU (again see [sources](#sources) for more information about registers ):
	- 0x10 describes the offset of the kernel data entry in our GDT.
	- 0x18 describes the offset of the kernel stack entry in our GDT.
	
```assembly
	call gdt_install

	_reload_segments:
		ljmp $0x08, $_reload_cs
	_reload_cs:
		mov $0x10, %ax
		mov %ax, %ds
		mov %ax, %es
		mov %ax, %fs
		mov %ax, %gs
		mov $0x18, %ax
		mov %ax, %ss
```

### Create the GDT and its entries:
In the file **gdt.c** we can find the functions ```void	gdt_install( void );``` and  ```void	gdt_set_entry( int entry_index, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran );```.  
The first one will call the second seven time, each time for an entry in the gdt, with the right flags to set.

**And this is it for the mandatory part of our project** ... Yes really!  
We are very consciensious person so ... **Let's do some bonuses !!**

## Bonus
> Let's create a shell like interface to build some usefull tools for our futur kernel.  

In facts nothing really special to describe here. There are 5 commands: *halt*, *reboot*, *stack*, *clear* and *gdt*.  

*halt*: Just pause the CPU. We will need to "hard" reboot the machine after that.  
*reboot*: It just reboots the machine.  
*stack*: print the stack content.  
*clear*: clears the terminal.  
*gdt*: prints informations about the gdt.

We have been carefull to make the terminal behave with the multiple screens.


## Sources
[Global GDT usage](http://www.osdever.net/bkerndev/Docs/gdt.htm)  
[Wikipedia - GDT](https://en.wikipedia.org/wiki/Global_Descriptor_Table)<br>
[Geeksforgeeks - GDT](https://www.geeksforgeeks.org/what-is-global-descriptor-table/)<br>
[LGDT instruction](https://www.felixcloutier.com/x86/lgdt:lidt)  
[SGDT instruction](https://www.felixcloutier.com/x86/sgdt)  
[Modern usage of a GDT](https://stackoverflow.com/questions/64741681/what-is-the-modern-usage-of-the-global-descriptor-tablegtd)  
[CPU Registers x86_64](https://wiki.osdev.org/CPU_Registers_x86-64)  
[Real VS Protected mode](https://en.wikipedia.org/wiki/X86_memory_segmentation)  
[Far jump instruction](https://www.felixcloutier.com/x86/jmp)  
