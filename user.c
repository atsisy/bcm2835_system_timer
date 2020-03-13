#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>

struct bcm2835_timer_regs {
    uint CS;  /** System Timer Control/Status */
    uint CLO; /** System Timer Counter Lower 32 bits */
    uint CHI; /** System Timer Counter Higher 32 bits */
    uint C0;  /** System Timer Compare 0.  DO NOT USE; is used by GPU.  */
    uint C1;  /** System Timer Compare 1 */
    uint C2;  /** System Timer Compare 2.  DO NOT USE; is used by GPU.  */
    uint C3;  /** System Timer Compare 3 */
};

int main(void)
{
	struct bcm2835_timer_regs *address;
	int fd;

	if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        	perror("open");
        	return -1;
    	}

    	address = (struct bcm2835_timer_regs *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x3F003000);
    	if (address == MAP_FAILED) {
    		perror("mmap");
        	close(fd);
        	return -1;
    	}

	struct timespec ts = {
		.tv_sec = 1,
		.tv_nsec = 0,
	};

	unsigned long long before = (unsigned long long)((((unsigned long long)address->CHI) << 32) | address->CLO);
	printf("%llu\n", before);
/*
	unsigned long long before, after;
	while(1){
		before = (unsigned long long)((((unsigned long long)address->CHI) << 32) | address->CLO);
		nanosleep(&ts, NULL);
		after = (unsigned long long)((((unsigned long long)address->CHI) << 32) | address->CLO);
		printf("%llu\n", after - before);
	}
*/
	return 0;
}
