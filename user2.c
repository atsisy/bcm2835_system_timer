#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include <stdint.h>

uint64_t read_core_timer(int fd)
{
	void *address;
	uint64_t count;

    	address = (void *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0x40000000);
    	if (address == MAP_FAILED) {
    		perror("mmap");
        	close(fd);
        	return -1;
    	}

	count = *((uint64_t *)(address + 0x1c));

	munmap(address, 4096);

	return count;
}

int main(void)
{
	int fd;
	uint64_t count;

	if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
        	perror("open");
        	return -1;
    	}

	count = read_core_timer(fd);

	close(fd);

	printf("%llu\n", count);


	return 0;
}
