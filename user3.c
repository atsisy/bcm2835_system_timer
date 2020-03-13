#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

struct ap804_timer_regs {

    /** The timer load register sets the time for the timer to count down.
        This value is loaded into the timer value register after the load
        register has been written or if the timer-value register has counted
        down to 0. */
    volatile uint32_t Load;

    /** This register holds the current timer value and is counted down when
        the counter is running. It is counted down each timer clock until the
        value 0 is reached. Then the value register is re-loaded from the
        timer load register and the interrupt pending bit is set. The timer
        count down speed is set by the timer pre-divide register. */
    volatile uint32_t Value;

    /** The standard SP804 timer control register consist of 8 bits but in the
        BCM implementation there are more control bits for the extra features.
        Control bits 0-7 are identical to the SP804 bits, albeit some
        functionality of the SP804 is not implemented. All new control bits
        start from bit 8 upwards. */
    volatile uint32_t Control;

    /** The timer IRQ clear register is write only. When writing this register
        the interrupt-pending bit is cleared. When reading this register it
        returns 0x544D5241 which is the ASCII reversed value for "ARMT". */
    volatile uint32_t IRQClear;

    /** The raw IRQ register is a read-only register. It shows the status of
        the interrupt pending bit. 0 : The interrupt pending bits is clear.
        1 : The interrupt pending bit is set.
        The interrupt pending bits is set each time the value register is
        counted down to zero. The interrupt pending bit can not by itself
        generates interrupts. Interrupts can only be generated if the
        interrupt enable bit is set. */
    volatile uint32_t RAWIRQ;

    /** The masked IRQ register is a read-only register. It shows the status
        of the interrupt signal. It is simply a logical AND of the interrupt
        pending bit and the interrupt enable bit. 0 : Interrupt line not
        asserted. 1 :Interrupt line is asserted, (the interrupt pending and
        the interrupt enable bit are set.)  */
    volatile uint32_t MaskedIRQ;

    /** This register is a copy of the timer load register. The difference is
        that a write to this register does not trigger an immediate reload of
        the timer value register. Instead the timer load register value is
        only accessed if the value register has finished counting down to
        zero. */
    volatile uint32_t Reload;

    /** The Pre-divider register is not present in the SP804. The pre-divider
        register is 10 bits wide and can be written or read from. This
        register has been added as the SP804 expects a 1MHz clock which we do
        not have. Instead the pre-divider takes the APB clock and divides it
        down according to:
        timer_clock = apb_clock/(pre_divider+1)
        The reset value of this register is 0x7D so gives a divide by 126. */
    volatile uint32_t PreDivider;

    /** The free running counter is not present in the SP804. The free running
        counter is a 32 bits wide read only register. The register is enabled
        by setting bit 9 of the Timer control register. The free running
        counter is incremented immediately after it is enabled. The timer can
        not be reset but when enabled, will always increment and roll-over.
        The free running counter is also running from the APB clock and has
        its own clock pre-divider controlled by bits 16-23 of the timer
        control register.
        This register will be halted too if bit 8 of the control register is
        set and the ARM is in Debug Halt mode. */
    volatile uint32_t FreeRunningCounter;

};

int main(void)
{
	struct ap804_timer_regs *address;
	int fd;

	if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        	perror("open");
        	return -1;
    	}

    	address = (struct ap804_timer_regs *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F00B000);
    	if (address == MAP_FAILED) {
    		perror("mmap");
        	close(fd);
        	return -1;
    	}

	address = (struct ap804_timer_regs *)((uint32_t)address + 0x408);

	printf("%d\n", address->Load);
/*
	uint32_t before, after;
	while(1){
		before = address->free_running_counter;
		nanosleep(&ts, NULL);
		after = address->free_running_counter;
		printf("value=%llu\n", after - before);
		fflush(stdout);
	}
*/
	return 0;
}
