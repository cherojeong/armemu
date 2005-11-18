#include "display.h"
#include "text.h"
#include "console.h"
#include "memmap.h"

void arm_enable_ints(void);
void armfunc(int a);

// gcc seems to emit a call to this
void __gccmain(void)
{
}

void cpufunc(void)
{
//	thumbfunc(17);
	armfunc(17);
}

int main(void)
{
	int i;
	char c;
	
	cpufunc();

	for(i = 0xff; i >= 0; i -= 0x10) 
		clear_display(i | (i<<8) | (i<<16) | (i<<24));
	clear_display(0);
	initialize_text();
	set_text_color(0xffffffff, 0);

	puts("console initialized\n");

	puts("emulator features:\n");
	if (*REG(SYSINFO_FEATURES) & SYSINFO_FEATURE_DISPLAY)
		puts("\tdisplay\n");
	if (*REG(SYSINFO_FEATURES) & SYSINFO_FEATURE_CONSOLE)
		puts("\tconsole\n");

	read_cpu_id();

	mmu_init();
	
	puts("enabling interrupts\n");
	arm_enable_ints();

	puts("keyboard test:\n");
	c = 'a';
	for(;;) {
		unsigned int key;

		/* do some cpu intensive stuff for a bit */
		cpufunc();

#if 0
		/* send a repeating char pattern to stdout */
		c++;
		if(c > 'z')
			c = 'a';
		*(unsigned int *)DEBUG_STDOUT = c;
#endif

		/* see if a keyboard interrupt went off */
		if(read_keyboard(&key) >= 0)
			if((key & KEY_MOD_UP) == 0)
				putchar(key);

//		puts("abc ");
//		draw_char('a', 0, 0);
//		draw_char('b', 6, 0);
//		draw_char('c', 12, 0);
//		test_display();
	}
}

void debug_dump_memory_words(void *mem, int len)
{
	*REG(DEBUG_MEMDUMPADDR) = (unsigned int)mem;
	*REG(DEBUG_MEMDUMPLEN) = len;
	*REG(DEBUG_MEMDUMP_WORD) = 1;
}

void irq_handler(void)
{
	int vector;

#if 0
	*REG(SYSINFO_TIME_LATCH) = 0; // latch the system time
	unsigned int  time[2];
	time[0] = *REG(SYSINFO_TIME_SECS);
	time[1] = *REG(SYSINFO_TIME_USECS);
	debug_dump_memory_words(time, 2);
#endif

	if(*REG(PIC_CURRENT_BIT) == 0)
		return;

	vector = *REG(PIC_CURRENT_NUM);
	if(vector == INT_KEYBOARD) {
		keyboard_int_handler();
	}
	
	/* edge trigger ack */
	*REG(PIC_CURRENT_BIT) = *REG(PIC_CURRENT_BIT);
}

void fiq_handler(void)
{
	puts("fiq\n");
}

void data_abort_handler(void)
{
	puts("data abort\n");
	
	puts("spinning forever...\n");
	for(;;);
}

void prefetch_abort_handler(void)
{
	puts("prefetch abort\n");

	puts("spinning forever...\n");
	for(;;);
}

void undefined_handler(void)
{
	puts("undefined\n");
	puts("spinning forever...\n");
	for(;;);
}

void swi_handler(void)
{
	puts("swi handler\n");
}

